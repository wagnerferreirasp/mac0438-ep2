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
#include <stdlib.h>  /* EXIT_SUCCESS, EXIT_FAILURE, atoi, malloc, NULL */
#include <pthread.h> /* pthread_t, pthread_create, pthread_join */
#include <unistd.h>  /* usleep */

pthread_mutex_t mutex;
int n, p, t, r, s;

int geraNumeroAleatorioDeZeroA(int limite) {
    return (rand() % limite);
}

void *aluno(void *_id) {
    int id = (int) _id;
    /* Define o tempo Tt := tempo até chegar, 0 <= Tt <= t */
    int tempoDeEspera = geraNumeroAleatorioDeZeroA(t);
    /* Define o tempo Tr := tempo que ficará na festa 0 <= Tr <= r */
    int tempoDeFesta = geraNumeroAleatorioDeZeroA(r);
    /* Espera Tt, usleep faz a thread dormir por 'x' microsegundos*/
    usleep(tempoDeEspera * 1000);
    /* Avisa que está na porta */
    printf("Aluno %d na porta.\n", id);
    /* Se o segurança está na sala   */
    /*     espera ele sair */
    /* Avisa que está na festa */
    /* Incrementa o número de alunos na sala */
    /* Espera Tr */
    /* Avisa que foi embora */
    /* Decrementa o número de alunos na sala */
    /* Incrementa o número de alunos que já participaram da festa */
}

void *seguranca(void *param) {
    printf("Sou o segurança\n");
    /* Define o tempo Tt := tempo até chegar, 0 <= Tt <= t */
    /* Espera Tt */
    /* Enquanto o número de alunos que já participaram da festa < n */
    /*     Avisa que está na porta */
    /*     Se o número de alunos na sala = 0 */
    /*         Bloqueia a entrada na sala */
    /*         Avisa que vai inspecionar a sala */
    /*     Senão Se o número de alunos na sala >= p */
    /*         Bloqueia a entrada na sala */
    /*         Avisa que vai expulsar os alunos */
    /*         Espera até que o número de alunos na sala seja 0 */
    /*     Avisa que está na ronda */
    /*     Desbloqueia a entrada */
    /*     Define o tempo 0 <= Ts <= S, o tempo de ronda */
    /*     Espera Ts */

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
    if (argc != 6) {
        mostraUso();
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]); /*    <convidados> */
    p = atoi(argv[2]); /* <minimo-alunos> */
    t = atoi(argv[3]); /*     <intervalo> */
    r = atoi(argv[4]); /*   <tempo-festa> */
    s = atoi(argv[5]); /*   <tempo-ronda> */
    srand (time(NULL));

    if (t < 0 || r < 0 || s < 0) {
        printf("Atenção! Os valores de <intervalo>, <tempo-festa> e <tempo-ronda> não devem ser negativos\n");
        mostraUso();
        exit(EXIT_FAILURE);
    }

    pthread_t *threadsAlunos = (pthread_t*) malloc(n * sizeof(pthread_t));

    int i, falhou = 0;

    falhou = pthread_mutex_init(&mutex, NULL);
    if (falhou) {
        printf("Ocorreu um erro ao inicializar o mutex\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n; i++)
    {
        falhou = pthread_create(&(threadsAlunos[i]),  /* pthread_t* tid */                           
                        NULL,                     /* const pthread_attr_t* attr */ 
                        (void *) aluno,           /* void* (*start_routine)(void *)  */  
                        (void*) i);               /* void *arg. */      
        /* Obs: Transformando i em void* diretamente, estamos passando o ponteiro de valor i à função aluno */
        if (falhou) {
            printf("Ocorreu um erro ao criar as threads dos alunos!\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t threadSeguranca;
    falhou = pthread_create(&threadSeguranca, /* pthread_t* tid */                           
                        NULL,                 /* const pthread_attr_t* attr */ 
                        (void *) seguranca,   /* void* (*start_routine)(void *)  */  
                        NULL);                /* void *arg. */  

    if (falhou) {
        printf("Ocorreu um erro ao criar a thread do segurança!\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n; i++) {
        pthread_join(threadsAlunos[i], NULL);
    }
    pthread_join(threadSeguranca, NULL);
    printf("Término de festa!\n");

    exit(EXIT_SUCCESS);
}
