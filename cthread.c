/*
 * cthread.h: arquivo de inclusão com os protótipos das funções a serem
 *            implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO: 11/09/2017
 *
 */
#ifndef __cthread__
#define __cthread__
#define	CRIACAO	0
#define	APTO	1
#define	EXEC	2
#define	BLOQ	3
#define	TERM	4

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "support.h"
#include "cdata.h"


//???
PFILA2 aptos;
PFILA2 bloqueados;
int inicializado = 0;
int novotid = 0;
ucontext_t dispatcher_ctxt;
TCB_t* main;
TCB_t* thread_atual;

//
// FUNÇÕES AUXILIARES
/*--------------------------------------------------------------------
Função: Insere um nodo na lista indicada, segundo o campo "prio" do TCB_t
	A fila deve estar ordenada (ou ter sido construída usado apenas essa funcao)
	O primeiro elemento da lista (first) é aquele com menor vamor de "prio"
Entra:	pfila -> objeto FILA2
	pnodo -> objeto a ser colocado na FILA2
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
--------------------------------------------------------------------*/
int	InsertByPrio(PFILA2 pfila, TCB_t *tcb) {
	TCB_t *tcb_it;
	tcb.state = APTO;
	if (FirstFila2(pfila)==0) {	// pfile vazia?
		do {
			tcb_it = (TCB_t *) GetAtIteratorFila2(pfila);
			if (tcb->prio < tcb_it->prio) {
				return InsertBeforeIteratorFila2(pfila, tcb);
			}
		} while (NextFila2(pfila)==0);
	}
	return AppendFila2(pfila, (void *)tcb);
}

typedef struct s_sem {
	int	count; // indica se recurso está ocupado ou não (livre > 0, ocupado = 0)
	PFILA2	fila; // ponteiro para uma fila de threads bloqueadas no semáforo
} csem_t;

void dispatcher(){
	FirstFila2(aptos);
	thread_atual = *GetAtIteratorFila2(PFILA2 pFila);
	DeleteAtIteratorFila2(PFILA2 pFila);
	thread_atual->state = 2;
	startTimer();
	setcontext(&(thread_atual->context));
}

void inicializar(){
	CreateFila2(aptos);
	CreateFila2(bloqueados);
	//
	main.tid = novotid;
	novotid++;
    	main.state = EXEC;
    	main.prio = 0;
    	cur_thread = main;
	//
	getcontext(&dispatcher_ctxt);
	dispatcher_ctxt.uc_stack.ss_sp = malloc(16384);
	dispatcher_ctxt.uc_stack.ss_size = 16384;
	dispatcher_ctxt.uc_link = &(main.context);
	makecontext(&dispatcher_ctxt, (void (*)(void))dispatcher, 0);
	//
	getcontext(&(main.context));
	// finaliza inicialização, retorna ao primeiro comando da lib chamado e volta ao "fluxo" da main
}

//
// FUNÇÕES PRINCIPAIS

int cidentify (char *name, int size){
	char id_grupo[65] = "Fernando Bock 242255\nJoao Henz xxxxxx\nLeonardo Wellausen xxxxxx\n";
	if (!inicializado){
		inicializar();
	}
	if (size >= 65){
		strncpy(name, id_grupo, size);
		return 0;
	}

	return -1;
}

int ccreate (void* (*start)(void*), void *arg, int prio){
	// garantia de inicialização da main e das filas
	if (!inicializado){
		inicializar();
	}
	// criação do Thread Control Block
	TCB_t* novotcb = (TCB_t*) malloc(sizeof(TCB_t));
    	novotcb->state = CRIACAO;
    	novotcb->prio = 0;
    	novotcb->tid = novotid;
	novotid++;
	// criação de contexto da thread
	getcontext(&(novotcb->context));
    	novotcb->context.uc_stack.ss_sp = malloc(16384);
    	novotcb->context.uc_stack.ss_size = 16384;
    	novotcb->context.uc_link = &dispatcher_ctxt;
   	makecontext(&(novotcb->context), (void (*)(void)) start, 1, arg);
	InsertByPrio(aptos, novotcb);
	
	return novotcb->tid;
}

int cyield(void){
	// garantia de inicialização da main e das filas
	if (!inicializado){
		inicializar();
	}
	TCB_t* novotcb;
	novotcb = thread_atual;
	novotcb->prio += stopTimer();
	InsertByPrio(aptos, thread_atual);
	dispatcher();
	
	return 0;
}

int cjoin(int tid){
	return -1;
}

int csem_init(csem_t *sem, int count){
	return -1;
}

int cwait(csem_t *sem){
	return -1;
}

int csignal(csem_t *sem){
	return -1;
}

#endif
