#define ZERO_PADDING 0x00000000
#define NON_ZERO_PADDING 0xFFFFFFFF
#define MEMORY_SIZE 65536
#define NUMBER_OF_BITS 32
#define BYTES_PER_WORD 4

struct state {
		// 0-12 general purpose. 13 is SP, 14 is LR, 15 is PC, 16 is CSPR
		uint32_t regs[17];
		uint8_t *memory;
		uint32_t fetched;
		uint32_t decoded;
} states;

typedef enum { false, true } bool;

//helper functions
uint32_t getBitMask();
uint32_t getBits(uint32_t value, uint32_t start_index, uint32_t size);
uint32_t makeMask(uint32_t val);
uint32_t rotateRight(uint32_t operand, uint32_t rotationAmount);
void printResults();
uint32_t getWord(uint32_t address);
void printNonZeroMem();

//decode execute and instruction functions
uint32_t decode(uint32_t fetched);
uint32_t check(uint32_t cond, uint32_t cpsr);
void setFlag(char c, uint32_t val);
uint32_t shifter(uint32_t shiftType, uint32_t content, uint32_t shiftAmount);
void multiply(uint32_t instruction);
void dataProcessing(uint32_t instruction);
void branch(uint32_t instruction);
uint32_t fetch (uint32_t loc);
void store (uint32_t value, uint32_t loc);
void singleData(uint32_t instruction);
void runInstruction(uint32_t instruction);
void execute();

