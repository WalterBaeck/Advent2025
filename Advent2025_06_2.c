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

  char* InputLine[5];
  int X, Y=0, XSize, YSize;

  for (Y=0; Y<5; Y++)
    if (!(InputLine[Y] = (char*)malloc(10000 * sizeof(char))))
    {
      fprintf(stderr, "Could not malloc InputLine[%d] for 10000 chars\n", Y);
      exit(3);
    }

  int StartX, EndX;
  char Operator;
  int Operand;
  unsigned long long Result, Sum = 0;

  /*************/
  /* Operation */
  /*************/
  for (Y=0; fgets(InputLine[Y], 10000, InputFile); Y++)
  {
    while ((InputLine[Y][strlen(InputLine[Y]) - 1] == '\r')
        || (InputLine[Y][strlen(InputLine[Y]) - 1] == '\n'))
      InputLine[Y][strlen(InputLine[Y]) - 1] = '\0';
    if (!Y)  XSize = strlen(InputLine[Y]);
  }
  YSize = Y;

/* Debug */
if (1)  printf("Found %d lines of %d chars\n", YSize, XSize);

  // Determine column widths by operators in last line
  for (StartX=0; StartX<XSize; StartX=EndX)
  {
    // Determine operator
    Operator = InputLine[YSize-1][StartX];
    switch (Operator)
    {
      case '+':  Result = 0;  break;
      case '*':  Result = 1;  break;
      default:
        fprintf(stderr, "Unknown operator %c at Y=%d,X=%d\n", Operator, YSize, X);
        exit(2);
    }
    // Determine column boundaries
    for (X=StartX+1; X<XSize; X++)
      if (InputLine[YSize-1][X] != ' ')  break;
    EndX = X;
    // Imagine a terminating column of spaces
    if (X == XSize)  EndX++;

/* Debug */
if (1)  printf("Found StartX=%d EndX=%d with Operator %c\n", StartX, EndX, Operator);

    // Determine operands
    for (X=EndX-2; X>=StartX; X--)
    {
      Operand = 0;
      for (Y=0; Y<YSize-1; Y++)
        if (InputLine[Y][X] == ' ')  continue;
        else Operand = 10*Operand + (InputLine[Y][X] - '0');
      switch (Operator)
      {
        case '+':  Result += Operand;  break;
        case '*':  Result *= Operand;  break;
      }

/* Debug */
if (1)  printf(" .. processing Operand %d to arrive at Result %llu\n", Operand, Result);

    } /* for (X) */

    Sum += Result;
  } /* for (StartX) */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found grand total %llu\n", Sum);
  return 0;
}
