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

  int NextX, NextY, OtherNextX, OtherNextY;
  int GridX[1000], GridY[1000];
  int GridIndex;
  int GridXNr, NrOfGridX = 0, GridYNr, NrOfGridY = 0;
  int NodeGridXNr[1000], NodeGridYNr[1000];

  typedef enum {eNorth, eEast, eSouth, eWest} tDir;
  tDir Dir[1000];

  char **Grid, ClockWise;

  int DeltaCoord;
  unsigned long long Area, MaxArea=0;

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
  printf("After %d InputLines, found %d Nodes\n", InputLineNr, NrOfNodes);

  // Divide the outer bounding box of the whole loop into a Grid of rectangles
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    // The accompanying line segment of the loop heads out to NodeNr+1
    if (NodeNr == NrOfNodes-1)  { NextX = X[0];  NextY = Y[0]; }
    else  { NextX = X[NodeNr+1];  NextY = Y[NodeNr+1]; }
    if      ((NextX == X[NodeNr]) && (NextY > Y[NodeNr]))  Dir[NodeNr] = eSouth;
    else if ((NextX < X[NodeNr]) && (NextY == Y[NodeNr]))  Dir[NodeNr] = eWest;
    else if ((NextX == X[NodeNr]) && (NextY < Y[NodeNr]))  Dir[NodeNr] = eNorth;
    else if ((NextX > X[NodeNr]) && (NextY == Y[NodeNr]))  Dir[NodeNr] = eEast;
    else
    {
      fprintf(stderr, "Unclear direction from Node #%d[Y=%d,X=%d] to [Y=%d,X=%d]\n",
          NodeNr, Y[NodeNr], X[NodeNr], NextY, NextX);
      exit(3);
    }
    // The new coordinate being added by NodeNr+1 is either X or Y
    if ((Dir[NodeNr] == eSouth) || (Dir[NodeNr] == eNorth))
    {
      // Find insertion spot for new Y coordinate
      for (GridYNr=0; GridYNr<NrOfGridY; GridYNr++)
        if (GridY[GridYNr] >= NextY)  break;
      if (GridY[GridYNr] == NextY)
      {
/* Debug */
if (1)  printf("Node[%d].Y-Coord = %d already known\n", NodeNr, NextY);
        continue;  // Y-Coordinate already known
      }
      // Move all larger values out by one spot
      GridIndex = GridYNr;
      for (GridYNr=NrOfGridY; GridYNr>GridIndex; GridYNr--)
        GridY[GridYNr] = GridY[GridYNr-1];
      GridY[GridIndex] = NextY;
      NrOfGridY++;
    }
    else /* Dir == eEast or Dir == eWest */
    {
      // Find insertion spot for new X coordinate
      for (GridXNr=0; GridXNr<NrOfGridX; GridXNr++)
        if (GridX[GridXNr] >= NextX)  break;
      if (GridX[GridXNr] == NextX)
      {
/* Debug */
if (1)  printf("Node[%d].X-Coord = %d already known\n", NodeNr, NextX);
        continue;  // X-Coordinate already known
      }
      // Move all larger values out by one spot
      GridIndex = GridXNr;
      for (GridXNr=NrOfGridX; GridXNr>GridIndex; GridXNr--)
        GridX[GridXNr] = GridX[GridXNr-1];
      GridX[GridIndex] = NextX;
      NrOfGridX++;
    }
  } /* for (NodeNr) */
  printf("Found NrOfGridY=%d and NrOfGridX=%d\n", NrOfGridY, NrOfGridX);

  // After the expansion of GridX,Y arrays has settled, link back to NodeNr
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    for (GridXNr=0; GridXNr<NrOfGridX; GridXNr++)
      if (GridX[GridXNr] == X[NodeNr])  {  NodeGridXNr[NodeNr] = GridXNr;  break; }
    for (GridYNr=0; GridYNr<NrOfGridY; GridYNr++)
      if (GridY[GridYNr] == Y[NodeNr])  {  NodeGridYNr[NodeNr] = GridYNr;  break; }
  }

  // Allocate NrOfGridX * NrOfGridY chars
  if (!(Grid = (char**)malloc(NrOfGridY * sizeof(char*))))
  {
    fprintf(stderr, "Could not allocate NrOfGridY=%d char* for Grid\n", NrOfGridY);
    exit(4);
  }
  for (GridYNr=0; GridYNr<NrOfGridY; GridYNr++)
  {
    if (!(Grid[GridYNr] = (char*)malloc(NrOfGridX * sizeof(char))))
    {
      fprintf(stderr, "Could not allocate NrOfGridX=%d char for Grid[%d]\n", NrOfGridX, GridYNr);
      exit(4);
    }
    memset(Grid[GridYNr], 0, NrOfGridX*sizeof(char));
  }

