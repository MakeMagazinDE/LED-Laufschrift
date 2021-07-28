/****************************************************************************************/
/* WLAN LED Laufschrift with ESP32                                                      */
/* WLAN web server allowing to scroll a message entered from a webpage                  */
/* on a display consisting of 8 8x8 LED arrays (Display module MAX7219)                 */
/* Version: 2.0                                                                         */
/* Board: ESP32 Pico Kit                                                                */
/* Partition scheme: Default                                                            */
/* Date: 10.10.2020                                                                     */
/* Compiled with Arduino 1.8.12                                                         */
/* ESP32 arduino extension: 1.0.4                                                       */
/*                                                                                      */
/* FLASH handling: https://randomnerdtutorials.com/esp32-flash-memory/                  */
/* ESP Async Web Server:                                                                */
/*   https://techtutorialsx.com/2017/12/01/esp32-arduino-asynchronous-http-webserver/   */
/*                                                                                      */
/* Libraries:                                                                           */
/*            MD_MAX72XX-master                                                         */
/*                NOTE!!!! In order to include the library in the program directory     */
/*                in file MD_MAX72xx_lib.h line 27 #include <MD_MAX72xx.h>              */
/*                must be commented out: //#include <MD_MAX72xx.h>                      */
/*            ESP32 Async Web Server: https://github.com/me-no-dev/ESPAsyncWebServer    */
/*            Async TCP: https://github.com/me-no-dev/AsyncTCP                          */
/*            Fastled: https://github.com/FastLED/FastLED                               */
/*            ESP32 Ping: https://github.com/marian-craciunescu/ESP32Ping               */
/*            Wifi: Part of ESP32 library                                               */
/*            SPIFFS: Part of ESP32 library                                             */
/*                                                                                      */
/* Other tools:                                                                         */
/*        ESP32 Filesystem Uploader:                                                    */
/*        https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide */
/*                                                                                      */
/* Usage of serial interfaces: Serial for debug interface                               */
/* Pin asignments:                                                                      */
/*     GPIO12 = MAX72x data pin                                                         */
/*     GPIO14 = MAX72x CS pin                                                           */
/*     GPIO27 = MAX72x CLK pin                                                          */
/*                                                                                      */
/* INIT.TXT: Init file with basic access parameters which must be uploaded via          */
/*           ESP32 Sketch Data Upload                                                   */
/*      File content:                                                                   */
/*           Line  1: Access Point AP: WLAN SSID, e.g. LED_TEXT                         */
/*           Line  2: Access Point AP: WLAN Password, e.g. default                      */
/*           Line  3: Access Point AP: Fixed IP-Address, e.g. 192.168.122.2             */
/*           Line  4: Access Point AP: Subnet mask, e.g. 255.255.255.0                  */
/*                                                                                      */
/*           Line  5: Station Mode STA: WLAN SSID, e.g. LED_IOT                         */
/*           Line  6: Station Mode STA: WLAN Password, e.g. default                     */
/*           Line  7: Station Mode STA: Hostname, e.g. Lauflicht                        */
/*           Line  8: Station Mode STA: Fixed IP-Address, e.g. 192.168.123.234          */
/*           Line 9:  Station Mode STA: Subnet mask, e.g. 255.255.255.0                 */
/*           Line 10: Station Mode STA: Gateway IP-Address, e.g. 192.168.123.1          */
/*           Line 11: Station Mode STA: DNS Server 1, e.g. 195.58.161.123               */
/*           Line 12: Station Mode STA: DNS Server 2, e.g. 212.186.211.21               */
/*                                                                                      */
/*           Line 13: Boot mode: 0 = Access Point, 1 = Station Mode, e.g. 0             */
/*           Line 14: LED display: Text, e.g. Demo text                                 */
/*           Line 15: LED display: Scrolling (0-1), 0 = No scolling,                    */
/*                                 1 = Scrolling, e.g. 0                                */
/*           Line 16: LED display: Brightness level (0-4), 0 = Lowest, 4 = Highest,     */
/*                                 e.g. 2                                               */
/*           Line 17: LED display: Speed level (0-4), 0 = Fastest, 4 = Slowest          */
/*                                 e.g. 2                                               */
/*           Line 18: LED display: Font (1-2), 1 = Own font, 2 = Standard font, e.g. 1  */
/*           Line 19: LED display: Char spacing (1-5), 1-5 Char spacings, e.g. 1        */
/*           Line 20: LED display: Text type (1-2), 1 = Text msg., 2 = Test msg., e.g. 1*/
/*           Line 21: LED display: Effect (1-2), 1 = No inverting, 2 = Inverting, e.g. 1*/
/*           Line 22: LED display: Autostart (1-2), 1 = No autostart, 2 = Autostart,    */
/*                                 e.g. 2                                               */
/*      Warning: No ENTER (CR/LF) allowed after line 22 !!!                             */  
/*                                                                                      */
/* FLASH structure                                                                      */
/*                        0 = REMOTE UPDATE VIA WLAN                                    */
/*                              0 = Normal boot                                         */
/*                              1 = Remote update                                       */
/*                                                                                      */
/* IP COMMAND REFERENCE                                                                 */
/*                                                                                      */
/* Message format for Ping                                                              */
/*  http://192.168.123.234/?login=<LOGIN>:<PASSWORD>/?p                                 */
/*  Message feedback: OK                                                                */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          /?p = Ping                                                                  */
/*                                                                                      */
/*  Message format for set bootloader                                                   */
/*  http://192.168.123.234/?login=<LOGIN>:<PASSWORD>/b=x                                */
/*  Message feedback: OK oder Error                                                     */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          b=0       Normal boot --> Nothing will happen                               */
/*          b=1       Bootloader via WLAN                                               */
/*                                                                                      */
/*  Message format for reset                                                            */
/*  http://192.168.123.234/?login=<LOGIN>:<PASSWORD>/r=x                                */
/*  Message feedback: OK oder Error                                                     */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          r=0       No reset --> Nothing will happen                                  */
/*          r=1       Reset without saving                                              */
/*                                                                                      */
/*  Message format for display test                                                     */
/*  http://192.168.123.234/?login=<LOGIN>:<PASSWORD>/d=x                                */
/*  Message feedback: OK oder Error                                                     */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          d=0       Stop dispaly test                                                 */
/*          d=1       Start display test                                                */
/*                                                                                      */
/*  Message format for static text                                                      */
/*  http://192.168.123.219/?login=<LOGIN>:<PASSWORD>/t=Text;b=Brightness;f=Font;        */
/*                                                   c=Char spacing,e=Effect            */
/*  Message feedback: OK oder Error                                                     */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          t         Text string                                                       */
/*          b=0-4     Brightness level (0-4), 0 = Lowest, 4 = Highest                   */
/*          f=1-2     Font (1-2), 1 = Own font, 2 = Standard font                       */
/*          c=1-5     Char spacing (1-5), 1-5 Char spacings                             */
/*          e=1-2     Effect (1-2), 1 = No inverting, 2 = Inverting                     */
/*                                                                                      */
/*  Message format for scrolling text                                                   */
/*  http://192.168.123.219/?login=<LOGIN>:<PASSWORD>/s=Text;s=Speed;b=Brightness;       */
/*                                                   f=Font;c=Char spacing,             */
/*  Message feedback: OK oder Error                                                     */
/*  <LOGIN> = Login Name                                                                */
/*  <PASSWORD> = Password                                                               */
/*          s         Scrolling text string                                             */
/*          e=0-4     Speed level (0-4), 0 = Fastest, 4 = Slowest                       */
/*          b=0-4     Brightness level (0-4), 0 = Lowest, 4 = Highest                   */
/*          f=1-2     Font (1-2), 1 = Own font, 2 = Standard font                       */
/*          c=1-5     Char spacing (1-5), 1-5 Char spacings                             */
/*                                                                                      */
/****************************************************************************************/


