import pandas as pd
import action_definitions_pb2 as act
import socket

session_recording_arr = []
DEVICES = 0
HOST = "192.168.101.106"
PORT = 9022

file_name = "rec_custom"

def makeCols(n):
    cols = ["time", "packet"]
    for i in range(1, 2+n):
        print(i)
        cols.append("id_" + str(i))
        cols.append("quat_w_" + str(i))
        cols.append("quat_x_" + str(i))
        cols.append("quat_y_" + str(i))
        cols.append("quat_z_" + str(i))
        cols.append("acc_x_" + str(i))
        cols.append("acc_y_" + str(i))
        cols.append("acc_z_" + str(i))
        cols.append("gyro_x_" + str(i))
        cols.append("gyro_y_" + str(i))
        cols.append("gyro_z_" + str(i))
        cols.append("temp_" + str(i))
    
    return cols

def parseACTMessage(actdnp):
    global session_recording_arr
    global DEVICES
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

    if len(device_data) > DEVICES:
        DEVICES = len(device_data)
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

        

def main():
    global DEVICES

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

    # save file
    temp_df = pd.DataFrame(session_recording_arr)

    temp_df.to_csv(file_name + "~temp.csv", index=False, header=False)

    cols = makeCols(DEVICES)

    df = pd.DataFrame(session_recording_arr, columns=cols)
    df.to_csv(file_name + ".csv", index=False)

    print("Good messages: %d" % good_messages)
    print("Bad messages: %d" % bad_messages)
    s.close()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        if len(session_recording_arr) > 0:
            temp_df = pd.DataFrame(session_recording_arr)

            temp_df.to_csv("~" + file_name + "temp.csv", index=False, header=False)
            cols = makeCols(DEVICES)
            df = pd.DataFrame(session_recording_arr, columns=cols)
            df.to_csv(file_name + ".csv", index=False)
        else:
            print(e)
