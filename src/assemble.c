#include "Instructions.c"

/* ------------------------- START OF HELPER ---------------------------*/

int isLabelLine(char* bufferLine, int rowSize) {
  for (int i = 0; i < rowSize; i++) {
    if (bufferLine[i] == ':') {
      return i;
    }
  }
  return 0;
}

void bracketFinder(char* operand, int operandIndex) {
  for (uint32_t i = 0; i < strlen(operand); i++) {
    if (operand[i] == '[') {
      tempTokens.bracketStart = operandIndex;
    } else if (operand[i] == ']') {
			tempTokens.bracketEnd = operandIndex;
		}
  }
}

uint32_t numberOfArgs(char* instruction, uint32_t rowSize) {
	uint32_t count = 0;
  tempTokens.containsBracket = false;
  tempTokens.bracketHasClosed = true;
  tempTokens.argumentsToCombine = 0;
  for (uint32_t i = 0; i < rowSize; i++) {
    if (instruction[i] == ',') {
      count++;
    }
    if (instruction[i] == '[') {
      tempTokens.containsBracket = true;
      tempTokens.bracketHasClosed = false;
    }
    if (instruction[i] == ',' && tempTokens.bracketHasClosed == false) {
      tempTokens.argumentsToCombine++;
    }
    if (instruction[i] == ']') {
      tempTokens.bracketHasClosed = true;
    }
  }
  tempTokens.argumentsToCombine++;
  return (count);
}

void tokenizer_help() {
  free(tempTokens.concat);
  tempTokens.concat = NULL;
  tempTokens.numberOfArgs = tempTokens.perceivedArgs;
  if (tempTokens.containsBracket == true) {
    for (uint32_t i = 0; i < tempTokens.perceivedArgs; i++) {
      bracketFinder(tempTokens.result[i], i);
    }
		uint32_t length = 0;
    for (uint32_t i = tempTokens.bracketStart; i < tempTokens.bracketEnd + 1; i++) {
      length = length + strlen(tempTokens.result[i]);
    }
    tempTokens.bracketConcat = calloc(length+tempTokens.argumentsToCombine, sizeof(char));
		uint32_t index = 0;
    for (uint32_t i = tempTokens.bracketStart; i < tempTokens.bracketEnd + 1; i++) {
      for (uint32_t j = 0; j < strlen(tempTokens.result[i]); j++) {
        tempTokens.bracketConcat[index] = tempTokens.result[i][j];
        index++;
      }

      if (i != tempTokens.bracketEnd) {
        tempTokens.bracketConcat[index] = ',';
        index++;
      }
      else {
        tempTokens.bracketConcat[index] = '\0';
      }
    }
    free(tempTokens.result[tempTokens.bracketStart]);
    tempTokens.result[tempTokens.bracketStart] = calloc(length+tempTokens.argumentsToCombine, sizeof(char));
    strcpy(tempTokens.result[tempTokens.bracketStart], tempTokens.bracketConcat);
    printf("combined arg: %s \n", tempTokens.result[tempTokens.bracketStart]);

    // Now we need to shift everything past this argument
    tempTokens.numberOfArgs = tempTokens.perceivedArgs - tempTokens.argumentsToCombine + 1;
    for (uint32_t i = tempTokens.numberOfArgs; i < tempTokens.perceivedArgs; i++) {
      free(tempTokens.result[i]);
      tempTokens.result[i] = NULL;
    }
  }
}

