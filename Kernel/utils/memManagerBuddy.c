// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef BUDDY

// Basado en: https://github.com/cloudwu/buddy/blob/master/buddy.c
#include "memManagerBuddy.h"

struct buddy{
	int level;
	uint8_t tree[SIZE*2-1];
};

static struct buddy self;

void * buddy_alloc(int);
int buddy_free(void *);

void * pvPortMalloc(size_t size){
	return buddy_alloc(size);
}

void vPortFree(void * pointer){
	buddy_free(pointer);
	// return;
}

static int heapAddress = 0;

void initMemoryManager(void *managedMemSize) {
    heapAddress = (int) managedMemSize;
	self.level = LEVEL;
	memset(self.tree , NODE_UNUSED , SIZE*2-1);
}

int is_pow_of_2(uint32_t x) {
	return !(x & (x-1));
}

uint32_t next_pow_of_2(uint32_t x) {
	if ( is_pow_of_2(x) )
		return x;
	x |= x>>1;
	x |= x>>2;
	x |= x>>4;
	x |= x>>8;
	x |= x>>16;
	return x + 1;
}

#define PAGE_SIZE 0x1000

int _index_offset(int index, int level, int max_level) {
	return (int*)((((index + 1) - (1 << level)) << (max_level - level)) * PAGE_SIZE + heapAddress);
}

void _mark_parent(int index) {
	while(1) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy > 0 && (self.tree[buddy] == NODE_USED || self.tree[buddy] == NODE_FULL)) {
			index = (index + 1) / 2 - 1;
			self.tree[index] = NODE_FULL;
		} else {
			return;
		}
	}
}

void * buddy_alloc(int s) {

	if (s % PAGE_SIZE != 0){
		s = (s / PAGE_SIZE) + 1;
	} else{
		s /= PAGE_SIZE;
	}
	int size;
	if (s == 0) {
		size = 1;
	} else {
		size = (int)next_pow_of_2(s);
	}
	int length = 1 << self.level;

	if (size > length)
		return NULL;

	int index = 0;
	int level = 0;

	while (index >= 0) {
		int nextStep = 1;
		if (size == length) {
			if (self.tree[index] == NODE_UNUSED) {
				self.tree[index] = NODE_USED;
				_mark_parent(index);
				return _index_offset(index, level, self.level);
			}
		} else {
			switch (self.tree[index]) {
				case NODE_USED: break;
				case NODE_FULL: break;
				case NODE_UNUSED:
					self.tree[index] = NODE_SPLIT;
					self.tree[index * 2 + 1] = NODE_UNUSED;
					self.tree[index * 2 + 2] = NODE_UNUSED;
				default:
					index = index * 2 + 1;
					length /= 2;
					level++;
					nextStep = 0;
			}
		}
		if (nextStep) {
			if (index & 1) {
				++index;
			} else {
				int cont = 1;
				while(cont) {
					level--;
					length *= 2;
					index = (index + 1) / 2 - 1;
					if (index < 0)
						return NULL;
					if (index & 1) {
						++index;
						cont = 0;
					}
				}
			}
		}
	}
	return NULL;
}

void _combine(int index) {
	while(1) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy < 0 || self.tree[buddy] != NODE_UNUSED) {
			self.tree[index] = NODE_UNUSED;
			while (((index = (index + 1) / 2 - 1) >= 0) &&  self.tree[index] == NODE_FULL){
				self.tree[index] = NODE_SPLIT;
			}
			return;
		}
		index = (index + 1) / 2 - 1;
	}
}

int buddy_free(void * pointer) {

	int offset=(int)pointer;

	offset=(offset- heapAddress)/PAGE_SIZE;


	if (offset >= (1<< self.level)){
		return -1;
	}	
	int left = 0;
	int length = 1 << self.level;
	int index = 0;

	while(1) {
		switch (self.tree[index]) {
			case NODE_USED:
				if (offset != left){
					return -1;
				}	
				_combine(index);
				return 0;
			case NODE_UNUSED:
				return -1;
			default:
				length /= 2;
				if (offset < left + length) {
					index = index * 2 + 1;
				} else {
					left += length;
					index = index * 2 + 2;
				}
				break;
			}
	}
}

uint64_t getSize(int level, int max_level) {
	return (1 << (max_level - level)) * PAGE_SIZE;
}

#include <stddef.h>
void buddy_dumpMM(int index , int level, uint64_t *size, uint64_t *used) {
	switch (self.tree[index]) {
	case NODE_UNUSED:
		*size += getSize(level, self.level);
		break;
	case NODE_USED:
		*size += getSize(level, self.level);
		*used += getSize(level, self.level);
		break;
	case NODE_FULL:
		buddy_dumpMM(index * 2 + 1, level+1, size, used);
		buddy_dumpMM(index * 2 + 2, level+1, size, used);
		break;
	default:
		buddy_dumpMM(index * 2 + 1, level+1, size, used);
		buddy_dumpMM(index * 2 + 2, level+1, size, used);
		break;
	}
}

char * dumpMM() {
	uint64_t size = 0, used = 0;
	buddy_dumpMM(0, 0, &size, &used);
	char *ans = pvPortMalloc(DUMP_MAX_SIZE);
    char *ret = ans;
	char buffer[20] = {0};
    ans += strcpy(ans, "Free memory: ");
    ans += strcpy(ans, itoa(size - used, buffer, 10, 20));
    ans += strcpy(ans, "\nTotal memory: ");
    ans += strcpy(ans, itoa(size, buffer, 10, 20));
	
	return ret;
}

#endif