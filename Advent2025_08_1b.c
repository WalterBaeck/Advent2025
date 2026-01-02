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
  char *InputPtr;
  int InputField, Num;

  typedef struct {
    int Coord[3];
    unsigned long long DistanceTo[1000];
    int Net;
  } tNode;
  tNode* Node;
  if (!(Node = (tNode*)malloc(1000 * sizeof(tNode))))
  {
    fprintf(stderr, "Could not allocate 1000 Nodes\n");
    exit(3);
  }

  int NodeNr=0, OtherNodeNr, NrOfNodes;
  int WireNr, NrOfWires;
  int NetNr=0, OtherNetNr, NrOfNets;
  int NetSize[1000] = {0};

  int Product = 1;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    for (InputField=0; InputPtr=strtok(InputField ? NULL : InputLine, ","); InputField++)
    {
      if (1 != sscanf(InputPtr, "%d", &Num))
      {
        fprintf(stderr, "Could not scan Num #%d from InputLine #%d in %s\n",
            InputField, InputLineNr, InputPtr);
        exit(2);
      }
      if (InputField >= 3)
      {
        fprintf(stderr, "More than 3 fields in InputLine #%d ?\n", InputLineNr);
        exit(2);
      }
      Node[NodeNr].Coord[InputField] = Num;
    } /* for (strtok) */
    if (InputField < 3)
    {
      fprintf(stderr, "Less than 3 fields in InputLine #%d ?\n", InputLineNr);
      exit(2);
    }
    NodeNr++;
  } /* while (fgets) */
  NrOfNodes = NodeNr;
  printf("Read %d Nodes from %d InputLines\n", NrOfNodes, InputLineNr);

  // Now that the coordinates of all Nodes are known, determine all distances
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    Node[NodeNr].Net = -1;
    for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
    {
      Node[NodeNr].DistanceTo[OtherNodeNr] = 0;
      for (InputField=0; InputField<3; InputField++)
      {
        int Distance = Node[NodeNr].Coord[InputField] - Node[OtherNodeNr].Coord[InputField];
        if (Distance < 0)  Distance = -Distance;
        Node[NodeNr].DistanceTo[OtherNodeNr] += ((unsigned long long)Distance * Distance);
      }
    }
  } /* for (NodeNr) */

  // With all distances (squared) computed, now make connections
  // Connections always run from a lower-numbered node to a higher-numbered
  NrOfWires = (NrOfNodes < 100) ? 10 : 1000;
  for (WireNr=0; WireNr<NrOfWires; WireNr++)
  {
    // Find the shortest unused distance
    unsigned long long MinDistance = 0;
    int MinNodeNr, MinOtherNodeNr;
    for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
      for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
        if (!Node[NodeNr].DistanceTo[OtherNodeNr])  continue;  // Already connected
        else if ((!MinDistance) || (Node[NodeNr].DistanceTo[OtherNodeNr] < MinDistance))
        {
          MinDistance = Node[NodeNr].DistanceTo[OtherNodeNr];
          MinNodeNr = NodeNr;
          MinOtherNodeNr = OtherNodeNr;
        }
    // Now make the connection.
    Node[MinNodeNr].DistanceTo[MinOtherNodeNr] = 0;

/* Debug */
if (1)  printf("Wire %3d: %3d[%d,%d,%d] - %3d[%d,%d,%d] dist=%llu\n", WireNr,
MinNodeNr, Node[MinNodeNr].Coord[0], Node[MinNodeNr].Coord[1],
Node[MinNodeNr].Coord[2],
MinOtherNodeNr, Node[MinOtherNodeNr].Coord[0], Node[MinOtherNodeNr].Coord[1],
Node[MinOtherNodeNr].Coord[2], MinDistance);

  } /* for (WireNr) */

  // Once all the connections have been made, link them into nets
  for (;;)
  {
    // Looking for the first node of a new Net
    for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
      if (Node[NodeNr].Net > -1)  continue;
      else
      {
        for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
          if (!Node[NodeNr].DistanceTo[OtherNodeNr])
          {
            Node[NodeNr].Net = NetNr;  Node[OtherNodeNr].Net = NetNr;
          }
        // Found anything ?
        if (Node[NodeNr].Net > -1)  break;
      }
    if (NodeNr >= NrOfNodes)  break; /* from outer forever loop */
    // As a new Net was successfully started, keep trying to mark new Nodes with it
    for (;;)
    {
      int AnyNodeFound = 0;
      for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
        if (Node[NodeNr].Net == -1)
        {
          for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
            if ((!Node[NodeNr].DistanceTo[OtherNodeNr]) && (Node[OtherNodeNr].Net == NetNr))
            {
              AnyNodeFound = 1;
              Node[NodeNr].Net = NetNr;
            }
        }
        else if (Node[NodeNr].Net == NetNr)
        {
          for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
            if ((!Node[NodeNr].DistanceTo[OtherNodeNr]) && (Node[OtherNodeNr].Net == -1))
            {
              AnyNodeFound = 1;
              Node[OtherNodeNr].Net = NetNr;
            }
        }
      if (!AnyNodeFound)  break; /* from inner forever loop */
    } /* forever */
    // This new Net has been fully marked now
    NetNr++;
  } /* forever */

/* Debug */
if (1)  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
printf("Node %3d: [%5d,%5d,%5d]  Net %2d\n", NodeNr, Node[NodeNr].Coord[0],
Node[NodeNr].Coord[1], Node[NodeNr].Coord[2], Node[NodeNr].Net);

  // Now that some Nets have been constructed, tally their size
  NrOfNets = NetNr;
  OtherNetNr = 0;
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (Node[NodeNr].Net == -1)  OtherNetNr++;
    else  NetSize[Node[NodeNr].Net]++;
  printf("After making %d connections, found %d circuits and %d solo boxes\n",
      NrOfWires, NrOfNets, OtherNetNr);
  // Then look for the 3 biggest ones
  for (InputField=0; InputField<3; InputField++)
  {
    int Max = 0, MaxNet;
    for (NetNr=0; NetNr<NrOfNets; NetNr++)
      if (NetSize[NetNr] > Max)
      {
        Max = NetSize[NetNr];
        MaxNet = NetNr;
      }
    printf(" .. found MaxNetSize[%d] = %d (from Net #%d)\n", InputField+1, Max, MaxNet);
    Product *= Max;
    NetSize[MaxNet] = 0;
  } /* for (InputField) */

  /****************/
  /* Finalisation */
  /****************/
  printf("Found Product %d\n", Product);
  return 0;
}
