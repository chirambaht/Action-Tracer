all: proto collector
basic: proto basic_test

HDRS = helper_3dmath.h I2Cdev.h MPU6050.h action_pi.hpp packager.h calibrator.hpp tracer_point.h action_tracer.h action_definitions.pb.h main.h 
OBJSB = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o action_definitions.pb.o packager.o tracer_point.o action_tracer.o basic_test.o
OBJS = I2Cdev.o MPU6050.o MPU6050_6Axis_MotionApps20.o action_definitions.pb.o packager.o tracer_point.o action_tracer.o main.o
PROTOGEN = action_definitions.pb.cc action_definitions.pb.h action_definitions_pb2.py ActionDefinitions.cs

EXE = collector

CXXFLAGS = -Wall -DON_PI -g -std=c++11

LOCAL_IP = $(shell ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | awk '{print $1}')
LOCAL_PORT = 5000

@ $(OBJS): $(HDRS)

collector: $(OBJS)  
	@ echo "Compiled data collection program"
	@ $(CXX) -o $(EXE) $^ -lwiringPi -lpthread -lprotobuf
	@ echo "Program made!"

basic_test: $(OBJSB) 
	@ echo "Compiled data collection program"
	@ $(CXX) -o $@_$(EXE) $^ -lwiringPi -lpthread -lprotobuf
	@ echo "Program made!"
	
clean:
	@ rm -f $(OBJS) $(EXE) *.o $(ACTISH) $(PROTOGEN) 
	@ echo "Ahh clean!"


gui_collector_debug:
	@ echo "http://$(LOCAL_IP):$(LOCAL_PORT)"
	@ gdbgui $(EXE) --host $(LOCAL_IP)  --port $(LOCAL_PORT)

debug:
	@ gdb $(EXE)

documentation:
	@ doxygen docs/Doxyfile

proto:
	@ rm -f action_definitions.pb.h action_definitions.pb.cc action_definitions_pb2.py
	@ protoc -I=. --cpp_out=. ./action_definitions.proto
	@ protoc -I=. --python_out=. ./action_definitions.proto
	@ protoc -I=. --csharp_out=. ./action_definitions.proto
	@ echo "Proto header files compiled"