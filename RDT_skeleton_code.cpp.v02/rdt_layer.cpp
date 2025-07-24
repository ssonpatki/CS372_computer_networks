/*
Utilized Sources:
    Kurose and Ross, Computer Networking: A Top-Down Approach, 8th Edition, Pearson
    ChatGPT to better understand the skeleton code and for explanations on error messages
    Geeks for Geeks (for better understanding RDT):
        1. Reliable Data Transfer (RDT) 1.0
        2. Reliable Data Transfer (RDT) 2.0
        3. Reliable Data Transfer (RDT) 3.0

Note: 
    Receiving warning for #pragma once, but should not hinder program execution
*/

#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "segment.cpp"
#include "unreliable.cpp"

using std::cout;
using std::endl;
using std::optional;
using std::string;
using std::vector;
using std::map;
using std::unordered_map;

/**
 * RDTLayer
 * 
 * Description:
 *   The reliable data transfer (RDT) layer is used as a communication layer to resolve issues over an unreliable
 *   channel.
 * 
 * 
 * Notes:
 * This file is meant to be changed.
 * 
 * 
 **/
class RDTLayer{
    /**
     * Class Scope Variables
     * 
     * 
     * 
     **/
    //private by default, but I prefer being explicit!
    private:
        int DATA_LENGTH;            //The length of the string data that will be sent per packet in characters.
        int FLOW_CONTROL_WIN_SIZE;  //Receive window size for flow-control.

        /*Note: sendChannel and receiveChannel are set in the rdt_main...
                We are using the optional<UnreliableChannel> type to have behavior
                similar to using none that is then set to an object in Python.
                
                We could use pointers all over, but since we do not do any none or nullptr checking
                or do any pointer manipulation anyway...
        */
        optional<UnreliableChannel*> sendChannel;
        optional<UnreliableChannel*> receiveChannel;
        string dataToSend;
        int currentIteration;       //Use this for segment "timeouts"
        int countSegmentTimeouts; //use this to track how many segments actually timeout

        /************************************************************
         * Add items here as needed!                                *
         ************************************************************/
        
        int oldSeqNum;  // Sequence num of oldest unacknowledged segment
        int nextSeqNum; // Sequence num for the next segment
        unordered_map<int, Segment> sentSeg;   // hash table (c++ container) maps int keys to segment objects
        unordered_map<int, int> sentTime;   // hash table container to map sequence num to its iteration num
        int timeout;    // wait x iterations before timeout
        map<int, string> buffer;  // stores received segments (in-order)
        int expectedSeqNum; // next expected sequence num
        string dataReceieved;   // final message
        unordered_map<int, int> sentLengths;    // hash table container to map seqnum to data length
        int lastAckSent;

    public:
        RDTLayer(){
            this->dataToSend = "";
            this->currentIteration = 0;
            this->countSegmentTimeouts = 0;

            /************************************************************
             * Add items here as needed!                                *
             ************************************************************/
        
            this->oldSeqNum = 0;
            this->nextSeqNum = 0;
            this->timeout = 5;
            this->expectedSeqNum = 0;
            this->DATA_LENGTH = 10;
            this->FLOW_CONTROL_WIN_SIZE = 75;
            this->lastAckSent = -1;
        }

        /**
         * setSendChannel()
         * 
         * Description:
         *   Called by main to set the unreliable sending lower-layer channel
         * 
         * 
         **/
        void setSendChannel(UnreliableChannel& channel){
            this->sendChannel = &channel;
        }

        /**
         * setReceiveChannel()
         * 
         * Description:
         *   Called by main to set the unreliable receiving lower-layer channel
         * 
         * 
         **/
        void setReceiveChannel(UnreliableChannel& channel){
            this->receiveChannel = &channel;
        }

        /**
         * setDataToSend()
         * 
         * Description:
         *   Called by main to set the string data to send
         * 
         * 
         **/
        void setDataToSend(string data){
            this->dataToSend = data;
        }

        /**
         * getDataReceived()
         * 
         * Description:
         *   Called by main to get the currently received and buffered string data, in order
         * 
         * 
         **/
        string getDataReceived(){
            /************************************************************
             * identify the data that has been received...              *
             *                                                          *
             * Add code to this function as needed!                     *
             ************************************************************/
            
            //cout << "getDataReceived(): Complete this..." << endl;

            string received = "";   // empty string to store data received
            
            // For each [segNum, dataSeg] pair in the buffer
                // iterate though the buffer (hash table) in key order
            // note: using const auto& to auto detect the variable type and to avoid copying segments
            for (const auto& [seqNum, dataSeg]: this->buffer) {   
                received += dataSeg;    // add dataSeg onto current received string
            }

            return received;
        }

