//#ifndef Assembler_h
#define Assembler_h

#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define LABLE_MAX 50
#define MAX_LINE 500
#define LABLE_NUM 20 //Max Lables
#define MEMORY_SIZE 4096 //Max Lines

typedef struct Lable {
	char name[LABLE_MAX];
	int position;
}Lable;

typedef struct Memory {
	char opcode[6];
	char rd[6];
	char rs[6];
	char rt[6];
	char imm[50];
}Memory;

int GetLables(FILE *file, Lable *lb);
char *LineContainLable(char *line);
int LineIsEmptyorRemark(char *line);
int LineIsCommand(char *line);
int ParseCommandToMem(Memory *mem, char *line, int index);
int LineIsWordSet(char *line);
int SecoundRun(FILE *file, Memory *Mem, Lable *lb, int lable_num);
void LabelToAddress (Memory *Mem, int i, Lable *lb, int lable_num);
void ParseWordToMem(Memory *mem, char *line);
void PrintDataToFile(Memory *mem, FILE *fPtrWrite, int pos);
int PrintOpCode(char *opcode, FILE *fPtrWrite);
int PrintWord(Memory *mem, int i, FILE *fPtrWrite);
void PrintRegNum(char *reg, FILE *fPtrWrite);
void PrintImmediate(char *imm, FILE *fPtrWrite);

