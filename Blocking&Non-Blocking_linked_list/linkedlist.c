#include "linkedlist.h"

int linked_list_init(linked_list_t **ll)
{
	if (ll == NULL)
	{
		printf("linked list pointer error \n");
		return -1;
	}

	/* do program */
	/* allocate linked list */
	/* initialize list head */
	/* initialize mutex lock for BLINKED_LIST version */

	(*ll) = (linked_list_t *)malloc(sizeof(linked_list_t));
	node_t *head = (node_t *)malloc(sizeof(node_t));

	head->level = 0;
	head->next = NULL;

	(*ll)->list_head = head;

#ifdef BLINKED_LIST
	int rc = pthread_mutex_init(&((*ll)->list_lock), NULL);
	assert(rc == 0);
#endif

	srand((unsigned)time(NULL));

	return 0;
}

void linked_list_destroy(linked_list_t *ll)
{
	/* do program */
	/* free all nodes in the linked list */
	/* free the linked list */

	node_t *ptr = ll->list_head;

	while (ptr != NULL)
	{
		ll->list_head = ptr->next;
		free(ptr);
		ptr = ll->list_head;
	}
	free(ll->list_head);
	free(ll);
}

long linked_list_get(long key, linked_list_t *ll)
{
	/* do program */
	/* if key is found, return value */
	/* if key is not found, return -1 */

	node_t *now = ll->list_head;

	while (now != NULL)
	{
		if (now->key == key)
		{
			return now->value;
		}
		else
		{
			now = now->next;
		}
	}
	return -1;
}

long linked_list_put(long key, long value, linked_list_t *ll) //here lock & CAS
{															  
	//CAS() -> first argument is address we want to update, second is value that first arg point
	/* do program */
	/* if succeeds, return 0 */
	/* if fails, return -1 */

	node_t *new_node = (node_t *)malloc(sizeof(node_t));


#ifdef BLINKED_LIST //blocked version
	// lock
	pthread_mutex_lock(&(ll->list_lock));
	//i++;
	new_node->key = key;
	new_node->value = value;

	if (ll->list_head->next == NULL)
	{
		ll->list_head->next = new_node;
		new_node->next = NULL;
		new_node->level = 1;
	}
	else
	{
		new_node->level = ll->list_head->next->level + 1;
		new_node->next = ll->list_head->next;
		ll->list_head->next = new_node;
	}

	pthread_mutex_unlock(&(ll->list_lock));
	// unlock
#else

	new_node->key = key;
	new_node->value = value;

	node_t *ptr = ll->list_head->next;
	
	while (CAS(&(ll->list_head->next), ptr, new_node) == 0)
	{
		ptr = ll->list_head->next;
	}

	new_node->next = ptr;



#endif

	return 0;
}
