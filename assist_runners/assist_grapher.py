import matplotlib.pyplot
import numpy
import pandas
import glob
import os

window_sizes = [0.01, 0.02, 0.05]

# make plot size bigger
matplotlib.pyplot.rcParams['figure.figsize'] = (15, 19)
# adjust the subplot spacing
matplotlib.pyplot.subplots_adjust(hspace=0.35)

# make moving average of data of same length
def moving_average(data, window_size):
    if window_size == 0:
        return data
    weights = numpy.repeat(1.0, window_size)/window_size
    sma = numpy.convolve(data, weights, 'valid')
    return sma

def fix_moving_average_output(data_array, x_axis):
    while (len(data_array) != len(x_axis)):
        # fill in the missing data with the last value
        data_array = numpy.append(data_array, data_array[-1])
    return data_array

# take in x,y and make a plot using a given filter and window sizes 1%, 2% ,5% length and return the plot
def get_comparative_data(x, data_incoming, filter="moving_average"):
    output_array = []
    for i in [ int(window_sizes[0]*len(data)), int(window_sizes[1]*len(data)), int(window_sizes[2]*len(data))]:
        filtered_data = moving_average(data_incoming, i)
        #ensure that the moving average filter is the same length as the x axis
        filtered_data = fix_moving_average_output(filtered_data, x)
        output_array.append(filtered_data)
    return output_array

# method to return subplot of data
def get_subplot(data_array_single, x_axis, y_axis_label):

    # assert len(data_array_single) == 1, "data must be a 1 dimensional array"

    data_array = get_comparative_data(x_axis, data_array_single)

    assert len(data_array) > 1, "data must be a multidimensional array"
    
    num_figs = len(data_array) + 2
    
    for i in range(num_figs):
        matplotlib.pyplot.subplot( num_figs, 1, i + 1 )
        matplotlib.pyplot.xlabel("Time (s)")
        matplotlib.pyplot.ylabel(y_axis_label)
        if (i == num_figs-1):
            matplotlib.pyplot.title("Original Data")
            matplotlib.pyplot.plot(x_axis, data_array_single)
        elif (i == num_figs-2):
            matplotlib.pyplot.title("Filtered data with fixed window of 80")
            matplotlib.pyplot.plot(x_axis, fix_moving_average_output(moving_average(data_array_single, 80), x_axis))
        else:
            matplotlib.pyplot.title(f"Filtered data with {window_sizes[i]*100}% window size ({int(window_sizes[i]*len(data))})")
            matplotlib.pyplot.plot(x_axis, data_array[i])




all_files = glob.glob("*.csv") # All available CSV files
all_files.sort()
# Ask user to select file to open

print("Available files:")
for i in range(len(all_files)):
    print(f"{i+1}. {all_files[i]}")
file_number = int(input("Select file to open: "))
file_name = all_files[file_number-1]

# open file in pandas and convert to numpy array and take the first C columns
C = 21
df = pandas.read_csv(file_name, header=0).iloc[: , :C]
data = df.to_numpy()

# delete all files in graphs folder
for file in glob.glob("graphs/*"):
    os.remove(file)

if not os.path.exists("graphs"):
    os.makedirs("graphs")
# Iterate through the columns and plot the data against column number 1 and skip column 2

for i in range(len(df.columns)-1):
    if (i < 1):
        continue
    x_axis = data[:,0]

    # Label the graph
    y_axis_label = str(df.columns[i+1]).replace("1", "").title()
    title = y_axis_label + ""

    get_subplot( data[:,i+1], x_axis, y_axis_label)

    matplotlib.pyplot.savefig(f"graphs/{title}_multi.png")
    matplotlib.pyplot.close()