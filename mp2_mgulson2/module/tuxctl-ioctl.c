/*
 * tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)


/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in
 * tuxctl-ld.c. It calls this function, so all warnings there apply
 * here as well.
 */
 static void tux_init_(struct tty_struct* tty);
 static unsigned long tux_buttons_(unsigned long arg);
 static void tux_set_LED_(struct tty_struct* tty, unsigned long arg);


//spin lock
static spinlock_t lock = SPIN_LOCK_UNLOCKED;
unsigned long flags;
static int ack;

//shared variables
//static volatile int dispNum, LEDIndicator, decimalPt;
static volatile unsigned tuxResponse0, tuxResponse1, tuxResponse2;
static char buffer[20];





/*  Mapping from 7-segment to bits
*     The 7-segment display is:
*          _A
*        F| |B
*          -Ga
*        E| |C
*          -D .dp
*
*     The map from bits to segments is:
*
*     __7___6___5___4____3___2___1___0__
*     | A | E | F | dp | G | C | B | D |
*     +---+---+---+----+---+---+---+---+
*
*     Arguments: >= 1 bytes
*        byte 0 - Bitmask of which LED's to set:
*
*        __7___6___5___4____3______2______1______0___
*         | X | X | X | X | LED3 | LED2 | LED1 | LED0 |
*         ----+---+---+---+------+------+------+------+
*/
char ASCIITOHEX[16] = {0xe7, 0x06, 0xc9, 0x2e, 0xad,0xed, 0x86, 0xef, 0xae, 0xee,
0x6d, 0xe0, 0xe1, 0x4f, 0xa9, 0xa8};





void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet) {
    unsigned a, b, c;
    char checkBioc, checkReset, bitmaskzeroone;

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];
	tuxResponse0 = packet[0];
	tuxResponse1 = packet[1];
	tuxResponse2 = packet[2];

  //handle bioc and reset events
  //bitmaskzeroone = 0x01000000;
  checkBioc = MTCP_BIOC_EVENT;
  checkReset = MTCP_RESET;
  //checkBioc = checkBioc | bitmaskzeroone;
  //buffer = (char) MTCP_LED_USR;
  //tuxctl_ldisc_put(tty, &buffer, 1);
  if(tuxResponse0 == checkBioc){
    tuxctl_ldisc_put(tty, &checkBioc, 1);
    //ioctl(fd, MTCP_BIOC_EVENT);  //linux ioctl
  }
  else if(tuxResponse0 == checkReset){
    tuxctl_ldisc_put(tty, &checkReset, 1);
    //ioctl(fd, MTCP_RESET);  //linux ioctl
  }
  else if(tuxResponse0 == MTCP_ACK){
	spin_lock_irqsave(lock, flags);    //spinlock
	ack= 1;
	spin_unlock_irqrestore(lock, flags);
  }
  ack=0;

  }
    /*printk("packet : %x %x %x\n", a, b, c); */
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
//call tuxct_ioctl in input.c
// provide args correctly
//


/*
 * tux_init_
 *   DESCRIPTION: initialize tux controller to the standard settings
 *   INPUTS: tty struct
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes tux
 */


void tux_init_(struct tty_struct* tty){
	//char R; //opcode
	//convert to R4R30R2R1R0
	//char first3R;
	char buffer;

	//MTCP_LED_USR
	buffer = (char) MTCP_LED_USR;
	tuxctl_ldisc_put(tty, &buffer, 1);
	//MTCP_BIOC_ON
	buffer = (char) MTCP_BIOC_ON;
	tuxctl_ldisc_put(tty, &buffer, 1);
	//SETLEDS TO PRERESET CONDITIONS (OPTIONALLY TAKE L)

}


/*
 * tux_buttons_
 *   DESCRIPTION: handles the TUX_BUTTONS IOCTL call
 *   INPUTS: arg that will be modified in function
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies arg
 */
