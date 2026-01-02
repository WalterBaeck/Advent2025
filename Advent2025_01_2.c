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
  char InputLine[100];

  unsigned int Pos = 50;
  int Sign, Distance;
  int NrOfZeroCrossings = 0;
  int MaxDistance = 0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if (*InputLine == 'L')       Sign = -1;
    else if (*InputLine == 'R')  Sign =  1;
    else
    {
      fprintf(stderr, "Could not recognize Sign %c at InputLine #%d:\n%s\n",
          *InputLine, InputLineNr, InputLine);
      exit(2);
    }
    if (1 != sscanf(InputLine+1, "%d", &Distance))
    {
      fprintf(stderr, "Could not interpret %s as a Distance at InputLine #%d\n",
          InputLine+1, InputLineNr);
      exit(2);
    }

    // Bookkeeping
    if (Distance > MaxDistance)  MaxDistance = Distance;

    // Perform the turn
    unsigned int OldPos = Pos;
    Pos += (1000 + Sign*Distance);
    Pos %= 100;

    // Any full rotation adds a zero crossing
    NrOfZeroCrossings += (Distance / 100);
    // Net displacement different than Sign ? Must have crossed zero.
    // Exception : when starting from OldPos==0, this should not be counted
    if (((Pos > OldPos) ^ (Sign == 1)) && (OldPos))
      NrOfZeroCrossings++;
    // Turning back to exactly zero has not been counted yet
    if ((Sign == -1) && (!Pos))
      NrOfZeroCrossings++;

/* Debug */
if (1)
  printf("%4d: %4s turn %d to Pos %u with NrZeroCross %d\n", InputLineNr, InputLine, Sign*Distance, Pos, NrOfZeroCrossings);
  }
  printf("Ended at Pos=%d with MaxDistance=%d\n", Pos, MaxDistance);

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found %d ZeroCrossings\n", InputLineNr, NrOfZeroCrossings);
  return 0;
}
