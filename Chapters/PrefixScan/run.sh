#!/usr/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: ./run.sh <th_num> <size>"
else
    rm res.txt

    gcc -o ./seq/sol ./seq/prefscan_seq.c
    gcc -o ./par/sol ./par/prefscan_pthred.c

    touch res.txt

    ./seq/sol $2 >> res.txt
    ./par/sol $1 $2 >> res.txt

    python ./res.py $1

    rm ./par/sol
    rm ./seq/sol
fi