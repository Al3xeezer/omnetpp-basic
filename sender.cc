// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class sender : public cSimpleModule
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(sender);

void sender::initialize()
{
    // Initialize is called at the beginning of the simulation.
    // To bootstrap the tic-toc-tic-toc process, one of the modules needs
    // to send the first message. Let this be `tic'.

}

void sender::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through

    EV << msg->getArrivalGate()->getFullName();
    send(msg, "out"); // send out the message
}

