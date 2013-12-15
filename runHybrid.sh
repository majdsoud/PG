export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
DATA=../data

NAME=testHybird
mkdir -p ${NAME}
cd ${NAME}
${BIN}/gm_sparse_phoenix -f ${DATA}/bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2
${BIN}/mm_sparse_graphchi file1 X.txt file2 Y.txt
cd -

