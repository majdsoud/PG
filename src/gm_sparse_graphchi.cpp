#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include "basic.cpp"
using namespace std;

char * fname;
int num_row;
int num_col;
int num_att;
int num_num;
int num_y;
vector<int> attribute;
vector<string> ylable;
double shold_dist = 0.15;
double square_shold = shold_dist * shold_dist;
ofstream foutx("x.txt");
ofstream fouty("y.txt");

struct stringMeta{
	vector<string> vtstr;
	double *vtdou;
	stringMeta(){}
	stringMeta(vector<string> vs, double *vd){vtstr = vs; vtdou = vd;}
};

void parse_command_line(int argc, char **argv){
    int c;
    extern char *optarg;
    while((c = getopt(argc, argv, "f:r:c:a:n:y:e:")) != EOF){
        switch(c){
			case 'f':
                fname = optarg;
			break;
            case 'r':
                num_row = atoi(optarg);
			break;
            case 'c':
                num_col = atoi(optarg);
            break;
            case 'a':
                num_att = atoi(optarg);
            break;
			case 'n':
                num_num = atoi(optarg);
            break;
            case 'y':
                num_y = atoi(optarg);
            break;
			case 'e':{
 				char *ch = optarg;
		   		int len = strlen(optarg);
				int i = 0;
				int start;
				int flag = 0;
				while(i < len){
					if(i < len && flag == 1)
						++i;
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
			cout<<"Usage: -f <input file> -r <row number> -c <col number> -a <attribute number> -n <num number> -y <y number> -e <eg col, 0 means not need, 1 means att,2 means num>"<<endl;
            exit(1);
        }
    }
}

void parse_y_line(){
    string line;
    ifstream fyl("../data/names");
    while(getline(fyl, line)){
        ylable.push_back(line);
    }
}

int main(int argc, char *argv[]){
    StartTimerAll();
    cout << "================== start  gm_sparse_graphchi ==================" << endl;
    double tall;
    parse_command_line(argc,argv);
    parse_y_line();
    ifstream fin(fname);

	int i,j,k;
	string line;
	string tmp;
	int* Y = new int[num_row];
	int rownum = 0;
	stringMeta* strMeta = new stringMeta[num_row];
 
	while(getline(fin, line) && rownum < num_row){
	    stringstream word(line);
		vector<string> vstr;
		double* vdou = new double[num_num];
		for(i = 0, j = 0; i < num_col; i++){
		    word >> tmp;
			if(attribute[i] == 1){
				vstr.push_back(tmp);
			}
			if(attribute[i] == 2){
				vdou[j++] = atof(tmp.c_str());
			}
		}

		strMeta[rownum] = stringMeta(vstr,vdou);
		word >> tmp;
		for(i = 0; i < num_y; i++){
		    if(ylable[i] == tmp){
		        Y[rownum]= i;
		        break;
		    }
		}
		rownum++;
	}	

    fouty<<num_y<<"\n";
	fouty<<num_row<<"\n";
	for(i=0;i<num_row;i++)
        fouty<<Y[i]<<"\n";

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
				for(k = 0; k < num_att; k++){
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

	for(i = 0; i < num_row; i++){
		vector<string> vTemp;
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;
    delete[] Y;
    fin.close();
    foutx.close();
    fouty.close();

	tall = GetTimerAll();
	cout << "total time: " << tall << endl;
	cout << "================== finish gm_sparse_graphchi ==================" << endl;
    return 0;
}
