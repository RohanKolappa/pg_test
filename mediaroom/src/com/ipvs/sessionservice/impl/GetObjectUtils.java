package com.ipvs.sessionservice.impl;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.session.handler.GetPortRequestHandler;
import com.ipvs.utils.FileUtils;



public class GetObjectUtils {
    public static final String STRING_DELIMITOR = ",";
    public static final String KEY_VALUE_PAIR_DELIMITOR = ":";
    
    /**
     * Input string format is key1:value,key2:value
     * Output return the map for given string
     * @param mappedString
     * @return
     */
    public static Map<String, String> getInfoMapping(String mappedString) throws MRException, Exception {
        if ((mappedString == null) || mappedString.isEmpty()) {
            return null;
        }

        StringTokenizer stringTokenizer = new StringTokenizer(mappedString, STRING_DELIMITOR);
        Map<String, String> stringMap = new HashMap<String, String>();
        String keyValuePair = null;
        String key = null;
        String value = null;

        while (stringTokenizer.hasMoreTokens()) {
            keyValuePair = stringTokenizer.nextToken();

            if (keyValuePair.indexOf(KEY_VALUE_PAIR_DELIMITOR) < 0) {
                throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input. String:=" + mappedString);
            }

            key = keyValuePair.substring(0, keyValuePair.indexOf(KEY_VALUE_PAIR_DELIMITOR)).trim();
            value = keyValuePair.substring(keyValuePair.indexOf(KEY_VALUE_PAIR_DELIMITOR) + 1).trim();
            stringMap.put(key, value);
        }

        return stringMap;
    }

    /**
     * Returns the xPath generated for given Delimiter.
     * @param listOfXpaths
     * @param delimitor
     * @return
     */
    public static String generateWhereXpathFromList(List<String> listOfXpaths, String delimiter, String objectType) {
        String  whereXpath = generateWhereXpathFromList(listOfXpaths, delimiter);
        if (!whereXpath.toString().isEmpty() && !SessionServiceConst.FILE.equalsIgnoreCase(objectType)) {
            return "[" + whereXpath.toString() + "]";
        } else if (!whereXpath.toString().isEmpty() && SessionServiceConst.FILE.equalsIgnoreCase(objectType)) {
            return "(" + whereXpath.toString() + ")";
        }
        return "";
    }
    
    
    
    public static String generateWhereXpathFromList(List<String> listOfXpaths, String delimiter) {
        StringBuffer whereXpathBuffer = new StringBuffer("");
        String delim = "";

        if (listOfXpaths != null) {
            for (String xpath : listOfXpaths) {
                whereXpathBuffer.append(" " + delim + " ");
                whereXpathBuffer.append("( " + xpath + ")");
                delim = delimiter;
            }
        }
       
        return whereXpathBuffer.toString();
    }

    public static List<String> generateXPath(String whereDateRange, String whereNID,
        String whereTagExpr, String whereTextSubStr, String searchTextSubStrIn, List<String> listOfWhereXpath, String objectType) {
        if (!"".equals(whereDateRange)) {
            String whereDateRangeXPath = generateXPathForDateRange(whereDateRange);

            if (!whereDateRangeXPath.isEmpty()) {
                listOfWhereXpath.add(whereDateRangeXPath);
            }
        }

        if (!"".equals(whereNID)) {
            String whereNIDXPath = generateXPath(whereNID, SessionServiceConst.WHERE_NID, objectType, SessionServiceConst.NID_CONST);

            if (!whereNIDXPath.isEmpty()) {
                listOfWhereXpath.add(whereNIDXPath);
            }
        }

        if (!"".equals(whereTagExpr)) {
            String whereTagExprXPath = generateXPath(whereTagExpr, SessionServiceConst.WHERE_TAG_EXPR, objectType, "");

            if (!"".equals(whereTagExprXPath)) {
                listOfWhereXpath.add(whereTagExprXPath);
            }
        }

        if (!"".equals(whereTextSubStr) && !objectType.equals(SessionServiceConst.FILE)) {
            String whereTextSubStrXPath = generateXPath(whereTextSubStr, SessionServiceConst.WHERE_TEXT_SUB_STR, searchTextSubStrIn, objectType, SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST);

            if (!"".equals(whereTextSubStrXPath)) {
                listOfWhereXpath.add(whereTextSubStrXPath);
            }
        }

        return listOfWhereXpath;
    }

    public static String addXPath(List<String> xpathList) {
        StringBuffer xpath = null;

        if ((xpathList == null) || (xpathList.size() == 0)) {
            return "";
        }

        for (int i = 0; i < xpathList.size(); i++) {
            if (xpath == null) {
                xpath = new StringBuffer();
            } else {
                xpath.append(" and ");
            }

            xpath.append("( " + xpathList.get(i) + " ) ");
        }

        return "[" + xpath.toString() + "]";
    }