/****************************************************************************************/
/* Libraries                                                                            */
/****************************************************************************************/
#include <EEPROM.h>                               /* Include EEPROM read/write */
#include <WiFi.h>                                 /* Include ESP32 WIFI library */
#include <ESPAsyncWebServer.h>                    /* Include ESP32 Async Web Server librarby */ 
#include "SPIFFS.h"                               /* Include ESP32 library to access files */
#include "src/MD_MAX72XX/MD_MAX72xx.h"            /* Include library for MAX72x */
#include "fontdata.h"                             /* Include file for character set definitions */
#include <ESP32Ping.h>                            /* Include ESP32 ping library */
/* Libraries for OTA */
#include <ESPmDNS.h>                              /* Include ESP DNS library */
#include <ArduinoOTA.h>                           /* Include OTA library */


/****************************************************************************************/
/* Definitions of hardware pins                                                         */
/****************************************************************************************/
#define MAX72_DATA_PIN  12                        /* MAX72x Data Pin */
#define MAX72_CS_PIN    14                        /* MAX72x CS Pin */ 
#define MAX72_CLK_PIN   27                        /* MAX72x CLK Pin */  


/****************************************************************************************/
/* Definitions for general settings                                                     */
/****************************************************************************************/
/* If this is uncommented here, status messages will be output via the serial interface, if commented out then disabled ! */
//#define _DEBUG_ 
#define EEPROM_SIZE 2                             /* Define the number of bytes of FLASH to be used */
#define MAX_CHARACTERS 255                        /* Maximum length of the string */
#define MAX_SEGMENTS 8                            /* Number of used Led matrixes */


