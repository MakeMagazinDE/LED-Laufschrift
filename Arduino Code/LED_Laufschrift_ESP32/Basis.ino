/****************************************************************************************/
/* Basis.ino                                                                            */
/* Collection of basis routines                                                         */
/*                                                                                      */
/* 1. INIT FILE routines:                                                               */
/* SUB_READ_INIT_FILE:      Subroutine to read out init file Init.txt                   */
/* SUB_WRITE_INIT_FILE:     Subroutine to save settings in init file Init.txt           */
/* SUB_CONFIG_SETTING:      Subroutine to change configuration settings via             */
/*                          console entry                                               */
/*                                                                                      */
/* 2. WLAN routines:                                                                    */
/* SUB_STA_WLAN_CONNECT:    Subroutine to connect to STA WLAN                           */
/* SUB_AP_WLAN:             Subroutine to create WLAN access point                      */
/*                                                                                      */
/* 3. BOOTLOADER routines:                                                              */
/* SUB_WLAN_BOOTLOADER:     Subroutine to initialize WLAN bootloader and wait           */
/*                          for upload                                                  */
/*                                                                                      */
/* 4. SPECIAL routines:                                                                 */
/* SUB_REPLACE_SPECIAL_CHARACTERS: Subroutine to replace special chars for function     */
/*                                 "toCharArray" to work correctly!                     */
/*                                                                                      */
/****************************************************************************************/


/****************************************************************************************/
/****************************************************************************************/
/* INIT FILE ROUTINES                                                                   */
/****************************************************************************************/
/****************************************************************************************/


