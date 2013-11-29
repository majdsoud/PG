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
std::ofstream fout("X.txt");

struct stringMeta {
	char* str;
	int id;

	stringMeta() { str = NULL; id  = -1; }
    stringMeta(char* s, int id) { this->id = id; this->str = s; }

    void dump() { printf("#%d:%s\n",id,str); }
};


class YixiuMR : public MapReduce<YixiuMR, stringMeta, int , std::string >
{
	stringMeta* strMeta;

public:
  	YixiuMR(stringMeta* strMeta) : strMeta(strMeta){}

    void *locate (data_type *data, uint64_t len) const
    {
        return data->str;
    }

    void map(data_type& p, map_container& out) const
    {
		std::string word1= "";
		std::string word2;
		int n =0;
		for(int i = 0; i < lineAll ; i++) 
			if(p.id!=i)
				if (strcmp(p.str,strMeta[i].str) == 0){
					word1 += "\t";
					std::stringstream stmp1; 
					stmp1 << (i+1);
					std::string tmp1 = stmp1.str();
					n++;
					word1 += tmp1;
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
		//fout << word2;
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
	//std::string x = "";
	//std::string y = "";
	//std::string z = "";
	int lineNum = 0;
	char tmp[lineAll][10];
	while(fin.getline(line, sizeof(line)))
	{
		//	std::stringstream word(line);
		//	word >> x;
		//	word >> y;
		//	word >> z;
		//tmp = x.c_str();
		//tmp[lineNum] = tmpp.c_str();typedef std::string wc_word;
		std::string tmpp;
		tmpp= line;
		strcpy( tmp[lineNum], tmpp.c_str());
		strMeta[lineNum] = stringMeta(tmp[lineNum],lineNum);
		lineNum++;

	}
	get_time (end);
    print_time("reading:", begin, end);

	get_time (begin);
	YixiuMR* mapReduce = new YixiuMR(strMeta);
	std::vector<YixiuMR::keyval> result;      
	CHECK_ERROR(mapReduce->run(strMeta,lineAll,result) < 0);
	get_time (end);
    print_time("calculating:", begin, end);


    for (int  i = 0; i < lineAll; i++)
    {
		//fout << result[i].val << "\t";
		fout << result[i].val;  
    }

	fin.clear();
	fin.close();

	printf("Finish\n");
	
    return 0;
}
