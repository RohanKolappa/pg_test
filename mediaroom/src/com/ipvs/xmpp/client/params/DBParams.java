package com.ipvs.xmpp.client.params;

import java.io.UnsupportedEncodingException;
import java.util.Iterator;
import java.util.Map;

import com.ipvs.common.MRLog;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.smack.packets.ResponseElement;


public class DBParams extends XmppParams {
        
    public static final String ADD="add";
    public static final String REPLACE="replace";
    public static final String DELETE="delete";

    public static final String READ_SINGLE="readSingle";
    public static final String READ_LIST="readList";
    
    public static final String GETCHILDREN="getChildren";
    public static final String IS_OBJECT_EXISTS="isObjectExist";
    
    public static final String READ_QUERY="readQuery"; 
     
    public static final String SUBSCRIBE="subscribe";
    public static final String UPDATESUBSCRIPTIONS="updateSubscriptions";
    public static final String UNSUBSCRIBE="unsubscribe";
    public static final String CREATE_DOMAIN="createDomain";
    public static final String DELETE_DOMAIN="deleteDomain";
    
    public static final String IMPORT_DB="importdb";
    public static final String EXPORT_DB="exportdb";    
    public static final String DELETE_DB="deletedb";
    
    public static final String GET_SUBSCRIBERS="getSubscribers";
    public static final String GET_SUBSCRIPTIONS="getSubscriptions";
    
    private String fromJID = null;
    private String toJID = null;
    
    private String action = null;
    private String serviceDomain= null;
    private String subscribeJID = null;
    
    private String nodeID = null;
   
   
    private String childXpath = null;
    private String[] whereXpath = null;
    private String itemData = null;
    
    private String[] sortBy = null;
    private String sortOrder= null;
    
   

	private int startFrom=-1;
    private int  countToFetch=-2;
    private String selectXML=null;   
    private String query=null;
    
    private String objectType=null;
    private String listName=null;
    private Map<String,String>  queryXPathList=null;
    private Map<String,String>  queryNIDList = null; 
    
    private String importDBParams = null;
    private String exportDBParams = null;    
    private String deleteDBParams = null;
    private String type = null; //import  export type
    private String xQueryPrefix = null;
    private String selectFormat = null;
    
    private String subscriptionParams = null;
   
    public long DBCallTimeOut = 40*1000; 
    
    public DBParams(){
        setNamespace(ResponseElement.IPVSDBSERVICE);
    }
    
    public long getDBCallTimeOut() {
        return DBCallTimeOut;
    }
    public void setDBCallTimeOut(long timeOut) {
        this.DBCallTimeOut = timeOut;
    }
    public String getSortOrder() {
		return sortOrder;
	}
	public void setSortOrder(String sortOrder) {
		this.sortOrder = sortOrder;
	}
    public void setQueryNIDList(Map<String, String> readQueryNIDList) {
		this.queryNIDList = readQueryNIDList;
	}
	public Map<String, String> getQueryNIDList() {
		return queryNIDList;
	}
	public Map<String, String> getQueryXPathList() {
        return queryXPathList;
    }
    public void setQueryXPathList(Map<String, String> readQueryXPathList) {
        this.queryXPathList = readQueryXPathList;
    }
    
    public String getFromJID() {
        return fromJID;
    }
    public void setFromJID(String fromJID) {
        this.fromJID = fromJID;
    }
    public String getToJID() {
        return toJID;
    }
    public void setToJID(String toJID) {
        this.toJID = toJID;
    }
    
    public String getObjectType() {
        return objectType;
    }
    public void setObjectType(String objectType) {
        this.objectType = objectType;
    }
	public String getListName() {
        return listName;
    }
    public void setListName(String listName) {
        this.listName = listName;
    }
    public String getQuery() {
        return query;
    }
    public void setQuery(String query) {
        this.query = query;
    }
    public String getSubscribeJID() {
		return nonNull(subscribeJID);
	}
	public void setSubscribeJID(String subscribeJID) {
		this.subscribeJID = subscribeJID;
	}
	public String getServiceDomain() {
        return nonNull(serviceDomain);
	}
	public void setServiceDomain(String serviceDomain) {
        this.serviceDomain = serviceDomain;
    }
 	public String getAction() {
		return action;
	}
	public void setAction(String action) {
		this.action = action;
	}
	private String nonNull(String value){
	    if(value==null)
            return "";
	    return value;
	}
    public String getNodeID() {
        return nodeID;
    }
    public void setNodeID(String nodeID) {
        this.nodeID = nodeID;
    }
 
