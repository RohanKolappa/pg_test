package com.ipvs.common.db;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;

public class ServiceRequestAction {
    public static final String ACTION_GET_REQUEST = "GetRequest";
    public static final String ACTION_REPLACE_REQUEST = "ReplaceRequest";
    public static final String ACTION_DELETE_REQUEST = "DeleteRequest";
    public static final String ACTION_ADD_REQUEST = "AddRequest";
    public static final String ACTION_QUERY_REQUEST = "ReadQueryRequest";
    
    private ServiceRequestActionFilter filter;
    
    //action    
    private String action = "GetRequest";
    //level of node
    private String NID = ""; //document level nid
    
    private List<String> whereXPathList = new ArrayList<String>(); //child level add
    //add 
    private Element operationDataEL = null;
    
    //cursor
    private String startFrom = "";
    private String countToFetch = "";
    
    //get
    private List<String> SortByXPathList = new ArrayList<String>();
    private String sortOrder =MRXMLDBUtils.SORT_ASCENDING;
   

	private String selectXml = null;
    
    //action
    private String query = "";
    private String childXpath = "";
    //private String elementContextXpathReplace = "";
    //private String elementContextXpathAdd = "";
    //private String elementContextXpathDelete = "";
    
	private String requestID="requestID";


	public ServiceRequestAction() {}
    
    public ServiceRequestAction(String action) {
        this.action = action;
    }
    
    /////////////////////

    public Element getActionEL() throws Exception {
        if (filter == null) {
        	filter = new ServiceRequestActionFilter();
        }
        
        Element selectEL = null;
        Element actionEL = MRXMLUtils.addElement(action);
        
        MRXMLUtils.addElement(actionEL, "RequestID",requestID);
        //create NID tag
        if (!filter.isNIDEnabled()) {
            NID = "";
        }
        if (action.equalsIgnoreCase(ACTION_QUERY_REQUEST)) {
    		//MRXMLUtils.addElement(actionEL, "query", query);
    		MRXMLUtils.addElement(actionEL, "Query", query);
            return actionEL;
        } else {
        	Element pathSpecEL = null;
        	if (action.equalsIgnoreCase(ACTION_ADD_REQUEST)) {
        		//create PathSpec
        		pathSpecEL = MRXMLUtils.addElement(actionEL, "ParentPathSpec");
        		//create NID
        		MRXMLUtils.addElement(actionEL, "ParentNID", NID);
        	}else{
        		//create PathSpec
        		pathSpecEL = MRXMLUtils.addElement(actionEL, "PathSpec");
        		//create NID
        		MRXMLUtils.addElement(actionEL, "NID", NID);
        	}
             //create WhereXPath
            selectEL = MRXMLUtils.addElement(pathSpecEL, "Where");
            if (filter.isWhereXPathEnabled() && whereXPathList.size()>0) {
                for (Iterator<String> iterator = whereXPathList.iterator(); iterator.hasNext();) {
                    String xPath = iterator.next();
                    MRXMLUtils.addElement(selectEL, "WhereXPath", xPath);
                }
            }else{
                MRXMLUtils.addElement(selectEL, "WhereXPath", "");
            }
            
            //create In
            selectEL = MRXMLUtils.addElement(pathSpecEL, "In");
            if (filter.isWhereXPathEnabled() && childXpath !=null) {
                MRXMLUtils.addElement(selectEL, "ChildXPath", childXpath);
            }else{
                MRXMLUtils.addElement(selectEL, "ChildXPath", "");
            }
            
            //for get 
            if (action.equalsIgnoreCase(ACTION_GET_REQUEST)) {
                
                selectEL = MRXMLUtils.addElement(pathSpecEL, "Select");
                Element listSpecEL = MRXMLUtils.addElement(selectEL, "ListSpec");
                
                
            	//create Cursor tag  
                if (!filter.isCursorEnabled()) {
                    MRXMLUtils.addElement(listSpecEL, "StartFrom", "");
                    MRXMLUtils.addElement(listSpecEL, "CountToFetch", "");
                }else{
                    MRXMLUtils.addElement(listSpecEL, "StartFrom", startFrom + "");
                    MRXMLUtils.addElement(listSpecEL, "CountToFetch", countToFetch + "");
                }
                                
                //create SortOrder  
                MRXMLUtils.addElement(listSpecEL, "SortOrder", sortOrder+"");
                
          	    //create SortByXPath  
                Element sortByXPathListEL = MRXMLUtils.addElement(listSpecEL,  "SortByXPathList");
                if (filter.isSortByXPathEnabled() && SortByXPathList.size()>0) {
                    for (Iterator<String> iterator = SortByXPathList.iterator(); iterator.hasNext();) {
                        String xPath = iterator.next();
                        MRXMLUtils.addElement(sortByXPathListEL, "SortByXpath", xPath);
                    }
                }else{
                    MRXMLUtils.addElement(sortByXPathListEL, "SortByXpath", "");
                }
                

                //create Select XML tag  
                Element itemSpec = MRXMLUtils.addElement(selectEL, "ItemSpec");
                Element selectXmlEL = MRXMLUtils.addElement(itemSpec, "SelectXML");
                if (filter.isSelectXMLEnabled()) {
                    if (selectXml != null) {
                    	MRXMLUtils.setValue(selectXmlEL, selectXml);
                    }
                }
            }else   if (action.equalsIgnoreCase(ACTION_ADD_REQUEST) || action.equalsIgnoreCase(ACTION_REPLACE_REQUEST)) {
            	//create operationData
                Element operationData = MRXMLUtils.addElement(actionEL, "OperationData");
                if (filter.isOperationDataEnabled()) {
                    if(operationDataEL==null){
                        System.out.print("operationDataEL is null");
                    }else{
                        MRXMLUtils.addElement(operationData, (Element) operationDataEL.clone());
                    }
                }
            }
        }
        return actionEL;
    }
	
