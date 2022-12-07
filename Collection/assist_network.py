import socket
import time

HOST = "192.168.1.75"
PORT = 9022

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# reuse the socket
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# connect to server
s.connect((HOST, PORT))
r = 0
# byte file
start_time = 0
end_time = 0

t = []

print("Threaded Test")
print("We will recevie 2000 packets.")
while True:
    if (r%500 == 0 and r != 0):
        end_time = time.time()
        t.append(end_time - start_time)
        
        if r == 2000:
            print("Done")
            print("Time taken to receive 500 packets: ", t[0])
            print("Time taken to receive 1000 packets: ", t[1])
            print("Time taken to receive 1500 packets: ", t[2])
            print("Time taken to receive 2000 packets: ", t[3])
            break
    try:
        # receive data from server
        data = s.recv(1024)
        if r == 0:
            start_time = time.time()
        

        if len(data) == 30:
            r+=1
    except KeyboardInterrupt:

        break
        

    except:
        pass

# average time taken to receive 500 packets
av = 0
for i in t:
    av += i

av = av/4
print(f"Average time taken to receive 500 packets: {round(av,5)} ms")