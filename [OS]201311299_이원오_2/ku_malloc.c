#include "ku_malloc.h"

void free_change(hnode_t* ptr, size_t size)		//change hnode block to hheader block
{
	hheader_t* block_ptr;
	hheader_t* tmp = (hheader_t*)ptr + FREE_SIZE;
	block_ptr = (hheader_t*) ptr;
	block_ptr->size = size;
	block_ptr->magic = (unsigned long)tmp ^ KU_MAGIC;
	unlink_free((hnode_t *)block_ptr);
}

hnode_t* next_find_fit(size_t size)
{
	hnode_t * tmp = global_base;
	if(free_list != NULL)
	{
		if(global_base != NULL)
		{
			while(tmp!= NULL)
			{
				if(tmp->size > size)
				{
					global_base = tmp;
					return tmp;
				}
				tmp = tmp->next;
			}
			tmp = free_list;
			while(tmp != global_base)
			{
				if(tmp->size > size)
				{
					global_base = tmp;
					return tmp;
				}
				tmp = tmp->next;
			}
			return NULL;
		}
		else		// begin from first
		{
			while(tmp!=NULL)
			{
				if(tmp->size > size)
				{
					global_base = tmp;
					return tmp;
				}
				tmp = tmp->next;
			}
			return NULL;
		}		
	}
	else
	{
		return NULL;	
	}	

}

void link_free(hnode_t* ptr)		
{
	hnode_t* tmp = free_list;
	while(tmp->next != NULL)
	{
		if(ptr < tmp->next && ptr>tmp)
		{
			if(ptr->size+FREE_SIZE <= (tmp->next) - (tmp+FREE_SIZE + tmp->size))
			{
				tmp->next = ptr;
				ptr->next = tmp->next;
				return;
			}
		}
		tmp = tmp->next;
	}
	tmp->next = ptr;
	ptr->next = NULL;
}


void unlink_free(hnode_t* ptr)
{
	hnode_t* tmp = free_list;
	while(tmp != NULL)
	{
		if(tmp == ptr)
		{
			hnode_t* prev = prev_search(ptr);
			prev->next = ptr->next;
		}
		tmp = tmp->next;
	}
}

hnode_t* prev_search(hnode_t* ptr)
{
	hnode_t* tmpNode;
	tmpNode = free_list;

	if(tmpNode == ptr)
	return tmpNode;	

	while(tmpNode->next != ptr)
	{
		tmpNode = tmpNode->next;
	}	
	return tmpNode;
}

void coalescing()
{
	hnode_t* tmp;
	tmp = free_list;
	while(tmp->next != NULL)
	{
		if(tmp + tmp->size + FREE_SIZE == tmp->next)
		{
			hnode_t* next = tmp->next->next;
			tmp->size = FREE_SIZE + tmp->size + tmp->next->size;
			tmp->next = next;
		}	
		tmp = tmp->next;
	}
}



void* malloc(size_t size)
{
	void* ptr;
	hheader_t* expand_ptr;
	hnode_t next_ptr;
	
	if(size == 0)
	{
		fprintf(stderr, "[Alloc] Addr : %p Length : 0\n",ptr);
		return NULL;
	}

	hnode_t* heap_list = next_find_fit(size); // find the best place to alloc

	if(heap_list != NULL)
	{
		free_change(heap_list, size);
		coalescing();
		heap_list += FREE_SIZE;
		fprintf(stderr, "[Alloc] Addr : %p Length : %lu\n", heap_list, size);
		return heap_list;
	}
	else		// fail to find the best place
	{
		expand_ptr = sbrk(FREE_SIZE + size);
		hheader_t *test = expand_ptr + FREE_SIZE;
		expand_ptr->size = size;
		expand_ptr->magic = ((unsigned long)test) ^ KU_MAGIC;		

		if(expand_ptr)
		{
			fprintf(stderr, "[Alloc] Addr : %p Length : %lu\n", expand_ptr+FREE_SIZE, size);
			unsigned long t_size = expand_ptr->size;
			return expand_ptr + FREE_SIZE;
		}
		else
		{
			fprintf(stderr, "[Alloc] Out of Heap Space!\n");
			return NULL;
		}
	}
}

void free(void* ptr)
{
	hheader_t* h_ptr = (hheader_t *)ptr;	
	hheader_t* hi = h_ptr - FREE_SIZE;
	hnode_t* rel;
	unsigned long size;
	unsigned long magic;
	size = hi->size;
	magic = hi->magic;
	
	if(((unsigned long)h_ptr ^ KU_MAGIC) == magic)
	{
		rel = (hnode_t *)hi;
		rel->size = hi->size;
		size = rel->size;

		if(free_list == NULL)
		{
			free_list = rel;
			free_list->next = NULL;
		}
		else
		{
			link_free(rel);		// find the right free Node and Add
			coalescing();		// sort the Free_list and merge 
		}
	}
	else
	{
		fprintf(stderr, "Cannot Free! Wrong approach!\n");
		return;
	}
	fprintf(stderr, "[Free] Addr : %p Length : %lu\n", ptr, size);
	
}


