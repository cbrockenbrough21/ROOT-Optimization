#!/bin/bash

# Define the output log file and the CSV file
LOG_FILE="vector_compression_test_results.txt"
CSV_FILE="vector_compression_test_results.csv"

# Clear the log file and CSV file if they exist
: > $LOG_FILE
: > $CSV_FILE

# Write the header to the CSV file
echo "Algorithm,Compression Level,Basket Size,AutoFlush,File Size (MB),Write Time (seconds)" > $CSV_FILE

# Define the compression algorithms, levels, basket sizes, and autoflush values you want to test
algorithms=(1 2)
compression_levels=(1 5 9)
basket_sizes=(64000)
autoflush_values=(2000)  # Add your desired AutoFlush values here

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
        
        # Run the StructWrite and append both stdout and stderr to the log file
        result=$(./VectorWrite $algo $level $basket_size $autoflush)

        # Extract file size and write time from the result using simple patterns
        file_size=$(echo "$result" | grep -oP 'FILE_SIZE=\K[0-9.]+')
        
        write_time=$(echo "$result" | grep -oP 'WRITE_TIME=\K[0-9.]+')

        # If we successfully extracted both values, append them to the CSV
        if [[ -n "$file_size" && -n "$write_time" ]]; then
          echo "$algo,$level,$basket_size,$autoflush,$file_size,$write_time" >> $CSV_FILE
        else
          echo "$algo,$level,$basket_size,$autoflush,ERROR,ERROR" >> $CSV_FILE
        fi
        
        echo "-----------------------------------------" | tee -a $LOG_FILE
      done
    done
  done
done

echo "Testing completed. Results written to $CSV_FILE and $LOG_FILE"