/* Debug */
if (0) {
for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  Grid[NodeGridYNr[NodeNr]][NodeGridXNr[NodeNr]] = '0' + (NodeNr % 10);
for (GridYNr=0; GridYNr<NrOfGridY; GridYNr++) {
  for (GridXNr=0; GridXNr<NrOfGridX; GridXNr++)
    if (!Grid[GridYNr][GridXNr]) putchar(' ');  else putchar(Grid[GridYNr][GridXNr]);
  putchar('\n'); }
}

  // Now try to colorize this schematic Grid by assuming the Loop runs ClockWise
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    // Consider the starting and ending points of this line segment
    if (NodeNr == NrOfNodes-1)  { NextX = NodeGridXNr[0];  NextY = NodeGridYNr[0]; }
    else  { NextX = NodeGridXNr[NodeNr+1];  NextY = NodeGridYNr[NodeNr+1]; }
    // Find the closest opposing wall
    int MinNodeNr = 1000, MinWall = 1000;
    for (OtherNodeNr=0; OtherNodeNr<NrOfNodes; OtherNodeNr++)
    {
      // Consider starting and ending points of any Other line segment
      if (OtherNodeNr == NrOfNodes-1)  { OtherNextX = NodeGridXNr[0];  OtherNextY = NodeGridYNr[0]; }
      else  { OtherNextX = NodeGridXNr[OtherNodeNr+1];  OtherNextY = NodeGridYNr[OtherNodeNr+1]; }
      switch (Dir[NodeNr])
      {
        case eNorth:
          if ((Dir[OtherNodeNr] != eSouth) || (OtherNextX < NextX))  continue;
          if ((NodeGridYNr[NodeNr] > NodeGridYNr[OtherNodeNr]) && (NextY < OtherNextY))
          {
            // Opposing wall found
/* Debug */
if (1)  printf("North #%d[GridX=%d,GridY=%d..%d] faces South #%d[GridX=%d,GridY=%d..%d]\n",
NodeNr, NextX, NodeGridYNr[NodeNr], NextY,
OtherNodeNr, OtherNextX, NodeGridYNr[OtherNodeNr], OtherNextY);
            if (OtherNextX - NextX < MinWall)
            { MinWall = OtherNextX - NextX;  MinNodeNr = OtherNodeNr; }
          }
          break;
        case eEast:
          if ((Dir[OtherNodeNr] != eWest) || (OtherNextY < NextY))  continue;
          if ((NodeGridXNr[NodeNr] < NodeGridXNr[OtherNodeNr]) && (NextX > OtherNextX))
          {
            // Opposing wall found
/* Debug */
if (1)  printf("East #%d[GridX=%d..%d,GridY=%d] faces West #%d[GridX=%d..%d,GridY=%d]\n",
NodeNr, NodeGridXNr[NodeNr], NextX, NextY,
    OtherNodeNr, NodeGridXNr[OtherNodeNr], OtherNextX, OtherNextY);
            if (OtherNextY - NextY < MinWall)
            { MinWall = OtherNextY - NextY;  MinNodeNr = OtherNodeNr; }
          }
          break;
        case eSouth:
          if ((Dir[OtherNodeNr] != eNorth) || (OtherNextX > NextX))  continue;
          if ((NodeGridYNr[NodeNr] < NodeGridYNr[OtherNodeNr]) && (NextY > OtherNextY))
          {
            // Opposing wall found
/* Debug */
if (1)  printf("South #%d[GridX=%d,GridY=%d..%d] faces North #%d[GridX=%d,GridY=%d..%d]\n",
NodeNr, NextX, NodeGridYNr[NodeNr], NextY,
OtherNodeNr, OtherNextX, NodeGridYNr[OtherNodeNr], OtherNextY);
            if (NextX - OtherNextX < MinWall)
            { MinWall = NextX - OtherNextX;  MinNodeNr = OtherNodeNr; }
          }
          break;
        case eWest:
          if ((Dir[OtherNodeNr] != eEast) || (OtherNextY > NextY))  continue;
          if ((NodeGridXNr[NodeNr] > NodeGridXNr[OtherNodeNr]) && (NextX < OtherNextX))
          {
            // Opposing wall found
/* Debug */
if (1)  printf("West #%d[GridX=%d..%d,GridY=%d] faces East #%d[GridX=%d..%d,GridY=%d]\n",
NodeNr, NodeGridXNr[NodeNr], NextX, NextY,
    OtherNodeNr, NodeGridXNr[OtherNodeNr], OtherNextX, OtherNextY);
            if (NextY - OtherNextY < MinWall)
            { MinWall = NextY - OtherNextY;  MinNodeNr = OtherNodeNr; }
          }
          break;
      } /* switch (Dir) */
    } /* for (OtherNodeNr) */
    switch (Dir[NodeNr])
    {
      case eNorth:
        for (GridXNr=NextX; (GridXNr<=NodeGridXNr[MinNodeNr]) && (GridXNr<NrOfGridX);
             GridXNr++)
          for (GridYNr=NodeGridYNr[NodeNr]; (GridYNr>=NextY) && (GridYNr>=0); GridYNr--)
            Grid[GridYNr][GridXNr] = '#';
        break;
      case eEast:
        for (GridYNr=NextY; (GridYNr<=NodeGridYNr[MinNodeNr]) && (GridYNr<NrOfGridY);
             GridYNr++)
          for (GridXNr=NodeGridXNr[NodeNr]; (GridXNr<=NextX) && (GridXNr<NrOfGridX); GridXNr++)
            Grid[GridYNr][GridXNr] = '#';
        break;
      case eSouth:
        for (GridXNr=NextX; (GridXNr>=NodeGridXNr[MinNodeNr]) && (GridXNr>=0);
             GridXNr--)
          for (GridYNr=NodeGridYNr[NodeNr]; (GridYNr<=NextY) && (GridYNr<NrOfGridY); GridYNr++)
            Grid[GridYNr][GridXNr] = '#';
        break;
      case eWest:
        for (GridYNr=NextY; (GridYNr>=NodeGridYNr[MinNodeNr]) && (GridYNr>=0);
             GridYNr--)
          for (GridXNr=NodeGridXNr[NodeNr]; (GridXNr>=NextX) && (GridXNr>=0); GridXNr--)
            Grid[GridYNr][GridXNr] = '#';
        break;
    } /* switch (Dir) */
  } /* for (NodeNr) */

