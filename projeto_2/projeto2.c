#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

float *meuArray;

// Variável global para o tamanho do array
int tamanhoGlobal = 10;

typedef struct
{
    int inicio;
    int fim;
    float *array;
} Parametros;

void preencherArrayDecrescente(float arr[], int tamanho)
{
    float numero = (float)tamanho;
    for (int i = 0; i < tamanho; i++)
    {
        arr[i] = numero;
        numero--;
    }
}

void preencherArrayFixo(float arr[], int tamanho)
{
    float arr2[6] = {4.0, 3.0, 1.0, 5.0, 6.0, 2.0};
    float numero = (float)tamanho;
    for (int i = 0; i < tamanho; i++)
    {
        arr[i] = arr2[i];
        numero--;
    }
}

void preencherArrayAleatorio(float arr[], int tamanho)
{
    srand(time(NULL));

    for (int i = 0; i < tamanho; i++)
    {
        arr[i] = (float)(rand() % 10 + 1);
    }
}

void bubbleSort(float arr[], int inicio, int fim)
{
    float temp;
    int trocado;

    do
    {
        trocado = 0;
        for (int i = inicio; i < fim - 1; i++)
        {
            if (arr[i] > arr[i + 1])
            {
                temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                trocado = 1;
            }
        }
    } while (trocado);
}

void *bubbleSort_multithread(void *param)
{
    Parametros *p = (Parametros *)param;
    bubbleSort(p->array, p->inicio, p->fim);
    return NULL;
}

void printArray(float array[], int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        printf(" %f,", array[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct timespec inicio, fim;
    clock_t inicio_clock, fim_clock;

    bool isPrintArray = false;
    bool multiThread = false;
    bool testAleatorio = false;
    bool testFixo = false;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "array=", strlen("array=")) == 0)
        {
            tamanhoGlobal = strtol(argv[i] + strlen("array="), NULL, 10);
        }
        else if (strcmp(argv[i], "multithread") == 0)
        {
            multiThread = true;
        }
        else if (strcmp(argv[i], "print") == 0)
        {
            isPrintArray = true;
        }
        else if (strcmp(argv[i], "aleatorio") == 0)
        {
            testAleatorio = true;
        }
        else if (strcmp(argv[i], "fixo") == 0)
        {
            testFixo = true;
        }
    }

    // Alocando memória para o meuArray após processar os argumentos da linha de comando
    meuArray = malloc(tamanhoGlobal * sizeof(float));

    if (testFixo)
    {
        preencherArrayFixo(meuArray, tamanhoGlobal);
    }
    else if (testAleatorio)
    {
        preencherArrayAleatorio(meuArray, tamanhoGlobal);
    }
    else
    {
        preencherArrayDecrescente(meuArray, tamanhoGlobal);
    }

    // Exibe o array antes da ordenação
    if (isPrintArray)
    {
        printf("Array antes da ordenacao:\n");
        printArray(meuArray, tamanhoGlobal);
    }

    // Iniciar o timer
    inicio_clock = clock();
    clock_gettime(CLOCK_REALTIME, &inicio);

    if (multiThread)
    {
        pthread_t thread1, thread2;
        Parametros p1, p2;
        pthread_attr_t attr; /* set of attributes for the thread */

        pthread_attr_init(&attr);

        int meio = tamanhoGlobal / 2;
        p1.array = meuArray;
        p1.inicio = 0;
        p1.fim = meio;

        p2.array = meuArray;
        p2.inicio = meio;
        p2.fim = tamanhoGlobal;

        pthread_create(&thread1, &attr, bubbleSort_multithread, &p1);
        pthread_create(&thread2, &attr, bubbleSort_multithread, &p2);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        if (isPrintArray)
        {
            printf("Array antes do merge:\n");
            printArray(meuArray, tamanhoGlobal);
        }
        // bubbleSort(meuArray, meio - 1, meio + 1);
        bubbleSort(meuArray, 0, tamanhoGlobal);
    }
    else
    {
        bubbleSort(meuArray, 0, tamanhoGlobal);
    }

    // Finalizar o timer
    fim_clock = clock();
    clock_gettime(CLOCK_REALTIME, &fim);

    // Exibe o array após a ordenação
    if (isPrintArray)
    {
        printf("Array apos a ordenacao:\n");
        printArray(meuArray, tamanhoGlobal);
    }

    // Exibir o timer
    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    double tempo_gasto_clock = ((double)(fim_clock - inicio_clock)) / CLOCKS_PER_SEC;

    printf("Tempo gasto: %f", tempo_gasto);
    printf("\n");

    return 0;
}
