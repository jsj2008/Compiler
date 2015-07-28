/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

HASHTABLE createHashtable() {
	HASHTABLE hashtbl = (HASHTABLE)malloc(sizeof(struct hashtable));
	//hashtbl->buckets = (NODE)malloc(sizeof(struct node)*50);
	hashtbl->count = 0;
	//printf("allocated inside\n");
	int i;
	for (i=0; i<SIZE; i++) {
		hashtbl->buckets[i] = NULL;
		hashtbl->tails[i] = NULL;
	}
	
	return hashtbl;
}

HASHTABLE insert_list(HASHTABLE hashtbl, int hashedVal, char str[], int type) {
	NODE cur = hashtbl->buckets[hashedVal];
	hashtbl->count++;
	NODE newNode = (NODE)malloc(sizeof(struct node));
	strcpy(newNode->str, str);
	newNode->type = type;
	newNode->next = NULL;
	
	if (cur == NULL) {
		//printf("empty");
		hashtbl->buckets[hashedVal] = newNode;
		hashtbl->tails[hashedVal] = newNode;
		//printf("inserted at bucket: %d, id: %d, val: %s\n", hashedVal, newNode->type, newNode->str);
		return hashtbl;
	}
	
	//printf("not empty");
	/*
	while(cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = newNode;
	*/
	hashtbl->tails[hashedVal]->next = newNode;
	hashtbl->tails[hashedVal] = newNode;
	//printf("inserted at bucket: %d, id: %d, val: %s\n", hashedVal, newNode->type, newNode->str);

	return hashtbl;
}

int hash(char str[]) {
	int len = strlen(str);
	int i, sum = 0;
	
	for (i=0; i<len; i++) {
		sum += (int)str[i];
	}
	return sum % SIZE;
}

void insert_hash(HASHTABLE hashtbl, char str[], int type) {
	int hashedVal = hash(str);
	//printf("hashedVal: %d\n", hashedVal);
	hashtbl = insert_list(hashtbl, hashedVal, str, type);
	
	//return hashtbl;	
}

void print_hash(HASHTABLE hashtbl) {
	int i;
	printf("hashtable: %d\n", hashtbl->count);
	for(i=0;i<SIZE;i++) {
		NODE cur = hashtbl->buckets[i];
		printf("%d:\t", i);
		while (cur != NULL) {
			printf("%s, %d  ==>  ", cur->str, cur->type);
			cur = cur->next;
		}
		printf("\n");
	}
	printf("--------------------\n");
}

HASHTABLE delete_hash(HASHTABLE hashtbl, char str[]) {
	int hashValue = hash(str);
	NODE cur = hashtbl->buckets[hashValue];
	NODE prev = hashtbl->buckets[hashValue];
	if (strcmp(cur->str, str) == 0) {
		hashtbl->tails[hashValue] = hashtbl->buckets[hashValue]->next;	
		hashtbl->buckets[hashValue] = hashtbl->buckets[hashValue]->next;
		return hashtbl;
	}
	
	while (cur != NULL) {
		if (strcmp(cur->str, str) == 0) {
			if (cur->next != NULL) {
				hashtbl->tails[hashValue] = cur->next;
			}
			else {
				hashtbl->tails[hashValue] = prev;	
			}

			prev->next = cur->next;
			return hashtbl;
		}
		cur = cur->next;
	}
	return hashtbl;
}

int find_hash(HASHTABLE hashtbl, char str[]) {
	int hashValue = hash(str);
	//printf("hash of inside find_hash: %s = %d\n", str, hashValue);
	NODE cur = hashtbl->buckets[hashValue];
	while(cur != NULL) {
		if (strcmp(cur->str, str) == 0) {
			return cur->type;
		}
		cur = cur->next;
	}
	return -1;
}

/*int main() {
	FILE *fp = fopen("tnt.txt", "r");
	HASHTABLE hashtbl = createHashtable();
	int count = 0;

	char str[30];
	while (!feof(fp)) {
		fscanf(fp, "%s\n", str);
		insert_hash(hashtbl, str, count);
		count++;
	}
	print_hash(hashtbl);
	return 0;
}*/
