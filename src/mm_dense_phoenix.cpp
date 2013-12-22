#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "basic.cpp"
#include "map_reduce.h"
using namespace std;

struct mm_data_t {
    int row_num;
    int rows;
    int *matrix_A;
    int *matrix_B;
    int matrix_len;
    int colmun_len;
    double* output;
};

class MatrixMulMR : public MapReduce<MatrixMulMR, mm_data_t, int, int>{
    int *matrix_A, *matrix_B;
    int matrix_size;
    int column_size;
    int row;
    double *output;

public:
    MatrixMulMR(int* _mA, int* _mB, int size, int col, double* out) : 
        matrix_A(_mA), matrix_B(_mB), matrix_size(size),column_size(col), row(0), output(out) {}
        
    void* locate(mm_data_t* d, uint64_t len) const{
        return d->matrix_A + d->row_num * d->matrix_len;
    }

    void map(mm_data_t const& data, map_container& out) const{
        int* a_ptr = data.matrix_A + data.row_num*data.matrix_len + 0;
        int* b_ptr = data.matrix_B + 0;
        
        double* output = data.output + data.row_num*data.colmun_len;
        int num = 0;
        for(int i = 0; i < data.matrix_len; i++) {
            if (a_ptr[i] == 1)  num++;
            for(int j = 0;j < data.colmun_len; j++) {
                output[j] += a_ptr[i] * b_ptr[j];
            }
            b_ptr += data.colmun_len;
        }
        for(int j = 0;j < data.colmun_len ; j++) {
            output[j] /= num;
        }
    }
     

    int split(mm_data_t& out){
        if (row >= matrix_size){
            return 0;
        }

        out.matrix_A = matrix_A;
        out.matrix_B = matrix_B;
        out.matrix_len = matrix_size;
	    out.colmun_len = column_size;
        out.output = output;
        out.rows = 1;
        out.row_num = row++;
        
        return 1;
    }
};

int main(int argc, char *argv[]) {
    StartTimerAll();
    cout << "================== start  mm_dense_phoenix ==================" << endl;
    double tall;
    int i,j;
    int fd_A, fd_B, file_size1,file_size2;
    int * fdata_A, *fdata_B;
    int matrix_len, matrix_collen, row_block_len;
    struct stat finfo_A, finfo_B;
    char *fname_A, *fname_B;
    
    srand( (unsigned)time( NULL ) );
    fname_A = argv[1];
    fname_B = argv[2];
    CHECK_ERROR ( (matrix_len = atoi(argv[3])) < 0);
    CHECK_ERROR ( (matrix_collen = atoi(argv[4])) < 0);
    file_size1 = ((matrix_len*matrix_len))*sizeof(int);
    file_size2 = ((matrix_len*matrix_collen))*sizeof(int);
    if(argv[5] == NULL)
        row_block_len = 1;
    else
        CHECK_ERROR ( (row_block_len = atoi(argv[5])) < 0);

    // Read in the file
    CHECK_ERROR((fd_A = open(fname_A,O_RDONLY)) < 0);
    // Get the file info (for file length)
    CHECK_ERROR(fstat(fd_A, &finfo_A) < 0);
#ifndef NO_MMAP
#ifdef MMAP_POPULATE
    // Memory map the file
    CHECK_ERROR((fdata_A = (int*)mmap(0, finfo_A.st_size + 1, 
        PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd_A, 0)) == NULL);
#else
    // Memory map the file
    CHECK_ERROR((fdata_A = (int*)mmap(0, finfo_A.st_size + 1, 
        PROT_READ, MAP_PRIVATE, fd_A, 0)) == NULL);
#endif
#else
    int ret;

    fdata_A = (char *)malloc (file_size1);
    CHECK_ERROR (fdata_A == NULL);

    ret = read (fd_A, fdata_A, file_size1);
    CHECK_ERROR (ret != file_size1);
#endif

    // Read in the file
    CHECK_ERROR((fd_B = open(fname_B,O_RDONLY)) < 0);
    // Get the file info (for file length)
    CHECK_ERROR(fstat(fd_B, &finfo_B) < 0);
#ifndef NO_MMAP
#ifdef MMAP_POPULATE
    // Memory map the file
    CHECK_ERROR((fdata_B = (int*)mmap(0, finfo_B.st_size + 1, 
        PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd_B, 0)) == NULL);
#else
    // Memory map the file
    CHECK_ERROR((fdata_B = (int*)mmap(0, finfo_B.st_size + 1, 
        PROT_READ, MAP_PRIVATE, fd_B, 0)) == NULL);
#endif
#else
    fdata_B = (char *)malloc (file_size2);
    CHECK_ERROR (fdata_B == NULL);

    ret = read (fd_B, fdata_B, file_size2);
    CHECK_ERROR (ret != file_size2);
#endif

    double *output = new double[matrix_len*matrix_collen];

    printf("MatrixMult: Calling MapReduce Scheduler Matrix Multiplication\n");

    vector<MatrixMulMR::keyval> result;
    MatrixMulMR mapReduce(fdata_A, fdata_B, matrix_len,matrix_collen, output);
    mapReduce.run(result);

    double Lower = 0;
    for(i=0;i<matrix_len;i++){
        for(j=0;j<matrix_collen;j++){
            if (output[i*matrix_collen+j] == 1) Lower++;   
        }
    }

    std::cout << "POS: " << Lower << std::endl;
    std::cout << "Dependency Degree: " << Lower << "/" << matrix_len << "= " <<Lower/matrix_len << std::endl;
    
    delete[] output;

    CHECK_ERROR(munmap(fdata_A, file_size1 + 1) < 0);
    close(fd_A);

    CHECK_ERROR(munmap(fdata_B, file_size2 + 1) < 0);
    close(fd_B);

	tall = GetTimerAll();
	cout << "total time: " << tall << endl;
	cout << "================== finish mm_dense_phoenix ==================" << endl;
    return 0;
}
