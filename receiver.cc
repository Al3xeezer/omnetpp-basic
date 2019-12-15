// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.


#include <string.h>
#include <omnetpp.h>
#include "myPacket_m.h"  // Use "myPacket" packet structure

/*Type definitions for myPacket*/
#define TYPE_PCK 0
#define TYPE_ACK 1
#define TYPE_NACK 2


using namespace omnetpp;


class receiver : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void createPck(unsigned int seq, unsigned short TYPE);
};

// The module class needs to be registered with OMNeT++
Define_Module(receiver);

void receiver::initialize()
{

}

void receiver::handleMessage(cMessage *msg)
{
    myPacket *pck = check_and_cast<myPacket *>(msg);

       if(pck->hasBitError()){
           createPck(pck->getSeq(), TYPE_NACK);
       }
       else
       {
           createPck(pck->getSeq(), TYPE_ACK);
       }
}

void receiver::createPck(unsigned int seq, unsigned short TYPE) /*Returns the pointer of the created pck*/
{
    myPacket *pck = new myPacket();
    pck->setSeq(seq);
    pck->setType(TYPE);
    pck->setBitLength(1024);

    send(pck,"out");
}

