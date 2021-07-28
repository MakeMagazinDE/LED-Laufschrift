/****************************************************************************************/
/* Led.ino                                                                              */
/* Collection of led routines                                                           */
/*                                                                                      */
/* SUB_SHIFT_DATA_IN:      Callback handler to shift data into the display              */
/* SUB_LED_SCROLL:         Subroutine for text scrolling                                */
/* SUB_LED_PRINT:          Subroutine for static text display                           */
/* SUB_REPLACE_CHAR:       Subroutine to replace characters in font                     */
/* SUB_LED_TEST:           Tests the display until test is stopped                      */
/*                                                                                      */
/****************************************************************************************/


/****************************************************************************************/
/****************************************************************************************/
/* LED display routines                                                                 */
/****************************************************************************************/
/****************************************************************************************/


/****************************************************************************************/
/* Subroutine callback handler to shift data into the display                           */
/****************************************************************************************/
uint8_t SUB_SHIFT_DATA_IN(uint8_t dev, MD_MAX72XX::transformType_t t)
{
  static char     *STRING_POINTER;                /* String pointer for actual display position */
  static uint8_t  CHAR_BUFFER[8];                 /* Buffer for char */
         uint8_t  COLUMN_DATA = 0;                /* Column data value */
  static uint16_t CURRENT_LENGTH;                 /* Actual length */
  static uint16_t SHOWN_LENGTH;                   /* Length to be shown */
  static uint16_t SHOWN_COLUMN;                   /* Number of columns already diaplayed */ 
  /* Switch states for state machine */                           
  static enum { S_IDLE, S_NEXT_CHAR, S_SHOW_CHAR, S_SHOW_SPACE, S_SHOW_END} SWITCH_STATE;

  if (WEB_MESSAGE_AVAILABLE == true)              /* New message waiting to be dsplayed? */
  {
    WEB_MESSAGE_AVAILABLE = false;                /* Set message available flag to false */
    mx.clear();                                   /* Clear and reset the display */
    /* Adjust new intensity value */
    mx.control(MD_MAX72XX::INTENSITY, BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);
    /* Adjust font */
    if (OWN_FONT_LIBRARY == true)                 /* Own font selected ? */
    {
      mx.setFont(charset);                        /* Initialize user defined library */    
    }
    else                                          /* Standard font selected */
    {
      mx.setFont(nullptr);                        /* Initialize standard font library */    
    }
    SWITCH_STATE = S_IDLE;                        /* Start state machine with state idle */
  }

  switch (SWITCH_STATE)                           /* State machine to control what to do on callback */
  {
    case S_IDLE:                                  /* Case idle = Reset the message pointer */
      STRING_POINTER = DISPLAY_MESSAGE;           /* Reset the pointer to start of message */
      SWITCH_STATE = S_NEXT_CHAR;                 /* Set switch state to next char */
      SHOWN_COLUMN = 0;                           /* Reset shown columns */
      break;                                      /* Break switch loop */

    case S_NEXT_CHAR:                             /* Display the next character from the font table */                      
      if (*STRING_POINTER == '\0')                /* End of string reached ? */
      {
        SWITCH_STATE = S_IDLE;                    /* Set switch state to idle */     
      }
      else                                        /* End of the string not reached? */
      {
        if (OWN_FONT_LIBRARY == true)             /* Handling of special characters in own library */
        {
          /* Call subroutine to replace chars in font */
          *STRING_POINTER = SUB_REPLACE_CHAR (*STRING_POINTER);   
        }
        /* Calculate length to be shown */
        SHOWN_LENGTH = mx.getChar(*STRING_POINTER++, sizeof(CHAR_BUFFER) / sizeof(CHAR_BUFFER[0]), CHAR_BUFFER);
        CURRENT_LENGTH = 0;                       /* Set current shown length to zero */
        SWITCH_STATE = S_SHOW_CHAR;               /* Set switch state to show char */
      }
      break;                                      /* Break switch loop */

    case S_SHOW_CHAR:                             /* Display the next part of the character */
      COLUMN_DATA = CHAR_BUFFER[CURRENT_LENGTH++];
      SHOWN_COLUMN++;                             /* Increase column counter */
      if (CURRENT_LENGTH < SHOWN_LENGTH)          /* End of character reached ? */
      {
        break;                                    /* Break switch loop */
      }
      /* Define character spacing or blank columns at the end of the message */
      if (*STRING_POINTER != '\0')                /* Not end of the text reached ? */
      {
        SHOWN_LENGTH = CHAR_SPACING;              /* Set spacing between the text */
        CURRENT_LENGTH = 0;
        SWITCH_STATE = S_SHOW_SPACE;              /* Set switch state to show space */
      }
      else                                        /* End of the text reached ? */
      {
        SHOWN_LENGTH = MAX_SEGMENTS*COL_SIZE;     /* One blank screen before new beginning */
        CURRENT_LENGTH = 0;
        SWITCH_STATE = S_SHOW_END;                /* Set switch state to show last spaces */
      }

    case S_SHOW_SPACE:                            /* Display the inter-character spacing = Blank column */
      CURRENT_LENGTH++;                           /* Increase current length */  
      SHOWN_COLUMN++;                             /* Increase column counter */  
      if (CURRENT_LENGTH == SHOWN_LENGTH)         /* Current length = shown length ? */
      {
        SWITCH_STATE = S_NEXT_CHAR;               /* Set switch state to show next char */
      }
      break;                                      /* Break switch loop */
      
    case S_SHOW_END:                              /* Display the inter-character spacing = Blank column */
      CURRENT_LENGTH++;                           /* Increase current length */  
      SHOWN_COLUMN++;                             /* Increase column counter */  
      if (CURRENT_LENGTH == SHOWN_LENGTH)         /* Current length = shown length ? */
      {
        END_SCROLL_MESSAGE = true;                /* Set end scroll message flag true */
        SWITCH_STATE = S_NEXT_CHAR;               /* Set switch state to show next char */
      }
      break;    

    default:                                      /* Default state */
      SWITCH_STATE = S_IDLE;                      /* Set switch state to idle */
  }
  return(COLUMN_DATA);
}