    public static String generateQueryString(MRRequest request, String viewObjectType, String startFrom, String countToFetch, 
    		String sortBy, String sortOrder, String detail, String whereDateRange, String whereNID, String whereTagExpr, 
    		String whereTextSubStr, String searchTextSubStrIn, String wherePermissions,List<String> listOfWhereXpath, 
    		String modelObjectType, String xQueryFileName, Hashtable<String, String> customeTokenMappingTable,List<String> pemObjectList) throws Exception {
    	String query = "";
    	try {            
    		String objectQuery = FileUtils.readFileResource(xQueryFileName, request.getClass());
            String objectListQuery = FileUtils.readFileResource(SessionServiceConst.OBJECTLISTXQUERY, request.getClass());
                        

            
            if (!"".equals(sortBy) && viewObjectType.equals(SessionServiceConst.TAGS) ||
                    viewObjectType.equals(SessionServiceConst.USERS)||
                    viewObjectType.equals(AMConst.Device)||
                    viewObjectType.equals(SessionServiceConst.PROFILES) ||
                    viewObjectType.equals(SessionServiceConst.MEDIAROOM)||
                    viewObjectType.equals(SessionServiceConst.PORTS)||
                    viewObjectType.equals(SessionServiceConst.MEDIA)||
                    viewObjectType.equals(SessionServiceConst.ALL)
                    ) {
                  sortBy = "/" + sortBy;                  
            } else if(!"".equals(sortBy)){
            	sortBy = "//" + sortBy;
            }

            generateXPath(whereDateRange, whereNID, whereTagExpr, whereTextSubStr, searchTextSubStrIn, listOfWhereXpath, viewObjectType);
            
            String whereXPath = generateWhereXpathFromList(listOfWhereXpath, "and", viewObjectType);
            
            	
            query = objectQuery + objectListQuery;
            query = query.replace(SessionServiceConst.STARTFROM, startFrom);
            query = query.replace(SessionServiceConst.COUNTTOFETCH, countToFetch);
            query = query.replace(SessionServiceConst.XPATHAND, whereXPath.toString());
            query = query.replace(SessionServiceConst.SORTBY, sortBy);
            query = query.replace(SessionServiceConst.SORTORDER, sortOrder);
            query = query.replace(SessionServiceConst.DETAIL, detail);
            query = query.replace(SessionServiceConst.OBJECTTYPE, viewObjectType);
            
            String protocol = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//Header/Protocol", "http");           
    		query = query.replace(SessionServiceConst._PROTOCOL_,protocol);
            if(viewObjectType.equals(SessionServiceConst.TAGS) ||
        	    viewObjectType.equals(SessionServiceConst.USERS)||
                    viewObjectType.equals(AMConst.Device)||
                    viewObjectType.equals(SessionServiceConst.PROFILES) ||
                    viewObjectType.equals(SessionServiceConst.MEDIAROOM)||
                    viewObjectType.equals(SessionServiceConst.PORTS)||
                    viewObjectType.equals(SessionServiceConst.MEDIA)||
                    viewObjectType.equals(SessionServiceConst.ALL)) {
                 query = query.replace(SessionServiceConst.FORMATTYPE, SessionServiceConst.TAG_LIST_FORMAT_TYPE);
            } if(viewObjectType.equals(SessionServiceConst.FILE) || viewObjectType.equals(SessionServiceConst.PORT) || viewObjectType.equals(AMConst.AssetResource) || viewObjectType.equals(SessionServiceConst.DIR)) {
                 query = query.replace(SessionServiceConst.FORMATTYPE, SessionServiceConst.LIST_FORMAT_TYPE);
            } else {
            	 query = query.replace(SessionServiceConst.FORMATTYPE, SessionServiceConst.ITEM_FORMAT_TYPE);          	
            }
            
            //replace custom tokens
            if(customeTokenMappingTable != null) {
            	Set<String> tokenSet = customeTokenMappingTable.keySet();
            	for(String token : tokenSet) {
            		String value = customeTokenMappingTable.get(token);
            		query = query.replace(token, value);
            	}
            }
           //System.out.println("query :"+query);
            
            if(sortBy.indexOf((SessionServiceConst.NUMBER_CONST)) >= 0) {
            	query = query.replace(SessionServiceConst.UPPERCASE, "");
            } 
            if(modelObjectType != null) {
            	if(modelObjectType.equals(SessionServiceConst.MEDIA_STREAM_PORT_RESOURCE)) {
            		modelObjectType = "#OBJ_"+ SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE + "_OBJ#" + " | " 
                              +  "#OBJ_"+ SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE + "_OBJ#" + " | " 
                              + "#OBJ_"+ SessionServiceConst.MEDIA_STORE_PORT_RESOURCE + "_OBJ#" + " | " 
                              + "#OBJ_"+ SessionServiceConst.MEDIA_STREAM_RELAY_PORT_RESOURCE + "_OBJ#" ;
                    query = query.replace(SessionServiceConst.OBJECT_TYPE, modelObjectType);            		 
            	} else {            		
            	    query = query.replace(SessionServiceConst.OBJECT_TYPE, "#OBJ_"+ modelObjectType + "_OBJ#");
            	} 
            }
            
    	 } catch (MRException exp) {
             System.out.println("query :"+query);            
             exp.printStackTrace();
             throw new MRException(exp.getCode(), exp.getDescription());
         } catch (Exception exp) {
             System.out.println("query :"+query);            
             exp.printStackTrace();
             throw new MRException(SessionServiceExceptionCodes.GET_LIST_FAILED, MRException.getStackTrace(exp));
         }
            return query;
    }
    
