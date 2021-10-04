all: collect_data

HDRS = helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h packager.h tracer_point.h main.h
CMN_OBJS = I2Cdev.o MPU6050.o 
CLD_OBJS = packager.o tracer_point.o main.o
 
# Set DMP FIFO rate to 100Hz.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.

CXXFLAGS = -DDMP_FIFO_RATE=1 -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`

 $(CMN_OBJS) $(CLD_OBJS): $(HDRS)

collect_data: $(CMN_OBJS) $(CLD_OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $@ $^ -l wiringPi -lm

clean:
	@ echo "Ahh clean!"
	@ rm -f $(CMN_OBJS) $(CLD_OBJS) collect_data