package com.ipvs.assetadminservice.impl;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.utils.Utils;

public class FileSMQueue {
    //todo: need a way to find currently another instance is running.
    private static long lastAction_time_out = 3 * 60 * 1000;
    //db queue status
    public static final int QUEUE_STATUS_COMPLETED = -1000;
    public static final int QUEUE_STATUS_ERROR = -10;
    public static final int QUEUE_STATUS_NOT_STARTED = 1000;
    public static final int QUEUE_STATUS_ERROR_RESUME = 10;
    public static final int QUEUE_STATUS_RUNNING = 1;
    //Queue states 
    public static final String QUEUE_STATE_SUSPENDED = "suspended";
    public static final String QUEUE_STATE_WAITING = "waiting";
    public static final String QUEUE_STATE_RUNNING = "running";
    //priority
    public static final int PRIORITY_HIGH = 1;
    public static final int PRIORITY_MEDIUM = 2;
    public static final int PRIORITY_LOW = 3;
    private Document fileStateMachineDoc = null;
    private Document fileStateMachineListDoc = null;
    private boolean suspendGlobal = false;
    private boolean running = true;
    private String running_nid = "";
    private Object sync_object = new Object();
    private boolean busy = false;
    private boolean executed_first = true;
    private static FileSMQueue fileSMQueue = null;
    private String queueState = QUEUE_STATE_WAITING;
    public synchronized static FileSMQueue getInstance() {
        if (fileSMQueue == null) {
            fileSMQueue = new FileSMQueue();
        }
        return fileSMQueue;
    }

    private int running_count = 0;

    public String toString() {
        return "queueState=" + queueState +"running=" + running + " suspended=" + suspendGlobal + " count=" + running_count + " nid='" + running_nid + "'";
    }

    public FileSM checkOutFileSM(Element queueEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler, String state, String responseHandlerName) throws Exception {
        if (running_count > 0) {
            log.addInfo("FileSMQueue:checkOutFileSM : " + toString(), MRLog.INFO, MRLog.NOTIFY);
        }
        FileSM fileSM = new FileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
        synchronized (sync_object) {
            running_count++;
        }
        return fileSM;
    }

    public void checkinFileSM(FileSM stateMachine, MRLog log) throws Exception {
        stateMachine = null;
        synchronized (sync_object) {
            running_count--;
        }
        if (running_count > 0) {
            log.addInfo("FileSMQueue:checkinFileSM : " + toString(), MRLog.INFO, MRLog.NOTIFY);
        }
    }

    public void processQueue(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        synchronized (sync_object) {
            if (busy) {
                return;
            }
            busy = true;
        }
        if (running_count > 0) {
            log.addInfo("FileSMQueue:processQueue : Call to processQueue busy?(" + busy + ")  " + toString(), MRLog.INFO, MRLog.NOTIFY);
        }
        try {
            if (checkCurrentlyRunning(request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler)) {
                //something is running ...
                return;
            }
            while (startNext(request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler)) {
                //something is running ...
                //log.addInfo("FileSMQueue:processQueue********************repeat to startNext running_count=" + running_count + " (running_nid='" + running_nid + "')", MRLog.INFO, MRLog.NOTIFY);
            }
        } finally {
            synchronized (sync_object) {
                busy = false;
            }
        }
    }

