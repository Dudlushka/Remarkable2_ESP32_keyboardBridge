#include "KeyBoardParser.h"
#include "Languages.h"

KbdRptParser kbparser;
Kb_map_t* pmap = LAYOUT_3;

//-------------------------------------------------------
//Keyboard press
//-------------------------------------------------------
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  MODIFIERKEYS modbits;
  *((uint8_t*)&modbits) = mod;


  Serial.print("OnKeyDown");

       if(SpecChar_process(key)){Serial.println("SpecCharMode ");}
  else if((key == HIDKEY_KP0) && (modbits.bmLeftAlt)){Serial.println("SpecCharEnter! ");SpecChar_enter();}
  else if(key == HIDKEY_F1){SendKBBridgePress_low(ControlModifier, Key_1, 0x00);SendKBBridgePress_low(0, 0, 0x00);}  //F1: ctrl+1 
  else if(key == HIDKEY_F2){SendKBBridgePress_low(ControlModifier, Key_2, 0x00);SendKBBridgePress_low(0, 0, 0x00);}  //F2: ctrl+2 
  else if(key == HIDKEY_F3){SendKBBridgePress_low(ControlModifier, Key_3, 0x00);SendKBBridgePress_low(0, 0, 0x00);}  //F3: ctrl+3 
  else if(key == HIDKEY_F4){SendKBBridgePress_low(ControlModifier, Key_4, 0x00);SendKBBridgePress_low(0, 0, 0x00);}  //F4: ctrl+4 

  else if(key == HIDKEY_F5){pmap = LAYOUT_0;}  //Language layout 1
  else if(key == HIDKEY_F6){pmap = LAYOUT_1;}  //Language layout 2
  else if(key == HIDKEY_F7){pmap = LAYOUT_2;}  //Language layout 3
  else if(key == HIDKEY_F8){pmap = LAYOUT_3;}  //Language layout 4


  else if(key == HIDKEY_F9){PrintAllChars();}
  else if(key == HIDKEY_F10){ConnectCmd    = true;localWifiConnect = true;}
  else if(key == HIDKEY_F11){ConnectCmd    = true;localWifiConnect = false;}
  else if(key == HIDKEY_F12){DisconnectCMD = true;}
  else
  {
    switch(key)
    {
      case HIDKEY_BACKSPACE : SendKBBridgePress_low(QTmodifier, Key_Backspace, 0x00);  break; //0X0008
      case HIDKEY_LEFT      : SendKBBridgePress_low(QTmodifier, Key_Left, 0x0);        break;
      case HIDKEY_RIGHT     : SendKBBridgePress_low(QTmodifier, Key_Right, 0x0);       break;
      case HIDKEY_UP        : SendKBBridgePress_low(QTmodifier, Key_Up, 0x0);          break;
      case HIDKEY_DOWN      : SendKBBridgePress_low(QTmodifier, Key_Down, 0x0);        break;
      case HIDKEY_INSERT    : SendKBBridgePress_low(QTmodifier, Key_Insert, 0x0);      break;
      case HIDKEY_DELETE    : SendKBBridgePress_low(QTmodifier, Key_Delete, 0x0);      break;
      case HIDKEY_HOME      : SendKBBridgePress_low(QTmodifier, Key_Home, 0x0);        break;
      case HIDKEY_END       : SendKBBridgePress_low(QTmodifier, Key_End, 0x0);         break;
      case HIDKEY_PAGEUP    : SendKBBridgePress_low(QTmodifier, Key_PageUp, 0x0);      break;
      case HIDKEY_PAGEDOWN  : SendKBBridgePress_low(QTmodifier, Key_PageDown, 0x0);    break;

      default: 
      if(!SearchForSpecialLayout(mod,key))
                SendKBBridgePress_low(QTmodifier, 0, OemToAscii(mod,key)); 
                break;
    }
  } 
}


//-------------------------------------------------------
//Keyboard release event
//-------------------------------------------------------
void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  Serial.print("OnKeyUP");
}


//-------------------------------------------------------
//Modifier event
//-------------------------------------------------------

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  QTmodifier = 0;

  if(afterMod.bmLeftCtrl)   QTmodifier |= ControlModifier;
  if(afterMod.bmLeftShift)  QTmodifier |= ShiftModifier ;
  if(afterMod.bmLeftAlt)    QTmodifier |= AltModifier ;
  if(afterMod.bmRightCtrl)  QTmodifier |= ControlModifier;
  if(afterMod.bmRightShift) QTmodifier |= ShiftModifier ;
  if(afterMod.bmRightAlt)   QTmodifier |= AltModifier ;

  //---------------------------------------------------------
  if (beforeMod.bmLeftCtrl   != afterMod.bmLeftCtrl)   {Serial.println("LeftCtrl changed"); }
  if (beforeMod.bmLeftShift  != afterMod.bmLeftShift)  {Serial.println("LeftShift changed");}
  if (beforeMod.bmLeftAlt    != afterMod.bmLeftAlt)    
  {
    Serial.println("LeftAlt changed");
    if((!afterMod.bmLeftAlt) && (SpecCharMode)){Serial.println("SpecCharRelease!");  SpecChar_release();}  
  }
  if (beforeMod.bmLeftGUI    != afterMod.bmLeftGUI)    {Serial.println("LeftGUI changed");}
  if (beforeMod.bmRightCtrl  != afterMod.bmRightCtrl)  {Serial.println("RightCtrl changed");}
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {Serial.println("RightShift changed");}
  if (beforeMod.bmRightAlt   != afterMod.bmRightAlt)   {Serial.println("RightAlt changed");}
  if (beforeMod.bmRightGUI   != afterMod.bmRightGUI)   {Serial.println("RightGUI changed"); }
}

