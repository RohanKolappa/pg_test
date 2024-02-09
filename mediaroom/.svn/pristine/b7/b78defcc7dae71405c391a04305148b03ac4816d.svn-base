package com.ipvs.mediaroomservice.handler;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AssetManagementRequestHandler;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.policyadminservice.handler.MediaRoomPolicyHelper;
import com.ipvs.policyadminservice.impl.GetRelayProfileLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;
import com.ipvs.utils.Utils;


public class GetRelayListRequestHandler implements MRRequestHandlerI {
    private int callCounter = 0;
    private int maxIteration = 0;

    //This Cache is for existing from loop
    private Hashtable<String, Element> sourceRelayNIDCache = null;
    private Hashtable<String, String> sourceAssetNIDCache = null;
  
    //Maintain the list of relays and profile corresponding to xpath
    private LinkedHashMap<Element, Element> sourceRelayProfileList = null;
    private LinkedHashMap<Element, Element> destRelayProfileList = null;
    private LinkedHashMap<Element, Element> noRelayProfileList = null;
    private LinkedHashMap<Element, Element> allCheckList = null;

    //Maintain the list of connection path from source to any destination
    private LinkedHashMap<Element, Map<Integer, Element>> srcRelayPathList = new LinkedHashMap<Element, Map<Integer, Element>>();

    //Maintain the list of single path from src to last src relay
    private LinkedHashMap<Integer, Element> srcRelayPath = new LinkedHashMap<Integer, Element>();

    @Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    @Override
    @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
    	
		// request.setCache(new MRRequestCache());

