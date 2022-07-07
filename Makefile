all: IMU_zero

HDRS = I2Cdev.h  MPU6050.h
CMN_OBJS = I2Cdev.o MPU6050.o

EXE = IMU_zero

IMU_OBJS = IMU_zero.o 

FAST_SLOW = 1000

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

CXXFLAGS = -Wall -DON_PI -g -DNFAST_VALUE=$(FAST_SLOW) -DNSLOW_VALUE=$(FAST_SLOW)

 $(CMN_OBJS) $(CLD_OBJS) $(IMU_OBJS): $(HDRS)

IMU_zero: $(CMN_OBJS) $(IMU_OBJS)
	@ echo "Compiled IMU Callibration program"
	@ $(CXX) -o $@ $^ -l wiringPi -lm

debug:
	@ gdbgui --host $(LOCAL_IP)  --port $(LOCAL_PORT) --args $(EXE) 3

clean:
	@ echo "Ahh clean!"
	@ rm -f $(CMN_OBJS) $(IMU_OBJS) collect_data IMU_zero
