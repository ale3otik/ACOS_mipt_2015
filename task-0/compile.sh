#!/bin/bash
#compile
gcc -ansi -pedantic -Wall -O0 home_task_1.c -o a_O0
gcc -ansi -pedantic -Wall -O1 home_task_1.c -o a_O1
gcc -ansi -pedantic -Wall -O2 home_task_1.c -o a_O2
gcc -ansi -pedantic -Wall -O3 home_task_1.c -o a_O3
gcc -ansi -pedantic -Wall -Os home_task_1.c -o a_os
#end of compile
