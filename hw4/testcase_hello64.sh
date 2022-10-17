#!/bin/bash

if [[ "$1" != "-d" && -n "$1" ]]; then
        your_program=$1;
else
        your_program="./ans_hw4";
fi

for i in {1..4}
do
        if [[ "$1" == "-d" || "$2" == "-d" ]]; then
                echo -e "[*] Running sample script scripts/hello$i..."
                diff -U 0 -w <(./ans_hw4 -s scripts/hello$i.txt 2>&1 | grep -v '^\*\*') <(bash -c "$your_program \-s scripts/hello$i.txt 2>&1 | grep \-v '^\*\*'")| tee /tmp/testcase_hello$i\_diff
                echo -e "\n==========================\n"
        else
                echo -e "[*] Running sample script scripts/hello$i..."
                echo -e "\n===== sample output ======\n"
                ./ans_hw4 -s scripts/hello$i.txt 2>&1 | grep -v '^\*\*'
                echo -e "\n=====  your output  ======\n"
                bash -c "$your_program \-s scripts/hello$i.txt 2>&1 | grep \-v '^\*\*'"
                echo -e "\n==========================\n"
        fi
done%