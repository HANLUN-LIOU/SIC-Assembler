// SIC pass1&2 assembler
// Author: 4110056029
// Time: 2023/06/09
// This program can complete pass1 and pass2, then convert source code (assembly language) into object code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define length 10

FILE *fsource;
FILE *fopcode;
FILE *finter;
FILE *fobprogram;

int objLength=0,symcount=0;
struct symtab{
	char symbol[10];
	int val;
}sym[1000];

void pass1();
void pass2();
 
int main(){
	
	pass1();
	pass2();
	
	return 0;
}	
void pass1(){
	//open file
	fsource=fopen("source.txt","r");
	fopcode=fopen("opcode.txt","r");
	finter=fopen("intermediate.txt","w");

	char LABEL[10] , OPCODE[10] , OPERAND[10] ,COMMENT[100];
	int i,sta_add,startAddress;
	int LOCCTR,line=1,opline=0;
	char c;
	struct optab{
		char opcode[10];
		int val;
	}op[1000];
	
	//load opcode
	do{
		fscanf(fopcode,"%s%X",op[opline].opcode,&op[opline].val);	
		opline++;
		c=fgetc(fopcode);
	}while(c!=EOF);

	//read first line
	fscanf(fsource, "%s%s%X",LABEL,OPCODE,&sta_add);
		
	if(strcmp(OPCODE, "START")==0){
		startAddress = sta_add;
		LOCCTR = startAddress;
		fprintf(finter, "%04x\t%s\t%s\t%X\n", LOCCTR, LABEL, OPCODE, sta_add);
		//read second input line
		fscanf(fsource, "%s%s%s", LABEL, OPCODE, OPERAND);
		line++;
	}
	else{
		LOCCTR=0;
	}
	int found=0;
	while(strcmp(OPCODE, "END") != 0){
		//write line to intermediate file
		if(LABEL[0] != '.'){
			fprintf(finter, "%04X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		}
		else {
			fprintf(finter, "\t%s\t%s\n",  LABEL,COMMENT);
		}
		//not a comment line
		if(LABEL[0] != '.') {
			if(strcmp(LABEL,"")!=0){
				for(i=0;i<symcount;i++){
					if(strcmp(LABEL, sym[i].symbol)==0){
						found=1;
						break;
					}
				}
				if(found==1){
					printf("deplicate symbol");
					return ;
				}
				else{
					strcpy(sym[symcount].symbol,LABEL);
					sym[symcount++].val=LOCCTR;
				}
			}
			//search OPTAB for OPCODE
			int opfound=0;
			for(i=0;i<opline;i++){
				if(strcmp(OPCODE,op[i].opcode)==0){
					opfound=1;
					break;
				}
			}
			if(opfound==1){
				LOCCTR=LOCCTR+3;
			}
			else if(strcmp(OPCODE, "WORD")==0){
				LOCCTR=LOCCTR+3;
			}
			else if(strcmp(OPCODE, "RESW")==0){
				LOCCTR=LOCCTR+3*atoi(OPERAND);
			}
			else if(strcmp(OPCODE, "RESB")==0){
				LOCCTR=LOCCTR+atoi(OPERAND);
			}
			else if(strcmp(OPCODE, "BYTE")==0){
				int len=0;
				for(i = 2; i < 10; i++) {
					if(OPERAND[i] != '\'') {
						len++;
					}
					else break;
				}	
				if(OPERAND[0] == 'C') {// character 
					LOCCTR=LOCCTR+len;
				}
				else if(OPERAND[0] == 'X') {// hexadecimal
					LOCCTR=LOCCTR+len/2+len%2;
				}
			}
			else{
				printf("invalid operation code");
				return ;
			}
		}
		//write line to intermediate file
		//fprintf(finter, "%04x\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		//read next input line
		int state=0;
		char str1[10],str2[10],str3[10];
		while(1){
			strcpy(str1,"");
			strcpy(str2,"");
			strcpy(str3,"");
			strcpy(COMMENT,"");
			do{
				fscanf(fsource, "%s",str1);
				
				state=1;
				if(str1[0]=='.') {
					state=4;
					c = getc(fsource);	
					while(1){
						if(c=='\n') break;
						c = getc(fsource);
						if(c=='\n') break;
						strncat(COMMENT, &c, 1);
					}
					break;
				}
				c = getc(fsource);
				if(c=='\t'||c==' '){
					state=2;
					fscanf(fsource, "%s",str2);
					c = getc(fsource);
				}
				if(c=='\t'||c==' '){
					state=3;
					fscanf(fsource, "%s",str3);
					c = getc(fsource);
				}
			}while(c!='\n'&&c!=EOF);
			line++;
			if(state==1){
				strcpy(LABEL,"");
				strcpy(OPCODE,str1);
				strcpy(OPERAND,"");
			}
			else if(state==2){
				strcpy(LABEL,"");
				strcpy(OPCODE,str1);
				strcpy(OPERAND,str2);
			}
			else if(state==3){
				strcpy(LABEL,str1);
				strcpy(OPCODE,str2);
				strcpy(OPERAND,str3);
			}
			else if(state==4){
				strcpy(LABEL,str1);
				strcpy(OPCODE,"");
				strcpy(OPERAND,"");
			}
			if(c==EOF||c=='\n'){
				break;
			}
		}
		//printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
	}
	fprintf(finter, "\t%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
	objLength = LOCCTR-startAddress;
	//close file
	fclose(fsource);
    fclose(fopcode);
    fclose(finter);
    //show symbol table
    printf("=== pass 1 symbol table ===\n");    
    for(i=0;i<symcount;i++){
    	printf("%s\t%X\n", sym[i].symbol,sym[i].val);
	}
	return;
}
void pass2(){
	//open file
	fopcode=fopen("opcode.txt","r");
	finter=fopen("intermediate.txt","r");
	fobprogram=fopen("object_Program.txt","w");
	
	char ADDRESS[10] , LABEL[10] , OPCODE[10] , OPERAND[10] , textRecord[1000],LOC[10];
	int i,j;
	int LOCCTR,line=1,opline=0,startAddress,operandAddress,operandLen=0,objectcodeLencount=0,Lencount=0;
	char c ,temp[10];
	struct optab{
		char opcode[10];
		int val;
	}op[1000];
	
	//load symbol table
	//for(i=0;i<symcount;i++){
    	//printf("%s\t%X\n", sym[i].symbol,sym[i].val);
	//}
	//load opcode table
	do{
		fscanf(fopcode,"%s%X",op[opline].opcode,&op[opline].val);	
		opline++;
		c=fgetc(fopcode);
	}while(c!=EOF);
	//printf("=== pass 2 opcode ===\n"); 
	//read first line
	fscanf(finter, "%X%s%s%s", &LOCCTR, LABEL, OPCODE, OPERAND);
	startAddress=LOCCTR;
	if(strcmp(OPCODE, "START") == 0) {
		//write Header record to object program
		fprintf(fobprogram, "H%-6s%06s%06X\n", LABEL, OPERAND, objLength);
		//write listing line
    	//printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	//read next input line
    	fscanf(finter, "%s%s%s%s", ADDRESS, LABEL, OPCODE, OPERAND);
    	c = getc(finter);
		Lencount++;
	}
	//initialize first Text record
	fprintf(fobprogram, "T%06X", LOCCTR);
	strcpy(textRecord, "");
	int opfound,symfound,Xfound=0;
	while(strcmp(OPCODE, "END") != 0){
		//not a comment line
		if(LABEL[0] != '.') {
			opfound=0;
			for(i=0;i<opline;i++){
				if(strcmp(OPCODE, op[i].opcode)==0){
					opfound=1;
					break;
				}
			}
			if(opfound==1){
				if(strcmp(OPERAND,"")!=0){
					symfound=0;
					for(j=0;j<symcount;j++){
						if(strcmp(OPERAND,sym[j].symbol)==0){
							symfound=1;
							break;
						}
					}
					if(symfound==1){
						operandAddress=sym[j].val;
					}
					else{
						operandAddress=0;
						printf("undefined symbol\n");
					}
				}
				
				else operandAddress=0;
				objectcodeLencount+=3;
				//assemble the object code instruction
				if(Xfound==1) operandAddress=operandAddress+(4096)*8;
				sprintf(textRecord,"%s%02X%04X",textRecord,op[i].val,operandAddress);
				
			}
			else if(strcmp(OPCODE,"BYTE")==0||strcmp(OPCODE,"WORD")==0){
				//convert constant to object code
				operandLen=0;
				int objCode=0;
				if(strcmp(OPCODE,"BYTE")==0){
					if(OPERAND[0]=='C'){
						for(i = 2; i < 10; i++) {
							if(OPERAND[i] == '\'') {
								break;
							}
							operandLen++;
							objCode = objCode*0x100 + OPERAND[i];
						}
						objectcodeLencount+=operandLen;
						char temp[operandLen*2];
						sprintf(temp, "%06X", objCode);
						strncat(textRecord, temp, operandLen*2);
					}
					else if(OPERAND[0]=='X'){
						for(i = 2; i < 10; i++) {
							if(OPERAND[i] == '\'') {
								break;
							}
							operandLen++;
							sprintf(textRecord,"%s%c",textRecord,OPERAND[i]);
						}
						objectcodeLencount+= (operandLen+1)/2;
						
					}
				}
				else if(strcmp(OPCODE,"WORD")==0){
					objectcodeLencount+=3;
					sprintf(temp,"%06X",atoi(OPERAND));
					sprintf(textRecord,"%s%06s",textRecord,temp);
				}
			}
			if(Lencount==length){//if object code will not fit into the current Text record
				//write Text record to object program
				fprintf(fobprogram, "%02X%s\n",objectcodeLencount, textRecord);
				//initialize new Text record
				Lencount=0;
				objectcodeLencount=0;
				strcpy(textRecord, "");
			}
			//add object code to Text record
			
		}
		//write listing line
		
		//read next input line
		int state;
		char OPERAND2[10];
		Xfound=0;
		state=0;
		strcpy(ADDRESS, "");
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(OPERAND, "");
		strcpy(OPERAND2, "");
		while(1) {
			c = getc(finter);
			
			if(c == '\n' || c == EOF) {
				if(LABEL[0]!='.'){
					Lencount++;
				}
				break;
			}
			else if(c == '\t') {
				state++;
			}
			else if(state == 0) {
				strncat(ADDRESS, &c, 1);
			}
			else if(state == 1) {
				strncat(LABEL, &c, 1);
			}
			else if(state == 2) {
				strncat(OPCODE, &c, 1);
			}
			else if(state == 3) {
				strncat(OPERAND, &c, 1);
			}
			if(LABEL[0]=='.') {
				while(1){
					c = getc(finter);
					if(c=='\n') break;
				}
				break;
			}
		}
		if(strstr(OPERAND,",X")!=0){
			Xfound=1;
			strncpy(OPERAND2, OPERAND, strlen(OPERAND)-2);
			strcpy(OPERAND,OPERAND2);
		}
		if(Lencount==1){
			strcpy(LOC,ADDRESS);
			fprintf(fobprogram, "T%06s", LOC);
		}	
	}
	//write last Text record to object program
	fprintf(fobprogram, "%02X%s\n",objectcodeLencount, textRecord);
	//write End record to object program
	fprintf(fobprogram, "E%06X\n",startAddress);
	//write LAST LISTING LINE
	
	//close file
	fclose(fopcode);
	fclose(finter);
	fclose(fobprogram);
	
	return;
}
