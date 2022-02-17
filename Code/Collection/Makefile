all: collect_data

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h packager.h tracer_point.h main.h
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o packager.o tracer_point.o main.o

EXE = collect_data

CXXFLAGS = -Wall -DON_PI -g -DMPU6050_DMP_FIFO_RATE_DIVISOR=0x04 -D'SEND_ADDRESS="192.168.0.149"' -DARRAY_SOLUTION -std=c++11 

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

@ $(OBJS): $(HDRS)

collect_data: $(OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $(EXE) $^ -l wiringPi -lm -lpthread
	@ echo "Program made!"

clean:
	@ echo "Ahh clean!"
	@ rm -f $(OBJS) $(EXE)

gui_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(EXE) --host $(LOCAL_IP)  --port $(LOCAL_PORT)
	# @ guigdb $(EXE) --host 

debug:
	@ gdb $(EXE)


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