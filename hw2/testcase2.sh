#!/bin/bash

if [[ "$1" != "-d" && -n "$1" ]]; then
        your_program=$1;
else
        your_program="./ans_logger";
fi

if [[ "$1" == "-d" || "$2" == "-d" ]]; then
        diff -U 0 -w <($HOME/ta/hw2/ans_logger ./sample 2>&1) <(bash -c "$your_program ./sample" 2>&1)|egrep -v "0x[0-9a-zA-Z]*$|\[[0-9]*\]|#[0-9]*"|tee /tmp/testcase2_diff
else
        echo -e "[*] Running sample..."
        echo -e "\n===== sample output ======\n"
        $HOME/ta/hw2/ans_logger ./sample
        echo -e "\n=====  your output  ======\n"
        $your_program ./sample
fi