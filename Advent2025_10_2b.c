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

  int LightNr, NrOfLights;
  int ButtonNr, NrOfButtons, Button[30][30];
  int Joltage[30] = {0};

  int ButtonEffect[30], ButtonMax[30], ButtonScore[30], ButtonDone[30];
  int MaxScore, MaxButton;

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
      ButtonEffect[ButtonNr] = Num;
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

    //////////////////// Decision making  ////////////////////
    memset(ButtonDone, 0, 30 * sizeof(int));
    memset(Total, 0, 30 * sizeof(int));
    memset(Push, 0, 30 * sizeof(int));
    NrOfPush = 0;

    for (;;)
    {
      // Select the button that yields maximum effect
      MaxScore = 0;   MaxButton = -1;
      for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
      {
        if (ButtonDone[ButtonNr])  continue;
        // See how often we can push this button and respect all Joltages
        ButtonMax[ButtonNr] = -1;
        for (LightNr=0; LightNr<NrOfLights; LightNr++)
          if (Button[ButtonNr][LightNr])
          {
            int MaxAllowed = Joltage[LightNr] - Total[LightNr];
            if (MaxAllowed < 0)
            {
              fprintf(stderr, "Light #%d wants Joltage %d but already has %d ??\n",
                  LightNr, Joltage[LightNr], Total[LightNr]);
              exit(4);
            }
            if ((ButtonMax[ButtonNr] == -1) || (MaxAllowed < ButtonMax[ButtonNr]))
              ButtonMax[ButtonNr] = MaxAllowed;
          }
        // With this maximum established, determine the total impact
        ButtonScore[ButtonNr] = ButtonMax[ButtonNr] * ButtonEffect[ButtonNr];
        if (ButtonScore[ButtonNr] > MaxScore)
        {  MaxScore = ButtonScore[ButtonNr];   MaxButton = ButtonNr; }

/* Debug */
if (1)  printf("   Button #%d has Max=%d and Effect=%d so Score=%d\n",
ButtonNr, ButtonMax[ButtonNr], ButtonEffect[ButtonNr], ButtonScore[ButtonNr]);

      } /* for (ButtonNr) */

      // When no Button was selected, the pushing is finished
      if ((!MaxScore) || (MaxButton == -1))  break;   /* from forever */

      // Go ahead and push the selected button as much as is allowed
      ButtonNr = MaxButton;

/* Debug */
if (1)  printf("  Selected Button #%d to be pushed %d times\n",
ButtonNr, ButtonMax[ButtonNr]);

      for (LightNr=0; LightNr<NrOfLights; LightNr++)
        if (Button[ButtonNr][LightNr])
          Total[LightNr] += ButtonMax[ButtonNr];
      Push[ButtonNr] = ButtonMax[ButtonNr];
      NrOfPush += ButtonMax[ButtonNr];
      ButtonDone[ButtonNr] = 1;
    } /* forever */

    // Verify that constraints have been met
    if (memcmp(Total, Joltage, NrOfLights * sizeof(int)))
    {
      fprintf(stderr, "After %d pushes, not all Joltages reached\n", NrOfPush);
      exit(4);
    }

    Sum += NrOfPush;
/* Debug */
if (1)  printf(" Adding %d presses to get at Sum=%d..\n", NrOfPush, Sum);
  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