	public String getRequestID() {
		return requestID;
	}
	public void setRequestID(String requestID) {
		this.requestID = requestID;
	}
	
    public String getSortOrder() {
		return sortOrder;
	}

	public void setSortOrder(String sortOrder) {
		this.sortOrder = sortOrder;
	}
    public String getQuery() {
        return query;
    }
    
    public void setQuery(String query) {
        this.query = query;
    }
    
    public String getAction() {
        return action;
    }
    
    public void setAction(String action) {
        this.action = action;
    }
    
    //////// NID
    public String getNID() {
        return NID;
    }
    
    public void setNID(String nid) {
        NID = nid;
    }
    
    public String getParentNID() {
        return NID;
    }
    
    public void setParentNID(String nid) {
        NID = nid;
    }
    
    //////// where xpath 
    public List<String> getWhereXPathList() {
        return whereXPathList;
    }
    
    public void setWhereXPathList(String[] arrwhereXPath) {
        for (int i = 0; i < arrwhereXPath.length; i++) {
            whereXPathList.add(arrwhereXPath[i]);
        }
    }
    
    public void setWhereXPathList(List<String> whereXPathList) {
        this.whereXPathList = whereXPathList;
    }
    
    public void addWhereXPath(String whereXPath) {
        if (whereXPathList == null) whereXPathList = new ArrayList<String>();
        whereXPathList.add(whereXPath);
    }
    
    public void removeWhereXPath(String whereXPath) {
        if (whereXPathList == null) whereXPathList = new ArrayList<String>();
        whereXPathList.remove(whereXPath);
    }
    
    ////////SortBy XPath
    public List<String> getSortByXPathList() {
        return SortByXPathList;
    }
    
    public void setSortByXPathList(List<String> SortByXPathList) {
        this.SortByXPathList = SortByXPathList;
    }
    
    public void setSortByXPathList(String[] arrSortByXPath) {
        for (int i = 0; i < arrSortByXPath.length; i++) {
            SortByXPathList.add(arrSortByXPath[i]);
        }
    }
    
    public void addSortByXPath(String SortByXPath) {
        if (SortByXPathList == null) SortByXPathList = new ArrayList<String>();
        SortByXPathList.add(SortByXPath);
    }
    
    public void removeSortByXPath(String SortByXPath) {
        if (SortByXPathList == null) SortByXPathList = new ArrayList<String>();
        SortByXPathList.remove(SortByXPath);
    }
    
    ////////operation Data
    public Element getOperationDataEL() {
        return operationDataEL;
    }
    
    public void setOperationDataEL(Element operationDataEL) {
        this.operationDataEL = operationDataEL;
    }
    
    public void setOperationDataString(String operationDataXml) throws Exception {
        operationDataEL = MRXMLUtils.addElementFromXml(operationDataXml);
    }
    
    ////////cursor
    public String getStartFrom() {
        return startFrom;
    }
    
    public void setStartFrom(String startFrom) {
        this.startFrom = startFrom;
    }
    
    public String getCountToFetch() {
        return countToFetch;
    }
    
    public void setCountToFetch(String countToFetch) {
        this.countToFetch = countToFetch;
    }
    
    ////////selectXml
    /*public Element getSelectXmlEL() {
        return selectXmlEL;
    }
    
    public void setSelectXmlEL(Element selectXmlEL) {
        this.selectXmlEL = selectXmlEL;
    }*/
    
    public void setSelectXml(String selectXml) throws Exception {
    	this.selectXml=selectXml;
     }

    ////////////// select xml filter
    public ServiceRequestActionFilter getFilter() {
        if (filter == null) filter = new ServiceRequestActionFilter();
        return filter;
    }
    
    //element context 
    public void setFilter(ServiceRequestActionFilter filter) {
        this.filter = filter;
    }
    
    public String getChildXpath() {
		return childXpath;
	}

	public void setChildXpath(String childXpath) {
		this.childXpath = childXpath;
	}
    
}
