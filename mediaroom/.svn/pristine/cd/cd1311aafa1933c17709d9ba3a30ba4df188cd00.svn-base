package com.ipvs.sessionservice.resource.handler; 

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.utils.Utils;

public class OffloadStateRequestHandler extends FileBaseRequestHandler {
   
	
	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document offloadStateRequestDoc = request.getMessage().getDocument();
		String requestName = MRXMLUtils.getAttributeValueXpath(offloadStateRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		Element offloadFileEl = MRXMLUtils.getElementXpath(offloadStateRequestDoc, "//OffloadFile");
		String fileNID = MRXMLUtils.getAttributeValue(offloadFileEl, "NID","");
		String transition = null;
		Element queueEL = null;
		boolean callNext = false;
		String error= null;
		int errorCode = -1;
		
		// create user request
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(offloadStateRequestDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		if (state == MediaStoreServiceConst.SUCCESS_STATE)
		{
			// Don't deleteGroup in case of ILM
			//MediaStoreServiceUtils.deleteMediaStreamFileDB(fileNID, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
			MRXMLDBUtils.deleteElement(request, log, db, fileNID);
		}
		try {
			String[] arrWhereXpath = { "//Internal/@fileNID='" + fileNID + "' and @queue_state > -1 and @state != 'completed' " };
			// do db read			
			queueEL=FileSMUtils.readQueueElement_db_whereXpath(request, db, log, arrWhereXpath);
			if (queueEL == null) {
				log.addInfo("FileSM:handleAgentResponse ABORT:requestName :='" + requestName + "' " + " ABORTING queueEL is NULL ", MRLog.INFO, MRLog.NOTIFY);
				callNext = true;
				return null;
			}			
			if (state == MediaStoreServiceConst.FAILURE_STATE) {
				error = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Description", "");
				if (error.equals("")) {
					String action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim();
					error = "Error during " + requestName + " '" + FileSMUtils.getActionDescription(action, log) + "'";
				}
				errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Code", ""),
						AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION);
			}
			int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""), -1);			
			if (queue_state != FileSMQueue.QUEUE_STATUS_RUNNING) {
				log.addInfo("FileSM:handleAgentResponse ABORT:: already done or timed out ?? queue_state " + queue_state + " " + queueEL.asXML(), MRLog.OK, MRLog.NOTIFY);
				callNext = true;
				return null;
			}
			String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim();
			transition = FileSMUtils.getTransitionFromEvent(request, xmpp, log, db, FileSM.TYPE_OFFLOAD, queueEL);
			if (transition != null) {
				FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, null,
						queueState, this.getClass().getName());
				try {
					if (error != null) {
						stateMachine.setErrorCode(errorCode);
						stateMachine.setError(error);
					}
					// call handle transition
					stateMachine.handleTransition(transition);
					String sm_state = stateMachine.getState();
					callNext = FileSM.STATE_COMPLETED.equals(sm_state) || stateMachine.getError() != null;
					if (callNext) {
						System.out.println("FileSM:handleAgentResponset: callNext ??" + sm_state + " ERROR='"
								+ ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) + "'");
					}
				} finally {
					FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
				}
			}

		} finally {
			if (callNext) {
				FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);
			}
		}
				return null;
	}
}
