package com.ipvsserv.xmpp;

import org.xmpp.packet.IQ;

public interface IQResultListener {
    void receivedAnswer(IQ packet);
    
    void answerTimeout(String packetId);
}
