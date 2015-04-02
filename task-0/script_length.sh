#!/bin/bash
#testing length

length=10000000
step=10000000
gallop=1000000
while (( $length < 250000001 )) 
do
    echo "$length"

    ./a_O0 new_test_length_O0.csv $length $gallop
    ./a_O1 new_test_length_O1.csv $length $gallop
    ./a_O2 new_test_length_O2.csv $length $gallop
    ./a_O3 new_test_length_O3.csv $length $gallop
    ./a_os new_test_length_os.csv $length $gallop

   length=$((length + step))

done


