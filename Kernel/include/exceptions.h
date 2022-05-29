#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <video_driver.h>
#include <syscalls.h>
#include <stdint.h>
#include <scheduler.h>

#define COUNT_REGS (18)

void zero_division();
void invalid_opcode();

uint64_t* get_registers(void);
void print_registers();

typedef void (* exception) ();

#endif