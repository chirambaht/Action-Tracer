import bpy
import math
import os
import socket
from datetime import datetime
import time
#create websocket client
s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

from typing import List





ACT = bpy.data.texts[ 'action_definitions_pb2.py' ].as_module()
FIL = bpy.data.texts[ 'filter_manager.py' ].as_module()



def diff( a, b ):
    assert( len( a ) == len( b ) )

    out = []
    for i in range( len( a ) ):
        out.append( a[ i ] - b[ i ] )
    
    return out


# docs: https://docs.blender.org/api/current/bpy.ops.mesh.html#bpy.ops.mesh.primitive_cube_add
def add_cube( size: float = 1.0,
              location: List[ float ] = [ 0.0, 0.0, 0.0 ],
              rotation: List[ float ] = [ 0.0, 0.0, 0.0 ],
              scale: List[ float ] = [ 2.0, 2.0, 2.0 ] ) -> None:
    bpy.ops.mesh.primitive_cube_add(
        size=size,
        calc_uvs=True,
        enter_editmode=False,
        align='WORLD',
        location=location,
        rotation=rotation,
        scale=scale,
    )


def Update3DViewPorts():
    for area in bpy.context.window.screen.areas:
        if area.type == 'VIEW_3D':
            area.tag_redraw()


#    bpy.ops.wm.save_mainfile()


def receive_data( objects ):
    DATA_POINT_COUNT = 3 # Euler Angle
    # DATA_POINT_COUNT = 4 # Quaternion
    TARGET_FRAME_RATE = 24
    TIME_PER_FRAME = int(1000 / TARGET_FRAME_RATE )# ms
    FILTER_BUFFER_SIZE = 5

    global ACT


    frame_count = 1
    rotation_modes = ['XYZ', 'XYZ', 'XYZ']
    # rotation_modes = ['QUATERNION', 'QUATERNION', 'QUATERNION']
    
    rots = []
    rotsmir = []
    rolls = [ 'mixamorig:RightArm',  'mixamorig:RightForeArm' ,'mixamorig:RightHand']
    rollsmir = [ 'mixamorig:LeftArm',  'mixamorig:LeftForeArm' ,'mixamorig:LeftHand']
    try:
        # reset the rotation of the bones
        for i in range( 0, len( objects ) ):
            objects[ i ].rotation_mode = 'XYZ'
            objects[ i ].rotation_euler = [ 0, 0, 0 ]

        for d in range(len(rolls)):
            temp = objects[ rolls[d] ]
            tempm = objects[ rollsmir[d] ]

            temp.rotation_mode = rotation_modes[d]
            tempm.rotation_mode = rotation_modes[d][::-1]
            
            rots.append( temp )
            rotsmir.append( tempm )

    except Exception as e:
        print( e )
        return

    fillies = []
    filliesmir = []
    for i in range(3):
        fillies.append( FIL.Filter( "median", DATA_POINT_COUNT, FILTER_BUFFER_SIZE ) )
        filliesmir.append( FIL.Filter( "mean", DATA_POINT_COUNT, FILTER_BUFFER_SIZE ) )

    start_time = 0
    current_time = 0
    last_packet_send_time = 0


    # send data
    while ( True ):

        data_packet = ACT.ActionDataNetworkPackage()
        recevied_data = s.recv( 4096 )

        try:
            data_packet.ParseFromString( recevied_data )
            
        except:
            print( "Error while parsing data" )
            continue
        if ( ( data_packet.packet_number == 0 ) ):
            for i in range( 0, len( objects ) ):
                objects[ i ].rotation_mode = 'XYZ'
                objects[ i ].rotation_euler = [ 0, 0, 0 ]
            print( "I have been told to disconnect, ouch!" )
            return
        if ( len( data_packet.device_data ) < 1 ):
            continue
        
        if start_time == 0:
            start_time = int(round(time.time() * 1000))
       
        current_time = int(round(time.time() * 1000)) - start_time
        if (current_time - last_packet_send_time) < TIME_PER_FRAME:
            continue

        packets = []

        for i in range(len(rots)):
            t = data_packet.device_data[ i ].data[ 13: 16 ]
            fillies[i].filter( t )
            t = fillies[i].getBufferDataAsList()
            packets.append( t ) # Use euler 
        
        # modified
        
        packets[2] = diff(packets[2], packets[1])
        packets[1] = diff(packets[1], packets[0])
        packetsMod = packets
        # Corrections
        packets[0][0] = packets[0][0] * -1
        packets[1][0] = packets[1][0] * -1
        packets[2][0] = packets[2][0] * -1
        packets[2][1] = packets[2][1] * -1
        
        
        # Mirror Corrections
        packetsMod[0][2] = packetsMod[0][2] * -1
        
        for i in range(3):
            rots[i].rotation_euler = packets[i]
            rotsmir[i].rotation_euler = packetsMod[i]

            # Record the animation
#            rots[i].keyframe_insert(data_path="rotation_euler", frame=frame_count)
#            rotsmir[i].keyframe_insert(data_path="rotation_euler", frame=frame_count)
        frame_count += 1
        last_packet_send_time = current_time
        bpy.ops.wm.redraw_timer( type='DRAW_WIN_SWAP', iterations=1 )
        

        #        blender_object.keyframe_insert(data_path="rotation_quaternion", frame=frame_count)

        #        blender_object.rotation_euler = pack[13:16]
        


def main():
    #add a cube
    #    add_cube()

    #select the cube
    cube = bpy.context.object.pose.bones

    try:
        s.connect( ( '192.168.1.60', 9022 ) )
        print( "We're in!" )
    except Exception as e:
        print( e )

    receive_data( cube )


main()