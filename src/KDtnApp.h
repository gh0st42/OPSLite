//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
//
// The model implementation for the Herald application.
//
// @author : Lars Baumgärtner (baumgaertner@cs.tu-darmstadt.de)
// @date   : 21-feb-2022
//
//

#ifndef KDTNAPP_H_
#define KDTNAPP_H_

#include <omnetpp.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string>
#include <map>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace omnetpp;
using namespace std;

#define KDTNAPP_SIMMODULEINFO " KDTNAPP>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE 1
#define FALSE 0
#define KDTNAPP_MSGTYPE_NONE 0
#define KDTNAPP_MSGTYPE_IMMEDIATE 1
#define KDTNAPP_MSGTYPE_PREFERENCE 2
#define KDTNAPP_START_ITEM_ID 22000
#define KDTNAPP_REGISTRATION_EVENT 92
#define KDTNAPP_DATASEND_EVENT 93

class KDtnApp : public cSimpleModule
{
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual int numInitStages() const;
    virtual void finish();

private:
    double totalSimulationTime;
    char prefixName[128] = "/dtn";

    double ttl;
    int nodeIndex;
    int totalNumNodes;
    double dataGenerationInterval;
    int nextGenerationNotification;
    vector<int> destNodes;

    int notificationCount;
    map<int, int> timesMessagesReceived;

    int usedRNG;

    cMessage *appRegistrationEvent;
    cMessage *dataTimeoutEvent;

    int dataSizeInBytes;

    long rx_count = 0;
    long tx_count = 0;

    // statistics related variable
    simsignal_t localDataBytesReceivedSignal;
    simsignal_t duplicateDataBytesReceivedSignal;
    simsignal_t totalDataBytesReceivedSignal;

    simsignal_t likedDataBytesReceivableByAllNodesSignal;
    simsignal_t nonLikedDataBytesReceivableByAllNodesSignal;
    simsignal_t totalDataBytesReceivableByAllNodesSignal;

    simsignal_t dataDelaySignal;
    simsignal_t dataDelayCountSignal;

    simsignal_t totalBundlesReceivedSignal;
    simsignal_t totalBundlesSentSignal;
};

#endif /* KDTNAPP_H_ */
