// define.h
// Code based on REALTEK RTL8019AS DRIVER FOR AVR ATMEGA16
// by Fred Eady (EDTP Electronics)
// Modified by Allen Porter <allen@thebends.org>

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <inttypes.h>

#define  enetpacketstatus     0x00
#define  nextblock_ptr        0x01
#define  enetpacketLenL       0x02
#define	 enetpacketLenH       0x03
//******************************************************************
//* REALTEK CONTROL REGISTER OFFSETS
//* All offsets in Page 0 unless otherwise specified
//******************************************************************
#define CR              0x00
#define PSTART          0x01
#define PAR0            0x01    // Page 1
#define CR9346          0x01    // Page 3
#define PSTOP           0x02
#define BNRY            0x03
#define TSR             0x04
#define TPSR            0x04
#define TBCR0           0x05
#define NCR             0x05
#define TBCR1           0x06
#define ISR             0x07
#define CURR            0x07   // Page 1
#define RSAR0           0x08
#define CRDA0           0x08
#define RSAR1           0x09
#define CRDAL           0x09
#define RBCR0           0x0A
#define RBCR1           0x0B
#define RSR             0x0C
#define RCR             0x0C
#define TCR             0x0D
#define CNTR0           0x0D
#define DCR             0x0E
#define CNTR1           0x0E
#define IMR             0x0F
#define CNTR2           0x0F
#define RDMAPORT        0X10
#define RSTPORT         0x18
//******************************************************************
//* RTL8019AS INITIAL REGISTER VALUES
//******************************************************************
#define rcrval		0x04
#define tcrval		0x00
#define dcrval		0x58    // was 0x48
#define imrval		0x11    // PRX and OVW interrupt enabled
#define txstart   	0x40
#define rxstart   	0x46
#define rxstop    	0x60
//******************************************************************
//*	RTL8019AS DATA/ADDRESS PIN DEFINITIONS
//******************************************************************
#define  rtladdr    PORTB
#define  rtldata    PORTA
#define  tortl      DDRA = 0xFF 	
#define  fromrtl    DDRA = 0x00 
//******************************************************************
//*	RTL8019AS 9346 EEPROM PIN DEFINITIONS
//******************************************************************
#define  EESK        0x08 //PORTD3 00001000
#define  EEDI        0x10 //PORTD4 00010000
#define  EEDO        0x20 //PORTD5 00100000
//******************************************************************
//*	RTL8019AS ISR REGISTER DEFINITIONS
//******************************************************************
#define  RST         0x80 //1000000
#define  RDC         0x40 //0100000
#define  OVW         0x10 //0001000
#define  PRX         0x01 //0000001
//******************************************************************
//*	RTL8019AS PIN DEFINITIONS
//******************************************************************
#define  ior_pin     0x40 //PORTD6 01000000
#define  iow_pin     0x80 //PORTD7 10000000
#define  rst_pin     0x10 //PORTD4 00010000
#define  INT0_pin    0x04 //PORTD2 00000100
#define  LE_pin      0x08 //PORTD3 00001000
//******************************************************************
//*	  PORT DEFINITIONS
//******************************************************************
#define databus	   PORTA
#define addrbus    PORTB
#define eeprom	   PORTD
#define iorwport   PORTD
#define resetport  PORTD
#define nop        asm("nop");
//******************************************************************
//*	RTL8019AS PIN MACROS
//******************************************************************
#define set_ior_pin iorwport |= ior_pin
#define clr_ior_pin iorwport &= ~ior_pin
#define set_iow_pin iorwport |= iow_pin
#define clr_iow_pin iorwport &= ~iow_pin
#define set_rst_pin resetport |= rst_pin
#define clr_rst_pin resetport &= ~rst_pin
#define set_le_pin  iorwport |= LE_pin
#define clr_le_pin  iorwport &= ~LE_pin
#define clr_EEDO eeprom &= ~EEDO
#define set_EEDO eeprom |= EEDO

#endif  // __DEFINE_H__
