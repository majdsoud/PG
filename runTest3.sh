GRAPHCHI_ROOT=../engine/graphchi-cpp
BIN=../bin
cd test
$BIN/src/phoenix/yixiu Skin_NonSkin.txt 60000
$BIN/src/graphchi/matrix_multiply file1 X.txt file2 y.txt
cd ..