/****************************************************************************************/
/* Definitions for WLAN settings and handling                                           */
/****************************************************************************************/
char AP_WLAN_SSID[21] = {0};                      /* String for Access Point WLAN_SSID */
char AP_WLAN_PWD [21] = {0};                      /* String for Access Point password of WLAN */
IPAddress AP_FIXED_IP;                            /* Fixed IP-Address for Access Point */
IPAddress AP_SUBNET;                              /* Subnet mask for Access Point */
char STA_WLAN_SSID[21] = {0};                     /* String for Station Mode WLAN_SSID */
char STA_WLAN_PWD [21] = {0};                     /* String for Station Mode password of WLAN */
char STA_HOSTNAME [21] = {0};                     /* String for Station Mode hostname */
IPAddress STA_FIXED_IP;                           /* Fixed IP-Address for Station Mode */
IPAddress STA_SUBNET;                             /* Subnet mask for Station Mode */
IPAddress STA_GATEWAY;                            /* Gateway IP-Address for Station Mode */
IPAddress STA_DNS_1;                              /* DNS Server 1 IP-Address for Station Mode */
IPAddress STA_DNS_2;                              /* DNS Server 2 IP-Address for Station Mode */


/****************************************************************************************/
/* Definitions for Webserver                                                            */
/****************************************************************************************/
AsyncWebServer server(80);                        /* Create AsyncWebServer object on port 80 */


/****************************************************************************************/
/* Definitions for object MD_Max                                                        */
/****************************************************************************************/
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW         /* Define hardware type */
/* Instantiation of the object MD_Max */
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, MAX72_DATA_PIN, MAX72_CLK_PIN, MAX72_CS_PIN, MAX_SEGMENTS);


/****************************************************************************************/
/* Constants                                                                            */
/****************************************************************************************/
const char* SW_VERSION = "2.0";                   /* Actual SW version */
const uint8_t MESSAGE_SIZE = 255;                 /* Maximum length of message */

