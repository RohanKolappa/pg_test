package com.ipvs.http.common;

import java.io.File;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.UserManager;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;

public class HttpAuthentication {
	
	public enum AuthScheme  { LOGIN, EXPIRY };
	public static final String INFINITE_EXPIRY = "INFINITE";
	
	public static String getAuthScheme(String expireTime) {
		if(expireTime ==null || expireTime.isEmpty()) {
			return AuthScheme.LOGIN.toString();
		}
		return AuthScheme.EXPIRY.toString();
	}
	
	public static String generateSignature(Element element, String timestamp,String authScheme,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		switch (AuthScheme.valueOf(authScheme)) {		
			case LOGIN: 
				    String signature =  generateLoginBasedSignature(request.getUserJID(),xmpp,log,db,serviceAgentFactory);
				    //if signature is null then return expire based signature. (Backward compatibility)
				    if(signature != null) {
				    	return signature;
				    }
			case EXPIRY: 
				    return generateExpiryBasedSignature(element, timestamp);				
			default: 
				throw new Exception("Invalid auth scheme");
		}
	}
	
	
	public static String generateLoginBasedSignature(String userJID,XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		String token = null;
		
		AuthTokenCache.UserResource userResource  = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).getUserResource(StringUtils.substringBefore(userJID, "@localhost"));
		
		//find user exists in XMPP cache if we do not find user in rest cache
		if(userResource == null) {
			UserManager userManager = UserManager.getInstance();	
			String resource = org.jivesoftware.smack.util.StringUtils.parseResource(userJID);

			if(resource == null|| resource.isEmpty()) {
				return null;
			}

			boolean available;
			try {
				available = userManager.checkJID(userJID, log, db, resource);
				if(!available) {
					return null;
				}	
			} catch (Exception e) {
				return null;
			}

			//for xmpp we are not setting any token using userjid as token			
			token =   userJID;

		} else {		

			token = userResource.getToken();
			
		}
		
