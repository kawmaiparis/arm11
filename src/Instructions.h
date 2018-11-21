#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#define COND_ALWAYS 0x0000000E
#define COND_SHIFT 28
#define ZERO 0x00000000
#define SET 0x00000001
#define PIPELINE_EFFECT 8
#define NUMBER_OF_BITS 32

//dataProcessing
#define DATAPROCESSING_I_SHIFT 25
#define DATAPROCESSING_OPCODE_SHIFT 21
#define DATAPROCESSING_S_SHIFT 20
#define DATAPROCESSING_RN_SHIFT 16
#define DATAPROCESSING_RD_SHIFT 12

#define ADD 0x00000004
#define SUB 0x00000002
#define RSB 0x00000003
#define AND 0x00000000
#define EOR 0x00000001
#define ORR 0x0000000C
#define MOV 0x0000000D
#define TST 0x00000008
#define TEQ 0x00000009
#define CMP 0x0000000A
#define ANDEQ 0x000000F4
//arbitary value
#define LSL 0xFFFFF12F

//multiply
#define MULFILL 0x00000009;
#define MULTIPLY_A_SHIFT 21
#define MULTIPLY_S_SHIFT 20
#define MULTIPLY_RD_SHIFT 16
#define MULTIPLY_RN_SHIFT 12
#define MULTIPLY_RS_SHIFT 8
#define MULTIPLY_FILL_SHIFT 4

//singleData
#define SINGLE_DATA_I_SHIFT 25
#define SINGLE_DATA_U_SHIFT 23
#define SINGLE_DATA_P_SHIFT 24
#define SINGLE_DATA_L_SHIFT 20
#define SINGLE_DATA_RN_SHIFT 16
#define SINGLE_DATA_RD_SHIFT 12
#define SINGLE_DATA_CONST 0x04000000

//branch
#define BRANCHFILL 0X0000000A
#define BRANCH_OFFSET 0x00FFFFFF
#define BRANCH_OFFSET_SHIFT 2
#define BRANCH_FILL_SHIFT 24

#define EQ 0x00000000
#define NE 0x00000001
#define GE 0x0000000A
#define LT 0x0000000B
#define GT 0x0000000C
#define LE 0x0000000C
#define AL 0x0000000E

enum instrType {PROCESS, MULTIPLY, TRANSFER, BRANCH, SPECIAL};

struct mapping{
		char** labels;
		uint32_t* addresses;
		uint32_t numberOfLabels;
} mappings;

struct tempToken{
		char** result;
		char* concat;
		char* bracketConcat;
		bool containsBracket;
		bool bracketHasClosed;
		uint32_t bracketStart;
		uint32_t bracketEnd;
		uint32_t argumentsToCombine;
		uint32_t perceivedArgs;
		uint32_t numberOfArgs;
} tempTokens;

struct storage {
		int size;
		uint32_t* storArr;
		bool assigned;
} store;

struct token {
		char** words;
		uint32_t numberOfElements;
		bool assigned;
} tokens;

void singleDataTokenizer(char* string);
char* indexCopy(char* str, int startIndex);
uint32_t rotateLeft(uint32_t operand, int rotationAmount);
uint32_t regCharToInt(char* reg);
uint32_t invert(uint32_t instruction);
uint32_t charsToInt(char* expression);
uint32_t Operand2Set(char* expr);
uint32_t getOpcode(char* instruction);
uint32_t dataProcessing(char** instruction);
uint32_t multiply(char** instruction);
bool isNeg(char* number);
void singleDataTokenizer(char* string);
uint32_t singleData(char** instruction, int currentAddress, int numberOfInstructions);
uint32_t getCond(char* bcond);
int stringCompare(char* str1, char* str2);
uint32_t branch(char** instruction, uint32_t numberofLabels, uint32_t counter);



