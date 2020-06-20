#define _CRT_SECURE_NO_WARNINGS
#include "Assembler.h"

int main(int argc, char* argv[])
{
	// Values setting:
	FILE *file1 = NULL, *file2 = NULL, *file3 = NULL; //File pointer
	Lable Lb_struct[LABLE_NUM]; //lable struct array
	Memory Mem[MEMORY_SIZE]; //Memory struct array
	int last_line = 0, lable_num = 0; // Lines & lables counting

	char str[10];
	int num, len;
	num = 1;
	// First run - Lable Reading
	file1 = fopen(argv[1], "r");
	if (file1 == NULL)
		exit(1);
	lable_num = GetLables(file1, Lb_struct);
	fclose(file1);	
	// End of first run
	// Secound run - "Memory reading"	
	file2 = fopen(argv[1], "r");
	if (file2 == NULL)
		exit(1);
	last_line = SecoundRun(file2, Mem, Lb_struct, lable_num);
	fclose(file2);	
	// Write Data to file
	file3 = fopen(argv[2], "w");
	if (file3 == NULL)
		exit(1);
	PrintDataToFile(Mem, file3, last_line);
	fclose(file3);
	// End of file writing
	
}

int GetLables(FILE *file, Lable *lb)
{
	int idx_ch_line = 0, idx_ch_label = 0 ,cnt_lbl = 0, pc = 0, command_line = 0; 	
	char line[MAX_LINE], dots[] = ":", tav, lable_line[LABLE_MAX];	
	while (!feof(file))
	{
		fgets(line, MAX_LINE, file); //read line		
		if (LineContainLable(line) != NULL) {
			// Read Lable and ignore tabs & spaces 
			idx_ch_line = 0;
			idx_ch_label = 0;
			do {
				tav = line[idx_ch_line];
				if ((tav != ':') && (tav != '\t') && (tav != ' ')) {
					lable_line[idx_ch_label] = tav;
					idx_ch_label++;
				}
				idx_ch_line++;
			} while (tav != ':');
			lable_line[idx_ch_label] = '\0';
			//Copy lable information to lable struct
			strcpy(lb[cnt_lbl].name, lable_line);
			lb[cnt_lbl].position = pc;
			//printf("%s \n", lb[cnt_lbl].name);
			//printf("%d \n", lb[cnt_lbl].position);
			cnt_lbl++;
			// Check if the line is lable only or contains a command after			
			while ((line[idx_ch_line] == ' ') || (line[idx_ch_line] == '\t'))//remove spaces & tabs
				idx_ch_line++;
			if ((line[idx_ch_line] == '\n') || (line[idx_ch_line] == '#'))
				continue;
		}

		if (LineIsCommand(line))
			pc++; //The line contains an operation --> increment program counter
			
	}
	return(cnt_lbl); //Return number of lables
}

int LineIsEmptyorRemark(char *line)
{
	int k = 0;
	// If line is blank
	if (strcmp(line, "\n") == 0)
		return 1;
	// If line contains only tabs or spaces
	while ((line[k] == '\t') || (line[k] == ' '))
		k++;
	if (line[k] == '\n')
		return 1;
	// If line is Remark, continue
	if (line[k] == '#')
		return 1;
	return 0;
}

int SecoundRun(FILE *file, Memory *Mem, Lable *lb, int lable_num)
{
	int k = 0, pc = 0, j = 0, s = 0, pos = 0, pos1 = 0;
	char line[MAX_LINE], dots[] = ":", wo[] = ".word", tav, option[6], rd[6], rs[6], rt[6], imm[50], tav1, wordP[15], wordN[15];	
	while (!feof(file))
	{
		if (fgets(line, MAX_LINE, file) == NULL)
			continue;
		if (LineIsCommand(line)) {
			ParseCommandToMem(Mem, line, pc);			
			LabelToAddress(Mem, pc, lb, lable_num);
			pc++;
		}		
		else if (LineIsWordSet(line))
			ParseWordToMem(Mem, line);		
	}	
	return(pc); //Return number of commands
}

int LineIsCommand(char *line) {
	char comma[] = ",", *comma_1 = NULL, *comma_2 = NULL, *comma_3 = NULL;
	// check existence of 3 comma
	comma_1 = strstr(line, comma);
	if (comma_1 == NULL)
		return 0;	
	comma_2 = strstr(comma_1+1, comma);
	if (comma_2 == NULL)
		return 0;
	comma_3 = strstr(comma_2 + 1, comma);
	if (comma_3 == NULL)
		return 0;
	if (strstr(line, "#") != NULL) //Check if the comma's not in comment
		if (comma_3 > (strstr(line, "#")))
			return 0;	
	return 1;
}

