package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.GetFileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

public class GetPVRFileRequestHandler extends GetFileBaseRequestHandler{


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.PVRFILE + SessionServiceConst.XQUERYXML;
		Document responseDoc = super.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, xqueryFileName,SessionServiceConst.GET_PVR_FILE);

		List<String> resourceNIDList = MRXMLUtils.getValuesXpath(responseDoc, "//File/@NID");
		if(resourceNIDList == null)
			return responseDoc;

		Boolean includeMediaRoomInfo = false;
		int startFrom = 1;
		int countToFetch = -1;

		/**
		 * fill PVR startTC and persistent information
		 */		
		
		XMPPTransaction.startBlock("getSessionListWithMediaRoomInfo");
		String whereXpath="//Entity[@type='PVRFile']";
		Document sessionListDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, startFrom, countToFetch, includeMediaRoomInfo,whereXpath);
		XMPPTransaction.endBlock("getSessionListWithMediaRoomInfo");
		List<?> listOfPVREntity = MRXMLUtils.getListXpath(sessionListDoc, "//Entity");
		if(listOfPVREntity.isEmpty()){
			return responseDoc;
		}
		for(Object object:listOfPVREntity){
			Element	pvrEntityEl = (Element) object;
			String pvrNID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(pvrEntityEl, "File"), SessionServiceConst.RESOURCENID);
			Element pvrFileElement = MRXMLUtils.getElementXpath(responseDoc,"//File[@NID='"+pvrNID+"']");
			if(pvrFileElement==null){
				continue;
			}
			Element sessionElement = MRXMLUtils.getParentElement(pvrEntityEl).getParent();

			//update persistent
			String type = MRXMLUtils.getAttributeValue(sessionElement, SessionServiceConst.TYPE_ATTRIBUTE,"");
			String persistent = SessionServiceConst.FALSE;
			if(type.equals(SessionServiceConst.SYSTEM)){
				persistent = SessionServiceConst.TRUE;
			}
			MRXMLUtils.setAttributeValue(pvrFileElement, SessionServiceConst.PERSISTENT_ATTRIBUTE, persistent);

			String connId = MRXMLUtils.getAttributeValueXpath(sessionElement, ".//Connection[@dstEntityId='"+MRXMLUtils.getAttributeValue(pvrEntityEl, SessionServiceConst.ID,"")+"']",SessionServiceConst.ID, "");
			Element pvrElement = MRXMLUtils.getElementXpath(sessionElement, ".//PVR[@connId='"+connId+"']");

			String strStartTC = MRXMLUtils.getAttributeValue(pvrElement, SessionServiceConst.START_TC_ATTRIBUTE,"");
			String strTrimlength = MRXMLUtils.getAttributeValue(pvrElement, SessionServiceConst.TRIMLENGTH,"");

			//update start tc
			MRXMLUtils.setAttributeValue(pvrFileElement, SessionServiceConst.START_TC_ATTRIBUTE, strStartTC);

			//update trimlength
			MRXMLUtils.setAttributeValue(pvrFileElement, SessionServiceConst.TRIMLENGTH, strTrimlength);

			//update playlength
			long startTC = Utils.getLongValue(strStartTC, 0);
			long trimlength = Utils.getLongValue(strTrimlength, 0);
			long now =xmpp.getCurrentTimeMillis();
			if(startTC!=0){
				MRXMLUtils.setAttributeValue(pvrFileElement, SessionServiceConst.PLAY_LENGTH, String.valueOf(( (now-startTC) < trimlength ? (now-startTC) : trimlength )));
			} else {
				MRXMLUtils.setAttributeValue(pvrFileElement, SessionServiceConst.PLAY_LENGTH, "0");
			}
		}

		return responseDoc; 

	}
}
