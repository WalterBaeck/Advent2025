#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/********************/
/* Helper functions */
/********************/

// Compute 10^(Len) ; note that this number will have Len+1 digits
int PowerOfTen(int Len)
{
  int Count, Num=1;
  for (Count=0; Count<Len; Count++)  Num *= 10;
  return Num;
}

// Compute the number obtained by repeating HalfNum twice
unsigned long long Duplicate(int HalfNum)
{
  char DuplicateString[20];
  unsigned long long Num;

  sprintf(DuplicateString, "%d%d", HalfNum, HalfNum);
  if (1 != sscanf(DuplicateString, "%llu", &Num))
  {
    fprintf(stderr, "Could not interpret DuplicateString %s\n", DuplicateString);
    exit(5);
  }

  return Num;
}

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
  char InputLine[1000];
  char* InputPtr = NULL;

  char InputSegment[20];
  char* MinusPtr;
  int MinLength, MaxLength;

  unsigned long long Sum = 0;
  unsigned long long Min, Max;

  char HalfSegment[11];
  int HalfMin, HalfMax, Half;

  /*************/
  /* Operation */
  /*************/
  if (!fgets(InputLine, 1000, InputFile))
  {
    fprintf(stderr, "Could not read InputLine.\n");
    exit(2);
  }
  while ((InputLine[strlen(InputLine) - 1] == '\r')
      || (InputLine[strlen(InputLine) - 1] == '\n'))
    InputLine[strlen(InputLine) - 1] = '\0';
  while (InputPtr = strtok(InputPtr ? NULL : InputLine, ","))
  {
    // Parse this range
    if (!(MinusPtr = strchr(InputPtr, '-')))
    {
      fprintf(stderr, "Could not find minus sign from pos %d\n", InputPtr - InputLine);
      exit(2);
    }
    // Get the Min value
    MinLength = MinusPtr - InputPtr;
    strncpy(InputSegment, InputPtr, MinLength);
    InputSegment[MinLength] = '\0';
    if (1 != sscanf(InputSegment, "%llu", &Min))
    {
      fprintf(stderr, "Could not interpret Min from pos %d : %s\n",
          InputPtr - InputLine, InputSegment);
      exit(2);
    }
    // Get the Max value
    MaxLength = strlen(InputPtr) - MinLength - 1;
    strncpy(InputSegment, MinusPtr+1, MaxLength);
    InputSegment[MaxLength] = '\0';
    if (1 != sscanf(InputSegment, "%llu", &Max))
    {
      fprintf(stderr, "Could not interpret Max from pos %d : %s\n",
          InputPtr - InputLine, InputSegment);
      exit(2);
    }

/* Debug */
if (1)
  printf("%21s: from %10llu to %10llu\n", InputPtr, Min, Max);

    // Depending on which boundary has even length
    if ((MinLength & 1) && (MaxLength & 1))
    {
      // Both odd
      if (MinLength != MaxLength)
      {
        fprintf(stderr, "Cannot handle different odd lengths from pos %d\n",
            InputPtr - InputLine);
        exit(3);
      }
      // No hits possible with both boundaries at odd length
      continue;
    }
    if ((!(MinLength & 1)) && (!(MaxLength & 1)) && (MinLength != MaxLength))
    {
      fprintf(stderr, "Cannot handle different even lengths from pos %d\n",
          InputPtr - InputLine);
      exit(3);
    }

    // Find the range of Half numbers to consider
    if (MinLength & 1)
      HalfMin = PowerOfTen(MinLength / 2);
    else
    {
      strncpy(HalfSegment, InputPtr, MinLength/2);
      HalfSegment[MinLength/2] = '\0';
      if (1 != sscanf(HalfSegment, "%d", &HalfMin))
      {
        fprintf(stderr, "Could not scan HalfMin from %s\n", HalfSegment);
        exit(4);
      }
    }
    if (Duplicate(HalfMin) < Min)  HalfMin++;
    if (MaxLength & 1)
      HalfMax = PowerOfTen(MaxLength/2) - 1;
    else
    {
      strncpy(HalfSegment, MinusPtr+1, MaxLength/2);
      HalfSegment[MaxLength/2] = '\0';
      if (1 != sscanf(HalfSegment, "%d", &HalfMax))
      {
        fprintf(stderr, "Could not scan HalfMax from %s\n", HalfSegment);
        exit(4);
      }
    }
    if (Duplicate(HalfMax) > Max)  HalfMax--;
      
/* Debug */
if (1)
  printf("Considering from %d%d to %d%d\n", HalfMin, HalfMin, HalfMax, HalfMax);

    // Finally, add the Duplicate of all the feasible Half numbers in this range
    for (Half=HalfMin; Half<=HalfMax; Half++)
      Sum += Duplicate(Half);

  } /* while (strtok) */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found Sum %llu\n", Sum);
  return 0;
}
