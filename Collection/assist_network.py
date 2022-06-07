
import socket
from datetime import datetime
import numpy as np
import time
import pandas as pd


col = ['Time', 'Count', 'Devices']
for f in range(3):
    for k in ["W", "I", "J", "K"]:
        col.append(k + str(f+1) )

def smooth_bin_string(b_string):
    t = []
    for i in b_string:
        t.append(hex(i))
    return b_string


def swapper(bin_string_array):
    r = len(bin_string_array)
    final_array = []

    for i in range(r//2):
        ss = "0x%2x%2x" % (bin_string_array[(i*2)+1], bin_string_array[i*2])
        si = int(ss.replace(" ", "0"), 16)
        final_array.append(np.int16(si))

    return final_array


def millis():
    t = datetime.now().m
    return t.second + t.n


ss = time.clock_gettime(1)
running = False


def check_time():
    global running
    global ss
    if running == False:
        ss = time.clock_gettime(1)
        running = True
    return ss


def packet_print(data_packet):
    
    print(f"{data_packet[1]:5d} - (", end="")
    for i in range(len(data_packet) // 4):
        for j in range(4):
            try:
                print( f"{data_packet[j+(i*len(data_packet) // 4)] / 10000:^2.3f} ", end="")
            except :
                print("")
                break

        if i < len(data_packet) // 4 - 1:
            print(" <=-=> ", end="")


    print (f")\tTime - {data_packet[0]:5d}")

def clean_arr(arr, val, offset):
    print(arr)
    for i in range(len(arr) - offset):
        arr[i + offset] = arr[i + offset] / val
    print(arr)
    return arr

def packet_file_print( data_packet, document):
    print(f"{data_packet[1]} - (", end="", file=document)
    for i in range(len(data_packet) // 4):
        for j in range(4):
            try:
                print(f"{data_packet[j+(i*len(data_packet) // 4)] / 10000:^4.4f} ", end="", file=document)
            except:
                print("")
                break
        if i < len(data_packet) // 4 - 1:
            print(" < = > ", end="", file=document)
    print (f")\tTime - {data_packet[0]:5d}", file=document)

c= 0
HOST = "192.168.1.101"  # The server's hostname or IP address
PORT = 9022  # The port used by the server
tt = 1
while (True):
    cs = []
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print("Waiting for TCP connection %d" % (tt))

    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()

    now = datetime.now()
    start_time = time.clock_gettime(1)
    end_time = time.clock_gettime(1)
    current_time = now.strftime("%Y%m%d-%H%M%S")

    logger = open(current_time+".act", "w")

    print("Connected to %s on port %s" % (str(addr), str(PORT)))
    print("Writting data to %s.act" % (current_time))

    while(True):
        data = conn.recv(1024)
        start_time = check_time()
        if not data:
            end_time = time.clock_gettime(1)
            running = False
            break
        # correct the order that data comes in (saved in t)
        t = swapper(data)[:15]

        
        packet_print(t)
        packet_file_print(t, logger)
        c = t[1]
        
        cs.append(clean_arr(t, 10000, 3))
    logger.close()

    print("Last log to %s.act" % (current_time))
    ft = (end_time - start_time) / 1

    print(
        f"Average packets received was {c/ft:.2f}/s. \n{c} packets were received in {ft:.3f}s.\nTotal packets Recevied: {c} or {len(cs)}")

    df = pd.DataFrame(cs, columns=col)
    df.to_csv(f"{current_time}.csv", index=False)
    tt += 1


# swap two numbers