        try {
            //perf-logs log.addInfo("GetRelayList:" + request.getUserJID()) + " getResponse - Start", MRLog.OK, MRLog.NOTIFY);           
            Document requestDoc = request.getMessage().getDocument();
            String sourceNID = null;
            Element srcElement = MRXMLUtils.getElementXpath(requestDoc, "//sourceNID");
            Element destElement = MRXMLUtils.getElementXpath(requestDoc, "//destNID");

            if (srcElement == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "sourceNID"));
            }

            if (destElement == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "destNID"));
            }
            String destNID = MRXMLUtils.getValue(destElement);
            sourceNID = MRXMLUtils.getValue(srcElement);
            Document policyRequestDoc = GetRelayProfileLib.createRequestDoc(destNID, sourceNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            sourceRelayNIDCache = new Hashtable<String, Element>();
            sourceAssetNIDCache = new Hashtable<String, String>();
        
            sourceRelayProfileList = new LinkedHashMap<Element, Element>();
            destRelayProfileList = new LinkedHashMap<Element, Element>();
            noRelayProfileList = new LinkedHashMap<Element, Element>();
            allCheckList = new LinkedHashMap<Element, Element>();

            Document relayResponseDoc = null;
            Document sourceDoc = MRXMLDBUtils.readElement(request, log, db, sourceNID);
            String streamType = getStreamType(sourceDoc);
            getRelayProfileList(policyRequestDoc, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, PolicyAdminConstants.DEST_RELAY, destNID);
            String relayType = PolicyAdminConstants.DEST_RELAY;
            
            //if destination relay is empty check for src relay	
            if (destRelayProfileList.isEmpty()) {               
                request.setSuperUser(true);
                getRelayProfileList(policyRequestDoc, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, PolicyAdminConstants.SRC_RELAY, destNID);

                if (!sourceRelayProfileList.isEmpty()) {
                    boolean isSourceAndDestRelay = false;
                    LinkedHashMap<Element, Element> newSrcRelayProfileList = null;
                    
                    String destinationAssetResourceNID  = MRXMLUtils.getValueXpath(policyRequestDoc, "//Destination//AssetResourceNID", "");
                    //check source relay is same as destination relay by comparing asset resource NID or destination nid                    
                    String destinationRelayNID = destNID;
                     
                    if(sourceAssetNIDCache.containsKey(destinationAssetResourceNID)) {
                		destinationRelayNID = sourceAssetNIDCache.get(destinationAssetResourceNID);
                	}
                   
                    if (sourceRelayNIDCache.containsKey(destinationRelayNID)) {
                        newSrcRelayProfileList = getRelayAndProfileIfSourceRelayContainesTheDest(destinationRelayNID, streamType);
                        isSourceAndDestRelay = true;
                    } else {
                        newSrcRelayProfileList = (LinkedHashMap<Element, Element>) sourceRelayProfileList.clone();
                    }

                    getRelayAndProfileFromSrcRelayList(policyRequestDoc, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, isSourceAndDestRelay, newSrcRelayProfileList, destinationRelayNID);

                    sourceRelayProfileList = (LinkedHashMap<Element, Element>) newSrcRelayProfileList.clone();
                    relayType = PolicyAdminConstants.SRC_RELAY;
                }
            }

           
            //if src relay and destination relay is empty then go for direct connection
            if (destRelayProfileList.isEmpty() && sourceRelayProfileList.isEmpty()) {
                getRelayProfileList(policyRequestDoc, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, PolicyAdminConstants.NO_RELAY, destNID);
                relayType = PolicyAdminConstants.NO_RELAY;
            }
            
            //if no dest relay ,src relay and direct connection then go for all check
            if(destRelayProfileList.isEmpty() && sourceRelayProfileList.isEmpty() && noRelayProfileList.isEmpty()) {
            	getRelayProfileList(policyRequestDoc, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, PolicyAdminConstants.ALL_CHECK, destNID);
            	relayType = PolicyAdminConstants.ALL_CHECK;
            }
            
            relayResponseDoc = generateRelayListResponseDoc(relayType, request, sourceDoc, db, log,serviceAgentFactory);

            return relayResponseDoc;
        } catch (MRException e) {
            e.printStackTrace();
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.PEM_GET_FAILED, exp.toString());
        }
    }

    //update the src relay list if the dest relay and src relay both are equal
    private LinkedHashMap<Element, Element> getRelayAndProfileIfSourceRelayContainesTheDest(String destNID, String streamType)
        throws MRException, Exception {
        Set<Element> keySet = sourceRelayProfileList.keySet();
        Iterator<Element> keySetIT = keySet.iterator();
        Element relayProfileEL = null;
        Element xPathEL = null;
        Element relayEL = null;
        Element relayResponseEL = null;

        LinkedHashMap<Element, Element> newSrcRelayProfileList = new LinkedHashMap<Element, Element>();

        while (keySetIT.hasNext()) {
            xPathEL = keySetIT.next();
            relayProfileEL = (Element) sourceRelayProfileList.get(xPathEL).clone();
            relayEL = getRelayElementFromRelayResponse(MRXMLUtils.getElementXpath(relayProfileEL, "Relay"), destNID);

            if (relayEL != null) {
                srcRelayPath = (LinkedHashMap<Integer, Element>) srcRelayPathList.get(xPathEL);

                if (srcRelayPath.size() > 1) {
                    relayResponseEL = srcRelayPath.get(srcRelayPath.size() - 1);
                    relayProfileEL = generateRelayProfileElement(MRXMLUtils.elementToDocument(relayResponseEL), streamType, MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(relayProfileEL, "Profile")));
                } else {
                    Document doc =MRXMLUtils.getDocument();
                    doc.add(MRXMLUtils.newElement("Relay"));
                    relayProfileEL = generateRelayProfileElement(doc, streamType, MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(relayProfileEL, "Profile")));
                }
            }

            newSrcRelayProfileList.put(xPathEL, relayProfileEL);
        }

        return newSrcRelayProfileList;
    }

    //Get the relay element for give relay NID and relay List Doc
    private Element getRelayElementFromRelayResponse(Element relayResponseEL, String destNID)
        throws MRException, Exception {
        Element relayEL = null;
        String xPath = "//MediaStreamRelayPortResource[@NID='" + destNID + "']";
        relayEL = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(relayResponseEL), xPath);

        return relayEL;
    }

    //find out is there any direct connection exists between last src relay list and dest when the dest relay is empty
    private void getRelayAndProfileFromSrcRelayList(Document policyRequestDoc, String streamType, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, boolean isSourceAndDestRelay,
        LinkedHashMap<Element, Element> newSrcRelayProfileList, String destNID)
        throws MRException, Exception {
        Set<Element> keySet = sourceRelayProfileList.keySet();
        Iterator<Element> keySetIT = keySet.iterator();
        Element relayProfileEL = null;
        Element xPathEL = null;
        Element relayEL = null;
        Element userEL = null;
        Document policyRequestDocClone =  (Document)policyRequestDoc.clone();
        Element sourceResourceEL = MRXMLUtils.getElementXpath(policyRequestDocClone, "//Source/Resource");
        Element sourceUserEL = MRXMLUtils.getElementXpath(policyRequestDocClone, "//Source/User");
      
        Iterator<Element> noRelayIT = null;
        Element profileEL = null;
        Element relayResponseEL = null;

        while (keySetIT.hasNext()) {
            xPathEL = keySetIT.next();
            relayProfileEL = sourceRelayProfileList.get(xPathEL);
            relayEL = MRXMLUtils.getElementXpath(relayProfileEL, "Relay/MediaStreamRelayPortResource");
            relayResponseEL = getRelayElementFromRelayResponse(relayProfileEL, destNID);

            if (relayResponseEL != null) {
                continue;
            }

            // no user for relay port
            userEL = MRXMLUtils.stringToElement("<User/>");

            MRXMLUtils.removeContent(sourceUserEL);
            MRXMLUtils.removeContent(sourceResourceEL);
            MRXMLUtils.copyElement(sourceUserEL,userEL);
            MRXMLUtils.copyElement(sourceResourceEL,relayEL); 
            
            request.setSuperUser(true);

            getRelayProfileList(policyRequestDocClone, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, PolicyAdminConstants.NO_RELAY, destNID);

            if (noRelayProfileList.isEmpty()) {
                newSrcRelayProfileList.remove(xPathEL);
            } else {
                noRelayIT = noRelayProfileList.keySet().iterator();

                while (noRelayIT.hasNext()) {
                    profileEL = noRelayProfileList.get(noRelayIT.next());
                    relayProfileEL = generateRelayProfileElement(MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(relayProfileEL, "Relay")), streamType, MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(profileEL, "Profile")));
                    newSrcRelayProfileList.put(xPathEL, relayProfileEL);
                }
            }
        }
    }

    // Generate the response document
    private Document generateRelayListResponseDoc(String relayType, MRRequest request, Document sourceDoc, MRDB db, MRLog log,ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Set<Element> keySet = null;
        Iterator<Element> keySetIT = null;
        Element keyEL = null;
        Element relayListResponse = MRXMLUtils.newElement("RelayList");
        Element relayProfileEL = null;
        Document relayResponse = null;

        Element relayResponseEl = MRXMLUtils.newElement("Relay");
        Element profileResponseEl = MRXMLUtils.newElement("Profile");

        relayResponse = MRXMLUtils.stringToDocument("<" + request.getResponseDataName() + ">" + "</" + request.getResponseDataName() + ">");

        Element responseXML = MRXMLUtils.newElement("ResponseXML");

        switch (PolicyAdminConstants.RelayTypeList.valueOf(relayType)) {
        case DestinationRelay:
            keySet = destRelayProfileList.keySet();

            break;

        case SourceRelay:
            keySet = sourceRelayProfileList.keySet();

            break;

        case NoRelay:
            keySet = noRelayProfileList.keySet();

            break;
            
        case ALLCheck:
        	
        	keySet = allCheckList.keySet();
        	
        	if (keySet.isEmpty()) {
                relayProfileEL = MRXMLUtils.newElement("RelayProfile");
                MRXMLUtils.addElement(relayProfileEL, relayResponseEl);
                MRXMLUtils.addElement(relayProfileEL, profileResponseEl);
                MRXMLUtils.addElement(relayListResponse, relayProfileEL);
            }
        	
        	break;
		default:
			break;
        }
        String sourceType = MediaRoomServiceConst.INPUT_ENCODER;
        if(sourceDoc.getRootElement().getName().equals(AMConst.MEDIA_STORE_PORT_RESOURCE)) {
        	sourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
        }
     
        if (keySet != null) {
            keySetIT = keySet.iterator();

            while (keySetIT.hasNext()) {
                keyEL = keySetIT.next();

                switch (PolicyAdminConstants.RelayTypeList.valueOf(relayType)) {
                case DestinationRelay:
                    relayProfileEL = destRelayProfileList.get(keyEL);

                    break;

                case SourceRelay:
                    relayProfileEL = sourceRelayProfileList.get(keyEL);

                    break;

                case NoRelay:
                    relayProfileEL = noRelayProfileList.get(keyEL);
                    
                    break;
                case ALLCheck:
                	relayProfileEL = allCheckList.get(keyEL);
				default:
					break;
                }

                relayProfileEL = mergeSourceProfileWithPolicyProfile(relayProfileEL, sourceDoc, sourceType, request, db, log,serviceAgentFactory);
                MRXMLUtils.addElement(relayListResponse, relayProfileEL);
            }
        }

        MRXMLUtils.addElement(responseXML,(Element)relayListResponse.detach());
        MRXMLUtils.addElement(relayResponse.getRootElement(),responseXML);

        return relayResponse;
    }

    private String getStreamType(Document sourceDoc) throws MRException, Exception {
        String streamType = MRXMLUtils.getValueXpath(sourceDoc, "//StreamType", MediaRoomServiceConst.V2D);

        if (streamType.isEmpty()) {
            streamType = MediaRoomServiceConst.V2D;
        }

        return streamType;
    }

    private Element mergeSourceProfileWithPolicyProfile(Element relayProfileEL, Document sourceDoc, String sourceType, MRRequest request, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String streamType = getStreamType(sourceDoc);
        String sdptemplate = null;
        String streamURL = MRXMLUtils.getValueXpath(sourceDoc, "//MediaStreamURL", "");
        String streamProfileInfoXpath = "//MediaStreamProfileInfo/" + streamType + "MediaStreamProfileInfo";
        Element streamProfileInfoElement = MRXMLUtils.getElementXpath(sourceDoc, streamProfileInfoXpath);
        
        if (streamURL.isEmpty() && !sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
        	return relayProfileEL;
        }

        Element profileEL = MRXMLUtils.getElementXpath(relayProfileEL, "Profile");

        if ((streamProfileInfoElement != null) && !profileEL.elements().isEmpty()) {
            String streamProfileName = streamType + "MediaStreamProfile";
            if (streamType.equals(MediaRoomServiceConst.V2D)) {
                //TBD: Merge the policy profile with source profile.
                return relayProfileEL;
            } else if (MediaRoomServiceConst.RTP.equals(streamType)) {
              
            	String configureHLS = MRXMLUtils.getValueXpath(profileEL, ".//HLSConfig/Enable", "false");
                System.out.println("configureHLS :="+configureHLS);
                String shlsCapability = MRXMLUtils.getValueXpath(streamProfileInfoElement, ".//HLSConfig/Enable", "false");
                System.out.println("shlsCapability :="+shlsCapability);
                boolean bhlsCapability = Utils.getBooleanValue(shlsCapability, false);
                System.out.println("bhlsCapability :="+bhlsCapability);
               
                
                if (sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
                    sdptemplate = MRXMLUtils.getValueXpath(streamProfileInfoElement, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
                    sdptemplate = MediaRoomServiceUtils.decodeFromFileSDPTemplate(sdptemplate);
                } else if (sourceType.equals(MediaRoomServiceConst.INPUT_ENCODER)) {
                    
                    //add recorder config to streamProfileInfoElement from profileEL. 
                    String recorderConfig = streamType+"RecoderConfig"; //TODO change this once bug #2421(RecorderConfig spelling is not correct in Profile xmls) is fixed
                    Element recorderConfigEl = MRXMLUtils.getElementXpath(profileEL, ".//"+recorderConfig);
                    if(recorderConfigEl != null) {
                        MRXMLUtils.addElement(streamProfileInfoElement, (Element)recorderConfigEl.clone());
                    }
                    
                    //TBD: UI has to update the SDP Template value in source port resource.
                    sdptemplate = SDPUtils.getSDPTemplateFromSDP(streamURL, log);
                    //make sure global is enabled before adding hls true in profile
                    bhlsCapability = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getServiceConfigManager().isHLSEnabled() && SDPUtils.isSourceHLSCapable(streamURL, log);
                    System.out.println("bhlsCapability :="+bhlsCapability);
                    
                }
                
                MRXMLUtils.getElementXpath(streamProfileInfoElement, "./" + MediaRoomServiceConst.SDPTEMPLATE).setText(sdptemplate);
                String enableHLS = "false";
                if(configureHLS.equals("true") && bhlsCapability) {
                    enableHLS = "true";
                }
                MRXMLUtils.setValueXpath(streamProfileInfoElement, ".//HLSConfig/Enable", enableHLS);
           }

            streamProfileInfoElement = (Element) streamProfileInfoElement.clone();
            streamProfileInfoElement.setName(streamProfileName);
            MRXMLUtils.replaceContentWithElement(profileEL, streamProfileInfoElement);
        }
        
        return relayProfileEL;
    }

    //Get the Relay List from asset management handler
    private Document getRelayFromAssetManagement(Element policyResponse, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String whereXPath = "";
        Document relayResponse = null;

        String resourceXpath = MRXMLUtils.getValueXpath(policyResponse, "MediaStreamResourceXPath", null);

        if ((resourceXpath == null)) {
            return relayResponse;
        }

        String profileXpath = MRXMLUtils.getValueXpath(policyResponse, "ProfileXPath", null);

        if ((profileXpath == null)) {
            return relayResponse;
        }

        whereXPath = "";

        if(resourceXpath.isEmpty()) {
        	whereXPath = "//MediaStreamRelayPortResource/Info";
        } else {
        	whereXPath = "//MediaStreamRelayPortResource/Info[" + resourceXpath + "]";
        }

        relayResponse = MediaRoomServiceUtils.handleGetRequest(whereXPath, AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db,
                serviceAgentFactory, resourceFactory);

        if (MRXMLUtils.getElementXpath(relayResponse, "//MediaStreamRelayPortResource") == null) {
            return null;
        }

        return relayResponse;
    }

    //Get the Profile List from asset management handler
    private Document getProfileFromAssetManagement(Element policyResponse, String streamType, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String whereXPath = "";
        Document profileResponse = null;
       
        String profileXpath = MRXMLUtils.getValueXpath(policyResponse, "ProfileXPath", "");

        whereXPath = "";
        if(profileXpath.isEmpty()) {
        	whereXPath = "//" + streamType + "MediaStreamProfile";
        } else {
        	whereXPath = "//" + streamType + "MediaStreamProfile[" + profileXpath + "]";
        }
        String dbObjectType = streamType + AMConst.MEDIA_STREAM_PROFILE_OBJECTTYPE;
        Document requestdoc = AssetAdminServiceUtils.getAssetAdminRequest(log, dbObjectType + "_BASE_ALL_GET_LIST.xml");
        MRRequest assetRequest = MRRequest.createRequest(request,requestdoc , MRRequest.getRequestName(AssetManagementRequestHandler.class.getName()), ServiceClient.ASSETADMINSERVICE);
        MRXMLUtils.setValueXpath(requestdoc, "//SortByXpath",  "$VAR//Priority");
        
        
        profileResponse = MediaRoomServiceUtils.handleGetRequest(whereXPath, dbObjectType , ServiceClient.ASSETADMINSERVICE, assetRequest, xmpp, log, db,
                serviceAgentFactory, resourceFactory);        
     
        return profileResponse;
    }

    //Parse the relay profile xpath and save the list in cache
    private Document getRelayProfile(Document policyRequestDoc, Document policyResponse, String streamType, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String destNID)
        throws MRException, Exception {
        List<?> xpathList = MRXMLUtils.getListXpath(policyResponse, "//XPath");
        policyResponse.getRootElement().elements("XPath");

        Element relayProfileEL = MRXMLUtils.newElement("RelayProfile");
        Document relayResponseDoc = null;
        String relayType = MRXMLUtils.getValueXpath(policyResponse, "//RelayType", null);
        Element xPathEL = null;
        Element relayEL = null;

        String relayNID = null;
        Document policyRequestDocClone = (Document)policyRequestDoc.clone();
        Element sourceResourceeEL = MRXMLUtils.getElementXpath(policyRequestDocClone, "//Source/Resource");
        Element sourceUserEL = MRXMLUtils.getElementXpath(policyRequestDocClone, "//Source/User");
        LinkedHashMap<String, Element> destNIDList = null;
        LinkedHashMap<String, String> destinationAssetNIDList = new LinkedHashMap<String, String>();

        for (int i = 0; (xpathList != null) && (i < xpathList.size()); i++) {
            xPathEL = (Element) xpathList.get(i);
            relayResponseDoc = getRelayFromAssetManagement(xPathEL, request, xmpp, db, log, resourceFactory, serviceAgentFactory);

            if ((relayResponseDoc == null) && (!relayType.equals(PolicyAdminConstants.NO_RELAY) && !relayType.equals(PolicyAdminConstants.ALL_CHECK))) {
                continue;
            }

            relayEL = null;
            relayNID = null;

            Element userEL = null;

            if (relayResponseDoc != null) {
                relayEL = MRXMLUtils.getElementXpath(relayResponseDoc, "//MediaStreamRelayPortResource");
            }

            switch (PolicyAdminConstants.RelayTypeList.valueOf(relayType)) {
            case SourceRelay:
                relayNID = MRXMLUtils.getAttributeValue(relayEL, "NID", "");

                if ((relayNID == null) || (relayNID.trim().length() == 0) || sourceRelayNIDCache.containsKey(relayNID)) {
                    continue;
                }

                sourceRelayNIDCache.putAll(getRelayNIDFromResponseDoc(relayResponseDoc));
                sourceAssetNIDCache.putAll(getAssetNIDFromResponseDoc(relayResponseDoc));
                // sourceRelayNIDCache.put(relayNID, relayEL);
               
                MRXMLUtils.removeContent(sourceResourceeEL);
                MRXMLUtils.removeContent(sourceUserEL);
                
                // no User for relay port
                userEL = MRXMLUtils.stringToElement("<User/>");
               
                MRXMLUtils.copyElement(sourceUserEL,userEL);
                MRXMLUtils.copyElement(sourceResourceeEL,relayEL);
                
                request.setSuperUser(true);
                maxIteration++;
                callCounter++;

                if (callCounter == 1) {
                    srcRelayPath = new LinkedHashMap<Integer, Element>();
                }

                srcRelayPath.put(callCounter, generateRelayElement(relayResponseDoc));
                getRelayProfileList( policyRequestDocClone, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, relayType, destNID);
                callCounter--;

                if (callCounter == (maxIteration - 1)) {
                    relayProfileEL = generateRelayProfileElement(relayResponseDoc, xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);

                    if (relayProfileEL == null) {
                        continue;
                    }

                    sourceRelayProfileList.put(xPathEL, relayProfileEL);
                    srcRelayPathList.put(xPathEL, srcRelayPath);
                }

                break;

            case DestinationRelay:

                if (relayEL == null) {
                    continue;
                }

                destNIDList = getRelayNIDFromResponseDoc(relayResponseDoc);
                destinationAssetNIDList = getAssetNIDFromResponseDoc(relayResponseDoc);
                String destinationAssetResourceNID  = MRXMLUtils.getValueXpath(policyRequestDoc, "//Destination//AssetResourceNID", "");
                if (destNIDList.containsKey(destNID) || destinationAssetNIDList.containsKey(destinationAssetResourceNID)) {
                    continue;
                }

                relayProfileEL = generateRelayProfileElement(relayResponseDoc, xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);

                if (relayProfileEL == null) {
                    continue;
                }

                destRelayProfileList.put(xPathEL, relayProfileEL);

                break;

            case Both:
                break;

            case NoRelay:
                Document doc = MRXMLUtils.getDocument();
                MRXMLUtils.addElement(doc, "Relay");
                //relayProfileEL = generateRelayProfileElement(MRXMLUtils.getDocument().add(MRXMLUtils.newElement("Relay")), xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);
                relayProfileEL = generateRelayProfileElement( doc, xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);

                if (relayProfileEL == null) {
                    continue;
                }

                noRelayProfileList.put(xPathEL, relayProfileEL);

                break;
            	
            case ALLCheck:
            	//if no relay then check profile exist or not
            	if (relayEL == null) {
            		 relayResponseDoc = MRXMLUtils.getDocument();
                     MRXMLUtils.addElement(relayResponseDoc, "Relay");
                }

            	//if relay present and make sure its not same as destination
                destNIDList = getRelayNIDFromResponseDoc(relayResponseDoc);

                if (destNIDList.containsKey(destNID)) {
                	relayResponseDoc = MRXMLUtils.getDocument();
                    MRXMLUtils.addElement(relayResponseDoc, "Relay");
                }
                
                // find profile exist or not
                relayProfileEL = generateRelayProfileElement(relayResponseDoc, xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);

                if (relayProfileEL == null) {
                    continue;
                }   
                
                allCheckList.put(xPathEL, relayProfileEL);

                break;

            default:
                break;
            }
        }

        return relayResponseDoc;
    }

    private LinkedHashMap<String, Element> getRelayNIDFromResponseDoc(Document relayResponseDoc)
        throws MRException, Exception {
        if (relayResponseDoc == null) {
            return null;
        }

        Element relayEL = null;
        List<?> relayELList = MRXMLUtils.getListXpath(relayResponseDoc, "//MediaStreamRelayPortResource");
        Iterator<?> relayELListIT = relayELList.iterator();
        LinkedHashMap<String, Element> relayNIDList = new LinkedHashMap<String, Element>();

        while (relayELListIT.hasNext()) {
            relayEL = (Element) relayELListIT.next();
            relayNIDList.put(MRXMLUtils.getAttributeValue(relayEL, "NID", ""), relayEL);
        }

        return relayNIDList;
    }
    
    private LinkedHashMap<String, String> getAssetNIDFromResponseDoc(Document relayResponseDoc)
    throws MRException, Exception {
    if (relayResponseDoc == null) {
        return null;
    }

    Element relayEL = null;
    List<?> relayELList = MRXMLUtils.getListXpath(relayResponseDoc, "//MediaStreamRelayPortResource");
    Iterator<?> relayELListIT = relayELList.iterator();
    LinkedHashMap<String, String> assetNIDList = new LinkedHashMap<String, String>();

    while (relayELListIT.hasNext()) {
        relayEL = (Element) relayELListIT.next();
        assetNIDList.put(MRXMLUtils.getValueXpath(relayEL, ".//AssetResourceNID", ""),MRXMLUtils.getAttributeValue(relayEL, "NID", ""));
    }

    return assetNIDList;
}

    //Get the profile xml and generate the response XML
    private Element generateRelayProfileElement(Document relayResponseDoc, Element xPathEL, String streamType, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Element profileEL = null;

        Document profileResponse = getProfileFromAssetManagement(xPathEL, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);
        profileEL = MRXMLUtils.getElementXpath(profileResponse, "//" + streamType + "MediaStreamProfile");

        if (profileEL == null) {
            return null;
        }

        return generateRelayProfileElement(relayResponseDoc, streamType, profileResponse);
    }

    //Generate the relay Element from relayResponseDoc
    private Element generateRelayElement(Document relayResponseDoc) throws MRException, Exception {
        Element relayResponseEl = MRXMLUtils.newElement("Relay");

        MRXMLUtils.addElementsFromDocument(relayResponseEl, relayResponseDoc, "//MediaStreamRelayPortResource");

        return relayResponseEl;
    }

    //Generate the relay Profile response XML from relay and profile response doc
    private Element generateRelayProfileElement(Document relayResponseDoc, String streamType, Document profileResponse)
        throws MRException, Exception {
        Element relayResponseEl = MRXMLUtils.newElement("Relay");
        Element profileResponseEl = MRXMLUtils.newElement("Profile");

        Element relayProfileEL = MRXMLUtils.newElement("RelayProfile");

        MRXMLUtils.addElementsFromDocument(relayResponseEl, relayResponseDoc, "//MediaStreamRelayPortResource");
        MRXMLUtils.addElementsFromDocument(profileResponseEl, profileResponse, "//" + streamType + "MediaStreamProfile");
        MRXMLUtils.addElement(relayProfileEL, relayResponseEl);
        MRXMLUtils.addElement(relayProfileEL, profileResponseEl);

        return relayProfileEL;
    }

    //get the PEM Xpath from policy admin
    private void getRelayProfileList(Document policyRequestDoc, String streamType, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String relayType, String destNID)
        throws MRException, Exception {
        log.addInfo(request.hashCode() + " GetRelayListRequestHandler:getRelayProfileList Begin" ,MRLog.OK, MRLog.NOTIFY);
        Document policyResponse = GetRelayProfileLib.getRelayProfileXPath(policyRequestDoc, relayType, request, xmpp, db, log, resourceFactory, serviceAgentFactory);
        log.addInfo(request.hashCode() + " GetRelayListRequestHandler:getRelayProfileList Got policy, Getting relay profile NIDs" ,MRLog.OK, MRLog.NOTIFY);
        // log.addInfo("Relay Policy Response :="+policyResponse.asXML(),MRLog.OK, MRLog.NOTIFY);
        getRelayProfile(policyRequestDoc, policyResponse, streamType, request, xmpp, db, log, resourceFactory, serviceAgentFactory, destNID);
        log.addInfo(request.hashCode() + " GetRelayListRequestHandler:getRelayProfileList End" ,MRLog.OK, MRLog.NOTIFY);
    }
    
}
