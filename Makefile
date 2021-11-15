# This is the overall makefile that will run the code from both ends.

check:
	@ echo "Done and dusted"

pi_compile:
	@ cd Code/Capture/Collection &&  $(MAKE) all

pi_clean:
	@ cd Code/Capture/Collection && $(MAKE) clean

pi_run:
	@ cd Code/Capture/Collection && ./collect_data

imu_zero_compile:
	@ cd Code/Capture/Offsets && $(MAKE) all

imu_zero_clean:
	@ cd Code/Capture/Offsets && $(MAKE) clean

imu_zero_run:
	@ cd Code/Capture/Offsets && ./IMU_zero