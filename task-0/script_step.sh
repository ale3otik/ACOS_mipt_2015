#!/bin/bash
#testing gallop
declare -i length=100000000
declare -i step=2
declare -i gallop=10
while [ $gallop -lt 100000000 ] 
do
echo "$gallop"
./a_O0 test_gallop_O0.csv $length $gallop gallop
./a_O1 test_gallop_O1.csv $length $gallop gallop
./a_O2 test_gallop_O2.csv $length $gallop gallop
./a_O3 test_gallop_O3.csv $length $gallop gallop
./a_os test_gallop_os.csv $length $gallop gallop
let "gallop = gallop * step"
done
