export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
cd test
$BIN/src/phoenix/yixiu input.txt 10000
$BIN/src/graphchi/matrix_multiply file1 X.txt file2 y.txt
cd ..
