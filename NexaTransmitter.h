#ifndef NexaTransmitter_h
#define NexaTransmitter_h

class NexaTransmitter
{

  public:
    //Constructor.
    //arg1 = pin to connected to 434Mhz transmitter
    //arg2 = the 26 bit long transmitter ID
    NexaTransmitter(short, unsigned long);
    //arg1 = true to turn switch on, false for off
    //arg2 = channel 1-4
    //arg3 = unit 1-4
    //arg4 = dimmer level 1-15
    void setSwitch(bool, short, short, short level = 0);
  private:
    short txPin;
    unsigned long transmitterId;
    short repeat;

    void transmit(bool, short, short, short);
    void sendBit(bool);
    void sendPair(bool);
};

#endif