int ParseCommandToMem(Memory *mem, char *line, int index)
{
	char tav, option[6], rd[6], rs[6], rt[6], imm[50], tav1, *line_ptr, *label_in_line;
	int k, i, j;
		line_ptr = line; //ptr to beginning of line
	label_in_line = LineContainLable(line);
	if (label_in_line != NULL)
		line_ptr = label_in_line + 1; //ptr to after dots of label
	
	k = 0;
	j = 0;

   // read opcode
	while (line_ptr[k] != '$')
	{
		if ((line_ptr[k] != '\t') && (line_ptr[k] != ' '))
		{
			option[j] = line_ptr[k];
			j++;
		}
		k++;
	}
	option[j] = '\0';
	// Read rd
	j = 0;
	while (line_ptr[k] != ',')
	{
		if ((line_ptr[k] != '\t') && (line_ptr[k] != ' '))
		{
			rd[j] = line_ptr[k];
			j++;
		}
		k++;
	}
	rd[j] = '\0';
	k = k + 1;	
	// Read rs
	j = 0;
	while (line_ptr[k] != ',')
	{
		if ((line_ptr[k] != '\t') && (line_ptr[k] != ' '))
		{
			rs[j] = line_ptr[k];
			j++;
		}
		k++;
	}
	rs[j] = '\0';
	k = k + 1;
	// Read rt
	j = 0;
	while (line_ptr[k] != ',')
	{
		if ((line_ptr[k] != '\t') && (line_ptr[k] != ' '))
		{
			rt[j] = line_ptr[k];
			j++;
		}
		k++;
	}
	rt[j] = '\0';
	k = k + 1;

	// Read immidiate
	j = 0;
	while ((line_ptr[k] != '\n') && (line_ptr[k] != '#'))
	{
		if ((line_ptr[k] != '\t') && (line_ptr[k] != ' '))
		{
			imm[j] = line_ptr[k];
			j++;
		}
		k++;
	}
	imm[j] = '\0';

	// Copy Line to mem structure
	strcpy(mem[index].opcode, option);
	strcpy(mem[index].rd, rd);
	strcpy(mem[index].rt, rt);
	strcpy(mem[index].rs, rs);
	strcpy(mem[index].imm, imm);

}

void LabelToAddress(Memory *Mem, int i, Lable *lb, int lable_num)
{
	Lable *ptrlb;
	int j;
	ptrlb = lb;
	for (j = 0; j < lable_num; j++) {		
		if (!strcmp((Mem + i)->imm, (ptrlb + j)->name))
			sprintf((Mem + i)->imm, "%d", (ptrlb + j)->position);						
	}
}

char *LineContainLable(char *line)
{
	char dots[] = ":", *label = NULL;
	label = strstr(line, dots);
	if (label != NULL) //dots found (potentially label)
		if (strstr(line, "#") != NULL) //Comment found
			if (label > (strstr(line, "#"))) //if dots in comment
				label = NULL; //label in comment
	return label;
}

int LineIsWordSet (char *line){
	char wo[] = ".word", *word_ptr = NULL;
	int isWordSet = 0;
	word_ptr = strstr(line, wo);
	if (word_ptr == NULL) //Check if ".word" not exist
		return 0;
	if (strstr(line, "#") != NULL) //Check if ".word" in comment
		if (word_ptr > (strstr(line, "#")))
			return 0;
	return 1;
}

