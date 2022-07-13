import matplotlib.pyplot
import numpy
import pandas
import glob
import os

all_files = glob.glob("*.csv") # All available CSV files

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


# Iterate through the columns and plot the data against column number 1 and skip column 2
    
for i in range(len(df.columns)-1):
    if i < 1:
        continue
    # Label the axis
    matplotlib.pyplot.xlabel(df.columns[0])
    matplotlib.pyplot.ylabel(df.columns[i])
    # Plot the data
    matplotlib.pyplot.plot(data[:,0], data[:,i+1])
    matplotlib.pyplot.title(f"{df.columns[i]}")
    matplotlib.pyplot.savefig(f"graphs/{df.columns[i]}.png")
    matplotlib.pyplot.close()

