clean:
	@ cd Offsets/ && $(MAKE) clean && cd ../
	@ cd Colletion/ && $(MAKE) clean && cd ../

all: 
	@ cd Colletion/ && $(Make)
	@ cd Offsets/ && $(Make)

compile_collection:
	@  cd Colletion/ && $(Make)

compile_offsets:
	@  cd Offsets/ && $(Make)

collect:
	@ cd Colletion/ && ./collect_data

IMU_zero:
	@ cd Offsets/ && ./IMU_zero