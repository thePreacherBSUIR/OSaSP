#include <stdio.h>
#include <dirent.h>

int print_dir(char const *dir_name);

int main(int argc, char const *argv[])
{
    printf("\ncurrent directory:\n");
    print_dir("./");
    printf("\nroot directory:\n");
    print_dir("../");
    printf("\n");
   
    return 0;
}

int print_dir(char const *dir_name){
    DIR *curr_dir = opendir(dir_name);
   
    if(!curr_dir){
        perror("opendir");
        return 1;
    }

    struct dirent *dir;

    int i = 1;
    
    while((dir = readdir(curr_dir))){
        printf("%3d.\t%s\n", i++, dir->d_name);
    }

    if(closedir(curr_dir)){
        perror("closedir");
        return 1;
    }

    return 0;
}
