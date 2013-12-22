Our algorithm includes two steps:
Step 1: generate matrices
Step 2: matrix multiply

Use the following rule to Name *.cpp:
1. For hybrid model: 
Step 1: gm_sparse_phoenix.cpp
Step 2: mm_sparse_graphchi.cpp
2. For phoenix:
Step 1: gm_dense_phoenix.cpp
Step 2: mm_dense_phoenix.cpp
3. For grapchi:
Step 1: gm_sparse_graphchi.cpp
Step 2: mm_sparse_graphchi.cpp

初始化 Lower = 0 Upper=0
如果一行里有一个是1, Lower ++
cout << "POS: " << Lower << endl; 
cout << "Dependency Degree: " << Lower / 总个数 << endl.


矩阵形式为：matrix X = a X a，matrix Y = a X b

./gm_dense_phoenix -f bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2
输出为X.txt & Y.txt
输入格式为：
-f是输入的文件，-r是该文件的行数，-a是有多少字段属性，-n是字段属性后面有多少数字属性，-c是一共多少字段属性要用，-e是具体要用的字段属性的列号

./gm_sparse_phoenix -f bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2
输出为X.txt & Y.txt
输入格式为：
-f是输入的文件，-r是该文件的行数，-a是有多少字段属性，-n是字段属性后面有多少数字属性，-c是一共多少字段属性要用，-e是具体要用的字段属性的列号

./gm_sparse_graphchi -f bank_10.txt -r 10 -a 11 -n 5 -c 1 -e 2
输出为X.txt & Y.txt
输入格式为：
-f是输入的文件，-r是该文件的行数，-a是有多少字段属性，-n是字段属性后面有多少数字属性，-c是一共多少字段属性要用，-e是具体要用的字段属性的列号

./mm_dense_phoenix matrix_file_A.txt matrix_file_B.txt 10 2 1
目前只允许读取数据，不可生成
输出为Output.txt
输入格式为：
矩阵A，矩阵B，10000是a，2是b，1是矩阵乘法并行的行数

bin/example_apps/mm_sparse_graphchi file1 ~/data/graphchi/demo/demo_x.txt file2 ~/data/graphchi/demo/demo_y.txt
输入X&Y文件
输出为  Lower