/****************************************************************************************/
/* Subroutine to read out init file Init.txt                                            */
/****************************************************************************************/
void SUB_READ_INIT_FILE (void)
{
  boolean BREAK_LOOP;                             /* Break loop flag: false = No break, true = Break */
  char RECEIVE_CHAR;                              /* Received byte from file reading */
  byte LINE_COUNTER;                              /* Line counter */
  byte IP_ADDRESS_COUNTER;                        /* IP-Address counter */
  int TEST_INT;                                   /* Test integer */
  String IP_ADDRESS_STRING;                       /* IP-Address string */

  #ifdef _DEBUG_
    Serial.print("  Open file Init.txt: ");
  # endif 

  BREAK_LOOP = false;                             /* Set break loop false */
  File file = SPIFFS.open("/Init.txt", FILE_READ);/* Open Init file for read */
  if(!file)                                       /* File does not exist? */
  {
    #ifdef _DEBUG_
      Serial.println ("Failed! File does not exist");
    # endif
    BREAK_LOOP = true;                            /* Set break loop true */ 
  }
  
  if (BREAK_LOOP == false)                        /* No error yet */
  {
    #ifdef _DEBUG_
      Serial.println ("Done");
      Serial.println ("  Read Init.txt content: ");
    # endif  
    LINE_COUNTER = 1;                             /* Set line counter to first line */
    LOOP_COUNTER = 0;                             /* Check for maximum received bytes to prevent buffer overflows */

    /* Delete AP WLAN_SSID string */
    memset (AP_WLAN_SSID, 0 , sizeof(AP_WLAN_SSID));
    /* Delete AP WLAN passowrd string */
    memset (AP_WLAN_SSID, 0 , sizeof(AP_WLAN_PWD));   
    /* Delete STA WLAN_SSID string */
    memset (STA_WLAN_SSID, 0 , sizeof(STA_WLAN_SSID));
    /* Delete STA WLAN passowrd string */
    memset (STA_WLAN_SSID, 0 , sizeof(STA_WLAN_PWD));
    /* Delete STA HOSTNAME string */
    memset (STA_HOSTNAME, 0 , sizeof(STA_HOSTNAME)); 
       
    /* Read until end of file or error detected */
    while((file.available()) && (BREAK_LOOP == false))                    
    {
      RECEIVE_CHAR = file.read();                 /* Read char byte from file */

      if (RECEIVE_CHAR == '\r')                   /* Carrage return detected ? */
      {
        /* Effect */
        if (LINE_COUNTER == 21)                   /* Receive effect ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Inverting effect set to: ");
          # endif              
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 1) && (TEST_INT <= 2))
          {
            if(TEST_INT == 1)                     /* No inverting effect selected ? */
            {
              INV_EFFECT = false;                 /* Set inverting effect false */
              ORIG_INV_EFFECT = false;
              #ifdef _DEBUG_
                Serial.println ("Off"); 
              # endif
            }
            else                                  /* Inverting effect selected ? */
            {
              INV_EFFECT = true;                  /* Set inverting effect true */
              ORIG_INV_EFFECT = true; 
              #ifdef _DEBUG_
                Serial.println ("On"); 
              # endif
            }
            LINE_COUNTER = 22;                    /* Set line counter to twentysecond line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Effect selection format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }

        /* Text type */
        if (LINE_COUNTER == 20)                   /* Receive text type ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Text type set to: ");
          # endif              
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 1) && (TEST_INT <= 2))
          {
            if(TEST_INT == 1)                     /* Text message selected ? */
            {
              #ifdef _DEBUG_
                Serial.print ("Text message, text set to: "); 
                Serial.println (WEB_PAGE_STRING);
              # endif
              MESSAGE_TYPE = 1;                   /* Save message type */
              ORIG_MESSAGE_TYPE = 1;           
            }
            else                                  /* Testmessage selected ? */
            {
              /* Assemble test string */
              if (STA_BOOT_MODE == true)          /* Station mode selected ? */
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
              else                                /* Access point mode selected ? */
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
              ORIG_WEB_PAGE_STRING = WEB_PAGE_STRING;
              #ifdef _DEBUG_
                Serial.print ("Test message, text set to: "); 
                Serial.println (WEB_PAGE_STRING);
              # endif
              MESSAGE_TYPE = 2;                   /* Save message type */
              ORIG_MESSAGE_TYPE = 2;                   
            }            
            LINE_COUNTER = 21;                    /* Set line counter to twentyfirst line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Text type format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* Char spacing */
        if (LINE_COUNTER == 19)                   /* Receive char spacing ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Char spacing set to: "); 
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 4))
          {
            CHAR_SPACING = TEST_INT;              /* Copy int value to char spacing byte */ 
            ORIG_CHAR_SPACING = TEST_INT;  
            #ifdef _DEBUG_             
              Serial.print (CHAR_SPACING);
              Serial.println (" pixels");
            #endif            
            LINE_COUNTER = 20;                    /* Set line counter to twentith line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Char spacing format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* Font */
        if (LINE_COUNTER == 18)                   /* Receive font type ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Font set to: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 1) && (TEST_INT <= 2))
          {
            if(TEST_INT == 1)                     /* Own library selected ? */
            {
              OWN_FONT_LIBRARY = true;            /* Set own library true */
              ORIG_OWN_FONT_LIBRARY = true;
              #ifdef _DEBUG_
                Serial.println ("Font 1: Own font"); 
              # endif
            }
            else                                  /* Standard font selected ? */
            {
              OWN_FONT_LIBRARY = false;           /* Set own library false */
              ORIG_OWN_FONT_LIBRARY = false;
              #ifdef _DEBUG_
                Serial.println ("Font 2: Standard font"); 
              # endif
            }          
            LINE_COUNTER = 19;                    /* Set line counter to nineteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Font selection format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }

        /* Speed level */
        if (LINE_COUNTER == 17)                   /* Receive speed level ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Message speed set to: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 4))
          {
            SPEED_LEVEL = TEST_INT;               /* Copy int value to speed level byte */ 
            ORIG_SPEED_LEVEL = TEST_INT;  
            #ifdef _DEBUG_             
              Serial.print (SPEED_LEVELS[SPEED_LEVEL]);
              Serial.println (" (10 = Fastest, 20, 30, 40, 50 = Slowest)");
            #endif            
            LINE_COUNTER = 18;                    /* Set line counter to eighteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Speed level format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* Brightness level */
        if (LINE_COUNTER == 16)                   /* Receive brightness level ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Message brightness set to: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 4))
          {
            BRIGHTNESS_LEVEL = TEST_INT;          /* Copy int value to brightness level byte */             
            ORIG_BRIGHTNESS_LEVEL = TEST_INT;
            #ifdef _DEBUG_             
              Serial.print (BRIGHTNESS_LEVELS[BRIGHTNESS_LEVEL]);
              Serial.println (" (2 = Lowest, 7, 10, 13, 15 = Brightest)");
            #endif            
            LINE_COUNTER = 17;                    /* Set line counter to seventeenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Brightness level format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* Text scrolling */
        if (LINE_COUNTER == 15)                   /* Receive scrolling yes/no ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Text scrolling set to: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 1))
          {
            if(TEST_INT == 0)                     /* No scrolling selected ? */
            {
              SCROLLING_TEXT = false;             /* Set scrolling flag false */
              ORIG_SCROLLING_TEXT = false;             
              #ifdef _DEBUG_
                Serial.println ("Static text"); 
              # endif
            }
            else                                  /* Scrolling selected ? */
            {
              SCROLLING_TEXT = true;              /* Set scrolling flag true */
              ORIG_SCROLLING_TEXT = true;
              #ifdef _DEBUG_
                Serial.println ("Scrolling text"); 
              # endif
            }
            LINE_COUNTER = 16;                    /* Set line counter to sixteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Scrolling value error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }        

        /* Text message */
        if (LINE_COUNTER == 14)                   /* Receive text message ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Text message: ");
          # endif 
          /* Check on too long SSID */
          if ((LOOP_COUNTER > 0) && (LOOP_COUNTER < 255))        
          {
            WEB_PAGE_STRING = IP_ADDRESS_STRING;  /* Copy received string to display string */
            ORIG_WEB_PAGE_STRING = IP_ADDRESS_STRING;   
            #ifdef _DEBUG_
              Serial.println (WEB_PAGE_STRING);
            # endif 
            LINE_COUNTER = 15;                    /* Set line counter to fifteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */ 
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                        
          }
          else                                    /* Not in range ? */
          {
             #ifdef _DEBUG_
              Serial.println ("Text message length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }

        /* Boot mode */
        if (LINE_COUNTER == 13)                   /* Receive boot mode ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    Boot mode set to: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 1))
          {
            if (TEST_INT == 0)                    /* AP mode ? */
            {
              STA_BOOT_MODE = false;              /* Set station boot mode false */
              #ifdef _DEBUG_
                Serial.println ("Accesss Point (AP)");
              # endif 
            }                                     
            else                                  /* STA mode ? */
            {
              STA_BOOT_MODE = true;               /* Set station boot mode true */
              #ifdef _DEBUG_
                Serial.println ("Station Mode (STA)");
              # endif 
            }             
            LINE_COUNTER = 14;                    /* Set line counter to fourteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */                               
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
            #ifdef _DEBUG_
              Serial.println ("Boot mode format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* STA DNS 2 IP-Address */
        if (LINE_COUNTER == 12)                   /* Receive STA DNS 2 IP-Address ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA DNS 2 IP-Address: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            STA_DNS_2[3] = TEST_INT;              /* Save 3rd digit */ 
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */                           
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_DNS_2);
            # endif 
            LINE_COUNTER = 13;                    /* Set line counter to thirteenth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("STA DNS 2 IP-Address format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }
        
        /* STA DNS 1 IP-Address */
        if (LINE_COUNTER == 11)                   /* Receive STA DNS 1 IP-Address ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA DNS 1 IP-Address: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            STA_DNS_1[3] = TEST_INT;              /* Save 3rd digit */ 
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */                           
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_DNS_1);
            # endif 
            LINE_COUNTER = 12;                    /* Set line counter to twelfth line */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("STA DNS 1 IP-Address format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 

        /* STA Gateway IP-Address */
        if (LINE_COUNTER == 10)                   /* Receive gateway IP-Address ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA gateway IP-Address: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            STA_GATEWAY[3] = TEST_INT;            /* Save 3rd digit */
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */                            
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_GATEWAY);
            # endif 
            LINE_COUNTER = 11;                    /* Set line counter to eleventh line */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("STA gateway IP-Address format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        } 
        
       /* STA subnet mask */
        if (LINE_COUNTER == 9)                    /* Receive STA subnet mask ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA subnet mask: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            STA_SUBNET[3] = TEST_INT;             /* Save 3rd digit */   
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */            
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_SUBNET);
            # endif 
            LINE_COUNTER = 10;                    /* Set line counter to tenth line */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("STA subnet mask format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }        

        /* STA Fixed IP-Address */
        if (LINE_COUNTER == 8)                    /* Receive STA Fixed IP-Address ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA fixed IP-Address: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            STA_FIXED_IP[3] = TEST_INT;           /* Save 3rd digit */ 
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */             
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_FIXED_IP);
            # endif 
            LINE_COUNTER = 9;                     /* Set line counter to nineth line */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("STA fixed IP-Address format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }        

        /* STA hostname */
        if (LINE_COUNTER == 7)                    /* Receive STA hostname ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA hostname: ");
          # endif 
          /* Check on too long hostname */
          if ((LOOP_COUNTER > 1) && (LOOP_COUNTER < 20))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_HOSTNAME);
            # endif 
            LINE_COUNTER = 8;                     /* Set line counter to eighth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            IP_ADDRESS_STRING = "";               /* Delete string */            
          }
          else                                    /* Not in range ? */
          {
             #ifdef _DEBUG_
              Serial.println ("STA hostname length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }
        
        /* STA WLAN Password */
        if (LINE_COUNTER == 6)                    /* Receive STA WLAN password ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA WLAN password: ");
          # endif 
          /* Check on too long WLAN password */
          if ((LOOP_COUNTER > 1) && (LOOP_COUNTER < 20))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_WLAN_PWD);
            # endif 
            LINE_COUNTER = 7;                     /* Set line counter to seventh line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */            
          }
          else                                    /* Not in range ? */
          {
             #ifdef _DEBUG_
              Serial.println ("STA WLAN password length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }
     
        /* STA SSID */
        if (LINE_COUNTER == 5)                    /* Receive STA SSID ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    STA WLAN SSID: ");
          # endif 
          /* Check on too long SSID */
          if ((LOOP_COUNTER > 1) && (LOOP_COUNTER < 20))        
          {
            #ifdef _DEBUG_
              Serial.println (STA_WLAN_SSID);
            # endif 
            LINE_COUNTER = 6;                     /* Set line counter to sixth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */            
          }
          else                                    /* Not in range ? */
          {
             #ifdef _DEBUG_
              Serial.println ("STA SSID length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }

       /* AP subnet mask */
        if (LINE_COUNTER == 4)                    /* Receive AP Subnet mask ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    AP subnet mask: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            AP_SUBNET[3] = TEST_INT;              /* Save 3rd digit */   
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */            
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (AP_SUBNET);
            # endif 
            LINE_COUNTER = 5;                     /* Set line counter to fifth line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("AP subnet mask format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }        

        /* AP Fixed IP-Address */
        if (LINE_COUNTER == 3)                    /* Receive AP Fixed IP-Address ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    AP fixed IP-Address: ");
          # endif 
          TEST_INT = IP_ADDRESS_STRING.toInt();
          /* In range ? */
          if ((TEST_INT >= 0) && (TEST_INT <= 255))
          {
            AP_FIXED_IP[3] = TEST_INT;            /* Save 3rd digit */ 
            IP_ADDRESS_COUNTER = 5;               /* Set to fifth address */              
          }
          else                                    /* Not in range ? */
          {
            LOOP_COUNTER = 255;                   /* Set loop counter out of range */
          }
          /* Check if 4 digits received and loop counter in range */
          if ((LOOP_COUNTER < 20) && (IP_ADDRESS_COUNTER == 5))        
          {
            #ifdef _DEBUG_
              Serial.println (AP_FIXED_IP);
            # endif 
            LINE_COUNTER = 4;                     /* Set line counter to forth line */
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            LOOP_COUNTER = 0;                     /* Reset loop counter */
            IP_ADDRESS_STRING = "";                /* Delete string for next digit */
          }
          else                                    /* Error ? */  
          {
            #ifdef _DEBUG_
              Serial.println ("AP fixed IP-Address format error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }
        
        /* AP WLAN Password */
        if (LINE_COUNTER == 2)                    /* Receive AP WLAN password ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    AP WLAN password: ");
          # endif 
          /* Check on too long WLAN password */
          if ((LOOP_COUNTER > 1) && (LOOP_COUNTER < 20))        
          {
            #ifdef _DEBUG_
              Serial.println (AP_WLAN_PWD);
            # endif 
            LINE_COUNTER = 3;                     /* Set line counter to third line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */     
            IP_ADDRESS_COUNTER = 1;               /* Set to first address */
            IP_ADDRESS_STRING = "";               /* Delete string for next digit */       
          }
          else                                    /* WLAN password length error ? */
          {
             #ifdef _DEBUG_
              Serial.println ("AP WLAN password length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }
        
        /* AP SSID */
        if (LINE_COUNTER == 1)                    /* Receive AP SSID ? */
        {
          #ifdef _DEBUG_
            Serial.print ("    AP WLAN SSID: ");
          # endif 
          /* Check on too long SSID */
          if ((LOOP_COUNTER > 1) && (LOOP_COUNTER < 20))        
          {
            #ifdef _DEBUG_
              Serial.println (AP_WLAN_SSID);
            # endif 
            LINE_COUNTER = 2;                     /* Set line counter to second line */
            LOOP_COUNTER = 0;                     /* Reset loop counter */            
          }
          else                                    /* SSID length error ? */
          {
             #ifdef _DEBUG_
              Serial.println ("AP SSID length error!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */
          }
        }   
      }
      else                                        /* No CR received ? */
      {
        if (RECEIVE_CHAR != '\n')                 /* No line feed ? */
        {
          if (LOOP_COUNTER < 255) 
          {
            if (LINE_COUNTER == 1)                /* Receive AP SSID ? */
            {
              /* Add received char */
              AP_WLAN_SSID[LOOP_COUNTER] = RECEIVE_CHAR;      
            }
            if (LINE_COUNTER == 2)                /* Receive AP WLAN password ? */
            {
              /* Add received char */
              AP_WLAN_PWD[LOOP_COUNTER] = RECEIVE_CHAR;      
            }            
            if (LINE_COUNTER == 3)                /* Receive AP fixed IP-Address ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_FIXED_IP[2] = TEST_INT;    /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to forth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_FIXED_IP[1] = TEST_INT;    /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_FIXED_IP[0] = TEST_INT;    /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }

            if (LINE_COUNTER == 4)                /* Receive AP subnet mask ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR; /* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_SUBNET[2] = TEST_INT;      /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to forth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_SUBNET[1] = TEST_INT;      /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    AP_SUBNET[0] = TEST_INT;      /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }
            
            if (LINE_COUNTER == 5)                /* Receive STA SSID ? */
            {
              /* Add received char */
              STA_WLAN_SSID[LOOP_COUNTER] = RECEIVE_CHAR;      
            }
            if (LINE_COUNTER == 6)                /* Receive STA WLAN password ? */
            {
              /* Add received char */
              STA_WLAN_PWD[LOOP_COUNTER] = RECEIVE_CHAR;      
            }                 
            if (LINE_COUNTER == 7)                /* Receive STA Hostname ? */
            {
              /* Add received char */
              STA_HOSTNAME[LOOP_COUNTER] = RECEIVE_CHAR;      
            }
            if (LINE_COUNTER == 8)                /* Receive fixed STA IP-Address ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_FIXED_IP[2] = TEST_INT;   /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to fourth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_FIXED_IP[1] = TEST_INT;   /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_FIXED_IP[0] = TEST_INT;   /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }
            
            if (LINE_COUNTER == 9)                /* Receive STA subnet mask ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_SUBNET[2] = TEST_INT;     /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to fourth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_SUBNET[1] = TEST_INT;     /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_SUBNET[0] = TEST_INT;     /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }            
            
            if (LINE_COUNTER == 10)               /* Receive STA gateway Ip-Address ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_GATEWAY[2] = TEST_INT;    /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to fourth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_GATEWAY[1] = TEST_INT;    /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_GATEWAY[0] = TEST_INT;    /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }
            
            if (LINE_COUNTER == 11)               /* Receive STA DNS 1 Address ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_1[2] = TEST_INT;      /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to fourth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_1[1] = TEST_INT;      /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_1[0] = TEST_INT;      /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }
            
            if (LINE_COUNTER == 12)               /* Receive STA DNS 2 Address ? */
            { 
              if (RECEIVE_CHAR != '.')            /* Delimiter not detected ? */
              {
                IP_ADDRESS_STRING += RECEIVE_CHAR;/* Add char */
              }
              else                                /* Delimiter detected ? */
              {             
                if (IP_ADDRESS_COUNTER == 3)      /* 3rd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_2[2] = TEST_INT;      /* Save 3rd digit */               
                    IP_ADDRESS_COUNTER = 4;       /* Set to fourth digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }
                if (IP_ADDRESS_COUNTER == 2)      /* 2nd digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_2[1] = TEST_INT;      /* Save 2rd digit */               
                    IP_ADDRESS_COUNTER = 3;       /* Set to third digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }                
                if (IP_ADDRESS_COUNTER == 1)      /* 1st digit ? */
                {
                  /* Convert to Int */
                  TEST_INT = IP_ADDRESS_STRING.toInt();
                  /* In range ? */
                  if ((TEST_INT >= 0) && (TEST_INT <= 255))
                  {
                    STA_DNS_2[0] = TEST_INT;      /* Save 1st digit */               
                    IP_ADDRESS_COUNTER = 2;       /* Set to second digit */
                    IP_ADDRESS_STRING = "";       /* Delete string for next digit */
                  }
                  else                            /* Not in range ? */
                  {
                    LOOP_COUNTER = 255;           /* Set loop counter out of range */
                  }
                }  
              }    
            }

            if (LINE_COUNTER == 13)               /* Receive boot mode ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            
            if (LINE_COUNTER == 14)               /* Receive text message ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            if (LINE_COUNTER == 15)               /* Receive scrolling yes/no ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            
            if (LINE_COUNTER == 16)               /* Receive brightness level ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            if (LINE_COUNTER == 17)               /* Receive speed level ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            if (LINE_COUNTER == 18)               /* Receive selected font ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            if (LINE_COUNTER == 19)               /* Receive char spacing ? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }            
            if (LINE_COUNTER == 20)               /* Receive message type? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }  
            if (LINE_COUNTER == 21)               /* Receive effect? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }
            if (LINE_COUNTER == 22)               /* Receive text type? */
            {
              /* Add received char */
              IP_ADDRESS_STRING += RECEIVE_CHAR;  /* Add char */     
            }                                                            
            LOOP_COUNTER++;                       /* Increment loop counter */
          }
          else                                    /* Length error */
          {
            #ifdef _DEBUG_
              Serial.println ("File length error!!!");
            # endif 
            BREAK_LOOP = true;                    /* Set break loop true */          
          }
        }
      }
    }
    file.close();                                 /* Close file after reading */
    
    if (BREAK_LOOP == false)
    { 
      /* Receive autostart status */ 
      #ifdef _DEBUG_
        Serial.print ("    Autostart mode to display text message after boot set to: ");
      # endif           
      TEST_INT = IP_ADDRESS_STRING.toInt();
      /* In range ? */
      if ((TEST_INT >= 1) && (TEST_INT <= 2))
      {
        if(TEST_INT == 1)                         /* No autostart selected ? */
        {
          AUTOSTART_FLAG = false;                 /* Set autostart false */
          #ifdef _DEBUG_
            Serial.println ("Off"); 
          # endif
        }
        else                                      /* Autostart selected ? */
        {
          AUTOSTART_FLAG = true;                  /* Set autostart true */
          #ifdef _DEBUG_
            Serial.println ("On"); 
          # endif
        }                               
      }
      else                                        /* Not in range ? */
      {
        #ifdef _DEBUG_
          Serial.println ("Autostart mode format error!");
        # endif 
        BREAK_LOOP = true;                        /* Set break loop true */
      }
    }     
  }


  if (BREAK_LOOP == true)                         /* Error ? */
  {
    /* Override settings from init file */
    BRIGHTNESS_LEVEL = 4;                         /* Set high brightness level */        
    OWN_FONT_LIBRARY = true;                      /* Set own library flag true */
    CHAR_SPACING = 1;                             /* Set spacing between chars to one */
    INV_EFFECT = false;                           /* No inverting effect */  
    WEB_PAGE_STRING = "INIT fail!";
    /* Convert to char string */
    WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    SUB_LED_PRINT();                              /* Call subroutine to display text */
    do                                            /* Endless loop until manual reset */     
    {
    }
    while (1);  
  }
}


/****************************************************************************************/
/* Subroutine to save settings in init file Init.txt                                    */
/*      If SAVE_ALL_FLAG is true all actual parameters are saved, if SAVE_ALL_FLAG      */
/*      is false all parameters except parameters for text display are saved            */
/****************************************************************************************/
void SUB_WRITE_INIT_FILE (boolean SAVE_ALL_FLAG)
{ 
  #ifdef _DEBUG_
    if (SAVE_ALL_FLAG == true)                    /* Complete saving ? */
    {
      Serial.println ();
      Serial.println ("Complete saving of values!"); 
    }
    else                                          /* Saving without text display parameters ? */
    {
      Serial.println ();
      Serial.println ("Saving of values except text display parameters!"); 
    }
    Serial.print ("  Remove old Init.txt: ");
  #endif
  if(SPIFFS.remove("/Init.txt"))                  /* Remove existing init file successful ? */
  {
    #ifdef _DEBUG_
      Serial.println("Done");                     /* Print success mesage */ 
    #endif
  }
  else                                            /* Remove existing init file not successful ? */
  {
    #ifdef _DEBUG_
      Serial.println("Failed");                   /* Print failed mesage */
    #endif
    /* Override settings from init file */
    BRIGHTNESS_LEVEL = 4;                         /* Set high brightness level */        
    OWN_FONT_LIBRARY = true;                      /* Set own library flag true */
    CHAR_SPACING = 1;                             /* Set spacing between chars to one */
    INV_EFFECT = false;                           /* No inverting effect */  
    WEB_PAGE_STRING = "File error";
    /* Convert to char string */
    WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    SUB_LED_PRINT();                              /* Call subroutine to display text */
    do                                            /* Endless loop until manual reset */
    {      
    }
    while (1);   
  }

  #ifdef _DEBUG_
    Serial.print ("  Open Init.txt: ");
  #endif
  /* Open init file for write access */
  File FileToWrite = SPIFFS.open("/Init.txt", FILE_WRITE);
  if(!FileToWrite)                                /* File opening was not successful ? */
  {
    #ifdef _DEBUG_
      Serial.println("Failed!");                  /* Print failed mesage */ 
    #endif
    /* Override settings from init file */
    BRIGHTNESS_LEVEL = 4;                         /* Set high brightness level */        
    OWN_FONT_LIBRARY = true;                      /* Set own library flag true */
    CHAR_SPACING = 1;                             /* Set spacing between chars to one */
    INV_EFFECT = false;                           /* No inverting effect */  
    WEB_PAGE_STRING = "File error";
    /* Convert to char string */
    WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    SUB_LED_PRINT();                              /* Call subroutine to display text */
    do                                            /* Endless loop until manual reset */
    {      
    }
    while (1);  
  }
  else                                            /* File opening was successful ? */
  {
    #ifdef _DEBUG_
      Serial.println("Done");
      Serial.print("  Writing Init.txt file: ");
    #endif  
    FileToWrite.println(AP_WLAN_SSID);            /* Line 01: Write AP WLAN SSID */
    FileToWrite.println(AP_WLAN_PWD);             /* Line 02: Write AP WLAN password */
    FileToWrite.println(AP_FIXED_IP);             /* Line 03: Write AP fixed IP-Address */
    FileToWrite.println(AP_SUBNET);               /* Line 04: Write AP subnet mask */  
    FileToWrite.println(STA_WLAN_SSID);           /* Line 05: Write STA WLAN SSID */
    FileToWrite.println(STA_WLAN_PWD);            /* Line 06: Write STA WLAN password */      
    FileToWrite.println(STA_HOSTNAME);            /* Line 07: Write STA hostname */
    FileToWrite.println(STA_FIXED_IP);            /* Line 08: Write STA fixed IP-Address */
    FileToWrite.println(STA_SUBNET);              /* Line 09: Write STA subnet mask */
    FileToWrite.println(STA_GATEWAY);             /* Line 10: Write STA gateway IP-Address */
    FileToWrite.println(STA_DNS_1);               /* Line 11: Write DNS 1 IP-Address */
    FileToWrite.println(STA_DNS_2);               /* Line 12: Write DNS 2 IP-Address */ 
    FileToWrite.println(STA_BOOT_MODE);           /* Line 13: Write boot mode */ 
    if (SAVE_ALL_FLAG == true)                    /* Save all parameters ? */
    {
      FileToWrite.println(WEB_PAGE_STRING);       /* Line 14: Write text message */
      FileToWrite.println(SCROLLING_TEXT);        /* Line 15: Write scolling text flag */
      FileToWrite.println(BRIGHTNESS_LEVEL);      /* Line 16: Write brightness level */   
      FileToWrite.println(SPEED_LEVEL);           /* Line 17: Write speed level */
      if (OWN_FONT_LIBRARY == false)              /* Own font not selected */
      {
        FileToWrite.println("2");                 /* Line 18: Write value two to init file */
      }
      else                                        /* Own font selected */
      {
        FileToWrite.println("1");                 /* Line 18: Write value one to init file */
      }    
      FileToWrite.println(CHAR_SPACING);          /* Line 19: Write char spacing */
      FileToWrite.println(MESSAGE_TYPE);          /* Line 20: Write message type*/  
      if (INV_EFFECT == false)                    /* No inverting selected */
      {
        FileToWrite.println("1");                 /* Line 21: Write value one to init file */
      }
      else                                        /* Inverting selected */
      {
        FileToWrite.println("2");                 /* Line 21: Write value two to init file */
      }  
    } 
    else                                          /* Save all parameters except message parameters ? */
    {
      FileToWrite.println(ORIG_WEB_PAGE_STRING);  /* Line 14: Write text message */
      FileToWrite.println(ORIG_SCROLLING_TEXT);   /* Line 15: Write scolling text flag */
      FileToWrite.println(ORIG_BRIGHTNESS_LEVEL); /* Line 16: Write brightness level */   
      FileToWrite.println(ORIG_SPEED_LEVEL);      /* Line 17: Write speed level */
      if (OWN_FONT_LIBRARY == false)              /* Own font not selected */
      {
        FileToWrite.println("2");                 /* Line 18: Write value two to init file */
      }
      else                                        /* Own font selected */
      {
        FileToWrite.println("1");                 /* Line 18: Write value one to init file */
      }    
      FileToWrite.println(ORIG_CHAR_SPACING);     /* Line 19: Write char spacing */
      FileToWrite.println(ORIG_MESSAGE_TYPE);     /* Line 20: Write message type*/
      if (ORIG_INV_EFFECT == false)               /* No inverting selected */
      {
        FileToWrite.println("1");                 /* Line 21: Write value one to init file */
      }
      else                                        /* Inverting selected */
      {
        FileToWrite.println("2");                 /* Line 21: Write value two to init file */
      }  
    }      
    if (AUTOSTART_FLAG == false)                  /* No autostart selected */
    {
      FileToWrite.println("1");                   /* Line 22: Write value one to init file */
    }
    else                                          /* Autostart selected */
    {
      FileToWrite.println("2");                   /* Line 22: Write value two to init file */
    }               
    FileToWrite.close();                          /* Close file */
    #ifdef _DEBUG_  
      Serial.println("Done");
      Serial.println("  Reseting the device ...");
    #endif
    ESP.restart();                                /* Restarting ESP device */
    do                                            /* Endless loop until reset */
    {}
    while (1);
  }
}


/****************************************************************************************/
/* Subroutine to change configuration settings via console entry                        */
/****************************************************************************************/
void SUB_CONFIG_SETTING (void)
{
  char RECEIVE_CHAR;                              /* Received byte from file reading */
  byte RECEIVE_BYTE_COUNT;                        /* Received byte counter */ 
  char INPUT_STRING[255] = {0};                   /* Input string */
  char YES_NO_BUFFER[2];                          /* Buffer for Yes/No decision */
  
  /* Display welcome configuration message with basic information */
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println ("*********************************************************");
  Serial.println ("* Configuration of initial connection parameters        *");
  Serial.println ("* Press enter if parameter should remain as is          *");
  Serial.println ("* Please enter IP addresses in format: 192,168,123,219  *");   
  Serial.println ("*********************************************************\n"); 

  delay (800);                                    /* 800 ms delay */
 
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  

  /* Step 1: Change actual AP_WLAN SSID */
  Serial.print ("Actual AP WLAN SSID is \"");
  Serial.print (AP_WLAN_SSID);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    memset(AP_WLAN_SSID, 0, sizeof(AP_WLAN_SSID));/* Clear contents of AP_WLAN_SSID */ 
    strcat(AP_WLAN_SSID, INPUT_STRING);           /* Copy string */
  }
  Serial.println ();
  Serial.print ("  AP_WLAN SSID set to \"");
  Serial.print (AP_WLAN_SSID);
  Serial.println ("\"");

  /* Step 2: Change actual AP WLAN password */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual AP WLAN password is \"");
  Serial.print (AP_WLAN_PWD);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    memset(AP_WLAN_PWD, 0, sizeof(AP_WLAN_PWD));  /* Clear contents of AP_WLAN_SSID */ 
    strcat(AP_WLAN_PWD, INPUT_STRING);            /* Copy string */
  }
  Serial.println ();
  Serial.print ("  AP WLAN password set to \"");
  Serial.print (AP_WLAN_PWD);
  Serial.println ("\"");  

  /* Step 3: Change actual AP IP-Address */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual AP fixed IP-Address is \"");
  Serial.print (AP_FIXED_IP);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    AP_FIXED_IP.fromString(INPUT_STRING);
  } 

  Serial.println ();
  Serial.print ("  AP fixed IP-Address set to \"");
  Serial.print (AP_FIXED_IP);
  Serial.println ("\"");    

  /* Step 4: Change actual AP subnet mask */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual AP subnet mask is \"");
  Serial.print (AP_SUBNET);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    AP_SUBNET.fromString(INPUT_STRING);
  } 

  Serial.println ();
  Serial.print ("  AP subnet mask set to \"");
  Serial.print (AP_SUBNET);
  Serial.println ("\"");  

  /* Step 5: Change actual STA WLAN SSID */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }    
  Serial.print ("Actual STA WLAN SSID is \"");
  Serial.print (STA_WLAN_SSID);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    /* Clear contents of WLAN_SSID */
    memset(STA_WLAN_SSID, 0, sizeof(STA_WLAN_SSID));       
    strcat(STA_WLAN_SSID, INPUT_STRING);          /* Copy string */
  }
  Serial.println ();
  Serial.print ("  STA WLAN SSID set to \"");
  Serial.print (STA_WLAN_SSID);
  Serial.println ("\"");

  /* Step 6: Change actual STA WLAN password */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA WLAN password is \"");
  Serial.print (STA_WLAN_PWD);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    /* Clear contents of STA_WLAN_SSID */
    memset(STA_WLAN_PWD, 0, sizeof(STA_WLAN_PWD));         
    strcat(STA_WLAN_PWD, INPUT_STRING);           /* Copy string */
  }
  Serial.println ();
  Serial.print ("  STA WLAN password set to \"");
  Serial.print (STA_WLAN_PWD);
  Serial.println ("\"");  

  /* Step 7: Change actual STA hostname */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA hostname is \"");
  Serial.print (STA_HOSTNAME);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    /* Clear contents of STA hostname */
    memset(STA_HOSTNAME, 0, sizeof(STA_HOSTNAME));         
    strcat(STA_HOSTNAME, INPUT_STRING);           /* Copy string */
  }
  Serial.println ();
  Serial.print ("  STA hostname set to \"");
  Serial.print (STA_HOSTNAME);
  Serial.println ("\"");    

  /* Step 8: Change actual STA IP-Address */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA fixed IP-Address is \"");
  Serial.print (STA_FIXED_IP);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    STA_FIXED_IP.fromString(INPUT_STRING);
  } 

  Serial.println ();
  Serial.print ("  STA fixed IP-Address set to \"");
  Serial.print (STA_FIXED_IP);
  Serial.println ("\"");    

  /* Step 9: Change actual STA subnet mask */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA subnet mask is \"");
  Serial.print (STA_SUBNET);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    STA_SUBNET.fromString(INPUT_STRING);
  } 

  Serial.println ();
  Serial.print ("  STA subnet mask set to \"");
  Serial.print (STA_SUBNET);
  Serial.println ("\"");  

  /* Step 10: Change actual STA gateway IP-Address */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA gateway IP-Address is \"");
  Serial.print (STA_GATEWAY);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    STA_GATEWAY.fromString(INPUT_STRING);
  } 

  Serial.println ();
  Serial.print ("  STA gateway IP-Address set to \"");
  Serial.print (STA_GATEWAY);
  Serial.println ("\"");  

  /* Step 11: Change actual STA DNS server 1 IP-Address */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA DNS 1 IP-Address is \"");
  Serial.print (STA_DNS_1);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    STA_DNS_1.fromString(INPUT_STRING);
  } 
  Serial.println ();
  Serial.print ("  STA_DNS 1 IP-Address set to \"");
  Serial.print (STA_DNS_1);
  Serial.println ("\""); 

  /* Step 12: Change actual STA_DNS server 2 IP-Address */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual STA DNS 2 IP-Address is \"");
  Serial.print (STA_DNS_2);
  Serial.print ("\", enter new value (<20 chars) or press <ENTER> to keep current name: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 20));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 20))                         
  {
    STA_DNS_2.fromString(INPUT_STRING);
  } 
  Serial.println ();
  Serial.print ("  STA_DNS 2 IP-Address set to \"");
  Serial.print (STA_DNS_2);
  Serial.println ("\"");

  /* Step 13: Change actual boot mode */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual boot mode is set to \"");
  if(STA_BOOT_MODE == false)                      /* Access point selected ? */
  {
    Serial.print ("Accesss Point (AP)\"");
  }
  else                                            /* Station mode selected ? */
  {
    Serial.print ("Station Mode (STA)\"");
  }
  Serial.print (", enter new value (0 = Access Point, 1 = Station Mode) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                         
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 0) && (TEST_INT <=1))        /* Value in range ? */
    {
      if(TEST_INT == 0)                           /* Access point selected ? */
      {
        STA_BOOT_MODE = false;                    /* Set station boot mode false */
      }
      else                                        /* No selected ? */
      {
        STA_BOOT_MODE = true;                     /* Set station boot mode true */
      }
    }
  }
  Serial.println ();
  Serial.print ("  Boot mode set to \"");
  if(STA_BOOT_MODE == false)                      /* Access point selected ? */
  {
    Serial.println ("Accesss Point (AP)\"");
  }
  else                                            /* Station mode selected ? */
  {
    Serial.println ("Station Mode (STA)\"");
  }
   
  /* Step 14: Text message */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual text message is \"");
  Serial.print (WEB_PAGE_STRING);
  Serial.print ("\", enter new value (<255 chars) or press <ENTER> to keep current text message: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 255));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 255))                         
  {
    WEB_PAGE_STRING = INPUT_STRING;               /* Copy string */
  }
  Serial.println ();
  Serial.print ("  Text message set to \"");
  Serial.print (WEB_PAGE_STRING);
  Serial.println ("\"");    

  /* Step 15: Text scrolling */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual text scrolling is set to \"");
  if(SCROLLING_TEXT == false)                     /* No scrolling selected ? */
  {
   Serial.print ("No scrolling");
  }
  else                                            /* Scrolling selected ? */
  {
    Serial.print ("Scrolling"); 
  }
  Serial.print ("\", enter new value (0 = No scolling, 1 = Scrolling) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 0) && (TEST_INT <=1))        /* Value in range ? */
    {
      if(TEST_INT == 0)                           /* Access point selected ? */
      {
        SCROLLING_TEXT = false;                   /* Set scrolling text mode false */
      }
      else                                        /* No selected ? */
      {
        SCROLLING_TEXT = true;                    /* Set sscrolling text mode true */
      }
    }
  }
  Serial.println ();
  Serial.print ("  Text scrolling set to \"");
  if(SCROLLING_TEXT == false)                     /* No scrolling text selected ? */
  {
    Serial.println ("No scrolling\"");
  }
  else                                            /* Scrolling text selected ? */
  {
    Serial.println ("Scrolling\"");
  }

  /* Step 16: Brightness value */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual message brightness is set to \"");
  Serial.print (BRIGHTNESS_LEVEL);
  Serial.print ("\", enter new value (0 = Lowest, 4 = Brightest) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 0) && (TEST_INT <= 4))       /* Value is in range ? */
    {
      BRIGHTNESS_LEVEL = TEST_INT;                /* Save new brightness level */
    }
  }
  Serial.println ();
  Serial.print ("  Message brightness set to \"");
  Serial.print (BRIGHTNESS_LEVEL);
  Serial.println ("\" (0 = Lowest, 4 = Brightest)");

  /* Step 17: Speed value */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual message speed is set to \"");
  Serial.print (SPEED_LEVEL);
  Serial.print ("\", enter new value (0 = Fastest, 4 = Slowest) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 0) && (TEST_INT <= 4))       /* Value is in range ? */
    {
      SPEED_LEVEL = TEST_INT;                     /* Save new speed level */
    }
  }
  Serial.println ();
  Serial.print ("  Message speed set to \"");
  Serial.print (SPEED_LEVEL);
  Serial.println ("\" (0 = Fastest, 4 = Slowest)");
 
  /* Step 18: Font */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual font is set to \"");
  if (OWN_FONT_LIBRARY == true)                   /* Own font selected ? */
  {
    Serial.print ("Font 1 (Own font)");  
  }
  else                                            /* Standard font selected ? */
  {
    Serial.print ("Font 2 (Standard font)");
  }
  Serial.print ("\", enter new value (1 = Own font, 2 = Standard font) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 1) && (TEST_INT <= 2))       /* Value is in range ? */
    {
      if(TEST_INT == 1)                           /* Own library selected ? */
      {
        OWN_FONT_LIBRARY = true;                  /* Set own library true */    
      }  
      else                                        /* Standard font selected ? */
      {
        OWN_FONT_LIBRARY = false; 
      }
    }
  }
  Serial.println ();
  Serial.print ("  Font set to \"");
  if (OWN_FONT_LIBRARY == true)                   /* Own font selected ? */
  {
    Serial.println ("Font 1 (Own font)\"");  
  }
  else                                            /* Standard font selected ? */
  {
    Serial.println ("Font 2 (Standard font)\"");
  }

  /* Step 19: Char spacing */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Actual char spacing is set to \"");
  Serial.print (CHAR_SPACING );
  Serial.print ("\", enter new value (1-5 pixels) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 1) && (TEST_INT <= 5))       /* Value is in range ? */
    {
      CHAR_SPACING = TEST_INT;                    /* Save new char spacing */
    }
  }
  Serial.println ();
  Serial.print ("  Char spacing set to \"");
  Serial.print (CHAR_SPACING);
  Serial.println ("\" pixels");

  /* Step 20: Text type */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Text type is set to \"");
  if (MESSAGE_TYPE == 1)                          /* Message type text message selected ? */
  {
    Serial.print ("Text message");  
  }
  else                                            /* Message type test message selected ? */
  {
    Serial.print ("Test message");
  }
  Serial.print ("\", enter new value (1 = Text msg., 2 = Test msg.) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 1) && (TEST_INT <= 2))       /* Value is in range ? */
    {
      MESSAGE_TYPE = TEST_INT;                    /* Save message type */
    }
  }
  Serial.println ();
  Serial.print ("  Text type set to \"");
  if (MESSAGE_TYPE == 1)                          /* Message type text message selected ? */
  {
    Serial.println ("Text message\"");  
  }
  else                                            /* Message type test message selected ? */
  {
    Serial.println ("Test message\"");
  }

  /* Step 21: Inverting effect */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Inverting effect is set to \"");
  if (INV_EFFECT == false)                        /* Inverting effect not selected ? */
  {
    Serial.print ("No inverting");  
  }
  else                                            /* Message type test message selected ? */
  {
    Serial.print ("Inverting");
  }
  Serial.print ("\", enter new value (1 = No inverting, 2 = Inverting) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 1) && (TEST_INT <= 2))       /* Value is in range ? */
    {
      if(TEST_INT == 1)                           /* No inverting effect selected ? */
      {
        INV_EFFECT = false;                       /* Set inverting effect false */
      }
      else                                        /* Inverting effect selected ? */
      {
        INV_EFFECT = true;                        /* Set inverting effect true */
      }      
    }
  }
  Serial.println ();
  Serial.print ("  Inverting effect set to \"");
  if (INV_EFFECT == false)                        /* Inverting effect not selected ? */
  {
    Serial.println ("No inverting\"");  
  }
  else                                            /* Message type test message selected ? */
  {
    Serial.println ("Inverting\"");
  }

  /* Step 22: Autostart */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.print ("Autostart mode to display text message after boot is set to \"");
  if (AUTOSTART_FLAG == false)                    /* Autostart not selected ? */
  {
    Serial.print ("No autostart");  
  }
  else                                            /* Autostart selected ? */
  {
    Serial.print ("Autostart");
  }
  Serial.print ("\", enter new value (1 = No autostart, 2 = Autostart) or press <ENTER> to keep current value: ");
  memset(INPUT_STRING, 0, sizeof(INPUT_STRING));  /* Clear contents of input buffer */
  RECEIVE_BYTE_COUNT = 0;                         /* Reset byte counter */
  RECEIVE_CHAR = 32;                              /* Delete received char */
  do
  {
    if (Serial.available()>0)                     /* New character available ? */
    {
      RECEIVE_CHAR = Serial.read();               /* Read char if available */
      /* No CR and no LF received */
      if ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r'))
      {
        /* Save char */
        INPUT_STRING [RECEIVE_BYTE_COUNT] = RECEIVE_CHAR;
        Serial.print (RECEIVE_CHAR);
        RECEIVE_BYTE_COUNT++;                     /* Increase byte counter */
      }
    }
  }
  while ((RECEIVE_CHAR != '\n') && (RECEIVE_CHAR != '\r') && (RECEIVE_BYTE_COUNT < 2));

  /* Not ENTER pressed and string not too long --> Save value */
  if ((RECEIVE_BYTE_COUNT > 0) && (RECEIVE_BYTE_COUNT < 2))                           
  {
    TEST_INT = atoi (INPUT_STRING);               /* Convert char string to integer */
    if ((TEST_INT >= 1) && (TEST_INT <= 2))       /* Value is in range ? */
    {
      if(TEST_INT == 1)                           /* No autostart selected ? */
      {
        AUTOSTART_FLAG = false;                   /* Set autostart false */
      }
      else                                        /* Autostart selected ? */
      {
        AUTOSTART_FLAG = true;                    /* Set autostart true */
      }      
    }
  }
  Serial.println ();
  Serial.print ("  Autostart mode to display text message after boot set to \"");
  if (AUTOSTART_FLAG == false)                    /* Inverting effect not selected ? */
  {
    Serial.print ("No autostart\"");  
  }
  else                                            /* Message type test message selected ? */
  {
    Serial.print ("Autostart\"");
  }

  /* Step 23: Save changes ? */
  delay (800);                                    /* 800 ms delay */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  Serial.println ();
  Serial.print ("Save changes (y/n)? ");

  /* Wait for char */
  while( Serial.available() <= 0) 
  {
  }

  if ( Serial.readBytesUntil('\n', YES_NO_BUFFER, 2) > 0)
  { 
    Serial.println (YES_NO_BUFFER[0]);            /* Print answer */
    /* Save selected ? */
    if( ((String)YES_NO_BUFFER[0] == "y") || ((String)YES_NO_BUFFER[0] == "Y"))
    {
      Serial.println ("Saving selected - Saving data in Init.txt: ");
      SUB_WRITE_INIT_FILE (true);                 /* Save all values in Init.txt */
      SUB_READ_INIT_FILE ();                      /* Retrieve data from Init.txt */    
    }
    else                                          /* No saving ? */
    {
      Serial.println ("No saving selected - Retrieving data from Init.txt...");
      SUB_READ_INIT_FILE ();                      /* Retrieve data from Init.txt */ 
    }
  }
}


