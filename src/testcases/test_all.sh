#!/bin/bash

make clean
make mysh framesize=999 varmemsize=999 
echo
for i in {1..10}; do
    
    # Run the given command and save its output to a temporary file
    mysh T_tc${i}.txt framesize=999 varmemsize=999 > T_tc${i}_output.txt

    # Compare the output with the expected result file
    diff -w -B T_tc${i}_output.txt T_tc${i}_result.txt > T_tc${i}_diff.txt
    echo "here"
    # Check if there are any differences
    if [ -s T_tc${i}_diff.txt ]; then
        echo "There are differences between the output and the expected result for T_tc${i}.txt:"
        cat T_tc${i}_diff.txt
    else
        echo "The output for T_tc${i}.txt matches the expected result."
    fi

    # Clean up temporary files
    rm T_tc${i}_output.txt T_tc${i}_diff.txt
done
