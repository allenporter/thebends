// ether.c
// Author: Allen Porter <allen@thebends.org>
// Large portions of this code were written by Fred Eady of EDTP Electronics

#include "ether.h"
#include "define.h"
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <string.h>

static unsigned char pageheader[4];
#define MTU 1500  // ignore everything above :/
static uint8_t packet[MTU];

static uint8_t MAC_ADDR[6];

#define	make16(varhigh,varlow)	((varhigh & 0xFF)* 0x100) + (varlow & 0xFF)

void ether_set_addr(const uint8_t* mac_addr) {
  memcpy(MAC_ADDR, mac_addr, 6);
}

//******************************************************************
//*     Delay millisecond Function
//*   This function uses Timer 1 and the A compare registers
//*   to produce millisecond delays.
//*
//******************************************************************
void delay_ms(unsigned int delay) {
  OCR1AH = 0x1C;
  OCR1AL = 0xCC;
  TCCR1B = 0x00;          // Stop Timer1
  unsigned int i;
  for (i = 0; i < delay; ++i) {
    TCCR1B = 0x00;          // Stop Timer1
    TCNT1H = 0x00;          // Clear Timer1
    TCNT1L = 0x00;
    TCCR1B = 0x09;          // Start Timer1 with clk/1
    while(!(TIFR & 0x10));
    TIFR |= 0x10;
  }
}

void write_rtl(unsigned int regaddr, unsigned int regdata) {
  rtladdr = regaddr;
  rtldata = regdata;
  tortl;
  nop;
  clr_iow_pin;
  nop;
  set_iow_pin;
  nop;
  fromrtl;
  PORTA = 0xFF; 
}

unsigned char read_rtl(unsigned int regaddr) {
  unsigned char byte_read;
  fromrtl;
  PORTA = 0xFF; 
  rtladdr = regaddr;
  clr_ior_pin;
  nop;
  byte_read = PINA;
  nop;
  set_ior_pin;
  nop;
  return byte_read;
}

void overrun() {
  unsigned char byte_read = read_rtl(CR);
  write_rtl(CR,0x21);
  delay_ms(2);
  write_rtl(RBCR0,0x00);
  write_rtl(RBCR1,0x00);
  if(!(byte_read & 0x04)) {
    //resend = 0;
  } else if(byte_read & 0x04) {
    read_rtl(ISR);
    if ((byte_read & 0x02) || (byte_read & 0x08)) {
      //resend = 0;
    } else {
      //resend = 1;
    }
  }
  write_rtl(TCR,0x02);
  write_rtl(CR,0x22);
  write_rtl(BNRY,rxstart);
  write_rtl(CR,0x62);
  write_rtl(CURR,rxstart);
  write_rtl(CR,0x22);
  write_rtl(ISR,0x10);
  write_rtl(TCR,tcrval);
}

void init_RTL8019AS() {
  fromrtl;         // PORTA data lines = input
  PORTA = 0xFF;
  DDRB = 0xFF;
  rtladdr = 0x00;  // clear address lines
  DDRC = 0xFF;
  DDRD = 0xFA;     // setup IOW, IOR, EEPROM,RXD,TXD,CTS,LE
  PORTD = 0x05;    // enable pullups on input pins

  clr_le_pin;      //initialize latch enable for HCT573
  clr_EEDO;
  set_iow_pin;    // disable IOW
  set_ior_pin;    // disable IOR
  set_rst_pin;    // put NIC in reset
  delay_ms(2);    // delay at least 1.6ms
  clr_rst_pin;    // disable reset line

  unsigned char byte_read = read_rtl(RSTPORT);  // read contents of reset port
  write_rtl(RSTPORT, byte_read);                // do soft reset
  delay_ms(10);                                 // give it time
  byte_read = read_rtl(ISR);                    // check for good soft reset
  if (!(byte_read & RST)) {
    while(1) {
      delay_ms(1000);
    }
  }
  write_rtl(CR, 0x21);     // stop the NIC, abort DMA, page 0
  delay_ms(2);             // make sure nothing is coming in or going out
  write_rtl(DCR, dcrval);
  write_rtl(RBCR0, 0x00);
  write_rtl(RBCR1, 0x00);
  write_rtl(RCR, 0x04);
  write_rtl(TPSR, txstart);  // 0x40
  write_rtl(TCR, 0x02);
  write_rtl(PSTART, rxstart);
  write_rtl(BNRY, rxstart);
  write_rtl(PSTOP, rxstop);
  write_rtl(CR, 0x61);
  write_rtl(CURR, rxstart);
  int i;
  for (i=0;i<6;++i) {
    write_rtl(PAR0+i, MAC_ADDR[i]);
  }
  write_rtl(CR,0x22);
  write_rtl(ISR,0xFF);
  write_rtl(IMR,imrval);
  write_rtl(TCR,tcrval);
}

