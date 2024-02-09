package com.ipvsserv.nxd.mdc.iq;

import java.util.ArrayList;
import java.util.Hashtable;

import org.dom4j.Element;
import org.xmpp.packet.IQ;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.mdc.util.IpvsServPacketError;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.mdc.ComponentEngine;
import com.ipvsserv.xmpp.mdc.DomainManager;
import com.ipvsserv.xmpp.mdc.IQHandler;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IQHandlerMDCImpl implements IQHandler {
    private IQ iq = null;
    private String name = "";
    private ComponentEngine component = null;
    private DomainManager domainManger;
    private boolean dbFailed=true;
    private String dbFailedError=null;
    private static int counter=0;
    //private ComponentEngineI component=null;
    public IQHandlerMDCImpl(IQ iq, String name, ComponentEngine component, DomainManager domainManger) {
        this.iq = iq;
        this.name = name;
        this.component = component;
        this.domainManger = domainManger;
        try{
            //DBMigrationModule inst =(DBMigrationModule)IpvsServer.getInstance().getModule(DBMigrationModule.class);
            //isDBMigrationComplete = inst.isCompleted();
           //DBMigrationError= inst.getMigrationError();
            if (!IpvsServer.getInstance().isDBReady()) {
              dbFailed = false;
              dbFailedError= IpvsServer.getInstance().getDBFailedError();
            }
        }catch(Exception ex){
            MRLog.error(ex);
        }
    }
    
    public void run() {
        try {
            process(iq);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            iq = null;
            name = null;
            component = null;
            domainManger = null;
        }
    }
   
    public static int runnigRequest = 0;
    public synchronized boolean  acceptRequest() {
        if(runnigRequest > component.getMaxRequestLimit()){
            return false;
        }
        runnigRequest++;
        return true;
    }
    public synchronized void  removeRequest() {
        runnigRequest--;
    }
    public boolean process(IQ iq) {
        long enterTime = System.currentTimeMillis();
        String packetID = iq.getID() + "_" + name;
        boolean accepted = acceptRequest();
        if(!accepted){
            try {
                MRLog.error("MaxRequestLimit reached.... recjecting packet-" + packetID + " "+ iq.toXML());
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_max_request_limit_reached, "Server Busy, Please try again.", packetID);
            } catch (Exception ex) {
                MRLog.error("MaxRequestLimit reached.... failed to process packet-" + iq.toXML());
            }
            return false;
        }
        try {
            if (IQ.Type.error == iq.getType() || IQ.Type.result == iq.getType()) {
                return false;
            }
             
            String nodeID = "";
            RequestMDCParams params = null;
            DomainMDC domain = null;
            String jid = null;
            NIDSchemeMDC node = null;
            
            //IQ reply = IQ.createResultIQ(iq);        
            Element requestElement = iq.getChildElement();
            String namespace = requestElement.getNamespaceURI();
            //dummy function. may be needed in future as per pubsub specifications.
            if ("http://jabber.org/protocol/disco#info".equals(namespace)) {
                return IQHandlerUtils.getIQDiscoInfo(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            /*if(!isDBMigrationComplete){
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_migration_failed, DBMigrationError, packetID);
                return false;
            }*/
            String actionName = IQHandlerUtils.getActionName(component, requestElement, packetID);
            if (actionName == null) {
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.not_acceptable, "actionName is empty", packetID);
                return false;
            }
            
            Element childElement = requestElement.element(actionName);
            params = new RequestMDCParams(childElement, namespace);
            if (params.getDomainName() == null) {
                //no need to attach the whole packet childElement
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_service_domain_required, "IPVS domain name is empty", packetID);
                return false;
            }
            
            //set back ward compatibility
            actionName = params.getAction();
            packetID = actionName + "_" + iq.getID() + "_" + name;
            //if(actionName.equalsIgnoreCase("export") || actionName.equalsIgnoreCase("import")){
             //   MRLog.debug("import export request");
            //}
            /// check if the db is initiated and read atleast once
            try {
                component.dbInititated();
            } catch (Exception ex) {
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_service_domain_required, "Database is not initialized. ", packetID, ex);
                return false;
            }
            if(!dbFailed){
                IQ reply = IQ.createResultIQ(iq);
                IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_migration_failed, dbFailedError, packetID);
                if(counter==0){
                    MRLog.debug( "Abort all request. "+dbFailedError+". Currrent request '" + actionName + "' from='" +params.getSubscribeJID() + "' NID='" +params.getNodeID() +"'. " );
                }
                counter++;
                if(counter>20){
                    counter=0;
                    //MRLog.debug( "Abort all request. isDBMigrationComplete=false. Currrent request '" + actionName + "' from='" +params.getSubscribeJID() + "' NID='" +params.getNodeID() +"'. " );
                }
                return false;
            }      
            //read and validate domain
            domain = (DomainMDC) domainManger.getDomain(params.getDomainName());
            if (domain == null) {
                if (params.getAction().equalsIgnoreCase(RequestIQConst.CREATE_DOMAIN)) {
                    domain = (DomainMDC) domainManger.addDomain(params.getDomainName());
                } else {
                    //no need to attach the whole packet childElement
                    IQ reply = IQ.createResultIQ(iq);
                    IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.unknown_ipvs_domain, "No domain found for '" + params.getDomainName() + "'", packetID);
                    return false;
                    
                }
            }
            node = new NIDSchemeMDC();
            nodeID = params.getNodeID();
            if(!nodeID.isEmpty())
            	nodeID = nodeID.trim();
            
            if (nodeID.isEmpty()) {
                if (!actionName.equals(RequestIQConst.UNSUBSCRIBE)&& 
                        !params.getAction().equalsIgnoreCase(RequestIQConst.READ_QUERY) && 
                        !params.getAction().equalsIgnoreCase(RequestIQConst.CREATE_DOMAIN)  &&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.EXPORT_DB)  &&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.IMPORT_DB)  &&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.DELETE_DB) 	&&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.GET_SUBSCRIBERS)&&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.GET_SUBSCRIPTIONS) &&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.SUBSCRIBE) &&
                        !params.getAction().equalsIgnoreCase(RequestIQConst.UPDATESUBSCRIPTIONS)
                        ) {
                    //no need to attach the whole packet childElement
                    IQ reply = IQ.createResultIQ(iq);
                    IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_invalid_node_id, "NodeID is empty", packetID);
                    return false;
                }
            } else {
               	node.parse(nodeID);
               	if ((node.getType() == NIDSchemeMDC.UNKNOWN || node.getType() == NIDSchemeMDC.DOMAIN)) {
               		IQ reply = IQ.createResultIQ(iq);
               		IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_invalid_node_id, "Invalid nodeid '" + nodeID + "'", packetID);
               		return false;
               	}
            }
            
            //read jid
            if (!params.getSubscribeJID().equals("")) {
                jid = params.getSubscribeJID().trim();
            } else {
                jid = Dom4jUtils.getAttributeValue(childElement, "jid", "").trim();  
            }
            
            //call actionName
            if (MRLog.isPacketdetailsLogEnable()) {
                MRLog.debug(actionName + " " + iq.toXML());
            }
            if (MRLog.isIpvsxmlpacketsLogEnable()) {
                MRLog.info(packetID + " (" + params.getAction() + ") request  " + params.toXML());
            }
                     
            //domain create and delete
            if (actionName.equals(RequestIQConst.CREATE_DOMAIN)) {
                return IQHandlerUtils.createDomain(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.DELETE_DOMAIN)) {
                return IQHandlerUtils.deleteDomain(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //add / replace
            if (actionName.equals(RequestIQConst.ADD)) {
                return IQHandlerUtils.add(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.REPLACE)) {
                return IQHandlerUtils.replace(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //delete
            if (actionName.equals(RequestIQConst.DELETE)) {
                return IQHandlerUtils.delete(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //read
            if (actionName.equals(RequestIQConst.READ_SINGLE)) {
                return IQHandlerUtils.readSingle(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.READ_LIST)) {
                return IQHandlerUtils.readList(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //disco items
            if (actionName.equals(RequestIQConst.GETCHILDREN)) {
                return IQHandlerUtils.getChildren(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.IS_OBJECT_EXISTS)) {
                return IQHandlerUtils.objectExists(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //query
            if (actionName.equals(RequestIQConst.READ_QUERY)) {
                return IQHandlerUtils.readQuery(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //subscribe
            if (actionName.equals(RequestIQConst.SUBSCRIBE)) {
            	return IQHandlerUtils.subscribeObject(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            //update Subscriptions
            if (actionName.equals(RequestIQConst.UPDATESUBSCRIPTIONS)) {
                return IQHandlerUtils.updateSubscriptions(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            if (actionName.equals(RequestIQConst.UNSUBSCRIBE)) {
                return IQHandlerUtils.unsubscribeObject(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            // import / export 
            if (actionName.equals(RequestIQConst.EXPORT_DB)) {
                return IQHandlerUtils.exportDB(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.IMPORT_DB)) {
                return IQHandlerUtils.importDB(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if (actionName.equals(RequestIQConst.DELETE_DB)) {
                return IQHandlerUtils.deleteDB(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            if(actionName.equals(RequestIQConst.GET_SUBSCRIBERS)) {
            	return IQHandlerUtils.getSubscribers(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            if(actionName.equals(RequestIQConst.GET_SUBSCRIPTIONS)) {
            	//return IQHandlerUtils.getSubscriptions(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
            }
            
            IQ reply = IQ.createResultIQ(iq);
            IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.service_unavailable, "service unavailable actionName='" + actionName + "'", packetID);
        } catch (Exception ex) {
            IQ reply = IQ.createResultIQ(iq);
            MRLog.error("failed to process packet-" + iq.toXML());
            IQHandlerUtils.sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.internal_server_error, "failed to process IQ.", packetID, ex);
        } finally {
            if(accepted) {
                removeRequest();
            }
        }
        
        return false;
    }    
}
