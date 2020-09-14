// I have used . to show the missing characters instead of _

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int LoadQAs(char *, char **, char **);	// Function to Load all the text in the file
char * DispClue(int difficulty, char * answer);
void WriteoutQuizResult(char *QuestionFile, int correctTotal, int askedTotal, int difficultylevel);
void randomNumbers(int *random1, int *random2, int len);
int stricmp (const char *p1, const char *p2);

// *argv[0] 1st command line parameter - filename containing questions
// *argv[1] 2nd command line parameter - difficulty level

int main(int argc, char const *argv[])
{
	char **allQs = NULL; // pointer for holding the list of questions
	char **allAs = NULL; // pointer for holding the list of answers
	int noOfQs = 0;
	
	char input[100];	// to store the answer from the user.
	char output[100];
	int correctTotal = 0;	// To calculate the score
	int askedTotal = 0;
	int result;
    int difficultylevel = 2;
	char QuestionFile[100];
		
	// get command line values - hardcoded currently
	difficultylevel = 4;
	strcpy(QuestionFile, "QnA.txt");

	noOfQs = LoadQAs(QuestionFile, (char **) &allQs, (char **) &allAs); // load the questions & answers
	
  printf("Total Number of Questions: %d\n", noOfQs);

	correctTotal = 0;
	for (askedTotal = 0; askedTotal < noOfQs; askedTotal++)
  {
		printf("%s\n", (char *) allQs[askedTotal]);	// Ask next question
		printf("%s\n", DispClue(difficultylevel, (char *) allAs[askedTotal]));	// display clue

		// get users input as answer
		printf("Enter the Answer: ");
		scanf("%s", input);

		// check answer
    //printf("checking Q %d : input [%s] v stored answer [%s]", askedTotal, input, (char *) allAs[askedTotal]);
	  if(stricmp(input, (char *) allAs[askedTotal]) == 0)
    {
      printf("Answer is Correct\n");
      correctTotal++;
    }
    else
    {
      printf("Answer is Wrong\n");
    }

		// display correct or incorrect and score so far
    printf("Score so far: %d/%d\n",correctTotal, askedTotal+1);
        
	} // for loop askedTotal
	
	
	// store results in quiz_history.txt
	WriteoutQuizResult(QuestionFile, correctTotal, askedTotal, difficultylevel);

	return 0;
} // main


//=========


void WriteoutQuizResult(char *QuestionFile, int correctTotal, int askedTotal, int difficultylevel)
{
	FILE * op;
		
    printf("All the questions were answered.\n");
    op = fopen("quiz_history.txt", "w");
	if(op != 0)
	{
		fprintf(op, "%s - Answered: %d of Total Questions: %d  - Difficulty Level %d", QuestionFile, correctTotal, askedTotal, difficultylevel);	// Write to the output file
		fclose(op);
	}
	else
	{
		perror("Error Reading the File");
	}
} // WriteoutQuizResult


// LoadQAs allocates memory for allQs and allAs, so takes pointers to the variable that storss the pointer 
int LoadQAs (char *QuestionFile, char **allQs, char **allAs)
{
	int eOF = 0;	// to find the end of the file
	char *line = malloc(100);	// to store one line from the file
	int noOfQs = 0;
	FILE * fp;
	int length;
	char *QMarkCharPos; 
  char *eolchar;
	int QMarkChar = '?';
  char **Qs, **As;

	Qs = malloc(sizeof(char *) * 100); // allocate for 100 questions, to start with
	As = malloc(sizeof(char *) * 100); // and for 100 answers. We will adjust later

	fp = fopen(QuestionFile, "r");	// Reading the file
	if (fp != NULL)
	{
		eOF = (fgets(line, 100, fp) == NULL);
		while (!eOF)
		{
			// extract question and answer from line
			QMarkCharPos = strrchr(line, QMarkChar);
			*QMarkCharPos = 0;

      if(*(QMarkCharPos + strlen(QMarkCharPos + 2)) == 13) // delete CR char at end of line
      {
        *(QMarkCharPos + strlen(QMarkCharPos + 2)) = 0;
      }

			Qs[noOfQs] = malloc(strlen(line)+1);
			As[noOfQs] = malloc(strlen(QMarkCharPos + 2)+1);
			strcpy(Qs[noOfQs], line); // copy the question, up to the '?'
			strcpy(As[noOfQs], (QMarkCharPos + 2)); // copy 2 characters after '?' to end of the line

      //printf("Q reading %d [%s] [%s]\n", noOfQs, Qs[noOfQs], As[noOfQs]);
			
      noOfQs++;

			eOF = (fgets(line, 100, fp) == NULL);
		} // while ! EOF
    
    fclose(fp); // Close QnA file

	} // if != NULL
  

	*allQs = realloc(*allQs, sizeof(char *) * (noOfQs+1)); 
	*allAs = realloc(*allAs, sizeof(char *) * (noOfQs+1));

  *allQs = (char *)Qs;
	*allAs = (char *)As;
  
  return(noOfQs);
	
} // LoadQAs


char * DispClue(int difficulty, char * answer)
{
	char *cluestr = (char *)malloc(100);
  char temp[10];
	int i;
	int len;
	int rand1, rand2;
	
  //printf("DispClue Ans in [%s]\n", answer);
	*cluestr = 0; // empty string - null first char
	switch(difficulty)
	{
		case 1:
			// no clue given
			*cluestr = 0;
      break;
		case 2:
			// string of dashes of same length as answer
			len = strlen(answer);
			for(i = 0 ; i < len ; i++)
			{
				strcat(cluestr, ".");
			}
      break;
		case 3:
			// Show first and last letter with string of dashes of same length as answer
			*cluestr = *answer; // set first letter
			*(cluestr + 1) = 0; // terminate string - null char
			len = strlen(answer);  
			for(i = 1 ; i < (len-1) ; i++) // loop from 2nd to 2nd last letter in answer
			{
				strcat(cluestr, ".");
			}

      *temp = *(answer + len - 1); // set last letter
			*(temp + 1) = 0; // terminate string - null char
      strcat(cluestr, temp);
      break;
		case 4:
			// Show two random letters with dashes otherwise
			len = strlen(answer);
			randomNumbers(&rand1, &rand2, len); // get two different random number between 0 and len

      *cluestr = 0;
			for(i = 0 ; i < len ; i++) // loop for each char in answer
			{
        *temp = 0;
				if ((i == rand1) || (i == rand2))
				{
					// copy in Answer character at this position
					*temp = *(answer + i); // set current letter
					*(temp + 1) = 0; // set null char		
				}
				else
				{
					strcpy(temp, ".");
				}
        strcat(cluestr, temp);
			}
      break;
	}
	
	return(cluestr);
	
} // DispClue



// get two different random number between 0 and len
void randomNumbers(int *random1, int *random2, int len) 
{
  srand(time(0));
  *random1 = rand() % len;
  *random2 = rand() % len;

	while(*random1 == *random2)
	{
		*random2 = rand() % len; // if rand2 is the same and rand1 then try again until different
	}
	
}


int stricmp (const char *p1, const char *p2)
{
  register unsigned char *s1 = (unsigned char *) p1;
  register unsigned char *s2 = (unsigned char *) p2;
  unsigned char c1, c2;
 
  do
  {
      c1 = (unsigned char) toupper((int)*s1++);
      c2 = (unsigned char) toupper((int)*s2++);
      if (c1 == '\0')
      {
            return c1 - c2;
      }
  }
  while (c1 == c2);
 
  return c1 - c2;
}
//===============================================================================