/****************************************************************************************/
/* Subroutine display scrolling text via the transform function                         */
/****************************************************************************************/
void SUB_LED_SCROLL(void)
{
  static uint32_t PREVIOUS_TIME = 0;              /* Previous time value */

  /* Time for scroll the text to the left? */
  if (millis() - PREVIOUS_TIME >= SPEED_LEVELS[SPEED_LEVEL])    
  {
      mx.transform(MD_MAX72XX::TSL);              /* Scroll along --> Address MAX72XX to do so */
      PREVIOUS_TIME = millis();                   /* Starting point for next scrolling time */
  }
}


/****************************************************************************************/
/* Subroutine to display static text                                                    */
/* Message area is padded with blank TOTAL_COLUMNSumns after printing.                  */
/****************************************************************************************/
void SUB_LED_PRINT(void)
{
  char      *STRING_POINTER;                      /* String pointer for actual display position */
  uint8_t   CHAR_BUFFER[8];                       /* Buffer for char */
  uint8_t   CURRENT_LENGTH;                       /* Actual length */
  uint16_t  CHARACTER_LENGTH;                     /* Length of a character */
  int16_t   TOTAL_COLUMNS;                        /* Number of total columns */
  /* Switch states for state machine */ 
  enum { S_NEXT_CHAR, S_SHOW_CHAR, S_SHOW_SPACE } SWITCH_STATE;

  /*Switch display off */
  mx.control(0, MAX_SEGMENTS-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  WEB_MESSAGE_AVAILABLE = false;                  /* Set message available to false */ 
  /* Adjust new intensity value */
  mx.control(MD_MAX72XX::INTENSITY, BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);
  /* Adjust font */
  if (OWN_FONT_LIBRARY == true)                   /* Own font selected ? */
  {
    mx.setFont(charset);                          /* Initialize user defined library */    
  }
  else                                            /* Standard font selected */
  {
    mx.setFont(nullptr);                          /* Initialize standard font library */    
  }
  STRING_POINTER = DISPLAY_MESSAGE;               /* Reset the pointer to start of message */
  SWITCH_STATE = S_NEXT_CHAR; 
  /* Set switch to next state */
  TOTAL_COLUMNS = ((MAX_SEGMENTS) * COL_SIZE) - 1;/* Determinate the number of colums for the dispaly */

  do                                              /* Loop until the number of available columns is filled up with characters */
  {
    switch(SWITCH_STATE)                          /* State machine to control what to do */
    {
      case S_NEXT_CHAR:                           /* Load the next character from the font table */
        if (*STRING_POINTER == '\0')              /* End of string reached ? */
        {
          CHARACTER_LENGTH = TOTAL_COLUMNS + 2;   /* Calculate remaining columns to fill up with blanks */
          CURRENT_LENGTH = 0;
          SWITCH_STATE = S_SHOW_SPACE;            /* Set switch state to display spaces */
          break;
        }
        if (OWN_FONT_LIBRARY == true)             /* Handling of special characters in own library */
        {
          /* Call subroutine to replace chars in font */
          *STRING_POINTER = SUB_REPLACE_CHAR (*STRING_POINTER); 
        }
        /* Determine the number of colums to display the character */
        CHARACTER_LENGTH = mx.getChar(*STRING_POINTER++, sizeof(CHAR_BUFFER) / sizeof(CHAR_BUFFER[0]), CHAR_BUFFER);

        CURRENT_LENGTH = 0;
        SWITCH_STATE = S_SHOW_CHAR;               /* Set switch state to display char */
        break;                                    /* Break switch loop */

      case S_SHOW_CHAR:                           /* Display the actual char */
        /* Set column value */
        mx.setColumn(TOTAL_COLUMNS--, CHAR_BUFFER[CURRENT_LENGTH++]);
        if (CURRENT_LENGTH == CHARACTER_LENGTH)   /* Done with font character, now prepare to display the space between chars */
        {
          CHARACTER_LENGTH = CHAR_SPACING;        /* Adjust char spacing */
          CURRENT_LENGTH = 0;
          SWITCH_STATE = S_SHOW_SPACE;            /* Set switch state to display spaces */
        }
        break;                                    /* Break switch loop */

      case S_SHOW_SPACE:                          /* Display inter-character spacing or end of message padding */
        mx.setColumn(TOTAL_COLUMNS--, 0);         /* Set column value to blank */
        CURRENT_LENGTH++;
        if (CURRENT_LENGTH == CHARACTER_LENGTH)   /* Space inserted --> Load next character */
        {
          SWITCH_STATE = S_NEXT_CHAR;             /* Set switch state to display next char */
        }
        break;                                    /* Break switch loop */

      default:                                    /* Default state */
        TOTAL_COLUMNS = -1;                       /* This definitely ends the do loop */
    }
  } 
  while (TOTAL_COLUMNS >= 0);                     /* Loop until the number of available columns is filled up with characters */

  /* Switch display on to display the text */
  mx.control(0, MAX_SEGMENTS-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  
  if (INV_EFFECT == true)                         /* Inverting display effect selected ? */
  {
    mx.transform(0, 7, MD_MAX72XX::TINV);         /* Invert Display */ 
  }
}


/****************************************************************************************/
/* Subroutine to replace characters in own font                                         */
/****************************************************************************************/
char SUB_REPLACE_CHAR(char TEST_CHAR)
{
  switch (TEST_CHAR)
  {     
    case 252:                                     /* ASCII code 252 = ü ? */
      TEST_CHAR = 129;
      break;                                      /* Break switch loop */
      
    case 228:                                     /* ASCII code 228 = ä ? */
      TEST_CHAR = 132;
       break;                                     /* Break switch loop */
       
    case 246:                                     /* ASCII code 246 = ö ? */
      TEST_CHAR = 148;
      break;                                      /* Break switch loop */

    case 220:                                     /* ASCII code 220 = Ü ? */
      TEST_CHAR = 146;
      break;                                      /* Break switch loop */

    case 196:                                     /* ASCII code 196 = Ä ? */
      TEST_CHAR = 141;
      break;                                      /* Break switch loop */

    case 214:                                     /* ASCII code 214 = Ö ? */
      TEST_CHAR = 145;
      break;                                      /* Break switch loop */

    case 223:                                     /* ASCII code 223 = ß ? */
      TEST_CHAR = 147;
      break;                                      /* Break switch loop */
   
    case 128:                                     /* ASCII code 128 = € ? */
      TEST_CHAR = 150;
      break;                                      /* Break switch loop */
          
    case 247:                                     /* ASCII code 247 = Divide ? */
      TEST_CHAR = 154;
      break;                                      /* Break switch loop */
  }  
  return (TEST_CHAR);                             /* Return new char */
}


/****************************************************************************************/
/* Subroutine to display a test sequence as long as the display test is activated       */
/****************************************************************************************/
void SUB_LED_TEST(void)
{
  int EFFECT_COUNTER;                             /* Loop counter to different effects */
  boolean INVERTED_FLAG;                          /* Inverting test control */
  boolean OLD_SCROLLING_TEXT;                     /* Save scrolling flag */   
  boolean OLD_OWN_FONT_LIBRARY;                   /* Save font flag */
  boolean OLD_INV_EFFECT;                         /* Save inverting effect */
  byte OLD_CHAR_SPACING;                          /* Save char spacing value */
  byte OLD_MESSAGE_TYPE;                          /* Save message type */
  byte OLD_BRIGHTNESS_LEVEL;                      /* Save brightness level */
  byte OLD_SPEED_LEVEL;                           /* Save speed level */
  String OLD_WEB_PAGE_STRING = "";                /* Save webpage sring */

  #ifdef _DEBUG_
    Serial.println ();
    Serial.println ("Display test sequence...");
  # endif
  OLD_SCROLLING_TEXT   = SCROLLING_TEXT;          /* Save scolling text flag */
  OLD_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;        /* Save font library flag */
  OLD_INV_EFFECT       = INV_EFFECT;              /* Save inverting effect flag */
  OLD_CHAR_SPACING     = CHAR_SPACING;            /* Save char spacing value */
  OLD_MESSAGE_TYPE     = MESSAGE_TYPE;            /* Save message type */
  OLD_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;        /* Save brightness level */  
  OLD_SPEED_LEVEL      = SPEED_LEVEL;             /* Save speed level */
  OLD_WEB_PAGE_STRING  = WEB_PAGE_STRING;         /* Save webpage string */
  mx.clear();                                     /* Clear and reset the display */
  INV_EFFECT = false;                             /* No inverting effect */
 
  do                                              /* Repeat test until dsplay test is disabled */
  {
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 1: Own font, Spacing 1, alternating brightness levels");
      WEB_PAGE_STRING = "Font 1 test";
      WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
      OWN_FONT_LIBRARY = true;                    /* Set own library flag true */
      CHAR_SPACING = 1;                           /* Set spacing between chars to one */
      for(BRIGHTNESS_LEVEL=0; BRIGHTNESS_LEVEL <= 4; BRIGHTNESS_LEVEL++)
      {
        WEB_MESSAGE_AVAILABLE = true;             /* Set new message available true */
        SUB_LED_PRINT();                          /* Call subroutine to display text */
        delay (1000);                             /* Delay 1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 2: Standard font, Spacing 1, alternating brightness levels");
      WEB_PAGE_STRING = "Font 2 test";
      WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
      OWN_FONT_LIBRARY = false;                   /* Set own library flag false */
      for(BRIGHTNESS_LEVEL=0; BRIGHTNESS_LEVEL <= 4; BRIGHTNESS_LEVEL++)
      {
        WEB_MESSAGE_AVAILABLE = true;             /* Set new message available true */
        SUB_LED_PRINT();                          /* Call subroutine to display text */
        delay (1000);                             /* Delay 1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    } 
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 3: Own font, alternating spacing, moderate brightness levels");
      WEB_PAGE_STRING = "Spacing";
      WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
      OWN_FONT_LIBRARY = true;                    /* Set own library flag true */
      BRIGHTNESS_LEVEL = 2;                       /* Set brightness to moderate brightness */
      for(CHAR_SPACING=1; CHAR_SPACING <= 6; CHAR_SPACING++)
      {
        WEB_MESSAGE_AVAILABLE = true;             /* Set new message available true */
        SUB_LED_PRINT();                          /* Call subroutine to display text */
        delay (1000);                             /* Delay 1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    } 
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 4: Special characters own font, Spacing 1, alternating brightness levels");
      DISPLAY_MESSAGE [0] = char(228);            /* Special char ä */
      DISPLAY_MESSAGE [1] = char(246);            /* Special char ö */
      DISPLAY_MESSAGE [2] = char(252);            /* Special char ü */
      DISPLAY_MESSAGE [3] = char(223);            /* Special char ß */
      DISPLAY_MESSAGE [4] = char(196);            /* Special char Ä */
      DISPLAY_MESSAGE [5] = char(214);            /* Special char Ö */
      DISPLAY_MESSAGE [6] = char(220);            /* Special char Ü */
      DISPLAY_MESSAGE [7] = char(128);            /* Special char € */
      DISPLAY_MESSAGE [8] = char(247);            /* Special char Divide */
      DISPLAY_MESSAGE [9] = ':';                  /* Special char : */
      DISPLAY_MESSAGE [10] = '?';                 /* Special char ? */
      DISPLAY_MESSAGE [11] = ',';                 /* Special char , */
      DISPLAY_MESSAGE [12] = '!';                 /* Special char ! */
      DISPLAY_MESSAGE [13] = '\0';                /* End of string */
      OWN_FONT_LIBRARY = true;                    /* Set own library flag true */
      CHAR_SPACING = 1;                           /* Set spacing between chars to one */
      for(BRIGHTNESS_LEVEL=0; BRIGHTNESS_LEVEL <= 4; BRIGHTNESS_LEVEL++)
      {
        WEB_MESSAGE_AVAILABLE = true;             /* Set new message available true */
        SUB_LED_PRINT();                          /* Call subroutine to display text */
        delay (1000);                             /* Delay 1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    } 
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 5: Inverting effect");
      WEB_PAGE_STRING = "Inverting";
      WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
      SUB_LED_PRINT();                            /* Call subroutine to display text */
      INVERTED_FLAG = false;                      /* Set inverted false */
      delay (700);                                /* Delay 0,7 sec */
      BRIGHTNESS_LEVEL = 3;                       /* Set brightness to high brightness */
      for(EFFECT_COUNTER=0; EFFECT_COUNTER <= 5; EFFECT_COUNTER++)
      {
        if (INVERTED_FLAG == false)               /* Not inverted ? */
        {
          INVERTED_FLAG = true;                   /* Set inverted true */
          mx.transform(0, 7, MD_MAX72XX::TINV);   /* Invert Display */
        }
        else                                      /* Inverted ? */
        {
          INVERTED_FLAG = false;                  /* Set inverted false */
          mx.transform(0, 7, MD_MAX72XX::TINV);   /* Invert Display */
        }      
        delay (1000);                             /* Delay 1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    }                                  
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 6: Scrolling text, Own font, Spacing 1, alternating brightness levels ");
      /* Define scrolling text messge */ 
      WEB_PAGE_STRING = "This is a scrolling test !!! ";              
      WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS); 
      BRIGHTNESS_LEVEL = 0;                       /* Set brightness to lowest brightness */  
      SPEED_LEVEL = 2;                            /* Set message speed to 2 = Moderate speed scroll */
      OWN_FONT_LIBRARY = true;                    /* Set own library flag true */
      CHAR_SPACING = 1;                           /* Set spacing between chars to one */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      BRIGHTNESS_LEVEL = 2;                       /* Set brightness to modest brightness */  
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      BRIGHTNESS_LEVEL = 4;                       /* Set brightness to highest brightness */  
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    } 
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 7: Scrolling text, Own font, Spacing 3, modest brightness levels ");
      BRIGHTNESS_LEVEL = 2;                       /* Set brightness to modest brightness */   
      SPEED_LEVEL = 2;                            /* Set message speed to 2 = Moderate speed scroll */
      CHAR_SPACING = 3;                           /* Set spacing between chars to one */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    } 
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      Serial.println ("  Teststep 8: Scrolling text, Own font, Spacing 1, different speeds "); 
      BRIGHTNESS_LEVEL = 2;                       /* Set brightness to modest brightness */   
      SPEED_LEVEL = 0;                            /* Set message speed to 0 = Fast */
      CHAR_SPACING = 1;                           /* Set spacing between chars to one */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */ 
    {   
      SPEED_LEVEL = 1;                            /* Set message speed to 1 = Quite fast */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */ 
    {   
      SPEED_LEVEL = 2;                            /* Set message speed to 2 = Moderate speed scroll */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */ 
    {           
      SPEED_LEVEL = 3;                            /* Set message speed to 3 = Quite slow */
      WEB_MESSAGE_AVAILABLE = true;               /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */ 
    {     
      SPEED_LEVEL = 4;                            /* Set message speed to 4 = Slow */
      WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
      END_SCROLL_MESSAGE = false;                 /* Set end scroll message false */
      do                                          /* Repeat until scrolling is done */
      {
        SUB_LED_SCROLL();                         /* Update Led display scrolling regulary */
        delay (10);                               /* 10 ms delay */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */   
        }
      } 
      while (END_SCROLL_MESSAGE == false);  
    }
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */ 
    {
      Serial.println ("  Teststep 9: Segment test");
      mx.clear();                                 /* Clear and reset the display */
      BRIGHTNESS_LEVEL = 2;                       /* Set brightness to modest brightness */ 
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 0; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 255);        /* Set column value all segments on */                     
        delay (100);                              /* Delay 0,1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    }
    delay (500);                                  /* Delay 0,5 sec */
    if (DISPLAY_TEST == true)                     /* Still in display test mode ? */
    {
      for(EFFECT_COUNTER=0; EFFECT_COUNTER <= MAX_SEGMENTS * 8; EFFECT_COUNTER++)
      {
        mx.setColumn(EFFECT_COUNTER, 0);          /* Set column value all segments off */                     
        delay (100);                              /* Delay 0,1 sec */
        if (DISPLAY_TEST == false)                /* Not anymore in test mode ? */
        {
          break;                                  /* Break for loop */
        }
      }
    }  
    if (DISPLAY_TEST == true)                      /* Still in display test mode ? */
    {
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 1);          /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 3);          /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 7);          /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 15);         /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 31);         /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 63);         /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 127);        /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }      
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 127);        /* Set column value */ 
      }
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 255);        /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 127);        /* Set column value */ 
      }        
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 63);         /* Set column value */ 
      }
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 31);         /* Set column value */ 
      }
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 15);         /* Set column value */ 
      }
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }      
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 7);          /* Set column value */ 
      }   
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 3);          /* Set column value */ 
      }           
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 1);          /* Set column value */ 
      }                    
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      }   
      for(EFFECT_COUNTER=MAX_SEGMENTS * 8; EFFECT_COUNTER >= 1; EFFECT_COUNTER--)
      {
        mx.setColumn(EFFECT_COUNTER, 0);          /* Set column value */ 
      }      
      delay (500);                                /* Delay 0,5 sec */
      if (DISPLAY_TEST == false)                  /* Not anymore in test mode ? */
      {
        break;                                    /* Break for loop */
      } 
    }
  }  
  while (DISPLAY_TEST == true);                   /* Repeat test until dsplay test is disabled */
  SCROLLING_TEXT   = OLD_SCROLLING_TEXT;          /* Restore scolling text flag */
  OWN_FONT_LIBRARY = OLD_OWN_FONT_LIBRARY;        /* Restore font library flag */
  INV_EFFECT       = OLD_INV_EFFECT;              /* Restore inverting effect flag */
  CHAR_SPACING     = OLD_CHAR_SPACING;            /* Restore char spacing value */
  MESSAGE_TYPE     = OLD_MESSAGE_TYPE;            /* Restore message type */
  BRIGHTNESS_LEVEL = OLD_BRIGHTNESS_LEVEL;        /* Restore brightness level */
  /* Adjust new intensity value */
  mx.control(MD_MAX72XX::INTENSITY, BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);  
  SPEED_LEVEL      = OLD_SPEED_LEVEL;             /* Restore speed level */
  WEB_PAGE_STRING  = OLD_WEB_PAGE_STRING;         /* Restore webpage string */
  SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
  mx.clear();                                     /* Clear and reset the display */ 
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */                                  
}  
