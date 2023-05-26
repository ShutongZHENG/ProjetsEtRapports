#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  int rank, iter, iter2;
  int nb_tests = 10;          // 测试的数量
  int nb_values[nb_tests];    // 每个测试中的值的数量
  int nb_values_max = 7000000;  // 值的最大数量
  int tag = 99;               // 消息的标签
  double *values;             // 值的数组

  MPI_Status status1, status2;  // 消息接收的状态
  MPI_Request r1, r2;           // 消息请求
  double time_begin, time_end;  // 计时变量

  MPI_Init(&argc, &argv);            // MPI初始化，启动MPI环境

  nb_values[0] = 0, nb_values[1] = 0, nb_values[2] = 1, nb_values[3] = 10;
  nb_values[4] = 100, nb_values[5] = 1000, nb_values[6] = 10000;
  nb_values[7] = 100000, nb_values[8] = 1000000, nb_values[9] = 7000000;  // 初始化每个测试中的值的数量

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 获取当前进程的rank（进程编号）

  values = (double *)malloc(nb_values_max * sizeof(double));  // 分配存储值的数组内存

  for (iter = 0; iter < nb_tests; iter++)  // 迭代进行每个测试
  {
    if (rank == 0)  // 如果当前进程是进程0
    {
      for (iter2 = 0; iter2 < nb_values_max; iter2++)  // 初始化值数组
        values[iter2] = rand() / (RAND_MAX + 1.);
      time_begin = MPI_Wtime();  // 记录开始时间

      MPI_Isend(values, nb_values[iter], MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &r1);  // 异步发送消息给进程1
      MPI_Irecv(values, nb_values[iter], MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &r2);  // 异步接收进程1发送的消息
      MPI_Wait(&r1, &status1);  // 等待发送完成
      MPI_Wait(&r2, &status2);  // 等待接收完成

      time_end = MPI_Wtime();  // 记录结束时间

      if (nb_values[iter] != 0)
      {
        printf("Me, process 0, sent and received %8d values "
               "(last = %4.2f) from process 1 in %8.6f seconds, bandwidth "
               "%7.2f MB/s.\n",
               nb_values[iter], values[nb_values[iter] - 1],
               time_end - time_begin,
               2. * nb_values[iter] * 8 / 1000000. / (time_end - time_begin));
      }
      else
      {
        printf("Me, process 0, sent and received %8d values in %8.6f "
               "seconds, bandwidth %7.2f MB/s.\n",
               nb_values[iter], time_end - time_begin,
               2. * nb_values[iter] * 8 / 1000000. / (time_end - time_begin));
      }
    }
    else if (rank == 1)  // 如果当前进程是进程1
    {
      MPI_Irecv(values, nb_values[iter], MPI_DOUBLE, 0, tag,
                MPI_COMM_WORLD, &r1);  // 异步接收进程0发送的消息
      MPI_Isend(values, nb_values[iter], MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &r2);  // 异步发送消息给进程0
      MPI_Wait(&r1, &status1);  // 等待接收完成
      MPI_Wait(&r2, &status2);  // 等待发送完成
    }
  }

  MPI_Finalize();  // 结束MPI环境
  return 0;
}
