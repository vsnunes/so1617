/*
// Operações sobre contas, versao 1
// Sistemas Operativos, DEI/IST/ULisboa 2016-17
*/

#ifndef CONTAS_H
#define CONTAS_H
#include <pthread.h>

#define NUM_CONTAS 10
#define TAXAJURO 0.1
#define CUSTOMANUTENCAO 1

#define ATRASO 1

pthread_mutex_t mutexContas[NUM_CONTAS];

void inicializarContas();
int contaExiste(int idConta);
int debitar(int idConta, int valor);
int creditar(int idConta, int valor);

/* Funcoes sem trincos logicos. Apenas devem ser utilizadas numa operacao transferir. */
int creditar_tr(int idConta, int valor);
int debitar_tr(int idConta, int valor);

int lerSaldo(int idConta);
int transferir(int idOrigem, int idDestino, int valor);
void simular(int numAnos);
void stop(int sig);
void destroyContas();

#endif
