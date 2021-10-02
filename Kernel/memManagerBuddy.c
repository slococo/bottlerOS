// #ifdef BUDDY
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define MANAGED_MEMORY_SIZE 1024 * 1024 * 64

// https://github.com/sdpetrides/BuddyAllocator

// char initMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
//     return 1;
// }

// void * memMalloc(const size_t memoryToAllocate) {
//     return NULL;
// }

typedef struct meta {
	unsigned char allo : 1;		// 0000000_ - allocated
	unsigned char left : 1;		// 000000_0 - first or second
	unsigned char size : 6;		// ______00 - n where (2^n)-1 is the block size
} Meta;

// static char myblock[MANAGED_MEMORY_SIZE];
static char *myblock;
void initMemoryManager(void * managedMemory) {
	myblock = managedMemory;
}

void unpack(Meta * m, int pos);

/* Fills myblock with zeros and creates first metadata */
void init_block() {
	memset(&myblock, '\0', MANAGED_MEMORY_SIZE);
	memset(&myblock, 54, 1);
}

/* Returns log base 2 of a double d */
double log2(double d) {
    return log(d) / log(2);  
}

/* Returns the level a reqSize will fit in */
int size_to_n(size_t reqSize) {
	reqSize+=1;
	double d = log2((double)reqSize);
	return (int)ceil(d);
}

/* Returns the position of the next block of the correct size */
int jump_next(int n, int pos) {
	int bits = pos>>(n);
	bits+=1;
	int ret = bits<<(n);

	if (ret == MANAGED_MEMORY_SIZE) {
		return ret;
	} else {
		return ret;
	}
	
}

/* Returns the position of the left half of a pair */
int jump_back(int n, int pos) {
	int bits = pos>>(n);
	bits-=1;
	return bits<<(n);
}

/* Fills a Meta struct with metadata at pos */
void unpack(Meta * m, int pos) {
	memset(m, myblock[pos], 1);
}

/* Returns whether position at level n is left or right partner */
int is_left(int n, int pos) {

	// Manipulate bits to set nth bit on
	int k = 1;
	k<<=(n);
	
	// Manipulate bits to zero bits above n
	unsigned int p = (unsigned int)pos;
	p<<=(31-n);
	p>>=(31-n);

	if (k == p) {
		return 0;	// Right
	} else {
		return 1;	// Left
	}
}

/*  Mergee two unallocated blocks with same size */
void merge(int pos, int pos2, int n) {
	
	// Create new meta and set size field
	char newMeta = (n+1)<<2;

	// Set left field
	if (is_left(n+1, pos)) {
		newMeta+=2;
	}

	// Add new meta
	myblock[pos] = newMeta;

	// Delete meta on right partner
	myblock[pos2] = 0;		
}

/* MYmymalloc */
void * mymalloc(size_t reqSize) {

	// Check if too big
	if (reqSize > MANAGED_MEMORY_SIZE - 1) {
		fprintf(stderr, "Error: Requested size too large\n");
	}

	// Traverse heap to find block of correct size - algo(n)
	int n = size_to_n(reqSize);
	int pos = 0;
	unsigned char c = 0;
	Meta * m = memset(&c, 0, 1);
	
	while (pos < MANAGED_MEMORY_SIZE) {
		// Read metadata
		unpack(m, pos);

		// Debugging
		if (m->size == 0) {
			exit(0);
		}

		if (n <= m->size) {
			if (m->allo == 1) {				
				// Jump
				pos = jump_next(n, pos);
				continue;
			} else if (m->size == n) {		
				// Allocate
				myblock[pos]+=1;
				pos+=1;
				return (void*)((long int)&myblock+pos);
			} else { 						
				// Partition

				// Get partner position
				int partner = jump_next((m->size)-1, pos);

				// Set Left
				char meta_1 = 2;
				char meta_2 = 0;
				
				// Set Size
				char s = ((m->size)-1)<<2;
				meta_1 = (meta_1 | s);
				meta_2 = (meta_2 | s);

				// Fill in metadata
				myblock[pos] = meta_1;
				myblock[partner] = meta_2;

				// Continue on same position with new size of block
				continue;
			}
		} else {
			// Jump
			pos = jump_next(n, pos);
			continue;
		}
	}

	fprintf(stderr, "Error: Did not allocate  %d\n", pos);

	return 0;
}

