/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

#define EoF 95
#define BUFLEN 50
#define TOTALBUFLEN 100
#define INVALID_OP 146
#define INVALID_ID 147
#define OPEN_CMNT 148
#define INVALID_CONST 149
#define testCase "test.txt"

char buf1[BUFLEN], buf2[BUFLEN];
int noOfChars = 0;
bool moreToRead = false;
int bufIndex = -1, lineNo = 0, curState = -1, nextState = -1;
char lexeme[30];
int lexLen = 0;
char fileEnd = (char) 3;
bool fileEndReached = false;
FILE* file = NULL; 
pair tTable[136][96];


void populateTransitionTable(pair tTable[136][96]) 
{
	FILE *f = fopen("tTable2.csv", "r");
	
	int i=0, j=0;
	while (i<136) {
		while (j<96) {
			fscanf(f, "%d,%d,", &(tTable[i][j].state), &(tTable[i][j].push));
			//printf("%d\t %d\n", tTable[i][j].state, tTable[i][j].push);
			j+=1;
		}
		j=0;
		i+=1;
		fgetc(f);
	}
	fclose(f);
}

void debug(char *str)
{
	printf("%s\n", str);
	printf("buf1 :: %s \t buf2 :: %s\tbufIndex :: %d \n", buf1, buf2, bufIndex);
	//printf("\n moreToRead:: %d \t No of chars read from file last:: %d \t remainingCharsRead in last iter :: %d \n ", moreToRead, noOfChars, remainingCharsRead);
	lexeme[lexLen] = '\0';
	printf("Lexeme :: %s\n", lexeme);
}
/*
Parser calls getTokens() fn, takes 10 tokens at a time. 
2 buffers, from source code buf1, buf2
Gets file name from *********
*/
FILE* openFile(char fileName[], char mode[])
{
	FILE* f = fopen(fileName, mode);
	if(!f)
	{
		printf("Error in opening file!\n");
		exit(1);
	}
	return f;
}	

bool isNumType(int tokenType)
{
	//TODO return tokenType == hash_find(tnt_hash, "NUM");
	return tokenType == 105;
}
bool isRNumType(int tokenType)
{
	//TODO return tokenType == hash_find(tnt_hash, "RNUM");
	return tokenType == 121;
}

void printToken(TK token[], int i)
{
	printf("Token %d: \t tokenType: %d \t token.lineNo %d\t", i+1, token[i].type, token[i].lineNo);
	if (isNumType(token[i].type))
	{
		printf("Content NUMBER: %d \n", token[i].num);
	}
	else if(isRNumType(token[i].type))
	{
		printf("Content REAL: %f\n",token[i].rnum);
	}
	else
	{
		printf("Content: %s\n", token[i].str);
	}
}

bool fillBuffer(char buf[])
{
	//returns false if eof is reached
	//noOfChars is number of chars read from file! if its < BUFLEN => eof reached!
	noOfChars = fread(buf, sizeof(char), BUFLEN, file);
	if (noOfChars < BUFLEN) 					//eof has been reached
	{
		buf[noOfChars] = fileEnd; 				//insert eof character
		return false;
	}
	return true;
}

void reloadBuffers(int initFlag)
{
	//if i=0 => initial fill 
	//else   => other fill
	if ((moreToRead && bufIndex == BUFLEN) || initFlag){
		moreToRead = fillBuffer(buf1); 
	}

	if (moreToRead && (bufIndex == TOTALBUFLEN || initFlag)){
		moreToRead = fillBuffer(buf2);
	}
}

char getNextChar()
{
	char ch;
	if ( bufIndex >= 0 && bufIndex < BUFLEN)
	{
		ch = buf1[bufIndex];
	}
	else if ( bufIndex >= BUFLEN && bufIndex< TOTALBUFLEN)
	{
		ch = buf2[bufIndex - BUFLEN];
	}
	
	bufIndex++;

	//reload buffers at bufIndex = BUFLEN and bufIndex  = TOTALBUFLEN
	if (moreToRead && (bufIndex==BUFLEN || bufIndex==TOTALBUFLEN))
	{
		reloadBuffers(0);
	}

	if(bufIndex == TOTALBUFLEN) 
		bufIndex = 0;
	return ch;	
}