    private boolean startNext(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        boolean callNext = false;
        try {
            Document doc = null;
            Element dbReadRootEL = null;
            Element queueEL = null;
            //read next in queue
            doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, "DBResponse");
            dbReadRootEL = MRXMLUtils.addElement(doc.getRootElement(), "DBRead");
            FileSMUtils.readNextInQueue(dbReadRootEL, request, xmpp, log, db);
            int objectCount = Utils.intValue(MRXMLUtils.getValueXpath(doc.getRootElement(), "//GetListObjectResponseData/ObjectCount", ""), 0);
            queueEL = MRXMLUtils.getElement(dbReadRootEL, "ExportImportFileListItem");
            String curr_nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
            if (queueEL == null) {
                log.addInfo("FileSMQueue:startNext: Nothing to process in queue. objectCount=" + objectCount + " ??? " + toString(), MRLog.INFO, MRLog.NOTIFY);
                synchronized (sync_object) {
                    running_nid = "";
                    running = false;
                    queueState = QUEUE_STATE_WAITING;
                }
                //todo:remove the catched xmls docs....until something appears in queue..
                //fileStateMachineDoc = null;
                //fileStateMachineListDoc = null;
                return false;
            }
            //if suspended do not start next one
            if (isSuspendGlobal()) {
                log.addInfo("FileSMQueue:startNext: SUSPENDED : objectCount:" + objectCount + " ??? " + toString(), MRLog.INFO, MRLog.NOTIFY);
                synchronized (sync_object) {
                    //running_nid = "";
                    running = false;
                }
                return false;
            }
            String state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
            String action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
            String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
            String responseHandlerName = FileSMUtils.getResponseHandler(type);
            FileSM stateMachine = checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
            try {
                synchronized (sync_object) {
                    running_nid = curr_nid;
                    //running: may be already set to true
                    running = true;
                    queueState = QUEUE_STATE_RUNNING;
                }
                int errorCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "errorCount", "0"), 0);
                if (errorCount > 3) {
                    MRXMLUtils.addAttribute(queueEL, "errorCount", "0");
                    MRXMLUtils.addAttribute(queueEL, "state_description", FileSMUtils.getActionDescription(FileSM.ACTION_QUEUE, log));
                    MRXMLUtils.addAttribute(queueEL, "state", FileSM.STATE_QUEUED);
                    MRXMLUtils.addAttribute(queueEL, "action", FileSM.ACTION_QUEUE);
                    state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
                    action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
                }
                String transition = FileSMUtils.getTransitionResume(action, log);                
                stateMachine.setState(state);
                stateMachine.handleTransition(transition);
                String sm_state = stateMachine.getState();
                callNext = FileSM.STATE_COMPLETED.equals(sm_state) || stateMachine.getError() != null;
                if(callNext){
                    System.out.println("FileSMQueue:startNext: callNext ??" + sm_state + " ERROR='" + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) + "'");
                }
            } finally {
                checkinFileSM(stateMachine, log);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return callNext;
    }
   
    private boolean checkCurrentlyRunning(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        return checkCurrentlyRunning(false, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler);
    }
    
    @SuppressWarnings("unchecked")
    private boolean checkCurrentlyRunning(boolean onreboot, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        boolean isRunning = false;
        //if timed out... try to check dms status...
        //in case dms is free and no response from dms...
        //close the current one and go to next
        try {
            if(!executed_first && onreboot) {
                return false;
            }
            boolean firstrun = executed_first;
            if(executed_first){
                synchronized (sync_object) {
                    if(!executed_first && onreboot) {
                        return false;
                    }
                    executed_first=false;
                }    
            }
            Document doc = null;
            Element dbReadRootEL = null;
            List<Element> list = null;
            int runningCount = 0;
            //first read the currently running    
            doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, "DBResponse");
            dbReadRootEL = MRXMLUtils.addElement(doc.getRootElement(), "DBRead");
            FileSMUtils.getCurrentInQueue(dbReadRootEL, request, xmpp, log, db);
            runningCount = Utils.intValue(MRXMLUtils.getValueXpath(doc.getRootElement(), "//GetListObjectResponseData/ObjectCount", ""), 0);
            if (runningCount == 0) {
                return isRunning;
            }
            int counter = 0;
            list = (List<Element>) MRXMLUtils.getListXpath(dbReadRootEL, "//ExportImportFileListItem[@queue_state='" + QUEUE_STATUS_RUNNING + "']");
            for (Element element : list) {
                try {
                    counter++;
                    Element queueEL = element;
                    String curr_nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
                    long dateUpdated = Utils.getLongValue(MRXMLUtils.getAttributeValue(queueEL, "dateUpdated", ""), 0);
                    long diff = System.currentTimeMillis() - dateUpdated;
                    if (diff < -1) {
                        //different server times
                        diff = diff * -1;
                    }
                    if (diff > lastAction_time_out || firstrun) {
                        //if prev action is timed out ... need to gracefully set error on prev queueEl and start next one
                        //send message to dms to see if any tar/untar pending
                        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
                        //String state_description = MRXMLUtils.getAttributeValue(queueEL, "state_description", "");
                        String state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
                        String action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
                        // In case of reboot, if the queue is stuck at "unpacking file" state, resume it from "initializing" state
                        // checking if the type is import and the action is handle_import_unpack
                        if(FileSM.TYPE_IMPORT.equalsIgnoreCase(type) && FileSM.ACTION_IMPORT_UNPACK.equalsIgnoreCase(action)){
                        	state = FileSM.STATE_READY;
                        	action = FileSM.ACTION_READY;
                        }
                        String responseHandlerName = FileSMUtils.getResponseHandler(type);
                        boolean checkDMSStatusNeeded = FileSMUtils.isDMSStatusCheckNeeded_pack_unpack(type, state);
                        boolean checkDMSStatusNeeded_ft = FileSMUtils.isDMSStatusCheckNeeded_ft(type, state);
                        if(onreboot  || firstrun ) {
                            String error = "MS rebooted, stale item in queue. '" + FileSMUtils.getActionDescription(action,log) + "'";
                            int errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                            FileSM.setQueueState(queueEL, FileSMUtils.getActionDescription(FileSM.ACTION_ERROR, log), 
                                    state, action, FileSMQueue.QUEUE_STATUS_ERROR);
                            int errorCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "errorCount", ""), 0)+1;
                            MRXMLUtils.addAttribute(queueEL, "errorCount", (errorCount) + "");
                            log.addInfo("FileSM:onreboot : " + errorCode + " " + error + "  " + toString(), MRLog.INFO, MRLog.NOTIFY);
                            FileSMUtils.addErrorToQueueElement(queueEL, errorCode, error);
                            FileSMQueue.getInstance().updateQueueElement_db(true, queueEL, request, db, log);
                            continue;
                        }                        
                        //log.addInfo("FileSMQueue:CHECK_BUSY checkDMSStatusNeeded " + checkDMSStatusNeeded +" checkDMSStatusNeeded_ft "+ checkDMSStatusNeeded_ft , MRLog.INFO, MRLog.NOTIFY);                    
                        else if (checkDMSStatusNeeded ) {
                            //check dms status..... hence ...return busy...
                            FileSM stateMachine = checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
                            try {
                                //log.addInfo("FileSMQueue::CHECK_BUSY checkCurrentlyRunning Remote timeout_check  ***(" + counter + "/" + runningCount + ")*****************TIMEOUT ? check dms status... time diff=" + diff + " state:" + state + " state_description:" + state_description, MRLog.INFO, MRLog.NOTIFY);
                                state = FileSM.STATE_CHECK_STATUS;
                                String transition = FileSMUtils.getTransitionOK(FileSM.ACTION_DMSSTATUS, log);//FileSM.CHECKSTATUS_TRANS_OK;
                                stateMachine.setState(state);
                                stateMachine.handleTransition(transition);
                            } finally {
                                checkinFileSM(stateMachine, log);
                            }
                            isRunning = true;
                            return isRunning;
                            
                        } else if ( (!checkDMSStatusNeeded_ft && ( diff > IPVSConst.getInstance().getMediafileDBTimeOut() || firstrun) ) 
                                || ( checkDMSStatusNeeded_ft &&  diff > IPVSConst.getInstance().getMediafileTimeOut() )) {
                            //todo: add for ft at filetransfer agent
                            FileSM stateMachine = checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
                            try {
                                //set current one as timed out.. and continue checking next...
                                log.addInfo("FileSMQueue:: Timed out during "+action+" '" + FileSMUtils.getActionDescription(action,log) + "'", MRLog.INFO, MRLog.NOTIFY);
                                String transition = FileSMUtils.getTransitionError(action, log);
                                stateMachine.setErrorCode(AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION);
                                stateMachine.setError("Timed out during '" + FileSMUtils.getActionDescription(action,log) + "'");
                                stateMachine.setState(state);
                                stateMachine.handleTransition(transition);
                            } finally {
                                checkinFileSM(stateMachine, log);
                            }
                        }
                        
                    } else {
                        isRunning = true;
                        //log.addInfo("FileSMQueue:checkCurrentlyRunning:CHECK_BUSY *** Not timed out (" + counter + "/" + runningCount + ")*****************, checking next one. diff=" + diff + ", nid=" + curr_nid, MRLog.INFO, MRLog.NOTIFY);
                    }
                }catch(Exception ex){
                    ex.printStackTrace();
                }finally{
                    //system rebooted
                    firstrun=false;
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return isRunning;
    }

    public void updateQueueElement_db(boolean done, Element queueEL, MRRequest request, MRDB db, MRLog log) throws Exception {
        String curr_nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "").trim();
        /*if (done) {
            log.addInfo("FileSMQueue:updateQueueElement*********done********** ' (curr_nid.equals(running_nid)) " + (curr_nid.equals(running_nid)) + " " + toString() + "\n\n", MRLog.INFO, MRLog.NOTIFY);
        }*/
        if (done) {
            setRunning_nid(curr_nid);
        }
        FileSMUtils.updateQueueElement_db(queueEL, request, db, log);
    }

    public boolean isSuspendGlobal() {
        synchronized (sync_object) {
            return suspendGlobal;
        }
    }

    public void setSuspendGlobal(boolean suspendGlobal) {
        synchronized (sync_object) {
            this.suspendGlobal = suspendGlobal;
            if(suspendGlobal){
                queueState = QUEUE_STATE_SUSPENDED;
            } else {
                queueState = QUEUE_STATE_RUNNING;
            }
        }
    }

    public String getQueueState() {
        synchronized (sync_object) {
            return this.queueState;
        }
    }    
        
    public String getRunning_nid() {
        return running_nid;
    }

    public void setRunning_nid(String curr_nid) {
        synchronized (sync_object) {
            if (curr_nid.equals(running_nid)) {
                running_nid = "";
            }
        }
    }

    public boolean isRunning() {
        return running;
    }

    public Document getFileStateMachineList(MRLog log) throws Exception {
        if (fileStateMachineListDoc == null) {
            fileStateMachineListDoc = MRXMLUtils.loadXMLResource(FileSM.FILE_STATELIST_XML, log);
        }
        return fileStateMachineListDoc;
    }

    public synchronized Document getFileStateMachine(MRLog log) throws Exception {
        if (fileStateMachineDoc == null) {
            fileStateMachineDoc = MRXMLUtils.loadXMLResource(FileSM.FILE_STATEMACHINE_XML, log);
            fileStateMachineListDoc = MRXMLUtils.loadXMLResource(FileSM.FILE_STATELIST_XML, log);
        }
        return fileStateMachineDoc;
    }

    public String resume(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        setSuspendGlobal(false);
        FileSMUtils.resumeQueue_db(request, db, log, xmpp);
        log.addInfo("FileSMQueue:resume:" + toString(), MRLog.INFO, MRLog.NOTIFY);
        //try executing next in queue---if nothing is running new one will be startrd
        FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);
        return null;
    }

    public String suspend(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        setSuspendGlobal(true);
        return null;
    }

    @SuppressWarnings("unchecked")
    public boolean cleanupDeleted(Document docdeleted, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        //block start next 
        busy = false;
        synchronized (sync_object) {
            if (busy) {
                return false;
            }
            busy = true;
        }
        try { //the agent response will come in DeleteExportImportFileRequestHandler
            String responseHandlerName = FileSMUtils.getResponseHandler(FileSM.TYPE_DELETE); //the agent response will come in d

            //read if any records there in db
            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, "DBResponse");
            Element dbReadRootEL = MRXMLUtils.addElement(doc.getRootElement(), "DBRead");
            FileSMUtils.getClearQueueCount(dbReadRootEL, request, xmpp, log, db);
            //if nohing in the queue just empty everything
            int objectCount = Utils.intValue(MRXMLUtils.getValueXpath(doc.getRootElement(), "//GetListObjectResponseData/ObjectCount", ""), 0);
            if (objectCount == 0) {
                //delete local files 
                FileSMUtils.cleanupLocalAllTemp(log);
                //send message to each DMS to delete all temp files
                //FileSMUtils.cleanupDMSTempFolder(request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, responseHandlerName);
                //update flags on deleted during incomplete Exporting
                FileSMUtils.resetFileStateFlag_db(null, "Busy", "Exporting", "Ready", "", request, db, log);
                
                //Cleanup temp files on the DMS
                FileSMUtils.cleanupDMSTempFiles(docdeleted, request, xmpp, db, resourceFactory, serviceAgentFactory, responseHandler, responseHandlerName, log);
                //no updating for import file status 
                //hope no files as "Busy", "Importing" 
                //...if there are then UI can delete them?... Need  to ask Iyer
                //if the queue is still there -- all incomplete imports can be gracefully deleted (as below).
                //however : we can easily delete all all incomplete import files ant their blobs ...Not a big thing
                if (docdeleted != null) {
                    List<Element> list = (List<Element>) MRXMLUtils.getListXpath(docdeleted.getRootElement(), "//"+AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                    for (Element element : list) {
                        try {
                            int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(element, "queue_state", ""), -1);
                            if (queue_state == FileSMQueue.QUEUE_STATUS_RUNNING || queue_state == FileSMQueue.QUEUE_STATUS_ERROR || queue_state == FileSMQueue.QUEUE_STATUS_ERROR_RESUME) {
                                //send message to each DMS to delete current queue element temp files
                                if (element != null) {
                                    String type = MRXMLUtils.getAttributeValue(element, "type", "");
                                    if(type.equalsIgnoreCase(FileSM.TYPE_IMPORT)) {
                                        String toAgentJID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@JID", "").trim();
                                        if(FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)){
                                            FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE, element, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                                        }
                                    }
                                }
                            }
                        } catch (Exception ex) {
                            log.addInfo("FileSMQueue:cleanupDeleted Error " + element.asXML(), MRLog.INFO, MRLog.NOTIFY);
                            ex.printStackTrace();
                        }
                    }
                    //delete any incomplete imports
                    FileSMUtils.cleanupFile_importDB(docdeleted, request, db, log);                    
                }
            } else {
                //if there is already something deleted 
                if (docdeleted != null) {
                    List<Element> list = (List<Element>) MRXMLUtils.getListXpath(docdeleted.getRootElement(), "//"+AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                    for (Element element : list) {
                        try {
                            int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(element, "queue_state", ""), -1);
                            if (queue_state == FileSMQueue.QUEUE_STATUS_RUNNING || queue_state == FileSMQueue.QUEUE_STATUS_ERROR || queue_state == FileSMQueue.QUEUE_STATUS_ERROR_RESUME) {
                                //send message to each DMS to delete current queue element temp files
                                String type = MRXMLUtils.getAttributeValue(element, "type", "");
                                if (element != null) {
                                    String toAgentJID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@JID", "").trim();
                                    if(FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)){
                                        /*
                                    	if (type.equalsIgnoreCase(FileSM.TYPE_EXPORT)) {
                                            FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_CLEANUP_TEMP_FILES, element, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                                        } else if (type.equalsIgnoreCase(FileSM.TYPE_IMPORT)) {
                                            FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE, element, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                                        }*/
                                    	FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE, element, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                                    }
                                }
                            }
                        } catch (Exception ex) {
                            log.addInfo("FileSMQueue:cleanupDeleted Error " + element.asXML(), MRLog.INFO, MRLog.NOTIFY);
                            ex.printStackTrace();
                        }
                    }
                    //update flags on deleted during incomplete Exporting
                    FileSMUtils.resetFileStateFlag_db(docdeleted, "Busy", "Exporting", "Ready", "", request, db, log);
                    //delete any incomplete imports
                    FileSMUtils.cleanupFile_importDB(docdeleted, request, db, log);
                }
            }
            
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            synchronized (sync_object) {
                busy = false;
            }
        }
        return true;
    }
     
    public void timeoutOnReboot(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler) throws Exception {
        try {
            MRLog.println("timeoutOnReboot executed_first " + executed_first);
            if(!executed_first){
                return ;
            }            
            synchronized (sync_object) {
                if(!executed_first){
                    return ;
                }
            }  
            checkCurrentlyRunning(true, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler) ;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
}
