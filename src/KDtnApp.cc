//
// The model implementation for the dtn application.
//
// @author : Lars Baumgärtner (baumgaertner@cs.tu-darmstadt.de)
// @date   : 21-feb-2022
//
//

#include <sstream>
#include "KDtnApp.h"

Define_Module(KDtnApp);

void KDtnApp::initialize(int stage)
{
    if (stage == 0)
    {

        // get parameters
        nodeIndex = par("nodeIndex");
        totalNumNodes = getParentModule()->getParentModule()->par("numNodes");

        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        dataSizeInBytes = par("dataSizeInBytes");

        stringstream iss(par("destNodes").stdstringValue());
        int number;
        while (iss >> number)
            destNodes.push_back(number);
        if (destNodes.size() == 0)
        {
            for (int i = 0; i < totalNumNodes; i++)
                destNodes.push_back(i);
        }

        ttl = par("ttl");
        totalSimulationTime =
            SimTime::parse(
                getEnvir()->getConfig()->getConfigValue(
                    "sim-time-limit"))
                .dbl();
        //notificationCount = totalSimulationTime/dataGenerationInterval;

        rx_count = 0;

        // setup prefix
        strcpy(prefixName, "/dtn");

        nextGenerationNotification = 0;
    }
    else if (stage == 1)
    {
    }
    else if (stage == 2)
    {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KDTNAPP_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.

        dataTimeoutEvent = new cMessage("Data Timeout Event");
        dataTimeoutEvent->setKind(KDTNAPP_DATASEND_EVENT);
        // add 0.1 secs to the first sending to avoid the simulation to send one more message than expected.
        if (dataGenerationInterval > 0)
            scheduleAt(simTime() + dataGenerationInterval,
                       dataTimeoutEvent);
        nextGenerationNotification = nodeIndex;

        // setup statistics signals
        localDataBytesReceivedSignal = registerSignal("localDataBytesReceived");
        duplicateDataBytesReceivedSignal = registerSignal(
            "duplicateDataBytesReceived");
        likedDataBytesReceivableByAllNodesSignal = registerSignal(
            "likedDataBytesReceivableByAllNodes");
        nonLikedDataBytesReceivableByAllNodesSignal = registerSignal(
            "nonLikedDataBytesReceivableByAllNodes");

        totalDataBytesReceivedSignal = registerSignal("totalDataBytesReceived");
        totalDataBytesReceivableByAllNodesSignal = registerSignal(
            "totalDataBytesReceivableByAllNodes");

        dataDelaySignal = registerSignal("dataDelay");
        dataDelayCountSignal = registerSignal("dataDelayCount");

        totalBundlesReceivedSignal = registerSignal("totalBundlesReceived");
        totalBundlesSentSignal = registerSignal("totalBundlesSent");
    }
    else
    {
        EV_FATAL << KDTNAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KDtnApp::numInitStages() const
{
    return 3;
}

void KDtnApp::handleMessage(cMessage *msg)
{

    // check message
    if (msg->isSelfMessage() && msg->getKind() == KDTNAPP_REGISTRATION_EVENT)
    {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg(
            "DTN App Registration");
        regAppMsg->setAppName("DTN");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");
    }
    else if (msg->isSelfMessage() && msg->getKind() == KDTNAPP_DATASEND_EVENT)
    {

        EV_INFO << "Sending data" << endl;
        tx_count++;

        // mark this message as received by this node
        timesMessagesReceived[nextGenerationNotification]++;

        //setup the data message for sending down to forwarding layer
        char tempString[128];
        sprintf(tempString, "D item-%0d",
                KDTNAPP_START_ITEM_ID + nextGenerationNotification);

        KDataMsg *dataMsg = new KDataMsg(tempString);

        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");

        sprintf(tempString, "/dtn/item-%0d",
                KDTNAPP_START_ITEM_ID + nextGenerationNotification);

        dataMsg->setDataName(tempString);
        dataMsg->setRealPayloadSize(dataSizeInBytes);
        dataMsg->setMsgUniqueID(nextGenerationNotification);

        sprintf(tempString, "Details of item-%0d",
                KDTNAPP_START_ITEM_ID + nextGenerationNotification);
        dataMsg->setDummyPayloadContent(tempString);

        dataMsg->setByteLength(dataSizeInBytes);
        dataMsg->setMsgType(0);
        dataMsg->setValidUntilTime(ttl);

        dataMsg->setInjectedTime(simTime().dbl());

        send(dataMsg, "lowerLayerOut");

        emit(totalDataBytesReceivableByAllNodesSignal,
             (1 * totalNumNodes * dataSizeInBytes));

        // schedule again after a complete round robin of all nodes
        nextGenerationNotification += totalNumNodes;
        if (dataGenerationInterval > 0)
            scheduleAt(simTime() + dataGenerationInterval, msg);
    }
    else if (dynamic_cast<KDataMsg *>(msg) != NULL)
    {
        EV_INFO << "Received data" << endl;
        rx_count++;

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // increment number of times seen
        timesMessagesReceived[dataMsg->getMsgUniqueID()]++;

        // check and emit stat signals
        if (timesMessagesReceived[dataMsg->getMsgUniqueID()] > 1)
        {
            emit(duplicateDataBytesReceivedSignal, (long)dataSizeInBytes);
        }
        else
        {
            emit(localDataBytesReceivedSignal, (long)dataSizeInBytes);
            emit(totalDataBytesReceivedSignal, (long)dataSizeInBytes);
            emit(dataDelaySignal,
                 (simTime().dbl() - dataMsg->getInjectedTime()));
            emit(dataDelayCountSignal, (long)1);
        }

        delete msg;
    }
    else
    {

        EV_INFO << KDTNAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }
}

void KDtnApp::finish()
{
    emit(totalBundlesReceivedSignal, (long)rx_count);
    emit(totalBundlesSentSignal, (long)tx_count);

    if (appRegistrationEvent->isScheduled())
        cancelEvent(appRegistrationEvent);
    delete appRegistrationEvent;

    if (dataTimeoutEvent->isScheduled())
        cancelEvent(dataTimeoutEvent);
    delete dataTimeoutEvent;

    timesMessagesReceived.clear();
}
