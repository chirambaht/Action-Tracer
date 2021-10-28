all: collect_data

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h packager.h tracer_point.h main.h
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o packager.o tracer_point.o main.o

CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`

@ $(OBJS): $(HDRS)

collect_data: $(OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $@ $^ -l wiringPi -lm
	@ echo "Program made!"

clean:
	@ echo "Ahh clean!"
	@ rm -f $(OBJS) collect_data