    public String getChildXpath() {
        return childXpath;
    }
    public void setChildXpath(String childXpath) {
        this.childXpath = childXpath;
    }
    public String getXQueryPrefix() {
        return xQueryPrefix;
    }
    public void setXQueryPrefix(String xQueryPrefix) {
        this.xQueryPrefix = xQueryPrefix;
    }    
    public String[] getWhereXpath() {
        return whereXpath;
    }
    public void setWhereXpath(String[] whereXpath) {
        this.whereXpath = whereXpath;
    }
    public String getItemData() {
        return itemData;
    }
    public void setItemData(String itemData) {
        this.itemData = itemData;
    }
    public String[] getSortBy() {
        return sortBy;
    }
    public void setSortBy(String[] SortBy) {
        this.sortBy = SortBy;
    }
    public int getStartFrom() {
        return startFrom;
    }
    public void setStartFrom(int startFrom) {
        this.startFrom = startFrom;
    }
    public int getCountToFetch() {
        return countToFetch;
    }
    public void setCountToFetch(int countToFetch) {
        this.countToFetch = countToFetch;
    }
    public String getSelectXML() {
        return selectXML;
    }
 
    public void setSelectXML(String selectXML) {
        this.selectXML = selectXML;
    }	
    ////////////////// import export //////////////
    
    @Override
    public void setXML(String payload) {
    }

    public String getImportDBParams() {
        return importDBParams;
    }

    public void setImportDBParams(String importDBParams) {
        this.importDBParams = importDBParams;
    }

    public String getExportDBParams() {
        return exportDBParams;
    }

    public void setExportDBParams(String exportDBParams) {
        this.exportDBParams = exportDBParams;
    } 
    
    public String getDeleteDBParams() {
        return deleteDBParams;
    }
    
    public void setDeleteDBParams(String deleteDBParams) {
        this.deleteDBParams = deleteDBParams;
    }
    
    //import export type
    public String getType() {
        return type;
    }
    public void setType(String type) {
        this.type = type;
    }
    
    //Subscription/Subscriber Response format
    public String getSelectFormat() {
		return selectFormat;
	}

	public void setSelectFormat(String selectFormat) {
		this.selectFormat = selectFormat;
	}
	
