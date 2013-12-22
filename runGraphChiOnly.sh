export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
DATA=../data

NAME=testGraphChiOnly
mkdir -p ${NAME}
cd ${NAME}

#${BIN}/gm_sparse_graphchi -f ${DATA}/bank_4521.txt -r 4521 -c 16 -a 8 -n 5 -y 2 -e 0:1:1:1:1:1:1:1:0:1:0:2:2:2:2:2
${BIN}/gm_sparse_graphchi -f ${DATA}/kdd10000.txt -r 10000 -c 41 -a 7 -n 34 -y 23 -e 2:1:1:1:2:2:1:2:2:2:2:1:2:2:2:2:2:2:2:2:1:1:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2

${BIN}/mm_sparse_graphchi file1 x.txt file2 y.txt
cd -

