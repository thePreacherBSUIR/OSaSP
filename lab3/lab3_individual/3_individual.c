#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int MaxChildNumber;
int ClildNumber;

int scanDirectory(char* dir);

char* getFullPath(char* dirname, struct dirent* dir);

int scanFile(const char* fileName, int* byteCount, int* wordCount);

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "\ninvalid arguments!\ncorrect format: %s <directory_name> <max amount of child PIDs>\n\n",argv[0]);
        return 1;
    }

    MaxChildNumber = atoi(argv[2]);

    if (MaxChildNumber < 1) {
        fprintf(stderr,"\nerror! max amount of child PIDs must b a number not lower than 1\n\n");
        return 1;
    }
    char* dir;

    dir = argv[1];
    ClildNumber = 0;
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| pid |                                                          filename                                                               | bytes | words | \n");
    if (scanDirectory(dir) != 0) {
        printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        fprintf(stderr, "\n|%5d| error! cannot open directory %-99s|       |       | \n\n", getpid(), dir);
    }
	
    while (wait(NULL) != -1) {
    }
    
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    return 0;
}

int scanDirectory(char* dir)
{
    struct dirent* curFile;
    char* fullName;
    int byteCount, wordCount;

    DIR* dirStream = opendir(dir);
    
    if (dirStream == NULL) {
        fprintf(stderr, "\nerror! cannot open directory %s\n\n", dir);
        return 1;
    }
    
    int scanRes;

    while ((curFile = readdir(dirStream)) != NULL) {
        if (curFile->d_type != DT_DIR) {
            if (MaxChildNumber <= ClildNumber)
            {
            	int wait_err;
            	
                wait_err = wait(NULL);
                
                if (wait_err == -1) 
                	fprintf(stderr, "\nerror! waiting failed\n\n");
                
                ClildNumber--;
            }
            ClildNumber++;

            switch(fork()){

            case (pid_t)0:{
                fullName= getFullPath(dir, curFile);
                
                if (scanFile(fullName, &byteCount, &wordCount) == 0) {
                    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                    printf("|%5d|%-129s|%7d|%7d|\n", getpid(), realpath(fullName, NULL), byteCount, wordCount);
                }
                else {
                    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                    printf("|%5d|%-129s|  cannot open  |\n", getpid(), realpath(fullName, NULL));
                }
                exit(0); 
                }       
            case (pid_t)-1: {
                fprintf(stderr, "\nerror! fork's creating failed\n\n");
                return 1;                               
            }          
            }
        }

        else if ((curFile->d_type == DT_DIR) && (strcmp(curFile->d_name, ".") != 0) && (strcmp(curFile->d_name, "..") != 0)) {
            char* fullName = getFullPath(dir, curFile);
            scanDirectory(fullName);
        }
    }
    if (closedir(dirStream) == -1) {
        fprintf(stderr, "\nerror! cannot close directory %s\n\n", dir);
        return 1;
    }
    return 0;
}

char* getFullPath(char* dirname, struct dirent* dir)
{
    char* filepath = calloc(strlen(dirname) + strlen(dir->d_name) + 2, 1);
    strcpy(filepath, dirname);
    strcat(filepath, "/");
    strcat(filepath, dir->d_name);

    return filepath;
}

int scanFile(const char* fileName, int* byteCount, int* wordCount)
{

    (*byteCount) = 0;
    (*wordCount) = 0;
    int fileopen = open(fileName, O_RDONLY);
    if (fileopen < 0) {
        return 1;
    }

    FILE* file = fdopen(fileopen, "r");

    int ch;

    char isWord = 0;
    while (1) {
        ch = fgetc(file);
        (*byteCount)++;

        if ((ch != ' ') && (ch != ';') && (ch != '.') && (ch != '-') && (ch != '!') && (ch != '?') && (ch != '\n') && (ch != '\r')) {
            if (isWord == 0) {
                isWord = 1;
                (*wordCount)++;
            }
        }
        else if (isWord == 1) {
            isWord = 0;
        }
        if (ch == EOF) {
            break;
        }
    }
    (*wordCount)--;
    (*byteCount) -= 2;
    if ((*byteCount) == -1)
        (*byteCount) = 0;
    fclose(file);

    return 0;
}