        /* ************************************************************************************************************** */
        /* getCountSegmentTimeouts()                                                                                      */
        /*                                                                                                                */
        /* Description:                                                                                                   */
        /* Called by main to get the count of segment timeouts                                                            */
        /*                                                                                                                */
        /*                                                                                                                */
        /* ************************************************************************************************************** */
        int getCountSegmentTimeouts()
        {
            return this->countSegmentTimeouts;
        }

        /**
         * processData()
         * 
         * Description:
         *   "timeslice". Called by main once per iteration
         * 
         * 
         **/
        void processData(){
            this->currentIteration++;
            this->processSend();
            this->processReceiveAndSendResponse();
        }

        /**
         * processSend()
         * 
         * Description:
         *   Manages Segment sending tasks
         * 
         * 
         **/
        void processSend(){
            Segment segmentSend;
            
            /**************************************************/
            //cout << "processSend(): Complete this..." << endl;
            /************************************************************
             * Add code to this function as needed!
             * 
             * You should pipeline segments to fit the flow-control window
             * The flow-control window is the constant RDTLayer.FLOW_CONTROL_WIN_SIZE
             * The maximum data that you can send in a segment is RDTLayer.DATA_LENGTH
             * These constants are given in # characters
             * 
             * Somewhere in here you will be creating data segments to send.
             * The data is just part of the entire string that you are trying to send.
             * The seqnum is the sequence number for the segment (in character number, not bytes)
             ************************************************************/

            if (!this->sendChannel.has_value()) {
                cout << "Send channel has not been set" << endl;
                return;
            }
            
            cout << "Length of Receive Unacked Packets List: 0" << endl;

            // send new seg of data only if
                // if # of unacknowledged chars < window size 
                // and if some data has not been sent
            int windowSize = this->oldSeqNum + this->FLOW_CONTROL_WIN_SIZE;
            int lengthDataToSend = (int)this->dataToSend.length();
            if (this->nextSeqNum < windowSize && this->nextSeqNum < lengthDataToSend) {
                int seqnum = this->nextSeqNum;  // set seqnum to the char idx to send
                // segSize = how many char to send
                int segSize = std::min(this->DATA_LENGTH, lengthDataToSend - seqnum);
                string data = this->dataToSend.substr(seqnum, segSize);    // seperate payload/substring
                
                // track sent segm and time for retransmission
                this->sentSeg[seqnum] = Segment();
                this->sentSeg[seqnum].setData(seqnum, data);
                this->sentTime[seqnum] = this->currentIteration;
                this->sentLengths[seqnum] = segSize;    // tack seg length for retransmission
                this->nextSeqNum += segSize;    // increment nextSeqNum by segSize

                /**************************************************/
                // Display sending segment
                segmentSend.setData(seqnum, data);
                cout << "Sending segment: " << segmentSend.to_string() << endl;

                //use the unreliable sendChannel to send the segment
                (*this->sendChannel)->send(segmentSend);

            }

            cout << "Length of Sent Unacked Packets List: " << this->sentSeg.size() << endl;

            // don't alter sentTime map during iterations
            vector<int> keysCheck;
            for (const auto& [seqNum, segTime]: this->sentTime) {
                keysCheck.push_back(seqNum);
            }

            // check for timeout (possible retransmission required)
            // For each [segNum, segTime] pair in the recorded sentTimes
                // iterate though the buffer (hash table) in key order
            // note: using const auto& to auto detect the variable type and to avoid copying segments
            for (int seqNum: keysCheck) {   
                // check if seg is not acknowledged && sender waited too long for ACK (timeout)
                    // then retransmission required
                int segTime = this->sentTime[seqNum];
                int timeWaited = this->currentIteration - segTime;
                if (seqNum >= this->oldSeqNum && seqNum < this->nextSeqNum && timeWaited > this->timeout) {
                    // timeout detected - retransmit
                    cout << "Timeout detected, retransmitting the following segment: " << this->oldSeqNum << endl;    // used for [debugging]
                    this->countSegmentTimeouts++;

                    // resent unacknowledged segs starting 
                    for (int seq = this->oldSeqNum; seq < this->nextSeqNum; ) {
                        // check if segment (seq) was saved in sentSeg
                        if (this->sentSeg.count(seq)) {
                            Segment& toResend = this->sentSeg[seq]; // get reference to seg at idx=[seq]
                            (*this->sendChannel)->send(toResend);  // use UnreliableChannel object to resend the seg
                            this->sentTime[seq] = this->currentIteration;   // update timestamp for seqnum

                            cout << "Resent segment: " << toResend.to_string() << endl; // used for [debugging]

                            if (this->sentLengths.count(seq)) {
                                seq += this->sentLengths[seq]; // increment seq pointer to next segment
                            } else {
                                cout << "Warning: sentLengths missing entry for seq: " << seq << ". Aborting retransmission..." << endl;
                                break;
                            }
                            
                        } else {
                            break;
                        }
                    }
                }
            }

            
        }

