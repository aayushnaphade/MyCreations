import os
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Conv2D, Lambda, Conv2DTranspose, concatenate
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.losses import MeanSquaredError
from sklearn.metrics import mean_squared_error
from matplotlib import pyplot as plt
import scipy.io as sio

# Load and preprocess the data
def load_and_preprocess_data(file_names, data_path):
    data_list = []
    for file_name in file_names:
        mat_contents = sio.loadmat(os.path.join(data_path, file_name))
        variable_name = file_name.split('.')[0]
        
        # Extract the real and imaginary parts
        real_values = mat_contents[variable_name][:, :, 0]
        imag_values = mat_contents[variable_name][:, :, 1]
        
        # Combine real and imaginary parts to form complex-valued images
        complex_data = real_values + 1j * imag_values
        
        # Take the magnitude of the complex data
        magnitude_data = np.abs(complex_data)
        
        # Expand dimensions and append to the data list
        magnitude_data = np.expand_dims(magnitude_data, axis=-1)
        data_list.append(magnitude_data)
    
    return np.array(data_list)

def complex_to_real_imag(x):
    real = tf.math.real(x)
    imag = tf.math.imag(x)
    return real, imag

def real_imag_to_complex(x):
    return tf.complex(x[0], x[1])

def calculate_snr(original, reconstructed):
    signal_power = np.sum(np.abs(original)**2)
    noise_power = np.sum(np.abs(original - reconstructed)**2)
    snr = 10 * np.log10(signal_power / noise_power)
    return snr

def calculate_rmse(original, reconstructed):
    return np.sqrt(np.mean((np.abs(original) - np.abs(reconstructed))**2))

# Data paths and file names
data_path = 'actual/'
file_names = [f'MR{i}.mat' for i in range(1, 113)]

# Load and preprocess the data
data_array = load_and_preprocess_data(file_names, data_path)

# Split data into train, validation, and test sets
def train_test_val_split(data_array, val_split=0.1, test_split=0.1):
    num_samples = data_array.shape[0]
    num_val = int(num_samples * val_split)
    num_test = int(num_samples * test_split)
    num_train = num_samples - num_val - num_test

    x_train = data_array[:num_train]
    x_val = data_array[num_train:num_train+num_val]
    x_test = data_array[num_train+num_val:]

    return x_train, x_val, x_test

x_train, x_val, x_test = train_test_val_split(data_array)

# Set the undersampling ratio (e.g., 0.5 means we keep 50% of the frequencies)
undersampling_ratio = 0.5

# Assuming you have a training data array named "x_train"
# Perform 2D Fourier transform on the training data
x_train_fourier = np.fft.fft2(x_train)

# Calculate the number of frequencies to keep in the undersampled Fourier data
num_frequencies_to_keep = int(undersampling_ratio * x_train.size)

# Get the indices of all frequencies in the Fourier data
all_indices = np.arange(x_train.size)

# Randomly select the indices of the frequencies to keep
undersampled_indices = np.random.choice(all_indices, size=num_frequencies_to_keep, replace=False)

# Create a new undersampled Fourier data by setting the frequencies not in undersampled_indices to zero
undersampled_fourier = np.zeros_like(x_train_fourier)
undersampled_fourier.flat[undersampled_indices] = x_train_fourier.flat[undersampled_indices]

# Perform inverse Fourier transform to get the undersampled training data
x_train_undersampled = np.abs(np.fft.ifft2(undersampled_fourier))

# Define the CNN model
def create_cnn_model(input_shape):
    # Complex input layer
    inputs = Input(shape=input_shape, dtype=tf.complex64)

    # Split complex input into real and imaginary parts
    real_part, imag_part = Lambda(complex_to_real_imag)(inputs)

    # Real-valued convolution layers with ReLU activation
    conv1_real = Conv2D(32, (3, 3), activation='relu', padding='same')(real_part)
    conv2_real = Conv2D(64, (3, 3), activation='relu', padding='same')(conv1_real)

    # Imaginary-valued convolution layers with ReLU activation
    conv1_imag = Conv2D(32, (3, 3), activation='relu', padding='same')(imag_part)
    conv2_imag = Conv2D(64, (3, 3), activation='relu', padding='same')(conv1_imag)

    # Concatenate real and imaginary parts along the channel axis
    merged_conv = concatenate([conv2_real, conv2_imag], axis=-1)

    # Real-valued transposed convolution layer
    deconv1_real = Conv2DTranspose(32, (3, 3), activation='relu', padding='same')(merged_conv)

    # Complex-valued output layer
    outputs = Conv2D(1, (3, 3), activation='linear', padding='same')(deconv1_real)

    # Merge real and imaginary parts back to complex-valued data
    outputs_complex = Lambda(real_imag_to_complex)([outputs, tf.zeros_like(outputs)])

    model = Model(inputs=inputs, outputs=outputs_complex)

    return model

