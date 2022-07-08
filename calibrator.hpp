#ifndef CALIBRATOR_HPP
##define CALIBRATOR_HPP
	#include "action_pi.hpp"

	#include <cstdint>

	namespace ActionTracer {
	const __int16_t FAST_SLOW_LIMIT = 500;

	const int iAx = 0;
	const int iAy = 1;
	const int iAz = 2;
	const int iGx = 3;
	const int iGy = 4;
	const int iGz = 5;

	int NFast = FAST_SLOW_LIMIT; // the bigger, the better (but slower)
	int NSlow = FAST_SLOW_LIMIT; // ..

	int LowValue[6];
	int HighValue[6];
	int Smoothed[6];

	int LowOffset[6]  = { 0 };
	int HighOffset[6] = { 0 };
	int Target[6]	  = { 0 };

	int N;

	void SetOffsets( MPU6050 * _device, int TheOffsets[6] ) {
		_device->setXAccelOffset( TheOffsets[iAx] );
		_device->setYAccelOffset( TheOffsets[iAy] );
		_device->setZAccelOffset( TheOffsets[iAz] );
		_device->setXGyroOffset( TheOffsets[iGx] );
		_device->setYGyroOffset( TheOffsets[iGy] );
		_device->setZGyroOffset( TheOffsets[iGz] );
	} // SetOffsets

	void GetSmoothed( MPU6050 * _device ) {
		int16_t RawValue[6];
		int		i;
		long	Sums[6] = { 0 };

		for( i = 1; i <= N; i++ ) {
			// get sums
			_device->getMotion6( &RawValue[iAx], &RawValue[iAy], &RawValue[iAz],
				&RawValue[iGx], &RawValue[iGy], &RawValue[iGz] );

			if( ( i % 500 ) == 0 ) {
			}

			for( int j = iAx; j <= iGz; j++ ) {
				Sums[j] = Sums[j] + RawValue[j];
			}
		} // get sums

		for( i = iAx; i <= iGz; i++ ) {
			Smoothed[i] = ( Sums[i] + N / 2 ) / N;
		}
	} // GetSmoothed

	void SetAveraging( int NewN ) {
		N = NewN;
	} // SetAveraging

	void PullBracketsIn( MPU6050 * _device ) {
		bool AllBracketsNarrow;
		bool StillWorking;
		int	 NewOffset[6];

		AllBracketsNarrow = false;
		StillWorking	  = true;

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

			SetOffsets( _device, NewOffset );
			GetSmoothed( _device );

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
	}			  // PullBracketsIn

	void PullBracketsOut( MPU6050 * _device ) {
		bool Done = false;
		int	 NextLowOffset[6];
		int	 NextHighOffset[6];

		while( !Done ) {
			Done = true;
			SetOffsets( _device, LowOffset );
			GetSmoothed( _device );

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

			SetOffsets( _device, HighOffset );
			GetSmoothed( _device );

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

			for( int i = iAx; i <= iGz; i++ ) {
				LowOffset[i]  = NextLowOffset[i];  // had to wait until ShowProgress done
				HighOffset[i] = NextHighOffset[i]; // ..
			}
		} // keep going
	}	  // PullBracketsOut

	int _calibrate_device( MPU6050 * _device, uint8_t falst_slow_limit ) {
		NFast		= falst_slow_limit;
		NSlow		= falst_slow_limit;
		Target[iAz] = 16384;

		SetAveraging( NFast );
		PullBracketsOut( _device );
		PullBracketsIn( _device );

		SetOffsets( _device, LowOffset );
		return 0;
	}
} // namespace ActionTracer

#endif