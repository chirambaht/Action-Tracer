import matplotlib.pyplot
import numpy
import pandas
import glob
import os

# make moving average of data of same length
def moving_average(data, window_size):
    weights = numpy.repeat(1.0, window_size)/window_size
    sma = numpy.convolve(data, weights, 'valid')
    return sma

all_files = glob.glob("*.csv") # All available CSV files
all_files.sort()
# Ask user to select file to open
print("Available files:")
for i in range(len(all_files)):
    print(f"{i+1}. {all_files[i]}")
file_number = int(input("Select file to open: "))
file_name = all_files[file_number-1]

# open file in pandas and convert to numpy array
df = pandas.read_csv(file_name, header=0)
data = df.to_numpy()

# delete all files in graphs folder
for file in glob.glob("graphs/*"):
    os.remove(file)

if not os.path.exists("graphs"):
    os.makedirs("graphs")
# Iterate through the columns and plot the data against column number 1 and skip column 2
    
for i in range(len(df.columns)-1):
    if i < 1:
        continue
    # if title contains "1" plot otherwise skip

    x_axis = data[:,0]
    if "1" in df.columns[i]:
        # Label the axis
        matplotlib.pyplot.xlabel(df.columns[0])
        matplotlib.pyplot.ylabel(df.columns[i+1])
        # Plot the data
        matplotlib.pyplot.plot(data[:,0], data[:,i+1])
        matplotlib.pyplot.title(f"{df.columns[i+1]}")
        matplotlib.pyplot.savefig(f"graphs/{df.columns[i+1]}_plain.png")
        matplotlib.pyplot.close()

    elif "2" in df.columns[i]:
        # Label the axis
        matplotlib.pyplot.xlabel(df.columns[0])
        matplotlib.pyplot.ylabel(df.columns[i+1])

        # Plot the data
        #run data through moving average filter
        filtered_data = moving_average(data[:,i+1], 200)
        
        #ensure that the moving average filter is the same length as the x axis
        while (len(filtered_data) != len(x_axis)):
            # fill in the missing data with the last value
            filtered_data = numpy.append(filtered_data, filtered_data[-1])
            # filtered_data = numpy.append(filtered_data, numpy.zeros(len(x_axis)-len(filtered_data)))

        matplotlib.pyplot.plot(x_axis, filtered_data)
        # matplotlib.pyplot.plot(data[:,0], data[:,i+1])
        matplotlib.pyplot.title(f"{df.columns[i+1]}")
        matplotlib.pyplot.savefig(f"graphs/{df.columns[i+1]}_filtered.png")
        matplotlib.pyplot.close()