/* Values for message type */
const char *MENU_OPTION_1[] = { "Textnachricht", "Testnachricht"};
/* Values for message speed */
const char *MENU_OPTION_2[] = { "Sehr schnell", "Schnell", "Mittel", "Langsam", "Sehr langsam"};
/* Values for message brightness */
const char *MENU_OPTION_3[] = { "Sehr dunkel", "Dunkel", "Mittel", "Hell", "Sehr hell"};
/* Values for char spacing */
const char *MENU_OPTION_4[] = { "1", "2", "3", "4","5" };
/* Values for font selection */
const char *MENU_OPTION_5[] = { "Schrift 1", "Schrift 2"};
/* Values for effect */
const char *MENU_OPTION_6[] = { "Kein", "Invertiert"};
/* Values for test mode */
const char *MENU_OPTION_7[] = { "Aus", "Ein"};
/* Values for boot mode */
const char *MENU_OPTION_8[] = { "Access Point", "WLAN Client"};
/* Values for autostart, reset and permanent storage */
const char *MENU_OPTION_9[] = { "Nein", "Ja"};

const char* KEYWORD = "login";                    /* Define keyword to be checked */
const char* LOGIN = "admin:default/";             /* Define login and password to be checked */ 


/****************************************************************************************/
/* Variables                                                                            */
/****************************************************************************************/
boolean WEB_MESSAGE_AVAILABLE = false;            /* Available Webmessage, false = not available, true = available */
boolean SCROLLING_TEXT;                           /* Scrolling of text: true = Scroll text, false = No scrolling */
boolean ORIG_SCROLLING_TEXT;                      /* Initial read scrolling flag */
boolean OWN_FONT_LIBRARY;                         /* Font library select: False = Built in font, True = Library from fontdate.h */
boolean ORIG_OWN_FONT_LIBRARY;                    /* Initial read font flag */
boolean DISPLAY_TEST;                             /* Enables the test display sequence: true = Display test, false = No display test */
boolean INV_EFFECT;                               /* Inverting effect: true = Inversion, false = No inversion */
boolean ORIG_INV_EFFECT;                          /* Initial read inverting effect */
boolean STA_BOOT_MODE;                            /* Boot mode: false = AP mode, true = STA mode */
boolean AUTOSTART_FLAG;                           /* Autostart: false = No autostart, true = Autostart of text message */
boolean SAVE_FLAG;                                /* Flag how to save message parameters in Init file: false = do not save message parameters, true = save message parameters */
boolean REBOOT_FLAG;                              /* Flag to reboot the device false = no reboot, true = reboot */
boolean REBOOT_SAVE_FLAG;                         /* Flag to be returned on how to save message parameters in Init file: false = do not save message parameters, true = save message parameters */
boolean END_SCROLL_MESSAGE;                       /* Flag to show that scrolling ended, False = still scrolling, True = Scrolling ended */
boolean BOOTLOADER_FLAG;                          /* Flag to start bootloader, False = Do not go in bootloader mode, True = Go in bootloader mode */
boolean RESET_FLAG;                               /* Flag to reset the device false = no reset, true = reset */
byte LOOP_COUNTER;                                /* Universal loop counter  */
byte CHAR_SPACING;                                /* Char spacing value */
byte ORIG_CHAR_SPACING;                           /* Initial read char spacing value */
byte MESSAGE_TYPE;                                /* Message type: 1=Textnachricht, 2=Testnachricht */
byte ORIG_MESSAGE_TYPE;                           /* Initial read message type */
byte BRIGHTNESS_LEVEL;                            /* Light intensity level of display (0-4) */
byte ORIG_BRIGHTNESS_LEVEL;                       /* Initial read brightness level */
byte BRIGHTNESS_LEVELS[] = {2, 7, 10, 13, 15};    /* Define intensity levels, 2 = darkest, 15 = brightests*/  
byte SPEED_LEVEL;                                 /* Scrolling speed of the display (0-4) */
byte ORIG_SPEED_LEVEL;                            /* Initial read speed level */
byte SPEED_LEVELS[] = {10, 20, 30, 40, 50};       /* Define speed levels, 10 fastest, 50 slowest */
int TEST_INT;                                     /* Test integer for conversion */
char DISPLAY_MESSAGE[MESSAGE_SIZE];               /* String to be displayed on LED display */
String WEB_PAGE_STRING = "";                      /* String for displayimg the webpage */
String ORIG_WEB_PAGE_STRING = "";                 /* Initial read webpage sring */
String RECEIVE_MESSAGE;                           /* String for receiving messages */
String SUBST_STRING;                              /* Substitution string for web page handling */
String TEMP_BUFFER;                               /* String for string manipulation */


