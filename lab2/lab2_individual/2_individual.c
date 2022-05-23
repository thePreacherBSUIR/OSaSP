#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

long long from_size, to_size, from_date, to_date;

char src[1024];

int srcLen;
FILE *resFile;

int isNotAnotherDirectory(const char *dest)
{
    if (strcmp(dest, ".") == 0 || strcmp(dest, "..") == 0)
    {
        return 0;
    }
    return 1;
}

int isDirectory(const struct stat *st)
{
    if (S_ISDIR(st->st_mode))
    {
        return 1;
    }
    return 0;
}

void writeAndPrintFile(const struct stat *st)
{
    if (from_size <= st->st_size && st->st_size <= to_size && from_date <= st->st_ctim.tv_sec && st->st_ctim.tv_sec <= to_date)
        printf("%s    %ld  %ld\n", src, st->st_size, st->st_ctim.tv_sec);
    fprintf(resFile, "%s    %ld  %ld\n", src, st->st_size, st->st_ctim.tv_sec);
}

void enterSelf()
{
    struct stat st;
    if (stat(src, &st))
    {
        fprintf(stderr, "error! file %s cannot be read\n", src);
        return;
    }
    if (isDirectory(&st))
    {
        DIR *dirPtr = opendir(src);
        if (!dirPtr)
        {
            fprintf(stderr, "error! directory %s cannot b opened\n", src);
            return;
        }

        struct dirent *dirOrFile;
       
        while ((dirOrFile = readdir(dirPtr)))
        {
            char *destName = dirOrFile->d_name;
            int destNameLen = strlen(destName);

            if (isNotAnotherDirectory(destName))
            {
                int oldDirLen = srcLen;

                strcat(src, "/");
                strcat(src, destName);
                srcLen += 1 + destNameLen;

                enterSelf();

                srcLen -= 1 + destNameLen;
                src[oldDirLen] = '\0';
            }
        }

        if (closedir(dirPtr))
        {
            fprintf(stderr, "error! directory %s cannot b closed!\n", src);
            return;
        }
    }
    else
    {
        writeAndPrintFile(&st);
    }
}

int argStrToInt(char *name, char *from, long long *to)
{
    *to = strtol(from, NULL, 10);
    if (errno != 0 || *to < 0)
    {
        fprintf(stderr, "error! parameter %s has 2 b a positive integer\n", name);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        fprintf(stderr, "\ninvalid arguments!.\ncorrect format: %s <directory_name> <output_file> <from_size> <to_size> <from_date> <to_date>\n\n", argv[0]);
        return 1;
    }

    resFile = fopen(argv[2], "w");
    if (resFile == NULL)
    {
        fprintf(stderr, "error! cannot create an output file");
        return 1;
    }

    argStrToInt("from_size", argv[3], &from_size);
    argStrToInt("to_size", argv[4], &to_size);
    argStrToInt("from_date", argv[5], &from_date);
    argStrToInt("to_date", argv[6], &to_date);

    strcpy(src, argv[1]);
    srcLen = strlen(src);
    enterSelf();

    if (fclose(resFile))
    {
        fprintf(stderr, "error! file %s cannot b closed", argv[2]);
        return 1;
    }
    return 0;
}
