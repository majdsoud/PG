#include <iostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <string.h>
#include "basic.cpp"
#include "map_reduce.h"
using namespace std;

char *fname;
int num_row;
int num_col;
int num_att;
int num_num;
int num_y;
double shold_dist = 0.15;
double square_shold = shold_dist * shold_dist;
vector<int> attribute;
vector<string> ylable;
ofstream foutx("x.txt");
ofstream fouty("y.txt");

struct stringMeta{
	int id;
	vector<string> vtstr;
	double *vtdou;
	stringMeta(){
	    id = -1;
    }
	stringMeta(int id, vector<string> vs, double* vd){
	    this->id = id;
	    vtstr = vs;
	    vtdou = vd;
	}
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

class YixiuMR : public MapReduce<YixiuMR, stringMeta, int, string>{
	stringMeta* strMeta;
public:
  	YixiuMR(stringMeta* strMeta): strMeta(strMeta){}
  	
  	void *locate (data_type *data, uint64_t len) const{
        return &data->id;
    }
    
    void map(data_type& p, map_container& out) const{
		string tmp1 = "";
		string tmp2 = "";
		int equalnum = 0;
		for(int i = 0; i < num_row; i++){
			if(p.id != i + 1){
				int isequal = 1;
				for(int j = 0; j < num_att; j++){ 
					if(p.vtstr[j] != strMeta[i].vtstr[j]){
						isequal = 0;
						break;
					}
				}
				if(isequal){  
					double cur_dist = 0;
					for(int k = 0; k < num_num; k++){
						double dist = p.vtdou[k] - strMeta[i].vtdou[k];
						cur_dist += dist * dist;
					}
					if(cur_dist <= square_shold){
						equalnum++;
						stringstream stmp2;
						stmp2 << (i + 1);
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
			stmp3 << equalnum;
			tmp2 += stmp3.str();
			tmp2 += tmp1;
		}
		tmp2 += "\n";
        emit_intermediate(out, p.id, tmp2);
	}
};

int main(int argc, char *argv[]){
    StartTimerAll();
    cout << "================== start  gm_sparse_phoenix ==================" << endl;
    double tall,tio,tgm;
	parse_command_line(argc,argv);
	parse_y_line();
    
    int i,j;
  	string line;
	string tmp;
	int* Y = new int[num_row];
	int rownum = 0;
	ifstream fin(fname);
	stringMeta* strMeta = new stringMeta[num_row];
	StartTimer();
	
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
		strMeta[rownum] = stringMeta(rownum+1, vstr, vdou);
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
    delete[] Y;

    tio = GetTimer();

	StartTimer();
	YixiuMR mapReduce(strMeta);
	std::vector<YixiuMR::keyval> result;
	CHECK_ERROR(mapReduce.run(strMeta, num_row, result) < 0);
	tgm = GetTimer();

    StartTimer();
	for (i = 0; i < num_row; i++){
		foutx << result[i].key << "\t";
		foutx << result[i].val;
    }
    tio += GetTimer();

	fin.close();
	foutx.close();
	fouty.close();
	for(i = 0; i < num_row; i++){
		vector<string> vTemp;
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;
	tall = GetTimerAll();
	cout << "io time: " << tio << endl;
	cout << "gm time: " << tgm << endl;
	cout << "other time: " << tall - tio - tgm << endl;
	cout << "total time: " << tall << endl;
	cout << "================== finish gm_sparse_phoenix ==================" << endl;
    return 0;
}
