/**
 * Mad Mad Access Patterns
 * CS 241 - Fall 2019
 */
#include "tree.h"
#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/
BinaryTreeNode *binarySearch(uint32_t offset, char *add, char *word) {
  if(offset == 0) return NULL;
  BinaryTreeNode* node = (BinaryTreeNode *) (add + offset);
  BinaryTreeNode* child;
  if (strcmp(word, node->word) == 0) return node;
  if (strcmp(word, node->word) < 0) {
    if ((child = binarySearch(node->left_child, add, word))) return child;
  }
  if (strcmp(word, node->word) > 0) {
    if ((child = binarySearch(node->right_child, add, word))) return child;
  }
  return NULL;
}

int main(int argc, char **argv) {
    if( argc < 3){
      printArgumentUsage();
      exit(1);
    }
    char* input = argv[1];
    int fd = open(input, O_RDONLY);
    if(fd < 0) {
      openFail(input);
      exit(2);
    }
    struct stat stats;
    if(fstat(fd, &stats) !=0) {
      openFail(input);
      exit(2);
    }
    char* add = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (add == (void *)-1) {
      mmapFail(input);
      exit(2);
    }
    if (strncmp(add, "BTRE", 4)) {
      formatFail(input);
      exit(2);
    }
    for (int i = 2; i < argc; i++) {
      BinaryTreeNode *node = binarySearch(4, add, argv[i]);
      if (node) {
        printFound(node->word, node->count, node->price);
      } else {
        printNotFound(argv[i]);
      }
    }
    close(fd);
    return 0;
}
