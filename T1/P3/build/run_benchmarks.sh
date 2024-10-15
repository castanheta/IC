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

echo "Value,MSE,PSNR" > "$GAUSSIAN_OUTPUT_FILE"
echo "Processing Gaussian filter values..."
for (( k=GAUSSIAN_BEGIN; k<=GAUSSIAN_END; k+=GAUSSIAN_STEP )); do
  echo "Gaussian Kernel Size: $k"
  metrics=$(./image_processor "$INPUT_IMAGE" --apply_gaussian "$k" --set_image grayscale --print_metrics 2>/dev/null)
  echo "$k,$metrics" >> "$GAUSSIAN_OUTPUT_FILE"
done

echo "Value,MSE,PSNR" > "$QUANTIZATION_OUTPUT_FILE"
echo "Processing Quantization levels..."
for (( q=QUANTIZATION_BEGIN; q<=QUANTIZATION_END; q+=QUANTIZATION_STEP )); do
  echo "Quantization Levels: $q"
  metrics=$(./image_processor "$INPUT_IMAGE" --quantize "$q" --set_image grayscale --print_metrics 2>/dev/null)
  echo "$q,$metrics" >> "$QUANTIZATION_OUTPUT_FILE"
done

echo "Processing completed. Results saved in $RESULTS_DIR."

