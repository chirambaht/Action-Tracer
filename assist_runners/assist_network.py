
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

def bin_to_good(data_in, bytes_per_data_point):
    pre_work = []
    data_packet = []
    
    for i in range(len(data_in) // bytes_per_data_point):
        pre_work.append(data_in[i*bytes_per_data_point:(i*bytes_per_data_point)+bytes_per_data_point])
    
    for i in pre_work:
        data_packet.append(int.from_bytes(i, byteorder="little"))

    return data_packet

def bin_to_readable_data(data_in, bytes_per_data_point):
    pre_work = bin_to_good(data_in, bytes_per_data_point)
    buffer_string_thing = ""

    print(pre_work)

    for i in pre_work:
        buffer_string_thing += str(int.from_bytes(i, byteorder="little")) + " "
    return buffer_string_thing


def swapper(bin_string_array):
    r = len(bin_string_array)
    final_array = []
    #print half the array length
    print("Half:",r//2)

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
HOST = "192.168.1.100"  # The server's hostname or IP address
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
    start_time = time.clock_gettime(1)
    end_time = time.clock_gettime(1)
    current_time = now.strftime("%Y%m%d-%H%M%S")

    logger = open(current_time+".act", "w")

    print("Connected to %s on port %s" % (str(HOST), str(PORT)))
    print("Writting data to %s.act" % (current_time))

    while(True):
        data = s.recv(255)
        start_time = check_time()

        if not data:
            end_time = time.clock_gettime(1)
            running = False
            break
        
        print("Received data length: %d" % (len(data)))
        # print data as hex string

        header = data[:12]
        device_data_1 = data[12:88]
        device_data_2 = data[88:164]
        device_data_3 = data[164:240]

        print(data)
        print("\nHeader:",bin_to_readable_data(header, 4))
        print("Device 1:",bin_to_readable_data(device_data_1, 2))
        print("Device 2:",bin_to_readable_data(device_data_2, 2))
        print("Device 3:",bin_to_readable_data(device_data_3, 2))

        # correct the order that data comes in (saved in t)
        t = swapper(data)
       
        cs.append(clean_arr(t, 10000, 3))
        c = t[1]
    logger.close()

    print("Last log to %s.act" % (current_time))
    ft = (end_time - start_time) / 1

    print(
        f"Average packets received was {c/ft:.2f}/s. \n{c} packets were received in {ft:.3f}s.\nTotal packets Recevied: {len(cs)}/{c} {round((len(cs)/c)*100,2)}%")

    df = pd.DataFrame(cs, columns=col)
    df.to_csv(f"{current_time}.csv", index=False)
    tt += 1


# swap two numbers
