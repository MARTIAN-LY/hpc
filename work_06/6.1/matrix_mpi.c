#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define SIZE 1000
#define PROCESS_NUM 4

double **a = NULL;
double **c = NULL;

//读入矩阵数据
void readMatrix(FILE *fis)
{
    a = (double **)malloc(sizeof(double *) * SIZE);

    for (int i = 0; i < SIZE; i++)
    {
        a[i] = (double *)malloc(sizeof(double) * SIZE);
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fscanf(fis, "%lf", &a[i][j]);
        }
    }
    fclose(fis);
}

//结果写入文件
void writeToFile(FILE *fos, double **matrix)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fprintf(fos, "%lf ", matrix[i][j]);
        }
    }
    fclose(fos);
    free(matrix);
}

//并行进程的计算
void calculate_branch(int process_rank)
{
    for (int i = process_rank; i < SIZE + process_rank; i += PROCESS_NUM)
    {
        for (int j = 0; j < SIZE; j++)
        {
            double tmp = 0;
            for (int k = 0; k < SIZE; k++)
            {
                tmp += a[i][k] * a[k][j];
            }
            c[i][j] = tmp;
        }
    }
}

int main(int argc, char *argv[])
{
    //在并行前读入数据
    //数据源：matrix.txt
    FILE *fis = fopen("matrix.txt", "r");
    if (fis == NULL)
    {
        printf("Error with opening files\n");
        return 0;
    }
    //读入矩阵数据
    readMatrix(fis);

    // c用来存储结果
    c = (double **)calloc(SIZE, sizeof(double *));
    for (int i = 0; i < SIZE; i++)
    {
        c[i] = (double *)calloc(SIZE, sizeof(double));
    }

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        //主进程计算结果中的4k列（k = 0，1，2，...)
        calculate_branch(rank);

        // tmp用来接收并行线程的结果
        double **tmp = (double **)calloc(SIZE, sizeof(double *));
        for (int i = 0; i < SIZE; i++)
        {
            tmp[i] = (double *)calloc(SIZE, sizeof(double));
        }

        for (int i = 1; i < PROCESS_NUM; i++)
        {
            MPI_Recv(tmp, SIZE * SIZE, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = i; j < SIZE + i; j += PROCESS_NUM)
            {
                for (int k = 0; k < SIZE; i++)
                {
                    c[j][k] = tmp[j][k];
                }
            }
        }
        free(tmp);
        FILE *fos = fopen("result.txt", "w");
        writeToFile(fos, c);
    }
    else
    {
        calculate_branch(rank);
        MPI_Send(c, SIZE * SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
