#include <syscalls.h>
#include <video_driver.h>
#include <keyboard.h>
#include <scheduler.h>
#include <queue.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <interrupts.h>
//TODO: sacar
#include "../include/scheduler.h"
#include "../include/syscalls.h"
#include "../include/interrupts.h"
//#define REGISTERS_COUNT 1

//----------------------------------------------------------------------
// read_handler: handler para leer un caracter del teclado
//----------------------------------------------------------------------
// Argumentos:
//  str: char* donde se debe guardar el caracter
//----------------------------------------------------------------------
// Retorno:
//  0 si no hay caracteres para ller
//  1 si hay caracteres para leer
//----------------------------------------------------------------------
int32_t read_handler(char* str){
    extern queue_t queue;
    if (is_empty(&queue)){
        *str = '\0'; //no hay caracteres para imprimir
        return 0;
    }          // No hay para leer (el puntero de escritura y lectura estan en la misma posicion), por lo que se devuelve 0
    *str = dequeue(&queue);           // Si hay para leer, guardamos el siguiente caracter en la primera posicion del string
    str[1] = '\0';
    return 1;                       // Devolvemos la cantidad de caracteres leidos
}
//----------------------------------------------------------------------
// write_handler: imprime un string en la pantalla del proceso que lo llama
//----------------------------------------------------------------------
// Argumentos:
//  str: el string que se desea imprimor
//  format: el color de la letra que se desea usar
//----------------------------------------------------------------------
int32_t write_handler(const char * str, formatType format){
//    if(process_array_is_empty()){
//        //No se cargaron procesos, por default imprime en LEFT
//        positionType position = ALL;
//        print(str, format, position);
//        return 0;
//    }
    print(str, format, ALL);        // Imprime por pantalla
    return 0;
}

