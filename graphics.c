#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/SDL2/SDL.h"
#include "graphics.h"


void ConvertBPP(uint8_t *one_bpp, uint8_t *eight_bpp)
{
	int i, j;
	for(j=0; j<256*224/8; j++)
	{
		for(i=0; i<8; i++)
		{
			if(((one_bpp[j] >> i) & 0x01) == 0x01)
			{
				eight_bpp[8*j+i] = 0xFF;
			}
			else
			{
				eight_bpp[8*j+i] = 0x00;
			}
		}
	}
	return;
}

void Rotate90(uint8_t *image)
{
	int i, j;
	uint8_t temp[256*224];
	for(i=255; i>=0; i--)
	{
		for(j=0; j<224; j++)
		{
			temp[j+(255-i)*224] = image[i+256*j];
		}
	}
	for(j=0; j<256*224; j++)
	{
		image[j] = temp[j];
	}
	return;
}