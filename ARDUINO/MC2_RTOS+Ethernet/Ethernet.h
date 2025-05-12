#ifndef ETHERNET_H
#define ETHERNET_H

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

void ETHERNET_init(void);
void ETHERNET_send(const char text);
//char ETHERNET_receive(void);

#endif
