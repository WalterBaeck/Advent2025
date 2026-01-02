#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************** Helper functions ****************************/

// Compute 10^(Len) ; note that this number will have Len+1 digits
int PowerOfTen(int Len)
{
  int Count, Num=1;
  for (Count=0; Count<Len; Count++)  Num *= 10;
  return Num;
}

// Compute the number obtained by concatenating #Repeats of PartNum 
unsigned long long Replicate(int PartNum, int Repeats)
{
  int Count;
  char ReplicateString[20] = "";
  unsigned long long Num;

  for (Count=0; Count<Repeats; Count++)
    sprintf(ReplicateString+strlen(ReplicateString), "%d", PartNum);
  if (1 != sscanf(ReplicateString, "%llu", &Num))
  {
    fprintf(stderr, "Could not interpret ReplicateString %s\n", ReplicateString);
    exit(5);
  }

  return Num;
}

// Ascertain that a Segment is not itself a repetition of segments
int IsReplicant(int Num)
{
  char FullString[10], PartString[10];
  int FullLength, PartLength, Divider, Count;

  sprintf(FullString, "%d", Num);
  FullLength = strlen(FullString);
  for (PartLength=1; PartLength<=(FullLength/2); PartLength++)
  {
    Divider = FullLength / PartLength;
    if ((Divider*PartLength) != FullLength)  continue;
    strncpy(PartString, FullString, PartLength);
    PartString[PartLength] = '\0';
    for (Count=1; Count<Divider; Count++)
      if (strncmp(PartString, FullString+(Count*PartLength), PartLength))
        break;
    if (Count >= Divider)
    {
/* Debug */
if (0)  printf("Segment %d is a replicant!\n", Num);
      return 1;
    }
  }

  return 0;
}

/****************************** Main program ******************************/

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

  unsigned long long Sum = 0, Sweep = 0;
  unsigned long long Min, Max, Replicant;

  int PartLength, Quotient;
  char PartSegment[11];
  int PartMin, PartMax, Part;

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
    Sweep += (Max - Min + 1);

/* Debug */
if (1)  printf("%21s: from %10llu to %10llu\n", InputPtr, Min, Max);

    // Consider any segmentation of the boundary strings
    for (PartLength=1; PartLength<=(MaxLength/2); PartLength++)
    {
      // Just consider any possibility
      PartMin = PowerOfTen(PartLength - 1);
      PartMax = PowerOfTen(PartLength) - 1;
      for (Quotient=MinLength/PartLength; (Quotient*PartLength)<=MaxLength; Quotient++)
      {
        // Avoid unreal Replicants
        if (Quotient < 2)  continue;
/* Debug */
if (1)
printf("Considering from %llu to %llu\n",Replicate(PartMin, Quotient),Replicate(PartMax, Quotient));
        for (Part=PartMin; Part<=PartMax; Part++)
        {
          // The Part itself should not be a Replicant,
          //   because then it would be counted twice !
          if (IsReplicant(Part))  continue;
          Replicant = Replicate(Part, Quotient);
          if ((Replicant >= Min) && (Replicant <= Max))
          {
/* Debug */
if (1)  printf("-> Adding Replicant %llu to Sum\n", Replicant);
            Sum += Replicant;
          }
        } /* for (Part) */
      } /* for (Quotient) */
    } /* for (PartLength) */

  } /* while (strtok) */
  printf("%llu numbers swept\n", Sweep);

  /****************/
  /* Finalisation */
  /****************/
  printf("Found Sum %llu\n", Sum);
  return 0;
}
