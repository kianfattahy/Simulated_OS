

# ../mysh < T_tc2.txt
my_list=(0 18 18 21 18 6 18 18 21 18 6)

for i in {1..10}; do
    cd ../
    make clean
    make mysh framesize=${my_list[${i}]} varmemsize=10
    cd testcases
    echo
    # Run the given command and save its output to a temporary file
    ../mysh < T_tc${i}.txt > T_tc${i}_output.txt

    # Compare the output with the expected result file
    diff -w -B T_tc${i}_output.txt T_tc${i}_result.txt > T_tc${i}_diff.txt
    
    # Check if there are any differences
    if [ -s T_tc${i}_diff.txt ]; then
        echo "There are differences between the output and the expected result for T_tc${i}.txt:"
        cat T_tc${i}_diff.txt
    else
        echo "The output for T_tc${i}.txt matches the expected result."
    fi

    # Clean up temporary files
    # rm T_tc${i}_output.txt 
    rm T_tc${i}_diff.txt
    # echo "\n\n\n\n\n\n"
done