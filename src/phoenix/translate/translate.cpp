#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
using namespace std;
int main(int argc, char *argv[])
{
    printf("\nStart translate\n\n");
    clock_t start,finish;
    double totaltime;
    start = clock();
    int fd_A, fd_B;
    int matrix_len, matrix_collen;
    char const* fname_A, *fname_B;
    fname_A = "matrix_file_A.txt";
    fname_B = "matrix_file_B.txt";

    char * fnamex;
    char * fnamey;
    fnamex = argv[1];
    fnamey = argv[2];

    ifstream finx(fnamex);
    ifstream finy(fnamey);
    
	matrix_len = atoi(argv[3]);
    matrix_collen = atoi(argv[4]);

	int value = 0;
    fd_A = open(fname_A,O_CREAT | O_RDWR,S_IRWXU);
    fd_B = open(fname_B,O_CREAT | O_RDWR,S_IRWXU);
    
    int rownum = 0;
    string tmp;
    string str;
    int i,j,ret;
    cout <<"Translate X.txt"<<endl;
	while(getline(finx, str) && rownum < matrix_len){
		stringstream word(str);
		for(j = 0; j < matrix_len; j++){
			word >> tmp;
			int tmp1 = atoi(tmp.c_str());
			ret = write(fd_A, &tmp1,sizeof(int));
            assert(ret == sizeof(int)); 
		}
		rownum++;
	}
	cout << "Translate Y.txt"<<endl;
	rownum = 0;
    while(getline(finy, str) && rownum < matrix_len){
		stringstream word(str);
		for(j = 0; j < matrix_collen; j++){
			word >> tmp;
			int tmp1 = atoi(tmp.c_str());
			ret = write(fd_B, &tmp1, sizeof(int));
            assert(ret == sizeof(int));
		}
		rownum++;
	}

    finx.close();
    finy.close();
    close(fd_A);
    close(fd_B);

    finish=clock();
    totaltime=(double)(finish - start)/CLOCKS_PER_SEC;
    printf("\n此程序的运行时间为: %.3lfs\n",totaltime);
    printf("\nFinish translate\n\n");
    return 0;
}
