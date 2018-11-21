#include "Instructions.h"

//Helper function to copy char* from a certain index
//startIndex from 0
//probably built infuction for this in the libraru
char* indexCopy(char* str, int startIndex) {
	int len = strlen(str);
	char* substr = calloc(len - startIndex, sizeof(char));
	//can abitary length with figure out max lenght later
	strncpy(substr, (str + startIndex ), (len - startIndex));
	return substr;
}

uint32_t rotateLeft(uint32_t operand, int rotationAmount) {
	return (operand << rotationAmount) | (operand >> (32 - rotationAmount));
}

uint32_t regCharToInt(char* reg) {
	if (reg[0] == ' ') {
		reg++;
	}
	reg++;
	return (uint32_t) atoi(reg);
}

uint32_t invert(uint32_t instruction) {
	uint32_t result = ((instruction & 0xFF000000) >> 24) |
	((instruction & 0x00FF0000) >> 8) |
	((instruction & 0x0000FF00) << 8) |
	((instruction & 0x000000FF) << 24);
	return result;
}

//-------------------------------------------------------DATA PROCESSING -------------------------------------------------
uint32_t charsToInt(char* expression) {
	uint32_t result = ZERO;
	char* newExpr = "";
	uint32_t x = 2;
	uint32_t hash = ZERO;

	if (expression[0] == ' ') {
		x += 1;
		hash +=1;
	}
	if (expression[hash + 1] == '-') {
		hash ++;
		x ++;
	}

	if (expression[x] == 'x') {
		newExpr = indexCopy(expression, x + 1);
		result = strtol(newExpr, NULL, 16);
	} else {
		newExpr = indexCopy(expression, hash + 1);
		result = (uint32_t) atoi(newExpr);
	}
	return result;
}

uint32_t DataProcessing_I;

uint32_t Operand2Set(char* expr) {
	// first part takes the immediate and converts into hexadecimal or decimal in uint32_t;
	char* expression = expr;
	char* newExpression = "";
	uint32_t constant = ZERO;
	uint32_t Operand2 = ZERO;
	uint32_t Rm = ZERO;
	int boolean = SET;
	int rIndex = ZERO;
	int xIndex = 2;
	int zeroIndex = SET;
	int hashIndex = ZERO;

	if (expression[0] == ' ') {
		rIndex += 1;
		xIndex += 1;
		zeroIndex += 1;
		hashIndex += 1;
	}
	if (expression[zeroIndex] == '0' && expression[xIndex] == 'x') {
		//finds string without 0x appended
		//fixing the indexing
		//converts string into hexadecimal in uint32_t format
		//for the case of the immediate in hexadecimal format
		constant = charsToInt(expression);
		DataProcessing_I = SET;
	} else if (expression[hashIndex] == '#' || expression[hashIndex] == '=') {
		//finds the string without the # appended
		//converts stirng into decimal in uint32_t format
		constant = charsToInt(expression);
		DataProcessing_I = SET;
		// all resulting cases will then be in register format
	} else if (expression[rIndex] == 'r') {
		newExpression = indexCopy(expression, rIndex);
		Rm = regCharToInt(newExpression);
		Operand2 = Rm;
		boolean = ZERO;
		DataProcessing_I = ZERO;
	}

	//Optional for shifted register
	//largest value represented in 8 bits is 0xFF
	if (boolean) {
		if (constant > 0x000000FF) {
			int shiftCount = ZERO;
			while ((constant & 0xFFFFFF00) > ZERO) {
				//checks if the first 24 bits are empty
				//keeps rotating left till the first 24 bits are empty
				constant = rotateLeft(constant, 1);
				shiftCount++;
			}
			//shiftCount should always be a multiple of 2 from 0 to 30
			if (shiftCount > 30) {
				printf("something broke");
			} else {
				if (shiftCount % 2 != 0) {
					constant = constant << 1;
					shiftCount = shiftCount + 1;
				}
				shiftCount  = (shiftCount) /  2;
			}
			//move the shiftCount to the correct position
			shiftCount = shiftCount << 8;
			//bitwise or to construct the Operand2
			Operand2 = constant | shiftCount;
		} else {
			// only the last 8 bits will be needed so its find to just set Operand2 as immediate
			Operand2 = constant;
		}
	}
	return Operand2;
}

//get Opcode
uint32_t getOpcode(char* instruction) {
	if (!strcmp("add", instruction)) {
		return ADD;
	} else if (!strcmp("sub", instruction)) {
		return SUB;
	} else if (!strcmp("rsb", instruction)) {
		return RSB;
	} else if (!strcmp("and", instruction)) {
		return AND;
	} else if (!strcmp("eor", instruction)) {
		return EOR;
	} else if (!strcmp("orr", instruction)) {
		return ORR;
	} else if (!strcmp("mov", instruction)) {
		return MOV;
	} else if (!strcmp("tst", instruction)) {
		return TST;
	} else if (!strcmp("teq", instruction)) {
		return TEQ;
	} else if (!strcmp("cmp", instruction)) {
		return CMP;
	} else if (!strcmp("andeq", instruction)) {
		return ANDEQ;
	} else if (!strcmp("lsl", instruction)){
		return LSL;
	} else
		return ZERO;
}

