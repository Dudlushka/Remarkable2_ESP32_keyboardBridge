# Remarkable2_ESP32_keyboardBridge
Remarkable ESP32 keyboardBridge

> Special thanks to **Cookiemonster**, who discovered the KeyboardBridge

## Enable the KeyboardBridge interface on your Remarkable 2

you have to SSH into your device, and add this line into 
xochitl.conf
(there is a lot of tutorial online if you don't know what I'm talking about, this is super easy, so don't be afraid! I used filezilla)
``
[Experimental]
KeyboardBridge=true
``
