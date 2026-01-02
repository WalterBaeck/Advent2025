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
  char InputLine[100];
  int InputLineNr = 0;

  int ParsingRanges = 1;
  char InputSegment[20];
  char* MinusPtr;
  int MinLength, MaxLength;
  unsigned long long Min[200], Max[200];
  int RangeNr=0, NrOfRanges;

  unsigned long long Num;
  int Sum = 0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if (ParsingRanges)
    {
      if (!strlen(InputLine))
      {
        ParsingRanges = 0;
        NrOfRanges = RangeNr;
/* Debug */
if (1)  printf("%d Ranges found after LineNr #%d\n", NrOfRanges, InputLineNr);
        continue;
      }
      // Parse this range
      if (!(MinusPtr = strchr(InputLine, '-')))
      {
        fprintf(stderr, "Could not find minus sign from #%d : %s\n", InputLineNr, InputLine);
        exit(2);
      }
      // Get the Min value
      MinLength = MinusPtr - InputLine;
      strncpy(InputSegment, InputLine, MinLength);
      InputSegment[MinLength] = '\0';
      if (1 != sscanf(InputSegment, "%llu", &Min[RangeNr]))
      {
        fprintf(stderr, "Could not interpret Min from #%d : %s\n", InputLineNr, InputLine);
        exit(2);
      }
      // Get the Max value
      MaxLength = strlen(InputLine) - MinLength - 1;
      strncpy(InputSegment, MinusPtr+1, MaxLength);
      InputSegment[MaxLength] = '\0';
      if (1 != sscanf(InputSegment, "%llu", &Max[RangeNr]))
      {
        fprintf(stderr, "Could not interpret Max from #%d : %s\n", InputLineNr, InputLine);
        exit(2);
      }

/* Debug */
if (1)  printf("#%d: from %14llu to %14llu\n", InputLineNr, Min[RangeNr], Max[RangeNr]);

      RangeNr++;
    }
    else /* !ParsingRanges */
    {
      if (1 != sscanf(InputLine, "%llu", &Num))
      {
        fprintf(stderr, "Could not interpret Num from #%d : %s\n", InputLineNr, InputLine);
        exit(2);
      }
      for (RangeNr=0; RangeNr<NrOfRanges; RangeNr++)
        if ((Num >= Min[RangeNr]) && (Num <= Max[RangeNr]))
        {
          Sum++;
          break;
        }
    } /* if (ParsingRanges) */
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
