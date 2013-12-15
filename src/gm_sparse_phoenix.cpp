#include <iostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <string.h>
#define TIMING
#include "map_reduce.h"
using namespace std;

char *fname;
int num_row;
int num_att;
int num_num;
int num_col;
double shold_dist = 0.15;
double square_shold = shold_dist * shold_dist;
vector<int> attribute;
ofstream foutx("X.txt");
ofstream fouty("Y.txt");

struct stringMeta{
	int id;
	vector<string> vtstr;
	double *vtdou;
	stringMeta(){id = -1;}
	stringMeta(int id, vector<string> vs, double* vd){this->id = id; vtstr = vs; vtdou = vd;}
};

void parse_command_line(int argc, char **argv){
    int c;
    extern char *optarg;
    while((c = getopt(argc, argv, "f:r:a:n:c:e:")) != EOF){
        switch(c){
			case 'f':
                fname = optarg;
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
                printf("Usage: -f <input file> -r <num row> -a <num attribute> -n <num number> -c <num eg> -e <eg>\n");
                exit(1);
        }
    }
}

class YixiuMR : public MapReduce<YixiuMR, stringMeta, int , string>{
	stringMeta* strMeta;
public:
  	YixiuMR(stringMeta* strMeta) : strMeta(strMeta){}
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
				for(int j = 0; j < num_col; j++){
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
    printf("Start gm_sparse_phoenix\n");
	struct timespec begin, end;
	get_time(begin);
	parse_command_line(argc,argv);
	int i,j;
  	string line;
	string tmp;
	int Y[num_row];
	int rownum = 0;
	ifstream fin(fname);
	stringMeta* strMeta = new stringMeta[num_row];
	
	while(getline(fin, line) && rownum < num_row){
		stringstream word(line);
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
		strMeta[rownum] = stringMeta(rownum+1, vstr, vdou);
		word >> tmp;
		if(tmp == "yes")
			Y[rownum] = 1;
		else
			Y[rownum] = 0;
		rownum++;
	}
	get_time(end);
    print_time("Reading input", begin, end);

	get_time(begin);
	fouty << "2\t" << num_row <<endl;
	fouty << "yes\tno" << endl; 
	for(i = 0; i < num_row; i++){
		if(Y[i])
			fouty << "1\t0\n";
		else
			fouty << "0\t1\n";
	}
	get_time(end);
    print_time("Writing Y", begin, end);

	get_time(begin);
	YixiuMR mapReduce(strMeta);
	std::vector<YixiuMR::keyval> result;
	CHECK_ERROR(mapReduce.run(strMeta, num_row, result) < 0);
	get_time(end);
    print_time("Calculating", begin, end);

	get_time(begin);
	for (i = 0; i < num_row; i++){
		foutx << result[i].key << "\t";
		foutx << result[i].val;
    }
	get_time(end);
    print_time("Writing X", begin, end);

	fin.close();
	foutx.close();
	fouty.close();
	for(i = 0; i < num_row; i++){
		vector<string> vTemp;
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;
	printf("Finish gm_sparse_phoenix\n");
    return 0;
}
