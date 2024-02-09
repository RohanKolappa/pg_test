package com.ipvs.sessionservice.handler.base;

import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.GetOffsetAtTimecodeRequestHandler;
import com.ipvs.mediaroomservice.handler.GetTimeCodeAtOffsetRequestHandler;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.session.handler.AddBookmarkRequestHandler;
import com.ipvs.sessionservice.session.handler.UpdateBookmarkRequestHandler;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public abstract class BookmarkBaseRequestHandler extends FileBaseRequestHandler implements MRRequestHandlerI {
   
		
	protected Document sendAddUpdateBookmarkRquestToDMS(Document bookmarkObjectDoc, String mediaStreamFileNID, String TC, String offset, Document streamDoc, String streamNID,
			String bookmarkFileNID, String trackName, MRRequest request,String requestName, MRLog log, XMPPI xmpp, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
		
		checkTimecodeAndOffset(offset, TC, requestName);

		String agentJID = "";
		String mediaUUID = "";
		String parentDirNID = "";
		Document mediaStreamFileDoc = null;
		String mediaGroupId="";
		Document queryResultDoc=null;
		String groupParentDirNID= "";
						
		//if stream found then check the stream is playback stream or live.
		// If source type is playback file then send message to media source
		// agent.
		// If it is a live stream then send message to immediate source agent.
		
		  if (streamDoc != null) {
              if (trackName.equals(MediaRoomServiceConst.RECORDTRACK)) {
                  agentJID = MRXMLUtils.getValueXpath(streamDoc, "//sourceAgentJID", "");
                  mediaUUID = MRXMLUtils.getValueXpath(streamDoc, "//destFileUUID", "");
                  parentDirNID = MRXMLUtils.getValueXpath(streamDoc, "//destFileParentDirNID", "");
              } else if (trackName.equals(MediaRoomServiceConst.STREAMTRACK)) {
                  String mediaSourceNID = MRXMLUtils.getValueXpath(streamDoc, "//mediaSourceNID", "");
                  String roomNID = MRXMLUtils.getValueXpath(streamDoc, "//roomNID", "");
                  Document mediaSrcDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
                  agentJID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceAgentJID", "");
                  mediaUUID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceFileUUID", "");
                  parentDirNID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceFileParentDirNID", "");
              }
          }
		  
		  //Checking if Media Group 
			if(bookmarkFileNID!=null && mediaStreamFileNID!=null  && !bookmarkFileNID.isEmpty() && !mediaStreamFileNID.isEmpty() && !bookmarkFileNID.equalsIgnoreCase(mediaStreamFileNID)){
				mediaStreamFileNID=bookmarkFileNID;
			}
		  
	      if (agentJID.isEmpty()) {
	    	  
	    	 String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.BOOKMARK_FILE_INFO + SessionServiceConst.XQUERYXML;
	         String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
	 		 query = query.replace("_XPATHAND_","'"+mediaStreamFileNID+"'");
	 		 request.setSuperUser(true);
	 		 queryResultDoc = GetObjectUtils.executeQuery(query,  AMConst.MEDIA_STREAM_FILE_RESOURCE, null, "","", request, xmpp, log, db, serviceAgentFactory, resourceFactory, true);
	 		 mediaStreamFileDoc=MRXMLUtils.getDocumentXpath(queryResultDoc, "//MediaStreamFileResource");
	 		 mediaUUID=MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//UUID", "");
	 		 parentDirNID=MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//ParentMediaDirNID", "");
	 		 mediaGroupId=MRXMLUtils.getValueXpath(queryResultDoc, "//GroupID", "");
	 		 groupParentDirNID=MRXMLUtils.getValueXpath(queryResultDoc, "//groupParentDirNID", "");
	      }	
	      
	      Element portEle = MRXMLUtils.getElementXpath(queryResultDoc, "//portEle");
	      Element mediaStorePortElement = MRXMLUtils.getElementXpath(portEle, "//" + AMConst.MEDIA_STORE_PORT_RESOURCE_NID);
	      String mediaStreamFileDirID = MRXMLUtils.getAttributeValue(mediaStorePortElement, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, "");
		
	      //Case 3 In case of Directory Mismatch
	      if(requestName.equalsIgnoreCase(SessionServiceConst.ADD_BOOKMARK_REQUEST) && !parentDirNID.isEmpty() && !groupParentDirNID.isEmpty() && !parentDirNID.equalsIgnoreCase(groupParentDirNID)){
	    	  Document timecodeOffsetRequestDoc  = getTimecodeOffsetRequestDocument(bookmarkObjectDoc, parentDirNID, log, mediaUUID, mediaStreamFileDirID);
	    	  sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, timecodeOffsetRequestDoc, mediaStorePortElement, AddBookmarkRequestHandler.class.getName());
	    	  return null;       	 
	      }
	      
	      
		String responseHandler="";
			 if (requestName.equalsIgnoreCase(SessionServiceConst.ADD_BOOKMARK_REQUEST)){
				 responseHandler= AddBookmarkRequestHandler.class.getName();
			 }else if(requestName.equalsIgnoreCase(SessionServiceConst.UPDATE_BOOKMARK_REQUEST)){
				 responseHandler= UpdateBookmarkRequestHandler.class.getName();
			 }
		Document addBookmarkDoc = getBookmarkRequestDocument(bookmarkObjectDoc, parentDirNID, log, mediaUUID, mediaStreamFileDirID ,mediaGroupId,requestName);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, addBookmarkDoc, mediaStorePortElement, responseHandler);
		 
		return null;
	}
	
	
	public void checkTimecodeAndOffset(String offset, String TC, String requestName) throws MRException {
		if (!"".equals(offset) && !"".equals(TC) && !requestName.equalsIgnoreCase(SessionServiceConst.UPDATE_BOOKMARK_REQUEST)) {
			return;
		}
    }
	
	public Document getBookmarkRequestDocument(Document bookmarkObjectDoc, String dirNID,
			 MRLog log, String mediaUUID,String dirId, String mediaGroupId, String mediaStoreRequestName) throws MRException, Exception {

		 
	   Element bookmarkEL = MRXMLUtils.getElementXpath(bookmarkObjectDoc, "//" + AMConst.BOOKMARK);
       String title = MRXMLUtils.getValueXpath(bookmarkEL, "./"+SessionServiceConst.TITLE_ELEMENT, "");
       String description = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.COMMENT, "");
       String bookmarkId = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.UUID, "");
       String dateCreated = MRXMLUtils.getValueXpath(bookmarkEL, "./DateCreated", "");       
       String tc = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.AT_TC, "");
       String NID=MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.NID);
       String offset = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.BOOKMARK_OFFSET, "");
       Element requstedMetaDataEL = (Element) MRXMLUtils.getElementXpath(bookmarkEL, "./" + SessionServiceConst.META_DATA_ELEMENT).clone();

		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document newRequestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, mediaStoreRequestName);
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(newRequestDoc, MRConst.HEADER_XPATH), MRConst.TYPE,  MRConst.REQUEST_TYPE_RPC);
		Element DataEL = MRXMLUtils.getElementXpath(newRequestDoc, "//Data");
		
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(mediaStoreRequestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(mediaStoreRequestName);
		if(mediaGroupId.isEmpty()){
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_FILE_ID, mediaUUID);
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.SYNC_CLIP_ID, "");
		}else{
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_FILE_ID, mediaGroupId);
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.SYNC_CLIP_ID, mediaUUID);
		}
		
		
		if(!bookmarkId.isEmpty() && bookmarkId.length()>0){
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_ID, bookmarkId);
		}
		MRXMLUtils.addAttribute(requestEl,  SessionServiceConst.TITLE_ELEMENT, title);	
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.COMMENT, description);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DATE_CREATED, dateCreated);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.AT_TC, tc);
		MRXMLUtils.addAttribute(requestEl,  SessionServiceConst.BOOKMARK_OFFSET, offset);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIR_NID, dirNID);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.STREAM_NID, "");
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIRID, dirId);
	
		MRXMLUtils.addAttribute(requestEl, "byFileNID", NID);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		if ((requstedMetaDataEL != null) && !requstedMetaDataEL.elements().isEmpty()) {	       

	            MRXMLUtils.addElement(requestEl, requstedMetaDataEL);       
	        }
		MRXMLUtils.addElement(requestDataEl, (Element)requestEl.detach());
		return newRequestDoc;
	}
	
	
	public Document createOrUpdateBookmarkInDB(MRRequest userRequest, Document agentResponseDoc, Document userRequestdoc, boolean addFile,String requestName, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			return userRequest.createResponseData();
		}
		
		String uuid = "";
		String timecode = "";
		String offset = "";
		String description="";
		String title ="";
		Element requstedMetaDataEL=null;
		
		Element bookmarkEL = MRXMLUtils.getElementXpath(agentResponseDoc, "//Bookmark");		
		if(requestName.equalsIgnoreCase(SessionServiceConst.ADD_BOOKMARK)){
			
			uuid = MRXMLUtils.getAttributeValue(bookmarkEL, "UUID", "");
			timecode = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.AT_TC, "");
			offset = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.BOOKMARK_OFFSET, "");
			
		}else if(requestName.equalsIgnoreCase(SessionServiceConst.UPDATE_BOOKMARK)){
			
			timecode = MRXMLUtils.getValueXpath(bookmarkEL, "//"+SessionServiceConst.AT_TC, "");
			offset = MRXMLUtils.getValueXpath(bookmarkEL, "//"+SessionServiceConst.BOOKMARK_OFFSET, "");
			description = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.COMMENT, "");
			title = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.TITLE_ELEMENT, "");
	        requstedMetaDataEL = (Element) MRXMLUtils.getElementXpath(bookmarkEL, "./" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone();
		}
		

    	String bookmarkNID = MRXMLUtils.getAttributeValue(bookmarkEL, "NID");
    	Document bookmarkDoc = MRXMLDBUtils.readElement(userRequest, log, db, bookmarkNID);
    	
      	if(!timecode.isEmpty() && timecode.length()>0) {
            MRXMLUtils.getElementXpath(bookmarkDoc, "//" + SessionServiceConst.AT_TC).setText(timecode);
        }
    
      	if(!offset.isEmpty() && offset.length()>0) {
            MRXMLUtils.getElementXpath(bookmarkDoc, "//" + SessionServiceConst.BOOKMARK_OFFSET).setText(offset);
        }
   		
   		if(!uuid.isEmpty() && uuid.length()>0) {
            MRXMLUtils.getElementXpath(bookmarkDoc, "//" + "UUID").setText(uuid);
        }
   		
   		if(!description.isEmpty() && description.length()>0) {
            MRXMLUtils.getElementXpath(bookmarkDoc, "//" + SessionServiceConst.COMMENT).setText(description);
        }
   		
   		
   		if(!title.isEmpty() && title.length()>0) {
            MRXMLUtils.getElementXpath(bookmarkDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME).setText(title);
        }
   		
   		
   		if(requstedMetaDataEL != null) {
            MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(bookmarkDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME), requstedMetaDataEL);
        }
   
    	MRXMLDBUtils.updateElement(userRequest, log, db, bookmarkNID, bookmarkDoc.getRootElement());
     	
		return userRequest.createResponseData(createBookmarkResponse(requestName, bookmarkDoc));
	}
	
	
	
	/**
     * Send the Get Tc or Offset request based on given offset or TC to DMS.
     * @param bookmarkObjectDoc
     * @param mediaStreamFileNID
     * @param TC
     * @param offset
     * @param streamDoc
     * @param streamNID
     * @param trackName
     * @param request
     * @param log
     * @param xmpp
     * @param db
     * @param resourceFactory
     * @param serviceAgentFactory
     * @throws MRException
     * @throws Exception
     */
	protected void sendGetTCorOffsetRequestToSourceStreamAgent(Document bookmarkObjectDoc, String mediaStreamFileNID, String TC, String offset,
			Document streamDoc, String streamNID, String trackName, MRRequest request, MRLog log, XMPPI xmpp, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
			if (!"".equals(offset) && !"".equals(TC)) {
                return;
            }

            String agentJID = "";
            String mediaUUID = "";
            String parentDirNID = "";

            //if stream found then check the stream is playback stream or live.
            //If source type is playback file then send message to media source agent.
            //If it is a live stream then send message to immediate source agent.
            if (streamDoc != null) {
                if (trackName.equals(MediaRoomServiceConst.RECORDTRACK)) {
                    agentJID = MRXMLUtils.getValueXpath(streamDoc, "//sourceAgentJID", "");
                    mediaUUID = MRXMLUtils.getValueXpath(streamDoc, "//destFileUUID", "");
                    parentDirNID = MRXMLUtils.getValueXpath(streamDoc, "//destFileParentDirNID", "");
                } else if (trackName.equals(MediaRoomServiceConst.STREAMTRACK)) {
                    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDoc, "//mediaSourceNID", "");
                    String roomNID = MRXMLUtils.getValueXpath(streamDoc, "//roomNID", "");
                    Document mediaSrcDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
                    agentJID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceAgentJID", "");
                    mediaUUID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceFileUUID", "");
                    parentDirNID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//lookbackSourceFileParentDirNID", "");
                }
            }

            // if there is no streams and get the agent jid from media store port.
            if (agentJID.isEmpty()) {
                Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
                String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

                //if the given file nid is media group then find the corresponding clip file and send message to clip media store agent
                if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
                    Element mediaGroupIDEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);
                    String mediaGroupID = MRXMLUtils.getValue(mediaGroupIDEL);
                    String clipID = MRXMLUtils.getAttributeValue(mediaGroupIDEL, MediaStoreServiceConst.CLIP_ID, "");

                    if (mediaGroupID.isEmpty() || clipID.isEmpty()) {
                        throw new MRException(SessionServiceExceptionCodes.NO_MEDIA_STREAM_FILE_FOUND, "No Media Stream File Found to send message");
                    }

                    String[] mediaGroupIdXpath = {
                            "//"+MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT+
                            "[" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "[.='" + mediaGroupID + "' " + "and @" +                      
                            MediaStoreServiceConst.CLIP_ID + " ='" + clipID + "'] and Type='"+MediaRoomServiceConst.MEDIACLIP+"'] "
                        };
                    
                    List<?> mediaStreamFileList = MediaStoreServiceUtils.getMediaStreamFileResourceList(mediaGroupIdXpath, request.getUserJID(), log, db);

                    if ((mediaStreamFileList == null) || mediaStreamFileList.isEmpty()) {
                        throw new MRException(SessionServiceExceptionCodes.NO_MEDIA_STREAM_FILE_FOUND, "No Media Stream File Found to send message");
                    }

                    mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) mediaStreamFileList.get(0));
                }

                mediaUUID = MediaStoreServiceUtils.getMediaStoreFileNID(mediaStreamFileDoc, request, log, db);
                parentDirNID = MediaStoreServiceUtils.getParentDirNID(mediaStreamFileDoc, request, log, db);

                String mediaStorePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDirRead(mediaStreamFileDoc, request, db, log);

                agentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID);
            }

            if ((agentJID == null) || (agentJID.trim().length() == 0)) {
                throw new MRException(MediaRoomServiceExceptionCodes.SOURCEAGENTNOTFOUND, "Source Agent JID Not Found");
            }

            String getOffsetAndTCRequestName = "";

            if ("".equals(TC)) {
                getOffsetAndTCRequestName = MRRequest.getRequestName(GetTimeCodeAtOffsetRequestHandler.class.getName());
            } else {
                getOffsetAndTCRequestName = MRRequest.getRequestName(GetOffsetAtTimecodeRequestHandler.class.getName());
            }

            Document mediaroomRequestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, getOffsetAndTCRequestName);
            String clientData = URLEncoder.encode(MRXMLUtils.documentToString(bookmarkObjectDoc), "UTF-8");

            String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
            String toAgentJID = agentJID;
            String toAgentName = MRConst.MEDIA_SERVICE;
            MRRequest mediaroomRequest = MRRequest.createRequest(request, mediaroomRequestDoc, getOffsetAndTCRequestName, ServiceClient.MEDIAROOMSERVICE,
                    clientData);
            Element getOffsetAtTimeCodeRequestDataEL = MRXMLUtils.getElementXpath(mediaroomRequestDoc, mediaroomRequest.getRequestDataXPath());
            MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("mediaUUID"));
            MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("prentDirNID"));
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//timecode", TC);
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//offset", offset);
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//mediaUUID", mediaUUID);
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//streamNID", streamNID);
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//prentDirNID", parentDirNID);

            String dataStr = MRXMLUtils.elementToString(getOffsetAtTimeCodeRequestDataEL);
            AppServerAgent.sendAgentRequestForUserRequest(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName,
                toAgentJID, toAgentName, dataStr, this.getClass().getName());
        }

    

    /**
     * Generate the list of object based xpaths for bookmark
     * @param requestDoc
     * @param requestName
     * @return
     * @throws MRException
     * @throws Exception
     */
    protected List<String> getBookmarkXPathList(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            	MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document requestDoc, String requestName) throws MRException, Exception {
    		Element getBookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName);
    		String whereFileNID = MRXMLUtils.getAttributeValue(getBookmarkEL, SessionServiceConst.WHERE_FILE_NID, "");
    		String whereSessionInfo = MRXMLUtils.getAttributeValue(getBookmarkEL, SessionServiceConst.WHERE_SESSION_INFO, "");
    		String wherebookmarkNID = MRXMLUtils.getAttributeValue(getBookmarkEL, SessionServiceConst.WHERE_NID, "");

        	Map<String, String> sessionInfoMap = GetObjectUtils.getInfoMapping(whereSessionInfo);

        	String sessionID = "";

        	if (sessionInfoMap != null) {
        		sessionID = sessionInfoMap.get(SessionServiceConst.SESSION_ID);
        	}

        	List<String> whereXpathList = new ArrayList<String>();

        	String whereTCRange = MRXMLUtils.getAttributeValue(getBookmarkEL, SessionServiceConst.WHERE_TC_RANGE, "");

        	Map<String, String> whereTCRangeMap = GetObjectUtils.getInfoMapping(whereTCRange);
        	String fromDate = "";
        	String toDate = "";
        	String interval = "";

        	//TBD: Check with Saurabh they are going to use the same format of DateRange for TCRange or not. 
        	if (whereTCRangeMap != null) {
        		fromDate = whereTCRangeMap.get(SessionServiceConst.FROM_DATE);
        		toDate = whereTCRangeMap.get(SessionServiceConst.TO_DATE);
        		interval = whereTCRangeMap.get(SessionServiceConst.INTERVAL);

        		Map<String, String> tcRangeMap = GetObjectUtils.getDateRange(fromDate, toDate, interval);

        		if (tcRangeMap != null) {
        			fromDate = tcRangeMap.get(SessionServiceConst.FROM_DATE);
        			toDate = tcRangeMap.get(SessionServiceConst.TO_DATE);

        			if (((fromDate != null) && (toDate != null))) {
        				whereXpathList.add(".[AtTC >= " + fromDate + " and AtTC <= " + toDate + "]");
        				//whereXpathList.add("//AtTC [xs:double(not(. = '')) and (. >=" + fromDate + ") and  (.<= " + toDate + ")]");
        			}
        		}
        	}

        	if (whereFileNID.isEmpty() && ((sessionID == null) || sessionID.isEmpty()) && wherebookmarkNID.isEmpty()) {
        		return whereXpathList;
        	}

        	String whereFileNIDXpath = null;
        	String bookmarkFileNID = "";
            String whereNIDXpath = null;
        	if (!sessionID.isEmpty()) {
        		/*
            	String connectionGroupID = sessionInfoMap.get(SessionServiceConst.CONN_GROUP_ID);
            	String connID = sessionInfoMap.get(SessionServiceConst.CONN_ID);
            	String entityID = sessionInfoMap.get(SessionServiceConst.ENTITY_ID);
        		 */
        		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
        		List<?> fileList = MRXMLUtils.getListXpath(sessionDoc, "//Entity[@type='"+SessionServiceConst.RECFILE+"' or @type='"+SessionServiceConst.PLYFILE+"']/File");
        		if(fileList.isEmpty()) {
                    throw new MRException(SessionServiceExceptionCodes.NO_BOOKMARK_CLIPFILE_FOUND, "No File found for bookmark clipID");
                }
        		Element file = (Element)fileList.get(0);
        		String clipFileNID = file.attributeValue(SessionServiceConst.RESOURCENID);
        		bookmarkFileNID = getBookmarkFileNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID, clipFileNID);
        	} else if (!whereFileNID.isEmpty()) {
            bookmarkFileNID = whereFileNID;
        	}

        	if (!bookmarkFileNID.isEmpty()) {
        		whereFileNIDXpath = ".[MediaStreamFileResourceNID='" + bookmarkFileNID + "']";
        		whereXpathList.add(whereFileNIDXpath);
        	}
        	
        	if (!wherebookmarkNID.isEmpty()) {
        	    whereNIDXpath = ".[@NID='" + wherebookmarkNID + "']";
                whereXpathList.add(whereNIDXpath);
            }

        	return whereXpathList;
    	}

    /**
     * Response XML Format
     * <Bookmark NID="" title="" TC="" offset="" owner=""
     *         dateCreated="" mediaStreamFileNID="">
     *           <Description />
     *           <MetaData>
     *              <Tag Name=""></Tag>
     *           </MetaData>
     * </Bookmark>
     * @param requestName
     * @param bookmarkDoc
     * @return
     */
    protected Element createBookmarkResponse(String requestName, Document bookmarkDoc) throws MRException, Exception {
        Element responseEl = MRXMLUtils.newElement(requestName);
        Element bookmarkResponseEL = MRXMLUtils.newElement(AMConst.BOOKMARK);
        Element bookmarkEL = bookmarkDoc.getRootElement();
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.NID, MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.NID));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.TITLE,
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, ""));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.TC, MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.AT_TC, ""));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.BOOKMARK_OFFSET.toLowerCase(),
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.BOOKMARK_OFFSET, ""));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.OWNER.toLowerCase(),
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.AUTHOR_USER_ID, ""));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.DATE_CREATED,
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + MediaStoreServiceConst.DATE_CREATED, ""));
        MRXMLUtils.addAttribute(bookmarkResponseEL, SessionServiceConst.MEDIA_STREAM_FILE_NID,
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, ""));

        MRXMLUtils.addElement(bookmarkResponseEL, SessionServiceConst.DESCRIPTION,
            MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.COMMENT, ""));
        MRXMLUtils.addElement(bookmarkResponseEL, (Element) MRXMLUtils.getElement(bookmarkEL, MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone());
        MRXMLUtils.addElement(responseEl, bookmarkResponseEL);

        return responseEl;
    }
    
    
    /**
     * Set the given values in request xml.
     * @param bookmarkObjectDoc
     * @param requestDoc
     * @param TC
     * @param offset
     * @param ownerUserJID
     * @param bookmarkFileNID
     * @param xmpp
     * @param log
     * @return
     * @throws MRException
     * @throws Exception
     */
    protected Document createBookmarkObjectDocument(Document bookmarkObjectDoc, Document requestDoc, String TC, String offset, String ownerUserJID,
            String bookmarkFileNID, XMPPI xmpp, MRLog log) throws MRException, Exception {
            Element bookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + AMConst.BOOKMARK);
            String title = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.TITLE, "");

            String description = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.DESCRIPTION, "");
            Element requstedMetaDataEL = MRXMLUtils.getElementXpath(bookmarkEL, "./" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
      

        String now = String.valueOf(xmpp.getCurrentTimeMillis());

        MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AT_TC, TC);

        MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, bookmarkFileNID);

        MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.BOOKMARK_OFFSET, offset);

        String dateCreated = MRXMLUtils.getValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_CREATED, "");

        if (dateCreated.isEmpty()) {
            MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_CREATED, now);
        }

        MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, now);

        if (!title.isEmpty()) {
            MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
        }

        if (!description.isEmpty()) {
            MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.COMMENT, description);
        }

        String ownerJID = MRXMLUtils.getValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AUTHOR_USER_ID, "");

        if (ownerJID.isEmpty()) {
            MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AUTHOR_USER_ID, ownerUserJID);
        }

        Element metaDataEL = MRXMLUtils.getElementXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

        if ((requstedMetaDataEL != null) && !requstedMetaDataEL.elements().isEmpty()) {
            MRXMLUtils.removeContent(metaDataEL);
            MRXMLUtils.addChildrenToElement(metaDataEL, requstedMetaDataEL);
        }

        return bookmarkObjectDoc;
    }

    protected String getMediaGroupFileNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String sessionID)
        throws MRException, Exception {
        Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
        Element groupFile = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.GRPFILE);
        if(groupFile == null) {
            throw new MRException(SessionServiceExceptionCodes.NO_MEDIA_GROUPFILE_FOUND, "No Media Group Found to Add Bookmark");
        }

        String resourceNID = MRXMLUtils.getAttributeValue(groupFile, SessionServiceConst.RESOURCENID, "");
        if ("".equals(resourceNID)){
        	throw new MRException(SessionServiceExceptionCodes.GROUP_FILE_RESOURCENID_NOT_FOUND, "GrpFile ResourceNID not found");
        }
        return resourceNID;
    }
    
    protected String getBookmarkFileNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String sessionID, String clipFileNID) throws Exception {
    	if(sessionID == null) {
            return clipFileNID;
        }
    	try {
    		return this.getMediaGroupFileNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sessionID);
    	} catch (Exception e){
    		return clipFileNID;
    	}
    	
    }
    
    protected String getSessionID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
    		MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,
    		String whereFileNID) throws Exception {
    	String whereXpath="//File[@" + SessionServiceConst.RESOURCENID + "='" +	whereFileNID + "']";
        Document sessionList = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(
        		request, xmpp, log, db, resourceFactory, serviceAgentFactory,1,-1, false,whereXpath);
        List<?> fileList = MRXMLUtils.getListXpath(sessionList, "//File");
        if(fileList.isEmpty()) {
            throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND, "No session found for whereFileNID=" + whereFileNID);
        }
        if(fileList.size() > 1) {
            throw new MRException(SessionServiceExceptionCodes.MULTIPLE_SESSIONS_FOUND_FOR_BOOKMARK_CLIP, "Multiple sessions found for whereFileNID=" + whereFileNID);
        }
        Element file = (Element)fileList.get(0);
        Element session = file.getParent().getParent().getParent();
        return session.attributeValue(SessionServiceConst.ID);
    }
    

    protected Document getStreamDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,ServiceAgentFactoryI serviceAgentFactory,
    		String sessionID, String clipFileNID) throws Exception {
	    String recordXpath = "data/destNID='" + clipFileNID + "'";
	    String playBackXpath = "data/sourceNID='" + clipFileNID + "'";
	    String streamXpath = "//Stream[ " + recordXpath + "  or " + playBackXpath + " ]";

	    // Get the roomNID
	    String roomNID = null;
	    if(sessionID != null) {
            roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionID);
        } else {
	    	roomNID = MediaRoomServiceUtils.getRoomNIDWithXPath(request, xmpp, log, db, streamXpath);
		    if(roomNID == null) {
                throw new MRException(SessionServiceExceptionCodes.NO_ROOM_FOUND_FOR_CLIPFILE, "No room found for given clipFileNID=" + clipFileNID);
            }
	    }
	    
	    Document streamdoc = MediaRoomServiceUtils.readElementByXpath(roomNID, request, log, db, streamXpath);
	    
	    if(streamdoc == null) {
            throw new MRException(SessionServiceExceptionCodes.NO_RECORDING_PLAYBACK_CONNECTION_FOUND, "No recording/playback connection found for bookmarking");
        }
	    return streamdoc;
    }
    
    protected void updateOffsetTC(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		String roomNID, boolean isLive, String clipFileNID, Element bookmarkEL) throws Exception  {
        String requestedTC = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.TC, "");
        String requestedOffset = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.OFFSET, "");
        if(!"".equals(requestedTC) || !"".equals(requestedOffset)) {
            return;
        }
        
        String trackName = MediaRoomServiceConst.STREAMTRACK;
        
        String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
        Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
        MediaTimeline trackTimeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);

        String trackState = trackTimeline.getLastState();

        long currentTrackOffset = trackTimeline.getLastOffset();
        
        long lastActionWallClock = trackTimeline.getLastActionWallClock();
       
        long WC = xmpp.getCurrentTimeMillis();
        if (trackState.equals(MediaRoomServiceConst.TRACK_STARTED_CURRENT) || trackState.equals(MediaRoomServiceConst.TRACK_STARTED_LOOKBACK)) {
           currentTrackOffset = currentTrackOffset + (WC - lastActionWallClock)  * (long)trackTimeline.getLastTimescale();
        }
      
        long startWallClock = trackTimeline.getStartWallclock();
     
        long currentTrackTC = startWallClock + currentTrackOffset;   
        
        
        if(isLive) { // Recording so use TC
            MRXMLUtils.setAttributeValue(bookmarkEL, SessionServiceConst.TC, String.valueOf(currentTrackTC));
        } else { // Playback so use Offset
        	MRXMLUtils.setAttributeValue(bookmarkEL, SessionServiceConst.OFFSET, String.valueOf(currentTrackOffset));
        }
        
    }
    
    protected String getClipFileNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
    		MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, String sessionID, String whereFileNID) throws Exception {
    	if(!"".equals(whereFileNID)) {
            return whereFileNID;
        }
        Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
        List<?> fileList = MRXMLUtils.getListXpath(sessionDoc, "//Entity[@type='"+SessionServiceConst.RECFILE+"' or @type='"+SessionServiceConst.PLYFILE+"']/File");
        if(fileList.isEmpty()) {
            throw new MRException(SessionServiceExceptionCodes.NO_BOOKMARK_CLIPFILE_FOUND, "No File found for bookmark clipID");
        }
        String grpFileXpath = "//GrpFile[@type='" + SessionServiceConst.SYNCPLY + "' or @type='" + SessionServiceConst.SYNCREC + "']";
        Element groupFileEL = MRXMLUtils.getElementXpath(sessionDoc, grpFileXpath);
        if (groupFileEL != null) {
            String clipID = MRXMLUtils.getAttributeValue(groupFileEL, SessionServiceConst.CLIPID);
            String clipIdXpath = "//File[@clipId='" + clipID + "']";
            Element file = MRXMLUtils.getElementXpath(sessionDoc, clipIdXpath);
            if(file != null) {
                return file.attributeValue(SessionServiceConst.RESOURCENID);
            }
        }
        Element file = (Element)fileList.get(0);
        return file.attributeValue(SessionServiceConst.RESOURCENID);
    }
    
    public void checkFileNID(String mediaStreamFileNID) throws MRException {
    	if (mediaStreamFileNID.isEmpty()) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIA_STREAM_FILE_NID_NOT_FOUND, "MediaStreamFile NID Not Found");
        }
    }

	public Document handleNewBookmark(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Element requestEl, String requestedTC, String requestedOffset, Element bookmarkEL, String operation) throws Exception {

		String whereFileNID = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.WHERE_FILE_NID, "");
		String whereSessionInfo = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.WHERE_SESSION_INFO, "");

		boolean isLive = false;
		String roomNID = null;
		String syncClipNID = null;

		// Get the sessionID
		String sessionID = null;
		//validate input
		if(whereFileNID.isEmpty() && whereSessionInfo.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.MISSING_INPUT, "Missing Required Info");
		}
		
		//set sync media clip as where file nid if offset or tc provided
		if (!"".equals(whereFileNID) && !("".equals(requestedTC) && "".equals(requestedOffset))) {
			syncClipNID = whereFileNID;
		} 
		else
		{   //if file nid procided and tc/offset not provided find offset/tc from session 
			if (!"".equals(whereFileNID)) {			
				try {
					sessionID = this.getSessionID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, whereFileNID);
				} catch (MRException mre) {
					if (mre.getCode() == SessionServiceExceptionCodes.NO_SESSION_FOUND) {
						// Could be a legacy request for which there is a room but
						// no session

						syncClipNID = whereFileNID;

						// Get the streamdoc
						Document streamDoc = this.getStreamDoc(request, xmpp, log, db, serviceAgentFactory, sessionID, syncClipNID);
						roomNID = MRXMLUtils.getValueXpath(streamDoc, "//roomNID", "");
						String destNID = MRXMLUtils.getValueXpath(streamDoc, "//destNID", "");
						if (destNID.equals(syncClipNID)) {
                            isLive = true;
                        }
					} else {
                        throw mre;
                    }
				}			
			} else if (!"".equals(whereSessionInfo)) {
				Map<String, String> sessionInfoMap = GetObjectUtils.getInfoMapping(whereSessionInfo);
				sessionID = sessionInfoMap.get(SessionServiceConst.SESSION_ID);
			}
			if (sessionID == null) {
                throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND, "No session found for whereSessionInfo=" + whereSessionInfo);
            }
			Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
			if (sessionDoc == null) {
				throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, MRXMLUtils.generateErrorXML("sessionID", sessionID));
			}
			String sessionType = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE);
			if (SessionServiceConst.MIXED.equals(sessionType)) {
                throw new MRException(SessionServiceExceptionCodes.BOOKMARK_NOT_SUPPORTED_FOR_MIXED_SESSION, MRXMLUtils.generateErrorXML("sessionID", sessionID));
            }

			// Get the clipFile
			syncClipNID = this.getClipFileNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID, whereFileNID);
			checkFileNID(syncClipNID);
			// Get roomNID and sessionType (RECORD or PLAYBACK)
			roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionID);
			if (sessionType.equalsIgnoreCase(SessionServiceConst.LIVE)) {
                isLive = true;
            }
		}

		// Update the offset/TC
		this.updateOffsetTC(request, xmpp, log, db, roomNID, isLive, syncClipNID, bookmarkEL);

		String tc = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.TC, "");
		String offset = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.OFFSET, "");
		Document bookmarkObjectDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, log);
		log.addInfo("BookmarkBaseRequestHandler TC=" + tc + " Offset=" + offset, MRLog.OK, MRLog.NOTIFY);

		if (SessionServiceConst.ADD.equals(operation)) {
			Document requestDoc = request.getMessage().getDocument();
			String bookmarkFileNID = this.getBookmarkFileNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID, syncClipNID);
			
			log.addInfo("AddBookmarkBaseRequestHandler clipFileNID=" + syncClipNID + " bookmarkFileNID=" + bookmarkFileNID, MRLog.OK, MRLog.NOTIFY);
			bookmarkObjectDoc = createBookmarkObjectDocument(bookmarkObjectDoc, requestDoc, tc, offset, request.getUserJID(), bookmarkFileNID, xmpp, log);

			AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, resourceFactory, serviceAgentFactory, bookmarkObjectDoc, AMConst.BOOKMARK);
			sendAddUpdateBookmarkRquestToDMS(bookmarkObjectDoc, syncClipNID, tc, offset, null, "", bookmarkFileNID, "", request,"AddBookmarkRequest", log, xmpp, db, resourceFactory, serviceAgentFactory);

		}
		return bookmarkObjectDoc;
	}

	
	public Document getTimecodeOffsetRequestDocument(Document bookmarkObjectDoc, String dirNID,
			 MRLog log, String mediaUUID,String dirId) throws MRException, Exception {
		String mediaStoreRequestName="GetOffsetTimecodeRequest";
		Element bookmarkEL = MRXMLUtils.getElementXpath(bookmarkObjectDoc, "//" + AMConst.BOOKMARK);
		String tc = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.AT_TC, "");
		String offset = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.BOOKMARK_OFFSET, "");
		String nid = MRXMLUtils.getAttributeValue(bookmarkEL, "./" + SessionServiceConst.NID, "");
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, mediaStoreRequestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(requestDoc, MRConst.HEADER_XPATH), MRConst.TYPE,  MRConst.REQUEST_TYPE_RPC);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(mediaStoreRequestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(mediaStoreRequestName);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_FILE_ID, mediaUUID);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_FILE_NID, nid);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.AT_TC, tc);
		MRXMLUtils.addAttribute(requestEl,  SessionServiceConst.BOOKMARK_OFFSET, offset);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIR_NID, dirNID);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIRID, dirId);
		MRXMLUtils.addElement(requestDataEl, (Element)requestEl.detach());
		return requestDoc;
	}
}
    
