#!/usr/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: ./run.sh <size> <kth_element> <thread_num>"
else
    rm res.txt

    gcc -o ./seq/sol ./seq/selct_sort_seq.c
    gcc -o ./par/sol -fopenmp ./par/select_sort_par.c

    touch res.txt

    ./seq/sol $1 $2 >> res.txt
    ./par/sol $1 $2 $3 >> res.txt

    python ./res.py $3

    rm ./par/sol
    rm ./seq/sol
fi
