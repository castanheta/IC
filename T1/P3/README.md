# Image Processor

This is a C++ application that processes images using OpenCV. It supports multiple operations, such as grayscale conversion, Gaussian filtering, quantization, histogram calculation, and image quality metrics (MSE and PSNR).

## Features

- **Load Image:** Open and load an image in color format.
- **Convert to Grayscale:** Convert a color image to grayscale.
- **Apply Gaussian Filter:** Apply a Gaussian blur with a user-specified kernel size.
- **Quantize Image:** Reduce the number of intensity levels in the image.
- **Calculate Metrics:** Compute Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR) between the original and processed image.
- **Generate Histogram:** Create a histogram of grayscale intensities and export it to a CSV file.
- **Display Images:** Show both the original and processed images in a window.

## Setup

### Prerequisites

- cmake
- gcc
- make
- OpenCV

### Steps

1. **Clone the repository**

2. **Install dependencies**

On Ubuntu, you can run:

```bash
sudo apt-get update
sudo apt-get install libopencv-dev build-essential cmake
```

3. **Build the project** using CMake:

   ```bash
   cd build
   cmake ..
   make
   ```

4. **Run the program** by providing the input image and desired options:

   ```bash
   ./image_processor <input_image_path> [OPTIONS]
   ```

## Available Options

- `--set_image <grayscale|red|green|blue|original>`  
  Set the image type to process:  
  - `grayscale`: Convert the image to grayscale.  
  - `red`, `green`, `blue`: Extract the respective color channel.

- `--apply_gaussian <kernel_size>`  
  Apply a Gaussian filter with the specified kernel size (must be an odd integer).

- `--quantize <levels>`  
  Quantize the image to the specified number of levels.

- `--export_histogram <output_file>`  
  Export the histogram of the grayscale image to a CSV file.

- `--print_metrics`  
  Print the MSE and PSNR values between the original and processed image.

- `--show`  
  Display both the original and processed images.

### Example

```bash
./image_processor image.jpg --set_image grayscale --apply_gaussian 5 --quantize 4 --export_histogram histogram.csv --print_metrics --show
```

This command performs the following steps:
- Converts the image to grayscale.
- Applies a Gaussian filter with a 5x5 kernel.
- Quantizes the image to 4 levels.
- Exports the grayscale image histogram to `histogram.csv`.
- Prints the MSE and PSNR values.
- Displays the original and processed images.

## Output

- **Histogram CSV:** If you specify the `--export_histogram` option, the application will generate a CSV file where each line contains the intensity value and its corresponding frequency.

- **Metrics:** If the `--print_metrics` option is used, the application prints the Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR) between the original and processed images.

## Notes

- Ensure that the input image path is correct, or the program will throw an error.
- The program will display images using OpenCV's `imshow` function. Press any key in the displayed window to close it.

## License

This project is licensed under the MIT License.
