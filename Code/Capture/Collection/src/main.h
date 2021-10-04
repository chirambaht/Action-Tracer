#ifndef _MAIN_H_
#define _MAIN_H_

#define SEND_TO_MOBILE

#define SERVER_IP  "127.0.0.1" // Server IP where data will be sent to
#define MOBILE_IP  "127.0.0.1" // Mobile IP where data will be sent to
#define PORT	   9022		   // Main UDP data port for the data to be sent through
#define N		   3		   // Number of points track on the body.
#define LOOP_DELAY 1000		   // ms to wait per loop cycle

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "packager.h"
#include "tracer_point.h"

#include <dirent.h>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <wiringPi.h>

using namespace ActionTracer;

TracePoint body_sensor[N]; // These are the N sensors on the body.
Packager   communicator;

// Packager mobile = new Packager( MOBILE_IP, PORT );

void setup();
void loop();
// namespace ActionTracer

#endif