    ////////////////// import export //////////////    
     public String toXML(){
        StringBuffer buf = new StringBuffer();
        if(getAction().equals(EXPORT_DB) || getAction().equals(IMPORT_DB) || getAction().equals(DELETE_DB)) {
            buf.append("<request xmlns='"+ResponseElement.IPVSDBSERVICE+"'>");
            buf.append("<"+getAction()+"   domain='"+getServiceDomain()+"' ");
            if(getDBCallTimeOut() >0 ) {
                buf.append(" DBCallTimeOut='"+ getDBCallTimeOut()+"' ");
            }            
            if(getType() !=null ) {
                buf.append(" type='"+ getType()+"' >");
            }
            if(getAction().equals(EXPORT_DB)) {
                if(getExportDBParams() !=null  ) {
                    buf.append( getExportDBParams() );             
                }    
            }else if(getAction().equals(IMPORT_DB)) {
                if(getImportDBParams() !=null ) {
                    buf.append( getImportDBParams() );             
                }   
            }else if(getAction().equals(DELETE_DB)) {
                if(getDeleteDBParams() !=null ) {
                    buf.append( getDeleteDBParams() );             
                }    
            }   
            buf.append("</"+getAction()+"></request>");
            return buf.toString();
        } 
        
        buf.append("<request xmlns='"+ResponseElement.IPVSDBSERVICE+"'>");
        buf.append("<"+getAction()+"   domain='"+getServiceDomain()+"' ");
        if(getNodeID() !=null ) {
            buf.append(" nodeID='"+ getNodeID()+"' ");
        }
        if(getSubscribeJID() !=null ) {
            buf.append(" subscribeJID='"+ getSubscribeJID()+"' ");
        }        
        if(getObjectType() !=null ) {
            buf.append(" objectType='"+ getObjectType()+"' ");
        }
        if(getListName() !=null ) {
            buf.append(" listName='"+ getListName()+"' ");
        }
        if(getChildXpath() !=null ) {
            try {
                buf.append(" childXPath='"+ Utils.encode(getChildXpath())+"' ");
            } catch (Exception ex) {
                MRLog.println("error encode childXPath='"+ getChildXpath()+"' ");
                ex.printStackTrace();
            }
        }
        if(getDBCallTimeOut() >0 ) {
            buf.append(" DBCallTimeOut='"+ getDBCallTimeOut()+"' ");
        }
        
        if(getSelectFormat() !=null ) {
            buf.append(" selectFormat='"+ getSelectFormat()+"' >");
        }
        
        buf.append(" >");
        
        if(getAction().equals(READ_QUERY)) {
            if(getQuery() !=null ) {
                buf.append("<query><![CDATA[ ").append( getQuery()).append(" ]]></query>");             
                if(getQueryXPathList()!=null && getQueryXPathList().size()>0) {
                    for (Iterator<String> iterator = queryXPathList.keySet().iterator(); iterator.hasNext();) {
                        String objecttype = iterator.next();
                        if(objecttype !=null) {
                            String pemXpath = queryXPathList.get(objecttype);
                            if(pemXpath !=null && pemXpath.trim().length()>0) {
                                buf.append("<queryXPath  objecttype='"+objecttype+"'><![CDATA[ ").append( pemXpath ).append(" ]]></queryXPath>");
                            }
                        }
                    }
                }
                if(getQueryNIDList()!=null && getQueryNIDList().size()>0) {
                    for (Iterator<String> iterator = queryNIDList.keySet().iterator(); iterator.hasNext();) {
                        String objecttype = iterator.next();
                        if(objecttype !=null) {
                            String pemXpath = queryNIDList.get(objecttype);
                            if(pemXpath !=null && pemXpath.trim().length()>0) {
                                buf.append("<queryNID  objecttype='"+objecttype+"'><![CDATA[ ").append( pemXpath ).append(" ]]></queryNID>");
                            }
                        }
                    }
                }
            }
        } else {
            /*if(getChildXpath() !=null ) {
                buf.append("<childXPath><![CDATA[ ").append( getChildXpath() ).append(" ]]></childXPath>");              
            }*/            
        	
        	if(getAction() == SUBSCRIBE || getAction() == UPDATESUBSCRIPTIONS
        		|| getAction() == UNSUBSCRIBE || getAction() == GET_SUBSCRIBERS) {
        		if(getSubscriptionParams() != null) {
        			buf.append(getSubscriptionParams());
        		}
        	}
        	
            if(getWhereXpath() !=null ) {
                for (int i = 0; i < whereXpath.length; i++) {
                    String str = whereXpath[i].trim() ;
                    if(str.length() >0)
                        buf.append("<whereXPath><![CDATA[ ").append( str ).append(" ]]></whereXPath>");              
                }
            }   
            if(getXQueryPrefix() !=null ) {
                buf.append("<xQueryPrefix><![CDATA[ ").append( getXQueryPrefix() ).append(" ]]></xQueryPrefix>");              
            } 
            if( getAction().equals(DELETE)){
               //do nothing
            } else if( getAction().equals(REPLACE)
                    || getAction().equals(ADD)){
                if(getItemData()!=null  ) {
                    //buf.append( "<data>"+ Utils.encode(getItemData()) +"</data>");
                    buf.append( "<data>"+ getItemData() +"</data>");
                }
            } else if( getAction().equals(READ_LIST) || getAction().equals(READ_SINGLE)) {
                // only get 
                if(getStartFrom() >-1) {
                    buf.append("<startFrom>").append( getStartFrom()).append("</startFrom>");
                }
                if(getCountToFetch() >-2) {
                    buf.append("<countToFetch>").append( getCountToFetch()).append("</countToFetch>");
                } 
                if(getSelectXML() !=null) {
                    try {
                        buf.append("<selectXML>").append( Utils.encode(getSelectXML())).append("</selectXML>");
                    } catch (UnsupportedEncodingException e) {
                        System.out.println("Error encoding selectXML " + getSelectXML());
                        e.printStackTrace();
                    }
                }  
                if(getSortOrder() !=null) {
                    buf.append("<sortOrder>").append( getSortOrder()).append("</sortOrder>");
                }  
                
                if(getSortBy() !=null ) {
                    for (int i = 0; i < sortBy.length; i++) {
                        String str = sortBy[i].trim() ;
                        if(str.length() >0)
                            buf.append("<sortByXPath><![CDATA[ ").append( str ).append(" ]]></sortByXPath>");              
                    }
                }            
            }
        }
        buf.append("</"+getAction()+"></request>");
        return buf.toString();
    }

	public String getSubscriptionParams() {
		return subscriptionParams;
	}

	public void setSubscriptionParams(String subscriptionParams) {
		this.subscriptionParams = subscriptionParams;
	}

}
 
	
 
