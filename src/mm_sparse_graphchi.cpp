#include <time.h>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include "basic.cpp"
#include "graphchi_basic_includes.hpp"
using namespace graphchi;

typedef int VertexDataType;
typedef int EdgeDataType;

struct VectorData{
	int rowNum;
    int vectorNum;
    int * adjlistVector;
	double * outputvector;

    VectorData(){
		adjlistVector = NULL;
		outputvector = NULL;
	}

    VectorData(int rn, int vn, int *av, double *ov) : rowNum(rn), vectorNum(vn), adjlistVector(av), outputvector(ov) {
	}
};

VectorData vData;
double Lower = 0;
int Upper=0;
int NumAll;

struct MatrixMultiplyProgram: public GraphChiProgram<VertexDataType, EdgeDataType> {
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &v, graphchi_context &gcontext) {
    	if(gcontext.iteration == 0){
			if(v.vertexid == 0){
			}else{
			    for(int i= 0; i < vData.vectorNum; i++){		    
			        double sum = 0;
			        int num_all = v.num_outedges();
					int flagg = 1;
			        if(vData.adjlistVector[v.vertexid - 1] == i )
			            sum++;
			        for(int j = 0; j < v.num_outedges(); j++){			        
						graphchi_edge<int> *edge = v.outedge(j);
						if(edge->vertex_id() != 0){
						    if(vData.adjlistVector[edge->vertex_id() - 1] == i )
			                    sum++;
			            }else{
    					    flagg = 0;
			            }
					}
					num_all += flagg;
					sum /= num_all;
					vData.outputvector[(v.vertexid - 1)*vData.vectorNum + i] = sum;				
			    }
            }
		}else{
            for(int i=0; i < v.num_inedges(); i++) {}
            for(int i=0; i < v.num_outedges(); i++) {}
            for(int i=0; i < v.num_edges(); i++) {}
        }
    }

    void before_iteration(int iteration, graphchi_context &gcontext) {
    }

    void after_iteration(int iteration, graphchi_context &gcontext) {
        for (int i=0; i<vData.rowNum; i++){
            for (int j=0; j < vData.vectorNum; j++){
            	//std::cout << vData.outputvector[i * vData.vectorNum + j] <<" ";
                if(vData.outputvector[i * vData.vectorNum + j] == 1)  Lower++;
	        }
	        //std::cout << std::endl;
        }
    }

    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }

    void after_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }

};

int main(int argc, const char ** argv) {
    StartTimerAll();
    std::cerr << "================== Start  mm_sparse_grapphchi ==================" << std::endl;
    double tall,tio,tmm;
    graphchi_init(argc, argv);
    metrics m("matrix_multiply");
    std::string fname1 = get_option_string("file1");
    std::string fname2 = get_option_string("file2");
   	int niters =1;
 	bool scheduler = 0;
 	
 	StartTimer();
    int nshards = convert_if_notexists<EdgeDataType>(fname1, get_option_string("nshards", "auto"));
	int rowNum = 0;
	int vectorNum = 0;
	std::string line;
    std::ifstream inputFile(fname2.c_str());
    getline(inputFile, line);
    vectorNum = atof(line.c_str());
    
    getline(inputFile, line);
    rowNum = atof(line.c_str());
    
    int *adjlistData = new int[rowNum];
    double *outputData = new double[rowNum*vectorNum];
    
    int fnum = 0;
    while(getline(inputFile, line)){
         adjlistData[fnum++] = atof(line.c_str());
    }
    tio = GetTimer();
    
    StartTimer();
    vData = VectorData(rowNum, vectorNum, adjlistData, outputData);
    MatrixMultiplyProgram program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(fname1, nshards, scheduler, m);
    engine.run(program, niters);
    tmm = GetTimer();
    
    metrics_report(m);
    
    std::cerr << "POS: " << Lower << std::endl;
    std::cerr << "Dependency Degree: " << Lower << "/" << rowNum << "= " <<Lower/rowNum << std::endl;

    delete[] adjlistData;
    delete[] outputData;
    
    tall = GetTimerAll();
	std::cerr << "io time: " << tio << std::endl;
	std::cerr << "mm time: " << tmm << std::endl;
	std::cerr << "other time: " << tall - tio - tmm << std::endl;
	std::cerr << "total time: " << tall << std::endl;
	std::cerr << "================== Finish mm_sparse_grapphchi ==================" << std::endl;
    
    return 0;
}
