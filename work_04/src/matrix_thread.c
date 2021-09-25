#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define SIZE 1000

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
    //结果存储地：result.txt
    FILE *fis = fopen("matrix.txt", "r");
    FILE *result = fopen("result.txt", "w");

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
    clock_t start, end;
    start = clock();
    pthread_create(&thrd1, NULL, (void *)calculate, &row[0]);
    pthread_create(&thrd2, NULL, (void *)calculate, &row[1]);
    pthread_create(&thrd3, NULL, (void *)calculate, &row[2]);
    pthread_create(&thrd4, NULL, (void *)calculate, &row[3]);

    pthread_join(thrd1,NULL);
    pthread_join(thrd2,NULL);
    pthread_join(thrd3,NULL);
    pthread_join(thrd4,NULL);

    end = clock();
    printf("算法用时: %lf \n", (double)(end - start) / CLOCKS_PER_SEC);

    //结果写入文件
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fprintf(result, "%lf ", c[i][j]);
        }
    }

    return 0;
}
