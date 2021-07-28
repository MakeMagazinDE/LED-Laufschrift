/****************************************************************************************/
/* Web.ino                                                                              */
/* Collection of subroutines to display and process webpages                            */
/* SUB_DECODE_MESSAGE:      Subroutine to decode client request other then webpages     */
/* SUB_DEFINE_ROUTES:       Subroutine to define the routes for ESPAsyncWebserver       */
/*                          to be once during setup                                     */
/* SUB_MESSAGE_ERROR:       Subroutine to be called if message error is detected        */   
/* SUB_WEB_PROCESSOR_MAIN:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */      
/* SUB_WEB_PROCESSOR_SUB1:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */ 
/* SUB_WEB_PROCESSOR_SUB2:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */
/* SUB_WEB_PROCESSOR_SUB3:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */
/* SUB_WEB_PROCESSOR_SUB4:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */
/* SUB_WEB_PROCESSOR_SUB5:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */
/* SUB_WEB_PROCESSOR_SUB6:  Subroutine to replace placeholder in Websites               */
/*                          with calculated values                                      */
/****************************************************************************************/


/****************************************************************************************/
/* Subroutine to decode client request                                                  */
/* Decodes the message string and return a function code to display different results:  */
/* Result code 0 = Send OK message                                                      */
/* Result code 1 = Access denied error message                                          */
/* Result code 2 = Command error message                                                */
/* Result code 3 = Parameter error message                                              */
/* Result code 4 = Send RECEIVE_BUFEFR                                                  */
/****************************************************************************************/
byte SUB_DECODE_MESSAGE()
{  
  int TEST_INT;                                   /* Test Integer value for string conversion*/
  int TEST_POS_1;                                 /* String test position 1 */
  int TEST_POS_2;                                 /* String test position 2 */
    
  #ifdef _DEBUG_
    Serial.print ("  Received message: ");
    Serial.println (RECEIVE_MESSAGE); 
  #endif 

  /* Step 1: Decode login and PW */
  TEMP_BUFFER = RECEIVE_MESSAGE.substring (0, 15);  
  #ifdef _DEBUG_ 
    Serial.print ("    Login and password: ");
    Serial.println (TEMP_BUFFER); 
  #endif    
  if (TEMP_BUFFER != LOGIN)                       /* Extracted substring is equal to login and password ? */     
  {
    #ifdef _DEBUG_
      Serial.println ("  Access denied error!!!");
      Serial.println ();
    #endif 
    return (1);                                   /* Set answer to 1 = Access denied error */  
  } 

  /*Step 2: Extract and handle command */
  TEMP_BUFFER = RECEIVE_MESSAGE.substring (15, 17);  
  #ifdef _DEBUG_ 
    Serial.print ("    Command: ");
    Serial.println (TEMP_BUFFER); 
  #endif 

    
  /**************************************************************************************/
  /*                       Command handler                                              */
  /**************************************************************************************/
  /* Command is ping ? */
   if (TEMP_BUFFER == "?p")                       /* Ping command ? */
  {     
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/?p                               */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          /?p = Ping                                                              */
    /*  Message feedback: OK                                                            */
    /*  Example: http://192.168.122.2/?login=admin:default/?p                           */
    #ifdef _DEBUG_ 
      Serial.println ("  Received ping request --> Answer OK");
      Serial.println (); 
    #endif
    return (0);                                   /* Set answer to 0 = OK message */   
  }

  /* Command is bootloader ? */
  if (TEMP_BUFFER == "b=")                        /* Bootloader command ? */
  {      
    /*  Message format for set bootloader                                               */
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/b=x                              */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          b=0       Normal boot --> Nothing will happen                           */
    /*          b=1       Bootloader via WLAN                                           */
    /*  Example: http://192.168.122.2/?login=admin:default/b=1                          */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (17, 18); 
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    #ifdef _DEBUG_ 
      Serial.print ("Received bootloader command b=");
      Serial.print (TEST_INT); 
    #endif 
    if ((TEST_INT < 0) || (TEST_INT > 1))         /* Out of range ? */
    {
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      if (TEST_INT == 1)                          /* Bootloader selected ? */
      {
        BOOTLOADER_FLAG = true;                   /* Set bootloader flag true */                
      }                  
      #ifdef _DEBUG_
        Serial.println (" --> Answer OK");
        Serial.println ();
      #endif  
      return (0);                                 /* Set answer to 0 = OK message */ 
    }
  }

  /* Command is reset ? */
  if (TEMP_BUFFER == "r=")                        /* Reset command ? */
  {      
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/r=x                              */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          r=0       No reset --> Nothing will happen                              */
    /*          r=1       Reset without saving                                          */
    /*  Example: http://192.168.122.2/?login=admin:default/r=1                          */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (17, 18); 
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    #ifdef _DEBUG_ 
      Serial.print ("Received reset command r=");
      Serial.print (TEST_INT); 
    #endif 
    if ((TEST_INT < 0) || (TEST_INT > 1))         /* Out of range ? */
    {
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      if (TEST_INT == 1)                          /* Reset selected ? */
      {
        RESET_FLAG = true;                        /* Set reset flag true */                
      }                  
      #ifdef _DEBUG_
        Serial.println (" --> Answer OK");
        Serial.println ();
      #endif  
      return (0);                                 /* Set answer to 0 = OK message */ 
    }
  }

  /* Command is display test ? */
  if (TEMP_BUFFER == "d=")                        /* Display test command ? */
  {      
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/d=x                              */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          d=0       Stop dispaly test                                             */
    /*          d=1       Start display test                                            */
    /*  Example: http://192.168.122.2/?login=admin:default/d=1                          */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (17, 18); 
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    #ifdef _DEBUG_ 
      Serial.print ("Received display test command d=");
      Serial.print (TEST_INT); 
    #endif 
    if ((TEST_INT < 0) || (TEST_INT > 1))         /* Out of range ? */
    {
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      if (TEST_INT == 1)                          /* Display test on selected ? */
      {
        DISPLAY_TEST  = true;                     /* Display test flag true */                
      }  
      else                                        /* Display test off selected ? */  
      {
        DISPLAY_TEST  = false;                    /* Display test flag false */                
      }                               
      #ifdef _DEBUG_
        Serial.println (" --> Answer OK");
        Serial.println ();
      #endif  
      return (0);                                 /* Set answer to 0 = OK message */ 
    }
  }

  /* Command is display static text ? */
  if (TEMP_BUFFER == "t=")                        /* Display static test command ? */
  {      
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/t=Text;b=Brightness;f=Font;      */
    /*                                                   c=Char spacing,e=Effect        */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          t         Text string                                                   */
    /*          b=0-4     Brightness level (0-4), 0 = Lowest, 4 = Highest               */
    /*          f=1-2     Font (1-2), 1 = Own font, 2 = Standard font                   */
    /*          c=1-5     Char spacing (1-5), 1-5 Char spacings                         */
    /*          e=1-2     Effect (1-2), 1 = No inverting, 2 = Inverting                 */
    /*  Example: http://192.168.122.2/?login=admin:default/t=Test;b=2;f=1;c=1;e=1       */
    #ifdef _DEBUG_ 
      Serial.print ("Received display static text command t=");
    #endif 
    /* Step 1: Extract text string --> Search for first ; */      
    TEST_POS_1 = 17;                             /* Set test position to start of text */
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < 19)                         /* No text found ? */
    {
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif   
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else
    { 
      /* Temporarily save the text string */
      ORIG_WEB_PAGE_STRING = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
      #ifdef _DEBUG_
        Serial.println ();
        Serial.print ("  Text message set to: ");             
        Serial.println (ORIG_WEB_PAGE_STRING);
      # endif    
    }
    /* Step 2: Extract brightness level --> Search for second ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "b=")                      /* Brightness header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */        
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of brightness value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 0) || (TEST_INT > 4))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */ 
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      /* Temporarily save the brightness level */
      ORIG_BRIGHTNESS_LEVEL = TEST_INT;     
      #ifdef _DEBUG_
        Serial.print ("  Message brightness set to: ");             
        Serial.print (BRIGHTNESS_LEVELS[ORIG_BRIGHTNESS_LEVEL]);
        Serial.println (" (2 = Lowest, 7, 10, 13, 15 = Brightest)");      
      # endif    
    }
    /* Step 3: Extract font selection --> Search for third ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "f=")                      /* Font selection header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of font selection value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 1) || (TEST_INT > 2))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      #ifdef _DEBUG_
        Serial.print ("  Font set to: ");
      # endif       
      if (TEST_INT == 1)                          /* Own font selected */
      {
        ORIG_OWN_FONT_LIBRARY = true;             /* Set own font library true */
        #ifdef _DEBUG_
          Serial.println ("Font 1: Own font"); 
        # endif        
      }
      else                                        /* Standard font selected */                     
      {
        ORIG_OWN_FONT_LIBRARY = false;            /* Set own font library false */
                #ifdef _DEBUG_
          Serial.println ("Font 2: Standard font"); 
        # endif
      }
    }
    /* Step 4: Extract char spacing --> Search for forth ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "c=")                      /* Char spacing header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of char spacing value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 1) || (TEST_INT > 5))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      /* Temporarily save the char spacing */
      ORIG_CHAR_SPACING = TEST_INT;     
      #ifdef _DEBUG_
        Serial.print ("  Char spacing set to: ");             
        Serial.print (ORIG_CHAR_SPACING);
        Serial.println (" pixels");
      # endif    
    }
    /* Step 5: Extract effect selection --> Search for fifth ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "e=")                      /* Effect selection header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */ 
      ORIG_CHAR_SPACING = CHAR_SPACING;           /* Reset value */    
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of font selection value */    
    TEST_POS_2 = RECEIVE_MESSAGE.length();        /* Determine length of the string */
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */ 
      ORIG_CHAR_SPACING = CHAR_SPACING;           /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 1) || (TEST_INT > 2))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */  
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */ 
      ORIG_CHAR_SPACING = CHAR_SPACING;           /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      #ifdef _DEBUG_
        Serial.print ("  Inverting effect set to: ");
      # endif             
      if(TEST_INT == 1)                           /* No selected ? */
      {
        ORIG_INV_EFFECT = false;                  /* Set inverting effect false */
        #ifdef _DEBUG_
          Serial.println ("Off"); 
        # endif
      }
      else                                        /* Yes selected ? */
      {
        ORIG_INV_EFFECT = true;                   /* Set inverting effect true */
        #ifdef _DEBUG_
          Serial.println ("On"); 
        # endif
      }
    }
    WEB_PAGE_STRING = ORIG_WEB_PAGE_STRING;       /* Copy temporary value in web page string */
    BRIGHTNESS_LEVEL = ORIG_BRIGHTNESS_LEVEL;     /* Copy temporary value in brightness level */
    OWN_FONT_LIBRARY = ORIG_OWN_FONT_LIBRARY;     /* Copy temporary value in own font library */
    CHAR_SPACING = ORIG_CHAR_SPACING;             /* Copy temporary value in char spacing */
    INV_EFFECT = ORIG_INV_EFFECT;                 /* Copy temporary value in inverting effect */
    /* Convert string to char */
    SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS);       
    SCROLLING_TEXT = false;                       /* Set static text */
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    #ifdef _DEBUG_
      Serial.println (" --> Answer OK");
      Serial.println ();
    #endif  
    return (0);                                   /* Set answer to 0 = OK message */
  }

  /* Command is display scrolling text ? */
  if (TEMP_BUFFER == "s=")                        /* Display scrolling test command ? */
  {      
    /*  http://192.168.122.2/?login=<LOGIN>:<PASSWORD>/s=Text;s=Speed;b=Brightness;     */
    /*                                                   f=Font;c=Char spacing,         */
    /*  <LOGIN> = Login Name                                                            */
    /*  <PASSWORD> = Password                                                           */
    /*          s         Scrolling text string                                         */
    /*          e=0-4     Speed level (0-4), 0 = Fastest, 4 = Slowest                   */
    /*          b=0-4     Brightness level (0-4), 0 = Lowest, 4 = Highest               */
    /*          f=1-2     Font (1-2), 1 = Own font, 2 = Standard font                   */
    /*          c=1-5     Char spacing (1-5), 1-5 Char spacings                         */
    /*  Example: http://192.168.122.2/?login=admin:default/s=Test;e=2;b=2;f=1;c=1       */
    #ifdef _DEBUG_ 
      Serial.print ("Received display static text command s=");
    #endif 
    /* Step 1: Extract scrolling text string --> Search for first ; */      
    TEST_POS_1 = 17;                              /* Set scrolling test position to start of text */
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < 19)                          /* No text found ? */
    {
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif   
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else
    { 
      /* Temporarily save the text string */
      ORIG_WEB_PAGE_STRING = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
      #ifdef _DEBUG_
        Serial.println ();
        Serial.print ("  Scolling message set to: ");             
        Serial.println (ORIG_WEB_PAGE_STRING);
      # endif    
    }
    /* Step 2: Extract speed level --> Search for second ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "e=")                      /* Speed header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */        
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of speed value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 0) || (TEST_INT > 4))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */ 
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      /* Temporarily save the speed level */
      ORIG_SPEED_LEVEL = TEST_INT; 
      #ifdef _DEBUG_
        Serial.print ("  Message speed set to: ");             
        Serial.print (SPEED_LEVELS[ORIG_SPEED_LEVEL]);
        Serial.println (" (10 = Fastest, 20, 30, 40, 50 = Slowest)");
      # endif    
    } 
    /* Step 3: Extract brightness level --> Search for third ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "b=")                      /* Brightness header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */ 
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of brightness value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 0) || (TEST_INT > 4))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */ 
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      /* Temporarily save the brightness level */
      ORIG_BRIGHTNESS_LEVEL = TEST_INT;     
      #ifdef _DEBUG_
        Serial.print ("  Message brightness set to: ");             
        Serial.print (BRIGHTNESS_LEVELS[ORIG_BRIGHTNESS_LEVEL]);
        Serial.println (" (2 = Lowest, 7, 10, 13, 15 = Brightest)");      
      # endif    
    }
    
    /* Step 4: Extract font selection --> Search for forth ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "f=")                      /* Font selection header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of font selection value */    
    TEST_POS_2 = RECEIVE_MESSAGE.indexOf (';',TEST_POS_1);
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 1) || (TEST_INT > 2))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      #ifdef _DEBUG_
        Serial.print ("  Font set to: ");
      # endif       
      if (TEST_INT == 1)                          /* Own font selected */
      {
        ORIG_OWN_FONT_LIBRARY = true;             /* Set own font library true */
        #ifdef _DEBUG_
          Serial.println ("Font 1: Own font"); 
        # endif        
      }
      else                                        /* Standard font selected */                     
      {
        ORIG_OWN_FONT_LIBRARY = false;            /* Set own font library false */
                #ifdef _DEBUG_
          Serial.println ("Font 2: Standard font"); 
        # endif
      }
    }
    /* Step 5: Extract char spacing --> Search for fifth ; */      
    TEST_POS_1 = TEST_POS_2+1;                    /* Set test position to start of header text */
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_1+2); 
    if (TEMP_BUFFER != "c=")                      /* Char spacing header not received correctly ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */      
      #ifdef _DEBUG_
        Serial.println (" --> Answer Command Error!!!");
        Serial.println ();
      #endif  
      return (2);                                 /* Set answer to 2 = Comand error */
    }
    TEST_POS_1 = TEST_POS_1 + 2;                  /* Set test position to start of char spacing value */    
    TEST_POS_2 = RECEIVE_MESSAGE.length();        /* Determine length of the string */
    if (TEST_POS_2 < TEST_POS_1+1)                /* No value found ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    TEMP_BUFFER = RECEIVE_MESSAGE.substring (TEST_POS_1, TEST_POS_2);
    TEST_INT = TEMP_BUFFER.toInt();               /* Convert into integer */
    if ((TEST_INT < 1) || (TEST_INT > 5))         /* Out of range ? */
    {
      ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;     /* Reset value */
      ORIG_SPEED_LEVEL = SPEED_LEVEL;             /* Reset value */        
      ORIG_BRIGHTNESS_LEVEL = BRIGHTNESS_LEVEL;   /* Reset value */ 
      ORIG_OWN_FONT_LIBRARY = OWN_FONT_LIBRARY;   /* Reset value */       
      #ifdef _DEBUG_
        Serial.println (" --> Answer Parameter Error!!!");
        Serial.println ();
      #endif  
      return (3);                                 /* Set answer to 3 = Parameter error */  
    }
    else                                          /* In range ? */
    {
      /* Temporarily save the char spacing */
      ORIG_CHAR_SPACING = TEST_INT;     
      #ifdef _DEBUG_
        Serial.print ("  Char spacing set to: ");             
        Serial.print (ORIG_CHAR_SPACING);
        Serial.println (" pixels");
      # endif    
    }
    
    WEB_PAGE_STRING = ORIG_WEB_PAGE_STRING;       /* Copy temporary value in web page string */
    SPEED_LEVEL = ORIG_SPEED_LEVEL;               /* Copy temporary value in speed level */
    BRIGHTNESS_LEVEL = ORIG_BRIGHTNESS_LEVEL;     /* Copy temporary value in brightness level */
    OWN_FONT_LIBRARY = ORIG_OWN_FONT_LIBRARY;     /* Copy temporary value in own font library */
    CHAR_SPACING = ORIG_CHAR_SPACING;             /* Copy temporary value in char spacing */
    /* Convert string to char --> Source string WEB_PAGE_STRING, Target string = DISPLAY_MESSAGE */
    SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS);       
    SCROLLING_TEXT = true;                        /* Set scrolling text */
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    #ifdef _DEBUG_
      Serial.println (" --> Answer OK");
      Serial.println ();
    #endif  
    return (0);                                   /* Set answer to 0 = OK message */
  }
}