void ParseWordToMem(Memory *mem, char *line)
{
	int k = 0, j = 0;
	long addr;
	char *lineptr;
	char wo[] = ".word", word_addr[15], word_data[15];
	lineptr = strstr(line, wo)+5;
	// Extract address
	while ((lineptr[k] == ' ') || (lineptr[k] == '\t'))
		k++;
	j = 0;
	while (line[k] != ' ')
	{
		word_addr[j] = lineptr[k];
		j++;
		k++;
	}
	word_addr[j] = '\0';		
	if ((strstr(word_addr,"0x")!=NULL)||(strstr(word_addr, "0X") != NULL))
		addr = strtol(word_addr, NULL, 16);
	else
		addr = strtol(word_addr, NULL, 10);
	
	while ((lineptr[k] == ' ') || (lineptr[k] == '\t'))
		k++;
	//Extract Data
	j = 0; 
	while (lineptr[k] != ' ')
	{
		if (lineptr[k] == '\n')
			break;
		word_data[j] = lineptr[k];
		j++;
		k++;
	}
	word_data[j] = '\0';
	// Change adress
	strcpy(mem[addr].opcode, "WORD");
	strcpy(mem[addr].rd, "NAN");
	strcpy(mem[addr].rs, "NAN");
	strcpy(mem[addr].rt, "NAN");
	strcpy(mem[addr].imm, word_data);
}

