import matplotlib.pyplot as plt
import numpy
import pandas
import glob
import os
import action_filters as act
import action_figures as act_fig





window_sizes = [ 0.01, 0.02, 0.05 ]

# make plot size bigger
plt.rcParams[ 'figure.figsize' ] = ( 20, 14 )
# adjust the subplot spacing
plt.subplots_adjust( hspace=0.35 )

all_files = glob.glob( "*.csv" )  # All available CSV files

all_files.sort()
# Ask user to select file to open

while ( all_files.__contains__( "_" ) ):
    for i in range( len( all_files ) ):
        if ( all_files[ i ].__contains__( "_" ) ):
            all_files.remove( all_files[ i ] )

print( "Available files:" )
for i in range( len( all_files ) ):
    print( f"{i+1}. {all_files[i]}" )
file_number = int( input( "Select file to open with twin: " ) )
# file_number = 5
file_name = all_files[ file_number - 1 ]

# open file in pandas and convert to numpy array and take the first C columns
C = 22
df = pandas.read_csv( file_name, header=0 ).iloc[ :, : C ]
df2 = pandas.read_csv( file_name[ :-4 ] + "_2.csv", header=0 ).iloc[ :, : C ]
df3 = pandas.read_csv( file_name[ :-4 ] + "_3.csv", header=0 ).iloc[ :, : C ]
data = df.to_numpy()
data2 = df2.to_numpy()
data3 = df3.to_numpy()

# delete all files in graphs folder
for file in glob.glob( "graphs/*" ):
    os.remove( file )

if not os.path.exists( "graphs" ):
    os.makedirs( "graphs" )
# Iterate through the columns and plot the data against column number 1 and skip column 2

colled_data = []
colled_data2 = []
colled_data3 = []
x_axis = data[ :, 0 ]
# get all data as columns
for i in range( C ):
    if i > 2:
        colled_data.append( data[ :, i ] )
    else:
        colled_data.append( data[ :, i ] - data[ :, i - 1 ] )
for i in range( C ):
    if i > 2:
        colled_data2.append( data2[ :, i ] )
    else:
        colled_data2.append( data2[ :, i ] - data2[ :, i - 1 ] )
for i in range( C ):
    if i > 2:
        colled_data3.append( data3[ :, i ] )
    else:
        colled_data3.append( data3[ :, i ] - data3[ :, i - 1 ] )

for i in range( len( colled_data ) ):
    nam = df.columns[ i ]
    dat = colled_data[ i ]
    dat2 = colled_data2[ i ]
    dat3 = colled_data3[ i ]
    plt.plot( x_axis, dat, label=f"{nam} raw" )
    plt.plot( x_axis, dat2, label=f"{nam} mean" )
    plt.plot( x_axis, dat3, label=f"{nam} median" )
    plt.plot( x_axis, act.mean_filter( dat, 500 ), label=f"{nam} post" )
    plt.plot( x_axis, act.mean_median_comp( dat, 200, 0.5 ), label=f"{nam} post comp" )
    plt.legend()
    plt.savefig( f"graphs/{nam}.png" )
    plt.close()
