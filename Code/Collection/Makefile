all: collector

HOST_ADDRESS = "169.254.215.54"

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h packager.h tracer_point.h main.h
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o packager.o tracer_point.o main.o

EXE = collector
ACTISH = actish

CXXFLAGS = -Wall -DON_PI -g -DMPU6050_DMP_FIFO_RATE_DIVISOR=0x04 -D'SEND_ADDRESS=$(HOST_ADDRESS)' -DARRAY_SOLUTION -DDEBUG  -std=c++11

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

@ $(OBJS): $(HDRS)

collector: $(OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $(EXE) $^ -l wiringPi -lm -lpthread
	@ echo "Program made!"
	

clean:
	@ echo "Ahh clean!"
	@ rm -f $(OBJS) $(EXE) *.o $(ACTISH) test

gui_collector_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(EXE) --host $(LOCAL_IP)  --port $(LOCAL_PORT)

debug:
	@ gdb $(EXE)

Actish:
	@ $(CXX) -c action_pi.hpp I2Cdev.cpp IMU_zero.hpp MPU6050.cpp MPU6050_6Axis_MotionApps20.cpp action.cpp packager.cpp tracer_point.cpp -DON_PI -l wiringPi -lm -lpthread -g
	@ $(CXX) -o $(ACTISH) I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o action.o packager.o tracer_point.o -l wiringPi -lm -lpthread -g
	@ echo "ACTISH made!"

gui_actish_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(ACTISH) --host $(LOCAL_IP)  --port $(LOCAL_PORT)


test:
	@ $(CXX) -c I2Cdev.cpp hr_algorithm.cpp  max30102.cpp test.cpp
	@ $(CXX) -o test I2Cdev.o hr_algorithm.o max30102.o test.o -l wiringPi -lm -lpthread -g -DON_PI

# MPU6050_DMP_FIFO_RATE_DIVISOR values and their linked rate
# 	n  --> Rate (Hz)
#    0 -->  200 
#    1 -->  100 
#    2 -->   66 
#    3 -->   50 
#    4 -->   40 
#    5 -->   33 
#    6 -->   28 
#    7 -->   25 
#    8 -->   22 
#    9 -->   20 
#    a -->   18 
#    b -->   16 
#    c -->   15 
#    d -->   14 
#    e -->   13 
#    f -->   12 
#   10 -->   11 
#   11 -->   11 
#   12 -->   10 
#   13 -->   10 