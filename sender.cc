
#include <string.h>
#include <omnetpp.h>
#include "myPacket_m.h"  // Use "myPacket" packet structure

/*Type definitions for myPacket*/
#define TYPE_PCK 0
#define TYPE_ACK 1
#define TYPE_NACK 2

/*Cases for state_machine*/
#define STATE_IDLE 0
#define STATE_BUSY 1

using namespace omnetpp;


class sender : public cSimpleModule
{
  public:
    virtual ~sender();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendCopyOf(myPacket *pck);

  private:
    cQueue *txQueue;                    /*Define queue for transmission*/
    unsigned short state_machine;       /*Define state_machine to react to diff events*/
    cMessage *control;                  /*Used as a trigger event for rtx*/
    myPacket *newPck;
};

// The module class needs to be registered with OMNeT++
Define_Module(sender);

sender::~sender(){
    txQueue->~cQueue();
}



void sender::initialize()
{
    /*Initialize the Queue for tx*/
    txQueue = new cQueue("txQueue");
    control = new cMessage("control");
    state_machine = STATE_IDLE;
    WATCH(state_machine);
}

void sender::handleMessage(cMessage *msg)
{
    if (msg!=control) {

        /*Cast <msg> to <myPacket>*/
        myPacket *pck = check_and_cast<myPacket *>(msg);

        /*State Machine*/
        if (msg->arrivedOn("inS")) {

            EV << "New pck from source"<<endl;

            switch (state_machine) {
                case STATE_IDLE:
                    sendCopyOf(pck);
                    break;

                case STATE_BUSY:
                    EV << "Queue +1";
                    txQueue->insert(pck);
                    break;
            }
        } else {
            switch (pck->getType()) {
                case TYPE_ACK:
                    if(txQueue-> isEmpty()){
                        state_machine = STATE_IDLE;
                    }else{
                        /*Remove acknowledged packet*/
                        txQueue->pop();

                        /*Read first packet of the queue (without removing it)*/
                        newPck = (myPacket *)txQueue->front();
                        sendCopyOf(newPck);
                    }
                    break;

                case TYPE_NACK:
                    /*Read first packet of the queue (without removing it)*/
                    newPck = (myPacket *)txQueue->front();
                    sendCopyOf(newPck);
                    break;
            }
        }
        //delete(pck);
    }
}

void sender::sendCopyOf(myPacket *pck)
{
    EV << "Sending pck";
    send(pck, "out");
    state_machine=STATE_BUSY;

    //simtime_t txFinishTime = pck->getSenderGate()->getTransmissionChannel()->getTransmissionFinishTime();
    simtime_t FinishTime = gate("out")->getTransmissionChannel()->getTransmissionFinishTime();
    scheduleAt(FinishTime,control);
}
