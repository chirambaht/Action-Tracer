all: IMU_zero

HDRS = helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h
CMN_OBJS = I2Cdev.o MPU6050.o

IMU_OBJS = IMU_zero.o 
# Set DMP FIFO rate to 100Hz.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.

CXXFLAGS = -DDMP_FIFO_RATE=1 -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`

 $(CMN_OBJS) $(IMU_OBJS): $(HDRS)

IMU_zero: $(CMN_OBJS) $(IMU_OBJS)
	@ echo "Compiled IMU Callibration program"
	@ $(CXX) -o $@ $^ -lm

clean:
	@ echo "Ahh clean!"
	@ rm -f $(CMN_OBJS) $(IMU_OBJS) IMU_zero
