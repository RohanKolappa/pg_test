package com.ipvs.policyadminservice.utils;

import java.util.ArrayList;
import java.util.List;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AssetAdminPEMUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class PEMUtils {
    public static String[] getWhereXPathArray(String whereXPath, String PEMXPath) {
        List<String> whereXPathList = new ArrayList<String>();

        if ((whereXPath != null) && (whereXPath.length() != 0)) {
            if ((PEMXPath != null) && (PEMXPath.length() != 0)) {
                whereXPathList.add(PEMXPath);
                whereXPathList.add(whereXPath);
            } else {
                whereXPathList.add(whereXPath);
            }
        } else if ((PEMXPath != null) && (PEMXPath.length() != 0)) {
            whereXPathList.add(PEMXPath);
        }

        String[] whereXPathArray = null;

        if ((whereXPathList != null) && (whereXPathList.size() != 0)) {
            whereXPathArray = new String[whereXPathList.size()];
            //System.out.println("List %%%%%%%%%%%%%%"+whereXPathList);
            whereXPathArray = whereXPathList.toArray(whereXPathArray);

            //System.out.println("wherexPath Arrya %%%%%% " + whereXPathArray[0]);
        }

        return whereXPathArray;
    }

    

    public static String getPEMXPath(String PEMXPath, List<String> roleObject, String dbObjectName, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
    	
    	if(PEMXPath.equals(PolicyAdminConstants.NULL))
    		 return SessionServiceConst.DUMMY_NOT_XPATH;
       
    	if (roleObject.size()==1 && roleObject.contains(AMConst.Device)) {
        	PEMXPath = "//" + AMConst.AssetResource + PEMXPath; 
            return AssetAdminPEMUtils.generateAssetResourceXPath(PEMXPath, dbObjectName, serviceName, request, schema, log);
        }
    	
        if(roleObject.size()==1 && roleObject.contains(AMConst.MEDIA_STREAM_PROFILE_OBJECTTYPE)){
        	 return "//*"  + PEMXPath;
        }
      
        if (serviceName.equals(ServiceClient.POLICYADMINSERVICE) ||serviceName.equals(ServiceClient.ASSETADMINSERVICE) 
        		|| ServiceClient.isSessionServiceRequest(serviceName)){
            return "//" + dbObjectName + PEMXPath;
        }

        return PEMXPath;
    }
}
