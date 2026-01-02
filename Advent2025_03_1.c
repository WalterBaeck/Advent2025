#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  int Len, FirstPos, SecondPos, First, Second, Num, MaxNum, Sum=0;

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
    MaxNum = 0;
    for (FirstPos=0; FirstPos<Len-1; FirstPos++)
    {
      First = InputLine[FirstPos] - '0';
      for (SecondPos=FirstPos+1; SecondPos<Len; SecondPos++)
      {
        Second = InputLine[SecondPos] - '0';
        Num = 10*First + Second;
        if (Num > MaxNum)
          MaxNum = Num;
      }
    }
    Sum += MaxNum;

/* Debug */
if (1)  printf("%3d: adding %d so Sum = %d\n", InputLineNr, MaxNum, Sum);
  }

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
