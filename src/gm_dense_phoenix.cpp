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

struct stringMeta{
	int id;
	vector<string> vtstr;
	double* vtdou;
	stringMeta(){}
	stringMeta(int id, vector<string> vs, double* vd){this->id=id; vtstr = vs; vtdou = vd;}
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

class YixiuMR : public MapReduce<YixiuMR, stringMeta, int , string >{
	stringMeta* strMeta;
public:
  	YixiuMR(stringMeta* strMeta) : strMeta(strMeta){}
    void *locate (data_type *data, uint64_t len) const{
        return &data->id;
    }
    void map(data_type& p, map_container& out) const{
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
					    double dist = p.vtdou[k] - strMeta[i].vtdou[k];
						cur_dist += dist * dist;
					}
					if(cur_dist > square_shold){
						isequal = 0;
				    }
			    }
				if(i)
				    tmp1 += "\t";
				if(isequal)
				    tmp1 += "1";
				else
				    tmp1 += "0";
		    }else{
			    if(i)
				    tmp1 += "\t";
			    tmp1 += "1";
			}
        }
        emit_intermediate(out, (p.id), tmp1);
    }
};

int main(int argc, char *argv[]) {
    printf("Start gm_dense_phoenix\n");
	struct timespec begin, end;
	get_time(begin);
	parse_command_line(argc,argv);
	int i, j, k;
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
		strMeta[rownum] = stringMeta(rownum, vstr, vdou);
		word >> tmp;
		if(tmp == "yes")
			Y[rownum]=1;
		else
			Y[rownum]=0;
		rownum++;
	}
	get_time(end);
    print_time("Reading:", begin, end);
        
    get_time(begin);
	YixiuMR mapReduce(strMeta);
	std::vector<YixiuMR::keyval> result;
	CHECK_ERROR(mapReduce.run(strMeta, num_row, result) < 0);
	get_time(end);
    print_time("Calculating", begin, end);
    
	get_time(begin);
	char const* fname_A, *fname_B; 
	fname_A = "matrix_file_A.txt";
    fname_B = "matrix_file_B.txt";
    int fd_A, fd_B, ret;
    
    fd_A = open(fname_A,O_CREAT | O_RDWR,S_IRWXU);
    fd_B = open(fname_B,O_CREAT | O_RDWR,S_IRWXU);
	
	
	int value1 = 1;
	int value2 = 0;
	for(i = 0; i < num_row; i++){
		if(Y[i]){
		    ret = write(fd_B, &value1,sizeof(int));
            assert(ret == sizeof(int));

		    ret = write(fd_B, &value2,sizeof(int));
            assert(ret == sizeof(int));
		}else{
		    ret = write(fd_B, &value2,sizeof(int));
            assert(ret == sizeof(int));
		    
		    ret = write(fd_B, &value1,sizeof(int));
            assert(ret == sizeof(int));
	    }
	}
	get_time(end);
    print_time("Writing Y:", begin, end);
    
    get_time(begin);
	for (i = 0; i < num_row; i++){
		for(j = 0; j < num_row; j++)
        	if(i == result[j].key){
			    stringstream word(result[j].val);
        	    for(k = 0; k < num_row; k++){
        	        word >> tmp;
            	    int tmp1 = atoi(tmp.c_str());
			        ret = write(fd_A, &tmp1,sizeof(int));
                    assert(ret == sizeof(int));
			    }
				break;
		    }
    }
	get_time(end);
    print_time("Writing X:", begin, end);

	fin.clear();
	fin.close();
	close(fd_A);
    close(fd_B);
	
	for(i = 0; i < num_row; i++){
		vector<string> vTemp;
		vTemp.swap(strMeta[i].vtstr);
		delete[] strMeta[i].vtdou;
	}
	delete[] strMeta;

	printf("Finish gm_dense_phoenix\n");
    return 0;
}
