#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <input_image>"
  exit 1
fi

INPUT_IMAGE="$1"
# Extract the image name
IMG_NAME=$(basename "$INPUT_IMAGE" | sed 's/\.[^.]*$//')

RESULTS_DIR="../results"
mkdir -p "$RESULTS_DIR"

# Variables for Gaussian Kernel sizes
GAUSSIAN_BEGIN=3
GAUSSIAN_END=150
GAUSSIAN_STEP=2

# Variables for Quantization Levels
QUANTIZATION_BEGIN=2
QUANTIZATION_END=256
QUANTIZATION_STEP=2

GAUSSIAN_OUTPUT_FILE="$RESULTS_DIR/${IMG_NAME}_gaussian.csv"
QUANTIZATION_OUTPUT_FILE="$RESULTS_DIR/${IMG_NAME}_quantization.csv"

echo "Value,MSE,PSNR,Time(ms)" > "$GAUSSIAN_OUTPUT_FILE"
echo "Processing Gaussian filter values..."
for (( k=GAUSSIAN_BEGIN; k<=GAUSSIAN_END; k+=GAUSSIAN_STEP )); do
  echo "Gaussian Kernel Size: $k"

  # Measure time
  start_time=$(date +%s%N) # nanoseconds
  metrics=$(./image_processor "$INPUT_IMAGE" --apply_gaussian "$k" --print_metrics 2>/dev/null)
  end_time=$(date +%s%N)

  # Calculate elapsed time in milliseconds
  elapsed_time=$(echo "scale=3; ($end_time - $start_time) / 1000000" | bc)

  echo "$k,$metrics,$elapsed_time" >> "$GAUSSIAN_OUTPUT_FILE"
done

echo "Value,MSE,PSNR,Time(ms)" > "$QUANTIZATION_OUTPUT_FILE"
echo "Processing Quantization levels..."
for (( q=QUANTIZATION_BEGIN; q<=QUANTIZATION_END; q+=QUANTIZATION_STEP )); do
  echo "Quantization Levels: $q"

  # Measure time
  start_time=$(date +%s%N) # nanoseconds
  metrics=$(./image_processor "$INPUT_IMAGE" --quantize "$q" --print_metrics 2>/dev/null)
  end_time=$(date +%s%N)

  # Calculate elapsed time in milliseconds
  elapsed_time=$(echo "scale=3; ($end_time - $start_time) / 1000000" | bc)

  echo "$q,$metrics,$elapsed_time" >> "$QUANTIZATION_OUTPUT_FILE"
done

echo "Processing completed. Results saved in $RESULTS_DIR."

