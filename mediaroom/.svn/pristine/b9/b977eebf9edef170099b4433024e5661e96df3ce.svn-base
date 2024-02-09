package com.ipvs.policyadminservice.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.xmpp.client.DBUpdateEvent;


public class PolicyAdminAgent implements ServiceAgentI {
    private MRLog log = null;
    //private MRDB db = null;
    //private XMPPI xmpp;
     ServiceAgentFactoryI serviceAgentFactory = null;

    /*  @SuppressWarnings("unused")
      private MRResourceFactoryI resourceFactory;
    */
    private String userJID = null;
    private String openfireUserJID = null;
    private PEMDBUtils objPEMDBUtils = null;
    private static PolicyAdminAgent thePolicyAdminAgent = null;
    
    public static synchronized PolicyAdminAgent getPolicyAdminAgent() {
    	if(PolicyAdminAgent.thePolicyAdminAgent == null)
    		PolicyAdminAgent.thePolicyAdminAgent = new PolicyAdminAgent();
    	return PolicyAdminAgent.thePolicyAdminAgent;
    }
    
    @Override
    public boolean handleEvent(MRClient client, Document doc, Element event)
        throws Exception {
    	if(objPEMDBUtils == null) {
    		return false;
    	}
        if (objPEMDBUtils.getParentNIDWatchList() == null) {
            return false;
        }
        DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(doc);
        if (dbUpdateEvent.getUpdateEventElement()==null) {
            return false;
        }
        boolean isDelete = false;
        boolean isAdd = false;

        String policyNID = dbUpdateEvent.getSubscribedNID();

        if ((policyNID == null) || (policyNID.length() == 0)) {
            return false;
        }

        String itemNID = null;
        String[] policyNIDArr = null;


        if (objPEMDBUtils.getParentNIDWatchList().containsKey(policyNID) || (!IPVSConst.getInstance().isEnableNewPubSub() && objPEMDBUtils.getNodeIDWatchList().containsKey(policyNID))) {
            String toUser = dbUpdateEvent.getTo();

            if (!toUser.equals(openfireUserJID)) {
                return false;
            }

            if (!IPVSConst.getInstance().isEnableNewPubSub()) {
                if (objPEMDBUtils.getParentNIDWatchList().containsKey(policyNID)) {
                    if(dbUpdateEvent.isAddorUpdate()){
                    itemNID=dbUpdateEvent.getUpdateNID();
                    isAdd = true;
                    }
                }


                if(dbUpdateEvent.isDelete()){
                    isDelete = true;
                }
            }

            try {
                String objectType = null;

                if (objPEMDBUtils.getParentNIDWatchList().containsKey(policyNID)) {
                    objectType = objPEMDBUtils.getParentNIDWatchList().get(policyNID);
                } else if (!IPVSConst.getInstance().isEnableNewPubSub()) {
                    if (objPEMDBUtils.getNodeIDWatchList().containsKey(policyNID)) {
                        objectType = objPEMDBUtils.getNodeIDWatchList().get(policyNID);
                    }
                }

                objPEMDBUtils.setPolicyDirtyFlag(objectType, true);
                
                PolicyManager.handlePolicyChanged(objectType,serviceAgentFactory);

                log.addInfo("Refresh Node objectType=" + objectType + " userJID=" + userJID, MRLog.NOTIFY, MRLog.NOTIFY);

                if (!IPVSConst.getInstance().isEnableNewPubSub()) {
                    if (isDelete) {
                        deleteNodeFromWatchList(policyNID);
                        log.addInfo("DeleteNodeFrom Watch List objectType=" + objectType + " NodeID=" + policyNID);
                    }

                    if (isAdd) {
                    	setNodeIDWatchList(objectType, itemNID);
                        log.addInfo("Add Node to Watch List objectType=" + objectType + " NID=" + itemNID);
                    }
                }
            } catch (Exception exp) {
                exp.printStackTrace();
            }
        }

        return false;
    }

    private void deleteNodeFromWatchList(String nodeID) {
    	objPEMDBUtils.getNodeIDWatchList().remove(nodeID);
    }

    /*  private void setNodeIDWatchList(List<?> objNodeList) {
          Iterator<?> objNodeListIT = objNodeList.iterator();
          Element objEL = null;
    
          while (objNodeListIT.hasNext()) {
              objEL = (Element) objNodeListIT.next();
              nodeIDWatchList.put(MRXMLUtils.getAttributeValue(objEL, "NID", ""), objEL.getName());
          }
      }
    */
    private void setNodeIDWatchList(String objectType, String NID) {
    	objPEMDBUtils.getNodeIDWatchList().put(NID, objectType);
    }

    @Override
    public String handleMessage(MRClient client, String fromJID, String message)
        throws Exception {
        return null;
    }

    @Override
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc) {
        this.log = log;
        //this.db = db;
        //this.xmpp = xmpp;
        /*    this.resourceFactory = resourceFactory;
            
        */
        this.userJID = userJID;
       try {
    	   this.serviceAgentFactory = xf.sf(userJID);
    	      
    	   
            //bare jid without resource
            openfireUserJID = ServiceAgentUtils.getBareJID(userJID);
            objPEMDBUtils = PEMDBUtils.getInstatnce(xf.xmpp(userJID));
            objPEMDBUtils.intializeWatchList(MRRequest.createRequest(this.userJID), db, log, xf.xmpp(userJID));
            log.addInfo("Initialize Policy Cache",MRLog.INFO,MRLog.NOTIFY);
    		ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), PolicyAdminAgent.class.getName(), MediaStoreServiceConst.READY, xf.xmpp(userJID), this.log);
            
        } catch (Exception exp) {
            exp.printStackTrace();
        }
    }

    public static Element getMatchingNodes(String parentElementNID, String objectType, String[] arrXpath, MRRequest request, MRDB db, MRLog log) throws Exception {
    	return thePolicyAdminAgent.objPEMDBUtils.getMatchingNodes(parentElementNID, objectType, arrXpath, request, db, log);
    }

	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub
		
	}

	}
