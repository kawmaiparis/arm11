#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "emulate.h"

// ----------------------------- NEW HELPER FUNCTIONS -------------------------------

uint32_t getBitMask() {
  uint32_t result = states.memory[states.regs[15]] | states.memory[states.regs[15]+1]<<8 | states.memory[states.regs[15]+2]<<16 | states.memory[states.regs[15]+3]<<24;
  return result;
}

//Masking function
uint32_t getBits(uint32_t value, uint32_t start_index, uint32_t size) {
  uint32_t mask = NON_ZERO_PADDING;
  uint32_t res;
  mask = mask >> (NUMBER_OF_BITS-size);
  mask = mask << (NUMBER_OF_BITS- start_index - size);
  res = (value & mask);
  res = res >> (NUMBER_OF_BITS- start_index - size);
  return res;
}

uint32_t makeMask(uint32_t val) {
  uint32_t mask = NON_ZERO_PADDING;
  mask = mask << (NUMBER_OF_BITS  - val);
  return mask;
}

uint32_t rotateRight(uint32_t operand, uint32_t rotationAmount) {
  return (operand >> rotationAmount) | (operand << (NUMBER_OF_BITS - rotationAmount));
}

void printResults() {
  printf("Registers:\n");
  for (uint32_t i = 0; i < 17; i++) {
    if (i < 10) {
      printf("$%d  : %10d (0x%08x)\n", i, states.regs[i], states.regs[i]);
    } else if (i < 13 && i >= 10) {
      printf("$%d : %10d (0x%08x)\n", i, states.regs[i], states.regs[i]);
    } else if (i == 15) {
      printf("PC  : %10d (0x%08x)\n", states.regs[i], states.regs[i]);
    } else if (i == 16) {
      printf("CPSR: %10d (0x%08x)\n", states.regs[i], states.regs[i]);
    }
  }
}

uint32_t getWord(uint32_t address) {
	uint32_t ret = 0;
	for (uint32_t i = 0; i < BYTES_PER_WORD; i++) {
		ret += states.memory[address + i];
		if (i < BYTES_PER_WORD - 1) {
			ret <<= 8;
		}
	}
	return ret;
}

void printNonZeroMem() {
	int foundNonZero = 0;
	for (uint32_t i = 0; i < MEMORY_SIZE; i += 4) {
		uint32_t mem = getWord(i);
		if (mem != 0) {
			if (!foundNonZero) {
				printf("Non-zero memory:\n");
				foundNonZero = 1;
			}
			printf("0x%08x: ", i);
			printf("%#010x\n", mem);
		}
	}
}

int isGpioProcess(uint32_t address) {
  switch (address) {
    case 0x20200000:
    printf("One GPIO pin from 0 to 9 has been accessed\n");
    break;
    case 0x20200004:
    printf("One GPIO pin from 10 to 19 has been accessed\n");
    break;
    case 0x20200008:
    printf("One GPIO pin from 20 to 29 has been accessed\n");
    break;
    case 0x20200028:
    printf("PIN OFF\n");
    break;
    case 0x2020001c:
    printf("PIN ON\n");
    break;
    default :
    return 0;
  }
  return 1;
}
// -------------------------- END OF HELPER FUNCTIONS --------------------------------

// -------------------------- DECODE/EXECUTE FUNCTIONS AND SUBFUNCTIONS --------------

uint32_t decode(uint32_t fetched) {
  return fetched;
}


uint32_t check(uint32_t cond, uint32_t cpsr) {
  //boolean is 1 if true, is 0 if false
  uint32_t boolean = 0;
  uint32_t toInts = getBits(cond, 0, 4);

    switch (toInts) {
      case 0: boolean = (getBits(cpsr, 1, 1) == 1); break;
      case 1: boolean = (getBits(cpsr, 1, 1) == 0); break;
      case 10: boolean = (getBits(cpsr, 0, 1) == getBits(cpsr, 2, 1)); break;
      case 11: boolean = (getBits(cpsr, 0, 1) != getBits(cpsr, 2, 1)); break;
      case 12: boolean = (getBits(cpsr, 1, 1) == 0 && getBits(cpsr, 0, 1) == getBits(cpsr, 3, 1)); break;
      case 13: boolean = (getBits(cpsr, 1, 1) == 1 || getBits(cpsr, 0, 1) != getBits(cpsr, 3, 1)); break;
      case 14: boolean = 1; break;
      default: break;
    }
    return boolean;
}

