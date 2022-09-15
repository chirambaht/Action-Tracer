# connect to web server and send data
import os
import socket
import time
from action_definitions_pb2 import ActionDataNetworkPackage
from datetime import datetime

#create websocket client
s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

# check if directory 'data' exists, if not create it
if ( not os.path.isdir( "test" ) ):
    os.mkdir( "test" )

# get current time as string
fname_bin = "test/raw_" + datetime.now().strftime( "%m%d%Y-%H%M%S" ) + ".bin"

open( fname_bin, "wb" ).close()

s.connect( ( '192.168.1.75', 9022 ) )
print( "Connected" )


def log_to_file( data_file_name, mode, data ):
    data_file = open( data_file_name, mode )
    data_file.write( data )
    data_file.close()


def receive_data():
    bad_packets = 0
    total_packets = 0
    good_packets = 0
    start_time = 0

    # receive data
    while ( True ):
        # global fname_bin

        data_packet = ActionDataNetworkPackage()
        recevied_data = s.recv( 65536 )

        if start_time == 0:
            start_time = time.time()
        if not recevied_data:
            continue

        log_to_file( fname_bin, "ab", recevied_data )

        try:

            data_packet.ParseFromString( recevied_data )
            print( data_packet.packet_number )
            # print( data_packet.__str__() )
        except:
            # print( "Error while parsing data" )
            bad_packets += 1
            continue

        if ( ( data_packet.packet_number == 0 ) ):
            end_time = time.time()
            print( "I have been told to disconnect, ouch!" )
            print( "Bad packets: ", bad_packets )
            print( "Total packets: ", total_packets )
            print( "Percentage: ", round( ( bad_packets / total_packets ) * 100, 2 ) )
            print( "Throughput (/s): ", round( total_packets / ( end_time - start_time ), 2 ) )
            print( "Good packets: ", good_packets, "=", total_packets - bad_packets )
            print( "Goodput (/s): ", round( good_packets / ( end_time - start_time ), 2 ) )
            return
        total_packets += 1
        if ( len( data_packet.device_data ) < 1 ):
            print( "No data in packet" )
            print( data_packet.device_data )
            bad_packets += 1
            continue

        for device_data in data_packet.device_data:
            if ( len( device_data.data ) < 30 ):
                continue
        good_packets += 1


receive_data()
