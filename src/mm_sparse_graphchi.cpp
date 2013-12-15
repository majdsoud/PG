#include <time.h>
#include <string>
#include <fcntl.h>
#include "graphchi_basic_includes.hpp"
using namespace graphchi;

typedef int VertexDataType;
typedef int EdgeDataType;

struct VectorData{
	int rowNum;
    int vectorNum;
	int *vectorValue;
    int ** adjlistVector;
	double ** outputvector;
	/* Default Construction */
    VectorData(){
		vectorValue = NULL;
		adjlistVector = NULL;
		outputvector = NULL;
	}

	/* 	Copy constructor */
    VectorData(int rn, int vn, int *vv, int **av, double **ov) : rowNum(rn), vectorNum(vn), vectorValue(vv), adjlistVector(av), outputvector(ov) {
	}
};

VectorData vData;
FILE *outputFile;
int Lower = 0;
int Upper=0;
int NumAll;
/* 	Read vector data */
inline void convertVector(std::string file2) {
	FILE * inputFile = fopen(file2.c_str(), "r");
	logstream(LOG_INFO) << "Reading "  << file2 << std::endl;

    /* guarantee inputFile is not NULL */
	if(inputFile == NULL){
		logstream(LOG_FATAL) << "Could not load:" << file2 << " error: " << strerror(errno) << std::endl;
		assert(inputFile == NULL);
	}

    /* Read rowNum and vectorNum in the first line */
	char *firstLine = (char*) malloc(32);
	int rowNum = 0;
	int vectorNum = 0;
	char delims[] = " z\t";
	if(fgets(firstLine, 32, inputFile) != NULL) {
		char * t = strtok(firstLine, delims);
		rowNum = atoi(t);
		t = strtok(NULL,delims);
		vectorNum = atoi(t);
		NumAll = vectorNum;
	}

    /* Initial the vData*/
	int **adjlistData;
	double **outputData;
	adjlistData = (int **)malloc(sizeof(int *)*rowNum);
	for (int i=0; i<rowNum; i++)
		adjlistData[i] = (int *)malloc(sizeof(int)*(vectorNum));

	outputData = (double **)malloc(sizeof(double *)*vectorNum);
	for (int j=0; j<vectorNum; j++)
		outputData[j] = (double *)malloc(sizeof(double)*(rowNum));

	vData = VectorData(rowNum, vectorNum, (int*) calloc(sizeof(int), (rowNum)), (int**) adjlistData, (double**) outputData);

	int maxlen = 100;
	int lineNum = 0;

    /* Read vectorValue in the second line */
	char *secondLine = (char*) malloc(maxlen);
	if(fgets(secondLine, maxlen, inputFile) != NULL) {
		FIXLINE(secondLine);
		char * t = strtok(secondLine, delims);
		vData.vectorValue[lineNum] = atoi(t);
		while((t = strtok(NULL,delims)) != NULL) {
			vData.vectorValue[lineNum++] = atoi(t);
		}
	}
	free(secondLine);

	char * vectorLine = (char*) malloc(maxlen);
    /* Read vectorValue[] and adjlistVector[][] line by line*/
	lineNum = 0;
   	while(fgets(vectorLine, maxlen, inputFile) != NULL) {
		FIXLINE(vectorLine);
		char * t = strtok(vectorLine, delims);
		int colNum = 0;
		vData.adjlistVector[colNum++][lineNum] = atoi(t);
		while((t = strtok(NULL,delims)) != NULL) {
			vData.adjlistVector[colNum++][lineNum] = atoi(t);
		}
        lineNum++;
	}
	free(vectorLine);
}

/**
  * GraphChi programs need to subclass GraphChiProgram<vertex-type, edge-type>
  * class. The main logic is usually in the update function.
  */
struct MatrixMultiplyProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {

