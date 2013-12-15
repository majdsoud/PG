export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
DATA=../data

NAME=testPhoenix
make -p ${NAME}
cd ${NAME}
${BIN}/gm_dense_phoenix -f ${DATA}/bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2
${BIN}/mm_dense_phoenix X.txt Y.txt 10 2 1
cd -

