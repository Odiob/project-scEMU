#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator8080.h"



void InitEmulator(int d, unsigned char *buffer, State *state, Ports *ports)
{
	state->mem = malloc(64000);
	state->pc = 0;
	state->int_enable = 0;
	state->elapsed_cycles = 0;
	int j;
	for(j=0; j<d; j++)
	{
		state->mem[j]=buffer[j];
	}
	for(j=0; j<7; j++) // change if adding more output ports
	{
		ports->out_activation[j] = 0;
	}
	return;
}

void GenerateInterrupt(State *state, uint8_t var)
{
	state->mem[state->sp-1] = (state->pc >> 8) & 0xff;
	state->mem[state->sp-2] = state->pc & 0xff;
	state->sp -= 2;
	state->pc = var*8;
	state->int_enable = 0;
	return;
}

void UnimplementedInstruction()
{
	printf("ERROR: found an unimplemented instruction");
	exit(1);
}

void flags(uint8_t var, struct Flags *f)
{
	if (var != 0) 
		f->z = 0;
	else
		f->z = 1;
	
	if ((var & 0x80) == 0)
		f->s = 0;
	else
		f->s = 1;
	
	int i, sum=0;
	for(i=0;i<8;i++)
		sum += (var >> i) & 0x01;
	if(sum%2 == 0)
		f->p = 1;
	else
		f->p = 0;
	return;
}