void PrintDataToFile(Memory *mem, FILE *fPtrWrite, int pos)
{
	char *opcode;
	int ret_val;
	int i = 0, num = 0;	
	for (i; i <= MEMORY_SIZE; i++)
	{
		// Print Opcode
		opcode = mem[i].opcode;
		ret_val = PrintOpCode(opcode, fPtrWrite); //Print opcode command
		if (!ret_val) {
			PrintRegNum(mem[i].rd, fPtrWrite);
			PrintRegNum(mem[i].rs, fPtrWrite);
			PrintRegNum(mem[i].rt, fPtrWrite);
			PrintImmediate(mem[i].imm, fPtrWrite);
			fprintf(fPtrWrite, "\n");
			continue;
		}
		ret_val = PrintWord(mem, i, fPtrWrite); //Print WORD
		if (!ret_val) {			
			fprintf(fPtrWrite, "\n");
			continue;
		}
		fprintf(fPtrWrite, "00000000"); //Print zeros to mem
		fprintf(fPtrWrite, "\n");
		continue;				
		/*
		if (strcmp(mem[i].rd, "$zero") == 0) // Printing Rd
			fprintf(fPtrWrite, "0");
		else if (strcmp(mem[i].rd, "$imm") == 0)
			fprintf(fPtrWrite, "1");
		else if (strcmp(mem[i].rd, "$v0") == 0)
			fprintf(fPtrWrite, "2");
		else if (strcmp(mem[i].rd, "$a0") == 0)
			fprintf(fPtrWrite, "3");
		else if (strcmp(mem[i].rd, "$a1") == 0)
			fprintf(fPtrWrite, "4");
		else if (strcmp(mem[i].rd, "$t0") == 0)
			fprintf(fPtrWrite, "5");
		else if (strcmp(mem[i].rd, "$t1") == 0)
			fprintf(fPtrWrite, "6");
		else if (strcmp(mem[i].rd, "$t2") == 0)
			fprintf(fPtrWrite, "7");
		else if (strcmp(mem[i].rd, "$t3") == 0)
			fprintf(fPtrWrite, "8");
		else if (strcmp(mem[i].rd, "$s0") == 0)
			fprintf(fPtrWrite, "9");
		else if (strcmp(mem[i].rd, "$s1") == 0)
			fprintf(fPtrWrite, "A");
		else if (strcmp(mem[i].rd, "$s2") == 0)
			fprintf(fPtrWrite, "B");
		else if (strcmp(mem[i].rd, "$gp") == 0)
			fprintf(fPtrWrite, "C");
		else if (strcmp(mem[i].rd, "$sp") == 0)
			fprintf(fPtrWrite, "D");
		else if (strcmp(mem[i].rd, "$fp") == 0)
			fprintf(fPtrWrite, "E");
		else if (strcmp(mem[i].rd, "$ra") == 0)
			fprintf(fPtrWrite, "F");

		if (strcmp(mem[i].rs, "$zero") == 0) // Printing Rs
			fprintf(fPtrWrite, "0");
		else if (strcmp(mem[i].rs, "$at") == 0)
			fprintf(fPtrWrite, "1");
		else if (strcmp(mem[i].rs, "$v0") == 0)
			fprintf(fPtrWrite, "2");
		else if (strcmp(mem[i].rs, "$a0") == 0)
			fprintf(fPtrWrite, "3");
		else if (strcmp(mem[i].rs, "$a1") == 0)
			fprintf(fPtrWrite, "4");
		else if (strcmp(mem[i].rs, "$t0") == 0)
			fprintf(fPtrWrite, "5");
		else if (strcmp(mem[i].rs, "$t1") == 0)
			fprintf(fPtrWrite, "6");
		else if (strcmp(mem[i].rs, "$t2") == 0)
			fprintf(fPtrWrite, "7");
		else if (strcmp(mem[i].rs, "$t3") == 0)
			fprintf(fPtrWrite, "8");
		else if (strcmp(mem[i].rs, "$s0") == 0)
			fprintf(fPtrWrite, "9");
		else if (strcmp(mem[i].rs, "$s1") == 0)
			fprintf(fPtrWrite, "A");
		else if (strcmp(mem[i].rs, "$s2") == 0)
			fprintf(fPtrWrite, "B");
		else if (strcmp(mem[i].rs, "$gp") == 0)
			fprintf(fPtrWrite, "C");
		else if (strcmp(mem[i].rs, "$sp") == 0)
			fprintf(fPtrWrite, "D");
		else if (strcmp(mem[i].rs, "$fp") == 0)
			fprintf(fPtrWrite, "E");
		else if (strcmp(mem[i].rs, "$ra") == 0)
			fprintf(fPtrWrite, "F");
		else if (strcmp(mem[i].rs, "NONO") == 0)
			num = 0;
		else
			fprintf(fPtrWrite, "0");


		if (strcmp(mem[i].rt, "$zero") == 0) // Printing Rt
			fprintf(fPtrWrite, "0");
		else if (strcmp(mem[i].rt, "$at") == 0)
			fprintf(fPtrWrite, "1");
		else if (strcmp(mem[i].rt, "$v0") == 0)
			fprintf(fPtrWrite, "2");
		else if (strcmp(mem[i].rt, "$a0") == 0)
			fprintf(fPtrWrite, "3");
		else if (strcmp(mem[i].rt, "$a1") == 0)
			fprintf(fPtrWrite, "4");
		else if (strcmp(mem[i].rt, "$t0") == 0)
			fprintf(fPtrWrite, "5");
		else if (strcmp(mem[i].rt, "$t1") == 0)
			fprintf(fPtrWrite, "6");
		else if (strcmp(mem[i].rt, "$t2") == 0)
			fprintf(fPtrWrite, "7");
		else if (strcmp(mem[i].rt, "$t3") == 0)
			fprintf(fPtrWrite, "8");
		else if (strcmp(mem[i].rt, "$s0") == 0)
			fprintf(fPtrWrite, "9");
		else if (strcmp(mem[i].rt, "$s1") == 0)
			fprintf(fPtrWrite, "A");
		else if (strcmp(mem[i].rt, "$s2") == 0)
			fprintf(fPtrWrite, "B");
		else if (strcmp(mem[i].rt, "$gp") == 0)
			fprintf(fPtrWrite, "C");
		else if (strcmp(mem[i].rt, "$sp") == 0)
			fprintf(fPtrWrite, "D");
		else if (strcmp(mem[i].rt, "$fp") == 0)
			fprintf(fPtrWrite, "E");
		else if (strcmp(mem[i].rt, "$ra") == 0)
			fprintf(fPtrWrite, "F");
		else if (strcmp(mem[i].rt, "NONO") == 0)
			num = 0;
		else
			fprintf(fPtrWrite, "0");

		if (strcmp(mem[i].opcode, "NONO") != 0) //If order is not .word, print num
		{
			if ((mem[i].imm[0] == '0') && (mem[i].imm[1] == 'x')) //Check if immidiate in hex
			{
				num = strtol(mem[i].imm, NULL, 16);
			}
			else //Imiddiate is decimal
			{
				num = atoi(mem[i].imm);
			}

			//Print immidiate in hex
			fprintf(fPtrWrite, "%04X", num);
		}

		if (i < pos) //Print \n except the last line
			fprintf(fPtrWrite, "\n");
		*/
	}
}

