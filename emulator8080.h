struct Flags
{
	uint8_t s:1;
	uint8_t z:1;
	uint8_t p:1;
	uint8_t cy:1;
	uint8_t ac:1;
	uint8_t pad:3;
};

typedef struct State
{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t pc;
	uint16_t sp;
	struct Flags flags;
	uint8_t *mem;
	uint8_t int_enable;
	uint64_t elapsed_cycles;
} State;

typedef struct Ports // add all supported ports by Intel 8080 in the future
{
	uint8_t in[4];
	uint8_t out[7];
	uint8_t out_activation[7];
} Ports;

void InitEmulator(int d, unsigned char *buffer, State *state, Ports *ports);
void GenerateInterrupt(State *state, uint8_t var);
void UnimplementedInstruction();
void flags(uint8_t var, struct Flags *f);
int translator(State *state, Ports *ports);