uint32_t dataProcessing(char** instruction) {
	uint32_t total = 0;
	uint32_t Cond = COND_ALWAYS;
	//I set if offset is constant, I not set if offset is shifter
	uint32_t S = 0;
	uint32_t Rn = 0;
	uint32_t Rm = 0;
	uint32_t Rd = 0;
	uint32_t Operand2 = 0;

	char* mnemonic = instruction[0];
	uint32_t Opcode = getOpcode(mnemonic);
	switch (Opcode) {
		case ADD :
		case SUB :
		case RSB :
		case AND :
		case EOR :
		case ORR :
			Rd = regCharToInt(instruction[1]);
			Rn = regCharToInt(instruction[2]);
			Operand2 = Operand2Set(instruction[3]);
			break;
		case MOV :
			Rd = regCharToInt(instruction[1]);
			Operand2 = Operand2Set(instruction[2]);
			break;
		case TST :
		case TEQ :
		case CMP :
			Rn = regCharToInt(instruction[1]);
			S = 1;
			Operand2 = Operand2Set(instruction[2]);
			break;
		case LSL :
			Opcode = MOV;
			Rd = regCharToInt(instruction[1]);
			Operand2 = (Operand2Set(instruction[2]) << 7) | Rd;
			DataProcessing_I = 0;
			break;
		case ANDEQ :
			return ZERO;
		default :
			break;
	}

	//shift each part to the right place and OR all of them together
	Cond = Cond << COND_SHIFT;
	DataProcessing_I = DataProcessing_I << DATAPROCESSING_I_SHIFT;
	Opcode = Opcode << DATAPROCESSING_OPCODE_SHIFT;
	S = S << DATAPROCESSING_S_SHIFT;
	Rn = Rn << DATAPROCESSING_RN_SHIFT;
	Rd = Rd << DATAPROCESSING_RD_SHIFT;
	total = Cond | DataProcessing_I | Opcode | S | Rn | Rd | Operand2 ;
	return total;
}


//-------------------------------------------------------MULTIPLY -------------------------------------------------

//main for multiply
uint32_t multiply(char** instruction) {
	uint32_t total = 0;
	uint32_t Rd = regCharToInt(instruction[1]);
	uint32_t Rm = regCharToInt(instruction[2]);
	uint32_t Rs = regCharToInt(instruction[3]);
	uint32_t Rn = 0;
	uint32_t A = 0;
	uint32_t S = 0;
	uint32_t cond = COND_ALWAYS;
	uint32_t fill = MULFILL;
	if (!strcmp(instruction[0], "mul")) {
		A = 0;
	} else {
		A = 1;
		Rn = regCharToInt(instruction[4]);
	}

	cond = cond << COND_SHIFT;
	A = A << MULTIPLY_A_SHIFT;
	S = S << MULTIPLY_S_SHIFT;
	Rd = Rd << MULTIPLY_RD_SHIFT;
	Rn = Rn << MULTIPLY_RN_SHIFT;
	Rs = Rs << MULTIPLY_RS_SHIFT;
	fill = fill << MULTIPLY_FILL_SHIFT;
	total = cond | A | S | Rd | Rn | Rs | fill | Rm;
	return total;
}

//-------------------------------------------------------SINGLE DATA ---------------------------------------------------

bool isNeg(char* number) {
	if (number[1] == '-') {
		return true;
	}
	return false;
}

void singleDataTokenizer(char* string) {
	if (tokens.assigned) {
		for (int i = 0; i < tokens.numberOfElements; i++) {
			free(tokens.words[i]);
			tokens.words[i] = NULL;
		}
	}
	free(tokens.words);

	char* tempStr = strdup(string);
	char* temp;
	uint32_t numElements = 0;
	uint32_t size = sizeof(string)/sizeof(string[0]);
	//number of elements is one more than number of commas
	for (uint32_t i = 0; i < size; i++) {
		if (string[i] == ',') {
			numElements ++;
		}
	}
	numElements ++;

	tokens.numberOfElements = numElements;
	tokens.words = calloc(numElements, sizeof(char*));

	temp = strtok (tempStr,"[ ] ,");
	tokens.words[0] = calloc(strlen(temp), sizeof(char));
	strcpy(tokens.words[0], temp);
	for (uint32_t i = 1; i < numElements; i++) {
		temp = strtok (NULL, "[ ] ,");
		tokens.words[i] = calloc(strlen(temp), sizeof(char));
		strcpy(tokens.words[i], temp);
	}
	tokens.assigned = true;
	free(tempStr);
}

