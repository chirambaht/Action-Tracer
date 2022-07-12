
import socket
from datetime import datetime
import struct
import numpy as np
import time
import pandas as pd

np.set_printoptions(suppress=True) #prevent numpy exponential 

col = ['Time', 'Count', 'Devices']
for f in range(3):
    for k in ["W", "I", "J", "K"]:
        col.append(k + str(f+1) )

ft =0

ss = time.time()
running = False

def check_time():
    global running
    global ss
    if running == False:
        ss = time.time()
        running = True
    return ss


given_packet_count = 0
csv_document_buffer = []
HOST = "192.168.1.100"  # The server's hostname or IP address
# HOST = "192.168.43.77"  # The server's hostname or IP address
PORT = 9022  # The port used by the server
connection_count = 1
lost_packets = 0
while (True):
    try:
        
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print("Waiting for TCP connection %d" % (connection_count))

        # s.bind((HOST, PORT))
        s.connect((HOST, PORT))

        now = datetime.now()
        start_time = time.time()
        end_time = time.time()
        current_time = now.strftime("%Y%m%d-%H%M%S")

        print("Connected to %s on port %s" % (str(HOST), str(PORT)))
        print("Writting data to %s.act" % (current_time))

        while(True):
            data = s.recv(255)
            start_time = check_time()

            if not data:
                end_time = time.time()
                running = False
                break
            
            # Skip to the next packet if not sufficient data is received
            if len(data) != 240:
                lost_packets += 1
                continue

            header = data[:12]
            rest_of_data = data[12:240]

            sens_data = np.frombuffer(rest_of_data, dtype=np.float32)
            h_data = np.frombuffer(header, dtype=np.float32)

            sens_data = np.round(sens_data, 4)
            
            print(f"\nTime: {int(h_data[0])}, Count: {int(h_data[1])}, Devices: {int(h_data[2])}")

            # print t data in groups of 19.
            for i in range(len(sens_data)//19):
                print(f"Device {i+1}:{sens_data[i*19:(i*19)+19]}")

            given_packet_count = int(h_data[1])
            cs.append(sens_data)

        print("Last log to %s.act" % (current_time))
        ft = (end_time - start_time) / 1
       
        connection_count += 1
    except KeyboardInterrupt:
        print("\nExiting...")
        end_time = time.time()
        print(
            f"Average packets received was {c/ft:.2f}/s. \n{c} packets were received in {ft:.3f}s.\nTotal packets Recevied: {len(cs)}/{c} {round((len(cs)/c)*100,2)}%.\n Lost packets: {lost_packets}")
        df = pd.DataFrame(cs, columns=col)
        df.to_csv(f"{current_time}.csv", index=False)
        break
    except Exception as e:
        print(e)
        break


# swap two numbers