/****************************************************************************************/
/* Setup procedure                                                                      */
/****************************************************************************************/
void setup()
{
  int SERIAL_AVAILABLE;                           /* Wait for serial available */  
  char RECEIVE_CHAR;                              /* Received char */
  
  /* Initialize FLASH size */
  EEPROM.begin(EEPROM_SIZE);                      /* Initialize FLASH with predefined size */
  
  /* Init serial interface for debug information */
  Serial.begin(115200);                           /* Start serial interface for terminal communication at 115000 Baud */
  /* Display welcome message with initialisation information */
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println ("*********************************************************");
  Serial.println ("* LED text scrolling display with ESP32                 *");
  Serial.println ("*                                                       *");
  Serial.print   ("* V");
  Serial.print   (SW_VERSION);
  Serial.println ("                                      @PT 2020    *");   
  Serial.println ("*********************************************************\n"); 

  #ifdef _DEBUG_
    Serial.println("\n\n\n"); 
    Serial.println ("Debug is enabled...");
    Serial.println(); 
  #else
    Serial.println("\n\n\n"); 
    Serial.println ("Debug is disabled...");
    Serial.println ("Remove commented out line #define _DEBUG_ to enable!");
    Serial.println();     
  # endif 
  
  /* Read init parameters from FLASH */
  LOOP_COUNTER = EEPROM.read(0);                  /* Read first position out of FLASH */
  if ((LOOP_COUNTER !=0) && (LOOP_COUNTER !=1))   /* FLASH never written before? --> First intialisation */
  {
    #ifdef _DEBUG_
      Serial.println ("Initialisation of FLASH memory ...");  
    # endif 
    EEPROM.write(0, 0);                           /* Set value for normal boot */ 
  }
  /* Read out FLASH values */
  #ifdef _DEBUG_
    Serial.print ("Memory location 0: Remote update flag: "); 
    Serial.print (EEPROM.read(0));
    Serial.println (" (Normal boot = 0, Remote update = 1)");
  # endif 
  
  /* Step 1: Initialize LED-display and display first message */
  #ifdef _DEBUG_
    Serial.println();
    Serial.print ("Step 1: Initialise LED-Display: ");
  # endif  
  /* LED matrix initialization */
  mx.begin();                                     /* Start MAX72XX driver */
  mx.setShiftDataInCallback(SUB_SHIFT_DATA_IN);   /* Define callback subroutine to shift data in */
  /* Display init message on LED display */
  WEB_PAGE_STRING = "Initialisation";
  /* Convert to char string */
  WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
  BRIGHTNESS_LEVEL = 4;                           /* Set high brightness level */        
  OWN_FONT_LIBRARY = true;                        /* Set own library flag true */
  CHAR_SPACING = 1;                               /* Set spacing between chars to one */
  INV_EFFECT = false;                             /* No inverting effect */
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */
  SUB_LED_PRINT();                                /* Call subroutine to display text */
  #ifdef _DEBUG_
    Serial.println("Done");                       /* Display success message */ 
  # endif

   /* Step 2: Initialize SPIFFS */
  #ifdef _DEBUG_
    Serial.println();
    Serial.println ("Step 2: Initialise file access and read Init.txt...");
    Serial.print("  Initialize file access: ");
  # endif 
  if(!SPIFFS.begin(true))                         /* Initialisation error ? */
  {  
    #ifdef _DEBUG_                                          
      Serial.println("Failed!");
    # endif
    WEB_PAGE_STRING = "SPIFFS fail!";
    /* Convert to char string */
    WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
    SUB_LED_PRINT();                              /* Call subroutine to display text */
    do                                            /* Endless loop until manual reset */
    {      
    }
    while (1);  
  }
  else                                            /* No error ? */
  { 
    #ifdef _DEBUG_                                            
      Serial.println("Done");                     /* Display success message */
    # endif  
    SUB_READ_INIT_FILE();                         /* Call subroutine to read init file */
  } 

  /* Check if any key is pressed */
  while( Serial.available())                      /* Clear incoming serial buffer */
  { 
    RECEIVE_CHAR = Serial.read();                 /* Read char if available */
  }  
  #ifdef _DEBUG_ 
    Serial.println("\n\n\n"); 
  # endif
  Serial.println(">> Press enter to change Init.txt settings within 3 seconds ...");
  SERIAL_AVAILABLE = 0;                           /* Serial available */
  /* Wait for new char received or timeout */
  while ((Serial.available() <= 0) && (SERIAL_AVAILABLE < 300))
  {                                               
    delay (10);                                   /* 10ms delay */
    SERIAL_AVAILABLE++;                           /* Increase serial available counter */
  }
  if (SERIAL_AVAILABLE < 300)                     /* Enter configuration mode */
  {
    SUB_CONFIG_SETTING();                         /* Call subroutine to configure settings */
  }
  
  /* Check on bootloader */
  if (EEPROM.read(0) == 1)                        /* Bootloader ? */
  {  
    /* Next boot = normal boot */
    EEPROM.write(0, 0);                           /* Set REMOTE UPLOAD FLAG: for 0=Normal  */
    EEPROM.commit();                              /* Write to FLASH memory */
    #ifdef _DEBUG_
      Serial.println("Starting to connect to STA WLAN ..."); 
    # endif 
    SUB_STA_WLAN_CONNECT();                       /* Call sub routine to connect to STA WLAN */ 
    SUB_WLAN_BOOTLOADER();                        /* Call sub routine for bootloader */    
  }

  /* Step 3: Create WLAN Access point or connect to WLAN in station mode */
  if (STA_BOOT_MODE == true)                      /* Station mode selected ? */
  {
    #ifdef _DEBUG_
      Serial.println();
      Serial.print ("Step 3: Connect to WLAN with SSID '");
      Serial.print (STA_WLAN_SSID);
      Serial.println ("...");
    # endif 
    SUB_STA_WLAN_CONNECT();                       /* Call sub routine to connect to STA WLAN */
  }                                  
  else                                            /* Access point mode selected ? */
  {
    #ifdef _DEBUG_
      Serial.println();
      Serial.print ("Step 3: Create WLAN Access point with SSID '");
      Serial.print (AP_WLAN_SSID);
      Serial.print ("' and password '");
      Serial.print (AP_WLAN_PWD);
      Serial.print ("': ");
    # endif 
    SUB_AP_WLAN ();                               /* Call subroutine to create WLAN access point */
  }

  /* Step 4: Define routes for answers from web page */
  #ifdef _DEBUG_
    Serial.println();
    Serial.print ("Step 4: Define routes for answers from web page: ");
  # endif 
  SUB_DEFINE_ROUTES ();                           /* Call subroutine to define routes for HTML equests */
  server.begin();                                 /* Start server */
  #ifdef _DEBUG_
    Serial.println("Done");                       /* Display success message */
  # endif 
  /* Initialize variables */
  DISPLAY_TEST = false;                           /* Disable display test */
  REBOOT_FLAG = false;                            /* Set reboot flag false */
  REBOOT_SAVE_FLAG = false;                       /* Set reboot save flag false */ 
  BOOTLOADER_FLAG = false;                        /* Set bootloader flag false */
  RESET_FLAG = false;                             /* Set reset flag false */
  Serial.println();
  Serial.println ("Initialisation finished! Ready to receive requests ...");
  Serial.println ();

  /* Define scroll messge for connection information*/ 
  if (STA_BOOT_MODE == true)                      /* Station mode selected ? */
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
  else                                            /* Access point mode selected ? */
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
  /* Convert to char string */
  WEB_PAGE_STRING.toCharArray(DISPLAY_MESSAGE,MAX_CHARACTERS); 
  SPEED_LEVEL = 2;                                /* Set message speed to 2 = Medium scrolling speed */
  SCROLLING_TEXT = true;                          /* Enable scrolling */
  MESSAGE_TYPE = 2;                               /* Message type: 2 = Testnachricht */ 
  WEB_MESSAGE_AVAILABLE = true;                   /* Set new message available true */

  /* Check on Autostart of message */
  if (AUTOSTART_FLAG == true)                     /* Autostart selected ? --> Display init message only once */
  {
    /* Display init message only once */
    END_SCROLL_MESSAGE = false;                   /* Set end scroll message false */
    do                                            /* Repeat until scrolling is done */
    {
      SUB_LED_SCROLL();                           /* Update Led display scrolling regulary */
      delay (10);                                 /* 10 ms delay */
    } 
    while (END_SCROLL_MESSAGE == false); 
    /* Restore values from Init.txt file */
    SCROLLING_TEXT   = ORIG_SCROLLING_TEXT;       /* Restore scolling text flag */
    OWN_FONT_LIBRARY = ORIG_OWN_FONT_LIBRARY;     /* Restore own font library flag */
    INV_EFFECT       = ORIG_INV_EFFECT;           /* Restore inverting effect flag */
    CHAR_SPACING     = ORIG_CHAR_SPACING;         /* Restore char spacing value */
    MESSAGE_TYPE     = ORIG_MESSAGE_TYPE;         /* Restore message type */
    BRIGHTNESS_LEVEL = ORIG_BRIGHTNESS_LEVEL;     /* Restore brightness level */  
    SPEED_LEVEL      = ORIG_SPEED_LEVEL;          /* Restore speed level */
    WEB_PAGE_STRING  = ORIG_WEB_PAGE_STRING;      /* Restore webpage string */
    /* Convert to char string */
    SUB_REPLACE_SPECIAL_CHARACTERS(WEB_PAGE_STRING).toCharArray(DISPLAY_MESSAGE,MESSAGE_SIZE);
    WEB_MESSAGE_AVAILABLE = true;                 /* Set new message available true */
  }
}


