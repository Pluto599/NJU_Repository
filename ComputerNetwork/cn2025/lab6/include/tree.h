#ifndef __TREE_H__
#define __TREE_H__

#include <stdint.h>
#include <stdio.h>

// do not change it
#define TEST_SIZE 100000

typedef struct TrieNode
{
    // 用数组存储
    struct TrieNode *children[2];
    int port;
    int valid;
} TrieNode;

#define TBL_SIZE (1 << 16)

typedef struct
{
    int port;
    TrieNode *subtrie;
} TableEntry;

void create_tree(const char*);
uint32_t *lookup_tree(uint32_t *);
uint32_t* read_test_data(const char* lookup_file);

void create_tree_advance(const char *);
uint32_t *lookup_tree_advance(uint32_t *);

#endif
