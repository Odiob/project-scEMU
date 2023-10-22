#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator8080.h"
#include "hshifter.h"


void Shift(Shifter *state, Ports *ports)
{
	if(ports->out_activation[4] != 0)
	{
		state->y = state->x;
		state->x = ports->out[4];
		ports->out_activation[4] = 0;
	}
	else if(ports->out_activation[2] != 0)
	{
		if(ports->out[2] > 8)
		{
			printf("HARDWARE SHIFTER ERROR: trying to shift of more than 8 bits");
			exit(1);
		}
		state->offset = ports->out[2];
		ports->out_activation[2] = 0;
	}
	else
	{
		return;
	}
	
	uint16_t temp = (state->x << 8) + state->y;
	ports->in[3] = (uint8_t)(temp >> (8 - state->offset));
	
	return;
}