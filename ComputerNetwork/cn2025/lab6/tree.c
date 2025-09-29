#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TrieNode *root = NULL;
TableEntry *direct_table;

// return an array of ip represented by an unsigned integer, size is TEST_SIZE
// 读取测试数据集，将IP地址转为无符号整数并通过指针返回
uint32_t *read_test_data(const char *lookup_file)
{
    uint32_t *arr = malloc(sizeof(uint32_t) * TEST_SIZE);

    FILE *fp = fopen(lookup_file, "r");
    if(!fp)
    {
        perror("Failed to open lookup file");
        exit(1);
    }
    for (int i = 0; i < TEST_SIZE; i++)
    {
        char ip[32];
        fscanf(fp, "%s", ip);

        unsigned int a, b, c, d;
        sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d);
        arr[i] = (a << 24) | (b << 16) | (c << 8) | d;
    }
    fclose(fp);

    return arr;
}

// Constructing an advanced trie-tree to lookup according to `forward_file`
// 创建基本的ip前缀树，表项来自forward_file，每一行格式为(ip, mask_len, port)
void create_tree(const char *forward_file)
{
    root = (TrieNode *)calloc(1, sizeof(TrieNode));

    FILE *fp = fopen(forward_file, "r");
    if (!fp)
    {
        perror("Failed to open forwarding table");
        exit(1);
    }

    char line[64];
    while (fgets(line, sizeof(line), fp))
    {
        unsigned int a, b, c, d, mask_len, port;
        sscanf(line, "%u.%u.%u.%u %u %u", &a, &b, &c, &d, &mask_len, &port);
        uint32_t ip = (a << 24) | (b << 16) | (c << 8) | d;

        TrieNode *cur = root;
        for (int i = 0; i < mask_len; i++)
        {
            int bit = (ip >> (31 - i)) & 1;
            if (!cur->children[bit])
            {
                cur->children[bit] = (TrieNode *)calloc(1, sizeof(TrieNode));
            }
            cur = cur->children[bit];
        }
        cur->port = port;
        cur->valid = 1;
    }
    fclose(fp);
}

// Look up the ports of ip in file `lookup_file` using the basic tree
// 根据create_tree创建的前缀树，去查找ip_vec中每个ip对应的port，保存到数组内并返回，查询不到的条目设置结果为-1，数组长度见tree.h
uint32_t *lookup_tree(uint32_t *ip_vec)
{
    uint32_t *res = malloc(sizeof(uint32_t) * TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; i++)
    {
        TrieNode *cur = root;
        int found_port = -1;

        for (int j = 0; j < 32; j++)
        {
            if (cur->valid)
            {
                found_port = cur->port;
            }

            int bit = (ip_vec[i] >> (31 - j)) & 1;
            if (!cur->children[bit])
            {
                break;
            }
            cur = cur->children[bit];
        }

        if (cur && cur->valid)
        {
            found_port = cur->port;
        }
        res[i] = found_port;
    }

    return res;
}

void create_tree_advance(const char *forward_file)
{
    direct_table = (TableEntry *)malloc(sizeof(TableEntry) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        direct_table[i].port = -1;
        direct_table[i].subtrie = NULL;
    }

    FILE *fp = fopen(forward_file, "r");
    if (!fp)
    {
        perror("Failed to open forwarding table");
        exit(1);
    }

    char line[64];
    while (fgets(line, sizeof(line), fp))
    {
        unsigned int a, b, c, d, mask_len, port;
        sscanf(line, "%u.%u.%u.%u %u %u", &a, &b, &c, &d, &mask_len, &port);
        uint32_t ip = (a << 24) | (b << 16) | (c << 8) | d;

        if (mask_len <= 16)
        {
            // 对于16位以内的前缀，直接填充表
            uint32_t prefix = ip & (0xFFFFFFFF << (32 - mask_len));
            uint32_t start = prefix >> 16;
            uint32_t end = start + (1 << (16 - mask_len)) - 1;

            for (uint32_t i = start; i <= end; i++)
            {
                direct_table[i].port = port;
            }
        }
        else
        {
            // 对于超过16位的前缀，插入到对应的Trie树中
            uint32_t table_index = ip >> 16;

            if (!direct_table[table_index].subtrie)
            {
                direct_table[table_index].subtrie = (TrieNode *)calloc(1, sizeof(TrieNode));
            }

            TrieNode *cur = direct_table[table_index].subtrie;
            for (int i = 16; i < mask_len; i++)
            {
                int bit = (ip >> (31 - i)) & 1;
                if (cur->children[bit] == NULL)
                {
                    cur->children[bit] = (TrieNode *)calloc(1, sizeof(TrieNode));
                }
                cur = cur->children[bit];
            }
            cur->port = port;
            cur->valid = 1;
        }
    }

    fclose(fp);
}

uint32_t *lookup_tree_advance(uint32_t *ip_vec)
{
    uint32_t *res = malloc(sizeof(uint32_t) * TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; i++)
    {
        uint32_t ip = ip_vec[i];
        uint32_t table_index = ip >> 16;

        int found_port = direct_table[table_index].port;

        if (direct_table[table_index].subtrie)
        {
            TrieNode *cur = direct_table[table_index].subtrie;
            int last_valid_port = -1;

            for (int j = 16; j < 32 && cur; j++)
            {
                if (cur->valid)
                {
                    last_valid_port = cur->port;
                }

                int bit = (ip >> (31 - j)) & 1;
                if (!cur->children[bit])
                {
                    break;
                }
                cur = cur->children[bit];
            }

            if (cur && cur->valid)
            {
                last_valid_port = cur->port;
            }

            if (last_valid_port != -1)
            {
                found_port = last_valid_port;
            }
        }

        res[i] = found_port;
    }

    return res;
}