#include <mpi.h>         // 包含MPI库的头文件
#include <stdio.h>       // 包含标准输入输出库的头文件
#include <stdlib.h>      // 包含标准库的头文件
#include <time.h>        // 包含时间库的头文件
#include <math.h>        // 包含数学库的头文件

int main( int argc, char *argv[])
{
    int rank, size;                     // 声明变量rank和size，用于进程通信
    unsigned long int n, hits, global;  // 声明变量n、hits和global，用于计算和汇总
    
    MPI_Init(&argc, &argv);              // MPI初始化，启动MPI环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 获取当前进程的rank（进程编号）
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // 获取进程的总数
    
    srand((unsigned)time(NULL));  // 初始化随机数生成器，根据当前时间

    hits = 0;  // 初始化命中计数器为0

    if(rank == 0 ){
        scanf("%ld", &n);  // 进程0从标准输入中读取n的值
    }

    MPI_Barrier(MPI_COMM_WORLD);  // 所有进程在此处进行同步

    MPI_Bcast(&n,1,MPI_UNSIGNED_LONG,0,MPI_COMM_WORLD);  // 进程0将n广播给所有其他进程

    printf("here %d ---> %ld\n",rank,n);  // 打印当前进程的rank和n的值

    for (unsigned long i = 0; i < n; i++)  // 进行n次迭代
    {
       double x , y ;
       x = rand() / (double)RAND_MAX;  // 生成[0,1)之间的随机数x
       y = rand()/ (double)RAND_MAX;  // 生成[0,1)之间的随机数y
       double dist = sqrt(x*x+y*y);   // 计算点(x,y)到原点的距离
       if (dist < 1.0)  // 如果距离小于1.0，即点在单位圆内
       {
        hits++;  // 命中计数器加1
       }
    }

    MPI_Reduce(&hits,&global,1,MPI_UNSIGNED_LONG,MPI_SUM,0,MPI_COMM_WORLD);  // 所有进程将命中计数器的值汇总到进程0中

    if (rank==0)  // 如果当前进程是进程0
    {
        double pi = 4 * (global/(n*(double)size));  // 计算估计的圆周率π的值
        printf("PI : %f\n", pi);  // 打印估计的圆周率π的值
    }

    MPI_Finalize();  // 结束MPI环境
    return 0;
}
