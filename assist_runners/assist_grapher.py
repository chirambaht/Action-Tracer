import matplotlib.pyplot
import numpy
import pandas
import glob
import os

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

if not os.path.exists("graphs"):
        os.makedirs("graphs")
# delete all files in graphs folder
for file in glob.glob("graphs/*"):
    os.remove(file)

# Convert all Accelerometer values to m/s^2
for i in range(len(df.columns)):
    # If column has Accel in its heading, convert all column data to m/s^2

    # If column has Gyro in its heading, convert to rad/s
    if "Accel" in df.columns[i]:
        print("Accel was: ", data[:,i], end="")
        data[i,1] = (data[i,1] / 16384 ) * 9.81 # Convert to m/s^2
        print("Now: ", data[:,i])
    elif "Gyro" in df.columns[i]:
        print("Gyro was: ", data[:,i], end="")
        data[i,1] = (data[i,1] / 16.4) * (2*numpy.pi/360) # Convert to rad/s
        print("Now: ", data[:,i])

# Iterate through the columns and plot the data against column number 1 and skip column 2
    
for i in range(len(df.columns)-1):
    if i < 1:
        continue
    # Label the axis
    matplotlib.pyplot.xlabel(df.columns[0])
    matplotlib.pyplot.ylabel(df.columns[i+1])
    # Plot the data
    matplotlib.pyplot.plot(data[:,0], data[:,i+1])
    matplotlib.pyplot.title(f"{df.columns[i+1]}")
    matplotlib.pyplot.savefig(f"graphs/{df.columns[i+1]}.png")
    matplotlib.pyplot.close()





