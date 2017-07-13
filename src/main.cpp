#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "stdarg.h"
#include "breg.h"
#include "shared.h"
#include "shell_RISCV.h"
#include "SHELL_MEM_RISC.h"
#include "shell_Memoria.h"
#include "memoria.h"
#include "specialkernel.h"
#include <bitset>

std::string hex_para_decimal(uint32_t hex12);
uint32_t gerainst(int n, ...);


int sc_main(int argc, char* argv[]){
	//Instanciacao dos modulos
	fetch Fetch("Fetch");
	decode Decode("Decode");
	execute Execute("Execute");

	mem Memoria_NOC("Mem_NoC");
	shell_mem_risc Mem_RISC_e_INST("Mem_Risc",0);
	ShellRISCV Shell_RISC("Shell_Risc");
	MemoriaShell Shell_Memoria("Shell_Memoria");

	breg Breg("Breg");

	sc_fifo < contexto* > e_f( 1);
	sc_fifo < contexto* > f_d( 1);
	sc_fifo < contexto* > d_e( 1);

	//Conexao das portas
	Fetch.p_mem(Mem_RISC_e_INST);
	Fetch.p_breg(Breg);
	Fetch.execute_fetch(e_f);
	Fetch.fetch_decode(f_d);

	Execute.p_breg(Breg);
	Execute.p_mem(Mem_RISC_e_INST);
	Execute.decode_execute(d_e);
	Execute.execute_fetch(e_f);

	Decode.p_breg(Breg);
	Decode.decode_execute(d_e);
	Decode.fetch_decode(f_d);

	//Conexoes com o exemplo de NoC
	sc_fifo<uint32_t> *masterShellFifo = new sc_fifo<uint32_t>(1);
	sc_fifo<uint32_t> *shellMasterFifo = new sc_fifo<uint32_t>(1);
	Mem_RISC_e_INST.shellMEM_RISC_Out(*masterShellFifo);
	Shell_RISC.shellIn(*masterShellFifo);
	Shell_RISC.shellOut(*shellMasterFifo);
	Mem_RISC_e_INST.shellMEM_RISC_In(*shellMasterFifo);


	sc_fifo<uint32_t> slaveShellFifo(1);
	sc_fifo<uint32_t> shellSlaveFifo(1);
	Memoria_NOC.memOut(shellSlaveFifo);
	Shell_Memoria.shellIn(shellSlaveFifo);
	Shell_Memoria.shellOut(slaveShellFifo);
	Memoria_NOC.memIn(slaveShellFifo);

	SpecialKernel multKernel("specialKernel");
	multKernel.connectMaster(&Shell_RISC);
	multKernel.connectSlave(&Shell_Memoria);


	//Criacao do contexto para inicializar filas bloqueantes
	contexto *dado_entrada = new contexto();

	e_f.write(dado_entrada);

	//Escrita na memoria das instrucoes
	/*ADDI $3, $2, 10*/
	Mem_RISC_e_INST.write_mem(0, gerainst(TIPO_I2_SHAMT,f3_ADDI,10, 2, 3));
    /* SLTI $1, $3, 11;*/
	Mem_RISC_e_INST.write_mem(1,gerainst(TIPO_I2_SHAMT,f3_SLTI,11, 3, 1));
//    /* XORI $3, $3, 10*/
	Mem_RISC_e_INST.write_mem(2,gerainst(TIPO_I2_SHAMT,f3_XORI,10, 3, 3));
//    /* ORI $9, $3, 31*/
	Mem_RISC_e_INST.write_mem(3,gerainst(TIPO_I2_SHAMT,f3_ORI,31, 3, 9));
//    /*ANDI $9, $9, 15*/
	Mem_RISC_e_INST.write_mem(4,gerainst(TIPO_I2_SHAMT,f3_ANDI,15, 9, 9));
//    /*BEQ $2,$9,13*/
//	Mem_RISC_e_INST.write_mem(5,gerainst(TIPO_B,13,9,2,f3_BEQ));
//    /*BNE $2,$9,13*/
//	Mem_RISC_e_INST.write_mem(6,gerainst(TIPO_B,13,9,2,f3_BNE));
//    /*LUI $26,31*/
	Mem_RISC_e_INST.write_mem(5,gerainst(TIPO_LUI,31,26));
	  /* SW $2, $9, 4 */
	Mem_RISC_e_INST.write_mem(6,gerainst(TIPO_S,4, 9, 2, f3_SW));
	//imm, rs1,func3,rd
	  /* LW $14, $0, 4 */
	Mem_RISC_e_INST.write_mem(7,gerainst(TIPO_I_REST0,4, 0, f3_LW, 14));
//    /* AUIPC $6,1*/
//	Mem_RISC_e_INST.write_mem(8,gerainst(TIPO_AUIPC,1,6));
//    /*ADD $7,$5,$9;*/
	Mem_RISC_e_INST.write_mem(8,gerainst(TIPO_R,f7_RESTO,5,9,f3_ADD_SUB,7));
//    /*SUB $11,$7,$3*/
	Mem_RISC_e_INST.write_mem(9,gerainst(TIPO_R,f7_SRA_SUB,3,7,f3_ADD_SUB,11));
//   /*SRA $5,$9,$2*/
	Mem_RISC_e_INST.write_mem(10,gerainst(TIPO_R,f7_SRA_SUB,2,9,f3_SRL_SRA,5));
//   /* SRL $17,$9,$2*/
	Mem_RISC_e_INST.write_mem(11,gerainst(TIPO_R,f7_RESTO,2,9,f3_SRL_SRA,17));
//   /*  SLL $5,$9,$3*/
	Mem_RISC_e_INST.write_mem(12, gerainst(TIPO_R,f7_RESTO,3,9,f3_SLL,5));
//    /*XOR $4,$2,$2*/
	Mem_RISC_e_INST.write_mem(13,gerainst(TIPO_R,f7_RESTO,2,2,f3_XOR,1));
//    /*AND $22,$9,$9;*/
	Mem_RISC_e_INST.write_mem(14,gerainst(TIPO_R,f7_RESTO,9,9,f3_AND,22));
//    /*OR $6,$31,$22;*/
	Mem_RISC_e_INST.write_mem(15,gerainst(TIPO_R,f7_RESTO,31,22,f3_OR,6));
//    /* JALR $23, $2, 1*/
//	//Mem_RISC_e_INST.write_mem(17,gerainst(TIPO_JALR,1, 2,0,23));
//    /*SRAI $13, $6, 2*/
	Mem_RISC_e_INST.write_mem(16,gerainst(TIPO_I2_SHAMT,f3_SRLI_SRAI,f7_SRAI,2, 6, 13));
//    /*SRLI $13, $13, 1*/
	Mem_RISC_e_INST.write_mem(17,gerainst(TIPO_I2_SHAMT,f3_SRLI_SRAI,f7_RESTO,1, 13, 13));
//    /*SLLI $13, $13, 2*/
	Mem_RISC_e_INST.write_mem(18,gerainst(TIPO_I2_SHAMT,f3_SLLI,f7_RESTO,2, 13, 13));
//    /* JAL $5,255;*/
//	//Mem_RISC_e_INST.write_mem(21,gerainst(TIPO_JAL,255,5));
//    /* SLTIU $10, $2, -2*/
	Mem_RISC_e_INST.write_mem(19,gerainst(TIPO_I2_SHAMT,f3_SLTIU,-2, 2, 10));
//    /* SLT $12, $3, $2*/
	//Mem_RISC_e_INST.write_mem(20,gerainst(TIPO_R,f7_RESTO,2,3,f3_SLT,12));
//    /* SLTU $12, $3, $9*/
	//Mem_RISC_e_INST.write_mem(21,gerainst(TIPO_R,f7_RESTO,9,3,f3_SLTU,12));
//     /*BLT $22,$4,10*/
	Mem_RISC_e_INST.write_mem(20,gerainst(TIPO_B,10,22,4,f3_BLT));
//    /*BGE $9,$5,888*/
//    //Mem_RISC_e_INST.write_mem(26,gerainst(TIPO_B,888,5,9,f3_BGE));
//    /*BLTU $2,$9,913*/
//   // Mem_RISC_e_INST.write_mem(27,gerainst(TIPO_B,913,19,2,f3_BLTU));
//    /*BGEU $29,$30,1829*/
//    //Mem_RISC_e_INST.write_mem(28,gerainst(TIPO_B,1829,30,29,f3_BGEU));

    sc_start();

    Memoria_NOC.dump_mem(0,50,'H');

	return 0;
}