int translator(State *state, Ports *ports)
{
	uint8_t temp_short;
	uint16_t temp;
	uint32_t temp_long;
	unsigned char *opcode = &state->mem[state->pc];
	switch(*opcode)
	{
		case 0x00: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x01: // LXI B
			state->b = opcode[2];
			state->c = opcode[1];
			state->elapsed_cycles += 10;
			state->pc += 3;
			break;
		case 0x02:
			printf("STAX B");
			UnimplementedInstruction();
			break;
		case 0x03: // INX B 
			temp = (state->b << 8) + state->c;
			temp++;
			state->b = (temp >> 8) & 0xff;
			state->c = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x04: // INR B
			temp = state->b + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->b & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->b = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x05: // DCR B
			temp = (uint16_t)state->b - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->b & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->b = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x06: // MVI B
			state->b = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x07: // RLC
			state->flags.cy = (state->a >> 7) & 0x01;
			state->a = ((state->a << 1) & 0xFE) + state->flags.cy;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x08: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x09: // DAD B
			temp_long = (state->h << 8) + state->l;
			temp = (state->b << 8) + state->c;
			temp_long = temp_long + (uint32_t)temp;
			state->h = (temp_long >> 8) & 0xff;
			state->l = temp_long & 0xff;
			if((temp_long & 0x00010000) == 0)
				state->flags.cy = 0;
			else
				state->flags.cy = 1;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x0A: // LDAX B
			temp = (state->b << 8) + state->c;
			state->a = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x0B: // DCX B
			temp = ((state->b << 8) & 0xFF00) + (state->c & 0x00FF) - 1;
			state->b = (temp >> 8) & 0xFF;
			state->c = temp & 0xFF;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x0C: // INR C
			temp = state->c + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->c & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->c = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x0D: // DCR C
			temp = (uint16_t)state->c - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->c & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->c = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x0E: // MVI C
			state->c = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x0F: // RRC
			state->flags.cy = state->a & 0x01;
			temp_short = ((state->a >> 1) & 0x7F) + ((state->a << 7) & 0x80);
			state->a = temp_short;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x10: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x11: // LXI D
			state->d = opcode[2];
			state->e = opcode[1];
			state->elapsed_cycles += 10;
			state->pc += 3;
			break;
		case 0x12: // STAX D
			state->mem[((state->d << 8) & 0xff00) + (state->e & 0x00ff)] = state->a;
			state->elapsed_cycles += 7;
			state->pc++;;
			break;
		case 0x13: // INX D
			temp = (state->d << 8) + state->e;
			temp++;
			state->d = (temp >> 8) & 0xff;
			state->e = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x14: // INR D
			temp = state->d + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->d & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->d = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x15: // DCR D
			temp = (uint16_t)state->d - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->d & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->d = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x16: // MVI D
			state->d = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x17: // RAL
			temp_short = ((state->a << 1) & 0xFE) + state->flags.cy;
			state->flags.cy = (state->a >> 7) & 0x01;
			state->a = temp_short;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x18: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x19: // DAD D
			temp_long = (state->h << 8) + state->l;
			temp = (state->d << 8) + state->e;
			temp_long = temp_long + (uint32_t)temp;
			state->h = (temp_long >> 8) & 0xff;
			state->l = temp_long & 0xff;
			if((temp_long & 0x00010000) == 0)
				state->flags.cy = 0;
			else
				state->flags.cy = 1;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x1A: // LDAX D
			temp = (state->d << 8) + state->e;
			state->a = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x1B: // DCX D
			temp = ((state->d << 8) & 0xFF00) + (state->e & 0x00FF) - 1;
			state->d = (temp >> 8) & 0xFF;
			state->e = temp & 0xFF;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x1C: // INR E
			temp = state->e + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->e & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->e = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x1D: // DCR E
			temp = (uint16_t)state->e - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->e & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->e = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x1E: // MVI E
			state->e = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x1F: // RAR
			temp_short = ((state->a >> 1) & 0x7F) + ((state->flags.cy << 7) & 0x80);
			state->flags.cy = state->a & 0x01;
			state->a = temp_short;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x20: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x21: // LXI H
			state->h = opcode[2];
			state->l = opcode[1];
			state->elapsed_cycles += 10;
			state->pc += 3;
			break;
		case 0x22: // SHLD
			state->mem[((opcode[2] << 8) & 0xFF00) + opcode[1]] = state->l;
			state->mem[((opcode[2] << 8) & 0xFF00) + opcode[1] + 1] = state->h;
			state->elapsed_cycles += 16;
			state->pc += 3;
			break;
		case 0x23: // INX H
			temp = (state->h << 8) + state->l;
			temp++;
			state->h = (temp >> 8)&0xff;
			state->l = temp&0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x24: // INR H
			temp = state->h + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->h & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->h = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x25: // DCR H
			temp = (uint16_t)state->h - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->h & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->h = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x26: // MVI H
			state->h = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x27: // DAA
			temp = state->a & 0xFF;
			if(((state->a & 0x0F) > 9) || (state->flags.ac == 1))
				temp = (uint16_t)(state->a + 6);
			if((((temp >> 4) & 0x0F) > 9) || (state->flags.cy == 1))
				temp = temp + 0x60;
			flags((uint8_t)temp,&(state->flags));
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if(((state->a & 0x0F) > 9) || (state->flags.ac == 1))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x28: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x29: // DAD H
			temp = (state->h << 8) + state->l;
			temp_long = (uint32_t)temp + (uint32_t)temp;
			state->h = (temp_long >> 8) & 0xff;
			state->l = temp_long & 0xff;
			if((temp_long & 0x00010000) == 0)
				state->flags.cy = 0;
			else
				state->flags.cy = 1;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x2A: // LHLD
			state->l = state->mem[((opcode[2] << 8) & 0xFF00) + opcode[1]];
			state->h = state->mem[((opcode[2] << 8) & 0xFF00) + opcode[1] + 1];
			state->elapsed_cycles += 16;
			state->pc += 3;
			break;
		case 0x2B: // DCX H
			temp = ((state->h << 8) & 0xFF00) + (state->l & 0x00FF) - 1;
			state->h = (temp >> 8) & 0xFF;
			state->l = temp & 0xFF;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x2C: // INR L
			temp = state->l + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->l & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->l = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x2D: // DCR L
			temp = (uint16_t)state->l - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->l & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->l = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x2E: // MVI L
			state->l = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x2F: // CMA
			temp_short = ~ state->a;
			state->a = temp_short;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x30: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x31: // LXI SP
			state->sp = ((opcode[2] << 8) & 0xFF00) + opcode[1];
			state->elapsed_cycles += 10;
			state->pc += 3;
			break;
		case 0x32: // STA
			temp = (opcode[2] << 8) + opcode[1];
			state->mem[temp] = state->a;
			state->elapsed_cycles += 13;
			state->pc += 3;
			break;
		case 0x33:
			printf("INX SP");
			UnimplementedInstruction();
			break;
		case 0x34: // INR M
			temp = (uint16_t)(state->mem[((state->h << 8) & 0xFF00) + (state->l & 0x00FF)] + 1);
			flags((uint8_t)temp,&(state->flags));
			if((state->mem[((state->h << 8) & 0xFF00) + (state->l & 0x00FF)] & 0x0f) != 0x0f)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->mem[((state->h << 8) & 0xFF00) + (state->l & 0x00FF)] = temp & 0xff;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x35: // DCR M
			temp_short = state->mem[(state->h << 8) + state->l];
			temp = (uint16_t)temp_short - 1;
			flags((uint8_t)temp,&(state->flags));
			if((temp_short & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->mem[(state->h << 8) + state->l] = temp & 0xff;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x36: // MVI M
			temp = (state->h << 8) + state->l;
			state->mem[temp] = opcode[1];
			state->elapsed_cycles += 10;
			state->pc += 2; 
			break;
		case 0x37: // STC
			state->flags.cy = 1;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x38: // NOP
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x39: // DAD SP
			temp = (state->h << 8) + state->l;
			temp_long = (uint32_t)temp + (uint32_t)state->sp;
			state->h = (temp_long >> 8) & 0xff;
			state->l = temp_long & 0xff;
			if((temp_long & 0x00010000) == 0)
				state->flags.cy = 0;
			else
				state->flags.cy = 1;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0x3A: // LDA
			temp = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			state->a = state->mem[temp];
			state->elapsed_cycles += 13;
			state->pc += 3;
			break;
		case 0x3B:
			printf("DCX SP");
			UnimplementedInstruction();
			break;
		case 0x3C: // INR A
			temp = state->a + 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->a & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->a = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x3D: // DCR A
			temp = (uint16_t)state->a - 1;
			flags((uint8_t)temp,&(state->flags));
			if((state->a & 0x0f) != 0)
				state->flags.ac = 0;
			else
				state->flags.ac = 1;
			state->a = temp & 0xff;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x3E: // MVI A
			state->a = opcode[1];
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0x3F:
			printf("CMC");
			UnimplementedInstruction();
			break;
		case 0x40: // MOV B,B
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x41: // MOV B,C
			state->b = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x42: // MOV B,D
			state->b = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x43: // MOV B,E
			state->b = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x44: // MOV B,H
			state->b = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x45: // MOV B,L
			state->b = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x46: // MOV B,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->b = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x47: // MOV B,A
			state->b = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x48: // MOV C,B
			state->c = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x49: // MOV C,C
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x4A: // MOV C,D
			state->c = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x4B: // MOV C,E
			state->c = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x4C: // MOV C,H
			state->c = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x4D: // MOV C,L
			state->c = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x4E: // MOV C,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->c = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x4F: // MOV C,A
			state->c = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x50: // MOV D,B
			state->d = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x51: // MOV D,C
			state->d = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x52: // MOV D,D
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x53: // MOV D,E
			state->d = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x54: // MOV D,H
			state->d = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x55: // MOV D,L
			state->d = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x56: // MOV D,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->d = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x57: // MOV D,A
			state->d = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x58: // MOV E,B
			state->e = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x59: // MOV E,C
			state->e = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x5A: // MOV E,D
			state->e = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x5B: // MOV E,E
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x5C: // MOV E,H
			state->e = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x5D: // MOV E,L
			state->e = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x5E: // MOV E,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->e = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x5F: // MOV E,A
			state->e = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x60: // MOV H,B
			state->h = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x61: // MOV H,C
			state->h = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x62: // MOV H,D
			state->h = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x63: // MOV H,E
			state->h = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x64: // MOV H,H
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x65: // MOV H,L
			state->h = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x66: // MOV H,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->h = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x67: // MOV H,A
			state->h = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x68: // MOV L,B
			state->l = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x69: // MOV L,C
			state->l = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x6A: // MOV L,D
			state->l = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x6B: // MOV L,E
			state->l = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x6C: // MOV L,H
			state->l = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x6D: // MOV L,L
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x6E: // MOV L,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->l = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x6F: // MOV L,A
			state->l = state->a;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x70: // MOV M,B
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->b;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x71: // MOV M,C
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->c;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x72: // MOV M,D
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->d;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x73: // MOV M,E
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->e;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x74: // MOV M,H
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->h;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x75: // MOV M,L
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->l;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x76:
			printf("HLT");
			UnimplementedInstruction();
			break;
		case 0x77: // MOV M,A
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->mem[temp] = state->a;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x78: // MOV A,B
			state->a = state->b;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x79: // MOV A,C
			state->a = state->c;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x7A: // MOV A,D
			state->a = state->d;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x7B: // MOV A,E
			state->a = state->e;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x7C: // MOV A,H
			state->a = state->h;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x7D: // MOV A,L
			state->a = state->l;
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x7E: // MOV A,M
			temp = ((state->h << 8) & 0xff00) + (state->l & 0x00ff);
			state->a = state->mem[temp];
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x7F: // MOV A,A
			state->elapsed_cycles += 5;
			state->pc++;
			break;
		case 0x80: // ADD B
			temp = (uint16_t)(state->a + state->b);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->b & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x81: // ADD C
			temp = (uint16_t)(state->a + state->c);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->c & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x82: // ADD D
			temp = (uint16_t)(state->a + state->d);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->d & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x83: // ADD E
			temp = (uint16_t)(state->a + state->e);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->e & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x84: // ADD H
			temp = (uint16_t)(state->a + state->h);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->h & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x85: // ADD L
			temp = (uint16_t)(state->a + state->l);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->l & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x86: // ADD M
			temp = (uint16_t)(state->a + state->mem[((state->h << 8) & 0xFF00) + (state->l & 0x00FF)]);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->mem[((state->h << 8) & 0xFF00) + (state->l & 0x00FF)] & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0x87: // ADD A
			temp = (uint16_t)(state->a + state->a);
			flags((uint8_t)temp,&(state->flags));			
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0F) + (state->a & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x88: // ADC B
			temp = state->a + state->b + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->b + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x89: // ADC C
			temp = state->a + state->c + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->c + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x8A: // ADC D
			temp = state->a + state->d + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->d + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x8B: // ADC E
			temp = state->a + state->e + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->e + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x8C: // ADC H
			temp = state->a + state->h + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->h + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x8D: // ADC L
			temp = state->a + state->l + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->l + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x8E:
			printf("ADC M");
			UnimplementedInstruction();
			break;
		case 0x8F: // ADC A
			temp = state->a + state->a + state->flags.cy;
			flags((uint8_t)temp,&(state->flags));
			temp_short = (state->a & 0x0F) + ((state->a + state->flags.cy) & 0x0F);
			if(((temp_short >> 4) & 0x01) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(((temp >> 8) & 0x01) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x90: // SUB B
			temp = (uint16_t)(state->a - state->b);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->b)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->b & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x91: // SUB C
			temp = (uint16_t)(state->a - state->c);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->c)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->c & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x92: // SUB D
			temp = (uint16_t)(state->a - state->d);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->d)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->d & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x93: // SUB E
			temp = (uint16_t)(state->a - state->e);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->e)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->e & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x94: // SUB H
			temp = (uint16_t)(state->a - state->h);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->h)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->h & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x95: // SUB L
			temp = (uint16_t)(state->a - state->l);
			flags((uint8_t)temp,&(state->flags));			
			if(state->a < state->l)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->l & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp & 0xFF;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x96:
			printf("SUB M");
			UnimplementedInstruction();
			break;
		case 0x97: // SUB A
			state->a = 0;
			flags(state->a,&(state->flags));			
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0x98:
			printf("SBB B");
			UnimplementedInstruction();
			break;
		case 0x99:
			printf("SBB C");
			UnimplementedInstruction();
			break;
		case 0x9A:
			printf("SBB D");
			UnimplementedInstruction();
			break;
		case 0x9B:
			printf("SBB E");
			UnimplementedInstruction();
			break;
		case 0x9C:
			printf("SBB H");
			UnimplementedInstruction();
			break;
		case 0x9D:
			printf("SBB L");
			UnimplementedInstruction();
			break;
		case 0x9E:
			printf("SBB M");
			UnimplementedInstruction();
			break;
		case 0x9F:
			printf("SBB A");
			UnimplementedInstruction();
			break;
		case 0xA0: // ANA B
			temp_short = state->a & state->b;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA1: // ANA C
			temp_short = state->a & state->c;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA2: // ANA D
			temp_short = state->a & state->d;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA3: // ANA E
			temp_short = state->a & state->e;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA4: // ANA H
			temp_short = state->a & state->h;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA5: // ANA L
			temp_short = state->a & state->l;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA6: // ANA M
			temp_short = state->a & state->mem[((state->h << 8) & 0xff00) + (state->l & 0x00ff)];
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0xA7: // ANA A
			temp_short = state->a & state->a;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0; // non-accurate behaviour - see note on AC flag
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA8: // XRA B
			temp_short = state->a ^ state->b;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xA9: // XRA C
			temp_short = state->a ^ state->c;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xAA: // XRA D
			temp_short = state->a ^ state->d;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xAB: // XRA E
			temp_short = state->a ^ state->e;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xAC: // XRA H
			temp_short = state->a ^ state->h;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xAD: // XRA L
			temp_short = state->a ^ state->l;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xAE:
			printf("XRA M");
			UnimplementedInstruction();
			break;
		case 0xAF: // XRA A
			temp_short = state->a ^ state->a;
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB0: // ORA B
			temp_short = state->a | state->b;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB1: // ORA C
			temp_short = state->a | state->c;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB2: // ORA D
			temp_short = state->a | state->d;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB3: // ORA E
			temp_short = state->a | state->e;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB4: // ORA H
			temp_short = state->a | state->h;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB5: // ORA L
			temp_short = state->a | state->l;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB6: // ORA M
			temp_short = state->a | state->mem[((state->h << 8) & 0xff00) + (state->l & 0x00ff)];
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 7;
			state->pc++;
			break;
		case 0xB7: // ORA A
			temp_short = state->a | state->a;
			flags(temp_short,&(state->flags)); 
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB8: // CMP B
			temp_short = state->a - state->b;
			flags(temp_short,&(state->flags));
			if(state->a < state->b)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->b & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xB9: // CMP C
			temp_short = state->a - state->c;
			flags(temp_short,&(state->flags));
			if(state->a < state->c)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->c & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBA: // CMP D
			temp_short = state->a - state->d;
			flags(temp_short,&(state->flags));
			if(state->a < state->d)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->d & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBB: // CMP E
			temp_short = state->a - state->e;
			flags(temp_short,&(state->flags));
			if(state->a < state->e)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->e & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBC: // CMP H
			temp_short = state->a - state->h;
			flags(temp_short,&(state->flags));
			if(state->a < state->h)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->h & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBD: // CMP L
			temp_short = state->a - state->l;
			flags(temp_short,&(state->flags));
			if(state->a < state->l)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->l & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBE: // CMP M
			temp = ((state->h << 8) & 0xFF00) + (state->l & 0x00FF);
			temp_short = state->a - state->mem[temp];
			flags(temp_short,&(state->flags));
			if(state->a < state->mem[temp])
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0F) < (state->mem[temp] & 0x0F))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xBF: // CMP A
			temp_short = 0;
			flags(temp_short,&(state->flags));
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xC0: // RNZ
			if(!state->flags.z)
			{
				temp = ((state->mem[(state->sp)+1] << 8) & 0xff00) + (state->mem[state->sp] & 0x00ff);
				state->pc = temp;
				state->elapsed_cycles += 11;
				state->sp += 2;
			}
			else
			{
				state->elapsed_cycles += 5;
				state->pc++;
			}
			break;
		case 0xC1: // POP B
			state->c = state->mem[state->sp];
			state->b = state->mem[(state->sp)+1];
			state->sp += 2;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0xC2: // JNZ
			if(!state->flags.z)
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			else
				state->pc += 3;
			state->elapsed_cycles += 10;
			break;
		case 0xC3: // JMP
			state->elapsed_cycles += 10;
			state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			break;
		case 0xC4: // CNZ
			if(!state->flags.z)
			{
				state->mem[state->sp - 1] = ((state->pc+3) >> 8) & 0xff;
				state->mem[state->sp - 2] = (state->pc+3) & 0xff;
				state->sp -= 2;
				state->elapsed_cycles += 17;
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00ff);
			}
			else
			{
				state->elapsed_cycles += 11;
				state->pc += 3;
			}
			break;
		case 0xC5: // PUSH B
			state->mem[(state->sp)-1] = state->b;
			state->mem[(state->sp)-2] = state->c;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc++;
			break;
		case 0xC6: // ADI
			temp = state->a + opcode[1];
			flags((uint8_t)temp,&(state->flags));
			if((temp >> 8) != 0)
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			temp_short = (state->a & 0x0f) + (opcode[1] & 0x0f);
			if((temp_short >> 4) != 0)
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = (uint8_t)(temp & 0xff);
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xC7: // RST 0
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x00;
			break;
		case 0xC8: // RZ
			if(state->flags.z)
			{
				temp = ((state->mem[(state->sp)+1] << 8) & 0xff00) + (state->mem[state->sp] & 0x00ff);
				state->pc = temp;
				state->elapsed_cycles += 11;
				state->sp += 2;
			}
			else
			{
				state->elapsed_cycles += 5;
				state->pc++;
			}
			break;
		case 0xC9: // RET
			temp = ((state->mem[(state->sp)+1] << 8) & 0xff00) + (state->mem[state->sp] & 0x00ff);
			state->pc = temp;
			state->elapsed_cycles += 10;
			state->sp += 2;
			break;
		case 0xCA: // JZ
			if(state->flags.z)
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			else
				state->pc += 3;
			state->elapsed_cycles += 10;
			break;
		case 0xCB:
			printf("JMP %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xCC: // CZ
			if(state->flags.z)
			{
				state->mem[state->sp - 1] = ((state->pc+3) >> 8) & 0xff;
				state->mem[state->sp - 2] = (state->pc+3) & 0xff;
				state->sp -= 2;
				state->elapsed_cycles += 17;
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00ff);
			}
			else
			{
				state->elapsed_cycles += 11;
				state->pc += 3;
			}
			break;
		case 0xCD: // CALL
			state->mem[state->sp - 1] = ((state->pc+3) >> 8) & 0xff;
			state->mem[state->sp - 2] = (state->pc+3) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 17;
			state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00ff);
			break;
		case 0xCE:
			printf("ACI %X", state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xCF: // RST 1
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x08;
			break;
		case 0xD0: // RNC
			if(!state->flags.cy)
			{
				temp = ((state->mem[(state->sp)+1] << 8) & 0xff00) + (state->mem[state->sp] & 0x00ff);
				state->pc = temp;
				state->elapsed_cycles += 11;
				state->sp += 2;
			}
			else
			{
				state->elapsed_cycles += 5;
				state->pc++;
			}
			break;
		case 0xD1: // POP D
			state->e = state->mem[state->sp];
			state->d = state->mem[state->sp + 1];
			state->sp += 2;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0xD2: // JNC
			if(!state->flags.cy)
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			else
				state->pc += 3;
			state->elapsed_cycles += 10;
			break;
		case 0xD3: // OUT
			ports->out[opcode[1]] = state->a;
			ports->out_activation[opcode[1]] = 1;
			state->elapsed_cycles += 10;
			state->pc += 2;
			break;
		case 0xD4: // CNC
			if(!state->flags.cy)
			{
				state->mem[state->sp - 1] = ((state->pc+3) >> 8) & 0xff;
				state->mem[state->sp - 2] = (state->pc+3) & 0xff;
				state->sp -= 2;
				state->elapsed_cycles += 17;
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00ff);
			}
			else
			{
				state->elapsed_cycles += 11;
				state->pc += 3;
			}
			break;
		case 0xD5: // PUSH D
			state->mem[(state->sp)-1] = state->d;
			state->mem[(state->sp)-2] = state->e;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc++;
			break;
		case 0xD6: // SUI
			temp = state->a - opcode[1];
			flags((uint8_t)temp,&(state->flags));
			if(state->a < opcode[1])
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0f) < (opcode[1] & 0x0f))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->a = temp;
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xD7: // RST 2
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x10;
			break;
		case 0xD8: // RC
			if(state->flags.cy)
			{
				temp = ((state->mem[(state->sp)+1] << 8) & 0xff00) + (state->mem[state->sp] & 0x00ff);
				state->pc = temp;
				state->elapsed_cycles += 11;
				state->sp += 2;
			}
			else
			{
				state->elapsed_cycles += 5;
				state->pc++;
			}
			break;
		case 0xD9:
			printf("RET");
			UnimplementedInstruction();
			break;
		case 0xDA: // JC
			if(state->flags.cy)
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			else
				state->pc += 3;
			state->elapsed_cycles += 10;
			break;
		case 0xDB: // IN
			state->a = ports->in[opcode[1]];
			state->elapsed_cycles += 10;
			state->pc += 2;
			break;
		case 0xDC:
			printf("CC %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xDD:
			printf("CALL %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xDE: // SBI
			temp_short = state->a - opcode[1] - state->flags.cy;
			flags(temp_short,&(state->flags));
			if((state->a & 0x0f) < ((opcode[1] + state->flags.cy) & 0x0f))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			if(state->a < (opcode[1] + state->flags.cy))
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			state->a = temp_short;
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xDF: // RST 3
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x18;
			break;
		case 0xE0:
			printf("RPO");
			UnimplementedInstruction();
			break;
		case 0xE1: // POP H
			state->l = state->mem[state->sp];
			state->h = state->mem[(state->sp)+1];
			state->sp += 2;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0xE2:
			printf("JPO %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xE3: // XTHL
			temp_short = state->l;
			state->l = state->mem[state->sp];
			state->mem[state->sp] = temp_short;
			temp_short = state->h;
			state->h = state->mem[state->sp + 1];
			state->mem[state->sp + 1] = temp_short;
			state->elapsed_cycles += 18;
			state->pc++;
			break;
		case 0xE4:
			printf("CPO %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xE5: // PUSH H
			state->mem[(state->sp)-1] = state->h;
			state->mem[(state->sp)-2] = state->l;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc++;
			break;
		case 0xE6: // ANI
			state->a = state->a & opcode[1];
			flags(state->a,&(state->flags));
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xE7: // RST 4
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x20;
			break;
		case 0xE8:
			printf("RPE");
			UnimplementedInstruction();
			break;
		case 0xE9: // PCHL
			state->elapsed_cycles += 5;
			state->pc = ((state->h << 8) & 0xFF00) + (state->l & 0x00FF);
			break;
		case 0xEA:
			printf("JPE %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xEB: // XCHG
			temp_short = state->h;
			state->h = state->d;
			state->d = temp_short;
			temp_short = state->l;
			state->l = state->e;
			state->e = temp_short;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xEC:
			printf("CPE %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xED:
			printf("CALL %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xEE:
			printf("XRI %X", state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xEF: // RST 5
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x28;
			break;
		case 0xF0:
			printf("RP");
			UnimplementedInstruction();
			break;
		case 0xF1: // POP PSW
			state->flags.cy = state->mem[state->sp] & 0x01;
			state->flags.p = (state->mem[state->sp] >> 2) & 0x01;
			state->flags.ac = (state->mem[state->sp] >> 4) & 0x01;
			state->flags.z = (state->mem[state->sp] >> 6) & 0x01;
			state->flags.s = (state->mem[state->sp] >> 7) & 0x01;
			state->a = state->mem[(state->sp)+1];
			state->sp += 2;
			state->elapsed_cycles += 10;
			state->pc++;
			break;
		case 0xF2:
			printf("JP %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xF3:
			printf("DI");
			UnimplementedInstruction();
			break;
		case 0xF4:
			printf("CP %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xF5: // PUSH PSW
			state->mem[(state->sp)-1] = state->a;
			temp_short = 2;
			temp_short += (state->flags.s << 7) + (state->flags.z << 6) + (state->flags.ac << 4) + (state->flags.p << 2) + state->flags.cy;
			state->mem[(state->sp)-2] = temp_short;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc++;
			break;
		case 0xF6: // ORI
			temp_short = state->a | opcode[1];
			flags(temp_short,&(state->flags));
			state->a = temp_short;
			state->flags.cy = 0;
			state->flags.ac = 0;
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xF7: // RST 6
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x30;
			break;
		case 0xF8:
			printf("RM");
			UnimplementedInstruction();
			break;
		case 0xF9:
			printf("SPHL");
			UnimplementedInstruction();
			break;
		case 0xFA: // JM
			if(state->flags.s)
				state->pc = ((opcode[2] << 8) & 0xFF00) + (opcode[1] & 0x00FF);
			else
				state->pc += 3;
			state->elapsed_cycles += 10;
			break;
		case 0xFB: // EI
			state->int_enable = 1;
			state->elapsed_cycles += 4;
			state->pc++;
			break;
		case 0xFC:
			printf("CM %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xFD:
			printf("CALL %X %X", state->mem[(state->pc)+2], state->mem[(state->pc)+1]);
			UnimplementedInstruction();
			break;
		case 0xFE: // CPI
			temp_short = (state->a - opcode[1]);
			flags(temp_short,&(state->flags));
			if(state->a < opcode[1])
				state->flags.cy = 1;
			else
				state->flags.cy = 0;
			if((state->a & 0x0f) < (opcode[1] & 0x0f))
				state->flags.ac = 1;
			else
				state->flags.ac = 0;
			state->elapsed_cycles += 7;
			state->pc += 2;
			break;
		case 0xFF: // RST 7
			state->mem[state->sp-1] = ((state->pc+1) >> 8) & 0xff;
			state->mem[state->sp-2] = (state->pc+1) & 0xff;
			state->sp -= 2;
			state->elapsed_cycles += 11;
			state->pc = 0x38;
			break;
		default:
			return 0;
	}
	
	return 1;
}