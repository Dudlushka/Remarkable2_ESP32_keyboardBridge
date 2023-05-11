//tested on: AI Thinker ESP32-CAM
//library: based on USB soft host library

//USB Keyboard connection: 
// 5V (red)
//GND (blk)
// D+ (green) IO pin  14
// D- (white) IO pin  15

//-------------------------------------------------------------------------
//Based on the "keyboardBridge" description discovered by "Cookiemonster"
//-------------------------------------------------------------------------

//press F11 to connect / F12 to disconnect to rm2

//F1: ctrl+1 (remarkable Title)
//F2: ctrl+2 (remarkable sub-Title)
//F3: ctrl+3 (remarkable text)
//F4: ctrl+4 (remarkable bullet point)

//F5: custom language layout 1 -empty / EN  (see Languages.h)
//F6: custom language layout 2 -empty / EN  (see Languages.h)
//F7: custom language layout 3 -empty / EN  (see Languages.h)
//F8: custom language layout 4 (default) Hungarian (see Languages.h)

//F9: nix
//F10: connect to Rm2 via local AP (RM2 connected to ESP AP) // not working yet!
//F11: connect to Rm2 (both ESP and RM2 on the same Wifi)
//f12: disconnect from Rm2

//------------------------------
//Special character entry mode:
//------------------------------
// (1) press left Alt (and keep pressing)
// (2) press "keypad 0" (then you can release 0) - its not part of the number
// (3) enter the 16 bit HEX code of the (unicode) caracter (only keypad numbers and a-f keys are accepted) 
// (4) release Alt
// example 1: Alt+0 2 7 1 2 (representing 0x2712) will draw a pen
// example 2: Alt+0 2 7 1 2 (representing 0x2712) will draw a pen
//-------------------------------------------------------------------------------

#include "WiFi.h"
#include "esp_wifi.h"

#include <ESP32-USB-Soft-Host.h>
#include "usbkbd.h"         // KeyboardReportParser
#include "usb_hid_keys.h"   // some key definitions
#include "KeyboardParser.h"

//-------------------------------------------------------------
// Don't forget to fill These!
//-------------------------------------------------------------
const char* ssid      = "SSID";
const char* password  = "password";

//Local AP ssid and password
const char* AP_ssid      = "RM2KB";
const char* AP_password  = "PussyJuice12345";

const uint16_t port = 1; // port TCP server
const char * rm2_ip = "192.168.1.4"; // ip of the Rm2
//const char * rm2_ip = "192.168.8.97"; // ip of the Rm2
//const char * rm2_ip = "192.168.4.3"; // ip of the Rm2

bool ConnectCmd = false;
bool DisconnectCMD = false;
bool ConnState = false;
bool localWifiConnect = true;

WiFiClient client;

#define DEBUG_ALL
#define FORCE_TEMPLATED_NOPS

//#define DP_P0  13  // always enabled - it works but interfere with the boot porcess if KB plugged
//#define DM_P0  12  // always enabled

#define PROFILE_NAME "AiThinker ESP CAM"
#define DP_P0  14  // always enabled
#define DM_P0  15  // always enabled
#define DP_P1  -1 // -1 to disable
#define DM_P1  -1 // -1 to disable
#define DP_P2  -1 // -1 to disable
#define DM_P2  -1 // -1 to disable
#define DP_P3  -1 // -1 to disable
#define DM_P3  -1 // -1 to disable

static void my_USB_DetectCB( uint8_t usbNum, void * dev )
{
  sDevDesc *device = (sDevDesc*)dev;
  printf("New device detected on USB#%d\n", usbNum);
  printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
  printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
  printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
  printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
  printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
  printf("desc.idVendor           = 0x%04x\n", device->idVendor);
  printf("desc.idProduct          = 0x%04x\n", device->idProduct);
  printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
  printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
  printf("desc.iProduct           = 0x%02x\n", device->iProduct);
  printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
  printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
}

static void my_USB_PrintCB(uint8_t usbNum, uint8_t byte_depth, uint8_t* data, uint8_t data_len)
{
  // if( myListenUSBPort != usbNum ) return;

  kbparser.Parse(data_len,data);  
  printf("in: ");
  for(int k=0;k<data_len;k++) {
    printf("0x%02x ", data[k] );
  }
  printf("\n");
}

usb_pins_config_t USB_Pins_Config =
{
  DP_P0, DM_P0,
  DP_P1, DM_P1,
  DP_P2, DM_P2,
  DP_P3, DM_P3
};

void setup()
{
  Serial.begin(115200);

  //------------------------------------------------------------------
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  

  //-----------------------------------------------------------------
  // Access Point 
  /*
  WiFi.softAP(AP_ssid, AP_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(WiFi.softAPIP());
  */
    
  //-----------------------------------------------------------------
  
  Serial.printf("USB Soft Host for %s\n", PROFILE_NAME );
  Serial.printf("TIMER_BASE_CLK: %d, TIMER_DIVIDER:%d, TIMER_SCALE: %d\n", TIMER_BASE_CLK, TIMER_DIVIDER, TIMER_SCALE );

  // USH.setTaskCore( 0 );
  // USH.setBlinkPin( (gpio_num_t) 2 );
  // USH.setTaskPriority( 16 );
  USH.init( USB_Pins_Config, my_USB_DetectCB, my_USB_PrintCB );

  //----------------------------------------------------------------

  Serial.println();
  Serial.println("Setup Done!");
}

void DisconnectRm2()
{
  ConnectCmd = false;
  client.stop();
  DisconnectCMD = false;
}


/*
//Some Access Point stuff
//it doesnt wanna work?!
int display_connected_devices()
{
    wifi_sta_list_t               wifi_sta_list;
    tcpip_adapter_sta_list_t      adapter_sta_list;

    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
    
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

    if (adapter_sta_list.num > 0)
    {
    
      for (int i = 0; i < adapter_sta_list.num; i++)
      {
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
        
        Serial.print((String)"[+] Device " + i + " | MAC : ");
        Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", station.mac[0], station.mac[1], station.mac[2], station.mac[3], station.mac[4], station.mac[5]);
        
        //Serial.println((String) " | IP " + ip4addr_ntoa((ip4_addr*)&(station.ip)));
        //rm_ip = station.ip;
      }
    }
}
*/


int ConnectToRm2(uint32_t retryTime)
{
  static uint32_t actTime = 0;
  static uint32_t prevTime = 0;

  if(!ConnState)
  {
    actTime = millis();
    if(actTime - prevTime > retryTime)
    {
      prevTime = actTime;

      /*
      //The device connected to the local AP
      if(localWifiConnect)
      {
          display_connected_devices();
          ConnectCmd = false;
          ConnState = false;
          return 0;
      }*/

      if(!client.connect(rm2_ip,port))
      {
          Serial.println("Connection failed.");
          ConnState = false;
          return 0;
      }
      else
      {
          Serial.println("Connection ok!");
          ConnectCmd = false;
          ConnState = true;
          return 1;
      }
    }  
  }
}


void loop()
{
  if(ConnectCmd){ConnectToRm2(100);}
  if(DisconnectCMD){DisconnectRm2();}
}
