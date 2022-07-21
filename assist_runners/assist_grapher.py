import matplotlib.pyplot as plt
import numpy
import pandas
import glob
import os
import action_filters as act
import action_figures as act_fig





window_sizes = [ 0.01, 0.02, 0.05 ]

# make plot size bigger
plt.rcParams[ 'figure.figsize' ] = ( 15, 19 )
# adjust the subplot spacing
plt.subplots_adjust( hspace=0.35 )


# take in x,y and make a plot using a given filter and window sizes 1%, 2% ,5% length and return the plot
def get_comparative_data( x, data_incoming, filter="moving_average" ):
    output_array = []
    for i in [
            int( window_sizes[ 0 ] * len( data ) ),
            int( window_sizes[ 1 ] * len( data ) ),
            int( window_sizes[ 2 ] * len( data ) )
    ]:

        filtered_data = act.mean_filter( data_incoming, i )

        output_array.append( filtered_data )
    return output_array


# method to return subplot of data
def get_subplot( data_array_single, x_axis, y_axis_label ):
    # clear the plot
    plt.clf()

    data_array = get_comparative_data( x_axis, data_array_single )

    assert len( data_array ) > 1, "data must be a multidimensional array"

    num_figs = len( data_array ) + 2

    for i in range( num_figs ):

        plt.subplot( num_figs, 1, i + 1 )
        plt.xlabel( "Time (s)" )
        plt.ylabel( y_axis_label )
        if ( i == num_figs - 1 ):
            plt.title( "Original Data" )
            plt.plot( x_axis, data_array_single )
        elif ( i == num_figs - 2 ):
            plt.title( "Filtered data with fixed window of 80" )
            plt.plot( x_axis, act.mean_filter( data_array_single, 80 ) )
        else:
            plt.title( f"Filtered data with {window_sizes[i]*100}% window size ({int(window_sizes[i]*len(data))})" )
            plt.plot( x_axis, data_array[ i ] )


all_files = glob.glob( "*.csv" )  # All available CSV files
all_files.sort()
# Ask user to select file to open

print( "Available files:" )
for i in range( len( all_files ) ):
    print( f"{i+1}. {all_files[i]}" )
# file_number = int( input( "Select file to open: " ) )
file_number = 5
file_name = all_files[ file_number - 1 ]

# open file in pandas and convert to numpy array and take the first C columns
C = 21
df = pandas.read_csv( file_name, header=0 ).iloc[ :, : C ]
data = df.to_numpy()

# delete all files in graphs folder
for file in glob.glob( "graphs/*" ):
    os.remove( file )

if not os.path.exists( "graphs" ):
    os.makedirs( "graphs" )
# Iterate through the columns and plot the data against column number 1 and skip column 2

colled_data = []
x_axis = data[ :, 0 ]
# get all data as columns
for i in range( C ):
    if i > 2:
        colled_data.append( data[ :, i ] )
    else:
        colled_data.append( data[ :, i ] - data[ :, i - 1 ] )

for i in range( len( colled_data ) ):
    nam = df.columns[ i ]
    dat = colled_data[ i ]
    act_fig.get_single_figure( dat, x_axis, nam, nam, "Time (s)", save=f"graphs/{nam}_o" )
    act_fig.get_single_figure( act.mean_filter( dat, 80 ), x_axis, nam, nam, "Time (s)", save=f"graphs/{nam}_mean" )
    act_fig.get_single_figure(
        act.median_filter_main( dat, 80 ), x_axis, nam, nam, "Time (s)", save=f"graphs/{nam}_median" )
