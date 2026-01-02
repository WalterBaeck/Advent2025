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
  char* InputPtr;

  int TileLineNr, Num;
  int TileNr=0, NrOfTiles=6;
  int FormNr, NrOfForms[6];
  char Tile[6][8][3][3];
  int NrOfSquares[6], Amount[6];

  int XSize,YSize, X,Y, XPos,YPos, XDiff,YDiff;
  char Grid[60][60];
  int Sum=0, EasyYes=0, EasyNo=0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 100, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';

    ////////////////////// Parsing Tile shapes //////////////////////
    if (TileNr < NrOfTiles)
    {
      TileLineNr = (InputLineNr - 1) % 5;
      switch (TileLineNr)
      {
        case 0:
          if (*InputLine != '0' + TileNr)
          {
            fprintf(stderr, "Expecting TileNr[%d] on InputLine #%d instead of %c\n",
                TileNr, InputLineNr, *InputLine);
            exit(2);
          }
          if (InputLine[1] != ':')
          {
            fprintf(stderr, "Expecting colon on InputLine #%d pos 1 instead of %c\n",
                InputLineNr, InputLine[1]);
            exit(2);
          }
          break;
        case 1:
        case 2:
        case 3:
          strncpy(Tile[TileNr][0][TileLineNr-1], InputLine, 3);
          break;
        case 4:
          if (strlen(InputLine))
          {
            fprintf(stderr, "Expecting blank line to close a tile shape at InputLine #%d\n",
                InputLineNr);
            exit(2);
          }
          // Count the squares taken by this tile - immutable with any transformation
          NrOfSquares[TileNr] = 0;
          for (Y=0; Y<3; Y++)  for (X=0; X<3; X++)
            if (Tile[TileNr][0][Y][X] == '#')  NrOfSquares[TileNr]++;
          // Create up to 8 Form variants of this Tile
          int FormsFound = 1;
          // Rotate into forms 1..3 ; flip vertical into form 4 ; rotate again into 5..7
          for (FormNr=1; FormNr<8; FormNr++)
          {
            for (Y=0; Y<3; Y++)  for (X=0; X<3; X++)
              switch (FormNr)
              {
                case 1: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][X][2-Y];    break;
                case 2: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][2-Y][2-X];  break;
                case 3: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][2-X][Y];    break;
                case 4: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][2-Y][X];    break;
                case 5: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][2-X][2-Y];  break;
                case 6: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][Y][2-X];    break;
                case 7: Tile[TileNr][FormsFound][Y][X] = Tile[TileNr][0][X][Y];      break;
              }
            // Check if this variant is identical to an already obtained Form
            for (Num=0; Num<FormsFound; Num++)
            {
              int Equal = 1;
              for (Y=0; Y<3; Y++)
                if (strncmp(Tile[TileNr][FormsFound][Y], Tile[TileNr][Num][Y], 3))
                  Equal = 0;
              if (Equal)  break;
            }
            if (Num >= FormsFound)  FormsFound++;
          } /* for (FormNr) */
          NrOfForms[TileNr] = FormsFound;
/* Debug */
if (1)  printf("Found %d Forms of Tile #%d with NrOfSquares=%d\n",
NrOfForms[TileNr], TileNr, NrOfSquares[TileNr]);
          TileNr++;
      } /* switch (TileLineNr) */
    } /* if (TileNr < NrOfTiles) */
    else
    {
      ////////////////////// Parsing Grid //////////////////////
      InputPtr = strtok(InputLine, "x");
      if (1 != sscanf(InputPtr, "%d", &XSize))
      {
        fprintf(stderr, "Could not scan XSize from %s on InputLine #%d\n",
            InputPtr, InputLineNr);
        exit(2);
      }
      InputPtr = strtok(NULL, ":");
      if (1 != sscanf(InputPtr, "%d", &YSize))
      {
        fprintf(stderr, "Could not scan YSize from %s on InputLine #%d\n",
            InputPtr, InputLineNr);
        exit(2);
      }
      ////////////////////// Parsing Amounts //////////////////////
      int TotalTiles = 0, TotalSquares = 0;
      for (TileNr=0; TileNr<NrOfTiles; TileNr++)
      {
        InputPtr = strtok(NULL, " ");
        if (1 != sscanf(InputPtr, "%d", &(Amount[TileNr])))
        {
          fprintf(stderr, "Could not scan Amount[%d] from %s on InputLine #%d\n",
              TileNr, InputPtr, InputLineNr);
          exit(2);
        }
        TotalTiles += Amount[TileNr];
        TotalSquares += (Amount[TileNr] * NrOfSquares[TileNr]);
      } /* for (TileNr) */
      printf("TotalTiles %d to fit in %dx%d = %d (so %.0f%% filled) with %d empty squares\n",
          TotalTiles, XSize, YSize , XSize * YSize, 900.0*TotalTiles / XSize / YSize,
          XSize * YSize - TotalSquares);
      if (XSize * YSize < TotalSquares)  EasyNo++;
      else if ((XSize/3) * (YSize/3) >= TotalTiles)
      {
        printf("%dx%d available 3x3 spaces for only %d tiles\n",
            (XSize/3), (YSize/3), TotalTiles);
        EasyYes++;
      }
    } /* parsing task */
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d with EasyYes=%d EasyNo=%d\n",
      InputLineNr, Sum, EasyYes, EasyNo);
  return 0;
}
