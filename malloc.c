/*
Name: Ijaz Mohamed Umar
ID: 1001496537
*/

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)      ((b) + 1)
#define BLOCK_HEADER(ptr)   ((struct _block *)(ptr) - 1)


static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;



struct _block 
{
	size_t  size;         /* Size of the allocated _block of memory in bytes */
	struct _block *prev;  /* Pointer to the previous _block of allcated memory   */
	struct _block *next;  /* Pointer to the next _block of allcated memory   */
	bool   free;          /* Is this _block free?                     */
	char   padding[3];
};


struct _block *freeList = NULL; /* Free list to track the _blocks available */

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
	struct _block *curr = freeList;
	while(curr)
	{
		curr = curr->next;
		num_blocks++;
	}

	printf("\nheap management statistics\n");
	printf("mallocs:\t%d\n", num_mallocs );
	printf("frees:\t\t%d\n", num_frees );
	printf("reuses:\t\t%d\n", num_reuses );
	printf("grows:\t\t%d\n", num_grows );
	printf("splits:\t\t%d\n", num_splits );
	printf("coalesces:\t%d\n", num_coalesces );
	printf("blocks:\t\t%d\n", num_blocks );
	printf("requested:\t%d\n", num_requested );
	printf("max heap:\t%d\n", max_heap );
}


/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes 
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct _block *findFreeBlock(struct _block **last, size_t size) 
{
	
	struct _block *curr = freeList;
	
#if defined FIT && FIT == 0
	// First fit 
	while (curr && !(curr->free && curr->size >= size)) 
	{
		*last = curr;
		curr  = curr->next;
	}
	num_reuses++;
#endif

#if defined BEST && BEST == 0
	//Best Fit
	struct _block *Final = NULL;
	size_t remainingBlock = 0;
	
	while (curr && !(curr->free && curr->size <= size)) 
	{
		*last = curr;
		if(curr->free && (remainingBlock < ((curr->size) - size) 
			|| ((curr->size) - size) >= 0))
		{
			remainingBlock = (curr->size) - size;
			Final = curr;
		}
		curr = curr->next;
	}
	if(Final)curr = Final;
	//num_reuses++;
#endif

#if defined WORST && WORST == 0
	//Worst Fit
	struct _block *Final = NULL;
	size_t remainingBlock = 0;
	
	while (curr && !(curr->free && curr->size >= size)) 
	{
		*last = curr;
		if(curr->free && (remainingBlock > ((curr->size) - size) 
			&& ((curr->size) - size) >= 0))
		{
			remainingBlock = (curr->size) - size;
			Final = curr;
		}
		curr = curr->next;
	}
	if(Final)curr = Final;
	//num_reuses++;
#endif

#if defined NEXT && NEXT == 0
	//Next Fit
	if(curr)curr = curr->next;
	while (curr && !(curr->free && curr->size >= size)) 
	{
		*last = curr;
		curr  = curr->next;
	}
	//num_reuses++;
#endif

	return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size) 
{
	
	num_grows++;
	/* Request more space from OS */
	struct _block *curr = (struct _block *)sbrk(0);
	struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

	assert(curr == prev);

	/* OS allocation failed */
	if (curr == (struct _block *)-1) 
	{
		return NULL;
	}

	/* Update freeList if not set */
	if (freeList == NULL) 
	{
		freeList = curr;
	}	

	/* Attach new _block to prev _block */
	if (last) 
	{
		last->next = curr;
	}

	/* Update _block metadata */
	curr->size = size;
	curr->next = NULL;
	curr->free = false;
	
	return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the 
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{
	//max_heap += size + sizeof(struct _block);
	num_requested+=size;
	if( atexit_registered == 0 )
	{
		atexit_registered = 1;
		atexit( printStatistics );
	}

	/* Align to multiple of 4 */
	size = ALIGN4(size);

	/* Handle 0 size */
	if (size == 0) 
	{
		return NULL;
	}

	/* Look for free _block */
	struct _block *last = freeList;
	struct _block *next = findFreeBlock(&last, size);
	struct _block *curr = next;// just doing it for easy naming and usage
	
	// TODO: Split free _block if possible 
	if(curr && ((curr->size - size) > sizeof(struct _block)))
	{
		num_splits++;
		int old_size = curr->size; 
		struct _block *old_next = curr->next;
		uint8_t *ptr = (uint8_t *)curr;
		curr->next=(struct _block *)(ptr + size + sizeof(struct _block));
		curr->next->free = true;
		curr->next->size = old_size - size - sizeof(struct _block);
		curr->next->next = old_next;
	}
	next=curr;

	//Could not find free _block, so grow heap
	if (next == NULL) 
	{
		next = growHeap(last, size);
		max_heap += size;

	}
	else
	{
		num_reuses++;
	}

	/* Could not find free _block or grow heap, so just return NULL */
	if (next == NULL) 
	{
		return NULL;
	}
   
	/* Mark _block as in use */
	next->free = false;
   
	//increment successful Malloc call
	num_mallocs++;
	
	//increment requested memory
	
	
	/* Return data address associated with _block */
	return BLOCK_DATA(next);
}

void *calloc(size_t nmemb, size_t size)
{
	void *zeroed_Block;
	zeroed_Block = malloc(nmemb*size);
	
	// Calloc zeroes out the memory block
	memset(zeroed_Block, 0, size);
	
	/* Return data address associated with _block which is zeroed out*/
	return zeroed_Block;
	
}

void *realloc(void *ptr, size_t size)
{
	void *newPtr;
	newPtr = malloc(size);
	
	memcpy(newPtr, ptr, size);
	
	//Returns reallocated memory
	return newPtr;
}

/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent 
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */ 
void free(void *ptr)  
{
	if (ptr == NULL) 
	{
		return; 
	}
	num_frees++;
	/* Make _block as free */
	struct _block *curr = BLOCK_HEADER(ptr);
	assert(curr->free == 0);
	curr->free = true;
	
	/* TODO: Coalesce free _blocks if needed */
	while(curr)
	{ 
		if(curr->next && (curr->free && curr->next->free))
		{
			num_coalesces++;
			curr->free = true;
			curr->size = curr->size + (curr->next->size) + sizeof(struct _block);
			curr->next = curr->next->next;
		}
		curr = curr->next;
	}	
}

/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
