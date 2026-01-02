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
  int Len=0, X, Y;
  int **Grid = NULL;

  int DiffX, DiffY, Neighbors, Sum=0;
  int Turn, Score;

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
      Len = strlen(InputLine);
    else if (Len != strlen(InputLine))
    {
      fprintf(stderr, "Len was determined as %d, but InputLineNr #%d has Len %d ?\n",
          Len, InputLineNr, strlen(InputLine));
      exit(2);
    }
    if (!(Grid = (int**)realloc(Grid, InputLineNr * sizeof(int*))))
    {
      fprintf(stderr, "Could not realloc Grid to %d Lines\n", InputLineNr);
      exit(3);
    }
    Y = InputLineNr - 1;
    if (!(Grid[Y] = (int*)malloc(Len * sizeof(int))))
    {
      fprintf(stderr, "Could not malloc Grid[%d] for %d integers\n", Y, Len);
      exit(3);
    }
    for (X=0; X<Len; X++)
      if      (InputLine[X] == '@')  Grid[Y][X] = 1;
      else if (InputLine[X] == '.')  Grid[Y][X] = 0;
      else
      {
        fprintf(stderr, "Unknown character %c at [X=%d,Y=%d]\n", InputLine[X], X, Y);
        exit(4);
      }
  } /* while (fgets) */
  printf("Read %d lines of %d chars\n", InputLineNr, Len);

  // Keep removing rolls, turn per turn
  for (Turn=1;; Turn++)
  {
    Score = 0;

    // Go over the whole Grid and consider every roll
    for (Y=0; Y<InputLineNr; Y++)
      for (X=0; X<Len; X++)
      {
        if (!Grid[Y][X])  continue;
        // Count the Neigbors for this roll
        Neighbors = 0;
        for (DiffY=-1; DiffY<=1; DiffY++)
          for (DiffX=-1; DiffX<=1; DiffX++)
          {
            if ((!DiffY) && (!DiffX))  continue;
            if ((Y+DiffY < 0) || (Y+DiffY >= InputLineNr))  continue;
            if ((X+DiffX < 0) || (X+DiffX >= Len))  continue;
            if (Grid[Y+DiffY][X+DiffX])  Neighbors++;
          }
        // Suitable amount of Neighbors ?
        if (Neighbors < 4)
        {
          Grid[Y][X] = 0;
          Score++;
        }
      } /* for (X) */

    // Turn is complete now
    if (!Score)  break;
    Sum += Score;
  } /* for (Turn) */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found %d removable rolls in %d turns\n", Sum, Turn);
  return 0;
}
