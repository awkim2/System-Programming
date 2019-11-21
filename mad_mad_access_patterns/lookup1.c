/**
 * Mad Mad Access Patterns
 * CS 241 - Fall 2019
 */
#include "tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

int binarySearch(FILE* file, char* word, long offset){
    if(offset == 0) return 0;
    fseek(file, offset, SEEK_SET);
    BinaryTreeNode node;
    fread(&node, sizeof(BinaryTreeNode), 1, file);
    fseek(file, offset + sizeof(BinaryTreeNode), SEEK_SET);
    char wordInFile[20];
    fread(wordInFile, 20, 1, file);
    if (strcmp(word, wordInFile) == 0){
      printFound(wordInFile, node.count, node.price);
      return 1;
    }
    if (strcmp(word, wordInFile) < 0){
      if (binarySearch(file,  word, node.left_child)){
        return 1;
      }
    }
    if (strcmp(word, wordInFile) > 0){
      if (binarySearch(file,  word, node.right_child)){
        return 1;
      }
    }
    return 0;
}

int main(int argc, char **argv) {
    if( argc < 3){
      printArgumentUsage();
      exit(1);
    }
    char* input = argv[1];
    FILE* file = fopen(input, "r");
    if(!file){
      openFail(input);
      exit(2);
    }
    //long pos = ftell(file);
    char buffer[4];
    fread(buffer, 1, 4, file);
    //printf("first 4 byte is %s\n", buffer);
    if(strcmp(buffer, "BTRE") != 0){
      formatFail(input);
      exit(2);
    }
    for(int i = 2; i < argc; i++){
      int res = binarySearch(file, argv[i], (long)4);
      if( res == 0){
        printNotFound(argv[i]);
      }
    }
    //fseek(file, (long) 4, SEEK_CUR);
    //long newpos = ftell(file);
    //printf("old pos is %ld, new pos is  %ld\n", pos, newpos);
    fclose(file);
    return 0;
}
