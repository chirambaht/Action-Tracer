clean:
	@ cd Offsets/ && $(MAKE) clean && cd ../
	@ cd Collection/ && $(MAKE) clean && cd ../

all: 
	@ cd Collection/ && $(Make)
	@ cd Offsets/ && $(Make)

compile_collection:
	@  cd Collection/ && $(Make)

compile_offsets:
	@  cd Offsets/ && $(Make)

collect:
	@ cd Collection/ && ./collect_data

IMU_zero:
	@ cd Offsets/ && ./IMU_zero