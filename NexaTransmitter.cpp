/*
 * Changed to comply with a definition of Nexa protocol in
 * http://elektronikforumet.syntaxis.se/wiki/index.php?title=RF_Protokoll_-_Nexa_självlärande
 * The timings of 1 and 0 have been inverted and the receiver to be
 * controlled is defined by channel and unit (channel is physical 4-way
 * switch in the current Nexa remote). Tested and works with current Nexa
 * hardware available at the time of writing.
 * Jussi Heikkilä 10/9/2013
 *
 * HE300 Automatic Protocol Transmitter 'Simulator'
 * David Edmundson 21/11/2009
 *
 * This class allows use of the homeeasy automatic protocol to be used in any Arduino program
 *
 * Based heavily on demo code by
 * Martyn Henderson 02/10/2009  http://martgadget.blogspot.com
 *
 * The code has been moved into a class, for easy re-usage and has been optimised 
 * in regards to calculating and storing the transmitter address.
 *
 * You dont need to learn the Arduino into the socket, because it can 
 * pretend to be a transmitter you already have.
 *
 * Use the Automatic protocol reciever code above to find out
 * your transmitter address, and reciepient ID. 
 *
 * Original code from Martyn
 * *Only* tested with one HE300 transmitter (HE Address 272946) with its
 * slider switch set to 1 which appears to be 0 in fact.
 * Seems to work with a switch socket (HE302S) and can turn it on and off 
 * without the ardunio being learned into the socket.
 *
 * Edited code works with a HE305 transmitter, connecting to a HEXXX light fitting. The reciepient code for these 
 * appears to be 0b1010. I based my timings based on actual timings received from /my/ transmitter.
 * These seem to match the numbers used by Barnaby and Peter but not from Martyn. Adjust if needed.
 *
 * Cheers to Barnaby and Peter, because without the code above
 * that shows the receiver output this wouldnt have been possible!
 *
 * If you make something cool using Nexa and Arduino, please 
 * post back here with a link to your code so we can all enjoy.
 *
 */

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
#include "NexaTransmitter.h"

#define delayOneHigh 250
#define delayOneLow 250
#define delayZeroHigh 250
#define delayZeroLow 1250

NexaTransmitter::NexaTransmitter(short pin, unsigned long remote) :
  txPin(pin),
  transmitterId(remote),
  repeat(5) // repeat the transmission 5 times like the Nexa remote
{
  pinMode(txPin, OUTPUT);      // transmitter pin.
}


//transmit a command and dim level to the desired unit in the channel
//there are 4 units in 4 channels, like in the default nexa remote
void NexaTransmitter::setSwitch(bool on, short channel, short unit, short level)
{
  for (int i = 0; i < repeat; i++) {
    transmit(on, channel, unit, level);
    delay(10);
  }
}


//transmit a single command
void NexaTransmitter::transmit(bool blnOn, short channel, short unit, short level)
{
  int i;
  bool bitToSend;
  // Do the latch sequence..
  digitalWrite(txPin, HIGH);
  delayMicroseconds(delayOneHigh);     // bit of radio shouting before we start. 
  digitalWrite(txPin, LOW);
  delayMicroseconds(2500);     // low for 2500 for latch 1

  // Send transmitter device ID 
  // This is a 26 bit code.
  // Start at MSB and iterate through to the lowest
  for(i=25; i>=0; i--)
  {
    //The typecasting seems a bit overkill, but 26 bits needs a long 
    //and the arduino compiler seems to love trying to 
    //convert everything to an standard int.
    //creates bitmask of only relevant bit. Check and send a 1 or 0 as applicable.
    bitToSend = (unsigned long)(transmitterId & ((unsigned long)1 << i)) != 0;
    sendPair(bitToSend);

  }

  // Send 26th bit - group on(0) off(1)
  sendPair(true);

  if (level > 0) {
    // dimmer level was given, send 11
    sendBit(true);
    sendBit(true);
  } else {
    // Send 27th bit - on(0)/off(1)
    sendPair(!blnOn);
  }

  // 2 bits - channel
  for(i=1; i>=0; i--)
  {
    bitToSend = ((4-channel) & (1 << i)) != 0;
    sendPair(bitToSend);
  }

  // 2 bits - unit
  for(i=1; i>=0; i--)
  {
    bitToSend = ((4-unit) & (1 << i)) != 0;
    sendPair(bitToSend);
  }

  if (level > 0) {
    // send the dimmer level
    for(i=3; i>=0; i--)
    {
      bitToSend = (level & (1 << i)) != 0;
      sendPair(bitToSend);
    }
  }

  digitalWrite(txPin, HIGH);   // high again (stop)
  delayMicroseconds(delayZeroHigh);      // wait a moment
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.
  //sendBit(false); // end with 0
}

void NexaTransmitter::sendBit(bool b)
{
  if (b)
  {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(delayOneHigh);
    digitalWrite(txPin, LOW);
    delayMicroseconds(delayOneLow);
  }
  else
  {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(delayZeroHigh);
    digitalWrite(txPin, LOW);
    delayMicroseconds(delayZeroLow);
  }
}

void NexaTransmitter::sendPair(bool b)
{
  // Send the Manchester Encoded data 01 or 10, never 11 or 00
  if(b)
  {
    sendBit(true);
    sendBit(false);
  }
  else
  {
    sendBit(false);
    sendBit(true);
  }
}
