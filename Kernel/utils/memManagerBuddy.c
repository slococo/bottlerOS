#ifdef BUDDY

// Basado en: https://github.com/cloudwu/buddy/blob/master/buddy.c

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define NODE_UNUSED 0
#define NODE_USED 1	
#define NODE_SPLIT 2
#define NODE_FULL 3

#define LEVEL 20

#define SIZE (1<<LEVEL)

struct buddy{
	int level;
	uint8_t tree[SIZE*2-1];
};

static struct buddy self;

void * buddy_alloc(int);
int buddy_free(void *);

void * pvPortMalloc(int size){
	return buddy_alloc(size);
}

int vPortFree(void * pointer){
	return buddy_free(pointer);
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
	return x+1;
}

#define PAGE_SIZE 0x1000

int _index_offset(int index, int level, int max_level) {
	return (int*)((((index + 1) - (1 << level)) << (max_level - level)) * PAGE_SIZE + heapAddress);
}

void _mark_parent(int index) {

	while(1) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy > 0 && (self.tree[buddy] == NODE_USED ||	self.tree[buddy] == NODE_FULL)) {
			index = (index + 1) / 2 - 1;
			self.tree[index] = NODE_FULL;
		} else {
			return;
		}
	}
}

void * buddy_alloc(int s) {

	if(s%PAGE_SIZE != 0){
		s=(s/PAGE_SIZE)+1;
	}else{
		s/=PAGE_SIZE;
	}
	int size;
	if (s==0) {
		size = 1;
	} else {
		size = (int)next_pow_of_2(s);
	}
	int length = 1 << self.level;

	if (size > length)
		return NULL;

	int index = 0;
	int level = 0;
	int nextStep = 1;

	while (index >= 0) {
		nextStep = 1;
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
					self.tree[index*2+1] = NODE_UNUSED;
					self.tree[index*2+2] = NODE_UNUSED;
				default:
					index = index * 2 + 1;
					length /= 2;
					level++;
					nextStep = 0;
			}
		}
		if( nextStep ){
			if (index & 1) {
				++index;
			} else {
				int cont = 1;
				while(cont) {
					level--;
					length *= 2;
					index = (index+1)/2 -1;
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

static void dumpMM(struct buddy * self, int index , int level) {
    char buffer[100];
	switch (self->tree[index]) {
	case NODE_UNUSED:
		printStringLen("(%d:%d)", _index_offset(index, level, self->level) , 1 << (self->level - level));
		break;
	case NODE_USED:
		printStringLen("(", 1);
		printStringLen(itoa(_index_offset(index, level, self->level), buffer, 10));
		printStringLen(":", 1);
        printStringLen(itoa(1 << (self->level - level), buffer, 10));
		printStringLen(")", 1);
		break;
	case NODE_FULL:
		printStringLen("{", 1);
		_dump(self, index * 2 + 1, level+1);
		_dump(self, index * 2 + 2, level+1);
		printStringLen("}", 1);
		break;
	default:
		printStringLen("(", 1);
		_dump(self, index * 2 + 1, level+1);
		_dump(self, index * 2 + 2, level+1);
		printStringLen(")", 1);
		break;
	}
}

void buddy_dumpMM(struct buddy * self) {
	dumpMM(self, 0 , 0);
	printStringLen("\n", 1);
}

#endif