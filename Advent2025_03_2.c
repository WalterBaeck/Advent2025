#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Variations.h"

int main(int Arguments, char* Argument[])
{
  /******************/
  /* Initialisation */
  /******************/
  FILE* InputFile = stdin;
  if (Arguments >= 2)
    if (!(InputFile = fopen(Argument[1], "r")))
    {
      fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
      exit(1);
    }
  int InputLineNr = 0;
  char InputLine[110];

  int Len, Pos, Digit[100];
  int MaxFound, Index, Choice[12];
  unsigned long long Num, MaxNum, Sum;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 110, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    Len = strlen(InputLine);
    for (Pos=0; Pos<Len; Pos++)
      Digit[Pos] = InputLine[Pos] - '0';

    // For every of the 12 digits that needs to be picked
    for (Index=0; Index<12; Index++)
    {
      MaxFound = -1;
      for (Pos=(Index ? Choice[Index-1]+1 : 0); Pos<Len-11+Index; Pos++)
        if (Digit[Pos] > MaxFound)
        {
          MaxFound = Digit[Pos];
          Choice[Index] = Pos;
        }
    } /* for (Index) */

    // Compose the number that was constructed
    MaxNum = 0;
    for (Index=0; Index<12; Index++)
    {
      MaxNum *= 10;
      MaxNum += Digit[Choice[Index]];
    }
    Sum += MaxNum;

/* Debug */
if (1)  printf("%3d: adding %llu so Sum = %llu\n", InputLineNr, MaxNum, Sum);
  }

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %llu\n", InputLineNr, Sum);
  return 0;
}
