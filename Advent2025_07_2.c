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
  unsigned long long Paths[200], NewPaths[200];
  unsigned long long Sum=0;

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
      memset(Paths, 0, Len * sizeof(unsigned long long));
      for (X=0; X<Len; X++)
        if (InputLine[X] == 'S')  Paths[X] = 1;
      continue;
    }
    if (Len != strlen(InputLine))
    {
      fprintf(stderr, "Len was determined as %d, but InputLineNr #%d has Len %d ?\n",
          Len, InputLineNr, strlen(InputLine));
      exit(2);
    }

    // Process this InputLine
    memcpy(NewPaths, Paths, Len * sizeof(unsigned long long));
    for (X=0; X<Len; X++)
      if (InputLine[X] == '^')
      {
        if ((X<1) || (X>Len-2))
        {
          fprintf(stderr, "InputLine #%d splitter too close to boundary at X=%d\n",
              InputLineNr, X);
          exit(2);
        }
        NewPaths[X-1] += Paths[X];
        NewPaths[X]    = 0;
        NewPaths[X+1] += Paths[X];
      }
      else if (InputLine[X] != '.')
      {
        fprintf(stderr, "Unknown char %c encountered at Y=%d,X=%d\n",
            InputLine[X], InputLineNr, X);
        exit(2);
      }
    memcpy(Paths, NewPaths, Len * sizeof(unsigned long long));
  } /* while (fgets) */
  printf("Read %d lines of %d chars\n", InputLineNr, Len);

  /****************/
  /* Finalisation */
  /****************/
  for (X=0; X<Len; X++)  Sum += Paths[X];
  printf("Found %llu possible timelines\n", Sum);
  return 0;
}