/****************************************************************************************/
/****************************************************************************************/
/* WLAN routines                                                                        */
/****************************************************************************************/
/****************************************************************************************/


/****************************************************************************************/
/* Subroutine to connect to WLAN in Station Mode                                        */
/****************************************************************************************/
void SUB_STA_WLAN_CONNECT (void)
{
  uint16_t WLAN_COUNTER;                          /* WLAN counter to check successfull connection */
  int      WLAN_STATUS;                           /* Actual WLAN status */
  boolean  PING_STATUS;                           /* True = Responding to ping, False = No response to ping */
  boolean  STA_WLAN_CONNECT;                      /* True = STA WLAN connected, False = STA WLAN not connected */

  STA_WLAN_CONNECT = true;                        /* Set STA WLAN connect = true */
  /* Override settings from init file */
  BRIGHTNESS_LEVEL = 4;                           /* Set high brightness level */        
  OWN_FONT_LIBRARY = true;                        /* Set own library flag true */
  CHAR_SPACING = 1;                               /* Set spacing between chars to one */
  INV_EFFECT = false;                             /* No inverting effect */  
  WEB_PAGE_STRING = "STA-Mode";
  /* Convert to char string */
  WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */
  SUB_LED_PRINT();                                /* Call subroutine to display text */
  do                                              /* Repeat until STA WLAN is connected */
  {
    #ifdef _DEBUG_
      Serial.print("  Disconnecting from WLAN: ");  
    # endif 
    /* Step A: Disconnect from WLAN if still connected */
    if (WiFi.status() == WL_CONNECTED)            /* STA WLAN connected? */
    {
      WiFi.disconnect();                          /* Disconnect from STA WLAN */
      #ifdef _DEBUG_
        Serial.println("Disconnected");
      # endif 
    }  
    else                                          /* WLAN not connected? */
    {
      #ifdef _DEBUG_
        Serial.println("No connection");
      # endif 
    }   
    
    /* Step B: Set STA fixed IP-Address */
    #ifdef _DEBUG_
      Serial.print("  Setting fixed STA IP-Address: "); 
    # endif
    /* Set fixed IP-Address failes ? */
    if (!WiFi.config(STA_FIXED_IP, STA_GATEWAY, STA_SUBNET, STA_DNS_1, STA_DNS_2)) 
    {
      #ifdef _DEBUG_
        Serial.println("Failed");
      # endif
      STA_WLAN_CONNECT = false;                   /* Set STA WLAN connect = false = Error */
    } 
    else                                          /* Set fixed IP-Address OK ? */
    {
      #ifdef _DEBUG_
        Serial.println("Done");
      # endif
    }

    /* Step C: Disable WLAN Sleep and config STA station mode */
    WiFi.setSleep(false);                         /* Switch sleep mode off */ 
    WiFi.setHostname(STA_HOSTNAME);               /* Set STA hostname for ESP */
    WiFi.mode (WIFI_STA);                         /* Configure station mode */
    
    if (STA_WLAN_CONNECT == true)                 /* No error yet ? */
    {
      /* Step D: Connecting to WLAN Network */
      #ifdef _DEBUG_
        Serial.print("  Connecting to WLAN ");
        Serial.print(STA_WLAN_SSID); 
        Serial.print(": "); 
      # endif
      WiFi.begin(STA_WLAN_SSID, STA_WLAN_PWD);    /* Connecting to STA WLAN */
      
      WLAN_COUNTER = 0;                           /* Set WLAN counter 0 */
      /* WLAN connected or 20 trials done ? */
      while ((WiFi.status() != WL_CONNECTED) && (WLAN_COUNTER < 20)) 
      {
        delay (100);                              /* Wait hundret miliseconds */
        WLAN_COUNTER++;                           /* Increment WLAN counter */
      }

      if (WLAN_COUNTER >= 20)                     /* Error connecting */
      {
        #ifdef _DEBUG_
          Serial.println("Failed");
        # endif
        WiFi.disconnect();                        /* Disconnect from STA WLAN */
        delay (1000);                             /* Waiting one second */
        #ifdef _DEBUG_
          Serial.print("  Reconnecting to STA_WLAN ");
          Serial.print(STA_WLAN_SSID); 
          Serial.print(": "); 
        # endif  
        WiFi.begin(STA_WLAN_SSID, STA_WLAN_PWD);  /* Connecting to WLAN */
        WLAN_COUNTER = 0;                         /* Set WLAN counter 0 */
        /* WLAN connected or 20 trials done ? */
        while ((WiFi.status() != WL_CONNECTED) && (WLAN_COUNTER < 20)) 
        {
          delay (1000);                            /* Wait hundret miliseconds */
          WLAN_COUNTER++;                         /* Increment WLAN counter */
        }
      }
      
      if (WLAN_COUNTER >= 20)                     /* Error connecting */
      {
        #ifdef _DEBUG_
          Serial.println("Failed");
        # endif       
      }
      else                                        /* Successfull connection ? --> Start ping */
      {
        /* Display WLAN information */
        #ifdef _DEBUG_
          Serial.println("Done"); 
          Serial.print ("  Ping server for connectivity check: ");
          Serial.print (STA_GATEWAY);
          Serial.print(": "); 
        # endif
        PING_STATUS = Ping.ping(STA_GATEWAY, 4);  /* Ping to STA IP-Address */
        if (PING_STATUS == true)                  /* Sucessful ping? */
        {
          #ifdef _DEBUG_
            Serial.println("Success");
          # endif
        }
        else                                      /* Unsucessful ping? */
        {
          #ifdef _DEBUG_
            Serial.println("Failed");
          # endif
          WLAN_COUNTER = 21;                      /* Set WLAN counter to error position */
        }  
      }
      
      if (WLAN_COUNTER >= 20)                     /* Error connecting or pinging ? */
      { 
        WiFi.disconnect();                        /* Wifo disconnect */
        WiFi.mode (WIFI_OFF);                     /* Switch WLAN off */
        WEB_PAGE_STRING = "STA-Error!";
        /* Convert to char string */
        WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
        WEB_MESSAGE_AVAILABLE = true;             /* Set new message available true */
        SUB_LED_PRINT();                          /* Call subroutine to display text */
        do                                        /* Endless loop until reset */
        {}
        while (1);
      }
      else                                        /* WLAN connected sucessfully and ping OK ? */
      {
        #ifdef _DEBUG_
          /* Display WLAN information */
          Serial.print("    IP-Address: ");
          Serial.println(WiFi.localIP());
          Serial.print("    ESP Mac Address: ");
          Serial.println(WiFi.macAddress());
          Serial.print("    Subnet Mask: ");
          Serial.println(WiFi.subnetMask());
          Serial.print("    Gateway IP: ");
          Serial.println(WiFi.gatewayIP()); 
          Serial.print("    Hostname: ");
          Serial.println(WiFi.getHostname());         
          Serial.print("    Signal strength (RSSI): ");
          Serial.print(WiFi.RSSI());
          Serial.println(" dBm");
          Serial.print("    WLAN status: ");
          WLAN_STATUS = WiFi.status();
          switch (WLAN_STATUS)
          {
            case 255:
              Serial.println("WL_NO_SHIELD");
              break;
            case 0:
              Serial.println("WL_IDLE_STATUS");
              break;
            case 1:
              Serial.println("WL_NO_SSID_AVAIL");
              break;
            case 2:
              Serial.println("WL_SCAN_COMPLETED");
              break;
            case 3:
              Serial.println("WL_CONNECTED");
              break;
            case 4:
              Serial.println("WL_CONNECT_FAILED");
              break;
            case 5:
              Serial.println("WL_CONNECTION_LOST");
              break;
            case 6:
              Serial.println("WL_DISCONNECTED");
              break;
            default:
              Serial.println("_UNKNOWN");
              break;
          }          
          Serial.print("    Use this URL: http://");
          Serial.print(WiFi.localIP());
          Serial.println("/main");          
        # endif 
        STA_WLAN_CONNECT = true;                  /* Set WLAN connect flag to true */
      }
    }       
  }
  while (STA_WLAN_CONNECT == false);
}


