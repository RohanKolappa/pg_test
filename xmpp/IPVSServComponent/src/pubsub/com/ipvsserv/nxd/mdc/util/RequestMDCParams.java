package com.ipvsserv.nxd.mdc.util;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.QName;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.EngineMDCUtils;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class RequestMDCParams {
    private String action = "";

    private String domainName = null;
    private String subscribeJID = "";

    private String objectType = "";
    private String listName = "";

    private String nodeID = "";
    private String parentNode = "";

    private String childXPath = "";
    private List<String> whereXPath = null;
    private Element itemData = null;
    private String textData = null;

    private List<String> orderbyXPath = null;
    private boolean asc_desc = true;

    private int startFrom = -1;
    private int countToFetch = -2;
    private String selectXML = "";
    private String query = "";
    private String type = "";
    private String xQueryPrefix="";
    private String selectFormat = "";
    
    public String getXQueryPrefix() {
        return xQueryPrefix;
    }

    public void setXQueryPrefix(String queryPrefix) {
        xQueryPrefix = queryPrefix;
    }

    private Map<String, String> queryXPathList = null;

    public boolean getAsc_desc() {
        return asc_desc;
    }

    public void setAsc_desc(boolean asc_desc) {
        this.asc_desc = asc_desc;
    }

    public Map<String, String> getQueryXPathList() {
        return queryXPathList;
    }

    public void setQueryXPathList(Map<String, String> queryXPathList) {
        this.queryXPathList = queryXPathList;
    }

    public Map<String, String> getQueryNIDList() {
        return queryNIDList;
    }

    public void setQueryNIDList(Map<String, String> queryNIDList) {
        this.queryNIDList = queryNIDList;
    }

    private Map<String, String> queryNIDList = null;

    private String DBCallTimeOut;

    public RequestMDCParams(String serviceDomina) {
        domainName = serviceDomina;
    }

    public RequestMDCParams(Element requestElement, String namespace) throws Exception {

        if (RequestIQConst.MEDIAROOM_NAME_SPACE.equals(namespace)) {
            String domainName2 = Dom4jUtils.getAttributeValue(requestElement, "domain", "").trim();
            if (!domainName2.equals("")) {
                domainName = domainName2;
            }

            subscribeJID = Dom4jUtils.getAttributeValue(requestElement, "subscribeJID", "").trim();
            action = requestElement.getName();

            ////common
            nodeID = Dom4jUtils.getAttributeValue(requestElement, "nodeID", "").trim();
            childXPath = Dom4jUtils.getAttributeValue(requestElement, "childXPath", "").trim();
            if(!childXPath.equals("")){
                try {
                    childXPath = CompUtils.decode(childXPath);
                } catch (UnsupportedEncodingException ex) {
                    MRLog.error("error childXPath decode  " + childXPath, ex);
                } 
            } else {
                childXPath = Dom4jUtils.value(requestElement, "childXPath", "").trim();
            }
            //childXPath = Dom4jUtils.value(requestElement, "childXPath", "").trim();
            
            objectType = Dom4jUtils.getAttributeValue(requestElement, "objectType", "").trim();
            listName = Dom4jUtils.getAttributeValue(requestElement, "listName", "").trim();
            DBCallTimeOut = Dom4jUtils.getAttributeValue(requestElement, "DBCallTimeOut", "").trim();
            type = Dom4jUtils.getAttributeValue(requestElement, "type", "").trim();
            whereXPath = Dom4jUtils.values(requestElement, "whereXPath");
            xQueryPrefix = Dom4jUtils.value(requestElement, "xQueryPrefix","").trim();
            //MRLog.debug("xQueryPrefix " + xQueryPrefix);            
            if (action.equals(RequestIQConst.READ_QUERY)) {
                query = Dom4jUtils.value(requestElement, "query", "").trim();
                queryXPathList = new Hashtable<String, String>();
                queryNIDList = new Hashtable<String, String>();

                List<?> list = requestElement.elements("queryXPath");
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    Element el = (Element) iterator.next();
                    String objecttype = Dom4jUtils.getAttributeValue(el, "objecttype", "").trim();
                    String val = el.getText();
                    if (!objecttype.equals("")) {
                        queryXPathList.put(objecttype, val);
                    }
                }
                list = requestElement.elements("queryNID");
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    Element el = (Element) iterator.next();
                    String objecttype = Dom4jUtils.getAttributeValue(el, "objecttype", "").trim();
                    String val = el.getText();
                    if (!objecttype.equals("")) {
                        queryNIDList.put(objecttype, val);
                    }
                }
            }

            if (action.equals(RequestIQConst.EXPORT_DB) || action.equals(RequestIQConst.IMPORT_DB) || action.equals(RequestIQConst.DELETE_DB)
            	|| action.equals(RequestIQConst.SUBSCRIBE) || action.equals(RequestIQConst.UNSUBSCRIBE) || action.equals(RequestIQConst.UPDATESUBSCRIPTIONS) 
            	|| action.equals(RequestIQConst.GET_SUBSCRIPTIONS) || action.equals(RequestIQConst.GET_SUBSCRIBERS)) {
            	
                itemData = Dom4jUtils.getFirstChild(requestElement);
            }else if (action.equals(RequestIQConst.READ_LIST) || action.equals(RequestIQConst.READ_SINGLE)) {
                startFrom = CompUtils.intValue(Dom4jUtils.value(requestElement, "startFrom", "0").trim(), 0);
                countToFetch = CompUtils.intValue(Dom4jUtils.value(requestElement, "countToFetch", "-1").trim(), -1);
                orderbyXPath = Dom4jUtils.values(requestElement, "sortByXPath");
                String asc_desc2 = Dom4jUtils.value(requestElement, "sortOrder", "").trim();
                if (asc_desc2.equalsIgnoreCase(RequestIQConst.SORT_DESCENDING)) {
                    asc_desc = false;
                } else {
                    asc_desc = true;
                }
                try {
                    selectXML = CompUtils.decode(Dom4jUtils.value(requestElement, "selectXML", "").trim());
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            } else {
                try {
                    //itemData = CompUtils.decode(Dom4jUtils.value(requestElement, "data", "").trim());
                    Element dataParent = Dom4jUtils.element(requestElement, "data");
                    if (dataParent != null) {
                        itemData = Dom4jUtils.getFirstChild(dataParent);
                        if (itemData != null) {
                            itemData.detach();
                            Dom4jUtils.removeNameSpace(itemData);
                            //System.out.println("itemData " + itemData.asXML());
                        } else {
                            textData = dataParent.getText();
                            try {
                                if(textData !=null){
                                    textData = CompUtils.decode(textData.trim());
                                    if(textData.startsWith("<") && textData.endsWith(">")){
                                        itemData = Dom4jUtils.stringToElement(textData);
                                        itemData.detach();
                                    }
                                }
                            } catch (Exception ex) {
                                //ex.printStackTrace();
                            }
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            //backward compatibility  should be tehre only for get
            String domainName2 = Dom4jUtils.getAttributeValue(requestElement, "serviceDomain", "").trim();
            subscribeJID = Dom4jUtils.getAttributeValue(requestElement, "subscribeJID", "").trim();
            action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();

            if (!domainName2.equals("")) {
                domainName = domainName2;
            }
            String elementName = requestElement.getName();
            action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
            nodeID = Dom4jUtils.getAttributeValue(requestElement, "node", "").trim();
            //delete, disco#items, subscribe, unsubscribe
            if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                if (action.equals(RequestIQConst.GETCHILDREN)) {
                    action = RequestIQConst.GETCHILDREN;
                } else if (action.equals(RequestIQConst.OBJECT_EXISTS)) {
                    action = RequestIQConst.IS_OBJECT_EXISTS;
                }
            } else if (elementName.equals("items")) {
                if (action.equals(RequestIQConst.READ_CHILD_NODES) || action.equals(RequestIQConst.READ_CHILDREN)) {
                    action = RequestIQConst.READ_LIST;
                } else if (action.equals(RequestIQConst.READ_NODE)) {
                    action = RequestIQConst.READ_SINGLE;
                }
                try {
                    String queryxml = Dom4jUtils.value(requestElement, "");
                    requestElement.setText("");
                    queryxml = CompUtils.decode(queryxml);
                    Element queryElementIPVS = null;
                    if (queryxml != null && queryxml.trim().length() > 0) {
                        queryElementIPVS = Dom4jUtils.stringToElement(queryxml);
                    }
                    if (queryElementIPVS != null) {
                        startFrom = CompUtils.intValue(Dom4jUtils.valueXpath(queryElementIPVS, RequestIQConst.START_FROM, 0 + ""), 0);
                        countToFetch = CompUtils.intValue(Dom4jUtils.valueXpath(queryElementIPVS, RequestIQConst.COUNT_TO_FETCH, "-1"), -1);
                        whereXPath = EngineMDCUtils.getXpathArray(queryElementIPVS);
                    }
                } catch (Exception ex) {
                    throw new Exception("Invalid request" + ex.getMessage());
                }
            } else if (elementName.equals("create")) {
                if (action.equals(RequestIQConst.CREATE_NODE) || action.equals(RequestIQConst.CREATE_CHILDNODES)) {
                    action = RequestIQConst.ADD;
                } else if (action.equals(RequestIQConst.UPDATE_NODE) || action.equals(RequestIQConst.UPDATE_CHILDNODES)) {
                    action = RequestIQConst.REPLACE;
                } else if (action.equals(RequestIQConst.CREATE_ROOT)) {
                    action = RequestIQConst.CREATE_DOMAIN;
                }
                try {
                    String xml = requestElement.getText();
                    try {
                        requestElement.setText("");
                        itemData = Dom4jUtils.stringToElement(CompUtils.decode(xml));
                        //itemData.detach();

                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                } catch (Exception ex) {
                    throw new Exception("Invalid request" + ex.getMessage());
                }
                // read parent NID
                Element configureElement = requestElement.getParent().element("configure");
                if (configureElement != null) {
                    Document doc = Dom4jUtils.getDocument();
                    Dom4jUtils.addElement(doc, configureElement.createCopy());
                    if (configureElement != null) {
                        Element formElement = configureElement.element(QName.get("x", "jabber:x:data"));
                        parentNode = parseParentNID(formElement);
                    }
                }
            } else if (elementName.equals("subscribe")) {
                action = RequestIQConst.SUBSCRIBE;
            }else if (elementName.equals("updateSubscriptions")) {
                action = RequestIQConst.UPDATESUBSCRIPTIONS;
            } else if (elementName.equals("unsubscribe")) {
                action = RequestIQConst.UNSUBSCRIBE;
            } else if (elementName.equals("delete")) {
                if (action.equals(RequestIQConst.DELETE_NODE) || action.equals(RequestIQConst.DELETE_CHILD_NODES)) {
                    action = RequestIQConst.DELETE;
                } else if (action.equals(RequestIQConst.DELETE_ROOT)) {
                    action = RequestIQConst.DELETE_DOMAIN;
                }
            }else if (elementName.equals("getSubscribers")) {
                action = RequestIQConst.GET_SUBSCRIBERS;
            }else if (elementName.equals("getSubscriptions")) {
                action = RequestIQConst.GET_SUBSCRIPTIONS;
            }

            /*//if ("http://jabber.org/protocol/disco#info".equals(namespace)) {
            if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                //return "disco#items";
                
            } else if ("http://jabber.org/protocol/pubsub".equals(namespace) || "http://jabber.org/protocol/pubsub#owner".equals(namespace)) {
                if (childElement.element("subscribe") != null) {
                }
                if (childElement.element("unsubscribe") != null) {
                }
                if (childElement.element("create") != null) {
                }
                if (childElement.element("items") != null) {
                }
                if (childElement.element("delete") != null) {
                }
            } */

        }

    }

    public String parseParentNID(Element formElement) {
        String val = null;
        if (formElement != null) {
            List<?> list = Dom4jUtils.getList(formElement, "field");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String var = Dom4jUtils.getAttributeValue(el, "var", "");
                String val2 = Dom4jUtils.value(el, "value", "");
                if (var.equalsIgnoreCase("pubsub#collection")) {
                    val = val2;
                    break;
                }
            }

        }
        return val;
    }

    public String getDomainName() {
        return domainName;
    }

    public String getRequestAttributes() {
        StringBuffer buf = new StringBuffer();
        buf.append(" subscribeJID='" + subscribeJID + "'");
        buf.append(" nodeID='" + nodeID + "'");
        try {
            buf.append(" childXPath='" + CompUtils.encode(childXPath) + "'");
        } catch (UnsupportedEncodingException ex) {
            MRLog.error("error childXPath encode  " + childXPath, ex);
        }
        buf.append(" objectType='" + objectType + "'");
        buf.append(" listName='" + listName + "'");
        buf.append(" DBCallTimeOut='" + DBCallTimeOut + "'");
        buf.append(" type='" + DBCallTimeOut + "'");
        return buf.toString();
    }

    public String getNodeID() {
        return nodeID;
    }

    public String getParentNode() {
        return parentNode;
    }

    public String getChildXpath() {
        return childXPath;
    }

    public List<String> getWhereXpath() {
        return whereXPath;
    }

    public Element getItemData() {
        return itemData;
    }

    public String getItemDataStr() {
        if (itemData != null) {
            return itemData.asXML();
        }
        return textData;
    }

    public String getTextData() {
        return textData;
    }

    public void setTextData(String textData) {
        this.textData = textData;
    }

    public List<String> getOrderby() {
        return orderbyXPath;
    }

    public int getStartFrom() {
        return startFrom;
    }

    public int getCountToFetch() {
        return countToFetch;
    }

    public String getSelectXML() {
        return selectXML;
    }

    public String getQuery() {
        return query;
    }

    public String getObjectType() {
        return objectType;
    }

    public String getListName() {
        return listName;
    }

    public void setDomainName(String domainName) {
        this.domainName = domainName;
    }

    public void setSubscribeJID(String subscribeJID) {
        this.subscribeJID = subscribeJID;
    }

    public String getAction() {
        return action;
    }

    public void setAction(String packetAction) {
        this.action = packetAction;
    }

    public String getSubscribeJID() {
        if (subscribeJID == null) {
            return "";
        }
        return subscribeJID;
    }

    public void setObjectType(String objectType) {
        this.objectType = objectType;
    }

    public void setListName(String listName) {
        this.listName = listName;
    }

    public void setNodeID(String nodeID) {
        this.nodeID = nodeID;
    }

    public void setParentNode(String parentNode) {
        this.parentNode = parentNode;
    }

    public void setChildXpath(String childXpath) {
        this.childXPath = childXpath;
    }

    public void setWhereXpath(List<String> whereXpath) {
        this.whereXPath = whereXpath;
    }

    public void addWhereXpath(String pwhereXpath) {
        if (whereXPath == null) {
            whereXPath = new ArrayList<String>();
        }
        this.whereXPath.add(pwhereXpath);
    }

    public void setItemData(Element itemData) {
        this.itemData = itemData;
    }

    public void setOrderby(List<String> orderby) {
        this.orderbyXPath = orderby;
    }

    public void setStartFrom(int startFrom) {
        this.startFrom = startFrom;
    }

    public void setCountToFetch(int countToFetch) {
        this.countToFetch = countToFetch;
    }

    public void setSelectXML(String selectXML) {
        this.selectXML = selectXML;
    }

    public void setQuery(String query) {
        this.query = query;
    }

    public String getType() {
        return type;
    }


    // ///////////todo move it to some other static context////////////
    public String toXML() {
        StringBuffer buf = new StringBuffer();
        buf.append("<request xmlns='" + RequestIQConst.MEDIAROOM_NAME_SPACE + "'>");
        buf.append("<" + getAction() + "   domain='" + getDomainName() + "' ");
        if (getNodeID() != null) {
            buf.append(" node='" + getNodeID() + "' ");
        }
        if (getSubscribeJID() != null) {
            buf.append(" subscribeJID='" + getSubscribeJID() + "' ");
        }
        if (getObjectType() != null) {
            buf.append(" objecType='" + getObjectType() + "' ");
        }
        if (getListName() != null) {
            buf.append(" listName='" + getListName() + "' ");
        }
        if (getChildXpath() != null) {
            //buf.append(" childXPath='" + getChildXpath() + "' ");
            try {
                buf.append(" childXPath='" + CompUtils.encode(getChildXpath()) + "' ");
            } catch (UnsupportedEncodingException ex) {
                MRLog.error("error childXPath encode  " + childXPath, ex);
            }             
        }
        buf.append(" >");

        if (getAction().equals(RequestIQConst.READ_QUERY)) {
            if (getQuery() != null) {
                buf.append("<query><![CDATA[ ").append(getQuery()).append(" ]]></query>");
            }
        } else {
            if (getWhereXpath() != null) {
                for (Iterator<String> iterator = whereXPath.iterator(); iterator.hasNext();) {
                    String str = iterator.next();
                    if (str.length() > 0)
                        buf.append("<whereXPath><![CDATA[ ").append(str).append(" ]]></whereXPath>");
                }
            }
            if (getAction().equals(RequestIQConst.DELETE)) {
                //do nothing
            } else if (getAction().equals(RequestIQConst.REPLACE) || getAction().equals(RequestIQConst.ADD)
            		|| getAction().equals(RequestIQConst.SUBSCRIBE) || getAction().equals(RequestIQConst.UNSUBSCRIBE)
            		|| getAction().equals(RequestIQConst.UPDATESUBSCRIPTIONS) || getAction().equals(RequestIQConst.GET_SUBSCRIBERS)
            		|| getAction().equals(RequestIQConst.GET_SUBSCRIPTIONS)) {
                if (getItemData() != null) {
                    try {
                        buf.append("<data>").append(getItemData().asXML()).append("</data>");
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            } else if (getAction().equals(RequestIQConst.READ_LIST) || getAction().equals(RequestIQConst.READ_SINGLE)) {
                // only get 
                if (getStartFrom() > -1) {
                    buf.append("<startFrom>").append(getStartFrom()).append("</startFrom>");
                }
                if (getCountToFetch() > -2) {
                    buf.append("<countToFetch>").append(getCountToFetch()).append("</countToFetch>");
                }
                if (getSelectXML() != null) {
                    //buf.append("<selectXML><![CDATA[ ").append( getSelectXML()).append("]]</selectXML>");
                    try {
                        buf.append("<selectXML>").append(CompUtils.encode(getSelectXML())).append("</selectXML>");
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }
                }
                if (getOrderby() != null) {
                    for (Iterator<String> iterator = orderbyXPath.iterator(); iterator.hasNext();) {
                        String str = iterator.next();
                        if (str.length() > 0) {
                            buf.append("<orderbyXPath><![CDATA[ ").append(str).append(" ]]></orderbyXPath>");
                        }
                    }
                }
            }
        }
        buf.append("</" + getAction() + "></request>");
        return buf.toString();
    }
}

/*
 public RequestMDCParams(Element requestElement, String namespace ) throws Exception {
        String actionName = null;
        Element childElement=null;
        if (requestElement != null) {
            //String namespace = requestElement.getNamespaceURI();
            if (namespace != null) {
                if ("http://jabber.org/protocol/disco#info".equals(namespace)) {
                } else if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                    action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
                    if (action.equals(RequestIQConst.GET_CHILDREN) ) {
                        action=RequestIQConst.GET_CHILDREN;
                    } else if (action.equals(RequestIQConst.OBJECT_EXISTS) ) {
                        action=RequestIQConst.EXISTS_OBJECT;
                    }                     
                } else if ("http://jabber.org/protocol/pubsub".equals(namespace) || "http://jabber.org/protocol/pubsub#owner".equals(namespace)) {
                    if (requestElement.element("subscribe") != null) {
                        requestElement = requestElement.element("subscribe");   
                        action = RequestIQConst.SUBSCRIBE_OBJECT;
                    }
                    if (requestElement.element("unsubscribe") != null) {
                        requestElement = requestElement.element("unsubscribe");   
                        action = RequestIQConst.UNSUBSCRIBE_OBJECT;
                    }
                    if (requestElement.element("create") != null) {
                        requestElement = requestElement.element("create");   
                        action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
                        if (action.equals(RequestIQConst.READ_CHILD_NODES) || action.equals(RequestIQConst.READ_CHILDREN) ) {
                            action=RequestIQConst.READ_LIST;
                        } else if (action.equals(RequestIQConst.READ_NODE) ) {
                            action=RequestIQConst.READ_SINGLE;
                        }                         
                    }
                    if (requestElement.element("items") != null) {
                        requestElement = requestElement.element("items");   
                        action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
                    }
                    if (requestElement.element("delete") != null) {
                        requestElement = requestElement.element("delete");   
                        action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
                    }
                    domainName  = Dom4jUtils.getAttributeValue(requestElement, "serviceDomain", "").trim();
                    subscribeJID = Dom4jUtils.getAttributeValue(requestElement, "subscribeJID", "").trim();

                
                } else if( RequestIQConst.MEDIAROOM_NAME_SPACE.equals(namespace)) {
                    if (requestElement.element(RequestIQConst.CREATE_DOMAIN) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }                    
                    if (requestElement.element(RequestIQConst.DELETE_DOMAIN) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }                     
                    if (requestElement.element(RequestIQConst.ADD_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.ADD_CHILD) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.REPLACE_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.REPLACE_CHILD) != null  ) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.DELETE_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.DELETE_CHILD) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.READ_SINGLE) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.READ_LIST) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.GETCHILDREN) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.EXISTS_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.EXISTS_CHILD) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.QUERY) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.SUBSCRIBE_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }
                    if (requestElement.element(RequestIQConst.UNSUBSCRIBE_OBJECT) != null) {
                        requestElement = requestElement.element(RequestIQConst.CREATE_DOMAIN);    
                    }                    
                }
            }      
        
        
        if(RequestIQConst.MEDIAROOM_NAME_SPACE.equals(namespace)) {
            String domainName2 = Dom4jUtils.getAttributeValue(requestElement, "domain", "").trim();
            if (!domainName2.equals("")) {
                domainName = domainName2;
            }
            
            subscribeJID = Dom4jUtils.getAttributeValue(requestElement, "subscribeJID", "").trim();
            action = requestElement.getName();
            nodeID = Dom4jUtils.getAttributeValue(requestElement, "nodeID", "").trim();
            childXpath = Dom4jUtils.getAttributeValue(requestElement, "childXpath", "").trim();
            objectType = Dom4jUtils.getAttributeValue(requestElement, "objectType", "").trim();
            listName = Dom4jUtils.getAttributeValue(requestElement, "listName", "").trim();
            
            if(action.equals(RequestIQConst.READ_LIST) || action.equals(RequestIQConst.READ_SINGLE)){
                startFrom = CompUtils.intValue(Dom4jUtils.value(requestElement, "startFrom", "0").trim(), 0);
                countToFetch = CompUtils.intValue(Dom4jUtils.value(requestElement, "countToFetch", "-1").trim(), -1);
                orderby = Dom4jUtils.values(requestElement, "orderby");
                whereXpath = Dom4jUtils.values(requestElement, "whereXpath");
            }

            query = Dom4jUtils.value(requestElement, "query", "").trim();
            try {
                itemData = CompUtils.decode(Dom4jUtils.value(requestElement, "data", "").trim());
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
            try {
                selectXML = CompUtils.decode(Dom4jUtils.value(requestElement, "selectXML", "").trim());
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }else {
            String domainName2 = Dom4jUtils.getAttributeValue(requestElement, "serviceDomain", "").trim();
            subscribeJID = Dom4jUtils.getAttributeValue(requestElement, "subscribeJID", "").trim();
            action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
            if (!domainName2.equals("")) {
                domainName = domainName2;
            }
            String elementName = requestElement.getName();
            action = Dom4jUtils.getAttributeValue(requestElement, "action", "none").trim();
            nodeID = Dom4jUtils.getAttributeValue(requestElement, "node", "").trim();
            //delete, disco#items, subscribe, unsubscribe
            if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
                if (elementName.equals(RequestIQConst.GET_CHILDREN) ) {
                    action=RequestIQConst.GET_CHILDREN;
                } else if (elementName.equals(RequestIQConst.OBJECT_EXISTS) ) {
                    action=RequestIQConst.EXISTS_OBJECT;
                }  
            }            
            else if(elementName.equals("items")) {
                if (elementName.equals(RequestIQConst.READ_CHILD_NODES) || elementName.equals(RequestIQConst.READ_CHILDREN) ) {
                    action=RequestIQConst.READ_LIST;
                } else if (elementName.equals(RequestIQConst.READ_NODE) ) {
                    action=RequestIQConst.READ_SINGLE;
                }                  
                try {
                    String queryxml = Dom4jUtils.value(requestElement, "");
                    requestElement.setText("");
                    queryxml = CompUtils.decode(queryxml);
                    Element queryElementIPVS = null;
                    if (queryxml != null && queryxml.trim().length() > 0) {
                        queryElementIPVS = Dom4jUtils.stringToElement(queryxml);
                    }                    
                    if (queryElementIPVS != null) {
                        startFrom = CompUtils.intValue(Dom4jUtils.valueXpath(queryElementIPVS, RequestIQConst.START_FROM, 0 + ""), 0);
                        countToFetch = CompUtils.intValue(Dom4jUtils.valueXpath(queryElementIPVS, RequestIQConst.COUNT_TO_FETCH, "-1"), -1);
                        whereXpath = EngineMDCUtils.getXpathArray(queryElementIPVS);
                  }                
                } catch (Exception ex) {
                    throw new Exception("Invalid request" + ex.getMessage());
                }                
            }            
            else if(elementName.equals("create")) {
                if (elementName.equals(RequestIQConst.CREATE_NODE) || elementName.equals(RequestIQConst.CREATE_CHILDNODES) ) {
                    action=RequestIQConst.ADD_OBJECT;
                } else if (elementName.equals(RequestIQConst.UPDATE_NODE) || elementName.equals(RequestIQConst.UPDATE_CHILDNODES) ) {
                    action=RequestIQConst.REPLACE_OBJECT;
                } else if (elementName.equals(RequestIQConst.CREATE_ROOT) ) {
                    action=RequestIQConst.CREATE_DOMAIN;
                }    
                try {
                    String xml = requestElement.getText();
                    requestElement.setText("");
                    itemData = CompUtils.decode(xml);
                } catch (Exception ex) {
                    throw new Exception("Invalid request" + ex.getMessage());
                }
                // read parent NID
                Element configureElement = requestElement.getParent().element("configure");
                if (configureElement != null) {
                    Document doc = Dom4jUtils.getDocument();
                    Dom4jUtils.addElement(doc, configureElement.createCopy());
                    if (configureElement != null) {
                        Element formElement = configureElement.element(QName.get("x", "jabber:x:data"));
                        parentNode = IQHandlerUtils.parseParentNID(formElement);
                    }
                }
            }
            else if (elementName.equals("subscribe")  ) {
                action=RequestIQConst.SUBSCRIBE_OBJECT;
            }
            else if (elementName.equals("unsubscribe")  ) {
                action=RequestIQConst.UNSUBSCRIBE_OBJECT;
            } else if (elementName.equals("delete") ) {
                if (elementName.equals(RequestIQConst.DELETE_NODE) || elementName.equals(RequestIQConst.DELETE_CHILD_NODES) ) {
                    action=RequestIQConst.DELETE_OBJECT;
                } else if (elementName.equals(RequestIQConst.DELETE_ROOT) ) {
                    action=RequestIQConst.DELETE_DOMAIN;
                }  
            }
            
//             //if ("http://jabber.org/protocol/disco#info".equals(namespace)) {
//            if ("http://jabber.org/protocol/disco#items".equals(namespace)) {
//                //return "disco#items";
//                
//            } else if ("http://jabber.org/protocol/pubsub".equals(namespace) || "http://jabber.org/protocol/pubsub#owner".equals(namespace)) {
//                if (childElement.element("subscribe") != null) {
//                }
//                if (childElement.element("unsubscribe") != null) {
//                }
//                if (childElement.element("create") != null) {
//                }
//                if (childElement.element("items") != null) {
//                }
//                if (childElement.element("delete") != null) {
//                }
//            }  
//        
//        }
//       
//    }

*/
