#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 用随机数初始化矩阵

void initMatrix(int *m, int n)
{

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            m[i * n + j] = rand() % 100;
        }
    }
}

// 矩阵相减

void substraction(int *A, int *B, int *C, int N, int cols)
{

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            C[i * cols + j] = A[i * cols + j] - B[i * cols + j];
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
    // 创建 MPI 数据类型
    MPI_Datatype type_temp, type_slice;
    // lbound 是边界的下界，displacement 是边界的上界
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
        // 初始化矩阵 A B
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
        substraction(A, B, CC, N, N);

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

    // 创建 vector<MPI_INT> 数据类型
    MPI_Type_vector(N, NL, N, MPI_INT, &type_temp);
    // 读取 int 类型的地址大小
    MPI_Type_size(MPI_INT, &int_size);

    lbound = 0;
    displacement = int_size * NL;

    MPI_Type_create_resized(type_temp, lbound, displacement, &type_slice);
    MPI_Type_commit(&type_slice);

    // 初始化 AL、BL 和 TEMP
    AL = (int *)malloc(NL * N * sizeof(int));
    BL = (int *)malloc(NL * N * sizeof(int));
    TEMP = (int *)malloc(NL * N * sizeof(int));

    /* 进程 0 将矩阵 A 的水平切片分发给 AL */
    MPI_Scatter(A, 1, type_slice, AL, N * NL, MPI_INT, 0, MPI_COMM_WORLD);

    /* 进程 0 将矩阵 B 的水平切片分发给 BL */
    MPI_Scatter(B, 1, type_slice, BL, N * NL, MPI_INT, 0, MPI_COMM_WORLD);

    substraction(AL, BL, TEMP, N, NL);

    MPI_Gather(TEMP, NL * N, MPI_INT, C, 1, type_slice, 0, MPI_COMM_WORLD);

    /* 验证结果 */
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
            printf("Super !\nMatrix product A-B in parallel\n"
                   "equal the sequential one\n");
        else
            printf("False result !\nMatrix product A-B in parallel\n"
                   "different from the sequential one\n");
    }

    free(AL);
    free(BL);
    free(TEMP);

    MPI_Finalize();   // 终止 MPI 环境

    return 0;
}
