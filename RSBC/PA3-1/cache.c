#include "cache.h"
#include <stdlib.h>

CacheLine cache[128][8];//my cache
extern uint8_t hw_mem[];//paddr

//find the block
////@return return the head of the line
static CacheLine* find(paddr_t paddr){
    uint32_t group_id = (paddr >> 6) & 0x7f;
    uint32_t tag = paddr >> 13;
    for(size_t i = 0; i < 8; i++)
        if(cache[group_id][i].valid_bit && cache[group_id][i].tag == tag)
            return &cache[group_id][i];
    return NULL;
} 

// //mem randomly write
static CacheLine* mem_copy(paddr_t paddr){
    uint32_t group_id = (paddr >> 6) & 0x7F;
    uint32_t index = paddr & 0x7;
    CacheLine* line = &cache[group_id][index];
    assert(line != NULL);
    uint32_t block_addr = paddr & ~0x3f;
    memcpy(line->data, hw_mem + block_addr, 64);
    line->valid_bit = true;
    line->tag = paddr >> 13;
    return line;
}

static uint32_t mem_read(uint8_t* data, uint32_t offset, size_t len){
    assert(offset + len <= 64);
    assert(data != NULL);
    uint32_t temp = 0;
    memcpy(&temp, data + offset, len);
    return temp;
}

static void mem_write(CacheLine* line, paddr_t paddr, uint32_t data, size_t len){
    uint32_t offset = paddr&0x3f;
    assert(offset + len <= 64);
    if(line != NULL)
        memcpy(line->data+offset, &data, len);
    memcpy(hw_mem + paddr, &data, len);
}

bool cross_block(paddr_t paddr, size_t len){
    return (paddr & 0x3f) + len > 64;
}

// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for(size_t group = 0; group < 128; group++)
	    for(size_t line = 0; line < 8; line++)
	        cache[group][line].valid_bit = false;
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
    if(cross_block(paddr, len)){
        uint32_t write_len = 64 - (paddr & 0x3f);
    	CacheLine* line_1 = find(paddr);
    	CacheLine* line_2 = find(paddr + len);
        mem_write(line_1, paddr, data, write_len);
        mem_write(line_2, paddr + write_len, data >> (write_len * 8), len - write_len);
    }
    else
    {
        CacheLine* line = find(paddr);
        mem_write(line, paddr, data, len);
        
	    uint32_t hw_mem_data;
        memcpy(&hw_mem_data, hw_mem + paddr, len);
        hw_mem_data = hw_mem_data << (32 - (len * 8)) >> (32 - (len * 8));
    	if(line!=NULL){
    	    uint32_t cache_data;
            memcpy(&cache_data, line->data + (paddr & 0x3f), len);
            cache_data = (cache_data << (32 - (len * 8))) >> (32 - (len * 8));
    	}
    }
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	//implement me in PA 3-1
	uint32_t data = 0x0;
	if(cross_block(paddr, len)){
    	CacheLine* line_1 = find(paddr);
    	CacheLine* line_2 = find(paddr + len);
        if(line_1 == NULL) line_1 = mem_copy(paddr);
        if(line_2 == NULL) line_2 = mem_copy(paddr + len);
        
        uint32_t read_len = 64 - (paddr & 0x3f);
        assert(line_1 != NULL && line_1->data != NULL);
        assert(line_2 != NULL && line_2->data != NULL);
        uint32_t data_1 = mem_read(line_1->data, paddr & 0x3f, read_len);
        uint32_t data_2 = mem_read(line_2->data, 0, len - read_len);
        
        data = (data_2 << (read_len * 8)) | data_1;
	}
	else
	{
    	CacheLine* line = NULL;
        line = find(paddr);
	    if(line == NULL){
	        line = mem_copy(paddr);
	    }
        data = mem_read(line->data, paddr & 0x3f, len);
	}
 	data = (data << (32 - (len * 8))) >> (32 - (len * 8));
	return data;
}
