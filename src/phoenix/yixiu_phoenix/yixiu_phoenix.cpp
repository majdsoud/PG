#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>

#define TIMING
#include "map_reduce.h"

using namespace std;

int num_row;
int num_att;
int num_num;
int num_col;
double shold_dist = 0.15;
vector<int> attribute;

ofstream foutx("X.txt");
ofstream fouty("Y.txt");

struct stringMeta{
	int id;
	vector<string> vtstr;
	double* vtdou;
	stringMeta(){}
	stringMeta(int id, vector<string> vs, double* vd){this->id=id; vtstr = vs; vtdou = vd;}
};
	
char * fname;

void parse_command_line(int argc, char **argv) 
{
    int c;
    extern char *optarg;
    while ((c = getopt(argc, argv, "f:r:a:n:c:e:")) != EOF) {
        switch (c) {
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
                printf("Usage: -f <input file> -r <num row> -a <num attribute> -n <num number> -c <num eg> -e <eg,like 1:3:10>\n");
                exit(1);
        }
    }
}

class YixiuMR : public MapReduce<YixiuMR, stringMeta, int , string >
{
	stringMeta* strMeta;
public:
  	YixiuMR(stringMeta* strMeta) : strMeta(strMeta){}

    void *locate (data_type *data, uint64_t len) const
    {
        return &data->id;
    }

    void map(data_type& p, map_container& out) const
    {
		string tmp1 = "";
		for(int i = 0; i < num_row; i++){
			if(p.id!=i){
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
						cur_dist +=  (p.vtdou[k] - strMeta[i].vtdou[k]) * (p.vtdou[k] - strMeta[i].vtdou[k]);
					}
					if (shold_dist * shold_dist < cur_dist)
						isequal = 0;
				}
				if(i!=0)
						tmp1 += "\t";
				if(isequal){
					tmp1 += "1";
				}else
					tmp1 += "0";
			}else{
				if(i!=0)
					tmp1 += "\t";
				tmp1 += "1";
			}
	
		}
		tmp1 += "\n";
        emit_intermediate(out, (p.id), tmp1);
	}
};

int main(int argc, char *argv[]) 
{
    printf("\nStart single_phoenix\n\n");
	struct timespec begin, end;
	get_time (begin);
	parse_command_line(argc,argv);
	int i,j;
	stringMeta* strMeta = new stringMeta[num_row];
	ifstream fin(fname);
  	string line;
	string tmp;
	int Y[num_row];
	int rownum = 0;
	while ( getline(fin,line) && rownum < num_row){
		stringstream word(line);
		vector<string> vstr;
		j = 0;
		for(i = 0; i < num_att; i++){
			if(i==attribute[j]){
				word >> tmp;
				vstr.push_back(tmp);
				j++;
			}
		}
		double* vdou = (double *)malloc(sizeof(double) * num_num);
		for(i = 0; i < num_num; i++){
			word >> tmp;
			vdou[i] = atof(tmp.c_str());
		}
		strMeta[rownum]= stringMeta(rownum,vstr,vdou);
		word >> tmp;
		if(tmp == "yes")
			Y[rownum]=1;
		else
			Y[rownum]=0;
		rownum++;
	}
	get_time (end);
    print_time("reading:", begin, end);

/*
	// Output the strMeta
	for(i = 0; i < num_row; i++){
		cout<<strMeta[i].id<<":";
		for(j=0;j<num_col;j++)
			cout<<" "<<strMeta[i].vtstr[j];
		cout<<endl;
	}


	for(i = 0; i < num_row; i++){
		cout<<strMeta[i].id<<":";
		for(j=0;j<num_num;j++)
			cout<<" "<<strMeta[i].vtdou[j];
		cout<<endl;
	}
*/
	get_time (begin);

	for(i = 0; i < num_row; i++){
		if(Y[i])
			fouty << "1\t0\n";
		else
			fouty << "0\t1\n";
	}
	get_time (end);
    print_time("writing Y:", begin, end);
	get_time (begin);
	YixiuMR* mapReduce = new YixiuMR(strMeta);
	std::vector<YixiuMR::keyval> result;
	CHECK_ERROR(mapReduce->run(strMeta,num_row,result) < 0);
	get_time (end);
    print_time("Calculating:", begin, end);
	get_time (begin);


	for (i = 0; i < num_row; i++){
		for(j = 0; j < (int)result.size(); j++){
        	if(i==result[j].key){
				foutx << result[j].val;
				break;
			}
    	}
    }
	get_time (end);
    print_time("writing X:", begin, end);

	fin.clear();
	fin.close();
	foutx.close();
	fouty.close();

	for(i = 0; i < num_row; i++){
		vector<string> vTemp; 
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;
	delete[] mapReduce;
	printf("\nFinish single_phoenix\n\n");
    return 0;
}