int PrintOpCode(char *opcode, FILE *fPtrWrite)
{
	int ret_val = 0;
	if (strcmp(opcode, "add") == 0) // Printing Opcode
		fprintf(fPtrWrite, "00");
	else if (strcmp(opcode, "sub") == 0)
		fprintf(fPtrWrite, "01");
	else if (strcmp(opcode, "and") == 0)
		fprintf(fPtrWrite, "02");
	else if (strcmp(opcode, "or") == 0)
		fprintf(fPtrWrite, "03");
	else if (strcmp(opcode, "sll") == 0)
		fprintf(fPtrWrite, "04");
	else if (strcmp(opcode, "sra") == 0)
		fprintf(fPtrWrite, "05");
	else if (strcmp(opcode, "srl") == 0)
		fprintf(fPtrWrite, "06");
	else if (strcmp(opcode, "beq") == 0)
		fprintf(fPtrWrite, "07");
	else if (strcmp(opcode, "bne") == 0)
		fprintf(fPtrWrite, "08");
	else if (strcmp(opcode, "blt") == 0)
		fprintf(fPtrWrite, "09");
	else if (strcmp(opcode, "bgt") == 0)
		fprintf(fPtrWrite, "0A");
	else if (strcmp(opcode, "ble") == 0)
		fprintf(fPtrWrite, "0B");
	else if (strcmp(opcode, "bge") == 0)
		fprintf(fPtrWrite, "0C");
	else if (strcmp(opcode, "jal") == 0)
		fprintf(fPtrWrite, "0D");
	else if (strcmp(opcode, "lw") == 0)
		fprintf(fPtrWrite, "0E");
	else if (strcmp(opcode, "sw") == 0)
		fprintf(fPtrWrite, "0F");
	else if (strcmp(opcode, "reti") == 0)
		fprintf(fPtrWrite, "10");
	else if (strcmp(opcode, "in") == 0)
		fprintf(fPtrWrite, "11");
	else if (strcmp(opcode, "out") == 0)
		fprintf(fPtrWrite, "12");
	else if (strcmp(opcode, "halt") == 0)
		fprintf(fPtrWrite, "13");
	else
		ret_val = 1;
	return ret_val;


}
 
int PrintWord(Memory *mem, int i, FILE *fPtrWrite)
{	
	int ret_val, num;
	if (strcmp(mem[i].opcode, "WORD") == 0) {
		if ((mem[i].imm[0] == '0') && (mem[i].imm[1] == 'x')) //Check if immidiate in hex			
			num = strtol(mem[i].imm, NULL, 16);
		else //immidiate is decimal			
			num = atoi(mem[i].imm);
		//Print immidiate in hex
		fprintf(fPtrWrite, "%08X", num);
		return 0;
	}
	return 1;
}

void PrintRegNum(char *reg, FILE *fPtrWrite)
{
	if (strcmp(reg, "$zero") == 0) // Printing Rd
		fprintf(fPtrWrite, "0");
	else if (strcmp(reg, "$imm") == 0)
		fprintf(fPtrWrite, "1");
	else if (strcmp(reg, "$v0") == 0)
		fprintf(fPtrWrite, "2");
	else if (strcmp(reg, "$a0") == 0)
		fprintf(fPtrWrite, "3");
	else if (strcmp(reg, "$a1") == 0)
		fprintf(fPtrWrite, "4");
	else if (strcmp(reg, "$t0") == 0)
		fprintf(fPtrWrite, "5");
	else if (strcmp(reg, "$t1") == 0)
		fprintf(fPtrWrite, "6");
	else if (strcmp(reg, "$t2") == 0)
		fprintf(fPtrWrite, "7");
	else if (strcmp(reg, "$t3") == 0)
		fprintf(fPtrWrite, "8");
	else if (strcmp(reg, "$s0") == 0)
		fprintf(fPtrWrite, "9");
	else if (strcmp(reg, "$s1") == 0)
		fprintf(fPtrWrite, "A");
	else if (strcmp(reg, "$s2") == 0)
		fprintf(fPtrWrite, "B");
	else if (strcmp(reg, "$gp") == 0)
		fprintf(fPtrWrite, "C");
	else if (strcmp(reg, "$sp") == 0)
		fprintf(fPtrWrite, "D");
	else if (strcmp(reg, "$fp") == 0)
		fprintf(fPtrWrite, "E");
	else if (strcmp(reg, "$ra") == 0)
		fprintf(fPtrWrite, "F");
}
void PrintImmediate(char *imm, FILE *fPtrWrite)
{
	char str[12];
	int num, len;
	if ((strstr(imm, "0x") != NULL) || (strstr(imm, "0X") != NULL)) //Check if immidiate in hex			
		num = strtol(imm, NULL, 16);
	else //immidiate is decimal			
		num = atoi(imm);
	//Print immidiate in hex		
	sprintf(str, "%03X", num);
	len = strlen(str);
	fprintf(fPtrWrite, str + (len - 3));
}