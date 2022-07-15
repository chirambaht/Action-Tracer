import matplotlib.pyplot
import numpy
import pandas
import glob
import os

b1 = 0.01
b2 = 0.0125
b3 = 0.015

# make moving average of data of same length
def moving_average(data, window_size):
    if window_size == 0:
        return data
    weights = numpy.repeat(1.0, window_size)/window_size
    sma = numpy.convolve(data, weights, 'valid')
    return sma

# take in x,y and make a plot using a given filter and window sizes 1%, 2% ,5% length and return the plot
def get_comparative_data(x, data_incoming, filter="moving_average"):
    output_array = []
    for i in [ int(b1*len(data)), int(b2*len(data)), int(b3*len(data))]:
        filtered_data = moving_average(data_incoming, i)
        #ensure that the moving average filter is the same length as the x axis
        while (len(filtered_data) != len(x)):
            # fill in the missing data with the last value
            filtered_data = numpy.append(filtered_data, filtered_data[-1])
        output_array.append(filtered_data)
    return output_array

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

tot = data.__len__()
print(f"1%: {int(tot*0.01)}")
print(f"1.25%: {int(tot*0.0125)}")
print(f"1.5%: {int(tot*0.015)}")

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

    matplotlib.pyplot.xlabel("Time (s)")
    matplotlib.pyplot.ylabel(y_axis_label)
    matplotlib.pyplot.title(f"{title}")

    all_plots = get_comparative_data(data[:,0], data[:,i+1], "")
    lim = -1
    # Plot the data
    # matplotlib.pyplot.plot(data[:,0],  data[:,i+1])
    # matplotlib.pyplot.legend(["Original",str(b1*100)+"%", str(b2*100)+"%",str(b3*100)+"%"])

    for pl in all_plots:
        matplotlib.pyplot.plot(data[:,0], pl)
    matplotlib.pyplot.legend([str(b1*100)+"%", str(b2*100)+"%",str(b3*100)+"%"])
    
    
    # matplotlib.pyplot.plot(x_axis, filtered_data, "r")
    
    matplotlib.pyplot.savefig(f"graphs/{title}_multi.png")
    matplotlib.pyplot.close()







