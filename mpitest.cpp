#include <math.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{ /*myid 和 numprocs分别记录某一个并行执行进程的标识和所有参加计算的进程的个数*/
    int myid, numprocs;
    int namelen; /*MPI_MAX_PROCESSOR_NAME为MPI预定义的宏，代表MPI的具体实现中允许机器名字的最大长度*/
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //机器名存在processor_name中，namelen是实际机器名的长度		MPI_Get_processor_name(processor_name, &namelen);				fprintf(stderr , "Hello world! Process %d of %d on %s \n", 				myid, numprocs, processor_name);						MPI_Finalize();	}
    //机器名存在processor_name中，namelen是实际机器名的长度		
    MPI_Get_processor_name(processor_name, &namelen);				
    fprintf(stderr , "Hello world! Process %d of %d on %s \n", myid, numprocs, processor_name);						
    MPI_Finalize();
    return 0;
}