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
if (1)  printf("Wire %3d: %3d[%d,%d,%d].net%2d - %3d[%d,%d,%d].net%2d dist=%llu NetNr=%d\n", WireNr,
MinNodeNr, Node[MinNodeNr].Coord[0], Node[MinNodeNr].Coord[1],
Node[MinNodeNr].Coord[2], Node[MinNodeNr].Net,
MinOtherNodeNr, Node[MinOtherNodeNr].Coord[0], Node[MinOtherNodeNr].Coord[1],
Node[MinOtherNodeNr].Coord[2], Node[MinOtherNodeNr].Net, MinDistance, NetNr);

    // Four different scenarios can happen to the Net situation:
    // 1. Two isolated nodes are connected to form a new net
    if ((Node[MinNodeNr].Net == -1) && (Node[MinOtherNodeNr].Net == -1))
    {
/* Debug */
if (1)  printf("   CAT1 new Net%3d = (%3d - %3d)\n", NetNr, MinNodeNr, MinOtherNodeNr);
      Node[MinNodeNr].Net = NetNr;  Node[MinOtherNodeNr].Net = NetNr;
      NetNr++;
    }
    // 2. An isolated node is connected to an existing net, extending it
    else if ((Node[MinNodeNr].Net == -1) && (Node[MinOtherNodeNr].Net > -1))
    {
/* Debug */
if (1)  printf("   CAT2 extend Net%3d from %3d to %3d\n",
Node[MinOtherNodeNr].Net, MinOtherNodeNr, MinNodeNr);
      Node[MinNodeNr].Net = Node[MinOtherNodeNr].Net;
    }
    else if ((Node[MinNodeNr].Net > -1) && (Node[MinOtherNodeNr].Net == -1))
    {
/* Debug */
if (1)  printf("   CAT2 extend Net%3d from %3d to %3d\n",
Node[MinNodeNr].Net, MinNodeNr, MinOtherNodeNr);
      Node[MinOtherNodeNr].Net = Node[MinNodeNr].Net;
    }
    // 3. Two nets are being joined into a single net
    else if (Node[MinNodeNr].Net != Node[MinOtherNodeNr].Net)
    {
/* Debug */
if (1)  printf("   CAT3 Net%3d from %3d goes to %3d to take over Net%3d\n",
Node[MinNodeNr].Net, MinNodeNr, MinOtherNodeNr, Node[MinOtherNodeNr].Net);
      // Convert all nodes belonging to the 2nd Net, to the 1st Net now
      OtherNetNr = Node[MinOtherNodeNr].Net;
      for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
        if (Node[NodeNr].Net == OtherNetNr)  Node[NodeNr].Net = Node[MinNodeNr].Net;
      // Cleanup by dropping all higher Nets one number down
      for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
        if (Node[NodeNr].Net > OtherNetNr)  Node[NodeNr].Net--;
      NetNr--;
    }
    // 4. Two nodes from the same net are connected again, forming a loop : do nothing
    else
/* Debug */
if (1)  printf("   CAT4 Net%3d,%2d now loops (%3d - %3d)\n",
Node[MinNodeNr].Net, Node[MinOtherNodeNr].Net, MinNodeNr, MinOtherNodeNr);

if ((WireNr==384) || (WireNr==385))
 { printf(" +++++++++++++++\n");  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
printf(" Node %3d:Net%3d\n", NodeNr, Node[NodeNr].Net);  printf(" +++++++++++++++\n"); }
  } /* for (WireNr) */

/* Debug */
if (1)  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
printf("Node %3d: [%5d,%5d,%5d]  Net %2d\n", NodeNr, Node[NodeNr].Coord[0],
Node[NodeNr].Coord[1], Node[NodeNr].Coord[2], Node[NodeNr].Net);

  // Now that some Nets have been constructed, tally their size
  NrOfNets = NetNr;
  OtherNetNr = 0;
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (Node[NodeNr].Net == -1)  OtherNetNr++;
  printf("After making %d connections, found %d circuits and %d solo boxes\n",
      NrOfWires, NrOfNets, OtherNetNr);
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (Node[NodeNr].Net > -1)
      NetSize[Node[NodeNr].Net]++;
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
