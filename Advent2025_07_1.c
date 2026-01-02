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
  char InputLine[200];

  int Len=0, X;
  char Status[200];

  int Sum=0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 200, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if (!Len)
    {
      Len = strlen(InputLine);
      strcpy(Status, InputLine);
      for (X=0; X<Len; X++)
        if (Status[X] == 'S')  Status[X] = '|';
      continue;
    }
    if (Len != strlen(InputLine))
    {
      fprintf(stderr, "Len was determined as %d, but InputLineNr #%d has Len %d ?\n",
          Len, InputLineNr, strlen(InputLine));
      exit(2);
    }

    // Process this InputLine
    for (X=0; X<Len; X++)
      if (InputLine[X] == '^')
      {
        if (Status[X] == '|')
        {
          if ((X<1) || (X>Len-2))
          {
            fprintf(stderr, "InputLine #%d splitter too close to boundary at X=%d\n",
                InputLineNr, X);
            exit(2);
          }
          Status[X-1] = '|';
          Status[X]   = '.';
          Status[X+1] = '|';
          Sum++;
        }
      }
      else if (InputLine[X] != '.')
      {
        fprintf(stderr, "Unknown char %c encountered at Y=%d,X=%d\n",
            InputLine[X], InputLineNr, X);
        exit(2);
      }
  } /* while (fgets) */
  printf("Read %d lines of %d chars\n", InputLineNr, Len);

  /****************/
  /* Finalisation */
  /****************/
  printf("Found %d active splits\n", Sum);
  return 0;
}