//sets and clears the respective flags in the CSPR register;
void setFlag(char c, uint32_t val) {
  switch (val) {
    case 0:
    switch (c) {
      case 'N': states.regs[16] &= 0x70000000; break;
      case 'Z': states.regs[16] &= 0xB0000000; break;
      case 'C': states.regs[16] &= 0xD0000000; break;
      case 'V': states.regs[16] &= 0xE0000000; break;
      default : break;
    } break;
    case 1:
    switch (c) {
      case 'N': states.regs[16] |= 0x80000000; break;
      case 'Z': states.regs[16] |= 0x40000000; break;
      case 'C': states.regs[16] |= 0x20000000; break;
      case 'V': states.regs[16] |= 0x10000000; break;
      default: break;
    } break;
    default: break;
  }
}

uint32_t shifter(uint32_t shiftType, uint32_t content, uint32_t shiftAmount) {
  uint32_t operand = 0;
  if (shiftType == 0) {
    //logical left
    operand = content << shiftAmount;

  } else if (shiftType == 1) {
    operand = content >> shiftAmount;

  } else if (shiftType == 2) {
    //need to preserve the sign bit i.e. the first bit
    if (getBits(content, 0, 1) == 0) {
      operand = content >> (shiftAmount);
    } else {
      operand = (content >> shiftAmount) | makeMask(shiftAmount);
    }

  } else if (shiftType == 3) {
    operand = rotateRight(content , shiftAmount);
  }
  return operand;
}

void multiply(uint32_t instruction) {
  uint32_t a = getBits(instruction, 10, 1);
  uint32_t s = getBits(instruction, 11, 1);
  uint32_t Rd = getBits(instruction, 12, 4);
  uint32_t Rn = getBits(instruction, 16, 4);
  uint32_t Rs = getBits(instruction, 20, 4);
  uint32_t Rm = getBits(instruction, 28, 4);
  uint32_t result;

  if (a == true) {
    result = states.regs[Rm] * states.regs[Rs] + Rn;
  } else {
    result = states.regs[Rm] * states.regs[Rs];
  }
  states.regs[Rd] = result;
  if (s == 1) {
    setFlag('N', getBits(result, 0, 1));
    setFlag('Z', result == 0x00000000);
  }
}


void dataProcessing(uint32_t instruction) {
  uint32_t opcode = getBits(instruction, 7, 4);
  uint32_t Rn = getBits(instruction, 12, 4);
  uint32_t Rd = getBits(instruction, 16, 4);
  uint32_t I = getBits(instruction, 6, 1);
  uint32_t S = getBits(instruction, 11, 1);
  uint32_t operand2 = getBits(instruction, 20, 12);
  uint32_t bit = getBits(instruction, 27, 1);
  uint32_t Rm = getBits(instruction, 28, 4);
  uint32_t shiftType = getBits(instruction, 25, 2);
  uint32_t sourceData = states.regs[Rn];
  uint32_t regData = states.regs[Rm];
  uint32_t result = 0;
  uint32_t isWritten = 1;
  uint32_t carryOut = 0;
  uint32_t shiftAmount = 0;
  uint32_t Rs = 0;
    switch (bit) {
      case 0:
        shiftAmount = getBits(instruction, 20, 5);
        break;
      case 1:
        Rs = getBits(instruction, 20, 4);
        shiftAmount = getBits(states.regs[Rs], 28, 4);
        break;
      default:
        break;
    }
    // if I = 1 then Operand2 is an immediate constant
    // if I = 0 then Operand2 is a shifted register
    if (I == 1) {
      //rotate is twice the value
      uint32_t rotate = (getBits(operand2, 20, 4)) * 2;
      operand2 = rotateRight(getBits(operand2, 24, 8), rotate);
    } else {
      operand2 = shifter(shiftType, regData, shiftAmount);
    }

    //determines carryOut once a shift has occurred
    switch(shiftType) {
      case 0: carryOut = getBits(regData, shiftAmount, 1); break;
      default : carryOut = getBits(regData, 32 - shiftAmount, 1); break;
    }

    switch (opcode) {
      case 0: // Rn AND operand2 // and
        result = sourceData & operand2;
        break;
      case 1: // Rn EOR operand2 //eor
        result = sourceData ^ operand2;
        break;
      case 2: // Rn - operand2 //sub
        result = sourceData - operand2;
        break;
      case 3: // operand2 - Rn //rsb
        result = operand2 - sourceData;
        break;
      case 4: // Rn + operand2 //add
        result = sourceData + operand2;
        break;
      case 8: // as and, but result not written // tst
        result = sourceData & operand2;
        isWritten = 0;
        break;
      case 9: // as eor, but result is not written //teq
        result = sourceData ^ operand2;
        isWritten = 0;
        break;
      case 10: // as sub, but result is not written //cmp
        result = sourceData - operand2;
        isWritten = 0;
        break;
      case 12: // Rn OR operand2 //orr
        result = sourceData | operand2;
        break;
      case 13: // operand2 (Rn is ignored) //mov
        result = operand2;
        break;
      default:
        break;
    }
    //updates the CSPR flags if the S bit is set
    if (S == 1) {
      if (result == 0) {
        setFlag('Z', 1);
      } else {
        setFlag('Z', 0);
      }
      setFlag('N', getBits(result, 0, 1));
      switch(opcode) {
        case 0:
        case 1:
        case 8:
        case 9:
        case 12:
        case 13:
          setFlag('C', carryOut);
        break;
        case 4:
          if (result < sourceData) {
            setFlag('C', 1);
          } else {
            setFlag('C', 0);
          }
          break;
        case 2:
        case 10:
          if (result > sourceData) {
            setFlag('C', 0);
          } else {
            setFlag('C', 1);
          }
          break;
        case 3:
          if (result > operand2) {
            setFlag('C', 0);
          } else {
            setFlag('C', 1);
          }
          break;
      }
    }
    // writes the result to the destination register;
    if (isWritten == 1) {
    states.regs[Rd] = result;
  }
}

