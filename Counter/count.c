#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*[>functinos to convert the numbers<]*/
/*unsigned long long atollu(char * num_str) {                                */
	/*unsigned long long num = 0; */
	/*int i;*/
	/*for (i = 0; i < strlen(num_str); ++i) {*/
		/*num = (num*10 + num_str[i] - '0');*/
	/*}*/

	/*return num;*/
/*}*/

/*float atofloat(char * num_str) {*/
	/*[>printf("before convert: %s\n", num_str);<]*/
	/*float res = 0.0;*/
	/*int i = 0, j;*/
	/*for ( ;num_str[i] != '%' && num_str[i] != '\0'; ++i) {*/
		/*[>printf("i = %d\n", i);<]*/
		/*if (num_str[i] == '.') {// count the # of bits after pt*/
			/*j = i;*/
			/*continue;*/
		/*}*/
		/*res = (res*10 + num_str[i] - '0');*/
	/*}*/

	/*int divisor = 1;*/
	/*int n;*/
	/*for (n = 0; n < (i -j); ++n) */
		/*divisor *= 10;*/

	/*if (num_str[i] == '%') // if find "%", divide the res by 100*/
		/*divisor *= 100;*/

	/*[>printf("divisor = %d\n", divisor);<]*/

	/*printf("after convert: %f\n", res/divisor);*/
	/*return res/divisor;*/
/*}*/




/*functions to find the event and count it*/
/*changes need to be made when creating a new func*/
	/*counter func:*/
	/*1. type of return value */
	/*2. check target line*/
	/*3. loops of strtok*/
	/*get_count func:*/
	/*1. callee*/

void L2RDmiss(int offset, char * line) {
	/*the target string in the log file:*/
	/*-- L2$ { 0] : RD (miss, access) = (  xxxx, xxxx) = xx%*/
	/*unsigned long long res = 0;*/

	/*check if the line is the target line or not*/
	char tar[4][20];
	strcpy(tar[0], "L2$");
	strcpy(tar[1], "RD");
	strcpy(tar[2], "miss");
	/*the number of the L2 $, "[ 0]"*/
	sprintf(tar[3], "%d]", offset);

	int i;
	for (i = 0; i < 4; ++i) {
		if (!strstr(line, tar[i])) 
			return;
			/*return res;*/
	}
	/*printf("target line is found\n");*/


	/*get count in string*/
	char * token;
	char res_str[20]; 

	token = strtok(line, " \t\n");
	for (i = 0; i < 8; ++i) { // set the # of loops based on the # of delimiters
		token = strtok(NULL, " \t\n");
		/*printf("%s\n", token);*/
	}

	for(i = 0; token[i] != '\0'; ++i)
		res_str[i] = token[i];
	res_str[i-1] = '\0'; // get rid of the last ","
	/*printf("%s\n", res_str);*/


	/*[>convert string into unsigned long long<]*/
	/*res = atollu(res_str);*/

	/*print out the results*/
	printf("L2RDmiss \t %s\n", res_str);
}

void L2WRmissRate(int offset, char * line) {
	/*the target string in the log file:*/
	/*-- L2$ { 0] : WR (miss, access) = (  xxxx, xxxx) = xx.x%*/
	/*float res = 0.0;*/

	char tar[5][20];
	strcpy(tar[0], "L2$");
	strcpy(tar[1], "WR");
	strcpy(tar[2], "miss");
	strcpy(tar[3], "access");
	/*the number of the L2 $, "[ 0]"*/
	sprintf(tar[4], "%d]", offset);

	
	/*check if the line is the target line or not*/
	int i;
	for (i = 0; i < 5; ++i) {
		if (!strstr(line, tar[i])) 
			return;
			/*return res;*/
	}
	/*printf("target line is found\n");*/


	/*get count in string*/
	char * token;
	char res_str[20]; 

	token = strtok(line, " \t\n");
	for (i = 0; i < 10; ++i) { // set the # of loops based on the # of delimiters
		token = strtok(NULL, " \t\n");
		/*printf("%s\n", token);*/
	}

	for(i = 0; token[i] != '\0'; ++i)
		res_str[i] = token[i];
	res_str[i] = '\0';
	/*printf("%s\n", res_str);*/


	/*[>convert string into float<]*/
	/*res = atofloat(res_str);*/

	/*print out the results*/
	printf("L2WRmissRate \t %s\n", res_str);

}

void cycles(char * line) {
	/*the target string in the log file:*/
	/*-- event became empty at cycle = xxxx*/
	/*unsigned long long res = 0;*/

	char * tar= "event became empty at cycle =";

	
	/*check if the line is the target line or not*/
	if (!strstr(line, tar)) 
		return;
		/*return res;*/
	/*printf("target line is found\n");*/


	/*get count in string*/
	char * token;
	char res_str[20]; 

	token = strtok(line, " \t\n");
	int i;
	for (i = 0; i < 7; ++i) { // set the # of loops based on the # of delimiters
		token = strtok(NULL, " \t\n");
		/*printf("%s\n", token);*/
	}

	for(i = 0; token[i] != '\0'; ++i)
		res_str[i] = token[i];
	res_str[i] = '\0';
	/*printf("%s\n", res_str);*/


	/*[>convert string into unsigned long long<]*/
	/*res = atollu(res_str);*/

	/*print out the results*/
	printf("cycles \t %s\n", res_str);

}




void get_count(FILE * fp) {
	char line[1024];

	/*[>list all of the events we will count <]*/
	/*unsigned long long L2RDmiss_n = 0;*/
	/*unsigned long long cycles_n = 0;*/
	/*float L2WRmissRate_n = 0.0;*/

	/*parse each line*/
	while (fgets(line, 1024, fp)) {
		/*check events and get the count*/
		L2RDmiss(0, line);
		cycles(line);
		L2WRmissRate(0, line);


	}
	

	/*[>print out the results<]*/
	/*printf("L2RDmiss \t %llu\n", L2RDmiss_n);*/
	/*printf("cycles \t %llu\n", cycles_n);*/
	/*printf("L2WRmissRate \t %f\n", L2WRmissRate_n);*/

}


int main (int argc, char * argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: count [filename]\n");
		exit(-1);
	}   


	/*open the file*/
	char * infile = argv[1];
	FILE * fp; 
	if((fp = fopen(infile, "r")) == NULL) {
		fprintf(stderr, "No such file.\n");
		exit(-1);
	}   
	/*printf("Open %s successfully.\n", infile);*/

	/*get the count*/
	get_count(fp);



	return 0;
}
