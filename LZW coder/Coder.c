#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX 65536
#define ASCII 256
int lenCoded = 0, nextEntry = 0;

typedef struct trieNode {
	char key;
	uint16_t value;
	struct trieNode *next;
	struct trieNode *prev;
	struct trieNode *children;
	struct trieNode *parent;
} trieNode_t;

void TrieCreate(trieNode_t **root);
trieNode_t *TrieCreateNode(char key, uint16_t data);
trieNode_t* TrieSearch(trieNode_t *root, const char *key);
void TrieAdd(trieNode_t **root, char *key, uint16_t data);
void TrieRemove(trieNode_t **root, char *key);
void TrieDestroy(trieNode_t* root);

void TrieCreate(trieNode_t **root)
{
	*root = TrieCreateNode('\0', 0);
}

trieNode_t *TrieCreateNode(char key, uint16_t data)
{
	trieNode_t *node = NULL;
	node = (trieNode_t *)malloc(sizeof(trieNode_t));

	if (NULL == node)
	{
		printf("Malloc failed\n");
		return node;
	}

	node->key = key;
	node->next = NULL;
	node->children = NULL;
	node->value = data;
	node->parent = NULL;
	node->prev = NULL;
	return node;
}

void TrieAdd(trieNode_t **root, char *key, uint16_t data)
{
	trieNode_t *pTrav = NULL;

	if (NULL == *root)
	{
		printf("NULL tree\n");
		return;
	}

	pTrav = (*root)->children;

	if (pTrav == NULL)
	{
		// First Node
		for (pTrav = *root; *key; pTrav = pTrav->children)
		{
			pTrav->children = TrieCreateNode(*key, 0xffff);
			pTrav->children->parent = pTrav;
			key++;
		}

		pTrav->children = TrieCreateNode('\0', data);
		pTrav->children->parent = pTrav;

		return;
	}

	if (TrieSearch(pTrav, key))
	{
		printf("Duplicate!\n");
		return;
	}

	while (*key != '\0')
	{
		if (*key == pTrav->key)
		{
			key++;

			pTrav = pTrav->children;
		}
		else
			break;
	}

	while (pTrav->next)
	{
		if (*key == pTrav->next->key)
		{
			key++;
			TrieAdd(&(pTrav->next), key, data);
			return;
		}
		pTrav = pTrav->next;
	}

	if (*key)
	{
		pTrav->next = TrieCreateNode(*key, 0xffff);
	}
	else
	{
		pTrav->next = TrieCreateNode(*key, data);
	}

	pTrav->next->parent = pTrav->parent;
	pTrav->next->prev = pTrav;


	if (!(*key))
		return;

	key++;

	for (pTrav = pTrav->next; *key; pTrav = pTrav->children)
	{
		pTrav->children = TrieCreateNode(*key, 0xffff);
		pTrav->children->parent = pTrav;
		key++;
	}

	pTrav->children = TrieCreateNode('\0', data);
	pTrav->children->parent = pTrav;
	return;
}

trieNode_t* TrieSearch(trieNode_t *root, const char *key)
{
	trieNode_t *level = root->children;
	trieNode_t *pPtr = NULL;

	int lvl = 0;
	while (1)
	{
		trieNode_t *found = NULL;
		trieNode_t *curr;

		for (curr = level; curr != NULL; curr = curr->next)
		{
			if (curr->key == *key)
			{
				found = curr;
				lvl++;
				break;
			}
		}

		if (found == NULL)
			return NULL;

		if (*key == '\0')
		{
			pPtr = curr;
			return pPtr;
		}

		level = found->children;
		key++;
	}
}

void TrieRemove(trieNode_t **root, char *key)
{
	trieNode_t *tPtr = NULL;
	trieNode_t *tmp = NULL;

	if (NULL == *root || NULL == key)
		return;

	tPtr = TrieSearch((*root)->children, key);

	if (NULL == tPtr)
	{
		printf("Key [%s] not found in trie\n", key);
		return;
	}

	while (1)
	{
		if (tPtr->prev && tPtr->next)
		{
			tmp = tPtr;
			tPtr->next->prev = tPtr->prev;
			tPtr->prev->next = tPtr->next;
			free(tmp);
			break;
		}
		else if (tPtr->prev && !(tPtr->next))
		{
			tmp = tPtr;
			tPtr->prev->next = NULL;
			free(tmp);
			break;
		}
		else if (!(tPtr->prev) && tPtr->next)
		{
			tmp = tPtr;
			tPtr->parent->children = tPtr->next;
			free(tmp);
			break;
		}
		else
		{
			tmp = tPtr;
			tPtr = tPtr->parent;
			tPtr->children = NULL;
			free(tmp);
		}
	}
}


