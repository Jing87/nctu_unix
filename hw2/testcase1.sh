#!/bin/bash

if [ -n "$1" ]; then
    your_program=$1;
else
    your_program="./ans_logger";
fi

sample_banner="\n===== sample output ====="
player_banner="\n=====  your output  ====="

echo "===== Testing basic usage. ====="
echo -e "\n[*] Case 1: $ ./logger"
echo -e "$sample_banner"
$HOME/ta/hw2/ans_logger
echo -e "$player_banner"
bash -c "$your_program"

echo -e "\n[*] Case 2: $ ./logger -h"
echo -e "$sample_banner"
$HOME/ta/hw2/ans_logger -h
echo -e "$player_banner"
bash -c "$your_program \-h"

echo -e "\n[*] Case 3: $ ./logger ls"
echo -e "$sample_banner"
$HOME/ta/hw2/ans_logger ls
echo -e "$player_banner"
bash -c "$your_program ls"

echo -e "\n[*] Case 4: $ ./logger ls 2>/dev/null"
echo -e "$sample_banner"
$HOME/ta/hw2/ans_logger ls 2>/dev/null
echo -e "$player_banner"
bash -c "$your_program ls 2>/dev/null"

echo -e "\n[*] Case 5: $ ./logger -o ls_al.txt -- ls -al"
echo -e "$sample_banner"
echo -e "\nstdout: "
$HOME/ta/hw2/ans_logger -o /tmp/ls_al_sample.txt -- ls -al
echo -e "\n$ cat ls_al_sample.txt"
cat /tmp/ls_al_sample.txt
echo -e "$player_banner"
echo -e "\nstdout: "
bash -c "$your_program \-o /tmp/ls_al_player.txt \-\- ls \-al"
echo -e "\n$ cat ls_al_player.txt"
cat /tmp/ls_al_player.txt