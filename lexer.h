/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

struct _pair {
	int state;
	int push;
};
typedef struct _pair pair;

enum _boolean {false, true};
typedef enum _boolean bool;

struct tk {
	int type;
	char str[36];
	int num;
	float rnum;
	int lineNo;
};

typedef struct tk TK;
extern int ERROR_FLAG;
//extern pair tTable[136][96];
//extern void populateTransitionTable(pair tTable[136][96]);
