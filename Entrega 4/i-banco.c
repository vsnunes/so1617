/*
// Projeto SO - exercicio 3, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2016-17
Grupo 27:
    Afonso Samora N 83417
    Vitor Nunes   N 83576
*/

#include "commandlinereader.h"
#include "contas.h"

#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include "comando.h"

#define MAXARGS 4
#define BUFFER_SIZE 100

int nProcessos = 0; /*numero de processos filhos a correr*/

pthread_mutex_t mutex_simular;

int main (int argc, char** argv) {
	char *args[MAXARGS + 1];
  char buffer[BUFFER_SIZE];


  inicializarContas();

	signal(SIGUSR1, stop);

	initThreads();

	printf("Bem-vinda/o ao i-banco\n\n");

	while (1) {
		int numargs;

		numargs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

		/* EOF (end of file) do stdin ou comando "sair" */
		if (numargs < 0 ||
			(numargs > 0 && (strcmp(args[0], COMANDO_SAIR) == 0))) {

			int st, i;
			pid_t p;
			comando_t c;

			/* Comando sair agora */
			if ((args[1] != NULL) && (strcmp(args[1], COMANDO_SAIR_AGORA) == 0))
			{
				kill(0, SIGUSR1);
			}

			printf("i-banco vai terminar\n--\n");
      /* E enviado o comando SAIR para uma das trabalhadoras */
			c = newComando(SAIR, 0, 0, 0);
			colocarPedido(c);

			for(i=0; i< NUM_TRABALHADORAS; i++)
				pthread_join(tarefas[i], NULL);

        for (i = 0; i < nProcessos; i++)
				{
					p = wait(&st);
					printf("FILHO TERMINADO (PID=%d; terminou %s)\n", p, WIFEXITED(st) ? "normalmente" : "abruptamente");
				}
			/*destroi os mutexes e as variaveis de condicao*/
			destroyContas();
			destroyThreads();

			printf("--\ni-banco terminou.\n");

			exit(EXIT_SUCCESS);

			}

			else if (numargs == 0)
				/* Nenhum argumento; ignora e volta a pedir */
				continue;

			/* Debitar */
			else if (strcmp(args[0], COMANDO_DEBITAR) == 0) {
				comando_t p;
				if (numargs < 3) {
					printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_DEBITAR);
				  continue;
				}

				p = newComando(DEBITAR, atoi(args[1]),0, atoi(args[2]));
				colocarPedido(p);
		}

		/* Creditar */
		else if (strcmp(args[0], COMANDO_CREDITAR) == 0) {
			comando_t p;

			if (numargs < 3) {
				printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_CREDITAR);
				continue;
			}


			p = newComando(CREDITAR, atoi(args[1]),0, atoi(args[2]));
			colocarPedido(p);

		}

		/* Ler Saldo */
		else if (strcmp(args[0], COMANDO_LER_SALDO) == 0) {
			comando_t p;


			if (numargs < 2) {
				printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_LER_SALDO);
				continue;
			}

			p = newComando(LERSALDO, atoi(args[1]),0, 0);
			colocarPedido(p);


		}

		/* Simular */
		else if (strcmp(args[0], COMANDO_SIMULAR) == 0) {
			pid_t pid;
			int f_descriptor;
			char s[30];

			pthread_mutex_lock(&mutexComandos);
				while(nComandos > 0)
					pthread_cond_wait(&podeSimular, &mutexComandos);

			pthread_mutex_unlock(&mutexComandos);


			pid = fork();

			if (pid < 0)
			{
				perror("Erro no fork");
			}
			else if (pid > 0) {
				nProcessos++;
			}
			else {

				sprintf(s,"i-banco-sim-%d.txt", getpid());
				f_descriptor = open(s, O_CREAT | O_WRONLY);
				if (f_descriptor != -1) {
					close(1); /* fecha o stdout */
					dup(f_descriptor);

					simular(atoi(args[1]));
					exit(EXIT_SUCCESS);
				}
			}


		}


		else if (strcmp(args[0], COMANDO_TRANSFERIR) == 0) {
			comando_t cmd = newComando(TRANSFERIR, atoi(args[1]), atoi(args[2]), atoi(args[3]));
			colocarPedido(cmd);
		}

		else {
		  printf("Comando desconhecido. Tente de novo.\n");
		}

	  }


    return 0;

}
