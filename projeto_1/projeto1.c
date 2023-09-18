#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

float **A;
float **B;
float **A_posicional;
float **B_posicional;

float **resultado_matricial;
float **resultado_posicional;

int tamanhoMatriz = 3;
int tamanhoMatriz_posicional = 3;

typedef struct
{
    int inicio;
    int fim;
} Parametros;

void inicializar_matrizes()
{
    A = malloc(tamanhoMatriz * sizeof(float *));
    B = malloc(tamanhoMatriz * sizeof(float *));
    resultado_matricial = malloc(tamanhoMatriz * sizeof(float *));
    resultado_posicional = malloc(tamanhoMatriz * sizeof(float *));

    for (int i = 0; i < tamanhoMatriz; i++)
    {
        A[i] = malloc(tamanhoMatriz * sizeof(float));
        B[i] = malloc(tamanhoMatriz * sizeof(float));
        resultado_matricial[i] = malloc(tamanhoMatriz * sizeof(float));
        resultado_posicional[i] = malloc(tamanhoMatriz * sizeof(float));
    }

    srand(time(NULL));

    for (int i = 0; i < tamanhoMatriz; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            A[i][j] = (float)rand() / (float)(RAND_MAX);
            B[i][j] = (float)rand() / (float)(RAND_MAX);
        }
    }
}

void multiplicacao_matricial()
{
    for (int i = 0; i < tamanhoMatriz; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            resultado_matricial[i][j] = A[i][j] * B[i][j];
        }
    }
}

void multiplicacao_posicional()
{
    for (int i = 0; i < tamanhoMatriz; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            resultado_posicional[i][j] = 0;
            for (int k = 0; k < tamanhoMatriz; k++)
            {
                resultado_posicional[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void *multiplicacao_matricial_multithread(void *param)
{
    Parametros *p = (Parametros *)param;
    for (int i = p->inicio; i < p->fim; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            resultado_matricial[i][j] = A[i][j] * B[i][j];
        }
    }
    return NULL;
}

void *multiplicacao_posicional_multithread(void *param)
{
    Parametros *p = (Parametros *)param;
    for (int i = p->inicio; i < p->fim; i++)
    {
        for (int j = 0; j < tamanhoMatriz_posicional; j++)
        {
            resultado_posicional[i][j] = 0;
            for (int k = 0; k < tamanhoMatriz_posicional; k++)
            {
                resultado_posicional[i][j] += A_posicional[i][k] * B_posicional[k][j];
            }
        }
    }
    return NULL;
}

void *multiplicacao_multithread(void *param)
{
    multiplicacao_matricial_multithread(param);
    multiplicacao_posicional_multithread(param);
}

void duplicarMatrizes()
{
    A_posicional = malloc(tamanhoMatriz * sizeof(float *));
    B_posicional = malloc(tamanhoMatriz * sizeof(float *));
    for (int i = 0; i < tamanhoMatriz; i++)
    {
        A_posicional[i] = malloc(tamanhoMatriz * sizeof(float));
        B_posicional[i] = malloc(tamanhoMatriz * sizeof(float));
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            A_posicional[i][j] = A[i][j];
            B_posicional[i][j] = B[i][j];
        }
    }
}

void exibirResultados()
{
    printf("Resultado Multiplicação Matricial:\n");
    for (int i = 0; i < tamanhoMatriz; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            printf("%f ", resultado_matricial[i][j]);
        }
        printf("\n");
    }

    printf("Resultado Multiplicação Posicional:\n");
    for (int i = 0; i < tamanhoMatriz; i++)
    {
        for (int j = 0; j < tamanhoMatriz; j++)
        {
            printf("%f ", resultado_posicional[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    struct timespec inicio, fim;
    clock_t inicio_clock, fim_clock;

    bool multiThread = false;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "matriz=", strlen("matriz=")) == 0)
        {
            tamanhoMatriz = strtof(argv[i] + strlen("matriz="), NULL);
            tamanhoMatriz_posicional = strtof(argv[i] + strlen("matriz="), NULL);
        }
        else if (strcmp(argv[i], "multithread") == 0)
        {
            multiThread = true;
        }
    }
    inicializar_matrizes();

    inicio_clock = clock();
    clock_gettime(CLOCK_REALTIME, &inicio);

    if (multiThread)
    {
        pthread_t thread1, thread2, thread3, thread4;
        Parametros p1, p2, p3, p4;

        p1.inicio = 0;
        p1.fim = tamanhoMatriz / 4;

        p2.inicio = tamanhoMatriz / 4;
        p2.fim = (tamanhoMatriz / 4) * 2;

        p3.inicio = (tamanhoMatriz / 4) * 2;
        p3.fim = (tamanhoMatriz / 4) * 3;

        p4.inicio = (tamanhoMatriz / 4) * 3;
        p4.fim = tamanhoMatriz;

        duplicarMatrizes();

        pthread_create(&thread1, NULL, multiplicacao_multithread, &p1);
        pthread_create(&thread2, NULL, multiplicacao_multithread, &p2);
        pthread_create(&thread3, NULL, multiplicacao_multithread, &p3);
        pthread_create(&thread4, NULL, multiplicacao_multithread, &p4);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_join(thread3, NULL);
        pthread_join(thread4, NULL);
    }
    else
    {
        multiplicacao_matricial();
        multiplicacao_posicional();
    }

    fim_clock = clock();
    clock_gettime(CLOCK_REALTIME, &fim);

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    double tempo_gasto_clock = ((double)(fim_clock - inicio_clock)) / CLOCKS_PER_SEC;
    printf(
        "Tempo gasto em uma matriz(%dx%d) com multiThread(%s): %f segundos, %f segundos clock.\n",
        tamanhoMatriz,
        tamanhoMatriz,
        (multiThread ? "Ativo" : "Desativado"),
        tempo_gasto,
        tempo_gasto_clock);

    // exibirResultados();

    return 0;
}