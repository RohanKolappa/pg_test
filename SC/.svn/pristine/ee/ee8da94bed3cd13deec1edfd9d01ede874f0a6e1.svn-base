package com.barco.nimbus.mediaroom;

import com.barco.nimbus.api.MessageBusI;
import com.barco.nimbus.api.common.SCMessage;
import com.ipvs.nimbus.MRMessageBusI;
import com.ipvs.nimbus.MRSCMessage;

public class MRComponentMessageBus implements MRMessageBusI {
	
	MessageBusI messageBus;
	
	MRComponentMessageBus(MessageBusI messageBus){
		this.messageBus = messageBus;
	}

	@Override
	public void sendMessage(MRSCMessage message) throws Exception {
		SCMessage scMessage = new SCMessage(
					message.getProtocol(), 
					message.getMessageDoc(), 
					message.getType(), 
					message.getService(),
					message.getRootName(), 
					message.getClientData(),
					message.getClientCallback(),
					message.getFrom(),
					message.getTo(), 
					message.getLogLevel());
		scMessage.setState(message.getState());
		scMessage.setId(message.getId());
		scMessage.setExtNamespace(message.getExtNamespce());
		this.messageBus.sendMessage(scMessage);
		
	}

	@Override
	public String getJID() {
		return this.messageBus.getJID();
	}

	@Override
	public void subscribeUserPresence(String userJID) throws Exception{
		this.messageBus.subscribeUserPresence(userJID);
	}

	@Override
	public void unSubscribeUserPresence(String userJID) throws Exception{
		this.messageBus.subscribeUserPresence(userJID);
	}

	@Override
	public void addDiscoFeature(String feature) {
		this.messageBus.addDiscoFeature(feature);
	}
}
