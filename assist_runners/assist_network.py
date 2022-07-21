import socket
from datetime import datetime
import numpy as np
import time
import pandas as pd

import action_filters as act





np.set_printoptions( suppress=True )  # prevent numpy exponential
# Settings above are for numpy

DEFAULT_WINDOW_SIZE = 80

col = [ "Time", "Count" ]
for f in range( 3 ):
    for k in [
            "Quat W", "Quat X", "Quat Y", "Quat Z", "Accel X (g)", "Accel Y (g)", "Accel Z (g)", "Gyro X (dps)",
            "Gyro Y (dps)", "Gyro Z (dps)", "Yaw", "Pitch", "Roll", "X", "Y", "Z", "Grav X", "Grav Y", "Grav Z"
    ]:
        col.append( k + " " + str( f + 1 ) )

first_received_packet_number = 0
given_packet_count = 1
total_run_time = 0
csv_document_buffer = []
csv_document_buffer_2 = []
csv_document_buffer_3 = []

filter_buffer = np.zeros( ( DEFAULT_WINDOW_SIZE, len( col ) - 2 ) )

send_ready = False
current_data_packet = []
clients = []

ss = time.time()
running = False


def filter_data( data ):
    temp_data = data
    sma = []
    window_size = len( data )

    # if not enough data, add the last value to the end of sma
    while len( temp_data ) > len( sma ):
        sma.append( sma[ -1 ] )
    return sma


def check_time():
    global running
    global ss
    if running == False:
        ss = time.time()
        running = True
    return ss


def run_filter( latest_data, filter_choice="mean" ):
    global filter_buffer
    # global DEFAULT_WINDOW_SIZE

    # remove first elemet, add latest data
    filter_buffer = np.delete( filter_buffer, 0, 0 )
    filter_buffer = np.append( filter_buffer, [ latest_data ], 0 )

    # filter data
    if filter_choice == "mean":
        return np.mean( filter_buffer, axis=0 )
    elif filter_choice == "median":
        return np.median( filter_buffer, axis=0 )
    else:
        return latest_data


HOST = "192.168.1.100"  # The server's hostname or IP address
MY_HOST_IP = "192.168.1.102"  # The server's hostname or IP address
# HOST = "192.168.43.77"  # The server's hostname or IP address
PORT = 9022  # The port used by the server
connection_count = 1
lost_packets = 0
s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

while ( True ):
    try:
        s.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
        print( "Waiting for TCP connection %d" % ( connection_count ) )

        # s.bind((HOST, PORT))
        s.connect( ( HOST, PORT ) )

        now = datetime.now()
        start_time = time.time()
        end_time = time.time()
        current_time = now.strftime( "%Y%m%d-%H%M%S" )

        print( "Connected to %s on port %s" % ( str( HOST ), str( PORT ) ) )
        print( "Writting data to %s.act" % ( current_time ) )

        while ( True ):
            data = s.recv( 240 )

            if ( send_ready == True ):
                continue
            start_time = check_time()

            if not data:
                end_time = time.time()
                running = False
                break

            # Skip to the next packet if not sufficient data is received
            if len( data ) != 240:
                lost_packets += 1
                continue

            header = data[ : 12 ]
            rest_of_data = data[ 12 : 240 ]

            h_data = np.frombuffer( header, dtype=np.float32 ).round( 3 )
            if first_received_packet_number < 10:
                first_received_packet_number = h_data[ 1 ]

            if ( h_data[ 0 ] == 0 ):
                continue

            # Add filter to header to see if packet can be valid
            if ( len( csv_document_buffer ) != 0 ):
                if ( h_data[ 0 ] < csv_document_buffer[ -1 ][ 0 ] ):
                    lost_packets += 1
                    continue

                # Add filter to header to see if packet can be valid
                if ( h_data[ 1 ] < csv_document_buffer[ -1 ][ 1 ] + 1 ):
                    lost_packets += 1
                    continue

            sens_data = np.frombuffer( rest_of_data, dtype=np.float32 ).round( 5 )
            current_data_packet = np.concatenate( ( h_data, sens_data ) )
            # send_ready = True

            # Add filter to header to count number of zeros in sens_data
            if ( np.count_nonzero( sens_data ) == 0 ):
                lost_packets += 1
                continue

            given_packet_count = int( h_data[ 1 ] )

            new_one = run_filter( sens_data )  # reprocess the data
            new_one_2 = run_filter( sens_data, "median" )  # reprocess the data

            csv_document_buffer.append( h_data[ : 2 ].tolist() + np.round( sens_data, 4 ).tolist() )

            csv_document_buffer_2.append( h_data[ : 2 ].tolist() + np.round( new_one, 4 ).tolist() )
            csv_document_buffer_3.append( h_data[ : 2 ].tolist() + np.round( new_one_2, 4 ).tolist() )

        print( "Last log to %s.act" % ( current_time ) )

        connection_count += 1
    except KeyboardInterrupt:
        print( "\nExiting..." )
        end_time = time.time()
        total_run_time = ( end_time - start_time ) / 1.0
        print(
            f"Average packets received was {given_packet_count/total_run_time:.2f}/s. \n{given_packet_count} packets were received in {total_run_time:.3f}s.\nTotal packets Recevied: {len(csv_document_buffer)}/{given_packet_count} {round((len(csv_document_buffer)/given_packet_count)*100,2)}%.\n Lost packets: {lost_packets}"
        )
        df = pd.DataFrame( csv_document_buffer, columns=col )
        df.to_csv( f"{current_time}.csv", index=False )
        df = pd.DataFrame( csv_document_buffer_2, columns=col )
        df.to_csv( f"{current_time}_2.csv", index=False )
        df = pd.DataFrame( csv_document_buffer_3, columns=col )
        df.to_csv( f"{current_time}_3.csv", index=False )
        s.close()
        break
    except Exception as e:
        print( e )
        break