// 					ORDEM
// AUIPC => IMEDIATO, depois rd (registrador destino)
// LUI => IMEDIATO, depois rd (registrador destino)
// TIPO R => FUNCT7,rs2,rs1,FUNCT3,rd.
// TIPO I => imm, rs1, funct3, rd
// TIPO S => imm(11-5), rs2, rs1, funct3, imm(4-0)
//TIPO JALR => imm, rs1,func3,rd
// TIPO I SHAMT => FUNCT7,shamt,rs1,FUNCT3,rd.
// JAL => imm(20), imm(10-1), imm(11), imm(19-12), rd
// Tipo B => imediato, rs2,rs1,funct3
uint32_t gerainst(int n, ...){
	uint32_t inst = 0;
	int imediato_JAL, imediato_S, imediato_B;
	int funct_3;
	va_list ap;

	va_start(ap, n);

	switch (n) {
	case TIPO_LUI :
		inst = (va_arg(ap, int) & 0xFFFFF) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		inst = inst | n;
//		std::cout <<std::dec<< "inst ->" << inst <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
		break;
	case TIPO_AUIPC :
		inst = (va_arg(ap, int) & 0xFFFFF) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		inst = inst | n;
//		std::cout <<std::hex<< "inst ->" << inst <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
		break;
		//Tipo I possui a mesma decodificacao
	case TIPO_I_REST0   :
		// desce pra jalr que é o mesmo
	case TIPO_JALR:
		inst = ((va_arg(ap, int) & 0xFFF) ) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 3;
		inst = ((va_arg(ap, int) & 0x7) | inst) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		inst |= n;

//		std::cout <<std::dec<< "inst ->" << inst <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
		break;
	//gerainst(TIPO_I2_SHAMT,0,10, 2, f3_ADDI, 3)
	case TIPO_I2_SHAMT :
		funct_3 = va_arg(ap, int);

		if(funct_3 == 1 || funct_3 == 5){
			// colocando f7
			inst = (va_arg(ap, int) & 0x7F) << 5;
			// colocando SHAMT
			inst = ((va_arg(ap, int) & 0x1F) | inst) << 5;
		}
		else{
			//Senao, coloca imediato de 12 bits
			inst = ((va_arg(ap, int) & 0xFFF) ) << 5;
		}
		// colocando rs1
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 3;
		// colocando funct3
		inst = ((funct_3 & 0x7) | inst) << 5;
		// colocando rd
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		// por fim opcode
		inst = inst | n;


//		std::cout << "inst ->" << std::bitset<32>(inst) <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
		break;
		// TIPO S => imm(11-5), rs2, rs1, funct3, imm(4-0)
	case TIPO_S :
		imediato_S = (va_arg(ap, int) );
		inst = ((imediato_S & 0xFE0) ) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 3;
		inst = ((va_arg(ap, int) & 0x07) | inst) << 5;
		inst = ((imediato_S & 0x1F) | inst) << 7;
		inst |= n;
//		std::cout <<std::dec<< "inst ->" << inst <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
		break;
		//gerainst(TIPO_B,10,22,4,f3_BLT)
	case TIPO_B :
		imediato_B = (va_arg(ap, int));
		// bit 12
		inst = (((imediato_B >> 11) & 0x1) ) << 6;
		// mais um pedaco imm[10:5]
		inst = (((imediato_B & 0x3F0) )) << 5;
		//rs2
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 5;
		//rs1
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 3;
		//funct3
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 4;
		//// mais um pedaco imm[4:1]
		inst = ((imediato_B & 0x01E ) | inst ) << 1;
		// mais um bit
		inst = (((imediato_B >> 10) & 0x1) | inst ) << 7;
		inst |= n;
		break;
    // JAL => imm(20), imm(10-1), imm(11), imm(19-12), rd
	case TIPO_JAL :
		imediato_JAL = (va_arg(ap, int) );
		inst = ((imediato_JAL & 0x100000) ) << 10;
		inst = (((imediato_JAL>>1)& 0x3FF) | inst) << 1;
		inst = ((imediato_JAL & 0x800) | inst) << 8;
		inst = ((imediato_JAL & 0xFF000) | inst) << 5;
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		inst |= n;

//		std::cout <<std::dec<< "inst ->" << inst <<std::endl;
//		std::cout <<std::hex<< "n ->" << n <<std::endl;
	break;
	case TIPO_R:
		// MODIFICA O F3 e F7
		// colocando f7
		inst = (va_arg(ap, int) & 0x7F) << 5;
//		std::cout << "inst"<< hex_para_decimal(inst) <<std::endl;
//		std::cout << std::hex << "Inst ->"<< inst <<std::endl;
		// colocando rs2
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 5;

		// colocando rs1
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 3;
		// colocando funct3
		inst = ((va_arg(ap, int) & 0x7) | inst) << 5;
		// colocando rd
		inst = ((va_arg(ap, int) & 0x1F) | inst) << 7;
		// por fim opcode
		inst = inst | n;
//		std::cout << "inst"<< hex_para_decimal(inst) <<std::endl;
//		std::cout << std::hex << "Inst ->"<< inst <<std::endl;
		break;
	default:
		break;
	}
	return inst;
}

std::string hex_para_decimal(uint32_t hex12){
	std::string sRet="";
	std::string x;
	std::stringstream ss;
	ss <<std::hex<<hex12;
	x= ss.str();


	for(unsigned int i=0;i<x.length();++i){
		switch(x[i]){
		case '0':sRet.append("0000");break;
		case '1':sRet.append("0001");break;
		case '2':sRet.append("0010");break;
		case '3':sRet.append("0011");break;
		case '4':sRet.append("0100");break;
		case '5':sRet.append("0101");break;
		case '6':sRet.append("0110");break;
		case '7':sRet.append("0111");break;
		case '8':sRet.append("1000");break;
		case '9':sRet.append("1001");break;
		case 'A':sRet.append("1010");break;
		case 'a':sRet.append("1010");break;
		case 'B':sRet.append("1011");break;
		case 'b':sRet.append("1011");break;
		case 'C':sRet.append("1100");break;
		case 'c':sRet.append("1100");break;
		case 'D':sRet.append("1101");break;
		case 'd':sRet.append("1101");break;
		case 'E':sRet.append("1110");break;
		case 'e':sRet.append("1110");break;
		case 'F':sRet.append("1111");break;
		case 'f':sRet.append("1111");break;

		}

	}
	return sRet;
}
