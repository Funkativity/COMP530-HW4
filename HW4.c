// Author: Victor Murta
// Homework one for COMP 530: Operating Systems
// UNC Chapel Hill, Fall 2019
///////////////////////////////////////////////////////////////////////////////
// Description : Write a C program on Linux to read in a stream of characters
// from standard input (i.e., the keyboard) and write them as 80 character lines
// to standard output (i.e., the “screen”) with the following changes:
// • Every enter/return (newline) character in the input stream is replaced by a space
// • Every adjacent pair of asterisks “**” in the input stream is replaced by a “^”.
// now we do this using separate linux processes

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#define LINE_SIZE   80

// just buffers input
void processRead(int outPipe[]){
    
    char curChar = getchar();
    while (curChar!= -1){
        write(outPipe[1], &curChar, sizeof(char));
        curChar = getchar();
    }
    // pass end of file down pipe
    write(outPipe[1], &curChar, sizeof(char));    
    // ("reads exiting now");
    exit(0);
}


//converts newlines to spaces
void processNewlines(int inPipe[], int outPipe[]){

    char curChar = 0;
    while (curChar!= -1){
        read(inPipe[0], &curChar, sizeof(char));
        // ("%d ", curChar);
        if (curChar == '\n'){
            curChar = ' ';
        }
        write(outPipe[1], &curChar, sizeof(char));
    }
    // pass end of file down pipe
    write(outPipe[1], &curChar, sizeof(char));    
    // ("newlines exiting now");
    exit(0);
}


//thread that transforms pairs of asterisks into '^'
void processAsterisks(int inPipe[], int outPipe[]){
    char curChar = 0;
    char nextChar = 0;
    char carrat = '^';

    while (curChar!= -1 && nextChar != -1){
        read(inPipe[0], &curChar, sizeof(char));
        // ("%d \n", curChar);
        if (curChar == '*'){
            read(inPipe[0], &nextChar, sizeof(char));
            if (nextChar == '*'){
                write(outPipe[1], &carrat, sizeof(char));
            }
            else {
                write(outPipe[1], &curChar, sizeof(char));
                write(outPipe[1], &nextChar, sizeof(char));
            }
        }
        // non-asterisk case
        else {
            write(outPipe[1], &curChar, sizeof(char));
        }
    }
    
    // pass end of file down pipe
    write(outPipe[1], &curChar, sizeof(char));    
    // ("asterisk exiting now");
    exit(0);
    // don't have an exit condition, process terminates with
    // the write thread
}

//fills a buffer with 80 characters then prints them
void processWrites(int inPipe[]){
    char line[LINE_SIZE];
    char curChar = 0;
    int index = 0;
    for (;;) {
        if (index == LINE_SIZE){
            puts(line);
            index = 0;
        }
        read(inPipe[0], &curChar, sizeof(char));
        // ("%d \n", curChar);
        if (curChar == -1){
            ("writes exiting now");
            exit(0);
        }
        line[index] = curChar;
        index++;
    }
}

int main(){

    int readToNewline[2];
    int newLineToAsterisk[2];
    int asteriskToWrite[2];

    pipe(readToNewline);
    pipe(newLineToAsterisk);
    pipe(asteriskToWrite);

    int isParent = fork();
    if (!isParent){
        processNewlines(readToNewline, newLineToAsterisk);
        exit(0);    //unnecessary I think, but I'm new to this, so just to be safe
    }

    isParent = fork();
    if (!isParent){
        processAsterisks(newLineToAsterisk, asteriskToWrite);
        exit(0);    //unnecessary I think, but I'm new to this, so just to be safe
    }

    isParent = fork();
    if (!isParent){
        processRead(readToNewline); 
        exit(0);    //unnecessary I think, but I'm new to this, so just to be safe
    }
    
    processWrites(asteriskToWrite);
    exit(0);    //unnecessary I think, but I'm new to this, so just to be safe

}