unsigned long tux_buttons_(unsigned long arg){


  int buttonSet;
  buttonSet  = 0;
  int  reverseBitmask;
  reverseBitmask=0xFFF0;
  char candrMask;
  candrMask = 0x08;
  char banddMask;
  banddMask = 0x04;
  char aandlMask;
  aandlMask = 0x02;
  char sanduMask;
  sanduMask = 0x01;
  char b, temp;
  char FF;
  FF = 0xFF;
  char firstHalf;
  firstHalf = 0x0F;
  //char firstHalf;
  char secondHalf;
  secondHalf =0xF0;
  char one;
  one = 0x01;
  char XORBitmask;
  //char temp;
	//add synchronization
	if(arg==0){
		return -EINVAL;
	}
	//buttonSet will be lower byte of arg
	//tuxctl_ldisc_get()

	//MTCP_POLL (suman claims this isn't necessary)
	//active low


	//set first half
	temp = (tuxResponse0 & firstHalf);   //isolate first half cbas
	temp = secondHalf | temp;   //set 1111cbas
	buttonSet = buttonSet ^ temp;

	//set second half

	//S:buttonSet[j=0] TuxResponse1[i=0] r=1
/*	b = (TuxResponseR >> i) &  one;
	b = b <<j;
	XORBitmask = b ^ FF;	//[1111111b] b=relevant bit
	buttonSet = XORBitmask ^ buttonSet; */

	//U:buttonSet[j=4] TuxResponse2[i=0] r=2
	b = (tuxResponse2 >> 0) &  one;  //placement in TuxResponse
	b = b <<4;					//placement in arg
	XORBitmask = b ^ FF;	//[1111111b] b=relevant bit
	buttonSet = XORBitmask ^ buttonSet;


	//D:buttonSet[j=5] TuxResponse2[i=1] r=2
	b = (tuxResponse2 >> 1) &  one;  //placement in TuxResponse
	b = b <<5;					//placement in arg
	XORBitmask = b ^ FF;	//[1111111b] b=relevant bit
	buttonSet = XORBitmask ^ buttonSet;


	//L:buttonSet[j=6] TuxResponse2[i=2] r=2
	b = (tuxResponse2 >> 2) &  one;  //placement in TuxResponse
	b = b <<6;					//placement in arg
	XORBitmask = b ^ FF;	//[1111111b] b=relevant bit
	buttonSet = XORBitmask ^ buttonSet;


	//L:buttonSet[j=7] TuxResponse2[i=3] r=2
	b = (tuxResponse2 >> 3) &  one;  //placement in TuxResponse
	b = b <<7;					//placement in arg
	XORBitmask = b ^ FF;	//[1111111b] b=relevant bit
	buttonSet = XORBitmask ^ buttonSet;


	//set arg[7:0]
	reverseBitmask = reverseBitmask | buttonSet;
	arg = reverseBitmask ^ arg;

  return arg;
}

/*
 * cntBitsSet
 *   DESCRIPTION: counts the number of bits set
 *   INPUTS: input
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: counts number of bits set to one
 */
char cntBitsSet(char input)
{
  char numSet;
  numSet = 0;
  while (input)
  {
    numSet += input & 1;
    numSet >>= 1;
  }
  return numSet;
}

/*
 * convertDecToHex
 *   DESCRIPTION: convert decimal to hex
 *   INPUTS: n, hex array
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */


void convertDecToHex(int n, char * hex) {
    int i;
    i = 0;
    while(n!=0) {
        int temp;
        temp  = 0;
        temp = n % 16;
        if(temp < 10) {
            hex[i] = temp + 48;
            i++;
        }
        else{
            hex[i] = temp + 55;
            i++;
        }
        n = n/16;
      }
}

/*
 * tuxSetLED
 *   DESCRIPTION: convert 
 *   INPUTS: input
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: counts number of bits set to one
 */