/****************************************************************************************/
/* Subroutine to create WLAN access point                                               */
/****************************************************************************************/
void SUB_AP_WLAN (void)
{
  /* Override settings from init file */
  BRIGHTNESS_LEVEL = 4;                           /* Set high brightness level */        
  OWN_FONT_LIBRARY = true;                        /* Set own library flag true */
  CHAR_SPACING = 1;                               /* Set spacing between chars to one */
  INV_EFFECT = false;                             /* No inverting effect */  
  WEB_PAGE_STRING = "AP-Mode";
  /* Convert to char string */
  WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */
  SUB_LED_PRINT();                                /* Call subroutine to display text */
  WiFi.mode(WIFI_AP);                             /* Set to Access point mode */
  WiFi.softAP(AP_WLAN_SSID, AP_WLAN_PWD);         /* Initialize the WLAN Soft Access Point */
  delay(100);                                     /* Wait 100ms for AP start */
  /* Configure IP-Address for access point */
  WiFi.softAPConfig(AP_FIXED_IP, AP_FIXED_IP, AP_SUBNET);  
  #ifdef _DEBUG_
    Serial.println("Done");                       /* Display success message */
    Serial.print("    Use this URL: http://");
    Serial.print (WiFi.softAPIP());
    Serial.println("/main");  
  # endif 
  delay (2000);                                   /* Display for 2 seconds */
}