void branch(uint32_t instruction) {
  uint32_t offset = getBits(instruction, 8, 24);
  uint32_t signBit = getBits(instruction, 10, 1);
  offset = offset << 2;
  offset = getBits(offset, 8, 24);
  if (signBit) {
    offset |= makeMask(8);
  }
  states.regs[15] += offset;
}

uint32_t fetch (uint32_t loc) {
  uint32_t res, res0, res1, res2, res3;
  res0 = states.memory[loc];
  res1 = states.memory[loc + 1];
  res2 = states.memory[loc + 2];
  res3 = states.memory[loc + 3];
  res = (res0) | (res1 << 8) | (res2 << 16) | (res3 << 24);
  return res;
}

void store (uint32_t value, uint32_t loc) {
  uint32_t val0, val1, val2, val3;
  val0 = (value & 0xFF000000) >> 24;
  val1 = (value & 0x00FF0000) >> 16;
  val2 = (value & 0x0000FF00) >> 8;
  val3 = value & 0x000000FF;
  states.memory[loc] = val3;
  states.memory[loc + 1] = val2;
  states.memory[loc + 2] = val1;
  states.memory[loc + 3] = val0;
}

void singleData(uint32_t instruction) {
  //I: 1 = Offset is shifted register, 0 = Offset is 12 bit immediate
  uint32_t I = getBits(instruction, 6, 1);

  // P: 1 = PRE -> apply offset before transferring, 0 = POST -> after
  uint32_t P = getBits(instruction, 7, 1);

  // U: 1 = ADD offset to the base register, 0 = SUBTRACT offset to the base register
  uint32_t U = getBits(instruction, 8, 1);

  // L: 1 = word is LOADED from memory, 0 = word is STORED in memory
  uint32_t L = getBits(instruction, 11, 1);

  uint32_t offset = getBits(instruction, 20, 12);
  uint32_t Rn = getBits(instruction, 12, 4);
  uint32_t bit = getBits(instruction, 27, 1);
  uint32_t Rm = getBits(instruction, 28, 4);
  uint32_t shiftType = getBits(instruction, 25, 2);
  uint32_t regData = states.regs[Rm];
  uint32_t shiftAmount = 0;
  uint32_t Rs = 0;

  switch (bit) {
    case 0:
      shiftAmount = getBits(instruction, 20, 5);
      break;

    //Optional case
    case 1:
      Rs = getBits(instruction, 20, 4);
      shiftAmount = getBits(states.regs[Rs], 28, 4);
      break;
  }

  if (I == 0) {
    uint32_t rotate = (getBits(offset, 20, 4)) * 2;
    offset = rotateRight(getBits(offset, 24, 8), rotate);
  } else {
    offset = shifter(shiftType, regData, shiftAmount);
  }

  uint32_t Rd = getBits(instruction, 16, 4);
  uint32_t loc = 0;
  int test = states.regs[Rn] + offset;
  int Gpio = isGpioProcess(test);

  if (Gpio) {
    if (U == 1) {
      if (L == 1) {
        if (P == 1) {
          loc = states.regs[Rn] + offset;
          if (loc > 0xFF) {
            loc = states.regs[15] + 4;
          }
          states.regs[Rd] = fetch(loc);
        }
      }
    }
  } else {
  if (U == 1){
    if (L == 1){
      if (P == 1) {
        loc = states.regs[Rn] + offset;
        if (loc > 65536) {
          printf("Error: Out of bounds memory access at address 0x%08x\n", loc);
        } else {
          states.regs[Rd] = fetch(loc);
        }
      } else {
        loc = states.regs[Rn];
        if (loc > 65536) {
          printf("Error: Out of bounds memory access at address 0x%08x\n", loc);
        } else {
          states.regs[Rd] = fetch(loc);
          states.regs[Rn] = states.regs[Rn] + offset;
        }
      }
    } else {
      if (P == 1) {
        loc = states.regs[Rn] + offset;
        store(states.regs[Rd], loc);
      } else {
        loc = states.regs[Rn];
        store(states.regs[Rd], loc);
        states.regs[Rn] = states.regs[Rn] + offset;
      }
    }
  } else {
    if (L == 1) {
      if (P == 1) {
        loc = states.regs[Rn] - offset;
        if (loc > 65536) {
          printf("Error: Out of bounds memory access at address 0x%08x\n", loc);
        } else {
          states.regs[Rd] = fetch(loc);
        }
      } else {
        loc = states.regs[Rn];
        if (loc > 65536) {
          printf("Error: Out of bounds memory access at address 0x%08x\n", loc);
        } else {
          states.regs[Rd] = fetch(loc);
          states.regs[Rn] = states.regs[Rn] - offset;
        }
      }
    } else {
      if (P == 1) {
        loc = states.regs[Rn] - offset;
        store(states.regs[Rd], loc);
      } else {
        loc = states.regs[Rn];
        store(states.regs[Rd], loc);
        states.regs[Rn] = states.regs[Rn] - offset;
      }
    }
  }
 }
}

