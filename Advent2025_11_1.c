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
  char* InputPtr;

  int NodeNr, OtherNodeNr, NrOfNodes=0;
  char Name[1000][4], NewName[4];
  int To[1000][30];
  int ToNr, NrOfTo[1000] = {0};

  int Pos, Choice[1000];
  int Path[1000];
  int NrOfPaths = 0;
  int Visited[1000] = {0};

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 200, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    if (InputLine[3] != ':')
    {
      fprintf(stderr, "InputLine #%d has %c instead of colon at [3]\n",
          InputLineNr, InputLine[3]);
      exit(2);
    }
    strncpy(NewName, InputLine, 3);
    NewName[3] = '\0';
    for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
      if (!strcmp(NewName, Name[NodeNr]))  break;
    if (NodeNr >= NrOfNodes)
      strcpy(Name[NrOfNodes++], NewName);
    // Populate this Node with connections
    for (ToNr=0; InputPtr = strtok(ToNr ? NULL : InputLine+5, " "); ToNr++)
    {
      for (OtherNodeNr=0; OtherNodeNr<NrOfNodes; OtherNodeNr++)
        if (!strcmp(InputPtr, Name[OtherNodeNr]))  break;
      if (OtherNodeNr >= NrOfNodes)
        strcpy(Name[NrOfNodes++], InputPtr);
      To[NodeNr][ToNr] = OtherNodeNr;
    }
    NrOfTo[NodeNr] = ToNr;
  } /* while (fgets) */
  printf("Found %d Nodes in %d InputLines\n", NrOfNodes, InputLineNr);

  // Traverse the whole tree to find all the connections to 'out'
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (!strcmp("you", Name[NodeNr]))  break;
  Pos = 0;  Path[Pos] = NodeNr;  Visited[NodeNr] = 1;  Choice[Pos] = -1;
  for(;;)
  {
    // Always try to pick the next possible output from here
    if (++Choice[Pos] < NrOfTo[Path[Pos]])
    {
      int NodeNr = To[Path[Pos]][Choice[Pos]];
      // But beware from loops !
      if (Visited[NodeNr])  continue;
      // Feasible new step : perhaps this is the exit ?
      if (!strcmp(Name[NodeNr], "out"))
      { NrOfPaths++;  continue; }   // Even so, keep on searching the tree
      // Otherwise, take this new step along the path
      Path[++Pos] = NodeNr;
      Visited[NodeNr] = 1;  
      Choice[Pos] = -1;
    }
    else
    {
      // All choices at current Pos exhausted - need to backtrack
      Visited[Path[Pos]] = 0;
      if (--Pos < 0)  break;
    }
  } /* forever */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found %d Paths\n", NrOfPaths);
  return 0;
}
