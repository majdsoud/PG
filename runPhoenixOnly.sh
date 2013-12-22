export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
DATA=../data

NAME=testPhoenix
mkdir -p ${NAME}
cd ${NAME}

#${BIN}/gm_dense_phoenix -f ${DATA}/bank_4521.txt -r 4521 -c 16 -a 8 -n 5 -y 2 -e 0:1:1:1:1:1:1:1:0:1:0:2:2:2:2:2
${BIN}/gm_dense_phoenix -f ${DATA}/kdd10000.txt -r 10000 -c 41 -a 7 -n 34 -y 23 -e 2:1:1:1:2:2:1:2:2:2:2:1:2:2:2:2:2:2:2:2:1:1:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2:2 1>ret.out 2>ret.time


#${BIN}/mm_dense_phoenix x.txt y.txt 4521 2 1
${BIN}/mm_dense_phoenix x.txt y.txt 10000 23 1 1>>ret.out 2>>ret.time

cd -

