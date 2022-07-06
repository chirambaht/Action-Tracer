clean:
	@ cd Offsets/ && $(MAKE) clean && cd ../
	@ cd Collection/ && $(MAKE) clean && cd ../

all: 
	@ cd Collection/ && $(MAKE) all
	@ cd Offsets/ && $(MAKE) all

compile_collection:
	@  cd Collection/ && $(MAKE) all

compile_offsets:
	@  cd Offsets/ && $(MAKE) all

collect:
	@ cd Collection/ && ./collect_data

IMU_zero:
	@ cd Offsets/ && ./IMU_zero