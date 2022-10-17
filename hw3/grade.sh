#!/bin/bash

let score=0
MAX_SCORE=70 # Totally 70 points for basic cases

ANS_PATH=./ans
OUT_PATH=./out

function test() {
    echo -en '[+] '$1'\t'
    cmp --silent $OUT_PATH/$1 $ANS_PATH/$1
    STATUS=$?

    if [ "${EXIT_CODE}" -eq $2 ] && [[ "${STATUS}" -eq 0 ]]; then
        let score=score+$3
        echo $3/$3
    else
        echo 0/$3
    fi
}

# Clean
mkdir -p $OUT_PATH
make clean > /dev/null

# Basic - write1
make write1 > /dev/null
LD_LIBRARY_PATH=. ./write1 > $OUT_PATH/write1
EXIT_CODE=$?
test write1 0 15

# Basic - alarm1
make alarm1 > /dev/null
LD_LIBRARY_PATH=. ./alarm1 > $OUT_PATH/alarm1
EXIT_CODE=$?
test alarm1 142 15

# Basic - alarm2
make alarm2 > /dev/null
LD_LIBRARY_PATH=. ./alarm2 > $OUT_PATH/alarm2
EXIT_CODE=$?
test alarm2 0 15

# Basic - alarm3
make alarm3 > /dev/null
set -m
{ sleep 2 && pkill -SIGINT alarm3; }&
LD_LIBRARY_PATH=. ./alarm3 > $OUT_PATH/alarm3
EXIT_CODE=$?
test alarm3 0 15

# Basic - jmp1
make jmp1 > /dev/null
LD_LIBRARY_PATH=. ./jmp1 > $OUT_PATH/jmp1
EXIT_CODE=$?
test jmp1 0 10


echo -e "Total\t\t"$score/$MAX_SCORE