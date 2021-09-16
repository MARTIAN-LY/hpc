#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>

#define SIZE 1024

int main()
{

    //数据源：matrix.txt
    //naive算法数据保存目的地
    //OpenBlas库算法数据保存目的地
    FILE *fis = fopen("matrix.txt", "r");
    FILE *result_naive = fopen("result_naive.txt", "w");
    FILE *result_blas = fopen("result_blas.txt", "w");

    if (fis == NULL)
    {
        printf("Error with opening files\n");
        return 0;
    }

    double **a = (double **)malloc(sizeof(double *) * SIZE);
    double **c = (double **)malloc(sizeof(double *) * SIZE);

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
        }
    }
    fclose(fis);

    //naive算法，三层for循环
    clock_t start, end;
    start = clock();
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
    end = clock();
    printf("naive算法用时: %lf \n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fprintf(result_naive, "%lf ", c[i][j]);
        }
    }

    //OpenBlas库算法
    //需要把二维数组转成一维
    double *b = (double *)malloc(sizeof(double) * SIZE * SIZE);
    double *d = (double *)malloc(sizeof(double) * SIZE * SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            b[i*SIZE+j] = a[i][j];
        }
        
    }
    
    start = clock();
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SIZE, SIZE, SIZE, 1.0, b, SIZE, b, SIZE, 0, d, SIZE);
    end = clock();
    printf("OpenBlas库算法用时: %lf \n", (double)(end - start) / CLOCKS_PER_SEC);
    
    for (int i = 0; i < SIZE * SIZE; i++)
    {
        fprintf(result_blas,"%lf ",d[i]);
    }

    fclose(result_naive);
    fclose(result_blas);
}


