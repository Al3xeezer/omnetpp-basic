
#include <string.h>
#include <omnetpp.h>
#include "myPacket_m.h"  // Use "myPacket" packet structure

/*Type definitions for myPacket*/
#define TYPE_PCK 0
#define TYPE_ACK 1
#define TYPE_NACK 2

using namespace omnetpp;


class source : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual myPacket *createPck();

  private:
    simtime_t meanTime;
    myPacket *firstPck;
    unsigned int seq;
};

// The module class needs to be registered with OMNeT++
Define_Module(source);

void source::initialize()
{
    /*Send packet to itself after meanTime (lambda=1/meanTime)*/
    meanTime=1/2;

     /*Initialize sequence number*/
    seq=1;

//    cMessage *msg = new cMessage("tictocMsg");
//    scheduleAt(meanTime,msg);

    firstPck = new myPacket();
    scheduleAt(meanTime, firstPck);

}

void source::handleMessage(cMessage *msg)
{
/*     The handleMessage() method is called whenever a message arrives
     at the module. Here, when an event is received, it sends the packet
     and creates another event in X seconds. Like a loop.

     exponential(): Generates random numbers from the exponential distribution.*/

//    send(msg, "out");
//    cMessage *msg2 = new cMessage("basicMsg");
//    scheduleAt(simTime()+exponential(meanTime),msg2);

    myPacket *pck = createPck();
    send(pck, "out");

    /*I use firstPck as the handleMessage trigger to make periodic events*/
    scheduleAt(simTime()+exponential(meanTime),firstPck);

}

myPacket *source::createPck() /*Returns the pointer of the created pck*/
{
    myPacket *pck = new myPacket();
    pck->setSeq(seq++);
    pck->setType(TYPE_PCK);
    pck->setBitLength(1024);

    return pck;
}

