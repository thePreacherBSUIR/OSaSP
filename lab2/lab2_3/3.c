#include <stdio.h>

const int stopChar = 24;

int main(int argc, char const *argv[])
{
	if(argc != 2){
		fprintf(stderr, "\ninvalid argument!\ncommand format: %s filename[name of file 2 write in]\n\n", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "w");
	if(!file){
		perror("fopen");
		return 1;
	}

	while(1){
		char c = getc(stdin);
		if(c == stopChar)
			break;
		if(fputc(c, file) == EOF){
			perror("fputc");
			return 1;
		}
	}

	if(fclose(file)){
		perror("fclose");
		return 1;
	}

	return 0;
}
