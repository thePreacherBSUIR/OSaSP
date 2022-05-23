#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	if(argc != 3){
		fprintf(stderr, "\ninvalid arguments!\ncommand format: %s filename[name of file 2 read from] lines_amount[amount of lines per block to output]\n\n", argv[0]);
		return 1;
	}

    char *ptr;
    int lines_amount = strtol(argv[2], &ptr, 10);
    
    if(lines_amount < 0){
        fprintf(stderr, "\ninvalid amount of lines!\n\n");
        return 1;
        }
    else if (strcmp(argv[2],ptr) == 0 || (strlen(ptr) != 0)){
        fprintf(stderr, "\nerror! amount of lines must b a number\n\n");
        return -1;
        }

	FILE *file = fopen(argv[1], "r");
	if(!file){
        perror("fopen");
		return 1;
	}

    int i = 0;
    
    while((i < lines_amount || lines_amount == 0)){
        char c;
        
        do{
            c = fgetc(file);
            if(c != EOF)
                printf("%c", c);
        } while (c != EOF && c != '\n');
        
        if(c == EOF)
            break;
        
        i++;
        
        if(i == lines_amount){
            getc(stdin);
            i = 0;
        }
    }

	if(fclose(file)){
        	perror("fclose");
		return 1;
	}

	return 0;
}