void tokenizer(char* instruction, uint32_t rowSize, uint32_t ind) {
  if (ind != 0) {
    if (tempTokens.containsBracket == true) {
      free(tempTokens.bracketConcat);
      tempTokens.bracketConcat = NULL;
    }
    for (int i = 0; i < tempTokens.numberOfArgs; i++) {
      free(tempTokens.result[i]);
      tempTokens.result[i] = NULL;
    }
    free(tempTokens.result);
    tempTokens.result = NULL;
  }
  tempTokens.perceivedArgs = numberOfArgs(instruction, rowSize) + 2;
  tempTokens.result = calloc(tempTokens.perceivedArgs, sizeof(char*));
  char lastChar = instruction[rowSize-1];
  char lastString[1];
  lastString[0] = lastChar;
  char* breaker = " ";
  char* argumentBreaker = ",";

  int numberArgs = numberOfArgs(instruction, rowSize);  // NOTE: This is 1 less than the perceived Args
  instruction[rowSize - 1] = ',';
  char* firstToken = strtok(instruction, breaker);
  tempTokens.result[0] = calloc(strlen(firstToken), sizeof(char));
  strcpy(tempTokens.result[0],  firstToken);

  for (uint32_t i = 0; i < numberArgs; i++) {
    char* token = strtok(NULL, argumentBreaker);
    tempTokens.result[i+1] = calloc(strlen(token), sizeof(char));
    strcpy(tempTokens.result[i+1], token);
  }

  char* token = strtok(NULL, argumentBreaker);

  tempTokens.concat = calloc(strlen(token) + 2, sizeof(char));
  strcpy(tempTokens.concat, token);
  tempTokens.concat[strlen(token)] = lastChar;
  tempTokens.concat[strlen(token)+1] = '\0';
  tempTokens.result[numberArgs+1] = calloc((strlen(token) + 2), sizeof(char));
  strcpy(tempTokens.result[numberArgs+1], tempTokens.concat);

  tokenizer_help();
}

//reduce arguments for optional
void reduceInstruction(char** inst) {
  char* str1 = strtok(inst[3], " \n");
  char* str2 = strtok(inst[4], "\n");
	printf("%s\n", str1);
	printf("%s\n", str2);
  char* combine = malloc(sizeof(char) * 10);
  combine = strcat(combine, "[");
  combine = strcat(combine, str1);
  combine = strcat(combine, ", ");
  combine = strcat(combine, str2);
  combine = strcat(combine, "]");
  inst[3] = combine;
}

/* --------------------------- START OF MAIN ---------------------------*/

