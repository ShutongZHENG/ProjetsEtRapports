// 包含 MPI 头文件
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 初始化矩阵为随机数
void initMatrix(int *m, int n)
{
    // 初始化矩阵为随机数
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            m[i * n + j] = rand() % 100;
        }
    }
}

// 矩阵相加
void addition(int *A, int *B, int *C, int N, int cols)
{
    // 矩阵相加
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            C[i * cols + j] = A[i * cols + j] + B[i * cols + j];
        }
    }
}

int main(int argc, char *argv[])
{
     // R^N 方阵
    int rank, size, N, NL;
    int *A;
    int *B;
    int *C;
    int *CC;
    int *AL, *BL, *TEMP;

    MPI_Datatype type_temp, type_slice;
    MPI_Aint lbound, displacement;
    int int_size;

    MPI_Init(&argc, &argv);    // 初始化 MPI 环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // 获取当前进程的 rank（进程编号）
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // 获取总的进程数

    if (rank == 0)
    {
         // 读取矩阵大小 N
        FILE *fichier = fopen("matrix_N.data", "r");
        fscanf(fichier, "%d", &N);
        fclose(fichier);
    }

     // 广播 N 给每个进程
    MPI_Bcast(&N, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

   // 将 R^N 方阵划分为多个进程的子矩阵
    if ((N % size) == 0)
        NL = N / size;
    else
    {
        fprintf(stderr, "N is not divisible by Nprocs:%d\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0)
    {
        // 初始化矩阵 A 和 B
        A = (int *)malloc(N * N * sizeof(int));
        B = (int *)malloc(N * N * sizeof(int));
        C = (int *)malloc(N * N * sizeof(int));
        initMatrix(A, N);
        initMatrix(B, N);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d\t", A[N * i + j]);
            }
            printf("\n");
        }

        printf("\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d\t", B[N * i + j]);
            }
            printf("\n");
        }
        CC = (int *)malloc(N * N * sizeof(int));
        addition(A, B, CC, N, N);

        printf("\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d\t", CC[N * i + j]);
            }
            printf("\n");
        }
    }

    MPI_Type_vector(N, NL, N, MPI_INT, &type_temp);    // 创建自定义数据类型 type_temp

    MPI_Type_size(MPI_INT, &int_size);    // 获取 MPI_INT 类型的大小

    lbound = 0;
    displacement = int_size * NL;

    MPI_Type_create_resized(type_temp, lbound, displacement, &type_slice);    // 创建调整大小后的自定义数据类型 type_slice
    MPI_Type_commit(&type_slice);    // 提交自定义数据类型 type_slice

    AL = (int *)malloc(NL * N * sizeof(int));    // 分配存储空间
    BL = (int *)malloc(NL * N * sizeof(int));    // 分配存储空间
    TEMP = (int *)malloc(NL * N * sizeof(int));    // 分配存储空间

    /* Processus 0 distribute in AL the horizontal slices of A */
    MPI_Scatter(A, 1, type_slice, AL, N * NL, MPI_INT, 0, MPI_COMM_WORLD);    // 进程 0 将矩阵 A 的水平切片分发给 AL

    /* Processus 0 distribute in BL the horizontal slices of B */
    MPI_Scatter(B, 1, type_slice, BL, N * NL, MPI_INT, 0, MPI_COMM_WORLD);    // 进程 0 将矩阵 B 的水平切片分发给 BL

    addition(AL, BL, TEMP, N, NL);    // 子矩阵相加

    MPI_Gather(TEMP, NL * N, MPI_INT, C, 1, type_slice, 0, MPI_COMM_WORLD);    // 将子矩阵收集到 C 中

    /* Verification of the results */
    if (rank == 0)
    {
        int Emax = 0;

        printf("\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d\t", C[N * i + j]);
            }
            printf("\n");
        }
        for (int iter = 0; iter < N * N; iter++)
        {
            if (Emax < abs(C[iter] - CC[iter]))
            {
                Emax = abs(C[iter] - CC[iter]);
            }
        }

        free(A);
        free(B);
        free(C);
        free(CC);
        if (Emax == 0)
            printf("Super !\nMatrix product A+B in parallel\n"
                   "equal the sequential one\n");
        else
            printf("False result !\nMatrix product A+B in parallel\n"
                   "different from the sequential one\n");
    }

    free(AL);    // 释放内存
    free(BL);    // 释放内存
    free(TEMP);    // 释放内存

    MPI_Finalize();    // 终止 MPI 环境

    return 0;
}
