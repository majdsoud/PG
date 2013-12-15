#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
using namespace std;

char * fnamein;
int num_row;
int num_att;
int num_num;
int num_col;
vector<int> attribute;
double shold_dist = 0.15;
double square_shold = shold_dist * shold_dist;
ofstream foutx("X.txt");
ofstream fouty("Y.txt");

struct stringMeta{
	vector<string> vtstr;
	double *vtdou;
	stringMeta(){}
	stringMeta(vector<string> vs, double *vd){vtstr = vs; vtdou = vd;}
};

void parse_command_line(int argc, char **argv){
    int c;
    extern char *optarg;
    while ((c = getopt(argc, argv, "f:r:a:n:c:e:")) != EOF) {
        switch (c) {
			case 'f':
                fnamein = optarg;
			break;
            case 'r':
                num_row = atoi(optarg);
			break;
            case 'a':
                num_att = atoi(optarg);
                break;
            case 'n':
                num_num = atoi(optarg);
                break;
			case 'c':
                num_col = atoi(optarg);
                break;
			case 'e':{
 				char *ch = optarg;
		   		int len = strlen(optarg);
				int i = 0;
				int start;
				int flag = 0;
				while(i < len){	
					if(i < len && flag == 1){
						++i;	
					}
					flag = 1;
					start = i;			
					while(i < len && *(ch+i) != ':') 
						++i;
					*(ch+i) = 0;
					int ttmp = atoi(optarg + start);
					attribute.push_back(ttmp);
				}
            }break;
			case '?':
                printf("argv error~~\n");
                exit(1);
        }
    }
}


int main(int argc, char *argv[]){
    clock_t start,finish;
    double totaltime;
    start = clock();
    parse_command_line(argc,argv);
    ifstream fin(fnamein);
 
	int t = 0;
	int i,j,k;
	string str;
	string tmp;
	int Y[num_row];
	int rownum = 0;
	stringMeta* strMeta = new stringMeta[num_row];

	while(getline(fin, str) && rownum < num_row){
		stringstream word(str);
		vector<string> vstr;
		j = 0;
		for(i = 0; i < num_att; i++){
		    word >> tmp;
		    if(i == attribute[j]){
				vstr.push_back(tmp);
				j++;
			}
		}
		double* vdou = new double[num_num];
		for(i = 0; i < num_num; i++){
			word >> tmp;
			vdou[i] = atof(tmp.c_str());
		}
		strMeta[rownum] = stringMeta(vstr,vdou);

		word >> tmp;
		if(tmp == "yes")
			Y[rownum]=1;
		else
			Y[rownum]=0;
		
		rownum++;
	}	
    printf("Finish reading\n");

	fouty << "2\t" << num_row <<endl;
	fouty << "yes\tno" << endl; 
	for(i = 0; i < num_row; i++){
		if(Y[i])
			fouty << "1\t0\n";
		else
			fouty << "0\t1\n";
	}
    printf("Finish writing Y\n");

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
		for(j = 0; j < num_row; j++){
			if(i != j){
				int isequal = 1;
				for(k = 0; k < num_col; k++){
					if(strMeta[i].vtstr[k] != strMeta[j].vtstr[k]){
						isequal = 0;
						break;
					}
				}
				if(isequal){
					double cur_dist = 0;
					for(int k = 0; k < num_num; k++){
					    double dist = strMeta[i].vtdou[k] - strMeta[j].vtdou[k];
						cur_dist += dist * dist;
					}
					if(cur_dist <= square_shold){
						equalnum++;
						stringstream stmp2;
						stmp2 << (j + 1);
						tmp1 += "\t";
						tmp1 += stmp2.str();
					}
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

    printf("Finish writing X");

	for(i = 0; i < num_row; i++){
		vector<string> vTemp; 
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;

    fin.close();
    foutx.close();
    fouty.close();

    finish = clock();
    totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("\n此程序的运行时间为: %.3lfs\n", totaltime);

    return 0;
}