void TrieDestroy(trieNode_t* root)
{
	trieNode_t *tPtr = root;
	trieNode_t *tmp = root;

	while (tPtr)
	{
		while (tPtr->children)
			tPtr = tPtr->children;

		if (tPtr->prev && tPtr->next)
		{
			tmp = tPtr;
			tPtr->next->prev = tPtr->prev;
			tPtr->prev->next = tPtr->next;
			free(tmp);
		}
		else if (tPtr->prev && !(tPtr->next))
		{
			tmp = tPtr;
			tPtr->prev->next = NULL;
			free(tmp);
		}
		else if (!(tPtr->prev) && tPtr->next)
		{
			tmp = tPtr;
			tPtr->parent->children = tPtr->next;
			tPtr->next->prev = NULL;
			tPtr = tPtr->next;
			free(tmp);
		}
		else
		{
			tmp = tPtr;
			if (tPtr->parent == NULL)
			{
				//Root
				free(tmp);
				return;
			}
			tPtr = tPtr->parent;
			tPtr->children = NULL;
			free(tmp);
		}
	}

}

//Code the input stream
uint16_t* LZW(char *k, int size, trieNode_t **root)
{
	int i = 0;
	char word[MAX] = "";
	uint16_t *coded = calloc(lenCoded, sizeof(uint16_t));
	word[0] = k[i];
	i++;

	while (i < size)
	{
		char next[2] = "", together[MAX] = "";
		next[0] = k[i];
		i++;
    		strcpy(together, word);
		strcat(together,  next);	

		if (TrieSearch(*root, together) != NULL)
		{
			strcpy(word, together);
		}
		else
		{
			lenCoded++;
			coded = realloc(coded, (lenCoded + 1)*sizeof(uint16_t));
			trieNode_t *result = TrieSearch(*root, word);
			coded[lenCoded - 1] = result->value;
			if (nextEntry < MAX - 1)
			{
				TrieAdd(root, together, nextEntry);
				nextEntry++;
			}
			strcpy(word, next);
		}
	}
	lenCoded++;
	coded = realloc(coded, (lenCoded + 1)*sizeof(uint16_t));
	trieNode_t *result = TrieSearch(*root, word);
	coded[lenCoded - 1] = result->value;
	return coded;
}


int main() {

	//Used when running the program from console
	/*int argc, char *argv[]
	if ( argc != 4 ) {
	printf( "usage: %s filename", argv[0] );
	exit(1);
	}*/
	
	FILE *ul, *iz;
	uint16_t *coded;
	trieNode_t *root; // ASCII from 0 till 255
	TrieCreate(&root);

	//Loading the dictionary
	int hex = 0x01;
	for (nextEntry = 1; nextEntry < ASCII; nextEntry++)
	{
		TrieAdd(&root, (char *)&hex, nextEntry);
		hex += 1;
	}

	//Opening input file
	ul = fopen("Input/input_i1", "rb"); //argv[1]
	if (ul != NULL)
	{
		printf("Nemoguce otvoriti ulaznu datoteku");
		exit(2);
	}

	//Number of symbols
	int symbol_counter = 0;
	fseek(ul, 0L, SEEK_END);
	symbol_counter = ftell(ul);

	//End the program if file is empty
	if (symbol_counter == 0)
	{
		printf("There is nothing in the file to process");
		exit(1);
	}

	char *k = calloc(symbol_counter + 1, sizeof(char));
	fseek(ul, 0L, SEEK_SET);
	fread(k, sizeof(char), symbol_counter, ul);
	fclose(ul);

	//Entering coding phase
	coded = LZW(k, symbol_counter, &root);

	//Opening output file
	iz = fopen("Coded/coded_i1", "wb+"); //
	if (iz != NULL)
	{
		printf("Nemoguce otvoriti izlaznu datoteku");
		exit(2);
	}

	//Writing coded data into output file
	fwrite(coded, sizeof(uint16_t), lenCoded, iz);

	fclose(iz);
	free(coded);
	free(k);
	/*Destroy the trie*/
	TrieDestroy(root);
	return 0;
}
