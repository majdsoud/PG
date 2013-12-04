矩阵形式为：matrix X = a X a，matrix Y = a X b

single_phoenix/single -f data/input_10000.txt -r 10000 -a 3 -n 0 -c 3 -e 0:1:2:3
用phoenix处理数据
输出为X.txt & Y.txt。
输入格式为：
-f是输入的文件，-r是该文件的行数，-a是有多少字段属性，-n是字段属性后面有多少数字属性，-c是一共多少字段属性要用，-e是具体要用的字段属性的列号
用input_10000.txt处理后的X.txt有200MB，Y.txt有40KB

g++ -o tanslate translate.cpp
translate/translate X.txt Y.txt 10000 2
将生成的文本X/Y转换形式，数据大的情况下，该步骤特别慢
输出为matrix_file_A.txt & matrix_file_B.txt
输入格式为：
X文件，Y文件，行数a，列数b
用input_10000.txt处理后的X.txt&Y.txt为输入，处理后的matrix_file_A.txt有400MB，matrix_file_B.txt为80KB

matrix_multiply/matrix_multiply matrix_file_A.txt matrix_file_B.txt 10000 2 1
矩阵乘法，目前只允许读取数据，不可生成。
输出为Output.txt
输入格式为：
矩阵A，矩阵B，10000是a，2是b，1是矩阵乘法并行的行数
输入Output.txt为53.1KB
