/*******************************************************************/
/** MAC 438 - Programação Concorrente                              */
/** IME-USP - Primeiro Semestre de 2016                            */
/** Prof. Marcel Parolin Jackowski                                 */
/**                                                                */
/** Segundo Exercício-Programa                                     */
/** Arquivo: main.c                                                */
/**                                                                */
/** Wagner Ferreira Alves    7577319                               */
/** Rafael Marinaro Verona   7577323                               */
/** 24/05/2016                                                     */
/*******************************************************************/

#include <stdio.h>   /* printf */
#include <stdlib.h>  /* EXIT_SUCCESS, EXIT_FAILURE, atoi, malloc, NULL, rand, srand */
#include <pthread.h> /* pthread_t, pthread_create, pthread_join, pthread_mutex_t */
#include <time.h>    /* time_t, time, nanosleep, struct timespec */

/* Conversão de milissegundos para nanossegundos: 1 ms = 1000000 ns */
#define MILI_PARA_NANO 1000000

pthread_mutex_t mutex;
int n, p, t, r, s;
int *filaDeAlunos;
int inicioDaFila, fimDaFila; //inicio = primeiro espaço ocupado, fim = primeiro espaço livre
int alunosNaFesta, alunosQueJaParticiparam;

int geraNumeroAleatorioDeZeroA(int limite) {
    return (rand() % limite);
}

void esperaEmMilissegundos(long ms) {
    struct timespec tempo;
    tempo.tv_sec = (time_t) (ms / 1000);
    tempo.tv_nsec = (long) (ms - tempo.tv_sec * 1000) * MILI_PARA_NANO;
    nanosleep(&tempo, NULL);
}

void *aluno(void *_id) {
    int id = *((int*)_id); /* valor apontado por _id é o id que queremos */
    /* Define o tempo Tt := tempo até chegar, 0 <= Tt <= t */
    int tempoAteChegar = geraNumeroAleatorioDeZeroA(t);
    /* Define o tempo Tr := tempo que ficará na festa 0 <= Tr <= r */
    int tempoDeFesta = geraNumeroAleatorioDeZeroA(r);
    /* Espera Tt, faz a thread dormir por 'tempoAteChegar' milissegundos*/
    esperaEmMilissegundos(tempoAteChegar);
    /* Avisa que está na porta */
    printf("Aluno %d na porta.\n", id);
    /* Coloca o aluno na fila de alunos para entrar */
    filaDeAlunos[fimDaFila++] = id;
    /* Verifica se é o primeiro da fila */
    while(filaDeAlunos[inicioDaFila] != id) {
    }
    /* Espera até a porta estar disponível */
    pthread_mutex_lock(&mutex);
    /* Avisa que está na festa */
    printf("Aluno %d na festa.\n", id);
    /* Retira da fila */
    inicioDaFila++;
    /* Libera a porta */
    pthread_mutex_unlock(&mutex);
    /* Incrementa o número de alunos na sala */
    alunosNaFesta++;
    /* Espera Tr */
    esperaEmMilissegundos(tempoDeFesta);
    /* Avisa que foi embora */
    printf("Aluno %d vai embora.\n", id);
    /* Decrementa o número de alunos na sala */
    alunosNaFesta--;
    /* Incrementa o número de alunos que já participaram da festa */
    alunosQueJaParticiparam++;
}

void *seguranca(void *param) {
    /* Define o tempo Tt := tempo até chegar, 0 <= Tt <= t */
    int tempoAteChegar = geraNumeroAleatorioDeZeroA(t);
    int tempoEntreRondas;
    /* Espera Tt */
    esperaEmMilissegundos(tempoAteChegar);
    printf("Segurança em ronda\n");
    /* Enquanto o número de alunos que já participaram da festa < n */
    while (alunosQueJaParticiparam < n) {
        /*     Avisa que está na porta */
        pthread_mutex_lock(&mutex);
        printf("Segurança na porta\n");
        /*     Se o número de alunos na sala = 0 */
        if (alunosNaFesta == 0) {
            /*         Avisa que vai inspecionar a sala */
            printf("Segurança inspeciona a sala.\n");
        }
        /*     Senão Se o número de alunos na sala >= p */
        else if (alunosNaFesta >= p) {
            /*         Avisa que vai expulsar os alunos */
            printf("Segurança expulsa alunos.\n");
            /*         Espera até que o número de alunos na sala seja 0 */
            while(alunosNaFesta > 0) {}
        }
        /*     Avisa que está na ronda */
        printf("Segurança em ronda\n");
        /*     Desbloqueia a entrada */
        pthread_mutex_unlock(&mutex);
        /*     Define o tempo 0 <= Ts <= S, o tempo de ronda */
        tempoEntreRondas = geraNumeroAleatorioDeZeroA(s);
        /*     Espera Ts */
        esperaEmMilissegundos(tempoEntreRondas);
    }
}

