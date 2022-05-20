#include <stdio.h>
#include "sys/stat.h"

int main(int argc, char const *argv[])
{
	if(argc != 3){
		fprintf(stderr, "\ninvalid arguments!\ncommand format: %s file1[file 2 copy from] file2[file 2 copy 2]\n\n", argv[0]);
     	return 1;
	}

  	FILE *file1 = fopen(argv[1], "r");
    	
    	if(!file1){
   		perror("fopen");
    		return 1;
    	}
    
    	FILE *file2 = fopen(argv[2], "w");
    
    	if(!file2){
    		perror("fopen");
   		fclose(file1);
      		return 1;
    	}

    	int code = 0;

    	struct stat file_stat;
    
    	if(stat(argv[1], &file_stat)){
      		perror("stat");
      		code = 1;
     		goto closing_files;
    	}

    	if(chmod(argv[2], file_stat.st_mode)){
        	perror("chmod");
        	code = 1;
        	goto closing_files;
    	}

    	char c;
    
    	while((c = fgetc(file1)) != EOF){
  
  	if(fputc(c, file2) == EOF){
      		perror("fputs");
          	code = 1;
            	goto closing_files;
     	}
}
    
closing_files:
    
  	if(fclose(file1)){
    		perror("fclose");
       	fclose(file2);
      		return 1;
      	}

    	if(fclose(file2)){
        	perror("fclose");
        	return 1;
    	}

    	return code;
}
