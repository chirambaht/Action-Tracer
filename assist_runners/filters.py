from matplotlib.pyplot import plot
import numpy as np
import matplotlib.pyplot

# create a moving average filter
def moving_average(data, window_size):
    weights = np.repeat(1.0, window_size)/window_size
    return np.convolve(data, weights, 'valid')

# create a kalman filter

# calculate yaw, pitch and roll from quaternions

# create a an array in ascending order of 20 values
def create_array(start, end):
    return np.arange(start, end, 1)

# create sine wave
def create_sine(start, end, freq):
    return np.sin(create_array(start, end) * freq)

# add noise to sine wave
def add_noise(sine, noise_amplitude):
    return sine + noise_amplitude * np.random.randn(len(sine))

a = create_sine(0, 100, 500)
b = add_noise(a, 0.5)
filtered = moving_average(a, 1)

# plot the data and the filtered data
print(len(a), len(b), len(filtered))
plot(a, 'r')
# plot(b, 'g')
# plot(filtered, 'b')
matplotlib.pyplot.show()

