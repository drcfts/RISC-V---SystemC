/*
 * memoria_instrucoes.h
 *
 *  Created on: 8 de jul de 2017
 *      Author: drcfts
 */

#ifndef SRC_MEMORIA_INSTRUCOES_H_
#define SRC_MEMORIA_INSTRUCOES_H_

#include <systemc.h>
#include "mem_if.h"
#include "shared.h"

using namespace std;

//Memoria de instrucoes que implementa a interface
//Criado para facilidade de conexao (fetch do processador)
//Por ser de instrucoes, n implementa saves e loads
struct mem_inst : public sc_module, public mem_if {

	int32_t read(const uint32_t adress);

	int32_t lw(const unsigned address, int32_t constante);

	int32_t lh(const unsigned address, int32_t constante);

	int32_t lhu(const unsigned address, int32_t constante);

	int32_t lb(const unsigned address, int32_t constante);

	int32_t lbu(const unsigned address, int32_t constante);

	void sw(const unsigned address, int32_t constante, int32_t dado);

	void sh(const unsigned address, int32_t constante, int32_t dado);

	void sb(const unsigned address, int32_t constante, int32_t dado);

	void write_mem(const unsigned address, uint32_t data);

	void dump_mem(int inicio, int fim, char formato);

	SC_CTOR(mem_inst){
		mem_ptr = new int32_t[MAX_MEM];
	}

private:
	int32_t *mem_ptr;
};

/****************************
 * ENDEREÇO VAI TER 32 BITS *
 ****************************
 * 	32 bits -------------------
 *	2 bits-- de dados(coluna) -
 *  20 bits-- tag		      -
 *  10 bits indica a linha    -
 *  ---------------------------
 *  O TAMANHO DA CACHE VAI SER 1024 = 2¹⁰
 * -------------------------
 * Matriz da memória de cache
 * bit de validade - 0 ou 1  -> indicando se foi preenchida ou nao
 * Tag -> uma parte do endereço para verificar se é aquele endereço mesmo
 * Dados -> é onde vai conter os dados como a memória é de 4K, vai ter um vetor
 * 4dados
 * **************************************************************
 *
 * typedef struct{
	bool validade;
	uint32_t tag_cache;
	int dados[4];
	uint32_t palavra_coluna;
	uint32_t linha_endereco_cache;

	}mem_cache;
*
*
*decode_mem(endereco){
 	 palavra_coluna = (endereco) & 0x3;
	 tag_cache = (endereco >> 12) & 0xFFFFF;
	 linha_endereco_cache = (endereco >> 2) & 0x3FF;
 }
*/



#endif /* SRC_MEMORIA_INSTRUCOES_H_ */
