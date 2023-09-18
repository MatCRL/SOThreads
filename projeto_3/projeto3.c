#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

float *meuArray;
int tamanhoGlobal = 10000;

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
    // Ler parametros
    bool isPrintArray = false;
    bool processos = false;
    bool testAleatorio = false;
    bool testFixo = false;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "array=", strlen("array=")) == 0)
        {
            tamanhoGlobal = strtol(argv[i] + strlen("array="), NULL, 10);
        }
        else if (strcmp(argv[i], "processos") == 0)
        {
            processos = true;
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

    // Alocando memória para o meuArray
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

    if (isPrintArray)
    {
        printf("Array antes da ordenacao:\n");
        printArray(meuArray, tamanhoGlobal);
    }

    struct timespec inicio, fim;
    clock_gettime(CLOCK_REALTIME, &inicio);

    if (processos)
    {

        int pipe_fds[2];
        pipe(pipe_fds);
        pid_t pid;

        if ((pid = fork()) == 0)
        {
            // Processo filho
            // printf("Executando o processo filho\n");
            close(pipe_fds[0]);
            bubbleSort(meuArray, 0, tamanhoGlobal / 2);
            write(pipe_fds[1], meuArray, sizeof(float) * tamanhoGlobal / 2);
            close(pipe_fds[1]);
            exit(0);
        }
        else
        {
            // Processo pai
            close(pipe_fds[1]);
            bubbleSort(meuArray, tamanhoGlobal / 2, tamanhoGlobal);

            wait(NULL); // Espera o processo filho terminar

            read(pipe_fds[0], meuArray, sizeof(float) * tamanhoGlobal / 2);
            close(pipe_fds[0]);

            // Realizando uma segunda ordenação com bubbleSort no array inteiro
            bubbleSort(meuArray, 0, tamanhoGlobal);
        }
    }
    else
    {
        bubbleSort(meuArray, 0, tamanhoGlobal);
    }

    // Finalizar o timer
    clock_gettime(CLOCK_REALTIME, &fim);

    // Exibe o array após a ordenação

    if (isPrintArray)
    {
        printf("Array após a ordenação:\n");
        printArray(meuArray, tamanhoGlobal);
    }
    // Exibir o timer
    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf(
        "Tempo gasto em um array(%d) com processo filho(%s): %f segundos.\n",
        tamanhoGlobal,
        (processos ? "Ativo" : "Desativado"),
        tempo_gasto);

    free(meuArray);

    return 0;
}
