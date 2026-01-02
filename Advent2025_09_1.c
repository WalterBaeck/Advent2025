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
  int InputField;
  char* InputPtr;
  int Num;

  int X[1000],Y[1000];
  int NodeNr = 0, OtherNodeNr, NrOfNodes;

  int DeltaCoord;
  unsigned long long Distance, MaxDistance=0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    for (InputField=0; InputPtr = strtok(InputField ? NULL : InputLine, ","); InputField++)
    {
      if (1 != sscanf(InputPtr, "%d", &Num))
      {
        fprintf(stderr, "Could not interpret %s as Coord[#%d] at InputLine #%d\n",
            InputPtr, InputField, InputLineNr);
        exit(2);
      }
      switch (InputField)
      {
        case 0: X[NodeNr] = Num;  break;
        case 1: Y[NodeNr] = Num;  break;
        default: 
          fprintf(stderr, "More than 2 numbers on InputLine #%d\n", InputLineNr);
          exit(2);
      }
    } /* for (strtok) */
    if (InputField != 2)
    {
      fprintf(stderr, "No 2 numbers on InputLine #%d\n", InputLineNr);
      exit(2);
    }
    NodeNr++;
  } /* while (fgets) */
  NrOfNodes = NodeNr;

  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
    {
      Distance = 1;
      for (InputField=0; InputField<2; InputField++)
      {
        switch (InputField)
        {
          case 0: DeltaCoord = X[NodeNr] - X[OtherNodeNr];  break;
          case 1: DeltaCoord = Y[NodeNr] - Y[OtherNodeNr];  break;
        }
        if (DeltaCoord < 0)  DeltaCoord = - DeltaCoord;
        DeltaCoord++;
        Distance *= (unsigned long long)DeltaCoord;
      }
      if (Distance > MaxDistance)  MaxDistance = Distance;
    }

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found MaxDistance %llu\n", InputLineNr, MaxDistance);
  return 0;
}
