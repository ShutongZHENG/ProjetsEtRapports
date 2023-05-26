#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef USE_MKL
#include <mkl.h>
#endif

int tag=1000;

/* 分配矩阵并初始化为零 */
void matrix_allocate(double **mat, int linenbr, int colnbr) {
  int iter;
  (*mat) = (double *) malloc(linenbr*colnbr*sizeof(double));
  for (iter=0; iter<linenbr*colnbr; iter++) {
    (*mat)[iter] = 0.; }
}

/* 用随机数初始化矩阵 */
void random_number(double *mat,int n) {
  int iterl,iterc;
  for(iterl=0; iterl<n; iterl++)
    for(iterc=0;iterc<n; iterc++)
      mat[iterl*n+iterc] =  rand() / (RAND_MAX + 1.);
}

/* 矩阵乘法 C = A*B */
void matmul(double *A, double *B, double *C,int nl, int nc, int nk) {
  int iterl,iterc,iterk;
  double sum;

#ifdef USE_MKL
  double alpha,beta;
  alpha = 1.0;
  beta = 0.0;
  mkl_set_num_threads(1);
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              nl, nc, nk, alpha, A, nk, B, nc, beta, C, nc);
#else
  for(iterl=0;iterl<nl;iterl++) {
    for(iterc=0;iterc<nc;iterc++) {
      sum = 0;
      for(iterk=0;iterk<nk;iterk++) {
        sum += A[iterl*nk+iterk]*B[iterk*nc+iterc]; }
      C[iterl*nc+iterc] = sum;
    }
  }
#endif
}

int main(int argc, char *argv[]) {
  int rank,Nprocs,N,NL;
  double *A,*B,*C,*CC;
  double *AL,*BL,*CL,*TEMP;
  MPI_Datatype type_temp,type_slice;
  int double_size;
  MPI_Aint lbound,displacement;
  int iter;
  MPI_Status status;
  int previous_rank,following_rank;

  double time_begin, time_end;


  /* 初始化 MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &Nprocs);

  if (rank == 0) {
    FILE * fichier = fopen("matrix_products.data","r");
    fscanf(fichier, "%d", &N);
    fclose(fichier);


  }

  /* 进程 0 广播 N */
  MPI_Bcast(&N,1,MPI_INTEGER,0,MPI_COMM_WORLD);

  /* N 必须能被 Nprocs 整除 */
  if ( (N % Nprocs) == 0)
    NL = N/Nprocs;
  else {
    fprintf(stderr, "N is not divisible by Nprocs\n");
    /* 程序终止 */
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  /* 进程 0 初始化矩阵 A 和 B */
  if (rank ==0) {
    /* 动态分配矩阵 A、B 和 C */
      time_begin = MPI_Wtime();
    matrix_allocate(&A,N,N);matrix_allocate(&B,N,N);
    matrix_allocate(&C,N,N);matrix_allocate(&CC,N,N);

    /* 初始化 A 和 B */
    random_number(A,N);
    random_number(B,N);

    /* 顺序计算 A*B */
    matmul(A,B,CC,N,N,N);
  }

  /* 分配本地矩阵的空间 */
  matrix_allocate(&AL,NL,N);matrix_allocate(&BL,N,NL);
  matrix_allocate(&CL,N,NL);matrix_allocate(&TEMP,NL,N);

  /* 为 N 行 * NL 列的 1 块数据构建数据类型 */
  MPI_Type_vector(N,NL,N,MPI_DOUBLE,&type_temp);
  MPI_Type_size(MPI_DOUBLE,&double_size);
  
  lbound = 0;
  displacement = double_size*NL;
  
  MPI_Type_create_resized(type_temp,lbound,displacement,
                          &type_slice);
  MPI_Type_commit(&type_slice);

  /* 进程 0 将矩阵 A 的水平切片分发给 AL */
  MPI_Scatter(A, N*NL, MPI_DOUBLE, AL, N*NL, MPI_DOUBLE,
              0, MPI_COMM_WORLD);

  /* 进程 0 将矩阵 B 的垂直切片分发给 BL */
  MPI_Scatter(B,1,type_slice,BL,N*NL,MPI_DOUBLE,
              0,MPI_COMM_WORLD);

  /* 计算对角块 */
  matmul(AL,BL,&(CL[rank*NL*NL]),NL,NL,N);

  /* 计算非对角块 */

  previous_rank = (Nprocs+rank-1)%Nprocs;
  following_rank = (rank+1)%Nprocs;
  for (iter=1; iter<Nprocs; iter++) {
    // 每个进程将它的 AL 切片发送给它的前一个进程
    // 并接收下一个进程的 AL 切片（AL 的值已经改变）
    MPI_Sendrecv_replace(AL,NL*N,MPI_DOUBLE,previous_rank,tag,
                         following_rank,tag,MPI_COMM_WORLD,&status);
    // 每个进程计算它上面或下面的对角块
    matmul(AL,BL,&(CL[( (rank+iter)%Nprocs )*NL*NL]),NL,NL,N);
  }

  /* 进程 0 将每个进程的 CL 切片收集起来形成矩阵 C */
  MPI_Gather(CL,NL*N,MPI_DOUBLE,C,1,type_slice,0,MPI_COMM_WORLD);

  /* 释放本地数组的空间 */
  free(AL); free(BL); free(CL); free(TEMP);

  /* 验证结果 */
  if (rank == 0) {
    time_end = MPI_Wtime();



    double Emax=0;
    for(iter=0; iter<N*N; iter++) {
      if (Emax < fabs(C[iter]-CC[iter])) {
        Emax = fabs(C[iter]-CC[iter]); }}
    free(A); free(B); free(C); free(CC);

    if (Emax < 1e-10)
      printf("Super !\nMatrix product A*B in parallel\n"
             "equal the sequential one\n");
    else
      printf("False result !\nMatrix product A*B in parallel\n"
             "different from the sequential one\n");
     
 
    printf("Rank: %d \n time :%f \n bandwidth %7.2f MB/s.\n",rank,(time_end - time_begin),  
       Nprocs* 2.* (N*NL + N*N)* 8 / 1000000. / (time_end - time_begin)*Nprocs); 

      
  }

  MPI_Type_free(&type_temp);
  MPI_Type_free(&type_slice);
  MPI_Finalize();
  return 0;
}