/****************************************************************************************/
/* Subroutine to define routres for HTML requests                                       */
/****************************************************************************************/
void SUB_DEFINE_ROUTES (void)
{ 
  /********************* Routes for loading CSS style sheet  ***************************/
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    /* Answer request by opening the style sheet */
    request->send(SPIFFS, "/Style.css", "text/css");
  });

  /********************* Routes for simple web messages  ***************************/
  server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) 
  {
    byte ANSWER_FLAG;                             /* Answer from decode message subroutine. Needs to be defined in function! */
                                
    #ifdef _DEBUG_
        Serial.println ();               
        Serial.println("New client request...");
    #endif 
    if (request->hasParam(KEYWORD))               /* Keyword for single command detected ? */
    {
      /* Save rest of message in receive buffer */
      RECEIVE_MESSAGE = request->getParam(KEYWORD)->value();
      ANSWER_FLAG = SUB_DECODE_MESSAGE();         /* Call subroutine to decode message */
      switch (ANSWER_FLAG)                        /* Send correct response based on returned answer from subroutine */
      {
        case 0:                                   /* 0 = Send OK message */
          request->send(200, "text/plain", "OK"); 
          break;
        case 1:                                   /* 1 = Access denied error */
          request->send(404, "text/plain", "Access denied error"); 
          break;
        case 2:                                   /* 2 = Command error */
          request->send(404, "text/plain", "Command error"); 
          break;
        case 3:                                   /* 3 = Parameter error */
          request->send(404, "text/plain", "Parameter error"); 
          break;            
        case 4:                                   /* 4 = Send receive buffer */
          /* Needs to be be this way to be accepted by Mediola !!! */       
          delay (300);                            /* 300ms delay */
          request->send(200, "text/html", RECEIVE_MESSAGE); 
          delay (300);                            /* 300ms delay */
          break;  
      }
    } 
    else                                          /* No keyword detected ? */ 
    {
      #ifdef _DEBUG_
        Serial.print ("  Message error!!! Keyword ");
        Serial.print (KEYWORD);
        Serial.println (" not detected!");
        Serial.println ();
      #endif  
      request->send(404, "text/plain", "Message error");
    }
  });

  /**************************** Routes for main page: Static text settings **********************************/
  server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Client connected: Display main page...");
    # endif
    DISPLAY_TEST = false;                         /* Set display test false */
    /* Answer request with displaying of main web page */
    request->send(SPIFFS, "/Main.html", String(), false, SUB_WEB_PROCESSOR_MAIN);
  });

  /* Main page: Button for saving of the values pressed */
  server.on("/m1save", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Main menu save button pressed: Saving data and update display...");
    # endif

    /* Check for first parameter: Text */
    if (request->hasParam("MAINSELECT1"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("MAINSELECT1")->value();
      if (RECEIVE_MESSAGE != "")                  /* Not an empty sring ? -->  Prepare received message for display */               
      {
        WEB_PAGE_STRING = RECEIVE_MESSAGE;        /* Copy received string to web page string */ 
      }
      #ifdef _DEBUG_
        Serial.print ("  Text message set to: ");             
        Serial.println (WEB_PAGE_STRING);
      # endif      
    }
   
    /* Check for second parameter: Brightness */
    if (request->hasParam("MAINSELECT2"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("MAINSELECT2")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      BRIGHTNESS_LEVEL = TEST_INT;                /* Copy int value to brightness level byte */   
      #ifdef _DEBUG_
        Serial.print ("  Message brightness set to: ");             
        Serial.print (BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);
        Serial.println (" (2 = Lowest, 7, 10, 13, 15 = Brightest)");
      # endif
    }
    
    /* Check for third parameter: Font */
    if (request->hasParam("MAINSELECT3"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("MAINSELECT3")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Font set to: ");
      # endif             
      if(TEST_INT == 1)                           /* Yes selected ? */
      {
        OWN_FONT_LIBRARY = true;                  /* Set own library true */
        #ifdef _DEBUG_
          Serial.println ("Font 1: Own font"); 
        # endif
      }
      else                                        /* No selected ? */
      {
        OWN_FONT_LIBRARY = false;                 /* Set own library false */
        #ifdef _DEBUG_
          Serial.println ("Font 2: Standard font"); 
        # endif
      }
    } 

    /* Check for forth parameter: Char spacing */
    if (request->hasParam("MAINSELECT4"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("MAINSELECT4")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      CHAR_SPACING = TEST_INT+1;                  /* Copy int value to char spacing byte */   
      #ifdef _DEBUG_
        Serial.print ("  Char spacing set to: ");             
        Serial.print (CHAR_SPACING);
        Serial.println (" pixels");
      # endif
    }

    /* Check for fifth parameter: Effect */
    if (request->hasParam("MAINSELECT5"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("MAINSELECT5")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Inverting effect set to: ");
      # endif             
      if(TEST_INT == 1)                           /* No selected ? */
      {
        INV_EFFECT = false;                       /* Set inverting effect false */
        #ifdef _DEBUG_
          Serial.println ("Off"); 
        # endif
      }
      else                                        /* Yes selected ? */
      {
        INV_EFFECT = true;                        /* Set inverting effect true */
        #ifdef _DEBUG_
          Serial.println ("On"); 
        # endif
      }
    }     
    /* Answer request with re-displaying of main menu */
    request->send(SPIFFS, "/Main.html", String(), false, SUB_WEB_PROCESSOR_MAIN);
    
    /* Convert string to char */
    SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS);       
    SCROLLING_TEXT = false;                       /* Set static text */
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
  });

  /************************** Routes for submenu page 1: Scrolling text settings *******************************/
   server.on("/submenu1", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 1 page...");
    #endif 
    DISPLAY_TEST = false;                         /* Set display test false */ 
    /* Answer request with displaying of submenu page 1 web page */
    request->send(SPIFFS, "/Submenu1.html", String(), false, SUB_WEB_PROCESSOR_SUBM1);
  }); 

  /* Submenu 1: Button for saving of the values pressed */
  server.on("/store1", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 1 save button pressed: Saving data and update display...");
    # endif

    /* Check for first parameter: Text */
    if (request->hasParam("SUB1SELECT1"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT1")->value();
      if (RECEIVE_MESSAGE != "")                  /* Not an empty sring ? -->  Prepare received message for display */               
      {
        WEB_PAGE_STRING = RECEIVE_MESSAGE;        /* Copy received string to web page string */
      }
    }
   
    /* Check for second parameter: Brightness */
    if (request->hasParam("SUB1SELECT2"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT2")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      BRIGHTNESS_LEVEL = TEST_INT;                /* Copy int value to brightness level byte */   
      #ifdef _DEBUG_
        Serial.print ("  Message brightness set to: ");             
        Serial.print (BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);
        Serial.println (" (2 = Lowest, 7, 10, 13, 15 = Brightest)");
      # endif
    }

    /* Check for third parameter: Speed */
    if (request->hasParam("SUB1SELECT3"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT3")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      SPEED_LEVEL = TEST_INT;                     /* Copy int value to speedlevel byte */   
      #ifdef _DEBUG_
        Serial.print ("  Message speed set to: ");             
        Serial.print (SPEED_LEVELS[SPEED_LEVEL]);
        Serial.println (" (10 = Fastest, 20, 30, 40, 50 = Slowest)");
      # endif
    }    
    
    /* Check for forth parameter: Font */
    if (request->hasParam("SUB1SELECT4"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT4")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Font set to: ");
      # endif             
      if(TEST_INT == 1)                           /* Yes selected ? */
      {
        OWN_FONT_LIBRARY = true;                  /* Set own library true */
        #ifdef _DEBUG_
          Serial.println ("Font 1: Own font"); 
        # endif
      }
      else                                        /* No selected ? */
      {
        OWN_FONT_LIBRARY = false;                 /* Set own library false */
        #ifdef _DEBUG_
          Serial.println ("Font 2: Standard font"); 
        # endif
      }
    } 

    /* Check for fifth parameter: Char spacing */
    if (request->hasParam("SUB1SELECT5"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT5")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      CHAR_SPACING = TEST_INT+1;                  /* Copy int value to char spacing byte */   
      #ifdef _DEBUG_
        Serial.print ("  Char spacing set to: ");             
        Serial.print (CHAR_SPACING);
        Serial.println (" pixels");
      # endif
    }

    /* Check for sixth parameter: Text type */
    if (request->hasParam("SUB1SELECT6"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB1SELECT6")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      MESSAGE_TYPE = TEST_INT;                    /* Copy int value to message type byte */ 
      #ifdef _DEBUG_
        Serial.print ("  Texttype set to: ");
      # endif             
      if(MESSAGE_TYPE == 1)                       /* Textmessage selected ? */
      {
        #ifdef _DEBUG_
          Serial.print ("Text message, text set to: "); 
          Serial.println (WEB_PAGE_STRING);
        # endif
      }
      else                                        /* Testmessage selected ? */
      {
        /* Assemble test string */
        if (STA_BOOT_MODE == true)                /* Station mode selected ? */
        {
          WEB_PAGE_STRING = "Open website at "; 
          WEB_PAGE_STRING += STA_FIXED_IP[0];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += STA_FIXED_IP[1];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += STA_FIXED_IP[2];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += STA_FIXED_IP[3];
          WEB_PAGE_STRING += "/main with a browser!";
        }
        else                                      /* Access point mode selected ? */
        {
          WEB_PAGE_STRING = "Connect to WLAN "; 
          WEB_PAGE_STRING += AP_WLAN_SSID;
          WEB_PAGE_STRING += ", then open website at "; 
          WEB_PAGE_STRING += AP_FIXED_IP[0];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += AP_FIXED_IP[1];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += AP_FIXED_IP[2];
          WEB_PAGE_STRING += ".";
          WEB_PAGE_STRING += AP_FIXED_IP[3];
          WEB_PAGE_STRING += "/main with a browser!";
        }
        #ifdef _DEBUG_
          Serial.print ("Test message, text set to: "); 
          Serial.println (WEB_PAGE_STRING);
        # endif
      }
    }     
    /* Answer request with re-displaying of szenario 1 */
    request->send(SPIFFS, "/Submenu1.html", String(), false, SUB_WEB_PROCESSOR_SUBM1);
    
    /* Convert string to char */
    SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS);    
    SCROLLING_TEXT = true;                        /* Set scrolling text true */
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
  });

  /************************** Routes for submenu page 2: General settings *******************************/
   server.on("/submenu2", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 2 page...");
    #endif  
    DISPLAY_TEST = false;                         /* Set display test false */
    /* Answer request with displaying of submenu page 2 web page */
    request->send(SPIFFS, "/Submenu2.html", String(), false, SUB_WEB_PROCESSOR_SUBM2);
  }); 

  /* Submenu 2: Button for saving of the values pressed */
  server.on("/store2", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 2 save button pressed: Saving data and update display...");
    # endif

    /* Check for first parameter: Boot mode */
    if (request->hasParam("SUB2SELECT1"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB2SELECT1")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Boot mode set to: ");
      # endif             
      if(TEST_INT == 1)                           /* Access point selected ? */
      {
        STA_BOOT_MODE = false;                    /* Set station boot mode false */
        #ifdef _DEBUG_
          Serial.println ("Accesss Point (AP)");
        # endif 
      }
      else                                        /* No selected ? */
      {
        STA_BOOT_MODE = true;                     /* Set station boot mode true */
        #ifdef _DEBUG_
          Serial.println ("Station Mode (STA)");
        # endif 
      }
    } 
   
    /* Check for second parameter: Autostart */
    if (request->hasParam("SUB2SELECT2"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB2SELECT2")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Autostart mode to display text message after boot set to: ");
      # endif           
      if(TEST_INT == 1)                           /* Autostart not selected ? */
      {
        AUTOSTART_FLAG = false;                   /* Set autostart false */
        #ifdef _DEBUG_
          Serial.println ("Off"); 
        # endif
      }
      else                                        /* No selected ? */
      {
        AUTOSTART_FLAG = true;                    /* Set autostart true */
        #ifdef _DEBUG_
          Serial.println ("On"); 
        # endif
      }
    } 

    /* Check for third parameter: Store */
    if (request->hasParam("SUB2SELECT3"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB2SELECT3")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Permanently save date in Init.txt file: ");
      # endif           
      if(TEST_INT == 1)                           /* Saving not selected ? */
      {
        SAVE_FLAG = false;                        /* Set saving flag false */
        REBOOT_SAVE_FLAG = false;                 /* Set reboot save flag false */
        #ifdef _DEBUG_
          Serial.println ("Off"); 
        # endif
      }
      else                                        /* No selected ? */
      {
        SAVE_FLAG = true;                         /* Set saving flag true */
        REBOOT_SAVE_FLAG = true;                  /* Set reboot save flag true */
        #ifdef _DEBUG_
          Serial.println ("On"); 
        # endif
      }
    }

    /* Check for forth parameter: Reset */
    if (request->hasParam("SUB2SELECT4"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB2SELECT4")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Simply reset the device: ");
      # endif           
      if(TEST_INT == 1)                           /* Resetting not selected ? */
      {
        #ifdef _DEBUG_
          Serial.println ("No"); 
        # endif
        REBOOT_FLAG = true;                       /* Set reboot flag true --> Saving plus reset */
      }
      else                                        /* No selected ? */
      {
        #ifdef _DEBUG_
          Serial.println ("Yes"); 
        # endif
        RESET_FLAG = true;                       /* Set reset flag true --> Just reset */
      }
    }
    /* Answer request with displaying reboot menu */
    request->send(SPIFFS, "/Reboot.html");    
  });

/************************** Routes for submenu page 3: AP settings *******************************/
   server.on("/submenu3", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 3 page...");
    #endif  
    DISPLAY_TEST = false;                         /* Set display test false */
    /* Answer request with displaying of submenu page 3 web page */
    request->send(SPIFFS, "/Submenu3.html", String(), false, SUB_WEB_PROCESSOR_SUBM3);
  }); 

  /* Submenu 3: Button for saving of the values pressed */
  server.on("/store3", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 3 save button pressed: Saving data and update display...");
    # endif

    if (request->hasParam("SUB3SSID"))            /* Variable for AP WLAN SSID ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3SSID")->value();
      TEST_INT = RECEIVE_MESSAGE.length();        /* Determine length of the string */
      /* Fill char array with string */
      RECEIVE_MESSAGE.toCharArray(AP_WLAN_SSID, TEST_INT+1);      
      #ifdef _DEBUG_
        Serial.print ("  AP WLAN SSID set to: ");
        Serial.println (AP_WLAN_SSID);
      #endif       
    }  
    if (request->hasParam("SUB3PASS"))            /* Variable for AP WLAN password ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3PASS")->value();
      TEST_INT = RECEIVE_MESSAGE.length();        /* Determine length of the string */
      /* Fill char array with string */
      RECEIVE_MESSAGE.toCharArray(AP_WLAN_PWD, TEST_INT+1);      
      #ifdef _DEBUG_
        Serial.print ("  AP WLAN password set to: ");
        Serial.println (AP_WLAN_PWD);
      #endif       
    }  
    if (request->hasParam("SUB3IPA"))             /* Variable for AP fixed IP-Address 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3IPA")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_FIXED_IP[0] = TEST_INT;                  /* Save 4th digit */  
    }  
    if (request->hasParam("SUB3IPB"))             /* Variable for AP fixed IP-Address 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3IPB")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_FIXED_IP[1] = TEST_INT;                  /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB3IPC"))             /* Variable for AP fixed IP-Address 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3IPC")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_FIXED_IP[2] = TEST_INT;                  /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB3IPD"))             /* Variable for AP fixed IP-Address 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3IPD")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_FIXED_IP[3] = TEST_INT;                  /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  AP fixed IP-Address set to: ");
        Serial.println (AP_FIXED_IP);
      #endif 
    }  
    if (request->hasParam("SUB3SUA"))             /* Variable for AP subnet mask 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3SUA")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_SUBNET[0] = TEST_INT;                    /* Save 4th digit */  
    }  
    if (request->hasParam("SUB3SUB"))             /* Variable for AP subnet mask 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3SUB")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_SUBNET[1] = TEST_INT;                    /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB3SUC"))             /* Variable for AP subnet mask 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3SUC")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_SUBNET[2] = TEST_INT;                    /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB3SUD"))             /* Variable for AP subnet mask 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB3SUD")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      AP_SUBNET[3] = TEST_INT;                    /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  AP subnet mask set to: ");
        Serial.println (AP_SUBNET);
      #endif 
    }
    REBOOT_SAVE_FLAG = false;                     /* Set reboot save flag false */
    REBOOT_FLAG = true;                           /* Set reboot flag true */
    /* Answer request with displaying reboot menu */
    request->send(SPIFFS, "/Reboot.html");    
  }); 

