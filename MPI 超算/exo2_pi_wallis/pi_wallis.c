#include <mpi.h>         // 包含MPI库的头文件
#include <stdio.h>       // 包含标准输入输出库的头文件
#include <stdlib.h>      // 包含标准库的头文件

int main( int argc, char *argv[])
{
    int rank, size;                     // 声明变量rank和size，用于进程通信

    long long nbblock, width ,time;     // 声明变量nbblock、width、time，用于计算和统计

    long long  begin, end;               // 声明变量begin和end，用于计算范围

    double res, global;                  // 声明变量res和global，用于计算和汇总结果

    MPI_Init(&argc, &argv);              // MPI初始化，启动MPI环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 获取当前进程的rank（进程编号）
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // 获取进程的总数

    nbblock = 3*1000*1000LL*100;         // 计算nbblock的值
    width =  nbblock/size;               // 计算每个进程的处理范围

    res = 1.0;                           // 初始化结果res为1.0
    time = 0;                            // 初始化计数器time为0

    begin = rank*width+1;                // 计算当前进程的起始位置
    end = begin + width;                 // 计算当前进程的结束位置

    if (rank == size-1)                  // 如果当前进程是最后一个进程
    {
        end = nbblock+1;                 // 结束位置为nbblock+1，保证覆盖整个范围
    }

    for ( long long int i = begin; i < end; i++){  // 在范围内循环迭代计算
            res = res* 4.0*(i*i) / (4.0*(i*i) -1.0);  // 计算每个元素的结果
            time ++;                                 // 计数器加1
     }

    MPI_Reduce(&res,&global,1,MPI_DOUBLE,MPI_PROD,0,MPI_COMM_WORLD);  // 将每个进程的结果汇总到进程0中

    if (rank == 0 )  // 如果当前进程是进程0
    {
        global = global*2.0;                           // 对结果进行最后的计算
        printf("Pi = %.12lf\n", global);               // 打印最终结果（π的近似值）
    }
    
    MPI_Allreduce(MPI_IN_PLACE,&time,1,MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);  // 将计数器time在所有进程中进行求和

    if (rank == 0) printf("Nb = %lld\n", time);  // 如果当前进程是进程0，打印最终的计数器值

    MPI_Finalize();  // 结束MPI环境
    return 0;
}
