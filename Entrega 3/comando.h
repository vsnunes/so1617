/* Tipo comando_t
 * Tipo de operacoes a realizar (em paralelo) sobre uma conta bancaria
 */

#ifndef __COMANDO_T__
#define __COMANDO_T__

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_TRABALHADORAS 3
#define CMD_BUFFER_DIM (NUM_TRABALHADORAS * 2)

#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_TRANSFERIR "transferir"
#define COMANDO_SAIR "sair"
#define COMANDO_SAIR_AGORA "agora"

pthread_mutex_t mutex, mutexComandos;
pthread_cond_t podeSimular;
sem_t haTarefas, haComandos;

int nComandos; /* numero de comandos em espera no buffer */

typedef struct
{
  int operacao;
  int idConta_origem;
  int idConta_destino;
  int valor;

} comando_t;

/* Operacoes sobre contas */
enum {	CREDITAR, DEBITAR, LERSALDO, TRANSFERIR, SAIR };

comando_t cmdbuffer[CMD_BUFFER_DIM];

int buff_write_idx, buff_read_idx;
pthread_t tarefas[NUM_TRABALHADORAS];

void initThreads(); /* Inicializa os mutexs e semaforos */
comando_t newComando(int operacao, int idConta_origem, int idConta_destino, int valor);
comando_t proximoPedido();
void colocarPedido(comando_t cmd);
void *executaPedidos();
void destroyThreads();
#endif
