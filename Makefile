OBJS	= main.o MPU6050.o
SOURCE	= main.c MPU6050.c
HEADER	= MPU6050.h
OUT	= action-tracer
CC	 = gcc
FLAGS	 = -g -c -Wall -lwiringPi 
LFLAGS	 = -lm
# -g option enables debugging mode 
# -c flag generates object code for separate files


all: $(OBJS)
	@ echo "Compiling and creating executable"
	@ $(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)


# create/compile the individual files >>separately<<
main.o: main.c
	@ echo "Compiling main"
	@ $(CC) $(FLAGS) main.c 

MPU6050.o: MPU6050.c
	$(CC) $(FLAGS) MPU6050.c 


# clean house
clean:
	@ echo "Cleaning up"
	@ rm -f $(OBJS) $(OUT)

# run the program
run: $(OUT)
	./$(OUT)