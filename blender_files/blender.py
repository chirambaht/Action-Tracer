import bpy
import math
import os
import socket
from datetime import datetime
#create websocket client
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

from typing import List

ACT = bpy.data.texts['action_definitions_pb2.py'].as_module()


# docs: https://docs.blender.org/api/current/bpy.ops.mesh.html#bpy.ops.mesh.primitive_cube_add
def add_cube(size: float = 1.0,
             location: List[float] = [0.0, 0.0, 0.0],
             rotation: List[float] = [0.0, 0.0, 0.0],
             scale: List[float] = [2.0, 2.0, 2.0]) -> None:
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


def receive_data(objects):
    global ACT
    frame_count = 1
    #    blender_object.rotation_mode = 'XYZ'
    rots = []
    rolls = ['wrist', 'elbow', 'arm']
    try:
        for i in rolls:
            temp = objects[i]
            temp.rotation_mode = 'QUATERNION'
            rots.append(temp)

    except Exception as e:
        print(e)
        return

    # send data
    while (True):

        data_packet = ACT.ActionDataNetworkPackage()
        recevied_data = s.recv(4096)

        try:
            data_packet.ParseFromString(recevied_data)
        except:
            print("Error while parsing data")
            continue
        if ((data_packet.packet_number == 0)):
            print("I have been told to disconnect, ouch!")
            return
        if (len(data_packet.device_data) < 1):
            continue

        for i, dev in enumerate(rots):
            pack = data_packet.device_data[i].data[:4]
            dev.rotation_quaternion = pack

        bpy.ops.wm.redraw_timer(type='DRAW_WIN_SWAP', iterations=1)

        #        blender_object.keyframe_insert(data_path="rotation_quaternion", frame=frame_count)

        #        blender_object.rotation_euler = pack[13:16]
        #        blender_object.keyframe_insert(data_path="rotation_euler", frame=frame_count)


def main():
    #add a cube
    #    add_cube()

    #select the cube
    cube = bpy.context.scene.objects

    try:
        s.connect(('192.168.1.60', 9022))
        print("We're in!")
    except Exception as e:
        print(e)

    receive_data(cube)


main()