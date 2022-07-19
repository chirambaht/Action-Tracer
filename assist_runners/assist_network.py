
import socket
from datetime import datetime
import struct
import numpy as np
import time
import pandas as pd
import threading

d_line = 0

def debug_print():
    global d_line
    print(f"Got to debug_print {d_line}")
    d_line += 1

np.set_printoptions(suppress=True) #prevent numpy exponential 

DEFAULT_WINDOW_SIZE = 80
col = ["Time", "Count"]
for f in range(3):
    for k in ["Quat W", "Quat X", "Quat Y", "Quat Z", "Accel X (g)", "Accel Y (g)", "Accel Z (g)", "Gyro X (dps)", "Gyro Y (dps)", "Gyro Z (dps)", "Yaw", "Pitch", "Roll", "X", "Y", "Z", "Grav X", "Grav Y", "Grav Z"]:
        col.append(k + str(f+1))

first_received_packet_number = 0
given_packet_count = 1
total_run_time =0
csv_document_buffer = []
csv_document_buffer_2 = []


filter_buffer = []

for i in range(80):
    filter_buffer.append(np.zeros(len(col)-2))


send_ready = False
current_data_packet = []
clients = []

ss = time.time()
running = False

# implement real time moving average filter
def moving_average(data, window_size):
    if window_size == 0:
        return data
    weights = np.repeat(1.0, window_size)/window_size
    sma = np.convolve(data, weights, 'valid')
    return sma

# loop through data and apply moving average filter
def filter_data(data):
    temp_data = data
    sma = []
    window_size = len(data)

    if window_size < DEFAULT_WINDOW_SIZE:
        sma = moving_average(temp_data, window_size)
    else:
        sma = moving_average(data, DEFAULT_WINDOW_SIZE)
    
    # if not enough data, add the last value to the end of sma
    while len(temp_data) > len(sma):
        sma.append(sma[-1])
    return sma

def check_time():
    global running
    global ss
    if running == False:
        ss = time.time()
        running = True
    return ss

def run_filter(latest_data):
    global filter_buffer

    if len(filter_buffer) < DEFAULT_WINDOW_SIZE:
        filter_buffer.append(latest_data)
    else:
        filter_buffer.pop(0)
        filter_buffer.append(latest_data)
    
    # convert filter_buffer to columns
    filter_buffer = np.array(filter_buffer)
    df = pd.DataFrame(filter_buffer, columns=col[2:]).to_numpy()
    print(df)
    # apply moving average filter
    filtered_data = filter_data(df)

    return filtered_data


# Network server thread
def server_thread(host, client_port=5005):
    # create a tcp/ip socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # bind the socket to the port
    server_address = (host, client_port)
    print('Starting up server on %s port %s' % server_address)
    sock.bind(server_address)

    # listen for incoming connections
    sock.listen(5)
    global send_ready
    global clients
    global current_data_packet

    while True:
        # wait for a connection
        print('waiting for a connection')
        connection, client_address = sock.accept()
        print('connection from', client_address)
        clients.append(connection)
    

        while True:
            # watch send_ready flag
            while send_ready == False:
                # check every 100ms if send_ready is True
                time.sleep(0.1)
                continue

            print("I have data to send")

            # send current_data_packet to all connected clients
            for client in clients:
                try:
                    client.send(current_data_packet)
                except:
                    print("Error sending data")
                    clients.remove(client)
                    continue
            # reset send_ready flag
            send_ready = False

        # Clean up the connection
        connection.close()


HOST = "192.168.1.100"  # The server's hostname or IP address
MY_HOST_IP = "192.168.1.102"  # The server's hostname or IP address
# HOST = "192.168.43.77"  # The server's hostname or IP address
PORT = 9022  # The port used by the server
connection_count = 1
lost_packets = 0
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# create a thread to handle the server
server_thread = threading.Thread(target=server_thread, args=(MY_HOST_IP, PORT))

# start the server thread
# server_thread.start()

while (True):
    try:
        
        
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
            data = s.recv(240)
            debug_print()
            if (send_ready == True):
                continue
            start_time = check_time()

            if not data:
                end_time = time.time()
                running = False
                break
            debug_print()
            # Skip to the next packet if not sufficient data is received
            if len(data) != 240:
                lost_packets += 1
                continue

            header = data[:12]
            rest_of_data = data[12:240]

            
            h_data = np.frombuffer(header, dtype=np.float32).round(3)
            if first_received_packet_number < 10:
                first_received_packet_number = h_data[1]

            print("Packet number: %d" % (h_data[1]))
            if (h_data[0] == 0):
                continue

            # Add filter to header to see if packet can be valid
            if (len(csv_document_buffer) != 0):
                if (h_data[0] < csv_document_buffer[-1][0]):
                    lost_packets += 1
                    continue
                
                # Add filter to header to see if packet can be valid
                if (h_data[1] < csv_document_buffer[-1][1] + 1):
                    lost_packets += 1
                    continue

            sens_data = np.frombuffer(rest_of_data, dtype=np.float32).round(5)
            current_data_packet = np.concatenate((h_data, sens_data))
            send_ready = True
            # Add filter to header to count number of zeros in sens_data
            if (np.count_nonzero(sens_data) == 0):
                lost_packets += 1
                continue

            
            # print(f"Time: {round(h_data[0],3)}, Count: {int(h_data[1])}, Devices: {int(h_data[2])}")

            given_packet_count = int(h_data[1])

            new_one = run_filter(filter_data) # reprocess the data
            csv_document_buffer.append(h_data[:2].tolist() + np.round(sens_data,4).tolist())
            csv_document_buffer_2.append(h_data[:2].tolist() + np.round(new_one,4).tolist())

        print("Last log to %s.act" % (current_time))
    
        connection_count += 1
    except KeyboardInterrupt:
        print("\nExiting...")
        # stop the server thread
        # server_thread.do_run = False
        # server_thread.join()
        end_time = time.time()
        total_run_time = (end_time - start_time) / 1.0
        print(
            f"Average packets received was {given_packet_count/total_run_time:.2f}/s. \n{given_packet_count} packets were received in {total_run_time:.3f}s.\nTotal packets Recevied: {len(csv_document_buffer)}/{given_packet_count} {round((len(csv_document_buffer)/given_packet_count)*100,2)}%.\n Lost packets: {lost_packets}")
        df = pd.DataFrame(csv_document_buffer, columns=col)
        df.to_csv(f"{current_time}.csv", index=False)
        df = pd.DataFrame(csv_document_buffer_2, columns=col)
        df.to_csv(f"{current_time}_2.csv", index=False)
        # close the connection
        s.close()
        break
    except Exception as e:
        print(e)
        break
