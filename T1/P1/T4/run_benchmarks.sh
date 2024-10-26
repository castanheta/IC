#!/bin/bash

# Set the output file for time log
output_file="execution_time.txt"

# Path to the input file (adjust as necessary)
input_file="../../assets/Sample1.txt"

# Step 1: Build the project
mkdir -p build
cd build
cmake ..
make

# Step 2: Check if the input file exists
if [[ ! -f "$input_file" ]]; then
    echo "Error: Input file $input_file not found!" | tee "../$output_file"
    exit 1
fi

# Step 3: Run the executable and measure execution time
echo "Running the program and measuring time..."
/usr/bin/time -o "../$output_file" -f "Elapsed Time: %E" ./text_processor "$input_file"

# Step 4: Notify user of completion
echo "Execution time has been saved to $output_file"
