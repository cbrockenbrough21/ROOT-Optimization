#!/bin/bash

# Define the output log file and the CSV file
LOG_FILE="compression_test_results.txt"
CSV_FILE="rdataframewrite_compression_test_results.csv"

# Clear the log file and CSV file if they exist
: > $LOG_FILE
: > $CSV_FILE

# Write the header to the CSV file
echo "Algorithm,Compression Level,AutoFlush,File Size (MB),Write Time (seconds)" > $CSV_FILE

# Define the compression algorithms you want to test
algorithms=(1 2)
compression_levels=(1 5 9)
autoflush_values=(0 1000 2000 3000)  # 0 means AutoFlush is off

# Loop through each combination of compression algorithm, compression level, and AutoFlush
for algo in "${algorithms[@]}"
do
  for level in "${compression_levels[@]}"
  do
    for autoflush in "${autoflush_values[@]}"
    do
      # Print the current test case to log
      echo "Testing: Algorithm $algo, Compression Level $level, AutoFlush $autoflush" | tee -a $LOG_FILE

      # Run the RDataFrameWrite and append both stdout and stderr to the log file
      result=$(./RDataFrameWrite $algo $level $autoflush)

      # Extract file size and write time from the result using simple patterns
      file_size=$(echo "$result" | grep -oP 'FILE_SIZE=\K[0-9.]+')
      write_time=$(echo "$result" | grep -oP 'WRITE_TIME=\K[0-9.]+')

      # If we successfully extracted both values, append them to the CSV
      if [[ -n "$file_size" && -n "$write_time" ]]; then
        echo "$algo,$level,$autoflush,$file_size,$write_time" >> $CSV_FILE
      else
        echo "$algo,$level,$autoflush,ERROR,ERROR" >> $CSV_FILE
      fi

      echo "-----------------------------------------" | tee -a $LOG_FILE
    done
  done
done

echo "Testing completed. Results written to $CSV_FILE and $LOG_FILE"
