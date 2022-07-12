
import socket
from datetime import datetime

import numpy as np
import time
# import pandas as pd


col = ['Time', 'Count', 'Devices']
for f in range(3):
    for k in ["W", "I", "J", "K"]:
        col.append(k + str(f+1) )


# Get the data from the packet as an array of integers
def bin_to_good(data_in, bytes_per_data_point, convert_to_float=False):
    pre_work = []
    data_packet = []
    
    for i in range(len(data_in) // bytes_per_data_point):
        pre_work.append(data_in[i*bytes_per_data_point:(i*bytes_per_data_point)+bytes_per_data_point])
    
    for i in pre_work:
        bin_string_array = i
        ss = "%2x%2x%2x%2x" % (bin_string_array[3],bin_string_array[2],bin_string_array[1], bin_string_array[0])
        # si = int(ss.replace(" ", "0"), 32)
        si = float(ss.replace(" ", "0"))

        if 1==0:
        # if convert_to_float:
            # data_packet.append(int.from_bytes(i, byteorder="little", signed=True) / 10000.0)
            data_packet.append(si / 10000.0)
        else:
            # data_packet.append(int.from_bytes(i, byteorder="little", signed=True) )
            data_packet.append(si)

    return data_packet

def bin_to_readable_data(data_in, bytes_per_data_point):
    pre_work = bin_to_good(data_in, bytes_per_data_point, False)
    buffer_string_thing = ""

    # print(pre_work)

    for i in pre_work:
        buffer_string_thing += str(i) + " "
    return buffer_string_thing


def millis():
    t = datetime.now().m
    return t.second + t.n


ss = time.time()
running = False


def check_time():
    global running
    global ss
    if running == False:
        ss = time.time()
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
HOST = "192.168.1.100"  # The server's hostname or IP address
# HOST = "192.168.43.77"  # The server's hostname or IP address
PORT = 9022  # The port used by the server
tt = 1
while (True):
    cs = []
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print("Waiting for TCP connection %d" % (tt))

    # s.bind((HOST, PORT))
    s.connect((HOST, PORT))

    now = datetime.now()
    start_time = time.time()
    end_time = time.time()
    current_time = now.strftime("%Y%m%d-%H%M%S")

    # logger = open(current_time+".act", "w")

    print("Connected to %s on port %s" % (str(HOST), str(PORT)))
    print("Writting data to %s.act" % (current_time))

    while(True):
        data = s.recv(255)
        start_time = check_time()

        if not data:
            end_time = time.time()
            running = False
            break
        
        print("Received data length: %d" % (len(data)))

        header = data[:12]

        rest_of_data = data[12:240]
        # correct the order that data comes in (saved in t)
        t = bin_to_good(rest_of_data, 4, True)
        h_data = bin_to_good(header, 4)
        
        print(f"\nTime: {h_data[0]}, Count: {h_data[1]}, Devices: {h_data[2]}")

        # print t data in groups of 19.
        for i in range(len(t)//19):
            print(f"Device {i+1}:{t[i*19:(i*19)+19]}")

        c = h_data[1]
    # logger.close()

    print("Last log to %s.act" % (current_time))
    ft = (end_time - start_time) / 1

    print(
        f"Average packets received was {c/ft:.2f}/s. \n{c} packets were received in {ft:.3f}s.\nTotal packets Recevied: {len(cs)}/{c} {round((len(cs)/c)*100,2)}%")

    # df = pd.DataFrame(cs, columns=col)
    # df.to_csv(f"{current_time}.csv", index=False)
    tt += 1


# swap two numbers
