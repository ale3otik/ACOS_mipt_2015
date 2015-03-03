#!/bin/bash
#union files
echo "   " >> test_length_O0.csv
echo "   " >> test_length_O1.csv
echo "   " >> test_length_O2.csv
echo "   " >> test_length_O3.csv
echo "   " >> test_length_os.csv

echo "   " >> test_gallop_O0.csv 
echo "   " >> test_gallop_O1.csv 
echo "   " >> test_gallop_O2.csv
echo "   " >> test_gallop_O3.csv
echo "   " >> test_gallop_os.csv 

cat test_length_O0.csv test_length_O1.csv test_length_O2.csv test_length_O3.csv test_length_os.csv test_gallop_O0.csv test_gallop_O1.csv test_gallop_O2.csv test_gallop_O3.csv test_gallop_os.csv> tests.csv
