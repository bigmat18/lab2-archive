#! /usr/bin/env python3

import sys, logging, os, argparse, struct, socket
import concurrent.futures, threading, subprocess, signal
import time

HOST = "127.0.0.1"  
PORT = 58449

# Setup configurazione per il file si logging
logging.basicConfig(filename=os.path.basename(sys.argv[0][:-3]) + '.log',
                    level=logging.DEBUG, datefmt='%d/%m/%y %H:%M:%S',
                    format='%(asctime)s - %(levelname)s - %(message)s')


def readN(socket: socket.socket, n: int):
  data: bytes = b''
  length = 0
  while len(data) < n:
    packet = socket.recv(n - length)
    if not packet:
      raise Exception('Socket closed')
    
    data += packet
    length += len(packet)
  return data

def writeN(pipe: int, data: bytes):
  length = len(data)
  while length > 0:
    written = os.write(pipe, data)
    data = data[written:]
    length -= written


# Gestore della connessione di tipo A
def handler_connection_A(conn: socket.socket, addr: str, pipe: int, 
                         mutex_pipe: threading.Lock(), mutex_log: threading.Lock()):
    with conn:
        # print(f"Contattato A da {addr}")
        
        # Recezione del valore lenght che essendo max 2048 uso 2 byte
        data = readN(conn, 2)
        assert len(data) == 2
        lenght = struct.unpack("!H", data)[0]
        assert lenght > 0
                
        # Recezione sequenza di byte che rappresenta la stringa
        data = readN(conn, lenght)
        assert len(data) == lenght
        
        # Scrittura byte sulla pipe
        mutex_pipe.acquire()
        writeN(pipe, struct.pack("H", lenght) + data)
        mutex_pipe.release()
        
        # Scrittura sul file di logging riguardo questa connessione
        mutex_log.acquire()
        logging.info(f"Connessione tipo A {lenght}")
        mutex_log.release()


# Gestore connessione di tipo B
def handler_connection_B(conn: socket.socket, addr: str, pipe: int, 
                         mutex_pipe: threading.Lock(), mutex_log: threading.Lock()):
    with conn:
        # print(f"Contattato B da {addr}")
        
        # Variabile per contare i byte presi
        num_byte = 0
        
        while True:
            # Recezione del valore lenght che essendo max 2048 uso 2 byte
            data = readN(conn, 2)
            assert len(data) == 2
            num_byte += 2
            lenght = struct.unpack("!H", data)[0]
            if lenght == 0: break
            assert lenght > 0
            
            # Recezione sequenza di byte che rappresenta la stringa
            data = readN(conn, lenght)
            assert len(data) == lenght
            num_byte += lenght
            
            # Scrittura byte sulla pipe
            mutex_pipe.acquire()
            writeN(pipe, struct.pack("H", lenght) + data)
            mutex_pipe.release()

        # Scrittura sul file di logging riguardo questa connessione
        mutex_log.acquire()
        logging.info(f"Connessione tipo B {num_byte}")
        mutex_log.release()


if __name__ == "__main__":
    
    # Setup del parsing per gli argomenti dalla linea di comando
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('number_thread', type=int)  
    parser.add_argument('-r', help='num reader', type=int, default=3)  
    parser.add_argument('-w', help='num writer', type=int, default=3)
    parser.add_argument('-v', help='call archivio with valgrid', action="store_true")

    args = parser.parse_args()
    assert args.number_thread > 0 or args.r > 0 or args.w > 0, "Il numero di thread deve essere maggiore di 0"

    # Creazione ed apertura delle pipe
    if not os.path.exists('caposc'):
        os.mkfifo('caposc')
        
    if not os.path.exists('capolet'):
        os.mkfifo('capolet')

    # Avvio programma archivio
    if args.v:
        archive = subprocess.Popen(["valgrind","--leak-check=full", 
                                    "--show-leak-kinds=all", 
                                    "--log-file=valgrind-%p.log", 
                                    "./archivio", str(args.r), str(args.w)])
    else: 
        archive = subprocess.Popen(['./archivio', str(args.r), str(args.w)])

    time.sleep(2)

    caposc = os.open('caposc', os.O_WRONLY)
    capolet = os.open('capolet', os.O_WRONLY)
        
    # print("[Pid archivio] " + str(archive.pid))
    
        
    # Avvio server
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        try:
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server.bind((HOST, PORT))
            server.listen()
            
            # Creazione mutex per la gestione della mutua esclusione per la pipe e la scrittura sul log
            mutex_pipe = threading.Lock()
            mutex_log = threading.Lock()
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.number_thread) as executor:
                
                # Ciclo che gestisce l'arrivo delle rischieste di connessione da parte dei client
                while True:
                    # print("In attesa di un client...")
                    conn, addr = server.accept()
                    data = conn.recv(1).decode()
                    assert data == 'a' or data == 'b';
                    
                    # Invio gestione del client all'handler richiesto dal tipo di connessione
                    if data == 'a':   executor.submit(handler_connection_A, conn, addr, capolet, mutex_pipe, mutex_log)
                    elif data == 'b': executor.submit(handler_connection_B, conn, addr, caposc, mutex_pipe, mutex_log)
                    else: break
        
        # Gestione interruzione con SIGINT
        except KeyboardInterrupt:
            pass
        
        # Chiusura server e invio comando di chiusura per archivio
        server.shutdown(socket.SHUT_RDWR)
        os.kill(archive.pid, signal.SIGTERM)
        
        # Chiusura ed eliminazione pipe 
        os.close(capolet)
        os.close(caposc)
        os.unlink("capolet")
        os.unlink("caposc")


