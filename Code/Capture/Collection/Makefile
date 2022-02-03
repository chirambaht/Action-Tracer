all: collect_data

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h packager.h tracer_point.h main.h
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o packager.o tracer_point.o main.o

EXE = collect_data

CXXFLAGS = -Wall -DON_PI -g

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

@ $(OBJS): $(HDRS)

collect_data: $(OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $(EXE) $^ -l wiringPi -lm
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