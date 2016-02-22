#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*functinos to convert the numbers*/
unsigned long long atollu(char * num_str) {                                
	unsigned long long num = 0; 
	int i;
	for (i = 0; i < strlen(num_str); ++i) {
		num = (num*10 + num_str[i] - '0');
	}

	return num;
}


float atofloat(char * num_str) {                                                            
	/*printf("before convert: %s\n", num_str);*/
	float res = 0.0;
	int i = 0, j;
	for ( ;num_str[i] != '%' && num_str[i] != '\0'; ++i) {
		/*printf("i = %d\n",
		 * i);*/
		if (num_str[i] == '.') {// count the # of bits after pt
			j = i;
			continue;
		}
		res = (res*10 + num_str[i] - '0');
	}

	int divisor = 1;
	int n;
	for (n = 0; n < (i - j - 1); ++n)
		divisor *= 10;

	if (num_str[i] == '%') // if find "%", divide the res by 100
		divisor *= 100;

	/*printf("divisor = %d\n", divisor);*/

	/*printf("after convert: %f\n", res/divisor);*/
	return res/divisor;
}

void calc_mean(FILE * fp, char * data_name) {
	double mean_val = -10.0;

	char line[1024];

	double tot_f = 0.0;
	unsigned long long tot_llu = 0;
	char * token;
	int times = 0;
	/*parse each line*/
	while (fgets(line, 1024, fp)) {
		/*check events and get the count*/
		if (strstr(line, data_name)) {
			++times;
			/*get the number in string*/
			token = strtok(line, " \t\n");
			token = strtok(NULL, " \t\n");
			/*printf("token = %s\n", token);*/
			if (strstr(token, ".") || (strstr(token, "%"))) {
				/*it is indicated that the # is float #*/
				tot_f += atofloat(token);
			}
			else
				tot_llu += atollu(token);

		}
		/*calculate the mean value*/
		if (times)
			mean_val = (tot_f + tot_llu) / times;
	}

	if (mean_val < 0)
		printf("No such data in file.\n");
	else
		printf("%f\n", mean_val);

}

int main (int argc, char * argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: mean [filename] data\n");
		exit(-1);
	}   


	/*open the file*/
	char * infile = argv[1];
	FILE * fp; 
	if((fp = fopen(infile, "r")) == NULL) {
		fprintf(stderr, "No such file.\n");
		exit(-1);
	}   
	/*printf("Open %s successfully.\n",infile);*/

	calc_mean(fp, argv[2]);

	return 0;
}