/* MYmyfree */
void myfree(void * ptr) {
	
	// Error Checking
	if (ptr <= (void *)&myblock || ptr > (void *)(&myblock + MANAGED_MEMORY_SIZE)) {
		fprintf(stderr, "Error en free\n");
		return;
	}

	// Get position
	// int pos = (int)(ptr-(void *)&myblock-1);
    int pos = (int)((char *)ptr-(char *)&myblock-1);

	// Check if valid metadata location
	if (pos%2 == 1 || myblock[pos] == 0) {
		fprintf(stderr, "Error con metadata\n");
		return;
	}


	// Initialize variables for merge
	unsigned char c1 = 0;
	unsigned char c2 = 0;
	Meta * m1 = memset(&c1, 0, 1); 
	Meta * m2 = memset(&c2, 0, 1);
	unpack(m1,pos);

	// Change allocated field
	myblock[pos] = myblock[pos] - 1;

	while (pos >= 0 && pos <= 8196){
		// Read metadata
		unpack(m1,pos);

		if (m1->left) {	// Left Partner

			// Get position of other partner and read metadata
			int pos2 = jump_next(m1->size, pos);

			if (pos2 >= 0 && pos2 <= MANAGED_MEMORY_SIZE - 2) {
				unpack(m2,pos2);
			} else {
				break;
			}

			// Merge or break
			if (m2->allo || m2->size != m1->size) {
				break;
			} else {
				merge(pos, pos2, m1->size);
			}

		} else {		// Right Partner

			// Get position of other partner and read metadata
			int pos2 = jump_back(m2->size,pos);

			if (pos2 >= 0 && pos2 <= MANAGED_MEMORY_SIZE -2) {
				unpack(m2,pos2);
			} else {
				break;
			}

			// Merge or break
			if (m2->allo || m2->size != m1->size) {
				break;
			} else {
				merge(pos2, pos, m1->size); 
			}	
		}
	}
}

int rando[20] = {
	56, 73,  2,  8, 76,
	56, 66, 21, 13,  4,
	99, 80, 25,  6, 38,
	10, 64, 12, 32, 71
};

/* testA: mymalloc() 1 byte 3000 times, then myfree() the 3000 1 byte pointers one by one */
void testA() {
	int i = 0;
	char * p[3000];

	while (i < 3000) {
		p[i] = (char *)mymalloc(sizeof(char)*1);
		i++;
	}

	i = 0;
	
	while (i < 3000) {
		myfree(p[i]);
		i++;
	}

	return;
}

/* testB: mymalloc() 1 byte and immediately myfree it 3000 times in a row */
void testB() {
	int i = 0;
	char * p;

	while (i < 3000) {
		p = (char *)mymalloc(sizeof(char)*1);
		myfree(p);
		i++;
	}

	return;
}

/* testC: Randomly choose between a 1 byte mymalloc() or myfree() 6000 times */ 
void testC() {
	int i = 1;
	int j = 0;
	char * p[6000];

	p[0] = (char *)mymalloc(sizeof(char)*1);	

	while (i+j < 1000) {

		if ((i+j)%2 != 0) {
			p[i] = (char *)mymalloc(sizeof(char)*1);
			i++;
		} else if (p[j] != 0) {
			myfree(p[j]);
			j++;
		} else {
			p[i] = (char *)mymalloc(sizeof(char)*1);
			i++;
		}
	}

	while (j <= i) {
		myfree(p[j]);
		j++;
	}

	return;
}

/* testD: Randomly choose between a randomly-sized mymalloc() or myfree 6000 times */
void testD() {
	int i = 1;
	int j = 0;
	char * p[6000];

	p[0] = (char *)mymalloc(sizeof(char)*(20));	

	while (i+j < 6000) {

		if ((i+j)%2 != 0) {
			p[i] = (char *)mymalloc(sizeof(char)*(((i%2)+1)*20));
			i++;
		} else if (p[j] != 0) {
			myfree(p[j]);
			j++;
		} else {
			p[i] = (char *)mymalloc(sizeof(char)*(((i%2)+1)*20));
			i++;
		}
	}

	while (j <= i) {
		myfree(p[j]);
		j++;
	}

	return;
}

/* testE: mymalloc 100 2bytes and myfrees 100 2bytes 10 times */
void testE() {
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	char * p[6000];	

	while (i < 6000) {
		k = 0;
		while (k < 100) {
			p[i] = (char *)mymalloc(sizeof(char)*(2));
			i++; k++;
		}

		m = 0;
		while (m < 100) {
			myfree(p[j]);
			j++; m++;
		}
	}

	return;
}

/* testF: mymalloc 2000 2bytes, myfrees 1000 2bytes, mymalloc 2000 2bytes, myfrees 3000 2bytes */
void testF() {
	int i = 0;
	int j = 0;
	char * p[6000];


	while (i < 2000) {
		p[i] = (char *)mymalloc(sizeof(char)*(2));
		i++;
	}

	while (j < 1000) {
		myfree(p[j]);
		j++;
	}

	while (i < 2000) {
		p[i] = (char *)mymalloc(sizeof(char)*(2));
		i++;
	}

	while (j < 4000) {
		myfree(p[j]);
		j++;
	}

	return;
}


int main(int argc, char const *argv[]) {

	// Inititialize variables for workflow
	int i, j;

	// Loop through fptr array
	for (j = 0; j < 6; j++) {

		// Initialize time elapsed
		double time_elapsed_in_seconds = 0.0;

		// Run the fuction 100 times and calculate total time elapsed
		for (i = 0; i < 100; i++) {

			// Initialize myblock
			init_block();

			clock_t start = clock();
			switch(j) {
				case 0:
					testA();
					break;
				case 1:
					testB();
					break;
				case 2:
					testC();
					break;
				case 3:
					testD();
					break;
				case 4:
					testE();
					break;
				case 5:
					testF();
					break;
			}
			clock_t end = clock();
			time_elapsed_in_seconds+=(end - start)/(double)CLOCKS_PER_SEC;
		}

		// Print the after execution time of 100 runs
		printf("Time Elapsed test%d: %f secs\n", j, time_elapsed_in_seconds/100);
	}

	return 0;
}

// #endif