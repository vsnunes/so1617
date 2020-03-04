#include <unistd.h>
#include <sys/types.h>
#include "comando.h"
#include "contas.h"

void initThreads() {
    int i;
    buff_write_idx = 0;
    buff_read_idx = 0;
    nComandos = 0;
	pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexComandos, NULL);
	sem_init(&haTarefas, 0, NUM_TRABALHADORAS);
	sem_init(&haComandos,0, 0);

    for(i = 0; i < NUM_TRABALHADORAS; i++)
        pthread_create(&tarefas[i], NULL, executaPedidos, NULL);

	fLog = fopen(LOG_FILE, "w");
	if (fLog == NULL)
		perror("Erro na escrita do ficheiro.");
}

comando_t newComando(int operacao, int idConta_origem, int idConta_destino, int valor) {
	comando_t comando;
	comando.operacao = operacao;
	comando.idConta_origem = idConta_origem;
    comando.idConta_destino = idConta_destino;
	comando.valor = valor;
	return comando;
}

comando_t proximoPedido() {
	comando_t cmd;
	sem_wait(&haComandos);
	pthread_mutex_lock(&mutex);
		cmd = cmdbuffer[buff_read_idx];
		buff_read_idx = (buff_read_idx + 1) % NUM_TRABALHADORAS;
	pthread_mutex_unlock(&mutex);
    sem_post(&haTarefas);
	return cmd;
}

void colocarPedido(comando_t cmd)
{
	sem_wait(&haTarefas); /* existem tarefas trabalhadoras disponiveis? */
	pthread_mutex_lock(&mutex);
		cmdbuffer[buff_write_idx] = cmd;
		buff_write_idx = (buff_write_idx + 1) % NUM_TRABALHADORAS;
        pthread_mutex_lock(&mutexComandos);
            nComandos++;
        pthread_mutex_unlock(&mutexComandos);
	pthread_mutex_unlock(&mutex);
    sem_post(&haComandos);

}

void *executaPedidos() {
	int saldo;
	comando_t cmd;
  char s[100];

    while(1) {
		comando_t next = proximoPedido();
		switch (next.operacao) {
			case CREDITAR:
                if (creditar(next.idConta_origem, next.valor) < 0)
                    printf("%s(%d, %d): Erro\n\n", COMANDO_CREDITAR, next.idConta_origem, next.valor);
                else {
                    printf("%s(%d, %d): OK\n\n", COMANDO_CREDITAR, next.idConta_origem, next.valor);


					sprintf(s, "%ld: %s(%d, %d)\n",pthread_self(), COMANDO_CREDITAR, next.idConta_origem, next.valor);
					fputs(s, fLog);
				}
            break;

			case DEBITAR:
                if (debitar(next.idConta_origem, next.valor) < 0)
                    printf("%s(%d, %d): Erro\n\n", COMANDO_DEBITAR, next.idConta_origem, next.valor);
                else {
                    printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, next.idConta_origem, next.valor);
					sprintf(s, "%ld: %s(%d, %d)\n",pthread_self(), COMANDO_DEBITAR, next.idConta_origem, next.valor);
					fputs(s, fLog);
				}
            break;

			case LERSALDO:
                saldo = lerSaldo(next.idConta_origem);
                if(saldo < 0)
                    printf("%s(%d): Erro.\n\n", COMANDO_LER_SALDO, next.idConta_origem);
			          else {
				            printf("%s(%d): O saldo da conta é %d.\n\n", COMANDO_LER_SALDO, next.idConta_origem, saldo);
							sprintf(s, "%ld: %s(%d)\n", pthread_self(), COMANDO_LER_SALDO,next.idConta_origem);
							fputs(s, fLog);
					}
            break;

            case TRANSFERIR:
                if (transferir(next.idConta_origem, next.idConta_destino, next.valor) < 0)
                    printf("Erro ao transferir %d da conta %d para a conta %d\n\n",next.valor, next.idConta_origem, next.idConta_destino);
                else {
                    printf("%s(%d, %d, %d): OK\n\n", COMANDO_TRANSFERIR, next.idConta_origem, next.idConta_destino, next.valor);
					sprintf(s, "%ld: %s(%d, %d, %d)\n", pthread_self(), COMANDO_TRANSFERIR, next.idConta_origem, next.idConta_destino, next.valor);
					fputs(s, fLog);
				}
            break;

			case SAIR:
				cmd = newComando(SAIR,0,0,0);
				colocarPedido(cmd); /* Coloca-se no buffer das restantes trabalhadoras o comando SAIR */
				pthread_exit(EXIT_SUCCESS);
			break;
		}

		pthread_mutex_lock(&mutexComandos);
		    nComandos--;
            pthread_cond_signal(&podeSimular);
        pthread_mutex_unlock(&mutexComandos);




    }

}

void destroyThreads(){
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexComandos);
    pthread_cond_destroy(&podeSimular);
	fclose(fLog);
}
