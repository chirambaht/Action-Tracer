from operator import index
import matplotlib.pyplot as py
import numpy as np
import pandas as pd

# # combine phone data from 3 source ibnto one dataframe matching the first 2 columns
# def combine_data(df1, df2, df3):
#     # concactinate all dataframes with index as column 0 and column 1
#     df = pd.concat([df1, df2, df3], axis=1)
#     # remove first column 
#     df = df.drop(df.columns[0], axis=1)
#     return df

# # round off the dataframe to x decimal places
# def round_off(df, x):
#     df = df.round(x)
#     return df

# # open 3 csv files and return a dataframe
# def open_csv(file1, file2, file3):
#     df1 = pd.read_csv(file1)
#     df2 = pd.read_csv(file2)
#     df3 = pd.read_csv(file3)
#     return df1, df2, df3

# a,b,c = open_csv("Accelerometer.csv", "Orientation.csv", "Gravity.csv")

# dft = combine_data(a,b,c)
# dft = round_off(dft, 5)
# # save dataframe to csv file
# dft.to_csv("phone_data.csv", index=False)

# make moving average of data of same length
def moving_average(data, window_size):
    if window_size == 0:
        return data
    weights = np.repeat(1.0, window_size)/window_size
    sma = np.convolve(data, weights, 'valid')
    return sma

def fix_moving_average_output(data_array, x_axis):
    while (len(data_array) != len(x_axis)):
        # fill in the missing data with the last value
        data_array = np.append(data_array, data_array[-1])
    return data_array

# open csv file and return a dataframe
def open_csv(file):
    df = pd.read_csv(file, header=0)
    return df

# convert pandas dataframe to numpy array
def convert_to_array(df):
    
    x = df.iloc[:, 0].values
    y = df.iloc[:, 1].values
    return x, y

# plot all data from numpy array
def plot_data(x, y, x_label, y_label, title):
    fig, ax = py.subplots(2, 1)
    ax[0].plot(x, y[0])
    ax[0].set_xlabel(x_label)
    ax[0].set_ylabel(y_label[0])
    ax[0].set_title(title[0])
    ax[1].plot(x, y[1])
    ax[1].set_xlabel(x_label)
    ax[1].set_ylabel(y_label[1])
    ax[1].set_title(title[1])
    py.show()
    return fig, ax


# plot data from numpy array with given x and y axis labels no subplots
def plot_data(x, y, x_label, y_label, title):
    
    py.xlabel(x_label)
    py.ylabel(y_label)
    py.title(title)
    py.plot(x, y)
    py.savefig(f"phone_data_{title}.png")
    


df = open_csv("phone_data.csv")

#show columns
# convert df to numpy array
# select column 0
x_axis = df.iloc[:, 0].values 

# remove first column
df = df.drop(df.columns[0], axis=1)

# plot rest of data against x axis
for i in range(len(df)):
    # select column i
    y_axis = df.iloc[:, i].values
    # plot data with coluine name as title
    # pass y axis throuth moving average function and fix output
    plot_data(x_axis, fix_moving_average_output(moving_average(y_axis, 200), x_axis), "Time", df.columns[i], df.columns[i])
    plot_data(x_axis, fix_moving_average_output(moving_average(y_axis, 2000), x_axis), "Time", df.columns[i], df.columns[i])
    # plot_data(x_axis, y_axis, "Time", df.columns[i], df.columns[i])
    #clear figure
    py.clf()
    
    