    public static Document getQueryList(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
            MRResourceFactoryI resourceFactory, String viewObjectType, HashMap<String, String> queryParams, String xQueryFileName, List<String> listOfWhereXPath, String modelObjectType,
            Hashtable<String, String> customeTokenMappingTable,List<String> pemObjectList, String requestName, boolean forceXMPP) throws Exception {
    	String query = "";
    	String detail = queryParams.containsKey("selectFormat") ? queryParams.get("selectFormat") : "" ;
    	String wherePermissions = queryParams.containsKey("wherePermissions") ? queryParams.get("wherePermissions") : "";
    	String sortBy = queryParams.containsKey("sortBy") ? queryParams.get("sortBy") : "";
    	String whereDateRange = queryParams.containsKey("whereDateRange") ? queryParams.get("whereDateRange") : "";
    	String whereNID = queryParams.containsKey("whereNID") ?  queryParams.get("whereNID") : "";
    	String whereTagExpr = queryParams.containsKey("whereTagExpr") ?  queryParams.get("whereTagExpr") : "";
    	String whereTextSubStr = queryParams.containsKey("whereTextSubStr") ?  queryParams.get("whereTextSubStr") : "";
    	String searchTextSubStrIn = queryParams.containsKey("searchTextSubStrIn") ?  queryParams.get("searchTextSubStrIn") : "";
    	String startFrom = queryParams.containsKey("startFrom") ?  queryParams.get("startFrom") : "1";
    	String countToFetch = queryParams.containsKey("countToFetch") ? queryParams.get("countToFetch") : "1000";
    	String sortOrder = queryParams.containsKey("sortOrder") ?  queryParams.get("sortOrder") : "";
    
    	query = generateQueryString(request, viewObjectType, startFrom, countToFetch, sortBy, sortOrder, detail, whereDateRange, whereNID, whereTagExpr, whereTextSubStr, searchTextSubStrIn, wherePermissions, listOfWhereXPath, modelObjectType, xQueryFileName, customeTokenMappingTable, pemObjectList);
    	Document resultDoc = executeQuery(query, viewObjectType, pemObjectList, detail,wherePermissions, request, xmpp, log, db, serviceAgentFactory, resourceFactory, forceXMPP);
    	
		Element responseEL = MRXMLUtils.newElement(requestName);
		if ((resultDoc != null)) {
			responseEL = resultDoc.getRootElement();
			responseEL.setName(requestName);
		}
		return request.createResponseData(responseEL);
    }
    