# Define the input shape (size of your images and the number of channels, which is 1 for magnitude images)
input_shape = (256, 256, 1)

# Create the CNN model
cnn_model = create_cnn_model(input_shape)

# Print the model summary
cnn_model.summary()

# Define the loss function for image reconstruction
mse_loss = MeanSquaredError()

# Compile the model
cnn_model.compile(optimizer=Adam(learning_rate=1e-4), loss=mse_loss)

# Train the CNN model with the undersampled data
cnn_model.fit(x_train_undersampled, x_train, batch_size=16, epochs=1, validation_data=(x_val, x_val))

# # Test the model on the test data
# x_test_fourier = np.fft.fft2(x_test)

# # Calculate the number of frequencies to keep in the undersampled Fourier data
# num_frequencies_to_keep_test = int(undersampling_ratio * x_test.size)

# # Randomly select the indices of the frequencies to keep for test data
# undersampled_indices_test = np.random.choice(all_indices, size=num_frequencies_to_keep_test, replace=False)

# # Create a new undersampled Fourier data for test data
# undersampled_fourier_test = np.zeros_like(x_test_fourier)
# undersampled_fourier_test.flat[undersampled_indices_test] = x_test_fourier.flat[undersampled_indices_test]

# # Perform inverse Fourier transform to get the undersampled test data
# x_test_undersampled = np.abs(np.fft.ifft2(undersampled_fourier_test))
# Test the model on the test data
x_test_fourier = np.fft.fft2(x_test)

# Calculate the number of frequencies to keep in the undersampled Fourier data for test data
num_frequencies_to_keep_test = int(undersampling_ratio * x_test.size)

# Get the indices of all frequencies in the Fourier data for test data
all_indices_test = np.arange(x_test.size)

# Randomly select the indices of the frequencies to keep for test data
undersampled_indices_test = np.random.choice(all_indices_test, size=num_frequencies_to_keep_test, replace=False)

# Create a new undersampled Fourier data for test data
undersampled_fourier_test = np.zeros_like(x_test_fourier)
undersampled_fourier_test.flat[undersampled_indices_test] = x_test_fourier.flat[undersampled_indices_test]

# Perform inverse Fourier transform to get the undersampled test data
x_test_undersampled = np.abs(np.fft.ifft2(undersampled_fourier_test))



# Test the model on the test data
reconstructed_images = cnn_model.predict(x_test_undersampled)

# Calculate SNR (Signal-to-Noise Ratio) and RMSE (Root Mean Squared Error)
snr_values = []
rmse_values = []
for i in range(x_test.shape[0]):
    original_image = x_test[i]
    reconstructed_image = reconstructed_images[i]
    
    # Calculate SNR
    snr = calculate_snr(original_image, reconstructed_image)
    snr_values.append(snr)
    
    # Calculate RMSE
    rmse = calculate_rmse(original_image, reconstructed_image)
    rmse_values.append(rmse)

# Print average SNR and RMSE values
avg_snr = np.mean(snr_values)
avg_rmse = np.mean(rmse_values)
print(f'Average SNR: {avg_snr:.2f}')
print(f'Average RMSE: {avg_rmse:.2f}')

# Visualize a few original, undersampled, and reconstructed images
num_visualize = 5
for i in range(num_visualize):
    plt.figure(figsize=(18, 6))
    
    # Visualize the undersampled image
    plt.subplot(1, 3, 1)
    plt.imshow(np.squeeze(x_test_undersampled[i]), cmap='gray')
    plt.title('Input Undersampled Image')
    
    # Visualize the original image
    plt.subplot(1, 3, 2)
    plt.imshow(np.squeeze(np.abs(x_test[i])), cmap='gray')
    plt.title('Original Image (Magnitude)')
    
    # Visualize the reconstructed image
    plt.subplot(1, 3, 3)
    plt.imshow(np.squeeze(np.abs(reconstructed_images[i])), cmap='gray')
    plt.title('Reconstructed Image (Magnitude)')
    
    plt.show()
