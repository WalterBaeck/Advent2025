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

  char InputSegment[20];
  char* MinusPtr;
  int MinLength, MaxLength;
  unsigned long long Min[200], Max[200], NewMin, NewMax;
  int RangeNr, NrOfRanges=0, StartRange, EndRange;

  unsigned long long Sum = 0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if (!strlen(InputLine))
    {
/* Debug */
if (1)  printf("%d Ranges found after LineNr #%d\n", NrOfRanges, InputLineNr);
      break;
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
    if (1 != sscanf(InputSegment, "%llu", &NewMin))
    {
      fprintf(stderr, "Could not interpret Min from #%d : %s\n", InputLineNr, InputLine);
      exit(2);
    }
    // Get the Max value
    MaxLength = strlen(InputLine) - MinLength - 1;
    strncpy(InputSegment, MinusPtr+1, MaxLength);
    InputSegment[MaxLength] = '\0';
    if (1 != sscanf(InputSegment, "%llu", &NewMax))
    {
      fprintf(stderr, "Could not interpret Max from #%d : %s\n", InputLineNr, InputLine);
      exit(2);
    }

/* Debug */
if (1)  printf("#%d: from %14llu to %14llu\n", InputLineNr, NewMin, NewMax);

    // Now figure out where this new Range belongs in the sorted list of existing Ranges
    for (RangeNr=0; RangeNr<NrOfRanges; RangeNr++)
      if (NewMin <= Max[RangeNr]+1)  break;
    StartRange = RangeNr;
    for (; RangeNr<NrOfRanges; RangeNr++)
      if (NewMax+1 < Min[RangeNr])  break;
    EndRange = RangeNr;

    // Is it a neat insertion in between existing Ranges ?
    if (StartRange == EndRange)
    {
/* Debug */
if (1)  printf(" >> Clean insertion as Range #%d\n", StartRange);
      // Before insertion can happen, push any further Ranges out of the way
      for (RangeNr=NrOfRanges; RangeNr>StartRange; RangeNr--)
      {  Min[RangeNr] = Min[RangeNr-1];   Max[RangeNr] = Max[RangeNr-1];  }
      NrOfRanges++;
      Min[StartRange] = NewMin;  Max[StartRange] = NewMax;
    }
    else /* Concatenating with existing Range(s) */
    {
      // Merge NewRange from StartRange up until EndRange-1
      if (NewMin < Min[StartRange])  Min[StartRange] = NewMin;
      Max[StartRange] = Max[EndRange-1];
      if (NewMax > Max[StartRange])  Max[StartRange] = NewMax;
/* Debug */
if (1)  printf(" ++ Concatenating into StartRange #%d: %llu - %llu\n",
StartRange, Min[StartRange], Max[StartRange]);
      // Delete any Ranges from StartRange+1 to EndRange-1
      int DeleteRanges = EndRange-1 - StartRange;
/* Debug */
if (1)  printf(" -- DeleteRanges = %d from %d up till %d\n",
DeleteRanges, StartRange+1, NrOfRanges-DeleteRanges);
      for (RangeNr=StartRange+1; RangeNr<NrOfRanges-DeleteRanges; RangeNr++)
      {  Min[RangeNr] = Min[RangeNr+DeleteRanges];  Max[RangeNr] = Max[RangeNr+DeleteRanges]; }
      NrOfRanges -= DeleteRanges;
    } /* if (StartRange == EndRange) */
  
/* Debug */
if (1)  for (RangeNr=0; RangeNr<NrOfRanges; RangeNr++)
printf(" Range %3d: %14llu - %14llu\n", RangeNr, Min[RangeNr], Max[RangeNr]);
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  for (RangeNr=0; RangeNr<NrOfRanges; RangeNr++)
    Sum += (Max[RangeNr] - Min[RangeNr] + 1);
  printf("With %d Ranges, found Sum %llu\n", NrOfRanges, Sum);
  return 0;
}
