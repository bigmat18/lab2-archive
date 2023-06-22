#! /usr/bin/env python3

import sys, logging, os, argparse, struct, socket
import concurrent.futures
import subprocess
import posix


HOST = "127.0.0.1"  
PORT = 58449

# configurazione del logging
# il logger scrive su un file con nome uguale al nome del file eseguibile
logging.basicConfig(filename=os.path.basename(sys.argv[0][:-3]) + '.log',
                    level=logging.DEBUG, datefmt='%d/%m/%y %H:%M:%S',
                    format='%(asctime)s - %(levelname)s - %(message)s')

def main(conn, addr):
    with conn:
        pipe = os.open("caposc", os.O_WRONLY)
        while True:
            lenght = conn.recv(4)
            lenght = struct.unpack("!i", lenght)
            assert lenght > 0
            data = conn.recv(lenght)
            if not data: break
            os.write(pipe, data.encode())
        os.close("caposc")
            

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-m', help='max threads', type = int)  
    parser.add_argument('-r', help='num reader', type = int)  
    parser.add_argument('-w', help='num writer', type = int)

    args = parser.parse_args()
    assert args.m > 0 or args.r or args.w, "Il numero di thread deve essere maggiore di 0"
  
    subprocess.Popen(['./main', str(args.r), str(args.w)])

    if not os.path.exists("caposc"):
        os.mkfifo("caposc")
        
    if not os.path.exists("capolet"):
        os.mkfifo("capolet")
        
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.m) as executor:
            try:
                server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)      
                server.bind((HOST, PORT))
                server.listen()
                while True:
                    print("In attesa di un client...")
                    conn, addr = server.accept()
                    executor.submit(main, conn, addr)
            except KeyboardInterrupt: 
                pass
            server.shutdown(socket.SHUT_RDWR)


