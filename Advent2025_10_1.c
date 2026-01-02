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

  int LightNr, NrOfLights, Target, State;
  int ButtonNr, NrOfButtons, Button[30];
  int Joltage[30];

  int Combo, NrOfBits, MinNrOfBits, Sum=0;

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
    Target = 0;
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
      if      (InputStart[LightNr+1] == '#')  Target |= (1<<LightNr);
      else if (InputStart[LightNr+1] != '.')
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
      Button[ButtonNr] = 0;
      for (Num=0; InputPtr = strtok(Num ? NULL : InputSegment, ","); Num++)
      {
        if (1 != sscanf(InputPtr, "%d", &LightNr))
        {
          fprintf(stderr, "Could not scan Button #%d Num #%d from %s on InputLine #%d\n",
              ButtonNr, Num, InputPtr, InputLineNr);
          exit(2);
        }
        Button[ButtonNr]  |= (1<<LightNr);
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
if (1)  printf("Found %d Lights, %d Buttons, Target=%X\n", NrOfLights, NrOfButtons, Target);

    // Exhaust all possible binary combinations of Buttons pressed
    MinNrOfBits = -1;
    for (Combo=0; Combo < (1<<NrOfButtons); Combo++)
    {
      State = 0;  NrOfBits = 0;
      for (ButtonNr=0; ButtonNr<NrOfButtons; ButtonNr++)
        if (Combo & (1<<ButtonNr))
        {
          State ^= Button[ButtonNr];
          NrOfBits++;
        }
      if (State == Target)
        if ((MinNrOfBits == -1) || (NrOfBits < MinNrOfBits))
          MinNrOfBits = NrOfBits;
    } /* for (Combo) */

    Sum += MinNrOfBits;

/* Debug */
if (1)  printf("  Adding %d presses to get at Sum=%d..\n", MinNrOfBits, Sum);

  } /* while (fgets) */

  /****************/
  /* Finalisation */
  /****************/
  printf("After %d InputLines, found Sum %d\n", InputLineNr, Sum);
  return 0;
}
