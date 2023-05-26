#include "mpi.h"              // 包含MPI库的头文件
#include <stdio.h>            // 包含标准输入输出库的头文件
#include <stdlib.h>           // 包含标准库的头文件

int main(int argc, char *argv[])
{
    int rank, value, size, finalValue;  // 声明变量rank、value、size、finalValue用于进程通信
    int tag = 0;                        // 消息的标签，用于区分不同的消息
    MPI_Status status;                  // 用于存储消息接收的状态
    value = 999;                        // 初始化value为999
    finalValue = 1100;                  // 初始化finalValue为1100
    MPI_Init(&argc, &argv);             // MPI初始化，启动MPI环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 获取当前进程的rank（进程编号）
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // 获取进程的总数

    while (value < finalValue)  // 当value小于finalValue时循环执行以下代码
    {
        if (rank == 0)  // 如果当前进程的rank为0（即进程0）
        {
            if (value < finalValue)  // 如果value小于finalValue
            {
                value++;  // value加1
                MPI_Send(&value, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);  // 向进程1发送value的值
                printf("Proc 0 sends %d to Proc 1\n", value);  // 打印消息，表示进程0发送了一个值给进程1

                if (value + size > finalValue)  // 如果value加上进程的总数大于finalValue
                {
                    break;  // 跳出循环
                }

                MPI_Recv(&value, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &status);  // 接收进程size-1发送的消息，存储在value中
                printf("Proc %d receives %d from Proc %d\n", rank, value, size - 1);  // 打印消息，表示进程0接收了来自进程size-1的值
            }
        }
        else  // 如果当前进程的rank不为0（即其他进程）
        {
            MPI_Recv(&value, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);  // 接收来自rank-1进程发送的消息，存储在value中
            printf("Proc %d receives %d from Proc %d\n", rank, value, rank - 1);  // 打印消息，表示当前进程接收了来自rank-1进程的值
            if (value < finalValue)  // 如果value小于finalValue
            {
                value++;  // value加1
                if (rank == size - 1)  // 如果当前进程是最后一个进程（进程总数减一）
                {
                    MPI_Send(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);  // 向进程0发送value的值
                    printf("Proc %d sends %d to Proc %d\n", rank, value, 0);  // 打印消息，表示当前进程发送了一个值给进程0
                }
                else
                {
                    MPI_Send(&value, 1, MPI_INT, rank + 1, tag, MPI_COMM_WORLD);  // 向下一个进程发送value的值
                    printf("Proc %d sends %d to Proc %d\n", rank, value, rank + 1);  // 打印消息，表示当前进程发送了一个值给下一个进程
                }

                if (value - 1 + size > finalValue)  // 如果value减一加上进程的总数大于finalValue
                {
                    printf("RANK: %d VALUE:%d\n", rank, value);  // 打印消息，表示当前进程的rank和value
                    break;  // 跳出循环
                }
            }
        }

        printf("RANK: %d VALUE:%d\n", rank, value);  // 打印消息，表示当前进程的rank和value
    }

    printf("Proc %d stopped\n", rank);  // 打印消息，表示当前进程停止
    MPI_Finalize();  // 结束MPI环境
    return 0;
}
