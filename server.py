#! /usr/bin/env python3

import sys, logging, os, argparse, struct, socket, concurrent.futures, threading, subprocess, signal


HOST = "127.0.0.1"  
PORT = 58449

logging.basicConfig(filename=os.path.basename(sys.argv[0][:-3]) + '.log',
                    level=logging.DEBUG, datefmt='%d/%m/%y %H:%M:%S',
                    format='%(asctime)s - %(levelname)s - %(message)s')


def handler_connection_A(conn: socket.socket, addr: str, pipe: int, 
                         mutex_pipe: threading.Lock(), mutex_log: threading.Lock()):
    with conn:
        print(f"Contattato A da {addr}")
        data = conn.recv(2)
        assert len(data) == 2
        
        lenght = struct.unpack("!H", data)[0]
        assert lenght > 0
        
        print(lenght)
        
        data = conn.recv(lenght)
        assert len(data.decode()) == lenght
        print(data)
        
        mutex_pipe.acquire()
        os.write(pipe, struct.pack("H", lenght) + data)
        mutex_pipe.release()
        
        mutex_log.acquire()
        logging.info(f"Connessione tipo A {lenght}")
        mutex_log.release()


def handler_connection_B(conn: socket.socket, addr: str, pipe: int, 
                         mutex_pipe: threading.Lock(), mutex_log: threading.Lock()):
    with conn:
        print(f"Contattato B da {addr}")
        num_byte = 0
        
        while True:
            data = conn.recv(2)
            assert len(data) == 2
            num_byte += 2
            
            lenght = struct.unpack("!H", data)[0]
            if lenght == 0: break
            assert lenght > 0

            data = conn.recv(lenght)
            assert len(data.decode()) == lenght
            print(data)

            num_byte += lenght
            
            mutex_pipe.acquire()
            os.write(pipe, struct.pack("H", lenght) + data)
            mutex_pipe.release()

        mutex_log.acquire()
        logging.info(f"Connessione tipo B {num_byte}")
        mutex_log.release()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('number_thread', type=int)  
    parser.add_argument('-r', help='num reader', type=int, default=3)  
    parser.add_argument('-w', help='num writer', type=int, default=3)
    parser.add_argument('-v', help='call archivio with valgrid', action="store_true")

    args = parser.parse_args()
    assert args.number_thread > 0 or args.r > 0 or args.w > 0, "Il numero di thread deve essere maggiore di 0"


    # if args.v:
    #     archive = subprocess.Popen(["valgrind","--leak-check=full", 
    #                       "--show-leak-kinds=all", 
    #                       "--log-file=valgrind-%p.log", 
    #                       "archivio", str(args.r), str(args.w)])
    # else: 
    #     archive = subprocess.Popen(['./archivio', str(args.r), str(args.w)])

    if not os.path.exists("caposc"):
        os.mkfifo("caposc")
        
    if not os.path.exists("capolet"):
        os.mkfifo("capolet")
        
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        try:
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)      
            server.bind((HOST, PORT))
            server.listen()
            caposc = os.open("caposc", os.O_WRONLY)
            capolet = os.open("capolet", os.O_WRONLY)
            
            mutex_pipe = threading.Lock()
            mutex_log = threading.Lock()
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.number_thread) as executor:
                while True:
                    print("In attesa di un client...")
                    conn, addr = server.accept()
                    data = conn.recv(1).decode()
                    assert data == 'a' or data == 'b';
                    
                    if data == 'a':   executor.submit(handler_connection_A, conn, addr, caposc, mutex_pipe, mutex_log)
                    elif data == 'b': executor.submit(handler_connection_B, conn, addr, capolet, mutex_pipe, mutex_log)
                    else: break
        except KeyboardInterrupt: 
            server.shutdown(socket.SHUT_RDWR)
            # os.kill(archive.pid, signal.SIGTERM)
            
            os.close("caposc")
            os.close("capolet")
            
            os.unlink("capolet")
            os.unlink("caposc")


