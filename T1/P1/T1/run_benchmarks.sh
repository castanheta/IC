#!/bin/bash

# Set the output file for time log
output_file="execution_time.txt"

# Path to the input file (adjust as necessary)
input_file="../../assets/Sample.txt"

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

# Step 3: Run the executable and measure execution time in milliseconds
echo "Running the program and measuring time..."
start_time=$(date +%s%3N)  # Capture start time in milliseconds
./text_processor "$input_file"  # Run the executable
end_time=$(date +%s%3N)  # Capture end time in milliseconds

# Calculate elapsed time in milliseconds
elapsed_time=$((end_time - start_time))

# Save the result to the output file
echo "Elapsed Time (ms): $elapsed_time" > "../$output_file"

# Step 4: Notify user of completion
echo "Execution time has been saved to $output_file"