        /**
        * processReceiveAndSendResponse()
        * 
        * Description:
        *   Manages Segment receive tasks
        * 
        * 
        **/
        void processReceiveAndSendResponse(){
            Segment segmentAck; // Segment acknowledging packet(s) received

            if (!this->sendChannel.has_value()) {
                cout << "Send channel has not been set" << endl;
                return;
            }

            //This call returns a list of incoming segments (see Segment class)...
            vector<Segment> listIncommingSegments = (*receiveChannel)->receive();

            cout << "Length of Receieved Unacked Packets List: " << listIncommingSegments.size() << endl;

            /*************************************************
             * What segments have been received?
             * How will you get them back in order?
             * This is where a majority of your logic will be implemented
             *************************************************/

            /*************************************************
             * How do you respond to what you have received?
             * How can you tell data segments apart from ack segments?
             *************************************************/
            //cout << "processReceiveAndSendResponse(): Complete this..." << endl;

            // process each segment received
            for (Segment& seg : listIncommingSegments) {
                // check for segment corruption
                if (!seg.checkChecksum()) {
                    cout << "Drop corrupted segment: " << seg.to_string() << endl;
                    continue;
                }

                int ackNum = seg.getAck();  // get ack num for seg

                // process data seg (ack == -1 when seg is data seg)
                if (ackNum == -1) {
                    // get specific values from seg.to_string()
                    string segString = seg.to_string();
                    int seqNum = -1;
                    string payload = "";
                    size_t seqStart = segString.find("seq: ") + 5;
                    size_t seqEnd = segString.find(",", seqStart);
                    size_t dataStart = segString.find("data: ");

                    // if fields are found in string (!= npos means != not found)
                    if (seqStart != string::npos && seqEnd != string::npos && dataStart != string::npos) {
                        try {
                            seqNum = std::stoi(segString.substr(seqStart, seqEnd - seqStart));
                            payload = segString.substr(dataStart + 6);
                        } catch (...) {
                            cout << "Error parsing segment string: " << segString << endl;
                            continue;
                        }
                    } else {
                        cout << "Malformed segment string: " << segString << endl;
                        continue;
                    }

                    // buffer seg if not received, add segment to buffer
                    if (this->buffer.count(seqNum) == 0) {
                        this->buffer[seqNum] = payload;
                        cout << "Buffered segment: " << seg.to_string() << endl;
                    } else {
                        cout << "Duplicate segment dropped: " << seg.to_string() << endl;
                    }

                    // update expectedSeqNum forward through buffer
                    while (this->buffer.count(this->expectedSeqNum)) {
                        this->expectedSeqNum += this->buffer[this->expectedSeqNum].length();
                    }
                
                // if seg is ACK seg
                } else if (ackNum >= 0) {
                    if (ackNum > this->oldSeqNum) {
                        cout << "Received cumulative ACK: " << ackNum << endl;

                        this->oldSeqNum = ackNum;
                        
                        // loop using iterator (traverse sentSeg container)
                        // remove all seg that have been cumulatively acked
                        for (auto it = this->sentSeg.begin(); it != this->sentSeg.end(); ) {
                            if (it->first < ackNum) {
                                this->sentTime.erase(it->first);
                                this->sentLengths.erase(it->first);
                                it = this->sentSeg.erase(it);   // delete current element and update iterator
                            } else {
                                ++it;   // no erase occured, increment iterator
                            }
                        }
                    }

                }
                
                // send ack (new data received in order, then prepare and send ACK)
                if (this->expectedSeqNum > this->lastAckSent) {
                    /*************************************************
                     * Somewhere in here you will be setting the contents of the ack segments to send.
                     * The goal is to employ cumulative ack, just like TCP does...
                     *************************************************/
                    int acknum = this->expectedSeqNum;

                    /*************************************************/
                    // Display response segment
                    segmentAck.setAck(acknum);
                    cout << "Sending ack: " << segmentAck.to_string() << endl;

                    // Use the Unreliable sendChannel to send the ack packet
                    (*this->sendChannel)->send(segmentAck);
                    this->lastAckSent = this->expectedSeqNum;
                } else {
                    cout << "No new ACK; last ACK sent is: " << this->lastAckSent << endl;
                }
            }
        }
};
