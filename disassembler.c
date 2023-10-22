#include <stdio.h>
#include <stdlib.h>
#include "disassembler.h"

int disassembler(unsigned char *bin_buffer, int pc)
{
	int len = 1;
	FILE *f = fopen("log.txt","a");
	if(f == NULL)
	{
		printf("ERROR: impossible to create the file log.txt");
		exit(1);
	}
	fprintf(f, "%X ",pc);
	
	
	switch(bin_buffer[pc])
	{
		case 0x00:
			fprintf(f, "NOP");
			break;
		case 0x01:
			fprintf(f, "LXI B %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x02:
			fprintf(f, "STAX B");
			break;
		case 0x03:
			fprintf(f, "INX B");
			break;
		case 0x04:
			fprintf(f, "INR B");
			break;
		case 0x05:
			fprintf(f, "DCR B");
			break;
		case 0x06:
			fprintf(f, "MVI B %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x07:
			fprintf(f, "RLC");
			break;
		case 0x08:
			fprintf(f, "NOP");
			break;
		case 0x09:
			fprintf(f, "DAD B");
			break;
		case 0x0A:
			fprintf(f, "LDAX B");
			break;
		case 0x0B:
			fprintf(f, "DCX B");
			break;
		case 0x0C:
			fprintf(f, "INR C");
			break;
		case 0x0D:
			fprintf(f, "DCR C");
			break;
		case 0x0E:
			fprintf(f, "MVI C %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x0F:
			fprintf(f, "RRC");
			break;
		case 0x10:
			fprintf(f, "NOP");
			break;
		case 0x11:
			fprintf(f, "LXI D %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x12:
			fprintf(f, "STAX D");
			break;
		case 0x13:
			fprintf(f, "INX D");
			break;
		case 0x14:
			fprintf(f, "INR D");
			break;
		case 0x15:
			fprintf(f, "DCR D");
			break;
		case 0x16:
			fprintf(f, "MVI D %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x17:
			fprintf(f, "RAL");
			break;
		case 0x18:
			fprintf(f, "NOP");
			break;
		case 0x19:
			fprintf(f, "DAD D");
			break;
		case 0x1A:
			fprintf(f, "LDAX D");
			break;
		case 0x1B:
			fprintf(f, "DCX D");
			break;
		case 0x1C:
			fprintf(f, "INR E");
			break;
		case 0x1D:
			fprintf(f, "DCR E");
			break;
		case 0x1E:
			fprintf(f, "MVI E %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x1F:
			fprintf(f, "RAR");
			break;
		case 0x20:
			fprintf(f, "NOP");
			break;
		case 0x21:
			fprintf(f, "LXI H %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x22:
			fprintf(f, "SHLD %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x23:
			fprintf(f, "INX H");
			break;
		case 0x24:
			fprintf(f, "INR H");
			break;
		case 0x25:
			fprintf(f, "DCR H");
			break;
		case 0x26:
			fprintf(f, "MVI H %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x27:
			fprintf(f, "DAA");
			break;
		case 0x28:
			fprintf(f, "NOP");
			break;
		case 0x29:
			fprintf(f, "DAD H");
			break;
		case 0x2A:
			fprintf(f, "LHLD %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x2B:
			fprintf(f, "DCX H");
			break;
		case 0x2C:
			fprintf(f, "INR L");
			break;
		case 0x2D:
			fprintf(f, "DCR L");
			break;
		case 0x2E:
			fprintf(f, "MVI L %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x2F:
			fprintf(f, "CMA");
			break;
		case 0x30:
			fprintf(f, "NOP");
			break;
		case 0x31:
			fprintf(f, "LXI SP %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x32:
			fprintf(f, "STA %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x33:
			fprintf(f, "INX SP");
			break;
		case 0x34:
			fprintf(f, "INR M");
			break;
		case 0x35:
			fprintf(f, "DCR M");
			break;
		case 0x36:
			fprintf(f, "MVI M %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x37:
			fprintf(f, "STC");
			break;
		case 0x38:
			fprintf(f, "NOP");
			break;
		case 0x39:
			fprintf(f, "DAD SP");
			break;
		case 0x3A:
			fprintf(f, "LDA %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0x3B:
			fprintf(f, "DCX SP");
			break;
		case 0x3C:
			fprintf(f, "INR A");
			break;
		case 0x3D:
			fprintf(f, "DCR A");
			break;
		case 0x3E:
			fprintf(f, "MVI A %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0x3F:
			fprintf(f, "CMC");
			break;
		case 0x40:
			fprintf(f, "MOV B,B");
			break;
		case 0x41:
			fprintf(f, "MOV B,C");
			break;
		case 0x42:
			fprintf(f, "MOV B,D");
			break;
		case 0x43:
			fprintf(f, "MOV B,E");
			break;
		case 0x44:
			fprintf(f, "MOV B,H");
			break;
		case 0x45:
			fprintf(f, "MOV B,L");
			break;
		case 0x46:
			fprintf(f, "MOV B,M");
			break;
		case 0x47:
			fprintf(f, "MOV B,A");
			break;
		case 0x48:
			fprintf(f, "MOV C,B");
			break;
		case 0x49:
			fprintf(f, "MOV C,C");
			break;
		case 0x4A:
			fprintf(f, "MOV C,D");
			break;
		case 0x4B:
			fprintf(f, "MOV C,E");
			break;
		case 0x4C:
			fprintf(f, "MOV C,H");
			break;
		case 0x4D:
			fprintf(f, "MOV C,L");
			break;
		case 0x4E:
			fprintf(f, "MOV C,M");
			break;
		case 0x4F:
			fprintf(f, "MOV C,A");
			break;
		case 0x50:
			fprintf(f, "MOV D,B");
			break;
		case 0x51:
			fprintf(f, "MOV D,C");
			break;
		case 0x52:
			fprintf(f, "MOV D,D");
			break;
		case 0x53:
			fprintf(f, "MOV D,E");
			break;
		case 0x54:
			fprintf(f, "MOV D,H");
			break;
		case 0x55:
			fprintf(f, "MOV D,L");
			break;
		case 0x56:
			fprintf(f, "MOV D,M");
			break;
		case 0x57:
			fprintf(f, "MOV D,A");
			break;
		case 0x58:
			fprintf(f, "MOV E,B");
			break;
		case 0x59:
			fprintf(f, "MOV E,C");
			break;
		case 0x5A:
			fprintf(f, "MOV E,D");
			break;
		case 0x5B:
			fprintf(f, "MOV E,E");
			break;
		case 0x5C:
			fprintf(f, "MOV E,H");
			break;
		case 0x5D:
			fprintf(f, "MOV E,L");
			break;
		case 0x5E:
			fprintf(f, "MOV E,M");
			break;
		case 0x5F:
			fprintf(f, "MOV E,A");
			break;
		case 0x60:
			fprintf(f, "MOV H,B");
			break;
		case 0x61:
			fprintf(f, "MOV H,C");
			break;
		case 0x62:
			fprintf(f, "MOV H,D");
			break;
		case 0x63:
			fprintf(f, "MOV H,E");
			break;
		case 0x64:
			fprintf(f, "MOV H,H");
			break;
		case 0x65:
			fprintf(f, "MOV H,L");
			break;
		case 0x66:
			fprintf(f, "MOV H,M");
			break;
		case 0x67:
			fprintf(f, "MOV H,A");
			break;
		case 0x68:
			fprintf(f, "MOV L,B");
			break;
		case 0x69:
			fprintf(f, "MOV L,C");
			break;
		case 0x6A:
			fprintf(f, "MOV L,D");
			break;
		case 0x6B:
			fprintf(f, "MOV L,E");
			break;
		case 0x6C:
			fprintf(f, "MOV L,H");
			break;
		case 0x6D:
			fprintf(f, "MOV L,L");
			break;
		case 0x6E:
			fprintf(f, "MOV L,M");
			break;
		case 0x6F:
			fprintf(f, "MOV L,A");
			break;
		case 0x70:
			fprintf(f, "MOV M,B");
			break;
		case 0x71:
			fprintf(f, "MOV M,C");
			break;
		case 0x72:
			fprintf(f, "MOV M,D");
			break;
		case 0x73:
			fprintf(f, "MOV M,E");
			break;
		case 0x74:
			fprintf(f, "MOV M,H");
			break;
		case 0x75:
			fprintf(f, "MOV M,L");
			break;
		case 0x76:
			fprintf(f, "HLT");
			break;
		case 0x77:
			fprintf(f, "MOV M,A");
			break;
		case 0x78:
			fprintf(f, "MOV A,B");
			break;
		case 0x79:
			fprintf(f, "MOV A,C");
			break;
		case 0x7A:
			fprintf(f, "MOV A,D");
			break;
		case 0x7B:
			fprintf(f, "MOV A,E");
			break;
		case 0x7C:
			fprintf(f, "MOV A,H");
			break;
		case 0x7D:
			fprintf(f, "MOV A,L");
			break;
		case 0x7E:
			fprintf(f, "MOV A,M");
			break;
		case 0x7F:
			fprintf(f, "MOV A,A");
			break;
		case 0x80:
			fprintf(f, "ADD B");
			break;
		case 0x81:
			fprintf(f, "ADD C");
			break;
		case 0x82:
			fprintf(f, "ADD D");
			break;
		case 0x83:
			fprintf(f, "ADD E");
			break;
		case 0x84:
			fprintf(f, "ADD H");
			break;
		case 0x85:
			fprintf(f, "ADD L");
			break;
		case 0x86:
			fprintf(f, "ADD M");
			break;
		case 0x87:
			fprintf(f, "ADD A");
			break;
		case 0x88:
			fprintf(f, "ADC B");
			break;
		case 0x89:
			fprintf(f, "ADC C");
			break;
		case 0x8A:
			fprintf(f, "ADC D");
			break;
		case 0x8B:
			fprintf(f, "ADC E");
			break;
		case 0x8C:
			fprintf(f, "ADC H");
			break;
		case 0x8D:
			fprintf(f, "ADC L");
			break;
		case 0x8E:
			fprintf(f, "ADC M");
			break;
		case 0x8F:
			fprintf(f, "ADC A");
			break;
		case 0x90:
			fprintf(f, "SUB B");
			break;
		case 0x91:
			fprintf(f, "SUB C");
			break;
		case 0x92:
			fprintf(f, "SUB D");
			break;
		case 0x93:
			fprintf(f, "SUB E");
			break;
		case 0x94:
			fprintf(f, "SUB H");
			break;
		case 0x95:
			fprintf(f, "SUB L");
			break;
		case 0x96:
			fprintf(f, "SUB M");
			break;
		case 0x97:
			fprintf(f, "SUB A");
			break;
		case 0x98:
			fprintf(f, "SBB B");
			break;
		case 0x99:
			fprintf(f, "SBB C");
			break;
		case 0x9A:
			fprintf(f, "SBB D");
			break;
		case 0x9B:
			fprintf(f, "SBB E");
			break;
		case 0x9C:
			fprintf(f, "SBB H");
			break;
		case 0x9D:
			fprintf(f, "SBB L");
			break;
		case 0x9E:
			fprintf(f, "SBB M");
			break;
		case 0x9F:
			fprintf(f, "SBB A");
			break;
		case 0xA0:
			fprintf(f, "ANA B");
			break;
		case 0xA1:
			fprintf(f, "ANA C");
			break;
		case 0xA2:
			fprintf(f, "ANA D");
			break;
		case 0xA3:
			fprintf(f, "ANA E");
			break;
		case 0xA4:
			fprintf(f, "ANA H");
			break;
		case 0xA5:
			fprintf(f, "ANA L");
			break;
		case 0xA6:
			fprintf(f, "ANA M");
			break;
		case 0xA7:
			fprintf(f, "ANA A");
			break;
		case 0xA8:
			fprintf(f, "XRA B");
			break;
		case 0xA9:
			fprintf(f, "XRA C");
			break;
		case 0xAA:
			fprintf(f, "XRA D");
			break;
		case 0xAB:
			fprintf(f, "XRA E");
			break;
		case 0xAC:
			fprintf(f, "XRA H");
			break;
		case 0xAD:
			fprintf(f, "XRA L");
			break;
		case 0xAE:
			fprintf(f, "XRA M");
			break;
		case 0xAF:
			fprintf(f, "XRA A");
			break;
		case 0xB0:
			fprintf(f, "ORA B");
			break;
		case 0xB1:
			fprintf(f, "ORA C");
			break;
		case 0xB2:
			fprintf(f, "ORA D");
			break;
		case 0xB3:
			fprintf(f, "ORA E");
			break;
		case 0xB4:
			fprintf(f, "ORA H");
			break;
		case 0xB5:
			fprintf(f, "ORA L");
			break;
		case 0xB6:
			fprintf(f, "ORA M");
			break;
		case 0xB7:
			fprintf(f, "ORA A");
			break;
		case 0xB8:
			fprintf(f, "CMP B");
			break;
		case 0xB9:
			fprintf(f, "CMP C");
			break;
		case 0xBA:
			fprintf(f, "CMP D");
			break;
		case 0xBB:
			fprintf(f, "CMP E");
			break;
		case 0xBC:
			fprintf(f, "CMP H");
			break;
		case 0xBD:
			fprintf(f, "CMP L");
			break;
		case 0xBE:
			fprintf(f, "CMP M");
			break;
		case 0xBF:
			fprintf(f, "CMP A");
			break;
		case 0xC0:
			fprintf(f, "RNZ");
			break;
		case 0xC1:
			fprintf(f, "POP B");
			break;
		case 0xC2:
			fprintf(f, "JNZ %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xC3:
			fprintf(f, "JMP %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xC4:
			fprintf(f, "CNZ %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xC5:
			fprintf(f, "PUSH B");
			break;
		case 0xC6:
			fprintf(f, "ADI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xC7:
			fprintf(f, "RST 0");
			break;
		case 0xC8:
			fprintf(f, "RZ");
			break;
		case 0xC9:
			fprintf(f, "RET");
			break;
		case 0xCA:
			fprintf(f, "JZ %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xCB:
			fprintf(f, "JMP %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xCC:
			fprintf(f, "CZ %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xCD:
			fprintf(f, "CALL %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xCE:
			fprintf(f, "ACI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xCF:
			fprintf(f, "RST 1");
			break;
		case 0xD0:
			fprintf(f, "RNC");
			break;
		case 0xD1:
			fprintf(f, "POP D");
			break;
		case 0xD2:
			fprintf(f, "JNC %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xD3:
			fprintf(f, "OUT %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xD4:
			fprintf(f, "CNC %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xD5:
			fprintf(f, "PUSH D");
			break;
		case 0xD6:
			fprintf(f, "SUI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xD7:
			fprintf(f, "RST 2");
			break;
		case 0xD8:
			fprintf(f, "RC");
			break;
		case 0xD9:
			fprintf(f, "RET");
			break;
		case 0xDA:
			fprintf(f, "JC %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xDB:
			fprintf(f, "IN %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xDC:
			fprintf(f, "CC %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xDD:
			fprintf(f, "CALL %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xDE:
			fprintf(f, "SBI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xDF:
			fprintf(f, "RST 3");
			break;
		case 0xE0:
			fprintf(f, "RPO");
			break;
		case 0xE1:
			fprintf(f, "POP H");
			break;
		case 0xE2:
			fprintf(f, "JPO %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xE3:
			fprintf(f, "XTHL");
			break;
		case 0xE4:
			fprintf(f, "CPO %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xE5:
			fprintf(f, "PUSH H");
			break;
		case 0xE6:
			fprintf(f, "ANI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xE7:
			fprintf(f, "RST 4");
			break;
		case 0xE8:
			fprintf(f, "RPE");
			break;
		case 0xE9:
			fprintf(f, "PCHL");
			break;
		case 0xEA:
			fprintf(f, "JPE %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xEB:
			fprintf(f, "XCHG");
			break;
		case 0xEC:
			fprintf(f, "CPE %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xED:
			fprintf(f, "CALL %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xEE:
			fprintf(f, "XRI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xEF:
			fprintf(f, "RST 5");
			break;
		case 0xF0:
			fprintf(f, "RP");
			break;
		case 0xF1:
			fprintf(f, "POP PSW");
			break;
		case 0xF2:
			fprintf(f, "JP %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xF3:
			fprintf(f, "DI");
			break;
		case 0xF4:
			fprintf(f, "CP %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xF5:
			fprintf(f, "PUSH PSW");
			break;
		case 0xF6:
			fprintf(f, "ORI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xF7:
			fprintf(f, "RST 6");
			break;
		case 0xF8:
			fprintf(f, "RM");
			break;
		case 0xF9:
			fprintf(f, "SPHL");
			break;
		case 0xFA:
			fprintf(f, "JM %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xFB:
			fprintf(f, "EI");
			break;
		case 0xFC:
			fprintf(f, "CM %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xFD:
			fprintf(f, "CALL %X %X", bin_buffer[pc+2], bin_buffer[pc+1]);
			len = 3;
			break;
		case 0xFE:
			fprintf(f, "CPI %X", bin_buffer[pc+1]);
			len = 2;
			break;
		case 0xFF:
			fprintf(f, "RST 7");
			break;
		default:
			return 0;
	}
	fprintf(f, "\n");
	
	fclose(f);
	return len;
}