void setLexer()
{
	file = openFile(testCase, "r");
	populateTransitionTable(tTable);
	reloadBuffers(1);
	bufIndex = 0;
	curState = 0;
	lineNo = 1;
}

void addToLexeme(char ch)
{
	if(lexLen == 25)
	{
		//25 is max string length
		lexeme[25] = '\0';
		printf("Lexical error at Line Number %d: Lexeme %s.. longer than 25 chars. Resetting..\n", lineNo, lexeme);
		lexLen = 0;
		//exit(1);
	}

	lexeme[lexLen++] = ch;
}

void lexicalError(tokenType)
{
	char msg[80];
	switch(tokenType)
	{
		case INVALID_OP 	: strcpy(msg, "Unrecognised operator/symbol "); break;
		case INVALID_ID 	: strcpy(msg, "Invalid Identifier. Hint: Id can't begin with number, has to be alphabets! "); break;
		case INVALID_CONST 	: strcpy(msg, "Invalid constant "); break;
		case OPEN_CMNT 		: strcpy(msg, "Comment not closed "); break;
	}
	printf("%s \" %s \" at line number: %d\n", msg, lexeme, lineNo);
	char fun[20];
	gets(fun);
	ERROR_FLAG = 1;
}

void createToken(TK tkList[], int tokenType, int* listLen)
{
	TK newToken;
	lexeme[lexLen] = '\0';
	
	if(lexLen == 0) {
		//do nothing. might be the case when last token was created and eof is reached.
		//Ex: Main EoF   ==> lexLen is zero.
	}
	else if (tokenType == INVALID_OP || tokenType == INVALID_ID || tokenType == OPEN_CMNT || tokenType == INVALID_CONST) {
		lexicalError(tokenType);
	}
	else {

		newToken.type = tokenType;
		newToken.lineNo = lineNo;

		if (isNumType(tokenType))
		{
			newToken.num = atoi(lexeme);
			strcpy(newToken.str, "\0");
		}
		else if(isRNumType(tokenType))
		{
			newToken.rnum = (float)atof(lexeme);
			strcpy(newToken.str, "\0");
		}
		else
		{
			strcpy(newToken.str, lexeme);
		}

		tkList[*listLen] = newToken;
		printToken(tkList, *listLen);
		++(*listLen);
	}

	lexLen = 0;
}

int getTokens(TK tkList[], int noOfTokens) 
{
	if(fileEndReached){						//if eof has already been encountered return without processing
		return 0;
	}
	char ch;
	pair transition;
	int tkListLen = 0;
	bool lineInc;

	if(bufIndex < 0)						//valid bufIndex values - zero to TOTALBUFLEN-1 
		setLexer();							//start reading from file, fill buffers, initialise lexer variables

	while(true)
	{
		ch = getNextChar();					//check buffer and get character
		
		if (ch == '\n'){					//increment line number after processing
			ch = ' '; 
			lineInc = true;
		}
		else lineInc = false;

		if (ch>=32 && ch <127)
		{	
			//if encounteredy is within the table
			//get next state and token push status
			transition = tTable[curState][ch-32];
			nextState = transition.state;
			//printf("curState: %d, ch-32: %d\n", curState, ch-32);
			if(transition.push != 0)
			{
				createToken(tkList, transition.push, &tkListLen);

			}
			if (ch != ' ' && curState != 23)
			{
				//add present char to lexeme if not space or comment
				addToLexeme(ch);
			}
			if(curState == 24 && nextState == 0)
			{
				//comment end 	//reset lexeme
				lexLen = 0;
			}
		}
		else if (ch > 127) {
			printf("Unrecognised character at line Number %d: \" %c \"..Aborting..\n", lineNo, ch);
		}
		else if (ch == fileEnd) {
			createToken(tkList, tTable[curState][EoF].push,&tkListLen);
			fileEndReached = true;
			return tkListLen;
		}

		curState = nextState;
		if (lineInc) ++lineNo;
				
		if(tkListLen == noOfTokens)
			return tkListLen;	
	}
}
/*
int main()
{
	TK list[10], token;
	int num = 10;
	int i;
	while(num == 10)
	{
		num = getTokens(list, 10);
	}

	return 0;
}*/

