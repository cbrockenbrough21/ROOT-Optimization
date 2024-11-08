#!/bin/bash

# Define the output log file and the CSV file
LOG_FILE="vector_compression_test_results.txt"
CSV_FILE="vector_compression_test_results.csv"

# Clear the log file and CSV file if they exist
: > $LOG_FILE
: > $CSV_FILE

# Write the header to the CSV file
echo "Algorithm,Compression Level,Basket Size,AutoFlush,File Size (MB),Write Time (seconds),Read Time (seconds)" > $CSV_FILE

# Define the compression algorithms, levels, basket sizes, and autoflush values you want to test
algorithms=(1 2)
compression_levels=(1 5 9)
basket_sizes=(64000)
autoflush_values=(2000)  # Add your desired AutoFlush values here

# Function to time the reading of a ROOT file
time_reading() {
  local root_file=$1
  # Run the Python script to time reading (assumes it prints the read time)
  read_time=$(python3 time_reading_script.py "$root_file" | grep -oP 'Read Time: \K[0-9.]+')
  echo "$read_time"
}

# Loop through each combination of compression algorithm, compression level, basket size, and autoflush
for algo in "${algorithms[@]}"
do
  for level in "${compression_levels[@]}"
  do
    for basket_size in "${basket_sizes[@]}"
    do
      for autoflush in "${autoflush_values[@]}"
      do
        # Print the current test case to log
        echo "Testing: Algorithm $algo, Compression Level $level, Basket Size $basket_size, AutoFlush $autoflush" | tee -a $LOG_FILE
        
        # Run the VectorWrite and capture the output
        result=$(./VectorWrite $algo $level $basket_size $autoflush)

        # Extract file size and write time from the result using simple patterns
        file_size=$(echo "$result" | grep -oP 'FILE_SIZE=\K[0-9.]+')
        write_time=$(echo "$result" | grep -oP 'WRITE_TIME=\K[0-9.]+')

        # If we successfully extracted both values, proceed to test reading
        if [[ -n "$file_size" && -n "$write_time" ]]; then
          # Time the reading of the new ROOT file
          read_time=$(time_reading LinVector.root)

          # If reading time is successful, log it; otherwise, mark it as ERROR
          if [[ -n "$read_time" ]]; then
            echo "$algo,$level,$basket_size,$autoflush,$file_size,$write_time,$read_time" >> $CSV_FILE
          else
            echo "$algo,$level,$basket_size,$autoflush,$file_size,$write_time,ERROR" >> $CSV_FILE
          fi
        else
          echo "$algo,$level,$basket_size,$autoflush,ERROR,ERROR,ERROR" >> $CSV_FILE
        fi
        
        echo "-----------------------------------------" | tee -a $LOG_FILE
      done
    done
  done
done

echo "Testing completed. Results written to $CSV_FILE and $LOG_FILE"
