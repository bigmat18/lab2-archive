#! /usr/bin/env python3

import sys, logging, os, argparse, struct, socket
import concurrent.futures
import subprocess
import posix


HOST = "127.0.0.1"  
PORT = 58449

logging.basicConfig(filename=os.path.basename(sys.argv[0][:-3]) + '.log',
                    level=logging.DEBUG, datefmt='%d/%m/%y %H:%M:%S',
                    format='%(asctime)s - %(levelname)s - %(message)s')

def recv_all(conn,n):
  chunks = b''
  bytes_recd = 0
  while bytes_recd < n:
    chunk = conn.recv(min(n - bytes_recd, 1024))
    if len(chunk) == 0:
      raise RuntimeError("socket connection broken")
    chunks += chunk
    bytes_recd = bytes_recd + len(chunk)
  return chunks

def handler_connection(conn, addr):
    with conn:
        print(f"Contattato da {addr}")
        data = recv_all(conn, 4)
        assert len(data) == 4
        
        lenght = struct.unpack("!i", data)[0]
        assert lenght > 0
        
        data = recv_all(conn, lenght)
        assert len(data) == lenght
        
        print(struct.unpack(f"{lenght}c", data)[0])
        
        pipe = os.open("caposc", os.O_WRONLY)
        os.write(pipe, struct.pack("i", lenght) + data)
        os.close("caposc")
            

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('number_thread', type=int)  
    parser.add_argument('-r', help='num reader', type=int, default=3)  
    parser.add_argument('-w', help='num writer', type=int, default=3)
    parser.add_argument('-v', help='call archivio with valgrid', action="store_true")

    args = parser.parse_args()
    assert args.number_thread > 0 or args.r > 0 or args.w > 0, "Il numero di thread deve essere maggiore di 0"

    # if args.v:
    #     subprocess.Popen(["valgrind","--leak-check=full", 
    #                       "--show-leak-kinds=all", 
    #                       "--log-file=valgrind-%p.log", 
    #                       "archivio", str(args.r), str(args.w)])
    # else: subprocess.Popen(['./archivio', str(args.r), str(args.w)])

    if not os.path.exists("caposc"):
        os.mkfifo("caposc")
        
    if not os.path.exists("capolet"):
        os.mkfifo("capolet")
        
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        try:
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)      
            server.bind((HOST, PORT))
            server.listen()
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.number_thread) as executor:
                while True:
                    print("In attesa di un client...")
                    conn, addr = server.accept()
                    executor.submit(handler_connection, conn, addr)
        except KeyboardInterrupt: 
            pass
        server.shutdown(socket.SHUT_RDWR)


