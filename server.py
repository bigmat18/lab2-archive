#! /usr/bin/env python3

import sys, logging, os, argparse
import concurrent.futures
import subprocess

HOST = "127.0.0.1"  
PORT = 58449

# configurazione del logging
# il logger scrive su un file con nome uguale al nome del file eseguibile
logging.basicConfig(filename=os.path.basename(sys.argv[0]) + '.log',
                    level=logging.DEBUG, datefmt='%d/%m/%y %H:%M:%S',
                    format='%(asctime)s - %(levelname)s - %(message)s')

def main():
    pass

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-m', help='max threads', type = int)  
  parser.add_argument('-r', help='num reader', type = int)  
  parser.add_argument('-w', help='num writer', type = int)

  args = parser.parse_args()
  assert args.m > 0 or args.r or args.w, "Il numero di thread deve essere maggiore di 0"
  
  subprocess.Popen(['./main', str(args.r), str(args.w)])
  try: 
    os.mkfifo("FIFO")
  except: 
    pass
  
  with open("FIFO") as file:
    for el in file:
      print(el)
  # with concurrent.futures.ThreadPoolExecutor(max_workers=args.m) as executor:
  #   executor.submit(main)
