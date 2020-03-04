#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define atrasar() sleep(ATRASO)


		     
int contasSaldos[NUM_CONTAS];

/* flag de sinalizacao do signal */
unsigned int f_stop = 0;

int contaExiste(int idConta) {
  return (idConta > 0 && idConta <= NUM_CONTAS);
}

void inicializarContas() {
  int i;
  for (i=0; i<NUM_CONTAS; i++)
	  {
		pthread_mutex_init(&mutexContas[i], NULL);
		contasSaldos[i] = 0;
	  }
}

int debitar(int idConta, int valor) {
  atrasar();
  pthread_mutex_lock(&mutexContas[idConta]);
  if (!contaExiste(idConta))
    {	pthread_mutex_unlock(&mutexContas[idConta]);
		return -1;
	}
  if (contasSaldos[idConta - 1] < valor)
    {	pthread_mutex_unlock(&mutexContas[idConta]);
		return -1;
	}
  atrasar();
  contasSaldos[idConta - 1] -= valor;
  pthread_mutex_unlock(&mutexContas[idConta]);
  return 0;
}

int creditar(int idConta, int valor) {
  atrasar();
  pthread_mutex_lock(&mutexContas[idConta]);
  if (!contaExiste(idConta))
	{	pthread_mutex_unlock(&mutexContas[idConta]);
		return -1;
	}
  contasSaldos[idConta - 1] += valor;
  pthread_mutex_unlock(&mutexContas[idConta]);
  return 0;
}

int lerSaldo(int idConta) {
  atrasar();
  
  if (!contaExiste(idConta))
    return -1;
  return contasSaldos[idConta - 1];
}

void stop(int sig)
{
	f_stop = 1;
}

void simular(int numAnos) {
    int ano, i, credito, saldo;
	
    for (ano = 0; ano <= numAnos; ano++)
        {
            printf("\nSIMULACAO: Ano %d\n=================\n", ano);
            for (i=1; i<=NUM_CONTAS; i++)
            {
                saldo = lerSaldo(i);
                printf("Conta %d, Saldo %d\n", i, saldo);
                credito = saldo * TAXAJURO - CUSTOMANUTENCAO;
                if (credito > 0)
                    creditar(i, credito);
            }
	    if (f_stop) {
                printf("Simulacao terminada por signal\n");
                return;
            }
        }
	
}
