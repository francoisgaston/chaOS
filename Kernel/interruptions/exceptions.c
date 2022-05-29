#include <exceptions.h>
#include <scheduler.h>

exception exceptions[] = {zero_division, 0,0,0,0,0, invalid_opcode};              // Arreglo de punteros a funcion de excepciones

static const char * Names[] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

void exceptionDispatcher(int exception) {
	exceptions[exception]();
    //terminate_process();//Matamos al programa que lanzo la excepcion
    terminate_process();
	return;
}

//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al realizarse una division por cero (exceptionID = 0)
//-----------------------------------------------------------------------
void zero_division() {
	write_handler("EXCEPCION generada: Division por cero\n", RED);
	print_registers();
}

/*
programa a llamar desde el Userland cuando quiero la excepcion
void zero_division() {
	zero_division_exc();
}

Funcion en ASM que genera la excepcion
zero_division_exc:
	push rbp
	mov rbp, rsp

	int 0h
	
	mov rsp, rbp
	pop rbp
	ret
*/


//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al utilizar un operador invalido (exceptionID = 6)
//-----------------------------------------------------------------------
void invalid_opcode() {
	write_handler("EXCEPCION generada: Invalid opcode\n", RED);
	print_registers();
}

/*
programa a llamar desde el Userland cuando quiero la exepcion
void invalid_opcode() {
	invalid_opcode_exc();
}

Funcion en ASM que genera la excepcion
invalid_opcode_exc:
	push rbp
	mov rbp, rsp

	int 6h
	
	mov rsp, rbp
	pop rbp
	ret
*/

//-----------------------------------------------------------------------
// print_registers: Imprime el estado de los registros
//-----------------------------------------------------------------------
// Argumentos:
//  void
//-----------------------------------------------------------------------
void print_registers(){
	write_handler("Registros al momento de la excepcion: \n", RED);
	// Pongo en reg los valores de los registros
	uint64_t * reg = get_registers();
    char reg_str[20];
    // Al llamar a write_handler, se consulta la posicion del proceso que se encuentra corriendo actualmente
    // y lo imprime en dicha posicion
    reg[RSP] = reg[ACTUAL_RSP]; //Los otros se guardan para el scheduler
    reg[RFLAGS] = reg[ACTUAL_RFLAGS]; //Los otros se guardan para el scheduler
	for(int i=0; i<=RFLAGS; i++){
		write_handler(Names[i], RED);
		to_hex(reg_str, reg[i]);
        write_handler(reg_str,RED);
        write_handler("\n", RED);
	}
}