/****************************************************************************************/
/* Subroutine to initialize WLAN bootloader and wait for upload                         */
/* Port defaults to 3232 (ArduinoOTA.setPort(3232))                                     */
/* Hostname defaults to esp3232-[MAC] (ArduinoOTA.setHostname("myesp32"))               */
/* No authentication by default (ArduinoOTA.setPassword("admin"))                       */
/* Password can be set with it's md5 value as well                                      */
/*      MD5(admin) = 21232f297a57a5a743894a0e4a801fc3                                   */
/*      ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3")                  */
/****************************************************************************************/
void SUB_WLAN_BOOTLOADER (void)
{  
  #ifdef _DEBUG_
    Serial.println();
    Serial.print ("Initializing bootloader: ");
  # endif  
    
  ArduinoOTA
    .onStart([]()                                 /* On start */
    {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)     /* Sketch upload ? */
        type = "sketch";
      else                                        /* U_SPIFFS selected ? */
        type = "filesystem";
      /* NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end() */
      Serial.println("Start updating " + type);
    })
    
    .onEnd([]()                                   /* Finished upload */ 
    {
      Serial.println("\nEnd");
    })

    /* On progress */
    .onProgress([](unsigned int progress, unsigned int total) 
    {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    
    .onError([](ota_error_t error)                /* On error */
    {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();                             /* Start OTA */

    WEB_PAGE_STRING = "Bootloader";
  /* Convert to char string */
  WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */
  SUB_LED_PRINT();                                /* Call subroutine to display text */ 

  /* Endless loop to wait for request from Arduino IDE */ 
  do
  {
    ArduinoOTA.handle();                          /* OTA handler */
  }
  while (true);
}


/****************************************************************************************/
/* Subroutine to replace special characters by new values as function "toCharArray"     */
/* handles special characters > ASCII 127 wrong by splitting it into two bytes          */
/****************************************************************************************/
String SUB_REPLACE_SPECIAL_CHARACTERS(String newString) 
{
  newString.replace("Ö", "\326");                 /* Replace with ASCII 214 */
  newString.replace("ö", "\366");                 /* Replace with ASCII 246 */
  newString.replace("Ä", "\304");                 /* Replace with ASCII 196 */
  newString.replace("ä", "\344");                 /* Replace with ASCII 228 */
  newString.replace("Ü", "\334");                 /* Replace with ASCII 220 */
  newString.replace("ü", "\374");                 /* Replace with ASCII 252 */
  newString.replace("ß", "\337");                 /* Replace with ASCII 223 */
  newString.replace("€", "\200");                 /* Replace with ASCII 128 */ 
  return newString;
}
