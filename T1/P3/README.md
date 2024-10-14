# Image Processor

This C++ image processing application allows you to perform various image transformations and analyses, such as applying filters, quantizing colors, calculating metrics, and exporting histograms. The program supports several command-line options to modify its behavior, and it uses OpenCV for image processing.

## Features

- **Show Original Image**: Display the original image.
- **Convert to Grayscale**: Display the grayscale version of the image.
- **Show RGB Channels**: Display the individual red, green, and blue color channels of the image.
- **Apply Gaussian Filter**: Apply a Gaussian blur filter with a specified kernel size.
- **Quantize Image**: Reduce the number of color levels in the image.
- **Export Histogram**: Export the histogram of the grayscale version of the image to a CSV file.
- **Calculate Metrics**: Print the Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR) between the original and transformed images.
- **Show Differences**: Display the original and transformed images for comparison.

## Setup

### Prerequisites

- cmake
- gcc
- make
- OpenCV

### Steps

1. **Clone the repository** (if applicable) or download the source files.

2. **Install dependencies**

On Ubuntu, you can run:

```bash
sudo apt-get update
sudo apt-get install libopencv-dev build-essential cmake
```

3. **Build the project** using CMake:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Run the program** by providing the input image and desired options:

   ```bash
   ./image_processor <input_image_path> [OPTIONS]
   ```

## Available Options

The following command-line options are available:

- `--show_original`: Display the original input image.
- `--show_grayscale`: Display the grayscale version of the image.
- `--show_rgb`: Display the individual red, green, and blue channels of the image.
- `--apply_gaussian <kernel_size>`: Apply a Gaussian filter to the image. The `kernel_size` must be a positive odd integer.
- `--quantize <levels>`: Quantize the image to the specified number of color levels.
- `--export_histogram <output_file>`: Export the grayscale image's histogram to a CSV file.
- `--print_metrics`: Print the Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR) between the original and transformed images.
- `--show_diff`: Display the original and transformed images side by side for comparison.

### Example Usage

1. **Display the original image:**

   ```bash
   ./image_processor image.jpg --show_original
   ```

2. **Apply a Gaussian filter with a kernel size of 7:**

   ```bash
   ./image_processor image.jpg --apply_gaussian 7
   ```

3. **Quantize the image to 16 levels and export the histogram to `histogram.csv`:**

   ```bash
   ./image_processor image.jpg --quantize 16 --export_histogram histogram.csv
   ```

4. **Calculate MSE and PSNR between the original and filtered images:**

   ```bash
   ./image_processor image.jpg --apply_gaussian 5 --print_metrics
   ```

## Notes

- Ensure that the input image path is correct, or the program will throw an error.
- The program will display images using OpenCV's `imshow` function. Press any key in the displayed window to close it.

## License

This project is licensed under the MIT License.