int main(int argc, char **argv) {

  // 0. First we take in the file
  assert (argc == 3);
  char *fileName = argv[1];
  char *outputName = argv[2];

  FILE *file;
  file = fopen(fileName, "r");

	uint32_t count = ZERO;
	uint32_t totalChars = ZERO;
	uint32_t currentChars = ZERO;
	uint32_t rowSize = ZERO;
	uint32_t numberOfLabels = ZERO;
  char ch;
  while((ch = fgetc(file)) != EOF) {
    if (ch == ':') {
      numberOfLabels++;
    }
    if (ch == '\n') {
      count++;
      if (currentChars > rowSize) {
        rowSize = currentChars;
      }
      currentChars = 0;
    }
    currentChars++;
    totalChars++;
  }
  rewind(file);

  char buffer[count][rowSize];
  for (uint32_t i = 0; i < count; i++) {
    for (uint32_t j = 0; j < rowSize; j++) {
			// We first pad the whole buffer with spaces
      buffer[i][j] = ' ';
    }
    char ch;
		uint32_t index = ZERO;
    while ((ch = fgetc(file)) != '\n') {
      buffer[i][index] = ch;
      index++;
    }
    buffer[i][index] = '\n';
  }

  // 1. Next we make a symbol table. They are mappings of string labels to int memory.
  // WOOP we are now at this part
  mappings.labels = calloc(numberOfLabels, sizeof(char*));
  mappings.addresses = calloc(numberOfLabels, sizeof(uint8_t));
  mappings.numberOfLabels = numberOfLabels;

  int addressDisplacement = ZERO;
  int labelIndex = ZERO;
  for (uint32_t i = 0; i < count; i++) {
		uint32_t indexOfColon = isLabelLine(buffer[i], rowSize);
    if (indexOfColon != 0) {  // This line is a label
      mappings.labels[labelIndex] = malloc(rowSize * sizeof(char));
      for (uint32_t j = 0; j < rowSize; j++) {
        mappings.labels[labelIndex][j] = ' ';
      }
      memcpy(mappings.labels[labelIndex], buffer[i], (indexOfColon));
      mappings.addresses[labelIndex] = i - addressDisplacement;
      addressDisplacement++;
      labelIndex++;
    }
  }

  // 2. Then we read the opcode and operands for instructions
  // Sort by instruction type and assert the correct number of operands
  // From the buffer, we now REMOVE all mentions to labels. What remains is an array of instructions
  int numberOfInstructions = count - numberOfLabels;
  char arr[numberOfInstructions][rowSize];

	uint32_t arrIndex = ZERO;
  for (uint32_t i = 0; i < count; i++) {
    if (isLabelLine(buffer[i], rowSize) == 0) {
      for (uint32_t j = 0; j < rowSize; j++) {
        arr[arrIndex][j] = buffer[i][j];
      }
      arrIndex++;
    }
  }

  uint32_t result;
	uint32_t resArrCounter = ZERO;

  for (uint32_t i = 0; i < numberOfInstructions; i++) {
    printf("Instruction %d : ", i);
    for (uint32_t j = 0; j < rowSize; j++) {
      printf("%c", arr[i][j]);
    }
  }
  uint32_t *resArr = calloc(numberOfInstructions + 1, sizeof(uint32_t));

  for (uint32_t i = 0; i < numberOfInstructions; i++) {
    tokenizer(arr[i], rowSize, i);
    int isEmptyInstruction = ZERO;
    char** instruction = tempTokens.result;
    char* mnemonic = instruction[0];

    //reduce arguments for opt_add05
    if (tempTokens.numberOfArgs > 4 && (strncmp("m", mnemonic, 1))) {
      reduceInstruction(instruction);
    }

    if (!strcmp("add", mnemonic) || !strcmp("sub", mnemonic) || !strcmp("rsb", mnemonic) ||
        !strcmp("and", mnemonic) || !strcmp("eor", mnemonic) || !strcmp("orr", mnemonic) ||
        !strcmp("mov", mnemonic) || !strcmp("tst", mnemonic) || !strcmp("teq", mnemonic) ||
        !strcmp("cmp", mnemonic) || !strcmp("andeq", mnemonic) || !strcmp("lsl", mnemonic)) {
      printf("running DATA PROCESSING: \n");
      result = dataProcessing(instruction);
    }
    else if (!strcmp("mul", mnemonic) || !strcmp("mla", mnemonic)) {
      printf("running MULTIPLY: \n");
      result = multiply(instruction);
    }
    else if (!strcmp("ldr", mnemonic) || !strcmp("str", mnemonic)) {
      printf("running SINGLE DATA: \n");
      result = singleData(instruction, i, numberOfInstructions);
    }
    else if (!strncmp("b", mnemonic, 1)) {
			printf("running BRANCH: \n");
			result = branch(instruction, numberOfLabels, i);
		}
		else if (!strcmp("end", mnemonic)) {
			printf("adding this to the end \n");
			result = (uint32_t) atoi(instruction[1]);
    } else {
      printf("instruction with nothing detected!");
      isEmptyInstruction = 1;
    }

    //check for empty instruction

    if (isEmptyInstruction) {
			numberOfInstructions--;
    } else {
			resArr[resArrCounter] = result;
			resArrCounter++;

      printf("\noriginal : %08"  PRIx32 "\n", result);
      result = invert(result);
      printf("inverted : %08"  PRIx32 "\n", result);
    }
  }

  FILE *output;

  if ((output = fopen(outputName, "wb")) == NULL) {
    printf("Cannot open output file");
    exit(EXIT_FAILURE);
  }


  for (uint32_t i = 0; i < resArrCounter; i++) {
    uint32_t *val = &resArr[i];
    fwrite(val,1,sizeof(uint32_t), output);
  }

  for (uint32_t j = 0; j < store.size; j++) {
		printf("hi");
    uint32_t check = store.storArr[j];
    uint32_t *ex = &store.storArr[j];
    if (check != 0) {
      fwrite(ex, 1, sizeof(uint32_t), output);
    }
  }
  free(resArr);
  return EXIT_SUCCESS;
}