//main for singleData
uint32_t singleData(char** instruction, int currentAddress, int numberOfInstructions) {
	uint32_t total = ZERO;
	uint32_t cond = COND_ALWAYS;
	if (!store.assigned) {
		store.storArr = calloc(10, sizeof(uint32_t));
		store.size = 0;
		store.assigned = true;
	}
	uint32_t I;
	uint32_t P;
	uint32_t U;
	uint32_t L;
	uint32_t Rn;
	uint32_t Rd;
	uint32_t offset;
	uint32_t address = ZERO;

	if (!strcmp(instruction[0], "ldr")) {
		L = SET;
	} else {
		L = ZERO;
	}

	Rd = regCharToInt(instruction[1]);
	singleDataTokenizer(instruction[2]);
	if (tempTokens.numberOfArgs == 3){
		if (tokens.numberOfElements == 1) {
			if (tokens.words[0][0] == 'r') {
				I = ZERO;
				P = SET;
				U = SET;
				Rn = regCharToInt(tokens.words[0]);
				offset = ZERO;
			} else {
				address = charsToInt(instruction[2]);
				if (address <= 0xFF ) {
					char** arg = calloc(3, sizeof(char*));
					arg[0] = "mov";
					arg[1] = instruction[1];
					arg[2] = instruction[2];
					return dataProcessing(arg);
				} else {
					store.storArr[store.size] = address;
					offset = (uint32_t) (sizeof(int) * (numberOfInstructions + store.size - currentAddress) - PIPELINE_EFFECT);
					store.size++;
					Rn = 0xf;
					U = SET;
					I = ZERO;
					P = SET;
				}
			}
		} else if (tokens.numberOfElements == 2) {
			if(tokens.words[1][0] == 'r'){
				I = SET;
			} else {
				I = ZERO;
			}
			if (isNeg(tokens.words[1])) {
				U = ZERO;
			} else {
				U = SET;
			}
			P = SET;
			Rn = regCharToInt(tokens.words[0]);
			offset = Operand2Set(tokens.words[1]);
		}
	} else if (tempTokens.numberOfArgs == 4) {
		if (instruction[3][0] == 'r') {
			I = SET;
			U = SET;
		} else {
			I = ZERO;
			if (isNeg(instruction[3])) {
				U = ZERO;
			} else {
				U = SET;
			}
		}
		P = ZERO;
		Rn = regCharToInt(tokens.words[0]);
		offset = Operand2Set(instruction[3]);
	}
	cond = cond << COND_SHIFT;
	I = I << SINGLE_DATA_I_SHIFT;
	U = U << SINGLE_DATA_U_SHIFT;
	P = P << SINGLE_DATA_P_SHIFT;
	L = L << SINGLE_DATA_L_SHIFT;
	Rn = Rn << SINGLE_DATA_RN_SHIFT;
	Rd = Rd << SINGLE_DATA_RD_SHIFT;
	total = cond | SINGLE_DATA_CONST | I | P | U | L | Rn | Rd | offset;
	return total;
}

//-------------------------------------------------------BRANCH----------------------------------------------------------

//get conditions
uint32_t getCond(char* bcond) {
	bcond++;
	if (!strcmp("eq", bcond)) {
		return EQ;
	} else if (!strcmp("ne", bcond)) {
		return NE;
	} else if (!strcmp("ge", bcond)) {
		return GE;
	} else if (!strcmp("lt", bcond)) {
		return LT;
	} else if (!strcmp("gt", bcond)) {
		return GT;
	} else if (!strcmp("le", bcond)) {
		return LE;
	} else {
		return AL;
	}
}

//compare two strings ignoring blank spaces and lines
//returning 0 if true, 1 if false
int stringCompare(char* str1, char* str2) {
	char* str1Temp = strdup(str1);
	char* str2Temp = strdup(str2);
	char* str1XSpace = strtok(str1Temp, " \n");
	char* str2XSpace = strtok(str2Temp, " \n");
	int result = strcmp(str1XSpace, str2XSpace);
	free(str1Temp);
	free(str2Temp);
	return result;
}

//main for branch
uint32_t branch(char** instruction, uint32_t numberofLabels, uint32_t counter) {
	uint32_t total = ZERO;
	uint32_t fill = BRANCHFILL;
	uint32_t cond = getCond(instruction[0]);
	uint32_t offset = ZERO;

	for (uint32_t x = 0; x < numberofLabels; x++) {
		bool cmpResult = !stringCompare(mappings.labels[x], instruction[1]);
		if (cmpResult) {
			offset = sizeof(uint32_t) * (mappings.addresses[x] - counter) - PIPELINE_EFFECT;
			offset = (offset >> BRANCH_OFFSET_SHIFT) & BRANCH_OFFSET;
		}
	}
	cond = cond << COND_SHIFT;
	fill = fill << BRANCH_FILL_SHIFT;
	total = cond | fill | offset;
	return total;
}