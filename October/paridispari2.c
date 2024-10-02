// Programma scritto da PAsquale Rossini top programmatore del corso


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

FILE* myReadFile(char *fileName, char *mode){
    FILE* file = fopen(fileName, mode);
    if(file == NULL){
        printf("%s name is not correct!", fileName);
        exit(2);
    }
    return file;
}

void evenOddFiles(char *nReadFile, char* nEvenFile, char* nOddFile){
    FILE* readFile = myReadFile(nReadFile, "r");
    FILE* evenFile = myReadFile(nEvenFile, "w");
    FILE* oddFile = myReadFile(nOddFile, "w");
    int i = 0, num = 0;
    while((i = fscanf(readFile, "%d", &num)) != EOF){
        if(num%2==0){
            fprintf(evenFile, "%d", num);
        }else{
            fprintf(oddFile, "%d", num);
        }
    }
    fclose(readFile);
    fclose(evenFile);
    fclose(oddFile);
}

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Use:%s 'input file name' 'first output file name' 'second output file name'", argv[0]);
        exit(1);
    }
    evenOddFiles(argv[1], argv[2], argv[3]);
    return 0;
}
