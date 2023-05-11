# Remarkable2_ESP32_keyboardBridge
Remarkable ESP32 keyboardBridge

> Special thanks to **Cookiemonster**, who discovered the KeyboardBridge, and described it to me

## Enable the KeyboardBridge interface on your Remarkable 2

you have to SSH into your device, and add this line into 
xochitl.conf
(there is a lot of tutorial online if you don't know what I'm talking about, this is super easy, so don't be afraid! I used filezilla)

```
[Experimental]
KeyboardBridge=true
```

## What is Keyboardbridge

KeyboardBridge listens on port 1, and you can send TCP messages, that the device interpret as keyboard key strokes
(sometimes it freezes... if you dont disconnect, and the 2nd time your connection attempt will be refused... so make sure to disconnect)

```
data[0] = 0;
data[1] = 0;
data[2] = 0;
data[3] = 3;  //keypress event? - constant uint32_t 3

data[4] = (uint8_t)(keycode >> 24);
data[5] = (uint8_t)(keycode >> 16);
data[6] = (uint8_t)(keycode >>  8);
data[7] = (uint8_t)(keycode >>  0); // keycodes defined in QT enum Key

data[8] =  (uint8_t)(modcode >> 24);
data[9] =  (uint8_t)(modcode >> 16);
data[10] = (uint8_t)(modcode >>  8);
data[11] = (uint8_t)(modcode >>  0); // Modifiers like ctrl/shift etc... defined in QT enum KeyboardModifier 

data[12] = 0; //length of string
data[13] = 0;
data[14] = 0;
data[15] = 2; //2:single (2byte) character

data[16] = (uint8_t)(utf16char >> 8);
data[17] = (uint8_t)(utf16char >> 0); // simple character
```
...will do a better description later

## The ESP32 code

There is a simple USB HID keyboard connected to the ESP32 (download the esp32 soft usb library), 
it parses the keystrokes, and translates its scan (like) codes into rm2 commands.
The device connects to the RM2 and acts like a real keyboard... You can press all kinds of characters, that are supported, not just the ones supported by that (hmmm) Type Folio

### Features:

* There are 4 switchable Language (keyboard layout) - you have to fill the arrays with your own characters!
* Windows-like Alt+xxxx special character input (not the same!)
* Function keys:

press F11 to connect / F12 to disconnect rm2

F1: ctrl+1 (remarkable Title)  
F2: ctrl+2 (remarkable sub-Title)  
F3: ctrl+3 (remarkable text)  
F4: ctrl+4 (remarkable bullet point)  

F5: custom language layout 1 -empty / EN  (see Languages.h)  
F6: custom language layout 2 -empty / EN  (see Languages.h)  
F7: custom language layout 3 -empty / EN  (see Languages.h)  
F8: custom language layout 4 (default) Hungarian (see Languages.h)  

F9: nix  
F10: connect to Rm2 via local AP (RM2 connected to ESP AP) // not working yet!  
F11: connect to Rm2 (both ESP and RM2 on the same Wifi)  
f12: disconnect from Rm2

### Special character entry:
1. press left Alt (and keep pressing)
2. press "keypad 0" (then you can release 0) - its not part of the number
3. enter the 16 bit HEX code of the (unicode) caracter (only keypad numbers and a-f keys are accepted) 
4. release Alt
example 1: Alt+0 2 7 1 2 (representing 0x2712) will draw a pen

but here are some examples:
[supported_chars.pdf](https://github.com/Dudlushka/Remarkable2_ESP32_keyboardBridge/files/11455525/supported_chars.pdf)

### Some future things to do
Any help is welcome

* Keep alive messages needed (maybe)
* AP mode
* Special key combo to add current time as a string (via NTP)
* keyboard LED control 
* USB composite device support ('cause wireless KBs are mostly composite devices with the mouse)
* bulky Caps-lock / shift behaviour bugfix
* Proper language arrays
* Some special "language" arrays - for example drawing frames like: ╔══╦══╦══╦

### To-do:
* its a very inital stuff. You have to add the SSID/password and the IP address of your rm2 to the main file
* Fill your own Language layout array - if English / Hungarian is not for you

{ To be continued... }








