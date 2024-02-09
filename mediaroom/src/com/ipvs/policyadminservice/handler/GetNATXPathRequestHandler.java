package com.ipvs.policyadminservice.handler;

import java.util.Hashtable;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.utils.FileUtils;


public class GetNATXPathRequestHandler implements MRRequestHandlerI {
	private static String NATXQUERY = "/com/ipvs/policyadminservice/xquery/NATXQuery.xml";

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID,
                streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    @Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	long dbtimeout = 1000;
    	Hashtable<String, String> table = new Hashtable<String, String>();
    	// Document doc = MRXMLUtils.loadXMLResource(GetNATXPathRequestHandler.NATXQUERY, log);
 	   	// String query = MRXMLUtils.getValueXpath(doc, "//NATXQuery", "");
 	   	// query = "for $d in index-scan('IPVSis_colnids_default', 'default.mediastreamfilterlist', 'EQ')/.. return $d";
 	   	// query = "for $d in index-scan('IPVSi_nids_default','default.tagobjectfilterlist.zzzz_1234465330250873f5e38', 'EQ')/.. return $d"; 
	String query = FileUtils.readFileResource(GetNATXPathRequestHandler.NATXQUERY, this.getClass());
 	   	String result = "";
 	   	try {
 	   		result = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
 	   	}
 	   	catch(Exception e) {
 	   		e.printStackTrace();
 	   		throw e;
 	   	}
    	return request.createResponseData("NATXPath", result);
    }
    
    /*
    public static void main(String[] args) throws Exception {
 	   	String xmlFilePath = "/com/ipvs/test/logfiles/NATXQuery.xml";
    	Document doc = MRXMLUtils.loadXMLResource(xmlFilePath, null);
    	System.out.println(MRXMLUtils.getValueXpath(doc, "//NATXQuery", ""));
    }
    */
}
