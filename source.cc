// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class source : public cSimpleModule
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  private:
    simtime_t meanTime;
};

// The module class needs to be registered with OMNeT++
Define_Module(source);

void source::initialize()
{
    // Send packet to itself after meanTime (lambda=1/meanTime)
    meanTime=1/2;
    cMessage *msg = new cMessage("tictocMsg");

    scheduleAt(meanTime,msg);

}

void source::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, when an event is receiver, it sends the packet
    // and creates another event in X seconds. Like a loop.

    // exponential(): Generates random numbers from the exponential distribution.

    send(msg, "out");

    cMessage *msg2 = new cMessage("basicMsg");
    scheduleAt(simTime()+exponential(meanTime),msg2);

}

