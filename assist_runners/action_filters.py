import numpy as np

# Constants
MEDIAN_FILTER = 0
MEAN_FILTER = 1


# median filter
def median_filter( data, window_size ):
    if window_size == 0:
        return data
    weights = np.repeat( 1.0, window_size ) / window_size
    median = np.convolve( data, weights, 'valid' )

    while len( median ) < len( data ):
        median = np.append( median, median[ -1 ] )

    return median


# mean filter
def mean_filter( data, window_size ):
    if window_size == 0:
        return data
    og_data = data.astype( np.float32 )

    for i in range( len( data ) ):
        if i < window_size:
            data[ i ] = np.mean( og_data[ : i + 1 ] )
        else:
            data[ i ] = np.mean( og_data[ i - window_size : i + 1 ] )

    return data


def mean_median_comp( data, window, mean_bias=0.5 ):
    return ( mean_bias * mean_filter( data, window ) ) + ( ( 1 - mean_bias ) * median_filter( data, window ) )