void mostraUso() {
    printf("Uso:\n"); 
    printf(" ./ep2 <convidados> <minimo-alunos> <intervalo> <tempo-festa> <tempo-ronda>\n");
    printf("Onde:\n");
    printf("       <convidados>: numero total de convidados para a festa\n");
    printf("    <minimo-alunos>: numero minimo de alunos na festa para o seguranca esvaziar a sala\n");
    printf("        <intervalo>: intervalo maximo de tempo dentre chegadas de convidados em ms\n");
    printf("      <tempo-festa>: tempo maximo de participacao na festa para cada aluno em ms\n");
    printf("      <tempo-ronda>: tempo maximo de ronda do seguranca em ms\n");
}

int main(int argc, char const *argv[])
{
    /* Verifica se o usuário digitou 5 parâmetros */
    if (argc != 6) {
        mostraUso();
        exit(EXIT_FAILURE);
    }

    /* Atribuição dos parâmetros */
    n = atoi(argv[1]); /*    <convidados> */
    p = atoi(argv[2]); /* <minimo-alunos> */
    t = atoi(argv[3]); /*     <intervalo> */
    r = atoi(argv[4]); /*   <tempo-festa> */
    s = atoi(argv[5]); /*   <tempo-ronda> */
    srand (time(NULL));
    alunosNaFesta = alunosQueJaParticiparam = 0;

    /* Verifica se os parâmetros são valores válidos */
    if (t < 0 || r < 0 || s < 0) {
        printf("Atenção! Os valores de <intervalo>, <tempo-festa> e <tempo-ronda> não devem ser negativos\n");
        mostraUso();
        exit(EXIT_FAILURE);
    }

    /* Cria vetor de threads para representar os alunos */
    pthread_t *threadsAlunos = (pthread_t*) malloc(n * sizeof(pthread_t));

    /* Cria uma thread para representar o segurança */
    pthread_t threadSeguranca;

    /* Inicializa uma fila para representar a ordem de chegada dos alunos */
    filaDeAlunos = malloc(n * sizeof(int));
    if (filaDeAlunos == NULL) {
        printf("Falha ao alocar memória\n");
        exit(EXIT_FAILURE);
    }
    inicioDaFila = fimDaFila = 0;

    /* Variáveis para índice, valores de retorno e id do aluno passado à thread */
    int i, falhou, *idAluno;

    /* Inicialização do Mutex */
    falhou = pthread_mutex_init(&mutex, NULL);
    if (falhou) {
        printf("Ocorreu um erro ao inicializar o mutex\n");
        exit(EXIT_FAILURE);
    }


    /* Inicialização das theads dos alunos */
    for (i = 1; i <= n; i++)
    {
        /* alocando novo ponteiro para um inteiro, a ser passado para a thread do aluno */
        idAluno = (int*) malloc(sizeof(int*)); 
        *idAluno = i;

        falhou = pthread_create(&(threadsAlunos[i]),  /* pthread_t* tid */                           
                        NULL,                     /* const pthread_attr_t* attr */ 
                        aluno,           /* void* (*start_routine)(void *)  */  
                        idAluno);               /* void *arg. */      
        if (falhou) {
            printf("Ocorreu um erro ao criar as threads dos alunos!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Inicialização da thread do segurança */
    falhou = pthread_create(&threadSeguranca, /* pthread_t* tid */                           
                        NULL,                 /* const pthread_attr_t* attr */ 
                        seguranca,   /* void* (*start_routine)(void *)  */  
                        NULL);                /* void *arg. */  

    if (falhou) {
        printf("Ocorreu um erro ao criar a thread do segurança!\n");
        exit(EXIT_FAILURE);
    }

    /* Espera as threads dos alunos e do segurança terminarem */
    for (i = 0; i < n; i++) {
        pthread_join(threadsAlunos[i], NULL);
    }
    pthread_join(threadSeguranca, NULL);

    /* Termina a festa */
    printf("Término de festa!\n");

    exit(EXIT_SUCCESS);
}
