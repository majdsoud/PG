#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "map_reduce.h"
#define TIMING

int lineAll = 0;

std::ofstream foutx("X.txt");
std::ofstream fouty("Y.txt");

struct stringMeta {
	char* str1;
	char* str2;
	char* str3;
	int id;

	stringMeta() { str1 = NULL; str2 = NULL; str3 = NULL; id  = -1; }
    stringMeta(char* s1,char* s2,char* s3, int id) { this->id = id; this->str1 = s1;this->str2 = s2;this->str3 = s3; }

    //void dump() { printf("#%d:%s\n",id,str1); }
};


class YixiuMR : public MapReduce<YixiuMR, stringMeta, int , std::string >
{
	stringMeta* strMeta;

public:
  	YixiuMR(stringMeta* strMeta) : strMeta(strMeta){}

    void *locate (data_type *data, uint64_t len) const
    {
        return data->str1;
    }

    void map(data_type& p, map_container& out) const
    {
		std::string word1= "";
		std::string word2;
		int n =0;
		//printf("1:%s 2:%s 3:%s\n",p.str1,p.str2,p.str3);
		for(int i = 0; i < lineAll ; i++) 
			if(p.id!=i){
				if (strcmp(p.str1,strMeta[i].str1) == 0)
				{
					if (strcmp(p.str2,strMeta[i].str2) == 0)
					{
						if (strcmp(p.str3,strMeta[i].str3) == 0)
						{
							word1 += "\t";
							std::stringstream stmp1; 
							stmp1 << (i+1);
							std::string tmp1 = stmp1.str();
							n++;
							word1 += tmp1;
						}
					}
				}
			}
		std::stringstream stmp2; 
		stmp2 << (p.id+1);
		std::string tmp2=stmp2.str();
		word2 = tmp2;
		word2 += "\t";
		if(n == 0){
			word2 += "1\t0";
		}else{
			std::stringstream stmp3; 
			stmp3 << n;
			std::string tmp3 = stmp3.str();
			word2 += tmp3;
			word2 += word1;
		}
		word2 += "\n";
		//std::cout<<(p.id+1)<<" "<<word2<<std::endl;
        emit_intermediate(out, (p.id+1), word2);
		//foutx << word2;
    }
};

int main(int argc, char *argv[]) 
{

	struct timespec begin, end;
	get_time (begin);

	char * fname;
	fname = argv[1];
	lineAll = atoi(argv[2]);
	stringMeta* strMeta = new stringMeta[lineAll];
	
	std::ifstream fin(fname);
	char line[100]={};
	std::string x = "";
	std::string y = "";
	std::string z = "";
	std::string yy = "";
	int lineNum = 0;
	char temp1[lineAll][10];
	char temp2[lineAll][10];
	char temp3[lineAll][10];
	int *yyy = (int *)malloc(lineAll*sizeof(int));

	while(fin.getline(line, sizeof(line)) && lineNum <= lineAll)
	{
		std::stringstream word(line);
		word >> x;
		word >> y;
		word >> z;
		word >> yy;
		if(strcmp(yy.c_str(),"y")==0)
			yyy[lineNum] = 1;
		else 
			yyy[lineNum] = 0;
		strcpy( temp1[lineNum], x.c_str());
		strcpy( temp2[lineNum], y.c_str());
		strcpy( temp3[lineNum], z.c_str());
		strMeta[lineNum] = stringMeta(temp1[lineNum],temp2[lineNum],temp3[lineNum],lineNum);
		lineNum++;
	}
	get_time (end);
    print_time("reading:", begin, end);

fouty<<"2\t"<<lineAll<<std::endl;
fouty<<"1\t0"<<std::endl;
for (int i=0;i<lineAll;i++){
	if(yyy[i])
		fouty<<"1\t0\n";
	else
		fouty<<"0\t1\n";
}

	get_time (begin);
	YixiuMR* mapReduce = new YixiuMR(strMeta);
	std::vector<YixiuMR::keyval> result;
	CHECK_ERROR(mapReduce->run(strMeta,lineAll,result) < 0);
	get_time (end);
    print_time("calculating:", begin, end);

    for (int  i = 0; i < lineAll; i++)
    {
		//foutx << result[i].val << "\t";
		foutx << result[i].val;
    }

	fin.clear();
	fin.close();
	free(yyy);
	delete [] strMeta;
	printf("Finish\n");
	
    return 0;
}
