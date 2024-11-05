# Audio Processor

C++ program that processes audio files (.wav) using [libsndfile](https://libsndfile.github.io/libsndfile/). It supports retrieving audio information, waveform plotting, histogram of amplitude values for each channel, quantization, and audio quality metrics (MSE and SNR).

## Features

- **Audio Information:** Get basic information about the audio file, such as sample rate, number of channels, and duration.
- **Waveform Visualization:** Generate a waveform plot of the audio signal and export it to a CSV file.
- **Channel Histogram:** Create a histogram of amplitude values for each channel (Left, Right, MID or SIDE) with different bin sizes and export it to a CSV file.
- **Quantize Audio:** Reduce the number of distinct amplitude levels in the audio signal and export the quantized audio to a new file.
- **Calculate Metrics:** Compute Mean Squared Error (MSE) and Signal-to-Noise Ratio (SNR) between the original and processed audio.

## Setup

### Prerequisites

- cmake
- gcc
- make
- libsndfile

### Steps

1. **Clone the repository**

1. **Install dependencies**

On Ubuntu, you can run:

```bash
sudo apt-get update
sudo apt-get install libsndfile-dev build-essential cmake
```

1. **Choose the directory** with the desired task to run:

    - T1: Basic audio information
    - T2: Waveform visualization
    - T3: Channel histogram
    - T4: Quantize audio
    - T5: Calculate metrics

1. **Build the project** using CMake:

   ```bash
   cd build
   cmake ..
   make
   ```

1. **Run the program** by providing the input audio and specific options:

    > T1
    ```bash
    ./audio_processor <input file>
    ```

    > T2
    ```bash
    ./audio_processor <input file> <output file name>
    ```

    > T3
    ```bash
    ./audio_processor <input file> <output file name> <channel> <bin size>
    ```

    > T4 & T5
    ```bash
    ./audio_processor <input file> <output file name> <quantization bits>
    ```

## Notes

- Ensure that the input audio file path is correct.
- When specifying output file names, do not include any extension.
- The channel parameter for the histogram task can be one of the following: `0` for left, `1` for right, `mid`, or `side`.
- The output files will be saved in the same directory as where the program is executed.

## License

This project is licensed under the MIT License.