export GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
cd test4
$BIN/src/phoenix/yixiu input.txt 100000
$BIN/src/graphchi/matrix_multiply file1 X.txt file2 Y.txt
cd ..
