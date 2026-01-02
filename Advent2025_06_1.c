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
  char InputLine[10000];
  char* InputPtr;

  int X, Y, XSize, YSize;
  int **Operand = NULL;
  char* Operator = NULL;
  int ParsingOperands = 1;

  unsigned long long Result, Sum = 0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 10000, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if ((*InputLine == '*') || (*InputLine == '+'))
    {
      if (!(Operator = (char*)malloc(XSize * sizeof(char))))
      {
        fprintf(stderr, "Could not malloc Operator for %d chars\n", XSize);
        exit(3);
      }
      ParsingOperands = 0;
    }
    if (!(Operand = (int**)realloc(Operand, InputLineNr * sizeof(int*))))
    {
      fprintf(stderr, "Could not realloc Operand to %d Lines\n", InputLineNr);
      exit(3);
    }
    X = 0;
    Y = InputLineNr - 1;
    if (!Y)
      Operand[Y] = NULL;
    else if (!(Operand[Y] = (int*)malloc(XSize * sizeof(int))))
    {
      fprintf(stderr, "Could not malloc Operand[%d] for %d integers\n", Y, XSize);
      exit(3);
    }
    while (InputPtr = strtok(X ? NULL : InputLine, " "))
    {
      if (Y)
      {
        if (X >= XSize)
        {
          fprintf(stderr, "XSize was determined as %d, but InputLineNr #%d has more ?\n",
              XSize, InputLineNr);
          exit(2);
        }
      }
      else
      {
        if (!(Operand[Y] = (int*)realloc(Operand[Y], (X+1) * sizeof(int))))
        {
          fprintf(stderr, "Could not malloc Operand[%d] for %d integers\n", Y, X+1);
          exit(3);
        }
      }
      if (ParsingOperands)
      {
        if (1 != sscanf(InputPtr, "%d", &(Operand[Y][X])))
        {
          fprintf(stderr, "Could not parse operand[Y=%d,X=%d] in %s\n", Y, X, InputPtr);
          exit(2);
        }
      }
      else
        Operator[X] = *InputPtr;
      X++;
    } /* while (strtok) */
    if (!Y)
      XSize = X;
    else if (X < XSize)
    {
      fprintf(stderr, "XSize was determined as %d, but InputLineNr #%d has less ?\n",
          XSize, InputLineNr);
      exit(2);
    }
  } /* while (fgets) */
  printf("Read %d lines with %d items each\n", InputLineNr, XSize);

  // Perform the operation in every column
  for (X=0; X<XSize; X++)
  {
    switch (Operator[X])
    {
      case '+':
        Result = 0;
        for (Y=0; Y<InputLineNr-1; Y++)  Result += Operand[Y][X];
        break;
      case '*':
        Result = 1;
        for (Y=0; Y<InputLineNr-1; Y++)  Result *= Operand[Y][X];
        break;
      default:
        fprintf(stderr, "Unknown operator %c at X=%d\n", Operator[X], X);
        exit(2);
    }
    Sum += Result;
  } /* for (X) */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found grand total %llu\n", Sum);
  return 0;
}
