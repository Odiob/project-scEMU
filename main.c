#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/SDL2/SDL.h"
#include "disassembler.h"
#include "emulator8080.h"
#include "hshifter.h"
#include "graphics.h"

#define DEBUG 0  // 0 = no debug, 1 = instruction log (too slow to be used at the moment)

int main(int argc, char* argv[])
{
	// FILE READ

	FILE *f = fopen(argv[1],"rb");
	if(f == NULL)
	{
		printf("ERROR: impossible to open the file %s", argv[1]);
		exit(1);
	}
	
	if(fseek(f, 0, SEEK_END) != 0)
	{
		printf("ERROR: impossible to determine the file size");
		exit(1);
	}
	int size = ftell(f);
	if(fseek(f, 0, SEEK_SET) != 0)
	{
		printf("ERROR: impossible to reset the binary file pointer");
		exit(1);
	}
	
	unsigned char *buffer = malloc(size);
	if(fread(buffer, size, 1, f) != 1)
	{
		printf("ERROR: impossible to read the whole binary");
		exit(1);
	}

	fclose(f);
	
	
	// INITIALIZATION
	
	State *status = malloc(sizeof(State));
	Ports *ports = malloc(sizeof(Ports)); 
	InitEmulator(size, buffer, status, ports);
	ports->in[0] = 0x0E;	// hardcoded input ports
	ports->in[1] = 0x08;
	ports->in[2] = 0x00;	// controls number of lives 
	
	Shifter *shifter_status = malloc(sizeof(Shifter)); ;
	shifter_status->offset = 0;
	
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Failed to initialize the SDL2 library\n");
        exit(1);
    }
	
    SDL_Window *window = SDL_CreateWindow("project-scemu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 224, 256, 0);
    if(!window)
    {
        printf("Failed to create window\n");
        exit(1);
    }
	
	uint8_t keep_window_open = 1;
	SDL_Event e;

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if(!window_surface)
    {
        printf("Failed to get the surface from the window\n");
        exit(1);
    }
	
	uint8_t vbuffer[256*224];
	SDL_Surface *render_surface = SDL_CreateRGBSurface(0, 224, 256, 8, 0xE0, 0x1C, 0x03, 0);
	render_surface->pixels = vbuffer;
	
	SDL_AudioSpec wavSpec[10];
	uint32_t wavLength[10];
	uint8_t *wavBuffer[10];
	SDL_LoadWAV("sounds/ufo_highpitch.wav", &(wavSpec[0]), &(wavBuffer[0]), &(wavLength[0]));
	SDL_LoadWAV("sounds/shoot.wav", &(wavSpec[1]), &(wavBuffer[1]), &(wavLength[1]));
	SDL_LoadWAV("sounds/explosion.wav", &(wavSpec[2]), &(wavBuffer[2]), &(wavLength[2]));
	SDL_LoadWAV("sounds/invaderkilled.wav", &(wavSpec[3]), &(wavBuffer[3]), &(wavLength[3]));
	SDL_LoadWAV("sounds/extendedplay.wav", &(wavSpec[4]), &(wavBuffer[4]), &(wavLength[4]));
	SDL_LoadWAV("sounds/fastinvader1.wav", &(wavSpec[5]), &(wavBuffer[5]), &(wavLength[5]));
	SDL_LoadWAV("sounds/fastinvader2.wav", &(wavSpec[6]), &(wavBuffer[6]), &(wavLength[6]));
	SDL_LoadWAV("sounds/fastinvader3.wav", &(wavSpec[7]), &(wavBuffer[7]), &(wavLength[7]));
	SDL_LoadWAV("sounds/fastinvader4.wav", &(wavSpec[8]), &(wavBuffer[8]), &(wavLength[8]));
	SDL_LoadWAV("sounds/ufo_lowpitch.wav", &(wavSpec[9]), &(wavBuffer[9]), &(wavLength[9]));
	SDL_AudioDeviceID deviceId[10];
	
	uint8_t i;
	i = 0;
	uint64_t time, last_int, audio_delay[10];
	last_int = SDL_GetTicks64();
	
	for(i=0; i<10; i++)
	{
		audio_delay[i] = 0;
		deviceId[i] = SDL_OpenAudioDevice(NULL, 0, &(wavSpec[i]), NULL, 0);
	}
	

	
	// MAIN LOOP
	
	while(keep_window_open)
	{
		
		// Input
		
		while(SDL_PollEvent(&e) > 0)
		{
			switch(e.type)
			{
				case SDL_QUIT:
					keep_window_open = 0;
					break;
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
						case SDLK_c:
							printf("c\n");
							ports->in[1] = ports->in[1] | 0x01;
							break;
						case SDLK_1:
							printf("1p\n");
							ports->in[1] = ports->in[1] | 0x04;
							break;
						case SDLK_2:
							printf("2p\n");
							ports->in[1] = ports->in[1] | 0x02;
							break;
						case SDLK_SPACE:
							printf("fire1\n");
							ports->in[0] = ports->in[0] | 0x10;
							ports->in[1] = ports->in[1] | 0x10;
							break;
						case SDLK_a:
							printf("left1\n");
							ports->in[0] = ports->in[0] | 0x20;
							ports->in[1] = ports->in[1] | 0x20;
							break;
						case SDLK_d:
							printf("right1\n");
							ports->in[0] = ports->in[0] | 0x40;
							ports->in[1] = ports->in[1] | 0x40;
							break;
						case SDLK_RSHIFT:
							printf("fire2\n");
							ports->in[2] = ports->in[2] | 0x10;
							break;
						case SDLK_LEFT:
							printf("left2\n");
							ports->in[2] = ports->in[2] | 0x20;
							break;
						case SDLK_RIGHT:
							printf("right2\n");
							ports->in[2] = ports->in[2] | 0x40;
							break;
					}
					break;
				case SDL_KEYUP:
					switch(e.key.keysym.sym)
					{
						case SDLK_c:
							printf("c\n");
							ports->in[1] = ports->in[1] & 0xFE;
							break;
						case SDLK_1:
							printf("1p up\n");
							ports->in[1] = ports->in[1] & 0xFB;
							break;
						case SDLK_2:
							printf("2p up\n");
							ports->in[1] = ports->in[1] & 0xFD;
							break;
						case SDLK_SPACE:
							printf("fire1 up\n");
							ports->in[0] = ports->in[0] & 0xEF;
							ports->in[1] = ports->in[1] & 0xEF;
							break;
						case SDLK_a:
							printf("left1 up\n");
							ports->in[0] = ports->in[0] & 0xDF;
							ports->in[1] = ports->in[1] & 0xDF;
							break;
						case SDLK_d:
							printf("right1 up\n");
							ports->in[0] = ports->in[0] & 0xBF;
							ports->in[1] = ports->in[1] & 0xBF;
							break;
						case SDLK_RSHIFT:
							printf("fire2 up\n");
							ports->in[2] = ports->in[2] & 0xEF;
							break;
						case SDLK_LEFT:
							printf("left2 up\n");
							ports->in[2] = ports->in[2] & 0xDF;
							break;
						case SDLK_RIGHT:
							printf("right2 up\n");
							ports->in[2] = ports->in[2] & 0xBF;
							break;
					}
					break;
			}
		}
		
		
		// Execute instruction
		
		if((int)(status->elapsed_cycles)*120 < 1000*2000) // eventually do not hardcode refresh rate and CPU frequency
		{
			if(DEBUG)
			{
				disassembler(status->mem, status->pc);
			}
			if(!translator(status, ports))
			{
				printf("ERROR: the selected binary contains invalid instructions");
				exit(1);
			}
		}
		
		time = SDL_GetTicks64();
		
		
		// Draw screen
		
		if((time - last_int >= 1000/120) && (status->int_enable == 1))
		{
			if(i == 0)
			{
				GenerateInterrupt(status, 1);
				last_int = time;
				i++;
				status->elapsed_cycles = 0;
			}
			else
			{
				ConvertBPP(&status->mem[9216], vbuffer);
				Rotate90(vbuffer);
				SDL_BlitSurface(render_surface, NULL, window_surface, NULL);
				SDL_UpdateWindowSurface(window);
				GenerateInterrupt(status, 2);
				last_int = time;
				i--;
				status->elapsed_cycles = 0;
			}
		}
		
		if(status->int_enable == 0)
		{
			status->elapsed_cycles = 0;
		}
		
		
		// Call hardware shifter
		
		Shift(shifter_status, ports);

		
		// Audio output
		
		if(ports->out_activation[3] != 0)
		{
			if(((ports->out[3] & 0x01) == 0x01) && (SDL_GetQueuedAudioSize(deviceId[0]) == 0) && (SDL_GetTicks64() - audio_delay[0] > 10))
			{
				SDL_QueueAudio(deviceId[0], wavBuffer[0], wavLength[0]);
				SDL_PauseAudioDevice(deviceId[0], 0);
				audio_delay[0] = SDL_GetTicks64();
			}
			if(((ports->out[3] & 0x02) == 0x02) && (SDL_GetQueuedAudioSize(deviceId[1]) == 0) && (SDL_GetTicks64() - audio_delay[1] > 980))
			{
				SDL_QueueAudio(deviceId[1], wavBuffer[1], wavLength[1]);
				SDL_PauseAudioDevice(deviceId[1], 0);
				audio_delay[1] = SDL_GetTicks64();
			}
			if(((ports->out[3] & 0x04) == 0x04) && (SDL_GetQueuedAudioSize(deviceId[2]) == 0) && (SDL_GetTicks64() - audio_delay[2] > 1000))
			{
				SDL_QueueAudio(deviceId[2], wavBuffer[2], wavLength[2]);
				SDL_PauseAudioDevice(deviceId[2], 0);
				audio_delay[2] = SDL_GetTicks64();
			}
			if(((ports->out[3] & 0x08) == 0x08) && (SDL_GetQueuedAudioSize(deviceId[3]) == 0) && (SDL_GetTicks64() - audio_delay[3] > 250))
			{
				SDL_QueueAudio(deviceId[3], wavBuffer[3], wavLength[3]);
				SDL_PauseAudioDevice(deviceId[3], 0);
				audio_delay[3] = SDL_GetTicks64();
			}
			if(((ports->out[3] & 0x10) == 0x10) && (SDL_GetQueuedAudioSize(deviceId[4]) == 0) && (SDL_GetTicks64() - audio_delay[4] > 250))
			{
				SDL_QueueAudio(deviceId[4], wavBuffer[4], wavLength[4]);
				SDL_PauseAudioDevice(deviceId[4], 0);
				audio_delay[4] = SDL_GetTicks64();
			}
			ports->out_activation[3] = 0;
		}
		
		if(ports->out_activation[5] != 0)
		{
			if(((ports->out[5] & 0x01) == 0x01) && (SDL_GetQueuedAudioSize(deviceId[5]) == 0))
			{
				SDL_QueueAudio(deviceId[5], wavBuffer[5], wavLength[5]);
				SDL_PauseAudioDevice(deviceId[5], 0);
			}
			if(((ports->out[5] & 0x02) == 0x02) && (SDL_GetQueuedAudioSize(deviceId[6]) == 0))
			{
				SDL_QueueAudio(deviceId[6], wavBuffer[6], wavLength[6]);
				SDL_PauseAudioDevice(deviceId[6], 0);
			}
			if(((ports->out[5] & 0x04) == 0x04) && (SDL_GetQueuedAudioSize(deviceId[7]) == 0))
			{
				SDL_QueueAudio(deviceId[7], wavBuffer[7], wavLength[7]);
				SDL_PauseAudioDevice(deviceId[7], 0);
			}
			if(((ports->out[5] & 0x08) == 0x08) && (SDL_GetQueuedAudioSize(deviceId[8]) == 0))
			{
				SDL_QueueAudio(deviceId[8], wavBuffer[8], wavLength[8]);
				SDL_PauseAudioDevice(deviceId[8], 0);
			}
			if(((ports->out[5] & 0x10) == 0x10) && (SDL_GetQueuedAudioSize(deviceId[9]) == 0))
			{
				SDL_QueueAudio(deviceId[9], wavBuffer[9], wavLength[9]);
				SDL_PauseAudioDevice(deviceId[9], 0);
			}
			ports->out_activation[5] = 0;
		}
		
	}
	
	
	// CLEANING
	
	SDL_DestroyWindow(window);
	SDL_FreeSurface(render_surface);
	for(i=0; i<10; i++)
	{
		SDL_CloseAudioDevice(deviceId[i]);
		SDL_FreeWAV(wavBuffer[i]);
	}
    SDL_Quit();
	free(status->mem);
	free(status);
	free(ports);
	free(shifter_status);
	
	return 0;
}
