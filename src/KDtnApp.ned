//
// The model for the Event Notifier (DTN) application.
//
// @author : Lars Baumgärtner (baumgaertner@cs.tu-darmstadt.de)
// @date   : 21-feb-2022
//
//
package opslite;

simple KDtnApp
{
     parameters:

        // assigned ID for the node
        int nodeIndex;
        
        // RNG to use
        int usedRNG = default(0);

        // how often data is generated
        double dataGenerationInterval @unit(s) = default(15min);

		// to which nodes should data be sent, provide as comma separated list
		// empty list means all hosts
		// "1 2 5"
		string destNodes;

        // data size
        int dataSizeInBytes = default(10000);

        // TTL of messages
        double ttl @unit(s) = default(999999999999s);

        // icon used
        @display("i=block/app");

        // statistics and signals
        @signal[localDataBytesReceived](type=long);
        @signal[duplicateDataBytesReceived](type=long);
        @signal[totalDataBytesReceived](type=long);

        @signal[likedDataBytesReceivableByAllNodes](type=long);
        @signal[nonLikedDataBytesReceivableByAllNodes](type=long);
        @signal[totalDataBytesReceivableByAllNodes](type=long);

        @signal[dataDelay](type=simtime_t);
        @signal[dataDelayCount](type=long);

        @signal[totalBundlesReceived](type=long);
        @signal[totalBundlesSent](type=long);


    gates:

        // interfaces to the lower layer
        input lowerLayerIn;
        output lowerLayerOut;

}