/****************************************************************************************/
/* Main Loop                                                                            */
/****************************************************************************************/
void loop() 
{    
   /* Scrolling selected and no display test ? */
  if ((SCROLLING_TEXT == true) && (DISPLAY_TEST == false))                    
  {
    SUB_LED_SCROLL();                             /* Update Led display scrolling regulary */
  }
  else                                            /* No scrolling or display test selected ? */
  {
    /* New message available and no display test ? */
    if ((WEB_MESSAGE_AVAILABLE == true) && (DISPLAY_TEST == false)) 
    {
      SUB_LED_PRINT();                            /* Call subroutine to display text */
    }
    /* Display test ? */
    if (DISPLAY_TEST  == true)
    {
      SUB_LED_TEST();                             /* Call subroutine for test seuence of LEE display */
    }
  }

  /* Check on rebooting */
  if (REBOOT_FLAG  == true)
  {
    delay (3000);                                 /* 3 seconds delay to enable loading of website */
    SUB_WRITE_INIT_FILE (REBOOT_SAVE_FLAG);       /* Call subroutine write init file with or without message parameter saving */ 
  }

  /* Check on bootloader */
  if (BOOTLOADER_FLAG  == true)
  {
    #ifdef _DEBUG_  
      Serial.println ();
      Serial.println("Reseting the device for bootloader ... ");
    #endif
    delay (3000);                                 /* 3 seconds delay to enable loading of website */
    EEPROM.write(0, 1);                           /* Write FLASH to anable bootloader on next reset */
    EEPROM.commit();                              /* Write to FLASH memory */ 
    ESP.restart();                                /* Restarting ESP device */
    do                                            /* Endless loop until reset */
    {}
    while (1);
  } 

  /* Check on reset */
  if (RESET_FLAG == true)
  {
    #ifdef _DEBUG_  
      Serial.println ();
      Serial.println("Reseting the device without saving data ... ");
    #endif
    delay (3000);                                 /* 3 seconds delay to enable loading of website */
    ESP.restart();                                /* Restarting ESP device */
    do                                            /* Endless loop until reset */
    {}
    while (1);
  }     
}       
