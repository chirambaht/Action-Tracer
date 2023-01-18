import pandas as pd
import action_definitions_pb2 as act
import socket

session_recording_arr = []
DEVICES = 0
HOST = "192.168.101.106"
PORT = 9022
DEVICE_ID = []
ALL_PACKETS = []

file_name = "rec_custom"

def makeCols(n):
    cols = ["time(s)", "packet"]
    for i in range(1, 1+n):
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

def get_array_data(device):
    data = []
    data.append(device.device_identifier_contents)
    data.append(device.quaternion.w)
    data.append(device.quaternion.x)
    data.append(device.quaternion.y)
    data.append(device.quaternion.z)
    data.append(device.accelerometer.x)
    data.append(device.accelerometer.y)
    data.append(device.accelerometer.z)
    data.append(device.gyroscope.x)
    data.append(device.gyroscope.y)
    data.append(device.gyroscope.z)
    data.append(device.temperature)
    return data

def get_last_device_packet_with_id(id):
    global ALL_PACKETS
    for packet in reversed(ALL_PACKETS):
        for device in packet.device_data:
            if device.device_identifier_contents == id:
                return device

def parseACTMessage(actdnp):
    global session_recording_arr
    global DEVICES
    global DEVICE_ID
    global ALL_PACKETS

    ALL_PACKETS.append(actdnp)

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
    this_packet = []
    for device in device_data:
        this_packet.append(device.device_identifier_contents)

        if device.device_identifier_contents not in DEVICE_ID:
            DEVICE_ID.append(device.device_identifier_contents)
            DEVICE_ID.sort()
    
    for i in DEVICE_ID:
        if i not in this_packet:
            last_device_packet = get_last_device_packet_with_id(i)
            row_arr += get_array_data(last_device_packet)

        else:
            for device in device_data:
                if device.device_identifier_contents == i:
                    row_arr += get_array_data(device)
                    break
    
    session_recording_arr.append(row_arr)

def condition_df(df):
    pass

def end_methods():
    global session_recording_arr
    global DEVICES
    global file_name
    
    temp_df = pd.DataFrame(session_recording_arr)

    temp_df.to_csv("~" + file_name + "temp.csv", index=False, header=False)

    cols = makeCols(DEVICES)

    df = pd.DataFrame(session_recording_arr, columns=cols)

    # correct time column
    df['time(s)'] = (df['time(s)'] - df['time(s)'][0]).round(5)
    # round all columns to 5 decimal places
    df = df.round(5)
    
    # fix packet number
    df['packet'] = df['packet'].astype(int)

    df.to_csv(file_name + ".csv", index=False)
    

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
    s.close()
    print("Good messages: %d" % good_messages)
    print("Bad messages: %d" % bad_messages)
    end_methods()

    

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        if len(session_recording_arr) > 0:
            end_methods()
        else:
            print(e)