    public static Document getQueryList(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
        MRResourceFactoryI resourceFactory, int startFrom, int countToFetch, String sortBy, String sortOrder, String detail, String whereDateRange,
        String whereNID, String whereTagExpr, String whereTextSubStr, String searchTextSubStrIn, String wherePermissions,List<String> listOfWhereXpath, 
        String queryFile, String viewObjectType,String modelObjectType,Hashtable<String,String> customeTokenMappingTable,List<String> pemObjectList) throws Exception {
    	String query = "";
    	
        try {            
        	query = generateQueryString(request, viewObjectType, String.valueOf(startFrom), String.valueOf(countToFetch), sortBy, sortOrder, detail, whereDateRange, whereNID, whereTagExpr, whereTextSubStr, searchTextSubStrIn, wherePermissions, listOfWhereXpath, modelObjectType, queryFile, customeTokenMappingTable, pemObjectList);
            return executeQuery(query, viewObjectType, pemObjectList, detail,wherePermissions, request, xmpp, log, db, serviceAgentFactory, resourceFactory, true);
                        
        } catch (MRException exp) {
            System.out.println("query :"+query);            
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getDescription());
        } catch (Exception exp) {
            System.out.println("query :"+query);            
            exp.printStackTrace();
            throw new MRException(SessionServiceExceptionCodes.GET_LIST_FAILED, MRException.getStackTrace(exp));
        }
    }
        
    public static Document executeQuery(String query, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
            MRResourceFactoryI resourceFactory) throws MRException,Exception{
        return executeQuery(query, "",null, "","", request, xmpp, log, db, serviceAgentFactory, resourceFactory, true);
    }
    
    public static Document executeQuery(String query, String viewObjectType,List<String> pemObjectList, String detail,String wherePermissions, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
            MRResourceFactoryI resourceFactory, boolean forceXMPP) throws MRException,Exception{
    	Document requestDoc = request.getMessage().getDocument();
    	String requestName= request.getRequestName().replace("Request", "");
    	Element requestElement = MRXMLUtils.getElementXpath(requestDoc, "//"+requestName);
    	String subscriptionID="";
    	if(requestElement!=null){
    		subscriptionID = MRXMLUtils.getAttributeValue(requestElement, "subscriptionID", "");
    	}	    	
				
		long dbtimeout = MRXMLDBUtils.TIME_OUT;         
        Map<String, String> pemXpathList = PolicyAdminServiceUtils.getObjectsFromReadQuery(query);
        //if user provided some pem object list merge those objects in the pemxpathList 
        if(pemObjectList !=null && !pemObjectList.isEmpty()) {
        	for(String pemObject: pemObjectList){
        		if(pemXpathList.containsKey(pemObject)){        			
        			continue;
        		}
        		pemXpathList.put(pemObject, "");
        	}
        }
     	Element operatorMatchingPoliciesEL = MRXMLUtils.newElement(PolicyManager.POLICY_RULE_LIST);    	
    	String dbObjectName = null;
    	Document matchingPoliciesListDoc = null;
    	List<String> roleObjectsFromViewObject = SessionObjectPermissions.getPolicyRoleObjectListForGivenViewObject(log, viewObjectType);
    	if (!request.isSuperUser()) {
        	String pemXPath = null;
        	String serviceName = null;
        	String action = PolicyAdminConstants.READ_ROLE_ACTION;
        	String protocol = MRXMLUtils.getValueXpath(requestDoc, "//Header/Protocol", "http"); 
        	for (Iterator<String> iterator = pemXpathList.keySet().iterator(); iterator.hasNext();) {
        		dbObjectName = iterator.next();
        		pemXPath = null;
        		serviceName = MRConfig.getObjectServiceName(dbObjectName);
        		List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);
        		if (!AMConst.SERVICE_RESOURCE.equalsIgnoreCase(dbObjectName) && !PolicyAdminConstants.GROUPTAGDEF_OBJECTTYPE.equalsIgnoreCase(dbObjectName) && !PolicyAdminConstants.BOOKMARK.equalsIgnoreCase(dbObjectName)) {
        			Document policiesResponseDoc =  PolicyAdminServiceUtils.getTargetXpathResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roleObjectList, action);
        			pemXPath =  validateTargetXPath(policiesResponseDoc, roleObjectList, dbObjectName, action, wherePermissions,roleObjectsFromViewObject, viewObjectType, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
        		    if(!pemXPath.equals(PolicyAdminConstants.NULL)) {
        	        	MRXMLUtils.addContent(operatorMatchingPoliciesEL, MRXMLUtils.getListXpath(policiesResponseDoc, "//" + PolicyManager.POLICY_RULE));
        	        	if((subscriptionID!=null && !subscriptionID.isEmpty())) {
        	        		AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).addUserToSubscriberPolicyCache(request.getUserJID(), viewObjectType, pemXPath, subscriptionID, protocol, log);
        	        	}
        	        }
        	       
        	    }
        		PolicyAdminServiceUtils.validateReadQueryPemXpath(pemXpathList, pemXPath, roleObjectList, dbObjectName, serviceName, request, db, log);
        	}
	        matchingPoliciesListDoc = MRXMLUtils.elementToDocument(operatorMatchingPoliciesEL);
        }
    	
    	if(pemXpathList != null) {
        	Set<String> tokenSet = pemXpathList.keySet();
        	for(String token : tokenSet) {
        		String value = pemXpathList.get(token);
        		
        		if(request.isSuperUser() || value.equals(""))
        			value = SessionServiceConst.DUMMY_XPATH;
        		
        		query = query.replace(token + SessionServiceConst.PEMXPATH, "(" + value + ")");
        		
        	}
        }

        String queryResult = null;
        if(forceXMPP) {
        	queryResult = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, pemXpathList, null);
        } else {
        	// process over REST
        	queryResult = DBUtils.readQuery(query, pemXpathList, db, log);
        }
        
        if ((queryResult == null) || queryResult.isEmpty()) {
            return null;
        }

        Document queryDoc = MRXMLUtils.stringToDocument(queryResult);
        SessionObjectPermissions.addObjectPermissions(request, xmpp, log, db, serviceAgentFactory, resourceFactory, queryDoc, matchingPoliciesListDoc, viewObjectType, detail);
        return queryDoc;
    	
    }   
    
    public static String validateTargetXPath(Document policiesResponseDoc, List<String> roleObjectList,String dbObjectType,String action,String wherePermissions,
    		List<String> roleObjectsFromViewObject,String viewObjectType, MRRequest request, XMPPI xmpp, MRLog log,MRDB db, ServiceAgentFactoryI serviceAgentFactory,
    		MRResourceFactoryI resourceFactory)  throws MRException, Exception {

    	String targetXPath = MRXMLUtils.getValueXpath(policiesResponseDoc, "//"+PolicyAdminConstants.TARGET_XPATH, PolicyAdminConstants.NULL);
    	if(!targetXPath.equals(PolicyAdminConstants.NULL)){
    		if(!wherePermissions.isEmpty() && (roleObjectList.containsAll(roleObjectsFromViewObject) || viewObjectType.equals(SessionServiceConst.PORT))){
    			targetXPath = PolicyAdminServiceUtils.getTargetXPathForGivenPermissions(policiesResponseDoc, roleObjectList, dbObjectType, action, wherePermissions, request,  xmpp,  log, db,  serviceAgentFactory,
    					resourceFactory);
    		}
    	}


    	if(!targetXPath.isEmpty()  && !targetXPath.equals(PolicyAdminConstants.NULL)) {
    		return "[" + targetXPath  +  "]";
    	}

    	return targetXPath;
    }
	
    
    public static String generateXPath(String stringToGenerateXPathFrom, String attributeForXPathGeneration, String objectType, String constantToReplace) {
    	return generateXPath(stringToGenerateXPathFrom, attributeForXPathGeneration, "", objectType, constantToReplace);
    }

    public static String generateXPath(String stringToGenerateXPathFrom, String attributeForXPathGeneration,String searchSubStrIn, String objectType, String constantToReplace) {
        StringBuffer generatedXPath = null;

        StringTokenizer st = new StringTokenizer(stringToGenerateXPathFrom, ",");

        while (st.hasMoreTokens()) {
            String token = st.nextToken();
            token = token.trim();

            if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_TAG_EXPR)) {
                token = token.replace("Name", "@Name");
                token = token.replace("Value", "@Value");
                token = "//Groups/Tag["+token+"]";
            }else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_METADATA_EXPR)) {            	
            	String regexName = "(Name\\s*!?=\\s*'\\w*')";
        		String regexValue = "Value\\s*=\\s*'(.*)'";
        		
        		Matcher matcherName = Pattern.compile(regexName).matcher(token);
        		Matcher matcherValue = Pattern.compile(regexValue).matcher(token);
        			
        		String name = "";
        		if (matcherName.find()) {
        			name = matcherName.group(1).replace("Name", "@Name");		        			        					
        		}
        		
        		String value = "";
        		if (matcherValue.find()) {
        			value = matcherValue.group(1);        			
        		}
            	
            	if (!name.isEmpty() && !value.isEmpty()) {
            		token = "matches(//MetaData/Tag[" + name + "], '" + value + "', 'i')";
            	} else if (name.isEmpty()) {
            		token = "matches(//MetaData/Tag, '" + value + "', 'i')";
            	} else {
            		token = "//MetaData/Tag[" + name + "]";
            	}	                       	
            }else if(attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_TEXT_SUB_STR)) {           
            	String skeletonXPath = getSkeletonXpathBasedOnWhereTextSubStr(attributeForXPathGeneration,searchSubStrIn, objectType);
            	token = skeletonXPath.replace(constantToReplace, token.toLowerCase());
            } else {           
            	String skeletonXPath = getSkeletonXPathBasedOnAttributeType(attributeForXPathGeneration, objectType);
            	token = skeletonXPath.replace(constantToReplace, token);
            }	
            if (generatedXPath == null) {
                generatedXPath = new StringBuffer("(" + token + ")");
            } else {
                generatedXPath.append(" or (" + token + ")");
            }
        }
        if (generatedXPath != null) {
           return "("+generatedXPath.toString()+")";
        } else {
            return "";
        }
    }
    //due to lower-case(metadata/tag) its failing. Becuase metadata/tag has multiple values. 
    public static String getSkeletonXpathBasedOnWhereTextSubStr(String attributeForXPathGeneration,String searchSubStrIn, String objectType){
    	if(!searchSubStrIn.isEmpty()) {
    		return getSkeletonXpathBasedOnWhereTextSubStrBasedOnSearchSubStrIn(attributeForXPathGeneration,searchSubStrIn,objectType);
    	}
    	
    	
    	if (SessionServiceConst.FILE.equalsIgnoreCase(objectType)) {
        	return " matches(//Info/Title, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')  " + " or  matches(//Info/Description, '" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" + " or matches(//MetaData/Tag,'" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')";
        }
    	else if (SessionServiceConst.USER.equalsIgnoreCase(objectType)) {
        	return " matches(//Username, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')  " + " or  matches(//Name, '" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')";
        }
        else if (SessionServiceConst.BOOKMARK.equalsIgnoreCase(objectType)) {
        	return " matches(//Title, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')  " + " or  matches(//Comment, '" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" + " or matches(//MetaData/Tag,'" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')";
        }
        else{
        	return " matches(//Title, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')  " + " or  matches(//Description, '" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" + " or matches(//MetaData/Tag,'" +
            SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')";
        }
    }

    private static String getSkeletonXpathBasedOnWhereTextSubStrBasedOnSearchSubStrIn(String attributeForXPathGeneration, String searchSubStrIn, String objectType) {
    	List<String> whereSubStrXpathList = new ArrayList<String>();
    	StringTokenizer st = new StringTokenizer(searchSubStrIn, ",");
    	while(st.hasMoreElements()) {
    		 String token = st.nextToken();
             token = token.trim();
             if(token.equals(SessionServiceConst.SEARCH_TEXT_SUB_STR_IN_BOOKMARK)) {
            	 continue;
             }
             
             if(token.equals(SessionServiceConst.SEARCH_TEXT_SUB_STR_IN_TITLE)) { 
            	 whereSubStrXpathList.add( "matches(//Info/Title, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" );;
             }
             
             if(token.equals(SessionServiceConst.SEARCH_TEXT_SUB_STR_IN_DESCRITPTION)) { 
            	 whereSubStrXpathList.add( "matches(//Info/Description, '" + SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" );
             }
             
             if(token.equals(SessionServiceConst.SEARCH_TEXT_SUB_STR_IN_METADATA)) { 
            	 whereSubStrXpathList.add( "matches(//MetaData/Tag,'" +SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST + "', 'i')" );
             }
    	}
    	
    	
		String whereXpath =   generateWhereXpathFromList(whereSubStrXpathList, "or");
		if(!whereXpath.isEmpty()) {
			whereXpath = "(" + whereXpath + ")";
		}
		return whereXpath;
	}

	public static String getSkeletonXPathBasedOnAttributeType(String attributeForXPathGeneration, String objectType) {
        if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_NID) || 
        		attributeForXPathGeneration.equalsIgnoreCase(SessionServiceResourceConst.BY_NID_ATTRIBUTE)) {
            return (".[@NID = '" + SessionServiceConst.NID_CONST + "']");
        } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_PARENT_DIR_NID)) {
            return ("//ParentMediaDirNID [.= '" + SessionServiceConst.NID_CONST + "']");
        } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_SESSION_CONFIG_NID)) {
            return ("//RecInfo/@SessionConfigNID = '" + SessionServiceConst.SESSION_CONFIG_NID_CONST + "'");
        } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_STREAM_TYPE)) {
            return ("//StreamType = '" + SessionServiceConst.STREAM_TYPE_CONST + "'");
        } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE)) {
            return ("//MediaStreamIOPortResourceNID/NIDValue = '" + SessionServiceConst.PORT_CONST + "'" );
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE)) {
	        return ("//MediaStreamDstPortResourceNID/NIDValue = '" + SessionServiceConst.PORT_CONST + "'" );
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.USERNAME)) {
	        return (".//Username = '" + SessionServiceConst.USER_NAME_CONST + "'" );	    	
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceResourceConst.TITLE)) {
   	        return (".//Title = '" + SessionServiceConst.TITLE_CONST + "'" );	    	
   	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_TYPE)) {
   	        return (".//Type = '" + SessionServiceConst.TYPE_CONST + "'" );
   	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_ASSET_NID)) {
            return (".[@NID = '" + SessionServiceConst.ASSET_NID_CONST + "']");
   	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.NAME)) {
   	    	return (".//Name = '" + SessionServiceConst.NAME_CONST + "'");
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_STATE)) {
   	    	return (".//StateFlag = '" + SessionServiceConst.STATE_CONST + "'");
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_MEDIA_GROUP_ID)) {
   	    	return (".//MediaGroupID = '" + SessionServiceConst.MEDIA_GROUP_ID_CONST + "'");
	    } else if (attributeForXPathGeneration.equalsIgnoreCase(SessionServiceConst.WHERE_OPERATION_MODE)) {
   	    	return (".//RemotelyOperatedMode = '" + SessionServiceConst.OPERATION_MODE_CONST + "'");
	    } 
            /*Should use matches(Info/Type , '1000') instead of contains. matches supports reg expression.
             * Contains only support string search*/
        return "";
    }

    public static String generateXPathForDateRange(String whereDateRange) {
    	Map<String, String> whereDateRangeMap = new HashMap<String, String>();
    			
    	try{
    		whereDateRangeMap = getInfoMapping(whereDateRange);
    	}catch(Exception e){
    		return "";
    	}
    	String fromDate = "";
    	String toDate = "";
    	String interval = "";

    	
    	
    	fromDate = whereDateRangeMap.get(SessionServiceConst.FROM_DATE);
    	toDate = whereDateRangeMap.get(SessionServiceConst.TO_DATE);
    	interval = whereDateRangeMap.get(SessionServiceConst.INTERVAL);


        Map<String, String> dateRangeMap = getDateRange(fromDate, toDate, interval);

        if (dateRangeMap == null) {
            return "";
        }

        String epochFromDate = dateRangeMap.get(SessionServiceConst.FROM_DATE);
        String epochToDate = dateRangeMap.get(SessionServiceConst.TO_DATE);

        if ((epochFromDate == null) || (epochToDate == null)) {
            return "";
        }

        String xpath = "//DateCreated[. >= " + epochFromDate + " and . <= " + epochToDate + "]";

        return xpath;
    }

    public static Map<String, String> getDateRange(String strFromDate, String strToDate, String strInterval) {
    	if (((strFromDate == null) || strFromDate.isEmpty()) && 
            ((strToDate == null) || strToDate.isEmpty()) && 
            ((strInterval == null) || (strInterval.isEmpty()))) {
    		return null;
        }

        long epochFromDate = 0;
        long epochToDate = 0;
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSSS Z");

        if ((strFromDate != null) && (strToDate != null)) {
            try {
                Date fromDate = format.parse(strFromDate.replaceAll("([\\+\\-]\\d\\d):(\\d\\d)","$1$2"));
                System.out.println(fromDate);
                epochFromDate = fromDate.getTime();

                Date toDate = format.parse(strToDate.replaceAll("([\\+\\-]\\d\\d):(\\d\\d)","$1$2"));
                System.out.println(toDate);
                epochToDate = toDate.getTime();
            } catch (Exception e) {
                e.printStackTrace();

                return null;
            }
        } else if ((strFromDate != null) && (strInterval != null)) {
            try {
                Date fromDate = format.parse(strFromDate);
                System.out.println(fromDate);
                epochFromDate = fromDate.getTime();
                epochToDate = calculateDate(fromDate, strInterval, "+");
            } catch (Exception e) {
                e.printStackTrace();

                return null;
            }
        } else if ((strToDate != null) && (strInterval != null)) {
            try {
                Date toDate = format.parse(strToDate);
                epochToDate = toDate.getTime();
                epochFromDate = calculateDate(toDate, strInterval, "-");
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            System.out.println("Invalid date/time information");

            return null;
        }

        if ((epochFromDate == 0) || (epochToDate == 0)) {
            return null;
        }

        Map<String, String> dateRangeMap = new HashMap<String, String>();
        dateRangeMap.put(SessionServiceConst.FROM_DATE, String.valueOf(epochFromDate));
        dateRangeMap.put(SessionServiceConst.TO_DATE, String.valueOf(epochToDate));

        return dateRangeMap;
    }

    public static long calculateDate(Date date, String strInterval, String operation) {
        int interval = Integer.valueOf(strInterval.substring(0, strInterval.length() - 1));

        if (operation.equalsIgnoreCase("-")) {
            interval = -interval;
        }

        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);

        if (strInterval.contains("Y")) {
            calendar.add(Calendar.YEAR, interval);
        } else if (strInterval.contains("M")) {
            calendar.add(Calendar.MONTH, interval);
        } else if (strInterval.contains("D")) {
            calendar.add(Calendar.DAY_OF_MONTH, interval);
        } else if (strInterval.contains("H")) {
            calendar.add(Calendar.HOUR_OF_DAY, interval);
        } else if (strInterval.contains("m")) {
            calendar.add(Calendar.MINUTE, interval);
        } else if (strInterval.contains("S")) {
            calendar.add(Calendar.SECOND, interval);
        } else if (strInterval.contains("s")) {
            calendar.add(Calendar.MILLISECOND, interval);
        }

        return calendar.getTimeInMillis();
    }
    
    public static void subscribeUser(Element requestElement, Document portDoc, String objectType, MRRequest request, ServiceAgentFactoryI serviceAgentFactory, String topic) throws Exception {
		String subscriptionId  = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.SUBSCRIPTION_ID,"");
		if(!subscriptionId.isEmpty()) {
			List<String> nidList = MRXMLUtils.getValuesXpath(portDoc, "//" + objectType + "/@NID");			
			AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).updateSubscriptions(request, nidList, subscriptionId, topic.toUpperCase());
		}
	}
    
    public static void subscribeUser(Element requestElement, Document resultDoc, String objectType, MRRequest request, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	String selectFormatType = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.SELECTFORMAT, "");
		GetObjectUtils.subscribeUser(requestElement, resultDoc, objectType, request, serviceAgentFactory, selectFormatType);	
	}

    public static void subscribeUser(String subscriptionId, Document portDoc, String objectType, MRRequest request, ServiceAgentFactoryI serviceAgentFactory, String topic) throws Exception {
		if(!subscriptionId.isEmpty()) {
			List<String> nidList = MRXMLUtils.getValuesXpath(portDoc, "//" + objectType + "/@NID");			
			AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).updateSubscriptions(request, nidList, subscriptionId, topic.toUpperCase());
		}
	}
    
    /**
     * @param whereNID (required - optional only if whereTitle is not empty)
     * @param whereTitle (required - optional only if whereNID is not empty)
     * @param objectType (required - get object based on whereTitle. 
     * 					  optional - for whereNID and NATIVE object retrieval. 
     * 					  If empty then would do a get call on native objects, currently only PortQuery accepts 'NATIVE' enum)
     * @param childXPath (required - for get call on NATIVE objects
     * 					  optional - in case of realList
     * @param arrWhereXPath (optional - in case of readList or readElement)
     * @param xmlFilePath (required - for get call on NATIVE objects)
     * @param handlerFilePath (required - for get call on NATIVE objects)  
     */
    
    public static Document getObjectXMLFromDB(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject, String xmlFilePath, String requestHandlerFilePath) throws Exception {		
    	
    	String objectType = requestObject.getObjectType();
    	String whereNID = requestObject.getNID();
    	String whereAttrValue = requestObject.getAttributeValue();
    	
    	if (whereAttrValue.isEmpty() && objectType.equals(SessionServiceConst.USER)) {        					
			String userJID = request.getUserJID();
			whereAttrValue = userJID.substring(0, userJID.indexOf("@"));	        						    	
    	}
    	
    	Document objectXMLDoc = null;
    	if (whereNID.isEmpty() && whereAttrValue.isEmpty()) {
    		throw new MRException(SessionServiceExceptionCodes.NID_AND_TITLE_MISSING, "Missing NID and title");
    	} else if (!whereNID.isEmpty()) {
			MRRequestI mrRequest = request.clone();
			objectXMLDoc = MRXMLDBUtils.readElement(mrRequest, log, db, whereNID);
		} else if (!objectType.isEmpty()) {				
			List<String> listOfWhereXpath = new ArrayList<String>();			        	
			listOfWhereXpath = requestObject.generateXPathBasedOnAttributeType(listOfWhereXpath, objectType, 
					requestObject.getAttributeType(), whereAttrValue, requestObject.getAttributeConstant());
	    	String[] arrWhereXPath = Arrays.copyOf(listOfWhereXpath.toArray(), listOfWhereXpath.size(), String[].class);			
			Element dbResponseEl = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType, whereNID, "", arrWhereXPath, null, "", "", null, 0, -1);
			
			// MIYE - TBD See how this works in the mediaroom code 
			if((MRXMLUtils.getListXpath(dbResponseEl, "//" + objectType)).size() > 1) {
			// if(MRXMLUtils.getList(dbResponseEl).size() > 1 ) { 
				throw new MRException(SessionServiceExceptionCodes.MATCHED_MULTIPLE_RECORDS, "Matched multiple records");
			}
			MRConfig config = MRConfig.getInstance(request.getServiceName(), objectType, log);
	        String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));                	            	    									
	        Element objectElement = MRXMLDBUtils.readElement(1, null, arrWhereXPath, rootNID, 1, null, null, request, db, log);
	        if(objectElement == null)
	        	throw new MRException(MRExceptionCodes.NODENOTFOUND, MRXMLUtils.generateErrorXML("nid", whereAttrValue));
	        objectXMLDoc = MRXMLUtils.elementToDocument(objectElement);
    	} else {			
    		String parentElName = requestObject.getRequestName();	
			Document getResponseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					requestObject, SessionServiceConst.SELECTFORMAT_NATIVE, xmlFilePath, requestHandlerFilePath);										
			objectXMLDoc = MRXMLUtils.getDocumentXpath(getResponseDoc, "//" + parentElName + "/*");	
			if(objectXMLDoc == null){
	        	throw new MRException(SessionServiceExceptionCodes.OTHER, "Title " +whereAttrValue + " not found !");
			}
		}
		
		return objectXMLDoc;		
	}
    
	    
    public static Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
    		RequestProcessor requestObject, String selectFormat, String xmlFilePath, String requestHandlerFilePath) throws Exception {
		
    	int beginIndex = xmlFilePath.indexOf("Get");
    	int endIndex = xmlFilePath.indexOf("Request");    	    	    
    	String xpath = "//" + xmlFilePath.substring(beginIndex, endIndex);
    	
		Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);		
		Element requestEl = MRXMLUtils.getElementXpath(requestDoc, xpath);
		
		String whereNID = requestObject.getNID();
		String whereAttrValue = requestObject.getAttributeValue();
		
		// Set default values		
		if (whereNID.isEmpty()) {
			String whereAttrType = requestObject.getAttributeType();
			if (whereAttrType.equals(SessionServiceResourceConst.TITLE)) {
				whereAttrType = SessionServiceConst.WHERE_TEXT_SUB_STR;
				whereAttrValue = "^" + whereAttrValue + "$"; // Matches the exact string				
			} else {
				whereAttrType = "where" + whereAttrType;
			}
			
			MRXMLUtils.setAttributeValue(requestEl, whereAttrType, whereAttrValue);
		} else {
			MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.WHERE_NID, whereNID);
		}		
		MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.SELECTFORMAT, selectFormat);
								
		Class<?> classObject = Class.forName(requestHandlerFilePath);
		Object handleObject = classObject.newInstance();
		
		String className = classObject.getName();
		String requestNameOfGetObject = MRRequest.getRequestName(className);
		Document getRequestDoc = SessionServiceUtils.getRequestXML(requestNameOfGetObject, log, requestEl);			
		MRRequest getRequest = MRRequest.createRequest(request, getRequestDoc, requestNameOfGetObject, request.getServiceName());	

		MRRequestHandlerI requestHandlerObject = (MRRequestHandlerI) handleObject;
		Document getObjectResponseDoc = requestHandlerObject.getResponse(getRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		// Generate response corresponding to the request handler		         				
		Element responseEl = MRXMLUtils.newElement(requestObject.getRequestName());
		Element objectListParentEl = MRXMLUtils.getElementXpath(getObjectResponseDoc, xpath);
		
		return request.createResponseData(MRXMLUtils.addChildrenToElement(responseEl, objectListParentEl));
					
	}
    
    public static Element getdstNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String dstTitle) throws Exception {
    	//String dstTitle = MRXMLUtils.getAttributeValue(getObjectElement, SessionServiceConst.DST_PORT_TITLE, "");
    	
    	//if dest title is empty user logged in resource title
    	if(dstTitle.isEmpty()) {
    		try {
				dstTitle = ServiceAgentUtils.getAgentName(request.getUserJID());
			} catch (Exception e) {
				return null;
			}    		
    	}
    	
    	if(dstTitle.isEmpty()) {
    		return null;
    	}
    	
    	Document getPortRequestDoc = MRXMLUtils.stringToDocument("<GetPort type='DstPort' whereTextSubStr='" + dstTitle + "'/>");
    	MRRequest getPortRequest = MRRequest.createRequest(request, getPortRequestDoc, "GetPortResponse", request.getServiceName());
    	GetPortRequestHandler handler = new GetPortRequestHandler();
    	Document response = handler.getResponse(getPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	return MRXMLUtils.getElementXpath(response, "//Port");
    }
    
}
