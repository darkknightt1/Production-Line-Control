#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IPAddress.h>

#include <EtherCard.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


#define HOMING    'O'
#define FEEDING   'F'
#define SORTING   'S'
#define PACKAGING 'P'
#define METAL     'M'
#define NONMETAL  'N'
#define FINISHED  'Z'
static byte mymac[] ={ 0x70, 0x69, 0x60, 0x2D, 0x30, 0x31 };
static byte myip[] = { 192, 168, 1, 10  };
static byte gwip[] = { 192, 168, 1, 1   };
static byte mask[] = { 255, 255, 255, 0 };
const int dstPort PROGMEM = 5000;
char Status= ','; 

int metalCounter = 0;
int nonmetalCounter =0;
byte Ethernet::buffer[50];
static void udpFunc(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len)
{
   IPAddress src(src_ip[0], src_ip[1], src_ip[2], src_ip[3]);
   Status = data[0];
   switch (Status)
    { 
      case HOMING:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Homing");  //current status
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");    //product stored
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        
        //Serial.println("Homing");
        break;

      case FEEDING:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Feeding");  //current status
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");    //product stored
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        Serial.println("Feeding");
        break;


        case PACKAGING:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Packaging");  //current status
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");    //product stored
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        Serial.println("Packaging");
        break;

      case METAL:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Metal");  //16 char
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(++metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        Serial.println("metal");
        break;

      case NONMETAL:
      lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Non Metal");  //16 char
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(++nonmetalCounter);
        Serial.println("nonmetal");
        break;

      case SORTING:
      lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Sorting");
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        break;

      case FINISHED:
      lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("CS: Finished");
        lcd.setCursor(0, 1);
        lcd.printstr("PS: ");
        lcd.setCursor(5, 1);  //second row 4th col.
        lcd.printstr("M:");
        lcd.print(metalCounter);
        lcd.setCursor(10, 1);
        lcd.printstr("N:");
        lcd.print(nonmetalCounter);
        Serial.println("Finished");
        break;

       default :
       break;
    }

}

void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9,HIGH);
  lcd.begin(); 
  lcd.backlight();
  Serial.begin(9600);
  
  ether.begin(sizeof Ethernet::buffer, mymac, SS);
  ether.staticSetup(myip, gwip, 0, mask);
  ether.udpServerListenOnPort(&udpFunc, dstPort);
  
  lcd.setCursor(0, 0);
  lcd.printstr("Ethernet");
  lcd.setCursor(0, 1);
  lcd.printstr("Connected");
}

void loop()
{
  ether.packetLoop(ether.packetReceive());
}
