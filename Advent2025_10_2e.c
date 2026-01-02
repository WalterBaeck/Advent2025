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
  char InputLine[1000];
  char *InputStart, *InputEnd, *InputPtr;

  char InputSegment[100];
  int Num;

  int LightNr, NrOfLights, OtherLightNr;
  int ButtonNr, NrOfButtons, Button[30][30];
  int Joltage[30] = {0};

  int LightOccurs, MinLightOccurs, MinLightNr;
  int Order, LightDone[30], LightOrder[30], ButtonDone[30];
  int ButtonAtOrder[30][30], NrOfButtonAtOrder[30];

  int Index, RaiseIndex;
  int Partition[30][30], OldPart[30], NewPart[30];
  int MaxAtRaise, MaxPossible, Remainder;

  int Total[30], OldTotal[30], NewTotal[30];
  int NrOfPush, MinNrOfPush, Sum=0;

  /*************/
  /* Operation */
  /*************/
  while (fgets(InputLine, 1000, InputFile))
  {
    InputLineNr++;
    while ((InputLine[strlen(InputLine) - 1] == '\r')
        || (InputLine[strlen(InputLine) - 1] == '\n'))
      InputLine[strlen(InputLine) - 1] = '\0';
    //////////////////// Parse the lights ////////////////////
    InputStart = InputLine;
    if (*InputStart != '[')
    {
      fprintf(stderr, "Could not find [ in InputLine #%d:\n%s\n",
          InputLineNr, InputLine);
      exit(2);
    }
    if (!(InputEnd = strchr(InputStart, ']')))
    {
      fprintf(stderr, "Could not find [ in InputLine #%d:\n%s\n",
          InputLineNr, InputLine);
      exit(2);
    }
    NrOfLights = InputEnd - InputStart - 1;
    if (NrOfLights > 30)
    {
      fprintf(stderr, "Cannot handle NrOfLights=%d on InputLine #%d\n",
          NrOfLights, InputLineNr);
      exit(3);
    }
    for (LightNr=0; LightNr<NrOfLights; LightNr++)
      if ((InputStart[LightNr+1] != '#') && (InputStart[LightNr+1] != '.'))
      {
        fprintf(stderr, "Unknown Light Target #%d : %c on InputLine #%d\n",
            LightNr, InputStart[LightNr+1], InputLineNr);
        exit(2);
      }
    //////////////////// Parse the buttons ////////////////////
    ButtonNr = 0;
    for (;;)
    {
      if (InputEnd[1] != ' ')
      {
        fprintf(stderr, "Expecting a space instead of %c at pos %d of InputLine #%d\n",
            InputEnd[1], InputEnd - InputLine + 1, InputLineNr);
        exit(2);
      }
      if (InputEnd[2] != '(')  break;
      InputStart = InputEnd + 3;
      if (!(InputEnd = strchr(InputStart, ')')))
      {
        fprintf(stderr, "Could not find ) in InputLine #%d:\n%s\n",
            InputLineNr, InputLine);
        exit(2);
      }
      strncpy(InputSegment, InputStart, InputEnd - InputStart);
      InputSegment[InputEnd - InputStart] = '\0';
      memset(Button[ButtonNr], 0, 30 * sizeof(int));
      for (Num=0; InputPtr = strtok(Num ? NULL : InputSegment, ","); Num++)
      {
        if (1 != sscanf(InputPtr, "%d", &LightNr))
        {
          fprintf(stderr, "Could not scan Button #%d Num #%d from %s on InputLine #%d\n",
              ButtonNr, Num, InputPtr, InputLineNr);
          exit(2);
        }
        Button[ButtonNr][LightNr] = 1;
      } /* for (strtok) */
      ButtonNr++;
      if (ButtonNr >= 30)
      {
        fprintf(stderr, "Cannot handle NrOfButtons=%d on InputLine #%d\n",
            ButtonNr+1, InputLineNr);
        exit(3);
      }
    } /* forever */
    NrOfButtons = ButtonNr;
    //////////////////// Parse the joltages ////////////////////
    if (InputEnd[2] != '{')
    {
      fprintf(stderr, "Expecting { instead of %c at pos %d of InputLine #%d\n",
          InputEnd[1], InputEnd - InputLine + 2, InputLineNr);
      exit(2);
    }
    InputStart = InputEnd + 3;
    if (!(InputEnd = strchr(InputStart, '}')))
    {
      fprintf(stderr, "Could not find } in InputLine #%d:\n%s\n",
          InputLineNr, InputLine);
      exit(2);
    }
    strncpy(InputSegment, InputStart, InputEnd - InputStart);
    InputSegment[InputEnd - InputStart] = '\0';
    for (LightNr=0; InputPtr = strtok(LightNr ? NULL : InputSegment, ","); LightNr++)
      if (1 != sscanf(InputPtr, "%d", &(Joltage[LightNr])))
      {
        fprintf(stderr, "Could not scan Joltage #%d from %s on InputLine #%d\n",
            LightNr, InputPtr, InputLineNr);
        exit(2);
      }
    if (LightNr != NrOfLights)
    {
      fprintf(stderr, "%d Joltages but %d Lights on InputLine #%d ?\n",
          LightNr, NrOfLights, InputLineNr);
      exit(2);
    }

/* Debug */
if (1)  printf("Line #%d: Found %d Lights, %d Buttons\n", InputLineNr, NrOfLights, NrOfButtons);

    //////////////////// Establish LightOrder ////////////////////
    memset(LightOrder, 0, 30 * sizeof(int));
    memset(LightDone, 0, 30 * sizeof(int));
    memset(ButtonDone, 0, 30 * sizeof(int));
    memset(NrOfButtonAtOrder, 0, 30 * sizeof(int));
    for (Order=0; Order<NrOfLights; Order++)
    {
      MinLightOccurs = -1;
      memset(ButtonAtOrder[Order], 0, 30 * sizeof(int));
      // Go over all lights that can still be affected by unused buttons
      for (LightNr=0; LightNr<NrOfLights; LightNr++)
      {
        if (LightDone[LightNr])  continue;
        LightOccurs = 0;
        for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
        {
          if (ButtonDone[ButtonNr])  continue;
          if (Button[ButtonNr][LightNr])  LightOccurs++;
        }
        if (!LightOccurs)
        {  LightDone[LightNr] = 1;  continue; }
        if ((MinLightOccurs == -1) || (LightOccurs < MinLightOccurs))
        {  MinLightOccurs = LightOccurs;  MinLightNr = LightNr; }
      }
      if (MinLightOccurs == -1)  break;
      // So, this LightNr is affected by fewest remaining available buttons
      LightOrder[Order] = MinLightNr;
      LightDone[MinLightNr] = 1;
/* Debug */
if (1)  printf(" At Order #%d, settle Light #%d with Buttons", Order, MinLightNr);
      for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
        if ((Button[ButtonNr][MinLightNr]) && (!ButtonDone[ButtonNr]))
        {
          ButtonDone[ButtonNr] = 1;
          ButtonAtOrder[Order][NrOfButtonAtOrder[Order]++] = ButtonNr;
/* Debug */
if (1)  printf(" %d", ButtonNr);
        }
/* Debug */
if (1)  putchar('\n');
    } /* for (Order) */

    //////////////////// Debugging ////////////////////
    void PrintDebug()
    {
      return;
      // Convert Partition[0..Order][Index] to Push[ButtonNr]
      int Push[30], Ord, But;
      memset(Push, 0, 30 * sizeof(int));
      for (Ord=0; Ord<=Order; Ord++)
        for (But=0; But<NrOfButtonAtOrder[Ord]; But++)
          Push[ButtonAtOrder[Ord][But]] = Partition[Ord][But];
      // Now display everything at this Order on two information lines
      printf("+%3d+", Total[0]);
      for (Num=1; Num<NrOfLights; Num++)  printf("%2d+", Total[Num]);
      printf("  Push#%3d#", Push[0]);
      for (Num=1; Num<NrOfButtons; Num++)  printf("%2d#", Push[Num]);
      putchar('\n');
      printf("-%3d-", Joltage[0] - Total[0]);
      for (Num=1; Num<NrOfLights; Num++)  printf("%2d-", Joltage[Num] - Total[Num]);
      printf("  P[%d]#%3d#", Order, Partition[Order][0]);
      for (Num=1; Num<NrOfButtonAtOrder[Order]; Num++)  printf("%2d#", Partition[Order][Num]);
      putchar('\n');
    }


    //////////////////// Priming ////////////////////
    MinNrOfPush = -1;
    memset(Total, 0, 30 * sizeof(int));
    Order = -1;

    for (;;)
    {
      //////////////////// Descent ////////////////////
      if (!NrOfButtonAtOrder[++Order])  // End reached
      {
        //////////////////// Result Check ////////////////////
        if (!memcmp(Total, Joltage, NrOfLights * sizeof(int)))
        {
          NrOfPush = 0;
          int Ord, But;
          for (Ord=0; Ord<Order; Ord++)
            for (But=0; But<NrOfButtonAtOrder[Ord]; But++)
              NrOfPush += Partition[Ord][But];
          if ((MinNrOfPush == -1) || (NrOfPush < MinNrOfPush))
          {
            MinNrOfPush = NrOfPush;
/* Debug */
if (1)  printf("  - MinNrOfPush=%d\n", MinNrOfPush);
          }
        }
        Order--;  // Explore further possibilities at the last Order
      } /* if last Order reached */
      else
      {
        // We have descended to a later Order.
        // Prime it for the subsequent Partition Increase with dummy choice [0,0 0,-1]
        memset(Partition[Order], 0, 30 * sizeof(int));
        Partition[Order][NrOfButtonAtOrder[Order]-1] = -1;
        ButtonNr = ButtonAtOrder[Order][NrOfButtonAtOrder[Order]-1];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr]--;
      }

      do {
      //////////////////// Partition Increase ////////////////////
      // The heart of the algorithm is this step, aiming to find the next Partition
      //   possible at this Order. Any choice within the current Partition is
      //   examined for the possibility of being increased, all while respecting
      //   the constraints for each Light.
      // This search is done from right to left.
      // All choices to the left of the tentative RaiseIndex are left intact,
      //   while everything to the right of RaiseIndex is reset to zero, and
      //   will be filled in completely redistributed to rightmost possible.
      // Besides respecting every Light's constraint, additionally it is demanded
      //   that the single Light to be fulfilled at this order, be completed.
      // If not, this new partition choice cannot be considered successful,
      //   and the search continues.
      //
      // The algorithm will slowly reshape a rightmost-leaning Partition
      //   over to the left, until the choices are maximised at the left side,
      //   and cannot be raised any further.

      LightNr = LightOrder[Order];
if (0)  printf("Entering Increase[Order=%d] - aiming for Light #%d\n", Order, LightNr);
PrintDebug();

      // Remember the current Partition choice, then clear everything
      memcpy(OldPart, Partition[Order], 30 * sizeof(int));
      for (Index=0; Index<NrOfButtonAtOrder[Order]; Index++)
      {
        ButtonNr = ButtonAtOrder[Order][Index];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] -= Partition[Order][Index];
      }
      // Establish the situation up till Order-1
      memset(Partition[Order], 0, 30 * sizeof(int));
      memcpy(OldTotal, Total, 30 * sizeof(int));
if (0)  printf("Removed Order[%d] from Totals\n", Order);
PrintDebug();

      // Consider RaiseIndex from right to left
      for (RaiseIndex=NrOfButtonAtOrder[Order]-1; RaiseIndex>=0; RaiseIndex--)
      {
        // Everything to the left of the RaiseIndex will remain as it was before
        memcpy(NewPart, OldPart, 30 * sizeof(int));
        memcpy(Total, OldTotal, 30 * sizeof(int));
        for (Index=0; Index<RaiseIndex; Index++)
        {
          ButtonNr = ButtonAtOrder[Order][Index];
          for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
            if (Button[ButtonNr][OtherLightNr])
              Total[OtherLightNr] += NewPart[Index];
        }
if (0)  printf(" At RaiseIndex[%d]:\n", RaiseIndex);
PrintDebug();
        // At the RaiseIndex itself, consider any increase possible
        MaxAtRaise = -1;
        ButtonNr = ButtonAtOrder[Order][RaiseIndex];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
          {
            if (Total[OtherLightNr] > Joltage[OtherLightNr])  break;
            if ((MaxAtRaise == -1) || (Joltage[OtherLightNr] - Total[OtherLightNr] < MaxAtRaise))
              MaxAtRaise = Joltage[OtherLightNr] - Total[OtherLightNr];
          }
        if (OtherLightNr<NrOfLights)  continue;  // next RaiseIndex
if (0)  printf(" Button #%d can go to Max %d from current %d\n", ButtonNr, MaxAtRaise, NewPart[RaiseIndex]);
        for (++NewPart[RaiseIndex]; NewPart[RaiseIndex]<=MaxAtRaise; NewPart[RaiseIndex]++)
        {
          // With this suggested increased value at RaiseIndex, recompute
          memcpy(NewTotal, Total, 30 * sizeof(int));
          ButtonNr = ButtonAtOrder[Order][RaiseIndex];
          for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
            if (Button[ButtonNr][OtherLightNr])
              NewTotal[OtherLightNr] += NewPart[RaiseIndex];
          // Complete rework of every choice to the right from RaiseIndex
          memset(NewPart+RaiseIndex+1, 0, (29-RaiseIndex) * sizeof(int));
          for (Index=NrOfButtonAtOrder[Order]-1; Index>RaiseIndex; Index--)
          {
            MaxPossible = -1;
            ButtonNr = ButtonAtOrder[Order][Index];
            for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
              if (Button[ButtonNr][OtherLightNr])
              {
                if (NewTotal[OtherLightNr] > Joltage[OtherLightNr])  break;
                if ((MaxPossible == -1) || (Joltage[OtherLightNr] - NewTotal[OtherLightNr] < MaxPossible))
                  MaxPossible = Joltage[OtherLightNr] - NewTotal[OtherLightNr];
              }
            if (OtherLightNr<NrOfLights)  break;  // next RaiseIndex
            NewPart[Index] = MaxPossible;
            for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
              if (Button[ButtonNr][OtherLightNr])
                NewTotal[OtherLightNr] += NewPart[Index];
          }
          if (Index>RaiseIndex)  continue;  /* for (NewPart[RaiseIndex]) */

          // So, we have found a new Partition by increasing the choice at RaiseIndex
          //  but does it satisfy the target Light ?
          if (NewTotal[LightNr] == Joltage[LightNr])
          {
            //////////////////// Sanity Check ////////////////////
            for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
              if (NewTotal[OtherLightNr] > Joltage[OtherLightNr])  break;
            if (OtherLightNr<NrOfLights)
            {
/* Debug */
if (1)  printf("  + Insanity: Order #%d Light #%d wants Joltage %d but Total at %d\n",
Order, OtherLightNr, Joltage[OtherLightNr], Total[OtherLightNr]);
PrintDebug();
              // This new Partition choice is not viable. Keep changing it.
              continue;  /* for (NewPart[RaiseIndex]) */
            }
            // Make it official
            memcpy(Partition[Order], NewPart, 30 * sizeof(int));
            memcpy(Total, NewTotal, 30 * sizeof(int));
            break;  /* from (NewPart[RaiseIndex]) */
          }  // Increase successful

          // If not, the (NewPart[RaiseIndex]) loop continues
        } /* for (NewPart[RaiseIndex]) */

        // Did we reach a successful increase here ?
        if (NewPart[RaiseIndex]<=MaxAtRaise)  break;  /* from (RaiseIndex) */
      } /* for (RaiseIndex) */

      // If the RaiseIndex loop did not exhaust, the increase was successful
      if (RaiseIndex>=0)  break;  /* from do..while */

      //////////////////// Ascent ////////////////////
      // do {Partition Increase} while() loop will continue at an earlier Order now
      } while (--Order >= 0);

      if (Order < 0)  break;  /* from forever */
      // else :increase was successful : forever loop will start with a Descent
    } /* forever */

    Sum += MinNrOfPush;
/* Debug */
if (1)  printf(" Adding %d pushes to get at Sum=%d..\n", MinNrOfPush, Sum);
//if (InputLineNr==3)  break;
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
