#!/bin/bash
#testing length 
#compile
gcc -O0 home_task_1.c -o a_O0
gcc -O1 home_task_1.c -o a_O1
gcc -O2 home_task_1.c -o a_O2
gcc -O3 home_task_1.c -o a_O3
gcc -os home_task_1.c -o a_os
#end of compile
declare -i length=100000
declare -i step=100000
declare -i gallop=100000
while [ $length -lt 16500000 ] 
do
echo "$length"
./a_O0 test_length_O0.csv $length $gallop
./a_O1 test_length_O1.csv $length $gallop
./a_O2 test_length_O2.csv $length $gallop
./a_O3 test_length_O3.csv $length $gallop
./a_os test_length_os.csv $length $gallop
let "length = length + step"
done