/* Debug */
if (1) {
for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  Grid[NodeGridYNr[NodeNr]][NodeGridXNr[NodeNr]] = '0' + (NodeNr % 10);
for (GridYNr=0; GridYNr<NrOfGridY; GridYNr++) {
  for (GridXNr=0; GridXNr<NrOfGridX; GridXNr++)
    if (!Grid[GridYNr][GridXNr]) putchar(' ');  else putchar(Grid[GridYNr][GridXNr]);
  putchar('\n'); }
}

  // Consider all possible squares between any two nodes on the loop
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    for (OtherNodeNr=NodeNr+1; OtherNodeNr<NrOfNodes; OtherNodeNr++)
    {
      Area = 1;
      for (InputField=0; InputField<2; InputField++)
      {
        switch (InputField)
        {
          case 0: DeltaCoord = X[NodeNr] - X[OtherNodeNr];  break;
          case 1: DeltaCoord = Y[NodeNr] - Y[OtherNodeNr];  break;
        }
        if (DeltaCoord < 0)  DeltaCoord = - DeltaCoord;
        DeltaCoord++;
        Area *= (unsigned long long)DeltaCoord;
      }
      if (Area < MaxArea)  continue;
      // Examine the potential new record holder for compliance
      int Compliant = 1;
      for (GridXNr=NodeGridXNr[NodeNr]; ;
           GridXNr+=(NodeGridXNr[NodeNr]<NodeGridXNr[OtherNodeNr] ? 1 : -1))
      {
        for (GridYNr=NodeGridYNr[NodeNr]; ;
             GridYNr+=(NodeGridYNr[NodeNr]<NodeGridYNr[OtherNodeNr] ? 1 : -1))
        {
          if (!Grid[GridYNr][GridXNr])  
          {  Compliant = 0;  break; }
          if (GridYNr == NodeGridYNr[OtherNodeNr])  break;
        }
        if (GridYNr != NodeGridYNr[OtherNodeNr])  break;
        if (GridXNr == NodeGridXNr[OtherNodeNr])  break;
      }
      if (Compliant)
      {
/* Debug */
if (1)  printf("#%d[GridX=%d,GridY=%d] x #%d[GridX=%d,GridY=%d] = %llu\n",
NodeNr, NodeGridXNr[NodeNr], NodeGridYNr[NodeNr],
OtherNodeNr, NodeGridXNr[OtherNodeNr], NodeGridYNr[OtherNodeNr], Area);
        MaxArea = Area;  // Compliant
      }
    } /* for (OtherNodeNr) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found MaxArea %llu\n", InputLineNr, MaxArea);
  return 0;
}