/************************** Routes for submenu page 4: STA settings *******************************/
   server.on("/submenu4", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 4 page...");
    #endif 
    DISPLAY_TEST = false;                         /* Set display test false */ 
    /* Answer request with displaying of submenu page 4 web page */
    request->send(SPIFFS, "/Submenu4.html", String(), false, SUB_WEB_PROCESSOR_SUBM4);
  }); 

  /* Submenu 4: Button for saving of the values pressed */
  server.on("/store4", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 4 save button pressed: Saving data and update display...");
    # endif

    if (request->hasParam("SUB4SSID"))            /* Variable for STA WLAN SSID ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4SSID")->value();
      TEST_INT = RECEIVE_MESSAGE.length();        /* Determine length of the string */
      /* Fill char array with string */
      RECEIVE_MESSAGE.toCharArray(STA_WLAN_SSID, TEST_INT+1);      
      #ifdef _DEBUG_
        Serial.print ("  STA WLAN SSID set to: ");
        Serial.println (STA_WLAN_SSID);
      #endif       
    }  
    if (request->hasParam("SUB4PASS"))            /* Variable for STA WLAN password ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4PASS")->value();
      TEST_INT = RECEIVE_MESSAGE.length();        /* Determine length of the string */
      /* Fill char array with string */
      RECEIVE_MESSAGE.toCharArray(STA_WLAN_PWD, TEST_INT+1);      
      #ifdef _DEBUG_
        Serial.print ("  STA WLAN password set to: ");
        Serial.println (STA_WLAN_PWD);
      #endif       
    } 
    if (request->hasParam("SUB4HOST"))            /* Variable for STA hostname ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4HOST")->value();
      TEST_INT = RECEIVE_MESSAGE.length();        /* Determine length of the string */
      /* Fill char array with string */
      RECEIVE_MESSAGE.toCharArray(STA_HOSTNAME, TEST_INT+1);      
      #ifdef _DEBUG_
        Serial.print ("  STA hostname set to: ");
        Serial.println (STA_HOSTNAME);
      #endif       
    } 
    if (request->hasParam("SUB4IPA"))             /* Variable for STA fixed IP-Address 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4IPA")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_FIXED_IP[0] = TEST_INT;                 /* Save 4th digit */  
    }  
    if (request->hasParam("SUB4IPB"))             /* Variable for STA fixed IP-Address 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4IPB")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_FIXED_IP[1] = TEST_INT;                 /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB4IPC"))             /* Variable for STA fixed IP-Address 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4IPC")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_FIXED_IP[2] = TEST_INT;                 /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB4IPD"))             /* Variable for fixed IP-Address 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4IPD")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_FIXED_IP[3] = TEST_INT;                 /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  STA fixed IP-Address set to: ");
        Serial.println (STA_FIXED_IP);
      #endif 
    }  
    if (request->hasParam("SUB4SUA"))             /* Variable for STA subnet mask 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4SUA")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_SUBNET[0] = TEST_INT;                   /* Save 4th digit */  
    }  
    if (request->hasParam("SUB4SUB"))             /* Variable for STA subnet mask 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4SUB")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_SUBNET[1] = TEST_INT;                   /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB4SUC"))             /* Variable for STA subnet mask 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4SUC")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_SUBNET[2] = TEST_INT;                   /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB4SUD"))             /* Variable for STA subnet mask 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4SUD")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_SUBNET[3] = TEST_INT;                   /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  STA subnet mask set to: ");
        Serial.println (STA_SUBNET);
      #endif 
    } 
    if (request->hasParam("SUB4GWA"))             /* Variable for STA gateway IP 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4GWA")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_GATEWAY[0] = TEST_INT;                  /* Save 4th digit */  
    }  
    if (request->hasParam("SUB4GWB"))             /* Variable for STA gateway IP 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4GWB")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_GATEWAY[1] = TEST_INT;                  /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB4GWC"))             /* Variable for STA gateway IP 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4GWC")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_GATEWAY[2] = TEST_INT;                  /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB4GWD"))             /* Variable for STA gateway IP 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4GWD")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_GATEWAY[3] = TEST_INT;                   /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  STA gateway IP set to: ");
        Serial.println (STA_GATEWAY);
      #endif 
    }
    if (request->hasParam("SUB4D1A"))             /* Variable for STA DNS 1 IP-Address 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D1A")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_1[0] = TEST_INT;                    /* Save 4th digit */  
    }  
    if (request->hasParam("SUB4D1B"))             /* Variable for STA DNS 1 IP-Address 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D1B")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_1[1] = TEST_INT;                    /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB4D1C"))             /* Variable for STA DNS 1 IP-Address 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D1C")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_1[2] = TEST_INT;                    /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB4D1D"))             /* Variable for STA DNS 1 IP-Address 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D1D")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_1[3] = TEST_INT;                    /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  STA DNS 1 IP-Address set to: ");
        Serial.println (STA_DNS_1);
      #endif 
    }
    if (request->hasParam("SUB4D2A"))             /* Variable for STA DNS 2 IP-Address 4th digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D2A")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_2[0] = TEST_INT;                    /* Save 4th digit */  
    }  
    if (request->hasParam("SUB4D2B"))             /* Variable for STA DNS 2 IP-Address 3rd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D2B")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_2[1] = TEST_INT;                    /* Save 3rd digit */  
    } 
    if (request->hasParam("SUB4D2C"))             /* Variable for STA DNS 2 IP-Address 2nd digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D2C")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_2[2] = TEST_INT;                    /* Save 2nd digit */  
    } 
    if (request->hasParam("SUB4D2D"))             /* Variable for STA DNS 2 IP-Address 1st digit ? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB4D2D")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */ 
      STA_DNS_2[3] = TEST_INT;                    /* Save 1st digit */  
      #ifdef _DEBUG_
        Serial.print ("  STA DNS 2 IP-Address set to: ");
        Serial.println (STA_DNS_2);
      #endif 
    }
    REBOOT_SAVE_FLAG = false;                     /* Set reboot save flag false */
    REBOOT_FLAG = true;                           /* Set reboot flag true */    
    /* Answer request with displaying reboot menu */
    request->send(SPIFFS, "/Reboot.html");         
  });      
       

  /************************** Routes for submenu page 5: Display test *******************************/
   server.on("/submenu5", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 5 page...");
    #endif  
    /* Answer request with displaying of submenu page 1 web page */
    request->send(SPIFFS, "/Submenu5.html", String(), false, SUB_WEB_PROCESSOR_SUBM5);
  }); 

  /* Submenu 5: Button for saving of the values pressed */
  server.on("/store5", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 5 save button pressed: Saving data and update display...");
    # endif   
    
    /* Check for forth parameter: Font */
    if (request->hasParam("SUB5SELECT1"))         /* Variable available? */
    {
      /* Save parameter into the receive mesage string */
      RECEIVE_MESSAGE = request->getParam("SUB5SELECT1")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert string to integer */
      #ifdef _DEBUG_
        Serial.print ("  Display test set to: ");
      # endif             
      if(TEST_INT == 1)                           /* No selected ? */
      {
        DISPLAY_TEST = false;                     /* Set display test mode false */
        #ifdef _DEBUG_
          Serial.println ("Off"); 
        # endif
      }
      else                                        /* No selected ? */
      {
        DISPLAY_TEST = true;                      /* Set display test mode true */
        #ifdef _DEBUG_
          Serial.println ("On"); 
        # endif
      }
    } 
    
    /* Answer request with re-displaying of szenario 1 */
    request->send(SPIFFS, "/Submenu5.html", String(), false, SUB_WEB_PROCESSOR_SUBM5);
  });

  /************************** Routes for submenu page 6: Bootloader *******************************/
   server.on("/submenu6", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println ("Client connected: Display submenu 6 page...");
      Serial.println ();
    #endif  
    DISPLAY_TEST = false;                         /* Set display test false */
    /* Answer request with displaying of submain page 6 web page */
    request->send(SPIFFS, "/Submenu6.html", String(), false, SUB_WEB_PROCESSOR_SUBM6);
  });   

  /* Submenu 6: Button for saving of the values pressed */
  server.on("/store6", HTTP_GET, [](AsyncWebServerRequest *request)
  {  
    #ifdef _DEBUG_
      Serial.println ();
      Serial.println ("Submenu 6: Button for saving of bootloader status pressed...");
    #endif 
    if (request->hasParam("SUB6BOOT"))            /* Variable for boot mode? */
    {  
      /* Save parameter into the receive message string */
      RECEIVE_MESSAGE = request->getParam("SUB6BOOT")->value();
      TEST_INT = RECEIVE_MESSAGE.toInt();         /* Convert into integer value and save variable */              
      #ifdef _DEBUG_
        Serial.print ("  Boot mode set to: ");
        if (TEST_INT == 1)                        /* Normal boot ? */
        {
          Serial.println ("Normal");
        }
        else                                      /* Bootloader */
        {
          Serial.println ("Bootloader");
        }
        Serial.println ();
      #endif 
      if (TEST_INT == 1)                          /* Normal boot ? */
      {
        /* Answer request by opening main page */
        request->send(SPIFFS, "/Main.html", String(), false, SUB_WEB_PROCESSOR_MAIN); 
      }
      else                                        /* Bootloader */
      {
        BOOTLOADER_FLAG = true;                   /* Set bootloader flag true */
        /* Answer request by opening bootloader page */
        request->send(SPIFFS, "/Bootloader.html");

      }
    }
    else                                          /* Variable not found --> Reload page anyway */
    {  
      /* Answer request by opening main page */
      request->send(SPIFFS, "/Main.html", String(), false, SUB_WEB_PROCESSOR_MAIN); 
    }
  }); 

  /*********************** Errorhandler on undefined routes ***************************/
  server.onNotFound(SUB_MESSAGE_ERROR);
}


