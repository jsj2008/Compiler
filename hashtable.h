/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#define SIZE 100

struct node {
	int type;
	char str[30];
	struct node *next;
};
typedef struct node *NODE;

struct hashtable {
	NODE buckets[SIZE];
	NODE tails[SIZE];
	int count;
};
typedef struct hashtable *HASHTABLE;
extern HASHTABLE tnt_hash;
extern char tnt_arr[147][40];