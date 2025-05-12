#include "Ethernet.h"
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x4A, 0xE0}; //Arduino shield MAC address
IPAddress megaip  (192, 168, 1, 15); //shield IP address "Master arduino"
IPAddress Unoip   (192, 168, 1, 10); //client ip address "Laptop or display"
IPAddress Laptopip(192, 168, 1, 2 ); //client ip address "Laptop or display"

//----------------------------------------------------------------------------------
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //array to store received data
String receivedData;                        //string to store received data
int packetSize;                             //variable to store received packet size
EthernetUDP LaptopUDP;                      //UDP object
EthernetUDP UnoUDP;                         //UDP object

unsigned int LaptopPort  = 5005;
unsigned int UnoPort = 5000;
//----------------------------------------------------------------------------------


void ETHERNET_init(void)
{
  Ethernet.init(10);
  Ethernet.begin(mac, megaip);                  //initialize ethernet
  while (UnoUDP.begin(UnoPort) == 0);           //initialize UDP to port number 5000
  Serial.println("Start");
  while (LaptopUDP.begin(LaptopPort) == 0)     //initialize UDP to port number 5005
    Serial.println("not connected");
  /*while (1)//stay in a loop waiting for a command from the laptop to start the production line application
    {
    packetSize = LaptopUDP.parsePacket();                     //get size of received packet
    if (packetSize > 0)                                       //data received
    {
       LaptopUDP.read(packetBuffer,  UDP_TX_PACKET_MAX_SIZE);    //read received data via UDP
       Serial.println(packetBuffer);
       if (packetBuffer=='S') { ETHERNET_send('S');  break; }   //stay until laptop send S , then reply back to laptop and send s to arduino uno to start
       memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);         //reset packet array to 0
    }
    }*/
}
void ETHERNET_send(const char text)
{
  //UDP.parsePacket();   //get ip and port of current connection ,must receive a data first to know the port
  //try using large port on both ends because the port change from 5000 to 57642 to skip this step
     LaptopUDP.beginPacket( Laptopip, LaptopPort );//intialize packet with current ip connection and current port
    LaptopUDP.print(text);
    LaptopUDP.endPacket();
  
  
  if (text != 'M' && text != 'N')
  {
    UnoUDP.beginPacket( Unoip, UnoPort );//intialize packet with current ip connection and current port
    UnoUDP.print(text);delay(5);
    UnoUDP.print(text);delay(5);
    UnoUDP.print(text);
    UnoUDP.endPacket();
  }
  else
  {
    UnoUDP.beginPacket( Unoip, UnoPort );//intialize packet with current ip connection and current port
    UnoUDP.print(text);
    UnoUDP.endPacket();
  }

}

/*char ETHERNET_receive(void)
  {
   packetSize = UDP.parsePacket();                     //get size of received packet
  if(packetSize > 0)                                  //data received
  {
    UDP.read(packetBuffer,  UDP_TX_PACKET_MAX_SIZE);    //read received data via UDP
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);    //reset packet array to 0


    return packetBuffer;
    }
    else
    {
      return '0';
    }
  }*/
