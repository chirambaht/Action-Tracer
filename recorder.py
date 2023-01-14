import pandas as pd
import action_definitions_pb2 as act
import socket

session_recording_arr = []


def parseACTMessage(actdnp):
    global session_recording_arr
    row_arr = []
    timestamp = actdnp.send_time
    packet_number  = actdnp.packet_number
    device_data = actdnp.device_data

    # time
    timestamp = ((timestamp.seconds) +
         timestamp.nanos / 1e9).__round__(4)
    row_arr.append(timestamp)

    # packet number
    packet_number = int(packet_number)
    row_arr.append(packet_number)

    # device data
    for device in device_data:
        row_arr.append(device.device_identifier_contents) # id

        row_arr.append(device.quaternion.w) # w
        row_arr.append(device.quaternion.x) # x
        row_arr.append(device.quaternion.y) # y
        row_arr.append(device.quaternion.z) # z

        row_arr.append(device.accelerometer.x) # x
        row_arr.append(device.accelerometer.y) # y
        row_arr.append(device.accelerometer.z) # z

        row_arr.append(device.gyroscope.x) # x
        row_arr.append(device.gyroscope.y) # y
        row_arr.append(device.gyroscope.z) # z

        row_arr.append(device.temperature) # temperature

    session_recording_arr.append(row_arr)


HOST = "192.168.1.75"
PORT = 9022

file_name = "rec_custom"

###  Metrics  ###
bad_messages = 0
good_messages = 0

# Open socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


s.connect((HOST, PORT))

print("Connected to %s on port %s" % (str(HOST), str(PORT)))

while(True):
    data_length = s.recv(4)
    data_length = int.from_bytes(data_length, byteorder='little')
    message = act.ActionMessage()
    
    data = s.recv(data_length)
    try:
        message.ParseFromString(data)
    except:
        message.Clear()
        continue

    # Handle different message types
    if message.action == act.DISCONNECT:
        print("Disconnecting...")
        break
    elif message.action == act.DATA:
        parseACTMessage(message.data)
        good_messages += 1
    else:
        bad_messages += 1


print("Good messages: %d" % good_messages)
print("Bad messages: %d" % bad_messages)

s.close()
temp_df = 
# Save to file
cols = ["Time", "Packet", "ID_1", "Quat_W_1", "Quat_X_1", "Quat_Y_1", "Quat_Z_1", "Accel_X_1", "Accel_Y_1", "Accel_Z_1", "Gyro_X_1", "Gyro_Y_1", "Gyro_Z_1", "Temp_1", "ID_2", "Quat_W_2", "Quat_X_2", "Quat_Y_2", "Quat_Z_2", "Accel_X_2", "Accel_Y_2", "Accel_Z_2", "Gyro_X_2", "Gyro_Y_2", "Gyro_Z_2", "Temp_2", "ID_3", "Quat_W_3", "Quat_X_3", "Quat_Y_3", "Quat_Z_3", "Accel_X_3", "Accel_Y_3", "Accel_Z_3", "Gyro_X_3", "Gyro_Y_3", "Gyro_Z_3", "Temp_3"]
df = pd.DataFrame(session_recording_arr, columns=cols)
df.to_csv(file_name + ".csv", index=False, header=False)