//----------------------------------------------------------------------
// ecec_handler: agrega procesos para que se ejecuten
//----------------------------------------------------------------------
// Argumentos:
//  cant: la cantidad de procesos que se desea ejecutar (1 si es en toda la pantalla, 2 si es en pantalal dividida)
//  programs: arreglo de program_t con los programas que se desean ejecutar
//----------------------------------------------------------------------
// Si cant==1, programs debe tener al menos 1 elemento, y se ejecuta el primero
// Si cant==2, programs debe tener al menos 2 elementos, el primero se ejecuta a la izquierda y el
//  segundo a la derecha
//----------------------------------------------------------------------
// Retorno:
//  -1 si cant no es valido
//  0 si logro ejecutar a los procesos
//----------------------------------------------------------------------
//uint8_t exec_handler(uint8_t cant, const executable_t* program){//Recibe un vector de program_t
//    if(cant == 0 || cant >= 2){
//        return -1;
//    }
////    else if(cant==1){
////        clear(ALL);
////        add_full_process(programs[0]);
//        //add_process(programs[0],ALL);
////    }else{
////        clear(ALL);//Limpio la pantalla y reinicio las posiciones de ambas subpantallas
////        add_two_processes(programs[0],programs[1]);
////        print_lines();
////    }
//    return 0;
//}
//----------------------------------------------------------------------
// exit_handler: termina el proceso que lo llama
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Retorno
//  -1 si no hay procesos para terminar
//  0 si logra terminar el proceso
//----------------------------------------------------------------------
//int32_t exit_handler(){
//    return terminate_process(1);
//}
//----------------------------------------------------------------------
// time_handler: obtiene la unidad del tiempo que se pide
//----------------------------------------------------------------------
// Argumentos:
//  time_unit: la unidad que se desea buscar (sec, min,....)
//----------------------------------------------------------------------
// Retorno:
//  El valor pedido o -1 si no es un parametro correcto
//----------------------------------------------------------------------
uint8_t time_handler(timeType time_unit){
    if(time_unit != SEC && time_unit != MIN && time_unit != HOUR && time_unit != DAY_WEEK &&
        time_unit != DAY_MONTH && time_unit != MONTH && time_unit != YEAR){
        return -1;
    }
    return get_time(time_unit);
}
//----------------------------------------------------------------------
// mem_handler: guarda 32 bytes a partir de una direccion que se pasa en un vector
//----------------------------------------------------------------------
// Argumentos:
//  init_dir: la direccion inicial desde donde se quiere leer
//  arr: arreglo donde se escriben los valores, debe tener al menos 32 posiciones
//----------------------------------------------------------------------
// Es la funcion que utliza el programa printmem. Completa con 0's si en algun momento
// se pasa del maximo
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de posiciones que se logro almacenar
//----------------------------------------------------------------------
int32_t mem_handler(uint64_t init_dir, uint8_t * arr){
    uint8_t i = 0;
    // Empiezo a completar el arreglo, siempre y cuando la direccion consultada sea menor a la ultima
    // Asi se evita overflow
    for(; (i < MAX_ARRAY_SIZE) && (init_dir + i < MAX_MEM_ADDRESS); i++){
//        arr[i] = get_data(init_dir + i);
        arr[i] = *((uint8_t*)init_dir + i);//char* para que +i avance de a 1
    }
    if(init_dir + i == MAX_MEM_ADDRESS){
//        arr[i++] = get_data(MAX_MEM_ADDRESS);
        arr[i++] = *((uint8_t*)MAX_MEM_ADDRESS);
    }
    // Si aun quedan espacios libres (i < 32), se completa con NULL
    for(int j = i; j < MAX_ARRAY_SIZE; j++){
        arr[j] = 0;
    }
    // En i se almacenan la cantidad real de datos que se pudieron almacenar
    return i;
}
//----------------------------------------------------------------------
// tick_handler: obtiene la cantidad de ticks que hizo el timer tick
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Retorno
//  La cantidad de ticks
//----------------------------------------------------------------------
uint64_t tick_handler(void){
    return ticks_elapsed();
}
//----------------------------------------------------------------------
// blink_handler: hace que el fondo del cursor de la pantalla cambie segun lo especificado en video_blink
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
int32_t blink_handler(void){
    video_blink(ALL);
    return 0;
}
//----------------------------------------------------------------------
// regs_handler: devuelve los valores de los arreglos cuando se utilizo la combinacion Control+s
//----------------------------------------------------------------------
// Argumentos:
//  regs_arr: un arreglo de al menos 18 uint64_t donde se almacenan los valores
//----------------------------------------------------------------------
uint8_t regs_handler(uint64_t * regs_arr){
    extern uint8_t regs_saved;
    uint64_t * inforeg_context = get_inforeg_context();
    for(uint8_t i = 0; i < REGISTERS_COUNT; i++){
        regs_arr[i] = inforeg_context[i];
    }
    return regs_saved;
}
//----------------------------------------------------------------------
// clear_handler: Limpiar la terminal de comandos
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
int32_t clear_handler(void){
    clear(ALL);
    return 0;
}
int32_t terminate_handler(uint64_t pid){
    int status = 0;
    if((status = terminate_process(pid))==-1){
        return -1;
    }
    //No tengo problema con haber liberado el stack
    //Pues no es un kernel desalojable (no me interrumpen aca)
    _int20();//por si me termino a mi mismo
    return status;
}
int32_t block_process_handler(uint64_t pid){
    int status = 0;
    if((status = block_process(pid))==-1){
        return -1;
    }
    _int20();//llama al scheduler para ver como sigue, es para el caso donde se bloquea a si mismo
    return status;
}
//hace _int20() adentro de wait, para que devuelva el codigo cuando ya esta disponible el que fue esperado
int32_t waitpid_handler(uint64_t pid){
    int status = 0;
    if((status = waitPid(pid))==-1){
        return -1;
    }
    return status;
}
int32_t yield_handler(){
    int status = 0;
    if((status = yield_current_process())==-1){
        return -1;
    }
    _int20();
    return status;
}
int32_t unblock_process_handler(uint64_t pid){
    int status = 0;
    if((status = unblock_process(pid))==-1){
        return -1;
    }
    return status;
}
int32_t exec_handler(executable_t* executable){
    if(executable==NULL){
        return -1;
    }
    return create_process(executable);
}
int32_t nice_handler(uint64_t pid, uint8_t priority){
    return change_process_priority(pid,priority);
}
uint64_t getpid_handler(){
    return get_current_pid();
}
int32_t exit_handler(){
    return terminate_handler(get_current_pid());
}
void* syscalls[]={&read_handler,&write_handler,&exec_handler,&exit_handler,&time_handler,&mem_handler,&tick_handler,&blink_handler,&regs_handler,&clear_handler,
                  &block_process_handler, &waitpid_handler,&yield_handler, &unblock_process_handler,&terminate_handler, &nice_handler, &getpid_handler};
void* syscall_dispatcher(uint64_t syscall_num){
    if(syscall_num<0 || syscall_num>=17){
        return NULL;
    }
    return syscalls[syscall_num];
}