//-----------------------------------------------------




int KbdRptParser::SearchForSpecialLayout(uint8_t mod, uint8_t key) 
{
  //Kb_map_t* pmap = LAYOUT_3;
  Kb_map_t* p_map = pmap; 

  uint16_t data;

  while(p_map->Scan != SCAN_TERM)
  {
    if(p_map->Scan == key)
    {
      if     (QTmodifier & AltModifier)   data = p_map->altUTF16;
      else if(QTmodifier & ShiftModifier) data = p_map->shftUTF16;
      else data = p_map->nUTF16;

      SendKBBridgePress_low(QTmodifier,p_map->QTkey, data);
      return 1;
    }
    p_map++;

  }
  return 0;
}

//-------------------------------------------------------------------
//"Number entry mode"
//-------------------------------------------------------------------

void KbdRptParser::SpecChar_enter()
{
    SpecCharMode = true;
    //SpecCharBuff
    memset(SpecCharBuff,0,CHARBUF_LEN);
    SpecCharBuff_i = 0;
}

int  KbdRptParser::SpecChar_process(uint8_t key)
{
    uint8_t data;

    if(!SpecCharMode) return 0;

    switch(key)
    {
      case 0x59 : data = '1'; break;   //1
      case 0x5a : data = '2'; break;   //2
      case 0x5b : data = '3'; break;   //3
      case 0x5c : data = '4'; break;   //4
      case 0x5d : data = '5'; break;   //5
      case 0x5e : data = '6'; break;   //6
      case 0x5f : data = '7'; break;   //7
      case 0x60 : data = '8'; break;   //8
      case 0x61 : data = '9'; break;   //9
      case 0x62 : data = '0'; break;   //0

      case 0x04 : data = 'a'; break;   //a 
      case 0x05 : data = 'b'; break;   //b
      case 0x06 : data = 'c'; break;   //c
      case 0x07 : data = 'd'; break;   //d
      case 0x08 : data = 'e'; break;   //e
      case 0x09 : data = 'f'; break;   //f
      
      default:
        SpecCharMode = false;
        return -1;
    }
    
    if(SpecCharBuff_i < CHARBUF_LEN-2)
    SpecCharBuff[SpecCharBuff_i++] = data;
    return 1;
}

int  KbdRptParser::SpecChar_release()
{
  
  uint32_t utf32_char;
  utf32_char = strtoul(SpecCharBuff,NULL,16);
  if(utf32_char)
  {
    Serial.print("Special character found! ");
    Serial.print(utf32_char,HEX);
    Serial.println("");
    SendKBBridgePress_low(QTmodifier,0,(uint16_t)(utf32_char & 0x0000ffff));
  }

  SpecCharMode = false;
  return 0;
}




void SendKBBridgePress_low(uint32_t modcode, uint32_t keycode, uint16_t utf16char)
{
  Serial.println("Send TCP KB data");

  uint8_t data[18];
  uint8_t send_len = 0;

    /*
    out << (qint32)3;
    out << (qint32)event->key();
    out << (qint32)event->modifiers();
    out << QString(event->text());
    */

  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 3;  //keypress event?

  data[4] = (uint8_t)(keycode >> 24);
  data[5] = (uint8_t)(keycode >> 16);
  data[6] = (uint8_t)(keycode >>  8);
  data[7] = (uint8_t)(keycode >>  0);

  data[8] =  (uint8_t)(modcode >> 24);
  data[9] =  (uint8_t)(modcode >> 16);
  data[10] = (uint8_t)(modcode >>  8);
  data[11] = (uint8_t)(modcode >>  0);

  if(utf16char){
    data[12] = 0; //length of string
    data[13] = 0;
    data[14] = 0;
    data[15] = 2; //2:single character
    data[16] = (uint8_t)(utf16char >> 8);
    data[17] = (uint8_t)(utf16char >> 0);
    send_len = 18;
  }
  else
  {
    data[12] = 0xff; //length of string
    data[13] = 0xff;
    data[14] = 0xff;
    data[15] = 0xff; //2:single character
    send_len = 16;
  }

  client.write(data,send_len);
}


//--------------------------------------------------
// This is a TesT!!! Print all supported characters
//--------------------------------------------------

void KbdRptParser::PrintAllChars()
{
  uint16_t i;

  i = specCharOffset;
  while(1)
  {

    
    SendKBBridgePress_low(0, 0, intToAscii((i>>12)&0x0f) );
    delay(2);
    SendKBBridgePress_low(0, 0, intToAscii((i>>8)&0x0f) );
    delay(2);
    SendKBBridgePress_low(0, 0, intToAscii((i>>4)&0x0f) );
    delay(2);
    SendKBBridgePress_low(0, 0, intToAscii((i>>0)&0x0f) );
    delay(2);
    SendKBBridgePress_low(0, 0, ':');
    delay(2);
    SendKBBridgePress_low(0, 0, ' ');
    SendKBBridgePress_low(0, 0, ' ');
    delay(2);
    SendKBBridgePress_low(0, 0, i);
    delay(2);
    SendKBBridgePress_low(0, 0, ' ');
    SendKBBridgePress_low(0, 0, ' ');
    delay(100); 

    i++;

    if(i%0x100 == 0){
      specCharOffset = i+1;
      return;
    }
  }
    
}


uint8_t KbdRptParser::intToAscii(uint8_t number) 
{
    if(VALUE_WITHIN(number, 0, 9))   return ('0' + number);
    if(VALUE_WITHIN(number, 10, 15)) return ('a' + number-10);
}
