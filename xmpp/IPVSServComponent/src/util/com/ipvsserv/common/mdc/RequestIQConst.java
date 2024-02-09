package com.ipvsserv.common.mdc;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class RequestIQConst {
    public static final String MEDIAROOM_NAME_SPACE ="ipvs:mediaroom:ipvsdbservice";
    
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
    
    //////////////////////// back word compatibility ////////
    public static final String CREATE_NODE = "createNode";
    public static final String UPDATE_NODE = "updateNode";
    public static final String CREATE_CHILDNODES = "createChildNode";
    public static final String CREATE_ROOT = "createRoot";
    public static final String UPDATE_CHILDNODES = "updateChildNode";
    
    public static final String DELETE_ROOT = "deleteRoot";
    public static final String DELETE_NODE = "deleteNode";
    public static final String DELETE_CHILD_NODES = "deleteChildNodes";
    
    public static final String OBJECT_EXISTS = "objectExists";
   //public static final String GET_CHILDREN = "getChildren";
    
    public static final String ROOT = "BulkAction";
    public static final String XPATH_LIST = "XPathList";
    public static final String XPATH = "XPath";
    public static final String REQUEST_FILTE_RXML = "RequestFilterXml";
    public static final String START_FROM = "StartFrom";
    public static final String COUNT_TO_FETCH = "CountToFetch";
    public static final String DEPTH = "Depth";
    public static final String READ_PARENT = "Parent";
    public static final String GET_CHILD_COUNT = "GetChildCount";
    
    public static final String READ_NODE = "readNode";
    public static final String READ_SCHEMA = "readSchema";
    public static final String READ_CHILDREN = "readChildren";
    public static final String EXECUTE_XQUERY = "executeXQuery";
    
    public static final String EXPORT_DB = "exportdb";
    public static final String IMPORT_DB = "importdb";
    public static final String DELETE_DB = "deletedb";
    
    public static final String GET_SUBSCRIBERS= "getSubscribers";
    public static final String GET_SUBSCRIPTIONS= "getSubscriptions";
    
    // tobe removed
    public static final String FIND_CHILD_NODES = "findChildNodes";
    public static final String READ_CHILD_NODES = "readChildNodes";
    public static final String CHILD_NODES_LIST = "childNodesList";
    public static final String CHILD_NODES_COUNT = "childNodesCount";
    public static final String RUN_XQUERY = "runXQuery";
    
    //public static final String SUBSCRIBE = "subscribe";
    //public static final String UNSUBSCRIBE = "unsubscribe";
    
    
    public static final String PREFIX_OBJ="#OBJ_";
    public static final String SUFFIX_OBJ="_OBJ#";
    public static final int PREFIX_OBJ_LEN=PREFIX_OBJ.length();
    public static final int SUFFIX_OBJ_LEN=SUFFIX_OBJ.length(); 
    public static final String SELECT_XML_VAR="$VAR";

    public static final String PREFIX_SELECTXML="<xpath>";
    public static final String SUFFIX_SELECTXML="</xpath>";
    public static final int PREFIX_SELECTXML_LEN=PREFIX_SELECTXML.length();
    public static final int SUFFIX_SELECTXML_LEN=SUFFIX_SELECTXML.length(); 
    public  static String SORT_ASCENDING= "ascending";
    public  static String SORT_DESCENDING= "descending";
    
    
    // import export tags ///
    public static final String EXPORT_MEDIA_DIR = "exportMedia";
    public static final String IMPORT_MEDIA_DIR = "importMedia";
    public static final String DELETE_MEDIA_DIR = "deleteMedia";
    public static final String EXPORT_MEDIA_FILE = "exportMediaFile";
    public static final String IMPORT_MEDIA_FILE = "importMediaFile";
    public static final String DELETE_MEDIA_FILE = "deleteMediaDFile";
    

}