/****************************************************************************************/
/* Subroutine to be called if message error is detected                                 */
/****************************************************************************************/
void SUB_MESSAGE_ERROR(AsyncWebServerRequest *request) 
{
    #ifdef _DEBUG_
      Serial.println ();               
      Serial.println("New client request...");
      Serial.println ("  Message error!!!");
      Serial.println ();
    #endif  
    request->send(404, "text/plain", "Message error");
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website main with calculated values             */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_MAIN (const String& REPLACE_TARGET)
{
  /* Processing field MAINTEXT1 = Current message */
  if(REPLACE_TARGET == "MAINTEXT1")
  {
    SUBST_STRING = WEB_PAGE_STRING;               /* Replace string */
    return SUBST_STRING;                          /* Return substring */
  }

  /* Processing field MAINTEXT2 = Nessage brightness */
  if(REPLACE_TARGET == "MAINTEXT2")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    for(LOOP_COUNTER=0; LOOP_COUNTER <= 4; LOOP_COUNTER++)
    {
      SUBST_STRING += "<option value=\"";
      SUBST_STRING += LOOP_COUNTER;
      SUBST_STRING += "\"";
      if (BRIGHTNESS_LEVEL == LOOP_COUNTER)       /* Default selection ? */
      {
        SUBST_STRING += " selected>";
      }
      else                                        /* No default selection ? */
      {
        SUBST_STRING += ">";
      }
      SUBST_STRING += MENU_OPTION_3[LOOP_COUNTER];
      SUBST_STRING += "</option><br>";
    }
    return SUBST_STRING;                          /* Return substring */
  }
  
  /* Processing field MAINTEXT3 = Font */
  if(REPLACE_TARGET == "MAINTEXT3")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if ( OWN_FONT_LIBRARY == true)                /* Font 1 = Own library ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_5[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_5[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Font 2 = Standard library ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_5[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_5[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }

  /* Processing field MAINTEXT4 = Char spacing */
  if(REPLACE_TARGET == "MAINTEXT4")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    for(LOOP_COUNTER=0; LOOP_COUNTER <= 4; LOOP_COUNTER++)
    {
      SUBST_STRING += "<option value=\"";
      SUBST_STRING += LOOP_COUNTER;
      SUBST_STRING += "\"";
      if ((CHAR_SPACING-1) == LOOP_COUNTER)        /* Default selection ? */
      {
        SUBST_STRING += " selected>";
      }
      else                                        /* No default selection ? */
      {
        SUBST_STRING += ">";
      }
      SUBST_STRING += MENU_OPTION_4[LOOP_COUNTER];
      SUBST_STRING += "</option><br>";
    }
    return SUBST_STRING;                          /* Return substring */
  }

    /* Processing field MAINTEXT5 = Effect */
  if(REPLACE_TARGET == "MAINTEXT5")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if ( INV_EFFECT == false)                     /* No inverting effect ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_6[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_6[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Inverting effect ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_6[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_6[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
  
  return String();                                /* Return substring */
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 1 with calculated values        */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM1 (const String& REPLACE_TARGET)
{
  /* Processing field MAINTEXT1 = Current message */
  if(REPLACE_TARGET == "SUB1TEXT1")
  {
    SUBST_STRING = WEB_PAGE_STRING;               /* Replace string */
    return SUBST_STRING;                          /* Return substring */
  }

  /* Processing field SUB1TEXT2 = Nessage brightness */
  if(REPLACE_TARGET == "SUB1TEXT2")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    for(LOOP_COUNTER=0; LOOP_COUNTER <= 4; LOOP_COUNTER++)
    {
      SUBST_STRING += "<option value=\"";
      SUBST_STRING += LOOP_COUNTER;
      SUBST_STRING += "\"";
      if (BRIGHTNESS_LEVEL == LOOP_COUNTER)       /* Default selection ? */
      {
        SUBST_STRING += " selected>";
      }
      else                                        /* No default selection ? */
      {
        SUBST_STRING += ">";
      }
      SUBST_STRING += MENU_OPTION_3[LOOP_COUNTER];
      SUBST_STRING += "</option><br>";
    }
    return SUBST_STRING;                          /* Return substring */
  }

  /* Processing field SUB1TEXT3 = Nessage speed */
  if(REPLACE_TARGET == "SUB1TEXT3")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    for(LOOP_COUNTER=0; LOOP_COUNTER <= 4; LOOP_COUNTER++)
    {
      SUBST_STRING += "<option value=\"";
      SUBST_STRING += LOOP_COUNTER;
      SUBST_STRING += "\"";
      if (SPEED_LEVEL == LOOP_COUNTER)            /* Default selection ? */
      {
        SUBST_STRING += " selected>";
      }
      else                                        /* No default selection ? */
      {
        SUBST_STRING += ">";
      }
      SUBST_STRING += MENU_OPTION_2[LOOP_COUNTER];
      SUBST_STRING += "</option><br>";
    }
    return SUBST_STRING;                          /* Return substring */
  }
  
  
  /* Processing field SUB1TEXT4 = Font */
  if(REPLACE_TARGET == "SUB1TEXT4")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if ( OWN_FONT_LIBRARY == true)                /* Font 1 = Own library ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_5[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_5[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Font 2 = Standard library ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_5[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_5[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }

  /* Processing field SUB1TEXT5 = Char spacing */
  if(REPLACE_TARGET == "SUB1TEXT5")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    for(LOOP_COUNTER=0; LOOP_COUNTER <= 4; LOOP_COUNTER++)
    {
      SUBST_STRING += "<option value=\"";
      SUBST_STRING += LOOP_COUNTER;
      SUBST_STRING += "\"";
      if ((CHAR_SPACING-1) == LOOP_COUNTER)       /* Default selection ? */
      {
        SUBST_STRING += " selected>";
      }
      else                                        /* No default selection ? */
      {
        SUBST_STRING += ">";
      }
      SUBST_STRING += MENU_OPTION_4[LOOP_COUNTER];
      SUBST_STRING += "</option><br>";
    }
    return SUBST_STRING;                          /* Return substring */
  }

    /* Processing field SUB1TEXT6 = Text type */
  if(REPLACE_TARGET == "SUB1TEXT6")             
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if (MESSAGE_TYPE == 1)                        /* Text messsage ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_1[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_1[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Test message ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_1[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_1[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
  
  return String();                                /* Return substring */
}

