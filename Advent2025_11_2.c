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

  int DacNode, FftNode;

  int Kind;
  long long PathsToExit[1000][4];

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

  // Find specialty nodes
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    if (!strcmp("dac", Name[NodeNr]))  DacNode = NodeNr;
    if (!strcmp("fft", Name[NodeNr]))  FftNode = NodeNr;
    for (Kind=0; Kind<4; Kind++)  PathsToExit[NodeNr][Kind] = -1;
  }

  // Starting point
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (!strcmp("svr", Name[NodeNr]))  break;

  // Recursive function
  void GetPathsToExit(int ThisNodeNr)
  {
    if (PathsToExit[ThisNodeNr][0] >= 0)
    {
/* Debug */
if (1)  { printf("PathsToExit[%s] known:", Name[ThisNodeNr]);
for (Kind=0; Kind<4; Kind++)  printf(" %lld", PathsToExit[ThisNodeNr][Kind]);
putchar('\n'); }
      return;
    }
    // Not known yet ? Need to compute
    memset(PathsToExit[ThisNodeNr], 0, 4 * sizeof(long long));
    // Perhaps this IS the exit
    if (!strcmp(Name[ThisNodeNr], "out"))
    {
      PathsToExit[ThisNodeNr][0] = 1;
/* Debug */
if (1)  { printf("PathsToExit[%s] set:", Name[ThisNodeNr]);
for (Kind=0; Kind<4; Kind++)  printf(" %lld", PathsToExit[ThisNodeNr][Kind]);
putchar('\n'); }
      return;
    }
/* Debug */
if (1)  printf("Computing over %d connections from %s\n", NrOfTo[ThisNodeNr], Name[ThisNodeNr]);
    for (int HereTo=0; HereTo<NrOfTo[ThisNodeNr]; HereTo++)
    {
      int NextNodeNr = To[ThisNodeNr][HereTo];
/* Debug */
if (1)  { printf("Invoking recursion to %s:", Name[NextNodeNr]);
for (Kind=0; Kind<4; Kind++)  printf(" %lld", PathsToExit[NextNodeNr][Kind]);
putchar('\n'); }
      GetPathsToExit(NextNodeNr);
      if (ThisNodeNr == DacNode)
      {
        PathsToExit[ThisNodeNr][0] = 0;
        PathsToExit[ThisNodeNr][1] += PathsToExit[NextNodeNr][0];
        PathsToExit[ThisNodeNr][2] = 0;
        PathsToExit[ThisNodeNr][3] += PathsToExit[NextNodeNr][2];
      }
      else if (ThisNodeNr == FftNode)
      {
        PathsToExit[ThisNodeNr][0] = 0;
        PathsToExit[ThisNodeNr][1] = 0;
        PathsToExit[ThisNodeNr][2] += PathsToExit[NextNodeNr][0];
        PathsToExit[ThisNodeNr][3] += PathsToExit[NextNodeNr][1];
      }
      else
        for (Kind=0; Kind<4; Kind++)
          PathsToExit[ThisNodeNr][Kind] += PathsToExit[NextNodeNr][Kind];
    } /* for (HereTo) */
/* Debug */
if (1)  { printf("PathsToExit[%s] set:", Name[ThisNodeNr]);
for (Kind=0; Kind<4; Kind++)  printf(" %lld", PathsToExit[ThisNodeNr][Kind]);
putchar('\n'); }
  } /* void GetPathsToExit(int ThisNodeNr) */

  // Call the recursive function from the top of the tree
  GetPathsToExit(NodeNr);

/* Debug */
if (1) { printf("PathsToExit from Node #%d: [", NodeNr);
for (Kind=0; Kind<4; Kind++)  printf(" %lld", PathsToExit[NodeNr][Kind]);
printf(" ]\n"); }

  /****************/
  /* Finalisation */
  /****************/
  printf("Found %lld Paths\n", PathsToExit[NodeNr][3]);
  return 0;
}
