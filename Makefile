all: proto collector
test: proto test_compile
RATE = 2
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

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h action_pi.hpp packager.h calibrator.hpp tracer_point.h cxxopts.hpp action_tracer.h action_definitions.pb.h main.h 
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o packager.o tracer_point.o action_tracer.o action_definitions.pb.o main.o 

EXE = collector
ACTISH = actish

CXXFLAGS = -Wall -DON_PI -g -DMPU6050_DMP_FIFO_RATE_DIVISOR=$(RATE) -std=c++11 # -DDEBUG 

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

# protoc -I=. --cpp_out=. ./action_definitions.proto
@ $(OBJS): $(HDRS)

collector: $(OBJS)
	@ echo "Compiled data collection program"
	@ $(CXX) -o $(EXE) $^ -lwiringPi -lpthread -lprotobuf
	@ echo "Program made!"
	
clean:
	@ rm -f $(OBJS) $(EXE) *.o $(ACTISH) 
	@ echo "Ahh clean!"
	

gui_collector_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(EXE) --host $(LOCAL_IP)  --port $(LOCAL_PORT)

debug:
	@ gdb $(EXE)

gui_actish_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(ACTISH) --host $(LOCAL_IP)  --port $(LOCAL_PORT)

documentation:
	@ doxygen docs/Doxyfile

proto:
	@ rm -f action_definitions.pb.h action_definitions.pb.cc action_definitions_pb2.py
	@ protoc -I=. --cpp_out=. ./action_definitions.proto
	@ protoc -I=. --python_out=. ./action_definitions.proto
	@ protoc -I=. --csharp_out=. ./action_definitions.proto
	@ echo "Proto header files compiled"

test_compile:
	g++ -Wall -g -o test_program action_definitions.pb.cc packager.cpp test.cpp -lpthread -lprotobuf

test_run:
	@ echo "Running program now"
	@ ./test_program