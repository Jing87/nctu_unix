#!/bin/bash

diff -uw <($1 -t SOCK) <(bash -c "$2 -t SOCK") | egrep -v '^.(testcase|ans_hw1|hw1|grep)' 

diff -uw <($1 -c sh) <(bash -c "$2 -c sh") | egrep -v '^.(testcase|ans_hw1|hw1|grep)'

diff -uw <($1 -f dev) <(bash -c "$2 -f dev") | egrep -v '^.(testcase|ans_hw1|hw1|grep)'