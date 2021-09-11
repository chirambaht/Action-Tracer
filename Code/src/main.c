#include <stdio.h>
#include "MPU6050.h"
#include "main.h"

int main(){
    int file = open("/dev/i2c-1", O_RDWR);
    if (file < 0) {
        exit(1);
    }

    MPU6050 device;

    MPU6050_Init(&device, &file); 

    printf("Hello world");

    for (;;){
        // This is the main for loop. It's gonna work hard
    }
    return 1;
}