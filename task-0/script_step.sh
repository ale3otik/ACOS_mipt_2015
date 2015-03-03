#!/bin/bash
#testing gallop
#compile
gcc -O0 home_task_1.c -o a_O0
gcc -O1 home_task_1.c -o a_O1
gcc -O2 home_task_1.c -o a_O2
gcc -O3 home_task_1.c -o a_O3
gcc -os home_task_1.c -o a_os
#end of compile
declare -i length=16000000
declare -i step=100000
declare -i gallop=100000
while [ $gallop -lt 9000000 ] 
do
echo "$gallop"
./a_O0 test_gallop_O0.csv $length $gallop
./a_O1 test_gallop_O1.csv $length $gallop
./a_O2 test_gallop_O2.csv $length $gallop
./a_O3 test_gallop_O3.csv $length $gallop
./a_os test_gallop_os.csv $length $gallop
let "gallop = gallop + step"
done