/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 2 with calculated values        */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM2 (const String& REPLACE_TARGET)
{
  /* Processing field SUB2TEXT1 = WLAN mode */
  if(REPLACE_TARGET == "SUB2TEXT1")
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if (STA_BOOT_MODE == false)                   /* No STA mode ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_8[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_8[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* STA mode ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_8[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_8[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
  /* Processing field SUB2TEXT2 = Autostart */
  if(REPLACE_TARGET == "SUB2TEXT2")
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if (AUTOSTART_FLAG == false)                  /* No Autostart ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_9[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_9[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Autostart ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_9[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_9[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
  /* Processing field SUB2TEXT3 = Permanent storage */
  if(REPLACE_TARGET == "SUB2TEXT3")
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if (SAVE_FLAG == false)                       /* No permanent save ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_9[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_9[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Permanent save ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_9[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_9[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
  /* Processing field SUB2TEXT4 = Reset */
  if(REPLACE_TARGET == "SUB2TEXT4")
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    SUBST_STRING += "<option value=\"1\" selected>";
    SUBST_STRING += MENU_OPTION_9[0];
    SUBST_STRING += "</option><br>";
    SUBST_STRING += "<option value=\"2\">";
    SUBST_STRING += MENU_OPTION_9[1];
    SUBST_STRING += "</option><br>";
    return SUBST_STRING;                          /* Return substring */
  }    
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 3 with calculated values        */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM3 (const String& REPLACE_TARGET)
{
  if (REPLACE_TARGET == "SUB3VALUE1")             /* AP WLAN SSID */
  {
    SUBST_STRING = AP_WLAN_SSID;                  /* Convert AP WLAN SSID to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE2")             /* AP WLAN password */
  {
    SUBST_STRING = AP_WLAN_PWD;                   /* Convert AP WLAN password to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE4")             /* AP fixed IP-Address 4th digit */
  {
    SUBST_STRING = AP_FIXED_IP[0];                /* Convert AP fixed IP-Address 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE5")             /* AP fixed IP 3rd digit */
  {
    SUBST_STRING = AP_FIXED_IP[1];                /* Convert AP fixed IP-Address 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE6")             /* AP fixed IP 2nd digit */
  {
    SUBST_STRING = AP_FIXED_IP[2];                /* Convert AP fixed IP-Address 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE7")             /* AP fixed IP 1st digit */
  {
    SUBST_STRING = AP_FIXED_IP[3];                /* Convert AP fixed IP-Address 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }      
  if (REPLACE_TARGET == "SUB3VALUE8")             /* AP subnet mask 4th digit */
  {
    SUBST_STRING = AP_SUBNET[0];                  /* Convert AP subnet mask 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE9")             /* AP subnet mask 3rd digit */
  {
    SUBST_STRING = AP_SUBNET[1];                  /* Convert AP subnet mask 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE10")            /* AP subnet mask 2nd digit */
  {
    SUBST_STRING = AP_SUBNET[2];                  /* Convert AP subnet mask 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB3VALUE11")            /* AP subnet mask 1st digit */
  {
    SUBST_STRING = AP_SUBNET[3];                  /* Convert AP subnet mask 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 4 with calculated values        */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM4 (const String& REPLACE_TARGET)
{
  if (REPLACE_TARGET == "SUB4VALUE1")             /* STA WLAN SSID */
  {
    SUBST_STRING = STA_WLAN_SSID;                 /* Convert STA WLAN SSID to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE2")             /* STA WLAN password */
  {
    SUBST_STRING = STA_WLAN_PWD;                  /* Convert STA WLAN password to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE3")             /* STA hostname */
  {
    SUBST_STRING = STA_HOSTNAME;                  /* Convert STA hostname to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }  
  if (REPLACE_TARGET == "SUB4VALUE4")             /* STA fixed IP-Address 4th digit */
  {
    SUBST_STRING = STA_FIXED_IP[0];               /* Convert STA fixed IP-Address 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE5")             /* STA fixed IP-Address 3rd digit */
  {
    SUBST_STRING = STA_FIXED_IP[1];               /* Convert STA fixed IP-Address 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE6")             /* STA fixed IP-Address 2nd digit */
  {
    SUBST_STRING = STA_FIXED_IP[2];               /* Convert STA fixed IP-Address 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE7")             /* STA fixed IP-Address 1st digit */
  {
    SUBST_STRING = STA_FIXED_IP[3];               /* Convert STA fixed IP-Address 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }      
  if (REPLACE_TARGET == "SUB4VALUE8")             /* STA subnet mask 4th digit */
  {
    SUBST_STRING = STA_SUBNET[0];                 /* Convert STA subnet mask 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE9")             /* STA subnet mask 3rd digit */
  {
    SUBST_STRING = STA_SUBNET[1];                 /* Convert STA subnet mask 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE10")            /* STA subnet mask 2nd digit */
  {
    SUBST_STRING = STA_SUBNET[2];                 /* Convert STA subnet mask 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE11")            /* STA subnet mask 1st digit */
  {
    SUBST_STRING = STA_SUBNET[3];                 /* Convert STA subnet mask 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE12")            /* STA gateway IP 4th digit */
  {
    SUBST_STRING = STA_GATEWAY[0];                /* Convert STA gateway IP 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE13")            /* STA gateway IP-Address 3rd digit */
  {
    SUBST_STRING = STA_GATEWAY[1];                /* Convert STA gateway IP-Address 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE14")            /* STA gateway IP-Address 2nd digit */
  {
    SUBST_STRING = STA_GATEWAY[2];                /* Convert STA gateway IP-Address 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE15")            /* STA gateway IP-Address 1st digit */
  {
    SUBST_STRING = STA_GATEWAY[3];                /* Convert STA gateway IP-Address 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE16")            /* STA DNS 1 IP-Address 4th digit */
  {
    SUBST_STRING = STA_DNS_1[0];                  /* Convert STA DNS 1 IP-Address 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE17")            /* STA DNS 1 IP-Address 3rd digit */
  {
    SUBST_STRING = STA_DNS_1[1];                  /* Convert STA DNS 1 IP-Address 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE18")            /* STA DNS 1 IP-Address 2nd digit */
  {
    SUBST_STRING = STA_DNS_1[2];                  /* Convert STA DNS 1 IP-Address 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE19")            /* STA DNS 1 IP-Address 1st digit */
  {
    SUBST_STRING = STA_DNS_1[3];                  /* Convert STA DNS 1 IP-Address 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE20")            /* STA DNS 2 IP-Address 4th digit */
  {
    SUBST_STRING = STA_DNS_2[0];                  /* Convert STA DNS 2 IP-Address 4th digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE21")            /* STA DNS 2 IP-Address 3rd digit */
  {
    SUBST_STRING = STA_DNS_2[1];                  /* Convert STA DNS 2 IP-Address 3rd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE22")            /* STA DNS 2 IP-Address 2nd digit */
  {
    SUBST_STRING = STA_DNS_2[2];                  /* Convert STA DNS 2 IP-Address 2nd digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
  if (REPLACE_TARGET == "SUB4VALUE23")            /* STA DNS 2 IP-Address 1st digit */
  {
    SUBST_STRING = STA_DNS_2[3];                  /* Convert STA DNS 2 IP-Address 1st digit to string */
    return (SUBST_STRING);                        /* Return message buffer */
  }
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 5 with calculated values        */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM5 (const String& REPLACE_TARGET)
{
  /* Processing field SUB5TEXT1 = Display test */
  if(REPLACE_TARGET == "SUB5TEXT1")
  {
    SUBST_STRING = "";                            /* Reset substring for substitution */
    /* Generate replacement string */
    if ( DISPLAY_TEST == false)                   /* No display test ? */
    {
      SUBST_STRING += "<option value=\"1\" selected>";
      SUBST_STRING += MENU_OPTION_7[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\">";
      SUBST_STRING += MENU_OPTION_7[1];
      SUBST_STRING += "</option><br>";
    }
    else                                          /* Dispaly test ? */
    {
      SUBST_STRING += "<option value=\"1\">";
      SUBST_STRING += MENU_OPTION_7[0];
      SUBST_STRING += "</option><br>";
      SUBST_STRING += "<option value=\"2\" selected>";
      SUBST_STRING += MENU_OPTION_7[1];
      SUBST_STRING += "</option><br>";   
    }
    return SUBST_STRING;                          /* Return substring */
  }
}


/****************************************************************************************/
/* Subroutine to replace placeholder in website submenu 6 with calculated values        */
/* For options see example for mode string to be returned                               */
/* <option value="1" selected>Normal</option><br><option value="2">Bootloader           */
/* </option><br>                                                                        */
/* Note that " must be \" in the string!                                                */
/****************************************************************************************/
String SUB_WEB_PROCESSOR_SUBM6 (const String& REPLACE_TARGET)
{
  if (REPLACE_TARGET == "SUB6SELECT1")            /* Boot mode */
  {
    /* Replacement string */
    SUBST_STRING = "<option value=\"1\" selected>Normal</option><br><option value=\"2\">Bootloader</option><br>";
    return (SUBST_STRING);                        /* Return message buffer */
  }
}