void get_frame(packet_cb cb) {
  // execute Send Packet command to retrieve the packet
  write_rtl(CR,0x1A);
  int i;
  for (i = 0; i < 4; ++i) {
    pageheader[i] = read_rtl(RDMAPORT);
  }
  uint16_t rxlen = make16(pageheader[enetpacketLenH],
                          pageheader[enetpacketLenL]);
  unsigned char byte_read = 0;
  for(i = 0; i < rxlen; ++i) {
    byte_read = read_rtl(RDMAPORT);
    // dump any bytes that will overrun the receive buffer
    if (i < MTU) {
      packet[i] = byte_read;
    }
  }
  while (!(byte_read & RDC)) {
    byte_read = read_rtl(ISR);
  }
  write_rtl(ISR,0xFF);

  // Pass off packet to registered handler
  (*cb)(packet, rxlen);
}

void ether_loop(packet_cb cb) {
  init_RTL8019AS();
  while (1) {
    write_rtl(CR, 0x22);

    // Wait for an incoming packet
    while (!(PIND & INT0_pin));

    unsigned char byte_read = read_rtl(ISR);
    if (byte_read & OVW) {
      overrun();
    }
    if (byte_read & PRX) {
      get_frame(cb);
    }
    byte_read = read_rtl(BNRY);
    unsigned char data_L = byte_read;
    write_rtl(CR, 0x62);
    byte_read = read_rtl(CURR);
    unsigned char data_H = byte_read;
    write_rtl(CR, 0x22);
    // buffer is not empty, get next packet
    if (data_L != data_H) {
      get_frame(cb);
    }
    // reset the interrupt bits
    write_rtl(ISR, 0xff);
  }
}

void ether_write(char* buf, uint16_t len) {
  // start the NIC
  write_rtl(CR, 0x22);

  // load beginning page for transmit buffer
  write_rtl(TPSR, txstart);

  // set start address for remote DMA operation
  write_rtl(RSAR0, 0x00);
  write_rtl(RSAR1, txstart);  // 0x40

  // clear the Interrupts
  write_rtl(ISR, 0xff);

  uint16_t packet_len = (len < 0x40) ? 0x40 : len;
  uint8_t packet_low = (packet_len & 0xff);
  uint8_t packet_high = ((packet_len >> 8) & 0xff);
  write_rtl(RBCR0, packet_low);
  write_rtl(RBCR1, packet_high);

  // do remote write operation
  write_rtl(CR, 0x12);

  uint16_t i;
  for (i = 0; i < len; ++i) {
    write_rtl(RDMAPORT, buf[i]);
  }

  // write some pad characters to fill out the packet to
  // the minimum length
  uint16_t pad = packet_len - len;
  for (i = 0; i < pad; ++i) {
    write_rtl(RDMAPORT, 0x00);
  }

  // make sure the DMA operation has successfully completed
  uint8_t byte_read = 0;
  while (!(byte_read & RDC)) {
    byte_read = read_rtl(ISR);
  }

  // load number of bytes to be transmitted
  write_rtl(TBCR0, packet_low);
  write_rtl(TBCR1, packet_high);

  // send the contents of the transmit buffer onto the network
  write_rtl(CR, 0x24); //_BV(2) | _BV(1)0x24);
}
