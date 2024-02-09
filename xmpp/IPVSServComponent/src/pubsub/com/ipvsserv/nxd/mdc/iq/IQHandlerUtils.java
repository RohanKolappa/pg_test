package com.ipvsserv.nxd.mdc.iq;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Packet;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.EngineMDCUtils;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.SchemaConst;
import com.ipvsserv.nxd.adminapi.db.DBActionsStatus;
import com.ipvsserv.nxd.adminapi.mediadir.DeleteMediaDir;
import com.ipvsserv.nxd.adminapi.mediadir.ExportMediaDir;
import com.ipvsserv.nxd.adminapi.mediadir.ImportMediaDir;
import com.ipvsserv.nxd.adminapi.mediafile.DeleteMediaFile;
import com.ipvsserv.nxd.adminapi.mediafile.ExportMediaFile;
import com.ipvsserv.nxd.adminapi.mediafile.ImportMediaFile;
import com.ipvsserv.nxd.mdc.sub.PubsubSubscription;
import com.ipvsserv.nxd.mdc.util.IpvsServPacketError;
import com.ipvsserv.nxd.mdc.util.MRPayloadHelper;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.nxd.mdc.util.IpvsServPacketError.IPVSCondition;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.mdc.ComponentEngine;
import com.ipvsserv.xmpp.mdc.DomainManager;
import com.ipvsserv.xmpp.mdc.IpvsDomain;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IQHandlerUtils {
    private static String servicever=null;
    public static String getServicever() {
        if(servicever==null){
            servicever="serviceVer='"+IpvsServer.getInstance().getServerVersion()+"'";
        }
        return servicever;
    }

    ////////////////////////////////////// static method getIQDiscoInfo  //////////////////////////////////////
    public static boolean getIQDiscoInfo(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, IpvsDomain domain, NIDSchemeMDC node, String jid, long enterTime) {
        // Send reply with success
        try {
            send(component, IQ.createResultIQ(iq), packetID);
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
        }
        return true;
    }

    ////////////////////////////////////// static method createDomain  //////////////////////////////////////
    //todo:deprecated
    public static boolean createDomain(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            node.setDomain(params.getDomainName());
            try {
                if (component.getDBApi().isCollectionExists(packetID, params.getDomainName(), SchemaConst.COL_TYPE_DATA, node.getCollectionName())) {
                    component.getDBApi().deleteCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_DATA, node.getCollectionName());
                }
            } catch (DBExecutionException ex) {
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " check and delete Collection " + node.getCollectionName() +" " + ex.getMessage(), packetID, ex);
                return true;
            }

            try {
                component.getDBApi().addCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_DATA, node.getCollectionName()); 
            } catch (DBExecutionException ex) {
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " create Collection " + node.getCollectionName()+" " + ex.getMessage(), packetID, ex);
                return true;
            }
             // subscription
            try {
                if (component.getDBApi().isCollectionExists(packetID, params.getDomainName(), SchemaConst.COL_TYPE_SCHEMA, node.getCollectionNameSub())) {
                    component.getDBApi().deleteCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_SCHEMA, node.getCollectionNameSub());
                }
            } catch (DBExecutionException ex) {
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " check and delete schema collection " + node.getCollectionNameSub()+" " + ex.getMessage(), packetID, ex);
                return true;
            }

            try {
                component.getDBApi().addCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_SCHEMA, node.getCollectionNameSub());
            } catch (DBExecutionException ex) {
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " create schema collection " + node.getCollectionNameSub()+" " + ex.getMessage(), packetID, ex);
                return true;
            }

 
            try {
                component.getDBApi().addIndex(packetID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
            } catch (DBExecutionException ex) {
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " create index " + node.getIndexName()+" " + ex.getMessage(), packetID, ex);
                return true;
            }

            try {
                component.getDBApi().addIndex(packetID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
            } catch (DBExecutionException ex) {
                //neglect
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " create collection index " + node.getIndexNameCol()+" " + ex.getMessage(), packetID, ex);
                return true;
            }

            nodeID = params.getAction();
            sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;

            return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add_domain, params.getAction() + " send response"+ ex.getMessage(), packetID);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " createRoot " + nodeID);
        }
    }
    //todo:deprecated
    public static boolean deleteDomain(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            node.setDomain(params.getDomainName());
            try {
                component.getDBApi().deleteCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_DATA, node.getCollectionName());
            } catch (DBExecutionException ex) {
                //neglect
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_delete_domain, params.getAction() + " delete collection " + node.getCollectionName(), packetID, ex);
                return true;
            }
            try {
                //if (domain.isSubscriptionsSaveEnabled()) {
                 component.getDBApi().deleteCollection(packetID, params.getDomainName(), SchemaConst.COL_TYPE_SCHEMA, node.getCollectionNameSub());
                //}
            } catch (DBExecutionException ex) {
                //neglect
                MRLog.error(iq.toXML(), ex);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_delete_domain, params.getAction() + " delete schema collection " + node.getCollectionNameSub(), packetID, ex);
                return true;
            }
            sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;

            domainManger.removeDomain(params.getDomainName()); //todo: make part of component
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_delete_domain,  params.getAction() + " send response"+ ex.getMessage(), packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " deleteRoot " + nodeID);
        }
        return true;
    }

    ////////////////////////////////////// static method create createRoot //////////////////////////////////////
 
  
    public static boolean add(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        String xml = params.getItemDataStr();
        List<String> nidList =  new ArrayList<String>(); ;
        boolean childAdd = false;
        Document docAdd = null;
        try {
            try {
                try {
                    //domain.getDbCache().markDirty(node, nodeID, params.getAction());
                    //domain.getDbCache().removeFromCache(node, nodeID, params.getAction());
                    childAdd = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
                    if (childAdd) {
                        docAdd = getDocumentList(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
                        nidList = renameEL(docAdd, "child added");
                        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                            component.getDBApi().addNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                        }else{
                            component.getDBApi().addNode(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                        }
                    } else {
                        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                            docAdd = Dom4jUtils.getDocument();
                            nidList = new ArrayList<String>();
                            Dom4jUtils.addElement(docAdd, "DBResp");
                            if(params.getItemData() !=null){
                                Dom4jUtils.addElement(docAdd.getRootElement(), (Element) params.getItemData().detach());
                            }
                            nidList.add(nodeID);                        
                            component.getDBApi().addDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), node.getCollectionName(), xml);
                        }else{
                            throw new DBExecutionException("document NID not provided");
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(iq.toXML(), ex);
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add, params.getAction() + " " + nodeID, packetID, ex);
                    return false;
                } //finally {
                   // domain.getDbCache().unmarkDirty(node, nodeID, params.getAction());
                //}
                //sendCreateReply(component, reply, packetID, nodeID);
                if (nidList == null || nidList.size() == 0) {
                    sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
                } else {
                    sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), docAdd.getRootElement().asXML());
                }
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_add, params.getAction() + " send Reply " + nodeID, packetID, ex);
                return false;
            }
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            //after send packet processing 
            if (!component.getPresenceHandler().isUserInPresenceSubList(jid)) {
                component.getPresenceHandler().presenceSubscriptionRequired(jid);
            }
            try{
                domain.getSubs().addDocumentSub(node, jid, params);
             }catch(Exception ex){
                 MRLog.error(iq.toXML(), ex);
             }            
            for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                String nid = iterator.next();
                NIDSchemeMDC schema = new NIDSchemeMDC();
                schema.parse(nid);
                // send notifications
                domain.getSubs().sendAddNotification(component, schema, iq, schema.getDocNodeID(), xml, params, component.getNotificationsRouter());
            }
        return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable()){
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " create " + nodeID);
            }
        }
    }

    public static boolean replace(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        String xml = params.getItemDataStr();
        List<String> nidList =   new ArrayList<String>(); 
        //boolean deletedDoc=false;
        boolean childReplace = false;
        Document docReplace = null;
        boolean isXqueryPrefix = false;
        try {
            try {
                try {
                    // MRLog.debug("TEMP: xml " + xml +" iq " + iq.toXML());
                    //domain.getDbCache().markDirty(node, nodeID, params.getAction());
                    //domain.getDbCache().removeFromCache(node, nodeID, params.getAction());
                    isXqueryPrefix = (params.getXQueryPrefix() !=null && !params.getXQueryPrefix().trim().equals(""));
                    childReplace = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
                    //old replace
                    if (!childReplace && node.getType() == NIDSchemeMDC.DOCUMENT && (params.getWhereXpath() == null || params.getWhereXpath().size() == 0)) {
                        docReplace = Dom4jUtils.getDocument();
                        Dom4jUtils.addElement(docReplace, "DBResp");
                        if(params.getItemData() !=null){
                            Dom4jUtils.addElement(docReplace.getRootElement(), (Element) params.getItemData().detach());
                        }
                        nidList.add(nodeID);
                        component.getDBApi().updateNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                    } else {
                        //new replace
                        docReplace = getDocumentList(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
                        if (docReplace != null && docReplace.getRootElement() != null) {
                            if (childReplace) {
                                if(isXqueryPrefix){
                                    nidList = renameEL(docReplace, "BULK child replaced");
                                } else {
                                    nidList = renameEL(docReplace, "child replaced");
                                }
                                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                                    component.getDBApi().updateNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                } else {
                                    component.getDBApi().updateNode(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                }
                            } else {
                                if(isXqueryPrefix){
                                    nidList = renameEL(docReplace, "BULK document replaced");
                                } else {
                                    nidList = renameEL(docReplace, "document replaced");
                                }
                                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                                    //one read has increased here. todo: may be get rid of this if,  its a performance hit.
                                    component.getDBApi().updateNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                } else {
                                    //todo: error if bulk document replace.... which is wrong any way
                                    Document doc = Dom4jUtils.stringToDocument(xml);
                                    Element root = doc.getRootElement();
                                    for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                                        String nid = iterator.next();
                                        NIDSchemeMDC schema = new NIDSchemeMDC();
                                        schema.parse(nid);
                                        if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
                                            Dom4jUtils.addAttribute(root, "NID", nid.trim());
                                            component.getDBApi().updateNode(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), schema.getNID(), node.getCollectionName(), root.asXML(), params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                        }
                                    }
                                }
                            }
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(iq.toXML(), ex);
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_replace, params.getAction() + " " + nodeID, packetID, ex);
                    return false;
                } finally {
                    //domain.getDbCache().unmarkDirty(node, nodeID, params.getAction());
                }

                if (nidList == null || nidList.size() == 0 ) {
                    sendReply(component, reply, params.getAction(),  reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
                } else {
                    sendReply(component, reply, params.getAction(),  reply.getID(), params.getRequestAttributes(), docReplace.getRootElement().asXML());
                }

            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_replace, params.getAction() + " send reply " + nodeID, packetID, ex);
                return false;
            }

            sendTimeDiff = System.currentTimeMillis() - enterTime;
            //after send packet processing 
            if (!component.getPresenceHandler().isUserInPresenceSubList(jid)) {
                component.getPresenceHandler().presenceSubscriptionRequired(jid);
            }
            //notify the each document for now: todo fix pattern for common notifications
            try{
                domain.getSubs().addDocumentSub(node, jid, params);
             }catch(Exception ex){
                 MRLog.error(iq.toXML(), ex);
             }
            if(isXqueryPrefix){
                for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                    String nid = iterator.next();
                    NIDSchemeMDC schema = new NIDSchemeMDC();
                    schema.parse(nid);
                    // send notifications
                    domain.getSubs().sendUpdateNotification(component, schema, iq, "BULK_NIDS", xml, params, component.getNotificationsRouter());
                    break;
                }
            }else{
                for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                    String nid = iterator.next();
                    NIDSchemeMDC schema = new NIDSchemeMDC();
                    schema.parse(nid);
                    // send notifications
                    domain.getSubs().sendUpdateNotification(component, schema, iq, schema.getDocNodeID(), xml, params, component.getNotificationsRouter());
                }
            }
            return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " create " + nodeID);
        }
    }

    //////////////////////////////////////static method delete  //////////////////////////////////////
    public static boolean delete(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        List<String> nidList =  new ArrayList<String>(); 
        boolean childDelete = false;
        Document docDelete = null;
        try {
            try {
                try {
                    //domain.getDbCache().markDirty(node, nodeID, params.getAction());
                    //domain.getDbCache().removeFromCache(node, nodeID, params.getAction());
                    childDelete = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
                    if (!childDelete && node.getType() == NIDSchemeMDC.DOCUMENT 
                            && params.getObjectType() != null && !params.getObjectType().trim().equals("")) {
                        String objectType = params.getObjectType().trim();
                        docDelete = Dom4jUtils.stringToDocument("<DBResp><" + objectType.trim() + " NID='" + nodeID + "' parentNID='" + node.getCollectionNodeID() + "'/>document deleted</DBResp>");
                        nidList.add(nodeID);
                        component.getDBApi().deleteDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), node.getCollectionName());
                    } else {
                        docDelete = getDocumentList(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
                        if (docDelete != null && docDelete.getRootElement() != null) {
                            if (childDelete) {
                                nidList = renameEL(docDelete, "child deleted");
                                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                                    component.getDBApi().deleteNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(),  params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                } else {
                                    component.getDBApi().deleteNode(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID,  node.getCollectionName(), params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
                                }
                            } else {
                                nidList = renameEL(docDelete, "document deleted");
                                //TODO:component.getDBApi().deleteNode(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, params.getWhereXpath(), params.getChildXpath());
                                for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                                    String nid = iterator.next();
                                    NIDSchemeMDC schema = new NIDSchemeMDC();
                                    schema.parse(nid);
                                    if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
                                        component.getDBApi().deleteDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, schema.getDocUri(), node.getCollectionName());
                                    }
                                }
                            }
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(iq.toXML(), ex);
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_delete, params.getAction() + " " + nodeID, packetID, ex);
                    return true;
                }// finally {
                 //   domain.getDbCache().removeFromCache(node, nodeID, params.getAction() + " " + nodeID);
                 //   domain.getDbCache().unmarkDirty(node, nodeID, params.getAction());
                //}
                if (docDelete == null || docDelete.getRootElement() == null) {
                    sendReply(component, reply,  params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
                } else {
                    sendReply(component, reply,  params.getAction(), reply.getID(), params.getRequestAttributes(), docDelete.getRootElement().asXML());
                }
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_delete, params.getAction() + " send reply " + nodeID, packetID, ex);
            }
            //after send packet processing
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            //notify the each document for now: todo fix pattern for common notifications
            try{
                domain.getSubs().addDocumentSub(node, jid, params);
            }catch(Exception ex){
                 MRLog.error(iq.toXML(), ex);
            }            
            if (docDelete == null || docDelete.getRootElement() == null) {
                //nothing is deleted 
            } else {
                //notify the each document for now: todo fix pattern for common notifications
                for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
                    String nid = iterator.next();
                    NIDSchemeMDC schema = new NIDSchemeMDC();
                    schema.parse(nid);
                    List<PubsubSubscription> subList = domain.getSubs().getDeleteSubscription(schema, params);
                    domain.getSubs().sendDeleteNotification(component, schema, subList, iq, schema.getNID(), params, component.getNotificationsRouter());
                    if (!childDelete) {
                        domain.getSubs().deleteDocumentSub(schema, params);
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable()) {
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " delete " + nodeID);
            }
        }
        return true;
    }

    ////////////////////////////////////// static method getPublishedItems  executeQuery  //////////////////////////////////////
    public static boolean readSingle(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        return readList(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
    }

    public static boolean readList(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        String xml = "";
        try {
            try {
                boolean useCatch = false;
                xml = "";
                //startFrom is zero based
                int countToFetchLimit = -1;
                int startFrom = params.getStartFrom() + 1;
                int countToFetch = params.getCountToFetch();
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    countToFetchLimit = domain.getCountToFetch_ByListName(node.getCollectionId());
                    if (countToFetch < 0) {
                        countToFetch = countToFetchLimit;
                    } else if (countToFetch > countToFetchLimit) {
                        countToFetch = countToFetchLimit;
                    }
                }
                //if (params.getAction().equals(RequestIQConst.READ_SINGLE) || params.getAction().equals("none")) {
                //    useCatch = true;
               // } else if ((countToFetch == countToFetchLimit || countToFetch == -1) && (params.getWhereXpath() == null || params.getWhereXpath().isEmpty())) {
                //    useCatch = true;
                //}
               // if (useCatch) {
                //    xml = domain.getDbCache().read(node, nodeID);
                //}

                if (xml != null && !xml.trim().equals("")) {
                    // do not read
                } else {
                    // domain.getDbCache().delete(node, nodeID, "");
                    try {
                        if (params.getAction().equals(RequestIQConst.READ_SINGLE) || params.getAction().equals("none")) {
                            if (node.getType() == NIDSchemeMDC.COLLECTION) {
                                xml = component.getDBApi().getNodesCol(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), startFrom, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), params.getSelectXML());
                            } else {
                                //xml = component.getDBApi().getNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, params.getWhereXpath(), params.getOrderby(), params.getSelectXML());
                                xml = component.getDBApi().getNode(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(),  "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), params.getSelectXML());
                            }
                           // if (useCatch) {
                               // domain.getDbCache().addToCache_read(node, nodeID, xml);
                           // }
                        } else if (params.getAction().equalsIgnoreCase(RequestIQConst.READ_LIST)) {
                            if (node.getType() == NIDSchemeMDC.COLLECTION) {
                                xml = component.getDBApi().getNodesCol(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), startFrom, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", true, params.getOrderby(), params.getAsc_desc(), params.getSelectXML());
                            } else {
                                xml = component.getDBApi().getNodes(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), startFrom, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), params.getSelectXML());
                            }
                           // if (useCatch) {
                            //    domain.getDbCache().addToCache_read(node, nodeID, xml);
                           // }
                        }
                    } catch (Exception ex) {
                        MRLog.error(iq.toXML(), ex);
                        sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_read, params.getAction() + " " + nodeID, packetID, ex);
                        return false;
                    }
                }
                sendReply(component, reply,  params.getAction(), reply.getID(), params.getRequestAttributes(), xml);
                if (MRLog.isIpvsxmlpacketsLogEnable()) {
                    MRLog.info(packetID + " (" + params.getAction() + ") response " + xml);
                }

            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_read, params.getAction() + " send reply " + nodeID, packetID);
                return false;
            }
            //after send packet processing
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            if (xml != null && !xml.trim().equals("")) {
                if (!component.getPresenceHandler().isUserInPresenceSubList(jid)) {
                    component.getPresenceHandler().presenceSubscriptionRequired(jid);
                }
                domain.getSubs().subscribe(node, jid, "", "", params);
            }
            return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " getPublishedItems " + nodeID);
        }
    }

    public static boolean readQuery(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        String xml = "";
        try {
            try {
                String xquery = params.getQuery();
                if (xquery == null || xquery.trim().equals("")) {
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_invalid_read_query, params.getAction(), packetID);
                    return false;
                }

                xml = component.getDBApi().readQuery(packetID, params.getDomainName(), xquery, node.getIndexNameCol(), node.getIndexName(), node.getCollectionName(), params.getQueryXPathList(), params.getQueryNIDList());
               
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_read_query, params.getAction() + " " + nodeID, packetID);
                return false;
            }
           
            sendReply(component, reply,  params.getAction(), reply.getID(), params.getRequestAttributes(),"<DBResp>"+xml+"</DBResp>" );
            if (MRLog.isIpvsxmlpacketsLogEnable()) {
                MRLog.info(packetID + " (query) response " + xml);
            }
            //after send packet processing
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            
            return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_read_query, params.getAction() + " send Reply " + nodeID, packetID);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " getPublishedItems " + nodeID);
        }
    }

    //////////////////////////////////////static method getIQDiscoItems  //////////////////////////////////////
    public static boolean objectExists(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        return getChildren(component, domainManger, iq, packetID, nodeID, params, domain, node, jid, enterTime);
    }

    public static boolean getChildren(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            List<String> list = new ArrayList<String>();
            //todo: test using plain iq.getChildElement()
            Element queryElement = EngineMDCUtils.discoReplyChild(nodeID);
            reply.setChildElement(queryElement);

            int colNode = 0;
            //if (params.getAction().equals(RequestIQConst.IS_OBJECT_EXISTS)) {
            //    if (domain.getDbCache().nodeExists(node, nodeID) == 1) {
            //        //send(component, reply, packetID);
             //       //sendTimeDiff = System.currentTimeMillis() - enterTime;
            //        sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(),"<DBResp>"+nodeID.trim()+"</DBResp>");
            //        sendTimeDiff = System.currentTimeMillis() - enterTime;                    
            //        return true;
            //    }
            // }
            list = null;
            //if (params.getAction().equals(RequestIQConst.GETCHILDREN)) {
            //    list = domain.getDbCache().discoItems(node, nodeID);
            //}

            //could be anything which will result in element or attribute. only values are read
            String childXpath = params.getChildXpath();
            if (childXpath == null || childXpath.trim().equals("")) {
                childXpath = "@NID";
            }
            if (list == null || list.isEmpty()) {
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    //(packetID, node.getIndexNameCol(), nodeID, node.getCollectionName(), params.getAction(), colNode);
                    list = component.getDBApi().getChildren(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), params.getAction(), params.getWhereXpath(), childXpath, params.getXQueryPrefix());
                } else {
                    //(packetID, node.getIndexName(), nodeID, node.getCollectionName(), params.getAction(), colNode);
                    list = component.getDBApi().getChildren(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), params.getAction(), params.getWhereXpath(), childXpath, params.getXQueryPrefix());
                }
            }
            if (list == null || list.isEmpty()) {
                //reply.setError(PacketError.Condition.item_not_found);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.item_not_found, "item not found for '" + nodeID + "'", packetID);
                return false;
            }
            /*for (Iterator<String> i = list.iterator(); i.hasNext();) {
                String NID = i.next();
                NID = EngineMDCUtils.removeAttribute(NID).trim();
                if (!NID.equalsIgnoreCase(nodeID.trim())) {
                    Element discoEL = EngineMDCUtils.getDiscoItem(NID, NID, "owner", "");
                    queryElement.add(discoEL);
                }
            }
            send(component, reply, packetID); */   
            StringBuffer buf = new StringBuffer();
            buf.append("<DBResp>");
            /*if(list.size()==0){
                System.out.println("nothing to print");
            }*/
            for (Iterator<String> i = list.iterator(); i.hasNext();) {
                String NID = i.next();
                NID = EngineMDCUtils.removeAttribute(NID).trim();
                if (NID.equalsIgnoreCase(nodeID.trim())) {
                    //todo: no need to check. 
                    if(!params.getAction().equals(RequestIQConst.GETCHILDREN)){
                       buf.append(""+ NID.trim()+"\n ");
                    }/*else{
                        //System.out.println("nothing to print");
                    }*/
                }else{
                    buf.append(""+ NID.trim()+"\n ");
                }
            }
            buf.append("</DBResp>");
            //send(component, reply, packetID);
            sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), buf.toString());
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            return true;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_get_children, params.getAction() + " " + nodeID, packetID, ex);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " discoItems " + nodeID);
        }
    }

    public static boolean executeQuery(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, IpvsDomain domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        Element requestElement = iq.getChildElement();
        try {
            String queryxml = "";
            try {
                Element itemstemp = requestElement.element("items");
                queryxml = Dom4jUtils.value(itemstemp, "");
                itemstemp.setText("");
                queryxml = CompUtils.decode(queryxml);
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_failed_to_decode_xml, nodeID, packetID, ex);
                return false;
            }
            if (MRLog.isIpvsxmlpacketsLogEnable()) {
                MRLog.info(packetID + " (executeQuery) request " + queryxml);
            }

            requestElement = requestElement.createCopy();
            reply.setChildElement(requestElement);
            Element items = requestElement.element("items");
            Element item = items.addElement("item");
            item.addAttribute("id", "owner");

            String xml = "";
            StringBuffer bufNodes = new StringBuffer();
            bufNodes.append("<executeQueryResult>");
            boolean bSuc = true;
            try {
                String name = "EXECUTE_XQUERY";
                bSuc = component.getDBApi().execute(name, params.getDomainName(), node.getCollectionName(), queryxml, bufNodes, 1, -1);
                bufNodes.append("<result>" + bSuc + "</result>");
            } catch (Exception ex) {
                MRLog.error(iq.toXML(), ex);
                bufNodes.append("<result>failed</result>");
                bufNodes.append("<error>" + ex.getMessage() + "</error>");
            }
            bufNodes.append("<executeQueryResult>");
            xml = bufNodes.toString();
            if (MRLog.isIpvsxmlpacketsLogEnable()) {
                MRLog.info(packetID + " (executeQuery) response " + xml);
            }

            if (xml != null && !xml.trim().equals("")) {
                xml = CompUtils.encode(xml);
                Element payloadEL = MRPayloadHelper.addPayloadText(xml);
                item.add(payloadEL);
            } else {
                Element payloadEL = MRPayloadHelper.addPayloadText("");
                item.add(payloadEL);
            }
            send(component, reply, packetID);
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_execute_query, params.getAction() + " " + nodeID, packetID, ex);
            return false;
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " executeQuery " + nodeID);
        }
        return true;
    }
    //////////////////////////////////////static method subscribe  //////////////////////////////////////
    public static boolean subscribeObject(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        //Element requestElement = null;//iq.getChildElement().createCopy();
        try {
            //added code to subscribe presence
        	if (!component.getPresenceHandler().isUserInPresenceSubList(jid)) {
                component.getPresenceHandler().presenceSubscriptionRequired(jid);
            }
        	
        	//subscribe
        	subscribe(node, params, domain, nodeID, jid);
            
            //send(component, reply, packetID);
            sendReply(component, reply, "subscribe", reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_subscribe, params.getAction() + " " + nodeID, packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " subscribe nodeid='" + nodeID + "'");
        }
        return true;
    }
    
    private static void subscribe(NIDSchemeMDC node, RequestMDCParams params, DomainMDC domain, String nodeID, String jid) throws Exception{
    	
    	Element subParamsEL = params.getItemData();
		String whereNID = Dom4jUtils.getAttributeValue(subParamsEL, "whereNID", "").trim();
		String subscriptionId = Dom4jUtils.value(subParamsEL, "whereSubscriptionID", "");
		String topic = Dom4jUtils.value(subParamsEL, "topic", "");
		
		if(nodeID.isEmpty()) {
			if(!whereNID.isEmpty()) {
				String[] nidList = whereNID.split(",");
				for (int i = 0; i < nidList.length; i++) {
					NIDSchemeMDC nodeScheme = new NIDSchemeMDC();
					nodeScheme.parse(nidList[i]);
					domain.getSubs().subscribe(nodeScheme, jid, subscriptionId, topic, params);
				}
			}
		}else {
			domain.getSubs().subscribe(node, jid, subscriptionId, topic, params);
		}
    }
    
    //////////////////////////////////static method updateaSubscriptions  ///////////////////////////////////
    public static boolean updateSubscriptions(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        //Element requestElement = null;//iq.getChildElement().createCopy();
        try {
            //added code to subscribe presence
        	if (!component.getPresenceHandler().isUserInPresenceSubList(jid)) {
                component.getPresenceHandler().presenceSubscriptionRequired(jid);
            }
        	Element subParamsEL = params.getItemData();
        	String subscriptionId = Dom4jUtils.value(subParamsEL, "whereSubscriptionID", "");
        	
    		//clean up old subscriptions
    		domain.getSubs().deleteSubscriptionsBasedOnSubscriptionId(subscriptionId);
    		
    		//subscribe
    		subscribe(node, params, domain, nodeID, jid);
            
            //send(component, reply, packetID);
            sendReply(component, reply, "updateSubscriptions", reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_update_subcriptions, params.getAction() + " " + nodeID, packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " updateSubscriptions nodeid='" + nodeID + "'");
        }
        return true;
    }
    

    //////////////////////////////////////static method unsubscribe  //////////////////////////////////////
    public static boolean unsubscribeObject(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        //Element requestElement = iq.getChildElement().createCopy();
        try {
            //reply.setChildElement(requestElement);
        	if (component.getPresenceHandler().isUserInPresenceSubList(jid)) {
        		component.getPresenceHandler().presenceSubscriptionNotRequired(jid);
        	}
        	if (nodeID == null || nodeID.equals("")) {
        		unsubscribe(null, params, domain, jid);
        	}else {
        		unsubscribe(node, params, domain, jid);
        	}
        		
        	
            //send(component, IQ.createResultIQ(iq), packetID);
            sendReply(component, reply, "unsubscribe", reply.getID(), params.getRequestAttributes(), "<DBResp></DBResp>");
            //component.getPresenceHandler().presenceSubscriptionNotRequired(jid);
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_unsubscribe, params.getAction() + " " + nodeID, packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " unsubscribe nodeid='" + nodeID + "' jid='" + jid + "'");
        }
        return true;
    }
    
    private static void unsubscribe(NIDSchemeMDC node, RequestMDCParams params, DomainMDC domain, String jid) throws Exception {
		
		if(node == null) {
			Element subParamsEL = params.getItemData();
			String whereNID = Dom4jUtils.getAttributeValue(subParamsEL, "whereNID", "").trim();
			String whereSubscriptionID = Dom4jUtils.value(subParamsEL, "whereSubscriptionID", "");
			if(!whereSubscriptionID.isEmpty()) {//based on subscriptionIDs
				String[] subIdList = whereSubscriptionID.split(",");
				for (int i = 0; i < subIdList.length; i++) {
					String subId = subIdList[i].trim();
					domain.getSubs().unsubscribe(null, jid, subId, params);
				}
			} else if(!whereNID.isEmpty()) {//based on NID
				NIDSchemeMDC unSubNode = new NIDSchemeMDC();
				unSubNode.parse(whereNID);
				domain.getSubs().unsubscribe(unSubNode, jid, null, params);
			}
		} else {
			domain.getSubs().unsubscribe(node, jid, null, params);
		}
		
    }
    
    //////////////////////////////getSubscriptionList ///////////////////////
    public static boolean getSubscribers(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
    
    	long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        //Element requestElement = null;//iq.getChildElement().createCopy();
        try {
        	Element getSubParamsEL = params.getItemData();
    		String selectFormat = Dom4jUtils.getAttributeValue(getSubParamsEL, "selectFormat", "").trim();
    		
        	String subscriptionXml = domain.getSubs().getSubscribers(node, params, selectFormat);

            //send(component, reply, packetID);
            sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp>" + subscriptionXml + "</DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_get_subscriptions, params.getAction() + " " + nodeID, packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " getSubsList nodeid='" + nodeID + "'");
        }
        return true;
    	
    }
    
    public static boolean getSubscriptions(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        
    	long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        //Element requestElement = null;//iq.getChildElement().createCopy();
        try {
        	List<PubsubSubscription> subs = domain.getSubs().getSubscriptions(node, params);

            //send(component, reply, packetID);
//            sendReply(component, reply, params.getAction(), reply.getID(), params.getRequestAttributes(), "<DBResp>" + subscriptionXml + "</DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_while_get_subscriptions, params.getAction() + " " + nodeID, packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " getSubsList nodeid='" + nodeID + "'");
        }
        return true;
    	
    }
    //////////////////////////////getSubscriptionList ///////////////////////
    

    ////////////////////////////// import export ///////////
    public static boolean exportDB(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            Element EL = params.getItemData();
            Element paramsEL = EL.element("params");
            if(params.getType().equalsIgnoreCase(RequestIQConst.EXPORT_MEDIA_DIR)){
                String mediaStreamDirResourceNID =Dom4jUtils.value(paramsEL ,"mediaStreamDirResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamDirResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DBActionsStatus.addToPendingList(mediaStreamDirResourceNID, mediaStreamDirResourceNID);
                        ExportMediaDir exportMediarDir = new ExportMediaDir();
                        exportMediarDir.setRequestEL(EL);
                        exportMediarDir.execute();
                        Dom4jUtils.addElement(EL, "zipFileName",exportMediarDir.getZipFilePath() );
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamDirResourceNID);
                    }
                }
            }else if(params.getType().equalsIgnoreCase(RequestIQConst.EXPORT_MEDIA_FILE)){
                String mediaStreamFileResourceNID = Dom4jUtils.value(paramsEL ,"mediaStreamFileResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamFileResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DBActionsStatus.addToPendingList(mediaStreamFileResourceNID, mediaStreamFileResourceNID);
                        ExportMediaFile exportMediarFile = new ExportMediaFile();
                        exportMediarFile.setRequestEL(EL);
                        exportMediarFile.execute();
                        Dom4jUtils.addElement(EL, "zipFileName",exportMediarFile.getZipFilePath() );
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamFileResourceNID);
                    }
                }
            }else{
                //todo:
            }
            
            sendReply(component, reply, "exportdb", reply.getID(), params.getRequestAttributes(), "<DBResp>"+EL.asXML()+"</DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_failed, params.getAction() + " " + params.getType(), packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " export type='" + params.getType() + "'");
        }
        return true;
    } 
    
    public static boolean importDB(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            Element EL = params.getItemData();
            Element paramsEL = EL.element("params");
            if(params.getType().equalsIgnoreCase(RequestIQConst.IMPORT_MEDIA_DIR)){
                String mediaStreamDirResourceNID =Dom4jUtils.value(paramsEL ,"mediaStreamDirResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamDirResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DBActionsStatus.addToPendingList(mediaStreamDirResourceNID, mediaStreamDirResourceNID);
                        ImportMediaDir importMediarDir = new ImportMediaDir();
                        importMediarDir.setRequestEL(EL);
                        importMediarDir.execute();
                        Dom4jUtils.addElement(EL, "zipFileName",importMediarDir.getZipFilePath() );
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamDirResourceNID);
                    }
                }                
            }else if(params.getType().equalsIgnoreCase(RequestIQConst.IMPORT_MEDIA_FILE)){
                String mediaStreamFileResourceNID =Dom4jUtils.value(paramsEL ,"mediaStreamFileResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamFileResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DBActionsStatus.addToPendingList(mediaStreamFileResourceNID, mediaStreamFileResourceNID);
                        ImportMediaFile importMediarFile = new ImportMediaFile();
                        importMediarFile.setRequestEL(EL);
                        importMediarFile.execute();
                        Dom4jUtils.addElement(EL, "zipFileName",importMediarFile.getZipFilePath() );
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamFileResourceNID);
                    }
                }       
            } else{
                //todo:
            }
            sendReply(component, reply, "importdb", reply.getID(), params.getRequestAttributes(), "<DBResp>"+EL.asXML()+"</DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_failed, params.getAction() + " " + params.getType(), packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " import type='" + params.getType() + "'");
        }
        return true;
    }    
    
    public static boolean deleteDB(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) {
        long sendTimeDiff = 0;
        IQ reply = IQ.createResultIQ(iq);
        try {
            Element EL = params.getItemData();
            Element paramsEL = EL.element("params");            
            if(params.getType().equalsIgnoreCase(RequestIQConst.DELETE_MEDIA_DIR)){
                String mediaStreamDirResourceNID =Dom4jUtils.value(EL ,"mediaStreamDirResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamDirResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DeleteMediaDir deleteMediarDir = new DeleteMediaDir();
                        deleteMediarDir.setRequestEL(EL);
                        deleteMediarDir.execute();
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamDirResourceNID);
                    }
                }                
            }else if(params.getType().equalsIgnoreCase(RequestIQConst.DELETE_MEDIA_FILE)) {
                String mediaStreamFileResourceNID = Dom4jUtils.value(paramsEL ,"mediaStreamFileResourceNID" ,"");
                if(DBActionsStatus.isFromPendingList(mediaStreamFileResourceNID)){
                    sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_already_running, params.getAction() + " " + params.getType(), packetID);
                    sendTimeDiff = System.currentTimeMillis() - enterTime;
                }else{
                    try{
                        DBActionsStatus.addToPendingList(mediaStreamFileResourceNID, mediaStreamFileResourceNID);
                        DeleteMediaFile deleteMediarFile = new DeleteMediaFile();
                        deleteMediarFile.setRequestEL(EL);
                        deleteMediarFile.execute();
                    }finally{
                        DBActionsStatus.remveFromPendingList(mediaStreamFileResourceNID);
                    }
                }       
            } else {
                //todo:
            }            
            sendReply(component, reply, "deletedb", reply.getID(), params.getRequestAttributes(), "<DBResp>"+EL.asXML()+"</DBResp>");
            sendTimeDiff = System.currentTimeMillis() - enterTime;
        } catch (Exception ex) {
            MRLog.error(iq.toXML(), ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_error_db_import_export_failed, params.getAction() + " " + params.getType(), packetID, ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable())
                MRLog.debug("ID: " + packetID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " delete type='" + params.getType() + "'");
        }
        return true;
    }  
    ///////////////////
    
    private static Document getDocumentList(ComponentEngine component, DomainManager domainManger, IQ iq, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        Document doc = null;
        int countToFetch = domain.getCountToFetch_ByListName(node.getCollectionId());
        boolean isXqueryPrefix = (params.getXQueryPrefix() !=null && !params.getXQueryPrefix().trim().equals(""));
        if(isXqueryPrefix ) {
            countToFetch = 2;
        }
        StringBuffer selectXml = new StringBuffer();
        selectXml.append("<aa><xpath>@*</xpath><xpath>name()</xpath></aa>");
        String xml2 = "";
        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
            xml2 = component.getDBApi().getDocumentList(packetID, params.getDomainName(), node.getType(), node.getIndexName(), nodeID, node.getCollectionName(), 1, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), selectXml.toString());
        } else {
            xml2 = component.getDBApi().getDocumentList(packetID, params.getDomainName(), node.getType(), node.getIndexNameCol(), nodeID, node.getCollectionName(), 1, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), selectXml.toString());
        }
        doc = Dom4jUtils.stringToDocument(xml2);
        return doc;
    }

    private static List<String> renameEL(Document doc, String action) {
        List<String> returnlist =   new ArrayList<String>(); 
        try {
            if (doc != null) {
                List<?> list = Dom4jUtils.getList(doc.getRootElement(), "aa");
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    Element el = (Element) iterator.next();
                    String value = el.getTextTrim();
                    el.setName(value);
                    el.setText(action);
                    String NID = Dom4jUtils.getAttributeValue(el, "NID", "");
                    returnlist.add(NID);
                }
                Element sumEL = Dom4jUtils.element(doc.getRootElement(), "GetListObjectResponseData");
                if (sumEL != null) {
                    sumEL.detach();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return returnlist;
     }
    ////////////////////////////////////// static method send  //////////////////////////////////////
   /* public static void sendCreateReply(ComponentEngine component, IQ reply, String packetID, String nodeID) throws Exception {
        Element elem = reply.setChildElement("pubsub", "http://jabber.org/protocol/pubsub");
        elem.addElement("create").addAttribute("node", nodeID);
        component.getXmppSession().sendPacket(reply);
    }

    public static void sendReplaceReply(ComponentEngine component, IQ reply, String packetID, String nodeID, String replaced) throws Exception {
        Element elem = reply.setChildElement("pubsub", "http://jabber.org/protocol/pubsub");
        Element delEL = elem.addElement("create");
        delEL.setText(CompUtils.encode(replaced));
        delEL.addAttribute("node", nodeID);
        component.getXmppSession().sendPacket(reply);
    }

    public static void sendDeleteReply(ComponentEngine component, IQ reply, String packetID, String nodeID, String deleted) throws Exception {
        Element elem = reply.setChildElement("pubsub", "http://jabber.org/protocol/pubsub");
        Element delEL = elem.addElement("delete");
        delEL.setText(CompUtils.encode(deleted));
        delEL.addAttribute("node", nodeID);
        component.getXmppSession().sendPacket(reply);
    }*/
    public static void sendReadReply2(ComponentEngine component, IQ reply,  String action,  String packetID, String nodeID, String xml) throws Exception {
        Element elem = reply.setChildElement("pubsub", "http://jabber.org/protocol/pubsub");
        Element items = elem.addElement("items");
        items.addAttribute("node", nodeID);
        Element item = items.addElement("item");
        item.addAttribute("id", "owner");
        Element payloadEL = MRPayloadHelper.addPayloadText(xml);
        item.add(payloadEL);
        //System.out.println(" reply " + reply.toXML() );
        component.getXmppSession().sendPacket(reply);
        
        /*
            <pubsub xmlns="http://jabber.org/protocol/pubsub">
            <items node="default.mediastreamfileresourcelist.msfrl_ee
            be67e1-891c-494f-b4dc-9951f354e42d" 
            subid="default.mediastreamfileresourcelist.msfrl_eebe67e
            1-891c-494f-b4dc-9951f354e42d" action="readNode" 
            subscribeJID="srcuser1@localhost/srcpc1" serviceDomain="default">
            <item id="owner">        
         */
    }
    public static void sendReply(ComponentEngine component, IQ reply, String action, String packetID, String requestAttributes, String xml) throws Exception {
        Element child=null;
        StringBuffer buf = new StringBuffer();
        try{
            buf.append("<response xmlns=\""+RequestIQConst.MEDIAROOM_NAME_SPACE+"\"><"+action+" id='"+packetID+"' "+requestAttributes + " "+getServicever() +">");
            //buf.append(CompUtils.encode(xml));
            buf.append(xml);
            buf.append("</"+action+"></response>");
            child = Dom4jUtils.stringToElement(buf.toString());
        }catch(Exception ex){
            MRLog.error(buf.toString(),ex);
            throw ex;
        }
        reply.setChildElement(child);
        component.getXmppSession().sendPacket(reply);
    }

    ////////////////////////////////////// static method send  //////////////////////////////////////
    public static void send(ComponentEngine component, Packet reply, String packetID) throws Exception {
        component.getXmppSession().sendPacket(reply);
    }

    ////////////////////////////////////// static method sendErrorPacket  //////////////////////////////////////
    
    //////////////////////////////////////static method sendErrorPacket  //////////////////////////////////////
    public static void sendErrorFromReply(ComponentEngine component, IQ reply, IPVSCondition error, String errorMsg, String packetID, Exception ex) {
        if (ex == null) {
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_db_connection_failed,getServicever() +":"+ errorMsg + " . unknown error.", packetID);
        } else if (ex instanceof DBNotConnectedException) {
            MRLog.error(ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_db_connection_failed, getServicever() +":"+   errorMsg + " " + ex.getMessage(), packetID);
        } else if (ex instanceof DBConnectTimedoutException) {
            MRLog.error(ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_db_connection_timed_out, getServicever() +":"+ errorMsg + " " + ex.getMessage(), packetID);
        } else if (ex instanceof DBExecutionException) {
            MRLog.error(ex);
            sendErrorFromReply(component, reply, IpvsServPacketError.IPVSCondition.ipvs_db_execution_failed, getServicever() +":"+ errorMsg + " " + ex.getMessage(), packetID);
        } else if (ex instanceof Exception) {
            sendErrorFromReply(component, reply, error, getServicever() +":"+ errorMsg + " " + ex.getMessage(), packetID);
        }
    }

    public static void sendErrorFromReply(ComponentEngine component, IQ reply, IPVSCondition error, String errorMsg, String packetID) {
        reply.setError(new IpvsServPacketError(error));
        if (errorMsg != null) {
            reply.getError().setText(errorMsg);
        }
        try {
            component.getXmppSession().sendPacket(reply);
        } catch (Exception ex) {
            MRLog.error(reply.toXML(), ex);
            ex.printStackTrace();
        }
    }

    //////////////////////////////////////static methods    //////////////////////////////////////
    public static String getActionName(ComponentEngine component, Element childElement, String name) {
        String actionName = null;
        if (childElement != null) {
            String namespace = childElement.getNamespaceURI();
            if (namespace != null) {
                if ("http://jabber.org/protocol/disco#info".equals(namespace)) {
                    return "disco#info";
                } else if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                    return "disco#items";
                } else if ("http://jabber.org/protocol/pubsub".equals(namespace) || "http://jabber.org/protocol/pubsub#owner".equals(namespace)) {
                    if (childElement.element("subscribe") != null) {
                        return "subscribe";
                    }
                    if (childElement.element("updateSubscriptions") != null) {
                        return "updateSubscriptions";
                    }
                    if (childElement.element("unsubscribe") != null) {
                        return "unsubscribe";
                    }
                    if (childElement.element("create") != null) {
                        return "create";
                    }
                    if (childElement.element("items") != null) {
                        return "items";
                    }
                    if (childElement.element("delete") != null) {
                        return "delete";
                    }
                    if (childElement.element("getSubscribers") != null) {
                        return "getSubscribers";
                    }
                    if (childElement.element("getSubscriptions") != null) {
                        return "getSubscriptions";
                    }
                } else if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                    return "disco#items";
                } else if ("http://jabber.org/protocol/pubsub".equals(namespace) || "http://jabber.org/protocol/pubsub#owner".equals(namespace)) {
                    if (childElement.element("subscribe") != null) {
                        return "subscribe";
                    }
                    if (childElement.element("updateSubscriptions") != null) {
                        return "updateSubscriptions";
                    }
                    if (childElement.element("unsubscribe") != null) {
                        return "unsubscribe";
                    }
                    if (childElement.element("create") != null) {
                        return "create";
                    }
                    if (childElement.element("items") != null) {
                        return "items";
                    }
                    if (childElement.element("delete") != null) {
                        return "delete";
                    }
                    if (childElement.element("getSubscribers") != null) {
                        return "getSubscribers";
                    }
                    if (childElement.element("getSubscriptions") != null) {
                        return "getSubscriptions";
                    }
                } else if (RequestIQConst.MEDIAROOM_NAME_SPACE.equals(namespace)) {
                    if (childElement.element(RequestIQConst.CREATE_DOMAIN) != null) {
                        return RequestIQConst.CREATE_DOMAIN;
                    }
                    if (childElement.element(RequestIQConst.DELETE_DOMAIN) != null) {
                        return RequestIQConst.DELETE_DOMAIN;
                    }
                    if (childElement.element(RequestIQConst.ADD) != null) {
                        return RequestIQConst.ADD;
                    }
                    if (childElement.element(RequestIQConst.REPLACE) != null) {
                        return RequestIQConst.REPLACE;
                    }
                    if (childElement.element(RequestIQConst.DELETE) != null) {
                        return RequestIQConst.DELETE;
                    }
                    if (childElement.element(RequestIQConst.READ_SINGLE) != null) {
                        return RequestIQConst.READ_SINGLE;
                    }
                    if (childElement.element(RequestIQConst.READ_LIST) != null) {
                        return RequestIQConst.READ_LIST;
                    }
                    if (childElement.element(RequestIQConst.GETCHILDREN) != null) {
                        return RequestIQConst.GETCHILDREN;
                    }
                    if (childElement.element(RequestIQConst.IS_OBJECT_EXISTS) != null) {
                        return RequestIQConst.IS_OBJECT_EXISTS;
                    }

                    if (childElement.element(RequestIQConst.READ_QUERY) != null) {
                        return RequestIQConst.READ_QUERY;
                    }
                    if (childElement.element(RequestIQConst.SUBSCRIBE) != null) {
                        return RequestIQConst.SUBSCRIBE;
                    }
                    if (childElement.element(RequestIQConst.UPDATESUBSCRIPTIONS) != null) {
                        return RequestIQConst.UPDATESUBSCRIPTIONS;
                    }
                    if (childElement.element(RequestIQConst.UNSUBSCRIBE) != null) {
                        return RequestIQConst.UNSUBSCRIBE;
                    }
                    if (childElement.element(RequestIQConst.EXPORT_DB) != null) {
                        return RequestIQConst.EXPORT_DB;
                    }
                    if (childElement.element(RequestIQConst.IMPORT_DB) != null) {
                        return RequestIQConst.IMPORT_DB;
                    }
                    if (childElement.element(RequestIQConst.DELETE_DB) != null) {
                        return RequestIQConst.DELETE_DB;
                    }
                    if (childElement.element(RequestIQConst.GET_SUBSCRIBERS) != null) {
                        return RequestIQConst.GET_SUBSCRIBERS;
                    }
                    if (childElement.element(RequestIQConst.GET_SUBSCRIPTIONS) != null) {
                        return RequestIQConst.GET_SUBSCRIPTIONS;
                    }
                }
            }
        }
        return actionName;
    }

    public static Element stringToElement(String xml) {
        try {
            if (xml == null || xml.equals("")) {
                return null;
            }
            Document doc = Dom4jUtils.stringToDocument(xml);
            return doc.getRootElement();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
     
}
