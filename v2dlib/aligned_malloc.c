/*
 * alligned_malloc.c
 *
 *  Created on: Nov 18, 2010
 *      Author: rkale
 */

#include <stdlib.h>
#include <errno.h>
#include "aligned_malloc.h"
#include "memory_tracker.h"

void* aligned_malloc(size_t size, size_t align)
{
	void * malloc_ptr;
	void * aligned_ptr;

	/* Error if align is not a power of two.  */
	if (align & (align - 1))
	{
		errno = EINVAL;
		return ((void*) 0);
	}

	if (size == 0)
		return ((void *) 0);

	/* Assume malloc'd pointer is aligned at least to sizeof (void*).
    If necessary, add another sizeof (void*) to store the value
    returned by malloc. Effectively this enforces a minimum alignment
    of sizeof double. */
	if (align < 2 * sizeof (void *))
		align = 2 * sizeof (void *);

	malloc_ptr = malloc (size + align);
	if (!malloc_ptr)
		return ((void *) 0);

	/* Align  We have at least sizeof (void *) space below malloc'd ptr. */
	aligned_ptr = (void *) (((size_t) malloc_ptr + align)
			& ~((size_t) (align) - 1));

	/* Store the original pointer just before p.  */
	((void **) aligned_ptr) [-1] = malloc_ptr;

	return aligned_ptr;
}

void aligned_free(void * aligned_ptr)
{
	if (aligned_ptr)
		free (((void **) aligned_ptr) [-1]);
}
