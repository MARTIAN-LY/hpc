#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 100

double **a;
double **c;

//把矩阵分成4份，每个线程处理一份
void calculate(void *arg)
{
    int n = *((int *)arg);

    for (int i = n; i < SIZE + n; i += 4)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                c[i][j] += a[i][k] * a[k][j];
            }
        }
    }
}

int main()
{
    //数据源：matrix.txt
    //结果存储地：result_thread.txt, result_omp.txt
    FILE *fis = fopen("matrix.txt", "r");
    FILE *result_thread = fopen("result_thread.txt", "w");
    FILE *result_omp = fopen("result_omp.txt", "w");

    if (fis == NULL)
    {
        printf("Error with opening files\n");
        return 0;
    }

    a = (double **)malloc(sizeof(double *) * SIZE);
    c = (double **)malloc(sizeof(double *) * SIZE);

    for (int i = 0; i < SIZE; i++)
    {
        a[i] = (double *)malloc(sizeof(double) * SIZE);
        c[i] = (double *)malloc(sizeof(double) * SIZE);
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fscanf(fis, "%lf", &a[i][j]);
            c[i][j] = 0;
        }
    }
    fclose(fis);

    //设置4个线程
    pthread_t thrd1, thrd2, thrd3, thrd4;
    int row[4] = {0, 1, 2, 3};

    //在多线程程序里不能用clock计时，clock会计算所有线程的总时间
    // clock_t start, end;
    // start = clock();
    // end = clock();
    struct timeval start, end;
    gettimeofday(&start, 0);

    pthread_create(&thrd1, NULL, (void *)calculate, &row[0]);
    pthread_create(&thrd2, NULL, (void *)calculate, &row[1]);
    pthread_create(&thrd3, NULL, (void *)calculate, &row[2]);
    pthread_create(&thrd4, NULL, (void *)calculate, &row[3]);
    pthread_join(thrd1, NULL);
    pthread_join(thrd2, NULL);
    pthread_join(thrd3, NULL);
    pthread_join(thrd4, NULL);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("time:%lf \n", elapsed);

    //结果写入文件
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fprintf(result_thread, "%lf ", c[i][j]);
        }
    }

    gettimeofday(&start, 0);
    //naive算法,三层for循环,但使用了omp的并行
#pragma omp parallel for
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < SIZE; k++)
            {
                c[i][j] += a[i][k] * a[k][j];
            }
        }
    }
    gettimeofday(&end, 0);
    seconds = end.tv_sec - start.tv_sec;
    microseconds = end.tv_usec - start.tv_usec;
    elapsed = seconds + microseconds * 1e-6;
    printf("time:%lf \n", elapsed);

    //结果写入文件
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fprintf(result_omp, "%lf ", c[i][j]);
        }
    }

    return 0;
}
