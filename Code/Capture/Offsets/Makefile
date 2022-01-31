all: IMU_zero

HDRS = I2Cdev.h  MPU6050.h
CMN_OBJS = I2Cdev.o MPU6050.o

EXE = IMU_zero

IMU_OBJS = IMU_zero.o 

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000
# Set DMP FIFO rate to 100Hz.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.

 $(CMN_OBJS) $(CLD_OBJS) $(IMU_OBJS): $(HDRS)

IMU_zero: $(CMN_OBJS) $(IMU_OBJS)
	@ echo "Compiled IMU Callibration program"
	@ $(CXX) -o $@ $^ -l wiringPi -lm

get_debug: $(CMN_OBJS) $(IMU_OBJS)
	@ echo "Compiled IMU Callibration program"
	@ $(CXX) -o $@ $^ -ggdb -O0 -l wiringPi -lm

debug:
	@ gdbgui $(EXE) --host $(LOCAL_IP)  --port $(LOCAL_PORT)


clean:
	@ echo "Ahh clean!"
	@ rm -f $(CMN_OBJS) $(IMU_OBJS) collect_data IMU_zero