void tux_set_LED_(struct tty_struct* tty, unsigned long arg){
   spin_lock_irqsave(lock, flags);    //spinlock
   if(ack==1){
	return;
   }
  spin_unlock_irqrestore(lock, flags);
  int dispNum, LEDIndicator, decimalPt, dispNumHex;
  int dispNumBitmask, LEDIndicatorBitmask, decimalPtBitmask;
  char buffer;
  char hex[4];
  char displayInfoPacket0, displayInfoPacket1, displayInfoPacket2, displayInfoPacket3, displayInfoPacket4;


  //spin_lock_irqsave(lock, flags);    //spinlock

  dispNumBitmask = 0xFFFF;  //low 16 bits
  LEDIndicatorBitmask = 0xF0000;  //low bits on third byte [21:17]
  decimalPtBitmask = 0xF000000;  //low 4 bits on highest byte [27:14]


  dispNum = arg && dispNumBitmask;
  LEDIndicator = arg && LEDIndicatorBitmask;
  decimalPt = arg && decimalPtBitmask;

  //spin_unlock_irqrestore(lock, flags);

  //displayInfoPacket0
  /*byte 0 - Bitmask of which LED's to set:*/

  /*        __7___6___5___4____3______2______1______0___
  *         | X | X | X | X | LED3 | LED2 | LED1 | LED0 |
  *         ----+---+---+---+------+------+------+------+*/

  //number of bits set in byte 0. The bytes should be sent in order of
  /*    increasing LED number. (e.g LED0, LED2, LED3 for a bitmask of 0x0D)
  */
  LEDIndicator =LEDIndicator>>16;
  //Send x number of packets based on which LEDS are on
  //5 statements for each option 0-4
  displayInfoPacket0 = LEDIndicator;

  convertDecToHex(dispNum, hex);
  buffer = (char) MTCP_LED_SET;
  tuxctl_ldisc_put(tty, &buffer, 1);

  //output
  switch(cntBitsSet(LEDIndicator)){
    case 0:
      break;
    case 1:
      //send LEDIndicator as displayInfoPacket0
      //displayInfoPacket1=LEDx
      tuxctl_ldisc_put(tty, &displayInfoPacket0, 1);
      displayInfoPacket1 =   ASCIITOHEX[hex[0]];
      tuxctl_ldisc_put(tty, &displayInfoPacket1, 1);
      break;

    case 2:
      //send LEDIndicator as displayInfoPacket0
      //displayInfoPacket1=LEDx
      //"                 "
      tuxctl_ldisc_put(tty, &displayInfoPacket0, 1);
      displayInfoPacket1 =   ASCIITOHEX[hex[0]];
      tuxctl_ldisc_put(tty, &displayInfoPacket1, 1);
      displayInfoPacket2 =   ASCIITOHEX[hex[1]];
      tuxctl_ldisc_put(tty, &displayInfoPacket2, 1);
      break;

    case 3:
      //send LEDIndicator as displayInfoPacket0
      //displayInfoPacket1=LEDx
      //"                 "
      tuxctl_ldisc_put(tty, &displayInfoPacket0, 1);
      displayInfoPacket1 =   ASCIITOHEX[hex[0]];
      tuxctl_ldisc_put(tty, &displayInfoPacket1, 1);
      displayInfoPacket2 =   ASCIITOHEX[hex[1]];
      tuxctl_ldisc_put(tty, &displayInfoPacket2, 1);
      displayInfoPacket3 =   ASCIITOHEX[hex[2]];
      tuxctl_ldisc_put(tty, &displayInfoPacket3, 1);
      break;

    case 4:
      //send LEDIndicator as displayInfoPacket0
      //displayInfoPacket1=LEDx
      //"                 "
      tuxctl_ldisc_put(tty, &displayInfoPacket0, 1);
      displayInfoPacket1 =   ASCIITOHEX[hex[0]];
      tuxctl_ldisc_put(tty, &displayInfoPacket1, 1);
      displayInfoPacket2 =   ASCIITOHEX[hex[1]];
      tuxctl_ldisc_put(tty, &displayInfoPacket2, 1);
      displayInfoPacket3 =   ASCIITOHEX[hex[2]];
      tuxctl_ldisc_put(tty, &displayInfoPacket3, 1);
      displayInfoPacket4  =   ASCIITOHEX[hex[3]];
      tuxctl_ldisc_put(tty, &displayInfoPacket4, 1);
      break;
  }
  return;
}

int tuxctl_ioctl(struct tty_struct* tty, struct file* file,
                 unsigned cmd, unsigned long arg) {
    switch (cmd) {
        case TUX_INIT:
          tux_init_(tty);
          return 0;
        case TUX_BUTTONS:
          tux_buttons_(arg);
          return 0;
        case TUX_SET_LED:
          tux_set_LED_(tty, arg);
          return 0;
        default:
            return -EINVAL;
    }
    return 0;
}
