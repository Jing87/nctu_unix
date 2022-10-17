#!/bin/bash

if [[ "$1" != "-d" && -n "$1" ]]; then
        your_program=$1;
else
        your_program="./ans_hw4";
fi

if [[ "$1" == "-d" || "$2" == "-d" ]]; then
        echo -e "[*] Running sample script scripts/guess.txt"
        echo -e "[*] Please input something twice..."
        diff -U 0 -w <(./ans_hw4 -s scripts/guess.txt sample/guess.nopie 2>&1 | grep -v '^\*\*') <(bash -c "$your_program \-s scripts/guess.txt sample/guess.nopie 2>&1 | grep \-v '^\*\*'")| tee /tmp/testcase_guess\_diff
else
        echo -e "[*] Running sample script scripts/guess.txt"
        echo -e "\n===== sample output ======\n"
        echo -e "[*] Please input something...\c"
        ./ans_hw4 -s scripts/guess.txt sample/guess.nopie 2>&1 | grep -v '^\*\*'
        echo -e "\n=====  your output  ======\n"
        echo -e "[*] Please input something...\c"
        bash -c "$your_program \-s scripts/guess.txt sample/guess.nopie 2>&1 | grep \-v '^\*\*'"
        echo -e "\n==========================\n"
fi