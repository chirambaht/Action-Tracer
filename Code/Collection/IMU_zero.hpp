#include "I2Cdev.h"
#include "MPU6050.h"
#include "action_pi.hpp"

#include <array>
#include <cstdio>

#ifdef ON_PI
	#include <wiringPi.h>
	#ifndef HIGH
		#define HIGH 1
	#endif
	#ifndef LOW
		#define LOW 0
	#endif
#endif

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

int NFAST_VALUE = 1000;
int NSLOW_VALUE = 1000;

using namespace std;

#define ZERO_ALL

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
// MPU6050 accelgyro;
MPU6050 accelgyro( 0x69 ); // <-- use for AD0 high

const char LBRACKET = '[';
const char RBRACKET = ']';
const char COMMA	= ',';
const char BLANK	= ' ';
const char PERIOD	= '.';

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;

const int usDelay			  = 3150;		 // empirical, to hold sampling to 200 Hz
const int NFast				  = NFAST_VALUE; // the bigger, the better (but slower)
const int NSlow				  = NSLOW_VALUE; // ..
const int LinesBetweenHeaders = 5;

int LowValue[6];
int HighValue[6];
int Smoothed[6];
int LowOffset[6];
int HighOffset[6];
int Target[6];
int LinesOut;
int N;

void ForceHeader() {
	LinesOut = 99;
}

void SetOffsets( int TheOffsets[6] ) {
	accelgyro.setXAccelOffset( TheOffsets[iAx] );
	accelgyro.setYAccelOffset( TheOffsets[iAy] );
	accelgyro.setZAccelOffset( TheOffsets[iAz] );
	accelgyro.setXGyroOffset( TheOffsets[iGx] );
	accelgyro.setYGyroOffset( TheOffsets[iGy] );
	accelgyro.setZGyroOffset( TheOffsets[iGz] );
} // SetOffsets

void GetSmoothed() {
	int16_t RawValue[6];
	int		i;
	long	Sums[6];
	for( i = iAx; i <= iGz; i++ ) {
		Sums[i] = 0;
	}
	//    unsigned long Start = micros();

	for( i = 1; i <= N; i++ ) {
		// get sums
		accelgyro.getMotion6( &RawValue[iAx], &RawValue[iAy], &RawValue[iAz],
			&RawValue[iGx], &RawValue[iGy], &RawValue[iGz] );
		if( ( i % 500 ) == 0 ) {
			printf( "%c", PERIOD );
		}
		// delayMicroseconds(usDelay);
		for( int j = iAx; j <= iGz; j++ ) {
			Sums[j] = Sums[j] + RawValue[j];
		}
	} // get sums
	//    unsigned long usForN = micros() - Start;
	//    printf(" reading at %d Hz\n", 1000000/((usForN+N/2)/N));
	for( i = iAx; i <= iGz; i++ ) {
		Smoothed[i] = ( Sums[i] + N / 2 ) / N;
	}
} // GetSmoothed

void Initialize() {
	// initialize device
	accelgyro.initialize();

	// verify connection
	if( !accelgyro.testConnection() ) {
		printf( "connection failed\n" );
		return;
	}

	printf( "connected!\n" );
	accelgyro.initialize();
} // Initialize

void SetAveraging( int NewN ) {
	N = NewN;
} // SetAveraging

void PullBracketsIn() {
	bool AllBracketsNarrow;
	bool StillWorking;
	int	 NewOffset[6];

	AllBracketsNarrow = false;
	ForceHeader();
	StillWorking = true;
	while( StillWorking ) {
		StillWorking = false;
		if( AllBracketsNarrow && ( N == NFast ) ) {
			SetAveraging( NSlow );
		} else {
			AllBracketsNarrow = true;
		} // tentative
		for( int i = iAx; i <= iGz; i++ ) {
			if( HighOffset[i] <= ( LowOffset[i] + 1 ) ) {
				NewOffset[i] = LowOffset[i];
			} else {
				// binary search
				StillWorking = true;
				NewOffset[i] = ( LowOffset[i] + HighOffset[i] ) / 2;
				if( HighOffset[i] > ( LowOffset[i] + 10 ) ) {
					AllBracketsNarrow = false;
				}
			} // binary search
		}
		SetOffsets( NewOffset );
		GetSmoothed();
		for( int i = iAx; i <= iGz; i++ ) {
			// closing in
			if( Smoothed[i] > Target[i] ) {
				// use lower half
				HighOffset[i] = NewOffset[i];
				HighValue[i]  = Smoothed[i];
			} // use lower half
			else {
				// use upper half
				LowOffset[i] = NewOffset[i];
				LowValue[i]	 = Smoothed[i];
			} // use upper half
		}	  // closing in
			  // ShowProgress();
	}		  // still working
} // PullBracketsIn

void PullBracketsOut() {
	bool Done = false;
	int	 NextLowOffset[6];
	int	 NextHighOffset[6];

	ForceHeader();

	while( !Done ) {
		Done = true;
		SetOffsets( LowOffset );
		GetSmoothed();
		for( int i = iAx; i <= iGz; i++ ) {
			// got low values
			LowValue[i] = Smoothed[i];
			if( LowValue[i] >= Target[i] ) {
				Done			 = false;
				NextLowOffset[i] = LowOffset[i] - 1000;
			} else {
				NextLowOffset[i] = LowOffset[i];
			}
		} // got low values

		SetOffsets( HighOffset );
		GetSmoothed();
		for( int i = iAx; i <= iGz; i++ ) {
			// got high values
			HighValue[i] = Smoothed[i];
			if( HighValue[i] <= Target[i] ) {
				Done			  = false;
				NextHighOffset[i] = HighOffset[i] + 1000;
			} else {
				NextHighOffset[i] = HighOffset[i];
			}
		} // got high values
		// ShowProgress();
		for( int i = iAx; i <= iGz; i++ ) {
			LowOffset[i]  = NextLowOffset[i];  // had to wait until ShowProgress done
			HighOffset[i] = NextHighOffset[i]; // ..
		}
	} // keep going
} // PullBracketsOut

__int16_t *obtain_offsets( int pin_number ) {
	// for each of the devices, set address of 0x69 and program

#ifdef ON_PI
	digitalWrite( pin_number, HIGH );
#endif

	// TODO: Add progress indicator

	Initialize();

	for( int i = iAx; i <= iGz; i++ ) { // set targets and initial guesses
		Target[i]	  = 0;				// must fix for ZAccel
		HighOffset[i] = 0;
		LowOffset[i]  = 0;
	} // set targets and initial guesses

	Target[iAz] = 16384;

	SetAveraging( NFast );
	PullBracketsOut();
	PullBracketsIn();

#ifdef ON_PI
	digitalWrite( pin_number, LOW );
#endif

	__int16_t *offsets = new __int16_t[6]{ ( LowOffset[0] ), ( LowOffset[1] ), ( LowOffset[2] ), ( LowOffset[3] ), ( LowOffset[4] ), ( LowOffset[5] ) };
	// Created and saved in memory to keep available

	return offsets;
}
