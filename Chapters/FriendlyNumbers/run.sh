#!/usr/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: ./run.sh <th_num> <start> <end>"
else
    rm res.txt

    gcc -o ./seq/sol ./seq/friendly_nums.c
    #gcc -o ./par/sol ./par/reduce_par.c
    gcc -o ./par/sol ./par/friendly_nums_par.c -fopenmp

    touch res.txt

    ./seq/sol $2 $3 >> res.txt
    ./par/sol $2 $3 $1 >> res.txt

    python ./res.py $1

    rm ./par/sol
    rm ./seq/sol
fi
