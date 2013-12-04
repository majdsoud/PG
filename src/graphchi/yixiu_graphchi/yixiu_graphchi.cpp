#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <time.h>

#define num_row 10000
#define num_col 4
#define num_y 2
using namespace std;

struct stringMeta{
	vector<string> vtstr;
	stringMeta(){}
	stringMeta(vector<string> vs){vtstr = vs;}
};

int main(int argc, char *argv[])
{
    clock_t start,finish;
    double totaltime;
    start=clock();

    ifstream fin("../../data/input_10000.txt");
    ofstream foutx("/home/xiuwenyi/singleMachine/X.txt");
    ofstream fouty("/home/xiuwenyi/singleMachine/Y.txt");
	int t = 0;
	int i,j,k;
	string str;
	string tmp;
	stringMeta* strMeta = new stringMeta[num_row];

	int Y[num_row];


	int rownum = 0;
	while(getline(fin, str) && rownum < num_row){
		stringstream word(str);
		vector<string> vstr;
		for(j = 0; j < num_col - 1; j++){
			word >> tmp;
			vstr.push_back(tmp);
		}
		strMeta[rownum]= stringMeta(vstr);

		word >> tmp;

		if(tmp == "yes")
			Y[rownum]=1;
		else
			Y[rownum]=0;

		rownum++;
	}
printf("finish reading");

	fouty << num_y << "\t" << num_row <<endl;
	fouty << "yes\tno" << endl; 
	for(i = 0; i < num_row; i++){
		if(Y[i])
			fouty << "1\t0\n";
		else
			fouty << "0\t1\n";
	}
printf("finish writing Y");
	string tmp1;
	string tmp2;
	int equalnum;
	for(i = 0; i < num_row; i++){
		tmp1 = "";
		stringstream stmp1; 
		stmp1 << (i+1);
		tmp2 = stmp1.str();
		tmp2 += "\t";
		equalnum = 0;
		for(k = 0; k < num_row; k++){
			if(i!=k){
				int isequal = 1;
				for(j = 0; j < num_col-1; j++){
					if(strMeta[i].vtstr[j] != strMeta[k].vtstr[j]){
						isequal = 0;
						break;
					}
				}
				if(isequal){
					equalnum++;
					stringstream stmp2; 
					stmp2 << (k+1);
					tmp1 += "\t";					
					tmp1 += stmp2.str();
				}
			}
		}
		if(equalnum == 0)
			tmp2 += "1\t0";
		else{
			stringstream stmp3; 
			stmp3 << (equalnum);
			tmp2 += stmp3.str();
			tmp2 += tmp1;
		}
		foutx << tmp2 <<endl;
	}
printf("finish writing X");
	for(i = 0; i < num_row; i++){
		vector<string> vTemp; 
		vTemp.swap(strMeta[i].vtstr);
	}
	delete[] strMeta;

    fin.close();
    foutx.close();
    fouty.close();

    finish=clock();
    totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
    printf("\n此程序的运行时间为: %.3lfs\n",totaltime);

    return 0;
}
