
my_list=(0 18 18 21 18 6 18 18 21 18 6)

for i in {9..9}; do
    cd ../
    make clean
    make mysh framesize=${my_list[${i}]} varmemsize=10
    cd testcases
    echo

    ../mysh < T_tc${i}.txt
done