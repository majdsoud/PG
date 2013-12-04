#!/bin/sh
#yixiu_phoenix/yixiu_phoenix -f ../data/bank_45211.txt -r 45211 -a 11 -n 5 -c 11 -e 0:1:2:3:4:5:6:7:8:9:10
yixiu_phoenix/yixiu_phoenix -f ../data/input_10000.txt -r 10000 -a 3 -n 0 -c 3 -e 0:1:2:3
translate/translate X.txt Y.txt 10000 2
matrix_multiply/matrix_multiply matrix_file_A.txt matrix_file_B.txt 10000 2 1
