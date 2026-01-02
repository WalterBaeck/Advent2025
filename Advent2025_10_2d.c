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

  int Index, NeededAtOrder[30], Partition[30][30], MaxPart[30][30];
  int Remainder;

  int Total[30], Push[30];
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
    memset(Push, 0, 30 * sizeof(int));
    Order = -1;

    for (;;)
    {
      //////////////////// Descent ////////////////////
      for (++Order; Order<NrOfLights; Order++)
      {
        // Reset every Partition all the way downstream
        if (!NrOfButtonAtOrder[Order])  break;  // End reached

        // Establish NeededAtOrder: what the target Light at this Order still requires
        LightNr = LightOrder[Order];
        NeededAtOrder[Order] = Joltage[LightNr] - Total[LightNr];  // May become negative !
        if (NeededAtOrder[Order] < 0)
        {
/* Debug */
if (1)  printf("  v Incomplete: Order #%d Light #%d wants Joltage %d but Total at %d\n",
Order, LightNr, Joltage[LightNr], Total[LightNr]);
PrintDebug();
          break;
        }

        // Establish MaxPart:
        //   how often each button within this Partition can still be pushed
        for (Index=0; Index<NrOfButtonAtOrder[Order]; Index++)
        {
          MaxPart[Order][Index] = -1;
          ButtonNr = ButtonAtOrder[Order][Index];
          for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
            if (Button[ButtonNr][OtherLightNr])
            {
              int Needed = Joltage[OtherLightNr] - Total[OtherLightNr];
              if (Needed < 0)
              {
                fprintf(stderr, "MaxPart[Order=%d][%d] Button #%d Light #%d Joltage %d Total %d\n",
                    Order, Index, ButtonNr, OtherLightNr, Joltage[OtherLightNr], Total[OtherLightNr]);
                exit(4);
              }
              if ((MaxPart[Order][Index] == -1) || (Needed < MaxPart[Order][Index]))
                MaxPart[Order][Index] = Needed;
            }
        } /* for (Index) */

        // Reset Partition to maximum-right-shifted
        memset(Partition[Order], 0, 30 * sizeof(int));
        Remainder = NeededAtOrder[Order];
        for (Index = NrOfButtonAtOrder[Order]-1; Index>=0; Index--)
        {
          Partition[Order][Index] =
            (Remainder < MaxPart[Order][Index] ? Remainder : MaxPart[Order][Index]);
          Remainder -= Partition[Order][Index];
          ButtonNr = ButtonAtOrder[Order][Index];
          Push[ButtonNr] = Partition[Order][Index];
          for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
            if (Button[ButtonNr][OtherLightNr])
              Total[OtherLightNr] += Push[ButtonNr];
/* Debug */
if (1)  printf("  v Order #%d Light #%d, setting Button=%d to Push=%d\n",
Order, LightNr, ButtonNr, Push[ButtonNr]);
PrintDebug();
          if (!Remainder)  break;
        } /* for (Index) */
      } /* for (Order) */

      //////////////////// Result Check ////////////////////
      if ((!NrOfButtonAtOrder[Order]) && (!memcmp(Total, Joltage, NrOfLights * sizeof(int))))
      {
        NrOfPush = 0;
        for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
          NrOfPush += Push[ButtonNr];
        if ((MinNrOfPush == -1) || (NrOfPush < MinNrOfPush))
        {
          MinNrOfPush = NrOfPush;
/* Debug */
if (1)  printf("  - MinNrOfPush=%d\n", MinNrOfPush);
        }
      }

      //////////////////// Ascent ////////////////////
Ascent:
      for (--Order; Order>=0; Order--)
        if (NrOfButtonAtOrder[Order] == 1)
        {
          // No free choice here, need to backtrack further
          // Bookkeeping for this abandoned Order
          Partition[Order][0] = 0;
          ButtonNr = ButtonAtOrder[Order][0];
/* Debug */
if (1)  printf("  ^ Trivial Order #%d resetting Button=%d from Push=%d\n",
Order, ButtonNr, Push[ButtonNr]);
          for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
            if (Button[ButtonNr][OtherLightNr])
              Total[OtherLightNr] -= Push[ButtonNr];
          Push[ButtonNr] = 0;
PrintDebug();
        }
        else
        {
          // So we have a Partition here with a real choice.
          // Change We Can Believe In means: raising any possible number,
          //   from one-but-last back till the foremost.
          // This will gradually shift a rightmost Partition to leftmost.
          int NonZeroSeen = 0;
          for (Index=NrOfButtonAtOrder[Order]-1; Index>0; Index--)
          {
            if (Partition[Order][Index])  NonZeroSeen=1;
            if ((Partition[Order][Index-1] < MaxPart[Order][Index-1]) && 
                (Partition[Order][Index-1] < NeededAtOrder[Order]) &&
                (NonZeroSeen))  break;
          }
          if (Index)
            break; /* from Ascent */
          else  /* Index=0 */
          {
            // Partition of max #n has gone maximum-left-shifted
            // Bookkeeping for this abandoned Order
            for (Index=0; Index<NrOfButtonAtOrder[Order]; Index++)
            {
              ButtonNr = ButtonAtOrder[Order][Index];
              for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
                if (Button[ButtonNr][OtherLightNr])
                  Total[OtherLightNr] -= Push[ButtonNr];
              Push[ButtonNr] = 0;
            }
/* Debug */
if (1)  printf("  ^ Exhausted Partition Order #%d\n", Order);
PrintDebug();
            // No further choices possible here, so need to backtrack further
          }
        } /* if real choice */
      if (Order < 0)  break;  /* from forever */

      //////////////////// Next choice ////////////////////

      // So we have found a choice Partition[Order][Index-1] that can be raised.
      Partition[Order][Index-1]++;
      ButtonNr = ButtonAtOrder[Order][Index-1];
      Push[ButtonNr]++;
      for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
        if (Button[ButtonNr][OtherLightNr])
          Total[OtherLightNr]++;
/* Debug */
if (0)  printf("  + Order #%d increasing Button=%d to Push=%d\n",
Order, ButtonNr, Push[ButtonNr]);
PrintDebug();

      // Then we reset all choices further down this Partition to zero ..
      int ResetIndex = Index;
      for (; Index<NrOfButtonAtOrder[Order]; Index++)
      {
        Partition[Order][Index] = 0;
        ButtonNr = ButtonAtOrder[Order][Index];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] -= Push[ButtonNr];
        Push[ButtonNr] = 0;
      }

      // .. and make everything maximum-right-shifted again
      Remainder = NeededAtOrder[Order];
      for (Index=0; Index<ResetIndex; Index++)
        Remainder -= Partition[Order][Index];
      for (Index = NrOfButtonAtOrder[Order]-1; Index>=ResetIndex; Index--)
      {
        Partition[Order][Index] =
          (Remainder < MaxPart[Order][Index] ? Remainder : MaxPart[Order][Index]);
        Remainder -= Partition[Order][Index];
        ButtonNr = ButtonAtOrder[Order][Index];
        Push[ButtonNr] = Partition[Order][Index];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] += Push[ButtonNr];
/* Debug */
if (0)  printf("  + Order #%d setting Button=%d to Push=%d\n",
Order, ButtonNr, Push[ButtonNr]);
PrintDebug();
        if (!Remainder)  break;
      }
      if (Remainder)
      {
        fprintf(stderr, "  + Order #%d raising Button #%d but cannot right-shift remainder\n",
            Order, ButtonNr);
        exit(5);
      }

      //////////////////// Sanity Check ////////////////////
      for (LightNr=0; LightNr<NrOfLights; LightNr++)
        if (Total[LightNr] > Joltage[LightNr])  break;
      if (LightNr<NrOfLights)
      {
/* Debug */
if (1)  printf("  + Insanity: Order #%d Light #%d wants Joltage %d but Total at %d\n",
Order, LightNr, Joltage[LightNr], Total[LightNr]);
PrintDebug();
        // This new Partition choice is not viable. Keep changing it.
        Order++;
        goto Ascent;
      }

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