void runInstruction(uint32_t instruction) {
  if (getBits(instruction, 4, 2) == 0) {
    if (getBits(instruction, 24, 4) == 9) {
      multiply(instruction);
      //printf("\n We have done Multiply!");
    } else {
      dataProcessing(instruction);
      //printf("\n We have done Data Processing!");
    }
  } else if (getBits(instruction, 4, 2) == 1) {
    singleData(instruction);
    //printf("\n We have done Single Return!");
  } else if (getBits(instruction, 4, 2) == 2) {
    branch(instruction);
    //printf("\n We have done Branch!");
  }
  else {
    printf("\n Warning, not a recognized Instruction!");
  }
}

void execute() {
  if (check(states.decoded, states.regs[16]) == true) {
    runInstruction(states.decoded);
  }
  else {
    //printf("\n Cond failed, Did Not Execute!");
  }
}

// -------------------------- END OF DECODE/EXECUTE FUNCTIONS AND SUBFUNCTIONS ------

// -------------------------- START OF MAIN -----------------------------------------

uint32_t main(uint32_t argc, char **argv) {
  assert (argc == 2);
  char *fileName = argv[1]; //This is the name of our binary file

  FILE *file;
  file = fopen(fileName, "rb"); // r for read, b for binary

  uint32_t count = 0;
  while(fgetc(file) != EOF) {
    count++;
  }
  file = fopen(fileName, "rb");

  unsigned char buffer[count];
  fread(buffer,sizeof(buffer),1,file); //Read bytes to our buffer

  uint32_t numberOfInstructions = count/4;

  states.memory = (uint8_t*)calloc(MEMORY_SIZE, sizeof(uint8_t));
  for (uint32_t i = 0; i < numberOfInstructions; i++) {
    states.memory[i*4] = buffer[i*4];
    states.memory[i*4 + 1] = buffer[i*4 + 1];
    states.memory[i*4 + 2] = buffer[i*4 + 2];
    states.memory[i*4 + 3] = buffer[i*4 + 3];
  }

  //initialize the cpu
  for (uint32_t i = 0; i < 17; i++) {
    states.regs[i] = ZERO_PADDING;
  }
  states.fetched = NON_ZERO_PADDING;
  states.decoded = NON_ZERO_PADDING;
  bool fetchHasItem = false;
  bool decodeHasItem = false;

  uint32_t noOfExecutesLeft = numberOfInstructions;
  uint32_t n = 0;
  uint32_t debug = 0;

  while (states.decoded != 0) {
    if (fetchHasItem == false && decodeHasItem == false) {
      states.fetched = getBitMask();
      if (debug) {
        printf("PC before increment 0x%08x\n", states.regs[15]);
        printf("\n %d: decoded is now: %x \n", n, states.decoded);
        printf(" %d: fetched is now: %x \n", n, states.fetched);
        printf("contents of r1 are 0x%08x\n", states.regs[1]);
        printf("contents of r2 are 0x%08x\n", states.regs[2]);
      }
      states.regs[15] = states.regs[15] + 4;   // Increment PC by
      if (debug) {
        printf("PC after increment 0x%08x\n", states.regs[15]);
      }
      fetchHasItem = true;
      n++;
      if (debug) {
        printf(" instructionsYetCalled: %d \n", noOfExecutesLeft);
      }
    }
    else if (fetchHasItem == true && decodeHasItem == false) {
      states.decoded = decode(states.fetched);
      if (debug) {
        printf("PC before increment 0x%08x\n", states.regs[15]);
        printf("\n %d: decoded is now: %x \n", n, states.decoded);
      }
      states.fetched = getBitMask();
      if (debug) {
        printf(" %d: fetched is now: %x \n", n, states.fetched);
        printf("contents of r1 are 0x%08x\n", states.regs[1]);
        printf("contents of r2 are 0x%08x\n", states.regs[2]);
      }
      states.regs[15] = states.regs[15] + 4;   // Increment PC by 4
      if (debug) {
        printf("PC after increment 0x%08x\n", states.regs[15]);
      }
      decodeHasItem = true;
      n++;
      if (debug) {
        printf(" instructionsYetCalled: %d \n", noOfExecutesLeft);
      }
    }
    else if (fetchHasItem == true) {
      if (debug) {
          printf("PC before increment 0x%08x\n", states.regs[15]);
      }
      if (states.decoded != NON_ZERO_PADDING && check(states.decoded, states.regs[16]) == true && (getBits(states.decoded, 4, 2) == 2)) {
        execute();
        states.decoded = NON_ZERO_PADDING;
        if  (debug) {
          printf("\n %d: decoded is now: %x \n", n, states.decoded);
        }
        states.fetched = getBitMask();
        if (debug) {
          printf(" %d: fetched is now: %x \n", n, states.fetched);
          printf("contents of r1 are 0x%08x\n", states.regs[1]);
          printf("contents of r2 are 0x%08x\n", states.regs[2]);
        }
        fetchHasItem = true;
        decodeHasItem = false;
        noOfExecutesLeft = noOfExecutesLeft - 2;
      } else {
        execute();
        states.decoded = states.fetched;
        states.fetched = getBitMask();
        if (debug) {
          printf("\n %d: decoded is now: %x \n", n, states.decoded);
          printf(" %d: fetched is now: %x \n", n, states.fetched);
          printf("contents of r1 are 0x%08x\n", states.regs[1]);
          printf("contents of r2 are 0x%08x\n", states.regs[2]);
          printf("state of cspr 0x%08x\n", states.regs[16]);
        }
        fetchHasItem = true;
        decodeHasItem = true;
        noOfExecutesLeft --;
      }
      states.regs[15] = states.regs[15] + 4;
      if (debug) {
        printf("PC after increment 0x%08x\n", states.regs[15]);
      }
      n++;
      if (debug) {
        printf(" instructionsYetCalled: %d \n", noOfExecutesLeft);
      }
    }
    else {
      printf("Something has gone horribly wrong\n");
    }
  }

  printResults();
  printNonZeroMem();

  return 0;
}

// -------------------------- END OF MAIN -----------------------------------------
