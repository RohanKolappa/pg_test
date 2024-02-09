package com.ipvs.test;

import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.MRResourceI;
import com.ipvs.common.factory.ResourceTypes;
import com.ipvs.common.factory.StreamTypes;

public class TestResourceFactoryImpl implements MRResourceFactoryI {
	public MRResourceI getResource(String NID) {
		TestResource resource = new TestResource();
		resource.setNID(NID);
		resource.setName("Name_" + NID);
		if("SourceNID".equals(NID)) {
			resource.setResourceType(ResourceTypes.SOURCE_PORT);
			resource.setUserJID("TxBot@localhost");
			resource.setAgentID("TxAgent");
		}
		else if("DestNID".equals(NID)) {
			resource.setResourceType(ResourceTypes.DEST_PORT);
			resource.setUserJID("RxBot@ipvs.com");
			resource.setAgentID("RxAgent");
		}
		resource.setStreamTypeID(StreamTypes.V2D);
		String[] groupList = new String[1];
		groupList[0] = "all";
		resource.setGroupList(groupList);
		resource.setConfirmationAgentFullJID("confirm@ipvs.com" + "/" + resource.getAgentID());
		return resource;
	}
}
