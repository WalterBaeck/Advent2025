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

  int Index, MaxAtOrder[30], Partition[30][30];

  int Total[30], Push[30];
  int FakeResult, NrOfPush, MinNrOfPush, Sum=0;

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

    //////////////////// Establish LightOrder  ////////////////////
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
if (1)  printf("  At Order #%d, settle Light #%d with Buttons", Order, MinLightNr);
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

    // Exhaust all possible combinations of button presses
    MinNrOfPush = -1;
    memset(Total, 0, 30 * sizeof(int));
    memset(Push, 0, 30 * sizeof(int));
    // Start out with every Partition of max #n set to [0,0, ,0,n]
    for (Order=0; Order<NrOfLights; Order++)
    {
      if (!NrOfButtonAtOrder[Order])  break;  // End reached
      LightNr = LightOrder[Order];
      int Needed = Joltage[LightNr] - Total[LightNr];  // May become negative !
      if (Needed < 0)  break;
      MaxAtOrder[Order] = Needed;
      memset(Partition[Order], 0, 30 * sizeof(int));
      Index = NrOfButtonAtOrder[Order] - 1;
      Partition[Order][Index] = Needed;
      ButtonNr = ButtonAtOrder[Order][Index];
/* Debug */
if (1)  printf("--Priming:At Order #%d with Light #%d, setting Button=%d to Push=%d\n",
Order, LightNr, ButtonNr, Needed);
      Push[ButtonNr] = Needed;
      for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
        if (Button[ButtonNr][OtherLightNr])
          Total[OtherLightNr] += Needed;
    } /* for (Order) */
    if ((!NrOfButtonAtOrder[Order]) && (!memcmp(Total, Joltage, NrOfLights * sizeof(int))))
    {
      NrOfPush = 0;
      for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
        NrOfPush += Push[ButtonNr];
      if ((MinNrOfPush == -1) || (NrOfPush < MinNrOfPush))
      {
        MinNrOfPush = NrOfPush;
/* Debug */
if (1)  printf("--MinNrOfPush=%d\n", MinNrOfPush);
      }
    }

    Order--;
    for (;;)
    {
      // Always try to change the Partition choice at current Order
      while (NrOfButtonAtOrder[Order] == 1)
      {
        // No free choice here, need to backtrack further
        // Bookkeeping for this abandoned Order
        ButtonNr = ButtonAtOrder[Order][0];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] -= Push[ButtonNr];
        Push[ButtonNr] = 0;
        // Now backtrack if possible
        if (--Order < 0)  break;
      }
      if (Order < 0)  break;  /* from forever */

      // So we have a Partition here with a real choice.
      // Change We Can Believe In means: raising any possible number,
      //   from one-but-last back till the foremost.
      // The last number always follows automatically to comply with MaxAtOrder.
      // This is the first to be freed up, now that we are reconsidering.
      for (Index=NrOfButtonAtOrder[Order]-1; Index>0; Index--)
        if (Partition[Order][Index])  break;
      if (!Index)
      {
        // Partition of max #n has shifted all the way to [n,0, ,0,0]
        // Bookkeeping for this abandoned Order
        ButtonNr = ButtonAtOrder[Order][0];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] -= Push[ButtonNr];
        Push[ButtonNr] = 0;
        // No further choices possible here, so need to backtrack up
        if (--Order < 0)  break;  /* from forever */
        continue;  /* with forever */
      }

      // As we have found a nonzero choice beyond Index=0,
      //   there is room to increase an earlier choice.
      Partition[Order][--Index]++;
      ButtonNr = ButtonAtOrder[Order][Index];
      Push[ButtonNr]++;
      for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
        if (Button[ButtonNr][OtherLightNr])
          Total[OtherLightNr]++;

      // Then we reset all choices further down this Partition to zero ..
      for (++Index; Index<NrOfButtonAtOrder[Order]; Index++)
      {
        Partition[Order][Index] = 0;
        ButtonNr = ButtonAtOrder[Order][Index];
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] -= Push[ButtonNr];
        Push[ButtonNr] = 0;
      }

      // .. and we figure out what the last choice should be
      int Needed = MaxAtOrder[Order];
      for (Index=0; Index<NrOfButtonAtOrder[Order]; Index++)
        Needed -= Partition[Order][Index];
      Partition[Order][--Index] = Needed;
      ButtonNr = ButtonAtOrder[Order][Index];
      Push[ButtonNr] = Needed;
      for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
        if (Button[ButtonNr][OtherLightNr])
          Total[OtherLightNr] += Needed;

      // Sanity check: has any of the Joltages been exceeded so far ?
      for (LightNr=0; LightNr<NrOfLights; LightNr++)
        if (Total[LightNr] > Joltage[LightNr])  break;
      if (LightNr<NrOfLights)
        // This new Partition choice is not viable. Keep changing it.
        continue;  /* with forever */

      // After this successful change of a Partition,
      //   we are ready to descend to all further Orders
      //   to reset every Partition downstream
      for (++Order; Order<NrOfLights; Order++)
      {
        if (!NrOfButtonAtOrder[Order])  break;  // End reached
        LightNr = LightOrder[Order];
        int Needed = Joltage[LightNr] - Total[LightNr];  // May become negative !
        if (Needed < 0)  break;
        MaxAtOrder[Order] = Needed;
        memset(Partition[Order], 0, 30 * sizeof(int));
        Index = NrOfButtonAtOrder[Order] - 1;
        Partition[Order][Index] = Needed;
        ButtonNr = ButtonAtOrder[Order][Index];
        Push[ButtonNr] = Needed;
        for (OtherLightNr=0; OtherLightNr<NrOfLights; OtherLightNr++)
          if (Button[ButtonNr][OtherLightNr])
            Total[OtherLightNr] += Needed;
      } /* for (Order) */

      if ((!NrOfButtonAtOrder[Order]) && (!memcmp(Total, Joltage, NrOfLights * sizeof(int))))
      {
        NrOfPush = 0;
        for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
          NrOfPush += Push[ButtonNr];
        if ((MinNrOfPush == -1) || (NrOfPush < MinNrOfPush))
        {
          MinNrOfPush = NrOfPush;
/* Debug */
if (1)  printf("  +MinNrOfPush=%d\n", MinNrOfPush);
        }
      }

      // Now keep changing Partition choices from all the way down
      Order--;

    } /* forever */

    Sum += MinNrOfPush;
/* Debug */
if (1)  printf("  Adding %d presses to get at Sum=%d..\n", MinNrOfPush, Sum);
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