	//Vertex update function.
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &v, graphchi_context &gcontext) {

    	if(gcontext.iteration == 0){  // When first iteration, gcontext.iteration=0
			if(v.vertexid == 0){  // Ignore v.vertexid=0
			}else{
				for(int i= 0; i < vData.rowNum; i++){  // read vector line by line
					double sum = 0;
					/* calculate matrix multiply */
					sum += vData.adjlistVector[i][v.vertexid - 1];
					int num_all = v.num_outedges();
					int flagg = 1;
					for (int j = 0; j < v.num_outedges(); j++){
						graphchi_edge<int> *edge = v.outedge(j);
						if(edge->vertex_id() != 0)
							sum += vData.adjlistVector[i][edge->vertex_id() - 1];
					    else
    					    flagg = 0;
					}
					num_all += flagg;
					std::cout<<v.vertexid<<" "<< sum << " "<<num_all<<std::endl;
					
					sum /= num_all;
					vData.outputvector[v.vertexid-1][i] = sum;
                }
            }
		} else {
			/* Do computation */

            /* Loop over in-edges (example) */
            for(int i=0; i < v.num_inedges(); i++) {
                // Do something
            //    value += vertex.inedge(i).get_data();
            }

            /* Loop over out-edges (example) */
            for(int i=0; i < v.num_outedges(); i++) {
                // Do something
                // vertex.outedge(i).set_data(x)
            }

            /* Loop over all edges (ignore direction) */
            for(int i=0; i < v.num_edges(); i++) {
                // vertex.edge(i).get_data()
            }

            // v.set_data(new_value);
        }
    }

    /**
     * Called before an iteration starts.
     */
    void before_iteration(int iteration, graphchi_context &gcontext) {
    }

    /**
     * Called after an iteration has finished.
     */
    void after_iteration(int iteration, graphchi_context &gcontext) {
	/* Write data to output file */
		for (int i=0;i<vData.vectorNum;i++)	{
			for (int j=0;j<vData.rowNum;j++){
			    if(1 == vData.outputvector[i][j])
				    Lower++;
			
				if(0 == j){
					fprintf(outputFile,"%lf", vData.outputvector[i][j]);
				}else{
					fprintf(outputFile,"\t%lf", vData.outputvector[i][j]);
				}
			}
			fprintf(outputFile,"\n");
		}
    }

    /**
     * Called before an execution interval is started.
     */
    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }

    /**
     * Called after an execution interval has finished.
     */
    void after_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }

};

int main(int argc, const char ** argv) {
	/* Define some variables for calculating running time. */
    clock_t start,finish;
    double totaltime;
    start=clock();

    /* GraphChi initialization will read the command line arguments and the configuration file. */
    graphchi_init(argc, argv);

    /* Metrics object for keeping track of performance counters and other information. Currently required. */
    metrics m("matrix_multiply");

    /* Basic arguments for application */
    std::string filename1 = get_option_string("file1");  // file1
    std::string filename2 = get_option_string("file2");  // file2
    //int niters           = get_option_int("niters", 1);  // Number of iterations
   	int niters =1;  // Set default number of iteration = 1
 	bool scheduler = get_option_int("scheduler", 0); // Whether to use selective scheduling, 0 = false

    /* Detect the number of shards or preprocess an input to create them */
    int nshards = convert_if_notexists<EdgeDataType>(filename1, get_option_string("nshards", "auto"));

	convertVector(filename2);  // Read data in file2 to struct

	outputFile = fopen("output.txt", "w");  // Open output text

    /* Run */
    MatrixMultiplyProgram program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename1, nshards, scheduler, m);
    engine.run(program, niters);

    /* Report execution metrics */
    metrics_report(m);
    
std::cout << std::endl;
std::cout << "POS: " << Lower << std::endl;
std::cout << "Dependency Degree: " << Lower/NumAll  << std::endl;

	/* Calculate and output the total runing time, */
    finish=clock();
    totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
    printf("\n此程序的运行时间为: %.3lf\n",totaltime);

	fclose(outputFile);  // Close the output file

    return 0;
}
