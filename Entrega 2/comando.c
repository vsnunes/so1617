#include "comando.h"
#include "contas.h"
#include <stdio.h>

void initThreads() {
    int i;
    buff_write_idx = 0;
	buff_read_idx = 0;
	pthread_mutex_init(&mutex, NULL); 
	sem_init(&haTarefas, 0, NUM_TRABALHADORAS);
	sem_init(&haComandos,0, 0);
    for(i = 0; i < NUM_TRABALHADORAS; i++)
        pthread_create(&tarefas[i], NULL, executaPedidos, NULL);
	
}

comando_t newComando(int operacao, int idConta, int valor) {
	comando_t comando;
	comando.operacao = operacao;
	comando.idConta = idConta;
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
	pthread_mutex_unlock(&mutex);
    sem_post(&haComandos);
	
}

void *executaPedidos() {
	int saldo;
	comando_t cmd;
    while(1) {
		comando_t next = proximoPedido();
		switch (next.operacao) {
			case CREDITAR:  
                if (creditar(next.idConta, next.valor) < 0)
                    printf("%s(%d, %d): Erro\n\n", COMANDO_CREDITAR, next.idConta, next.valor);
                else
                    printf("%s(%d, %d): OK\n\n", COMANDO_CREDITAR, next.idConta, next.valor);
            break;
			case DEBITAR:  
                if (debitar(next.idConta, next.valor) < 0)
                        printf("%s(%d, %d): Erro\n\n", COMANDO_DEBITAR, next.idConta, next.valor);
                    else
                        printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, next.idConta, next.valor);
            break;
			case LERSALDO: 
                saldo = lerSaldo(next.idConta);
                if(saldo < 0)
                    printf("%s(%d): Erro.\n\n", COMANDO_LER_SALDO, next.idConta);
			else
				printf("%s(%d): O saldo da conta é %d.\n\n", COMANDO_LER_SALDO, next.idConta, saldo);
            break;
			
			case SAIR:
				cmd = newComando(SAIR,0,0); 
				colocarPedido(cmd); /* Coloca-se no buffer das restantes trabalhadoras o comando SAIR */
				pthread_exit(EXIT_SUCCESS);
			break;
		}
		
		
	}
}
