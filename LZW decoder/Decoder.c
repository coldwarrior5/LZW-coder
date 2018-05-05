#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX 65536
#define ASCII 256
int  nextEntry, lenCoded = 0;


uint16_t findInDictionary(char word[MAX], char **dict)
{
	for (int search = 0; search < nextEntry; search++)
	{
		if (!strcmp(word, dict[search]))
		{
			return search;
			break;
		}
	}
	return MAX-1;
}

//Decode the input stream
char* LZW(uint16_t *k, int size, char **dict)
{
	int i = 0;
	char loading[MAX]="";
	char *decoded = calloc(lenCoded, sizeof(char));

	while (i < size)
	{
		uint16_t key=k[i];
		i++;
		
		if (key != nextEntry)
		{
			char word[MAX] = "";
			strcpy_s(word, strlen(dict[key]) + 1, dict[key]);

			if (nextEntry < MAX-1) 
			{
				char together[MAX] = "";
				int loadingSize = strlen(loading);
				strcpy(together, loading);
				strcat(together,  word);
				if (findInDictionary(together, dict) == MAX - 1)	//find in the dictionary
				{
					char added[MAX] = "";
					strcpy(added, loading);
					for (int check = 0; check < (int)strlen(word); check++)
					{
						strncat(added, &word[check], 1);

						if (findInDictionary(added, dict) == MAX - 1)
						{
							if (nextEntry < MAX - 1)
							{
								dict[nextEntry] = malloc((strlen(added) + 1) * sizeof(char));
								strcpy(dict[nextEntry], added);
								nextEntry++;
							}
							strncpy(loading, &together[loadingSize + check], strlen(together) - check + 1);
							break;
						}

					}
					lenCoded += strlen(word);
					decoded = realloc(decoded, (lenCoded + 1)*sizeof(char));
					strcat(decoded, word);
				}
				else
				{
					if (lenCoded != 0)
					{
						strcpy(loading,  together);
					}
					else
					{
						lenCoded += strlen(word);
						decoded = realloc(decoded, (lenCoded + 1)*sizeof(char));
						strcpy(decoded, word);
						strcpy(loading, together);
					}
				}
			}
			else
			{
				lenCoded += strlen(word);
				decoded = realloc(decoded, (lenCoded + 1)*sizeof(char));
				strcat(decoded, word);
			}
		}
		else 
		{
			char checkWord[MAX] = "";
			strcpy(checkWord, loading);
			strncat(checkWord, &checkWord[0], 1);
			if (nextEntry < MAX - 1)
			{
				dict[nextEntry] = malloc((strlen(checkWord) + 1) * sizeof(char));
				strcpy(dict[nextEntry], checkWord);
				nextEntry++;
			}
			lenCoded += strlen(checkWord);
			decoded = realloc(decoded, (lenCoded + 1)*sizeof(char));
			strcat(decoded, checkWord);
			strcpy(loading, checkWord);
		}
	}
	return decoded;
}


int main() {

	//Used when running the program from console
	/*int argc, char *argv[]
	if ( argc != 4 ) {
	printf( "usage: %s filename", argv[0] );
	exit(1);
	}*/

	FILE *ul, *iz;
	char *decoded;
	char **dict = calloc (MAX, sizeof(char *)); // ascii od 0 do 127, moguce kombinacije 256

	//Loading the dictionary
	int hex = 0x00;
	for (nextEntry = 0; nextEntry < ASCII; nextEntry++)
	{
		dict[nextEntry] = calloc(2, sizeof(char));
		strcpy_s(dict[nextEntry], 2*sizeof(char),(char *)&hex);
		hex = hex + 1;
	}

	//Opening input file
	ul = fopen("Coded/coded_i1", "rb"); //argv[1]
	if (ul != NULL)
	{
		printf("Nemoguce otvoriti ulaznu datoteku");
		exit(2);
	}

	//Number of symbols
	int symbol_counter = 0;
	fseek(ul, 0L, SEEK_END);
	symbol_counter = ftell(ul);
	symbol_counter /= sizeof(int16_t);

	//End the program if file is empty
	if (symbol_counter == 0)
	{
		printf("There is nothing in the file to process");
		exit(1);
	}

	uint16_t *k = calloc(symbol_counter, sizeof(uint16_t));
	fseek(ul, 0L, SEEK_SET);
	fread(k, sizeof(uint16_t), symbol_counter, ul);
	fclose(ul);

	//Entering coding phase
	decoded = LZW(k, symbol_counter, dict);

	//Opening output file
	iz = fopen("Output/output_i1", "wb+"); //
	if (iz != NULL)
	{
		printf("Nemoguce otvoriti izlaznu datoteku");
		exit(2);
	}

	//Writing coded data into output file
	fwrite(decoded, sizeof(char), strlen(decoded), iz);

	fclose(iz);
	free(k);
	free(dict);
	return 0;
}