		return generateSignature(userJID, token, AuthScheme.LOGIN.toString());		
		
	}

	public static String generateExpiryBasedSignature(Element element, String timestamp) throws Exception {
		String apiKey = MRXMLUtils.getAttributeValueXpath(element, ".//ApiKey", SessionServiceResourceConst.KEY_ATTRIBUTE, "");
		String sharedSecret = MRXMLUtils.getAttributeValueXpath(element, ".//ApiKey", SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE, "");
		return generateSignature(apiKey, sharedSecret, timestamp);
	}
	
	private  static String generateSignature(String apiKey,String sharedSecret, String timestamp) throws Exception {
		String input = apiKey + sharedSecret + timestamp;
		MessageDigest messageDigest = MessageDigest.getInstance("MD5");
		messageDigest.update(input.getBytes());
		String signature = String.format("%032x", new BigInteger(1, messageDigest.digest()));
		return signature;
		
	}
	
	// generate system parameter signature with API key and shared secret
	public static String generateSystemParamsSignature(Element element, Map<String,String> sparams) throws Exception {
		String sharedSecret = MRXMLUtils.getAttributeValueXpath(element, ".//ApiKey", SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE, "");
		List<String> sparamsList = new ArrayList<String>();
		for(String name:sparams.keySet()) {
			sparamsList.add(name+"="+sparams.get(name));
		}
			
		String input =  sharedSecret +";"+StringUtils.join(sparamsList, ",") ;
		MessageDigest messageDigest = MessageDigest.getInstance("MD5");
		messageDigest.update(input.getBytes());
		return String.format("%032x", new BigInteger(1, messageDigest.digest()));
	}
	
	public static void validateSignature(HttpServletRequest req, HttpServletResponse resp, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory,MRDB db, MRLog log, boolean validateTimeStamp) throws Exception {
		String userRequestExpireTimeStamp = req.getParameter(HttpConst.TIMESTAMP_PARAMETER);
		String authScheme = AuthScheme.EXPIRY.toString();
		if(userRequestExpireTimeStamp == null || userRequestExpireTimeStamp.isEmpty()) {
			authScheme = AuthScheme.LOGIN.toString();
		}
		
		switch(AuthScheme.valueOf(authScheme)) {
		case LOGIN:
			validateSignatureBasedOnLoginAuthScheme( req,  resp,  xmpp,  serviceAgentFactory,db, log);
			return;
		case EXPIRY:
			validateSignatureBasedOnExpiryAuthScheme( req, resp, xmpp, serviceAgentFactory, log, validateTimeStamp);
			default:				
		}
		
	}
	
	private static void validateSignatureBasedOnExpiryAuthScheme(HttpServletRequest req, HttpServletResponse resp, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRLog log,	boolean validateTimeStamp) throws Exception {
		String userRequestExpireTimeStamp = req.getParameter(HttpConst.TIMESTAMP_PARAMETER);		
		Element apiKeyEl = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getAPIKey();
		String userRequestAPIKey = req.getParameter(HttpConst.API_KEY_PARAMETER);

		String userRequestSignature = req.getParameter(HttpConst.SIGNATURE_PARAMETER);
		String appServerSharedSecret = MRXMLUtils.getAttributeValueXpath(apiKeyEl, ".//ApiKey[@key='"+userRequestAPIKey+"']", SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE,"");
		String signature = generateSignature(userRequestAPIKey, appServerSharedSecret, userRequestExpireTimeStamp);
		long currentTimeStamp = xmpp.getCurrentTimeMillis();
		if(!MessageDigest.isEqual(signature.getBytes(), userRequestSignature.getBytes()) ){
			throw new MRException(HttpServletResponse.SC_UNAUTHORIZED, " UnAuthorized URL");
		}
		
		if(validateTimeStamp && (!userRequestExpireTimeStamp.equals(INFINITE_EXPIRY) && currentTimeStamp >= (Long.parseLong(userRequestExpireTimeStamp)))){
			throw new MRException(HttpServletResponse.SC_UNAUTHORIZED, " UnAuthorized URL");
		}
	}

	private static void validateSignatureBasedOnLoginAuthScheme(HttpServletRequest req,	HttpServletResponse resp, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRDB db,MRLog log) throws Exception {
		String userJID = req.getParameter(HttpConst.USERJID_PARAMETER);	
		String userRequestSignature = req.getParameter(HttpConst.SIGNATURE_PARAMETER);
		String signature = generateLoginBasedSignature(userJID, xmpp, log, db, serviceAgentFactory);
		if(signature == null || !MessageDigest.isEqual(signature.getBytes(), userRequestSignature.getBytes())) {
			throw new MRException(HttpServletResponse.SC_UNAUTHORIZED, " UnAuthorized URL");
		}
	}			


	public static String generateThumbnailURL(String sourceNID, String type, String timestamp, String msIPAddress,String authScheme, MRLog log, ServiceAgentFactoryI serviceAgentFactory, 
			 MRRequest request,MRDB db,XMPPI xmpp,MRResourceFactoryI resourceFactory,String templateTitle) throws Exception {
			
			String apiKeyTable = "/var/status/APIKeyTable.xml";
			File file = new File(apiKeyTable);
			if (!file.exists()) {
				log.addInfo("APIKeyTable not found, Thumbnail URL will be Empty for NID: "+ sourceNID + MRExceptionCodes.XML_NOT_FOUND ,MRLog.ERROR,MRLog.NOTIFY);
				return "";
	        }
			Document apiKeyDoc;
	        try {
	        	apiKeyDoc = MRXMLUtils.getDocument(file);
	        } catch (Exception ex) {
	        	log.addInfo("APIKeyTable not valid Thumbnail URL will be Empty for NID: "+ sourceNID +MRExceptionCodes.XML_NOT_VALID ,MRLog.ERROR,MRLog.NOTIFY);
	        	return "";
	        }
	        String signature = HttpAuthentication.generateSignature(apiKeyDoc.getRootElement(), timestamp, authScheme, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	        		HttpAuthentication.generateExpiryBasedSignature(apiKeyDoc.getRootElement(), timestamp);
	        Element apikeyEl = MRXMLUtils.getElement(apiKeyDoc, "ApiKey");
	        String apiKey = MRXMLUtils.getAttributeValue(apikeyEl, "key");
	        String protocol = MRXMLUtils.getValueXpath(((MRRequest)request).getMessage().getDocument(), "//Header/Protocol", "http");
	        String publishURL = protocol + "://" + msIPAddress + "/mediajs/thumbnail?mediaID=" + sourceNID + "&type="+type ;
	        return appendSystemAuthenticationQueryParameters(publishURL, apiKey, timestamp, authScheme, signature, request.getUserJID(),templateTitle);
		}
	 
	 
	 public static String appendSystemAuthenticationQueryParameters(String url, String apikey,String timestamp,String authscheme,String signature,String userJID,String templateTitle)  {
		 
		 //do not append system parameters for component user		 
		 if(userJID.equals("managementserver.localhost")){
			 return url;
		 }
		  
		 String urlUserJID = userJID;
		 if(!templateTitle.isEmpty()) {
			 String username = org.jivesoftware.smack.util.StringUtils.parseName(userJID);
			 urlUserJID = username + "@localhost/" + templateTitle;
		 }
		 url += "&userJID=" + urlUserJID +"&signature=" + signature;
		 
		 switch(AuthScheme.valueOf(authscheme)) {
		 case EXPIRY:
			 return url += "&timestamp=" + timestamp + "&apiKey=" + apikey;
		 case LOGIN:
		 default:
			 return url;
		 }
	 }
	
}
