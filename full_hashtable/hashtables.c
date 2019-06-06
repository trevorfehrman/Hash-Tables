#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
  Hash table key/value pair with linked list pointer.
  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
typedef struct LinkedPair {
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable {
  int capacity;
  LinkedPair **storage;
} HashTable;

void hash_table_remove(HashTable *ht, char *key);
char *hash_table_retrieve(HashTable *ht, char *key);
/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL) {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function
  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char * u_str = (unsigned char *)str;

  while ((c = *u_str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

/*
  Fill this in.
  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
	HashTable *ht = malloc(sizeof(HashTable));
	ht->capacity = capacity;
	ht->storage = calloc(capacity, sizeof(LinkedPair *));
	return ht;
}

/*
  Fill this in.
  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.
  Inserting values to the same index with existing keys can overwrite
  the value in th existing LinkedPair list.
 */

void hash_table_insert(HashTable *ht, char *key, char *value)
{
	// Check to see if this key already exists, if so delete the old version. 
    if (hash_table_retrieve(ht, key) != NULL)
	{
      	hash_table_remove(ht, key);
    }

	// find the right place to insert the new element
    LinkedPair *linkedPair = create_pair(key,value);
    int address = hash(key, ht->capacity);
    if (ht->storage[address] == NULL)
	{
        ht->storage[address] = linkedPair;
	}
    else
    {	//loop over the length of the LL until the last one is found, and insert there
		LinkedPair *linkedNextPair = ht->storage[address];
		while(linkedNextPair->next != NULL)
		{
			linkedNextPair = linkedNextPair->next;
		}
		linkedNextPair->next = linkedPair; 
    }
}

/*
  Fill this in.
  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.
  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
	// find the location of the thing to delete
    int address = hash(key, ht->capacity);
    if (strcmp(ht->storage[address]->key, key) == 0)
    { // easy access to the hash table location
        LinkedPair *scheduledForDeletion = ht->storage[address];
		ht->storage[address] = ht->storage[address]->next;
		destroy_pair(scheduledForDeletion);
    }
    else
    { // loop through the LL to find the item to delete
        LinkedPair *linkedNextPair = ht->storage[address];
        while(linkedNextPair->next != NULL)
        {
            if (strcmp(linkedNextPair->next->key, key) == 0)
            {
                LinkedPair *scheduledForDeletion = linkedNextPair->next;
				// keep track of the connections to keep the LL intack
                linkedNextPair->next = linkedNextPair->next->next;    
                destroy_pair(scheduledForDeletion);
                break;
            }
            linkedNextPair = linkedNextPair->next;
        }        
    }
}

/*
  Fill this in.
  Should search the entire list of LinkedPairs for existing
  keys.
  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{	// check to see if the item exists in the table, null if not
    int address = hash(key, ht->capacity);
    if (ht->storage[address] == NULL)
    {
      return NULL;
    }
    else
    { // check to see if the item exists in the LL at the hash location by looping over the LL
        LinkedPair *linkedNextPair = ht->storage[address];
        while(linkedNextPair != NULL)
        {
          if (strcmp(linkedNextPair->key, key) == 0) 
          {
            return linkedNextPair->value;
          } 
          linkedNextPair = linkedNextPair->next;
        }
        return NULL;    
    }
}

/*
  Fill this in.
  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{	// loop over every item in the table
    for(int i = 0; i < ht->capacity; i++)
    {	// if there is something there, delete it. 
        if (ht->storage[i] != NULL)
        {	// but make sure to first traverse the LL and delete everything in it
            LinkedPair *linkedNextPair = ht->storage[i]->next;
            while(linkedNextPair != NULL)
            {
                LinkedPair *scheduledForDeletion = linkedNextPair;
                linkedNextPair = linkedNextPair->next;
                destroy_pair(scheduledForDeletion);
            }
        }
        destroy_pair(ht->storage[i]);
    }
    free(ht);
}

/*
  Fill this in.
  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.
  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{	// create a new hash table with double size 
  	HashTable *new_ht = create_hash_table(ht->capacity * 2);
	
	// loop over every element in the old hash table
	for(int i = 0; i < ht->capacity; i++)
    {	// when you find something... 
		if (ht->storage[i] != NULL) 
		{	//take it and insert it into the new hsh table
			hash_table_insert(new_ht, ht->storage[i]->key, ht->storage[i]->value);
			// and make sure to insert all the LL it may have attached. 
			if (ht->storage[i]->next != NULL)
			{
				LinkedPair *linkedNextPair = ht->storage[i]->next;
				hash_table_insert(new_ht, linkedNextPair->key, linkedNextPair->value);
				while(linkedNextPair->next != NULL)
				{
					linkedNextPair = linkedNextPair->next;
					hash_table_insert(new_ht, linkedNextPair->key, linkedNextPair->value);
				}
			}
		}
    }
    destroy_hash_table(ht); // the king is dead
  	return new_ht; 			// long live the king
}


#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n");
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n");

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));

  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);

  destroy_hash_table(ht);

  return 0;
}
#endif