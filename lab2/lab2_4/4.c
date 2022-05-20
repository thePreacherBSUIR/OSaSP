#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	if(argc != 3){
		fprintf(stderr, "\ninvalid arguments!\ncommand format: %s filename[name of file 2 read from] lines_amount[amount of lines per block to output]\n\n", argv[0]);
		return 1;
	}

    int lines_amount = strtol(argv[2], NULL, 10);
    
    if(lines_amount < 0){
        fprintf(stderr, "\ninvalid amount of lines!\n");
        return 1;
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
