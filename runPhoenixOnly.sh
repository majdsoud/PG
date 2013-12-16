export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
DATA=../data

NAME=testPhoenix
mkdir -p ${NAME}
cd ${NAME}
${BIN}/gm_dense_phoenix -f ${DATA}/bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2 1>ret.out 2>ret.time
${BIN}/mm_dense_phoenix matrix_file_A.txt  matrix_file_B.txt 10 2 1 1>>ret.out 2>>ret.time
cd -

