package com.ipvs.sessionservice.impl;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Hashtable;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.file.handler.BasePublishFileRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.impl.SessionPolicyUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class SessionResourceUtils {
	
	// Debug print
	@SuppressWarnings("unused")
	private static void debugPrint(Document doc) throws Exception {
		Document tmp = (Document)doc.clone();
		List<?> list = MRXMLUtils.getListXpath(tmp, "//MediaStreamFileResource");
		for(Object object: list) {
			MRXMLUtils.removeContent((Element)object);
		}
		System.out.println(MRXMLUtils.documentToString(tmp));
	}

	public static Document getQueryResult(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, Document queryDoc) throws Exception {
		if(db != null) {
	    	long dbtimeout = MRXMLDBUtils.TIME_OUT;
	    	Hashtable<String, String> table = new Hashtable<String, String>();	    	
	    	String query = 	FileUtils.readFileResource(SessionServiceConst.SESSIONXQUERYLIBXML, request.getClass());
	    	query = query + "\nlet $q := document {" +
	    				MRXMLUtils.elementToString(queryDoc.getRootElement()) +
	    				"}\nlet $result := local:handleQuery($q/query)\nreturn $result";
	    	String queryResult = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
			return MRXMLUtils.stringToDocument(queryResult);
		}
		
		// Dummy result
		System.out.println("TBD getQueryResult");
		Document result = MRXMLUtils.stringToDocument("<result/>");
		List<?> list = queryDoc.getRootElement().elements();
		for(Object object: list) {
			Element element = (Element)object;
			String elementName = MRXMLUtils.getAttributeValue(element,SessionServiceConst.ELEMENTNAME);
			element.setName("resultitem");
			MRXMLUtils.addAttribute(element,"elementName", elementName);
			long now = System.currentTimeMillis();
			Element resource = MRXMLUtils.stringToElement(
					"<" + elementName + "Resource" + " NID='" + now + "'/>");
			element.add(resource);
			result.getRootElement().add((Element)element.clone());
		}
		// SessionResourceUtils.debugPrint(result);
		return result;
	}
	
	public static Document getMediaStreamFileDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String mediaType, long trimLength,
			String fileTitle, String fileDescription,
			String parentDirNID, long now, String mediaGroupID, String clipId, 
			Element groups, Element metaData, String sessionId, String sessionConfigNID, String mediaStoreFileNID) throws Exception  {
		
		String userJID = request.getUserJID();
        String username = userJID;
        int serverindex = userJID.indexOf("/");
        if (serverindex > 0) {
            username = userJID.substring(0, serverindex);
        }

        
        Document mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, 
        		AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
        
        if(mediaStoreFileNID.isEmpty())
        	mediaStoreFileNID = Utils.getUUID();
        
        MediaStoreServiceUtils.updateMediaStreamFileDocWithInfo(xmpp, mediaStreamFileDoc, mediaType, fileTitle, fileDescription, parentDirNID, groups, mediaStoreFileNID, 
        		"",  MediaStoreServiceConst.NOT_READY,  MediaStoreServiceConst.TRUE,metaData,String.valueOf(now),String.valueOf(now),SessionServiceConst.FALSE, null);
		
	
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.OWNER_EL_NAME, username);	 
	   
	    // Set the MediaGroup ID
	    MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + 
        		MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaRoomServiceConst.MEDIAGROUPID, 
        		mediaGroupID);
        
        // Set the Clip ID
	    Element mediaGroupEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaRoomServiceConst.MEDIAGROUPID);
	    mediaGroupEL.addAttribute(MediaRoomServiceConst.CLIPID, clipId);
	    
	    // Set the Media Type
        if (mediaType.equalsIgnoreCase(MediaRoomServiceConst.PVRMEDIACLIP)) {
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PVR_TRIM_LENGTH_EL_NAME, String.valueOf(trimLength));
        } else if (mediaType.equalsIgnoreCase(MediaRoomServiceConst.MEDIAGROUP)) {
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.BUSY);
        }
	 
	 
	 
	    // Set the sessionId, sessionConfigNID
	    Element recInfo = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.REC_INFO);
	    recInfo.addAttribute(MediaStoreServiceConst.SESSION_ID, sessionId);
	    recInfo.addAttribute(MediaStoreServiceConst.SESSION_CONFIG_NID, sessionConfigNID);

	    	    
	    return mediaStreamFileDoc;
	}

    public static void createStreamFileResource(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		Document sessionDoc, Element element,String requestedDirNID, long now, String type,
    		Element groups, String sessionId, String sessionConfigNID,String targetAccessXpath) throws Exception {

		
		XMPPTransaction.startBlock("createStreamFileResource");
		
		Document mediaStreamFileDoc =  null;
		
		// Get the title and description
		String fileTitle = MRXMLUtils.getAttributeValue(element,SessionServiceConst.RESOURCETITLE);
		Element metaDataEL = MRXMLUtils.getElement(element, MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
		if(metaDataEL == null) {
            //metaDataEL = MRXMLUtils.stringToElement("<MetaData id='' name=''><Tag Name=''>none</Tag></MetaData>");
			metaDataEL = MRXMLUtils.stringToElement("<MetaData></MetaData>");
        }
		
		
		//check the key attribute specified or not
		String keyAttribute = MRXMLUtils.getAttributeValue(element, SessionServiceConst.KEY,"");
		String action = MRXMLUtils.getAttributeValue(element, SessionServiceConst.ACTION,"");
		
		if(SessionServiceConst.RECFILE.equals(type) && (!keyAttribute.isEmpty())) {
			log.addInfo("SessionResourceUtils:createStreamFileResource - Check File Exist based on Key=" + keyAttribute +" and action:="+action);
			String whereXpath = null;
			if(keyAttribute.equals(SessionServiceConst.KEY_TITLE)) {				
				whereXpath = "//Title[.='"+fileTitle+"']";
			} else if(keyAttribute.equals(SessionServiceConst.KEY_METADATA)){
				String keyMetaDataTagName = MRXMLUtils.getAttributeValue(element, SessionServiceConst.KEY_METADATA_TAG_NAME,"");
				if(keyMetaDataTagName.isEmpty()) {
					throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Input Not Valid. Key Metadata Tag Name Required for MetaData Key");
				}
				String metaDataValue = MRXMLUtils.getValueXpath(metaDataEL, "./Tag[@Name='"+keyMetaDataTagName+"']", "");
				if(metaDataValue.isEmpty()) {
					throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Input Not Valid. No MetaData value found for given metadata key tag name");
				}
				whereXpath = "//MetaData/Tag[@Name='"+keyMetaDataTagName+"' and .='"+metaDataValue+"']";
			}
			
			if(whereXpath == null) {
				throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Input Not Valid. Key Attribute not valid");
			}
			
			MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MediaStreamFileResource, log);
	    	String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log)); 	    	
			Element fileResEL  =MRXMLDBUtils.readElement(-1, null, new String[]{whereXpath}, parentNID, null, request, log, db);
			
			if(fileResEL != null) {				
				
				mediaStreamFileDoc =MRXMLUtils.getDocument();
				mediaStreamFileDoc.setRootElement(fileResEL);
				
				MRXMLUtils.setAttributeValue(element, SessionServiceConst.CLIPID, MRXMLUtils.getAttributeValueXpath(mediaStreamFileDoc, "//MediaGroupID", "ClipID", ""));				
				MRXMLUtils.setAttributeValue(element, SessionServiceConst.MEDIAGROUPID, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//MediaGroupID", ""));
				MRXMLUtils.setAttributeValue(element, SessionServiceConst.RESOURCETITLE, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//Title", ""));
				MRXMLUtils.setAttributeValue(element, SessionServiceConst.STREAM_TYPE, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//StreamType", ""));
				MRXMLUtils.setAttributeValue(element, SessionServiceConst.RESOURCENID, MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), SessionServiceConst.NID,""));
				
				if(action.isEmpty()) {
					throw new MRException(SessionServiceExceptionCodes.ERROR_ON_EXIST, "Can not start Recording File already exist");
				} 
				
				if(action.equals(SessionServiceConst.ACTION_OVERWRITE)){
					//TBD
					throw new MRException(SessionServiceExceptionCodes.ACTION_NOT_SUPPORTED, "Action not supported");
				}
			}
		} 
		
		if(mediaStreamFileDoc == null) {
				mediaStreamFileDoc = createMediaStreamFileResource( request,  xmpp,  log,  db,  resourceFactory,  serviceAgentFactory,
								sessionDoc,  element, requestedDirNID,  now,  type, groups,  sessionId,  sessionConfigNID,fileTitle,metaDataEL,targetAccessXpath);
		}
		             		
        Element resourceElement = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
        String resourceNID = MRXMLUtils.getAttributeValue(resourceElement,"NID");
        String mediaType = MRXMLUtils.getValueXpath(resourceElement, "//Type", "");
        MRXMLUtils.addAttribute(element,SessionServiceConst.RESOURCENID, resourceNID);
		
		// Update the cache
		request.getCache().put(resourceNID, mediaStreamFileDoc);
		new BasePublishFileRequestHandler().sendFileNotification(request, xmpp, log, db, resourceFactory, serviceAgentFactory, resourceNID, SessionServiceConst.ADD, mediaType);
        // System.out.println("SessionResourceUtils:createStreamFileResource doc=" + MRXMLUtils.documentToString(resourceDoc));
		XMPPTransaction.endBlock("createStreamFileResource");
	}

	private static Document createMediaStreamFileResource(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, Document sessionDoc,
			Element element, String requestedDirNID, long now, String type,Element groups, String sessionId, String sessionConfigNID,
			String fileTitle,Element metaDataEL,String targetAccessXpath) throws MRException,Exception {

		Document mediaStreamFileDoc = null;
		String fileDescription = MRXMLUtils.getValueXpath(element, ".//description", "");
		
		// Get the mediaType
		String mediaType = null;
		if(SessionServiceConst.GRPFILE.equals(type)) {
			mediaType = MediaRoomServiceConst.MEDIAGROUP;
		} else if(SessionServiceConst.RECFILE.equals(type)) {
			mediaType = MediaRoomServiceConst.MEDIACLIP;
		} else if(SessionServiceConst.PVRFILE.equals(type)) {
			mediaType = MediaRoomServiceConst.PVRMEDIACLIP;
		}
		// Get the trimlength
		long trimLength = 0;
		if(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {
			try {
				trimLength = Long.parseLong(MRXMLUtils.getAttributeValue(element,SessionServiceConst.TRIMLENGTH));
			}
			catch(Exception e) {
				trimLength = Long.parseLong(MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
			}
		}
		
		// Find the mediaGroupId
		String mediaGroupId = "";
		if(MediaRoomServiceConst.MEDIACLIP.equals(mediaType)) {
			String groupFileId = MRXMLUtils.getAttributeValue(element,SessionServiceConst.GRPFILEID);
			Element groupFileElement = MRXMLUtils.getElementXpath(sessionDoc, "//" + 
					SessionServiceConst.GRPFILE + "[@" + SessionServiceConst.ID + "='" + groupFileId + "']");
			if(groupFileElement != null) {
				mediaGroupId = MRXMLUtils.getAttributeValue(groupFileElement,SessionServiceConst.MEDIAGROUPID);
			}
		}else if(MediaRoomServiceConst.MEDIAGROUP.equals(mediaType)) {
			mediaGroupId = MRXMLUtils.getAttributeValue(element,SessionServiceConst.MEDIAGROUPID);
		}

		// Find the clipId
		String clipId = MRXMLUtils.getAttributeValue(element,SessionServiceConst.CLIPID);

		// Get the dir NID
		String parentDirNID = "";
		if(!MediaRoomServiceConst.MEDIAGROUP.equals(mediaType)) {
			Element entity = MRXMLUtils.getParentElement(element);
			String fileID = MRXMLUtils.getAttributeValue(entity,SessionServiceConst.ID);
			// Find the Connection that uses SessionResourceUtils clip file
			String cXPath = "//" + SessionServiceConst.CONNECTION + "[@" +
							SessionServiceConst.DSTENTITYID + "='" + 
							fileID + "']";
			Element connection = MRXMLUtils.getElementXpath(sessionDoc, cXPath);
			// System.out.println("Connection xpath=" + cXPath + connection);
			// Find the Source for the connection
			String srcEntityID = MRXMLUtils.getAttributeValue(connection,SessionServiceConst.SRCENTITYID);
			String srcXPath = "//" + SessionServiceConst.ENTITY + 
					"[@" + SessionServiceConst.ID + "='" + srcEntityID + "']/" + 
					SessionServiceConst.PORT;
			Element srcPort = MRXMLUtils.getElementXpath(sessionDoc, srcXPath); 
			
			//don't create file if source has some errors
			Element srcEntityEl = srcPort.getParent();
			checkSourceEntityErrorState(srcEntityEl);
			
			// System.out.println("SrcPort xpath=" + srcXPath + srcPort);
			String sourceNID = MRXMLUtils.getAttributeValue(srcPort,SessionServiceConst.RESOURCENID);			
			//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
			XMPPTransaction.startBlock("getDirNID");
			parentDirNID = SessionPolicyUtils.getDirNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sourceNID, mediaType, requestedDirNID,targetAccessXpath);
			XMPPTransaction.endBlock("getDirNID");
			//XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
			
		}
		
		// Generate the mediaStreamFileDoc
		mediaStreamFileDoc = SessionResourceUtils.getMediaStreamFileDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				mediaType, trimLength, fileTitle, fileDescription, parentDirNID, 
				now, mediaGroupId, clipId, groups, metaDataEL, sessionId, sessionConfigNID, "");
      		
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileDoc, AMConst.MEDIA_STREAM_FILE_RESOURCE);        
  
		return mediaStreamFileDoc;
		
	}
	
	public static void checkSourceEntityErrorState(Element srcEntityEl) throws MRException,Exception {
		String state = MRXMLUtils.getAttributeValue(srcEntityEl, SessionServiceConst.STATE,"");
		if(!state.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.SOURCE_ENTITY_ERROR, "Source Entity Error");
		}
	}

	public static boolean isUsedAsDestination(Document sessionDoc, String id) throws Exception {
		List<?> list = MRXMLUtils.getListXpath(sessionDoc, 
				"//Connection[@dstEntityId='" + id + "']" );
		// System.out.println(list.isEmpty());
		return !list.isEmpty();
	}

	public static boolean isUsedInConnection(Document sessionDoc, String id) throws Exception {
		List<?> list = MRXMLUtils.getListXpath(sessionDoc, 
				"//Connection[@dstEntityId='" + id + "' or @srcEntityId='" + id +"']" );
		return !list.isEmpty();
	}
	
	public static boolean isDestEntity(Document sessionDoc, String id) throws Exception {
		String entityType = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//Entity[@id='" + id + "']", "type", "");
		 return (SessionServiceConst.DSTPORT.equals(entityType) || 
				 SessionServiceConst.RECFILE.equals(entityType) ||
				 SessionServiceConst.PVRFILE.equals(entityType)
				 );
	}
	
	public static boolean isValidSrcAndDestPort(Element resource, String resourceType) throws Exception {
		
		String portType = MRXMLUtils.getValueXpath(resource, ".//Info/Type" , "");
		
		return ((resource != null)
			   && ((SessionServiceConst.SRC_PORT.equals(resourceType)
					&& (SessionServiceConst.STREAM_SRC.equals(portType) || SessionServiceConst.BOTH.equals(portType)))
			   || (SessionServiceConst.DST_PORT.equals(resourceType)
					&& (SessionServiceConst.STREAM_DST.equals(portType) || SessionServiceConst.BOTH.equals(portType)))
				));
	}
	
	public static void updateResourceElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		Element resourceElement, String elementName, String type) throws Exception {
		Document queryDoc = MRXMLUtils.stringToDocument("<query/>");

		Element qelement = MRXMLUtils.stringToElement("<queryitem " + SessionServiceConst.ELEMENTNAME + "='" + elementName + "'/>");
		String resourceTitle =MRXMLUtils.getAttributeValue( resourceElement,SessionServiceConst.RESOURCETITLE);
		String resourceNID = MRXMLUtils.getAttributeValue(resourceElement,SessionServiceConst.RESOURCENID);
		if(resourceTitle == null) {
            resourceTitle = "";
        }
		if(resourceNID == null) {
            resourceNID = "";
        }
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCETITLE, resourceTitle);
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCENID, resourceNID);
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.TYPE_ATTRIBUTE, type);
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCEJID, "");		
		queryDoc.getRootElement().add(qelement);
		Document resultDoc = SessionResourceUtils.getQueryResult(request, xmpp, log, db, queryDoc);
		// System.out.println("resultDoc=" + MRXMLUtils.documentToString(resultDoc));
		Element resultitem = MRXMLUtils.getElementXpath(resultDoc, "//resultitem");
		Element resultResource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resultitem, "./"+PolicyAdminConstants.RESOURCE));
		
		if(resultResource == null) {
    		throw new MRException(SessionServiceExceptionCodes.DEST_RESOURCE_NOT_FOUND, "Dest Resource Not Found for title=" + resourceTitle);
		}
		if("".equals(resourceNID)) {
			resourceElement.addAttribute(SessionServiceConst.RESOURCENID,
					resultResource.attributeValue(SessionServiceConst.NID));
		}
		if("".equals(resourceTitle)) {
			resourceElement.addAttribute(SessionServiceConst.RESOURCETITLE,
					MRXMLUtils.getValueXpath(resultResource, ".//Title", ""));
		}
		Document resultResourceDoc = MRXMLUtils.elementToDocument((Element)resultResource.detach());
		request.getCache().put(resourceNID, resultResourceDoc);
	}

	private static String getSyncClipEntityId(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		Document sessionDoc) throws Exception {
		String clipEntityId = "";
		List<?> recFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + 
				SessionServiceConst.ENTITY + "[@type='" + SessionServiceConst.RECFILE + "']");
		for(Object o:recFileList) {
			// Find the Source Resource
			Element recFile = (Element)o;
			clipEntityId = recFile.attributeValue(SessionServiceConst.ID);
			Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//" + 
					SessionServiceConst.CONNECTION + "[@dstEntityId='" + clipEntityId + "']");
			if(connection == null) {
                continue;
            }
			String srcEntityId = connection.attributeValue(SessionServiceConst.SRCENTITYID);
			Element srcEntity = MRXMLUtils.getElementXpath(sessionDoc, "//" + 
					SessionServiceConst.ENTITY + "[@id='" + srcEntityId + "']");
			try{
				checkSourceEntityErrorState(srcEntity);
			}catch(Exception exp) {
				continue;
			}
			Element resource = srcEntity.element(SessionServiceConst.PORT);
			if(resource == null) {
                continue;
            }
			// Check if the Source is Ready or Unmanaged
			String resourceNID = resource.attributeValue(SessionServiceConst.RESOURCENID);
			Document srcDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
			String stateFlag = MRXMLUtils.getValueXpath(srcDoc, "//State/StateFlag", "");
			if(SessionServiceConst.READY.equals(stateFlag) || 
					SessionServiceConst.UNMANAGED.equals(stateFlag))
             {
                break;
			// System.out.println(MRXMLUtils.documentToString(srcDoc));
            }
		}
		return clipEntityId;
	}

	public static void updateSessionDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		Document sessionDoc, String sessionId, String sessionConfigNID) throws Exception {
        long now = System.currentTimeMillis();
        
        //set the name if the session name is empty
        String sessionName = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.NAME,"");  
        if(sessionName.isEmpty()){
        	SimpleDateFormat sdf = new SimpleDateFormat("MMM dd HH:mm");
        	String sdate = sdf.format(new Date(now));
        	int  index = request.getUserJID().indexOf("@");
        	String userName = request.getUserJID().substring(0,index); 
			sessionName = userName  +" " +sdate;
			MRXMLUtils.setAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.NAME, sessionName);
		} 

        /**
         * Set the sessionType to be LIVE/PLAYBACK/MIXED
         */
        
        String sessionType = "";
        List<?> srcPortList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = '" + SessionServiceConst.SRCPORT + "']");		
        List<?> plyFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = '" + SessionServiceConst.PLYFILE + "']");		
        if(!srcPortList.isEmpty()) {
            sessionType = SessionServiceConst.LIVE;
        }
        if(!plyFileList.isEmpty()) {
            sessionType = SessionServiceConst.PLAYBACK;
        }
        if(!srcPortList.isEmpty() && !plyFileList.isEmpty()) {
            sessionType = SessionServiceConst.MIXED;
        }
        if (request.getUserJID().equals(request.getAppServerJID())) {
        	sessionType = SessionServiceConst.SYSTEM.toUpperCase();
        }
        
		MRXMLUtils.addAttribute(sessionDoc.getRootElement(),SessionServiceConst.TYPE_ATTRIBUTE, sessionType);
		
		List<?> pvrElList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = 'SrcPort']/" + SessionServiceConst.PVR);
		for(Object object: pvrElList) {
			Element pvrEl = ((Element)object);
			String trimLength = MRXMLUtils.getAttributeValue(pvrEl, SessionServiceConst.TRIMLENGTH, "");
			if(trimLength.isEmpty()) {
                MRXMLUtils.setAttributeValue(pvrEl, SessionServiceConst.TRIMLENGTH, MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
            }
		}
        
        Element entityList = MRXMLUtils.getElementXpath(sessionDoc, "//EntityList");
        Element connectionList = MRXMLUtils.getElementXpath(sessionDoc, "//ConnectionList");
		Document queryDoc = MRXMLUtils.stringToDocument("<query/>");
		
		/**
		 * Generate the querydoc
		 */
		
		//add operator query
		Element qelement = createOperatorQueryElement(request);
	
		
		queryDoc.getRootElement().add(qelement);
		List<?> resourceList = MRXMLUtils.getListXpath(sessionDoc, "//*[@resourceNID!='' or @resourceTitle!='']");
		for(Object object: resourceList) {
			Element element = (Element)object;
			String elementName = element.getName();
			qelement = MRXMLUtils.stringToElement("<queryitem " + SessionServiceConst.ELEMENTNAME + "='" + elementName + "'/>");
			MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCEJID, "");
			String resourceTitle =MRXMLUtils.getAttributeValue( element,SessionServiceConst.RESOURCETITLE);
			String resourceNID = MRXMLUtils.getAttributeValue(element,SessionServiceConst.RESOURCENID);
			if(resourceTitle == null) {
                resourceTitle = "";
            }
			if(resourceNID == null) {
                resourceNID = "";
            }
			MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCETITLE, resourceTitle);
			MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCENID, resourceNID);
			if(		SessionServiceConst.FILE.equals(elementName) ||
					SessionServiceConst.PORT.equals(elementName)) {
				Element parent = MRXMLUtils.getParentElement(element);
				String id = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.ID);
				String type = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.TYPE_ATTRIBUTE);
				// if(!SessionResourceUtils.isUsedInConnection(sessionDoc, id))
				// 	continue;
				if(	SessionServiceConst.FILE.equals(elementName) &&
					SessionResourceUtils.isUsedAsDestination(sessionDoc, id)	) {
					continue;
				}
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.TYPE_ATTRIBUTE, type);
				queryDoc.getRootElement().add(qelement);
			}
		}
		
		/**
		 * Get the result for the query
		 */
		Document resultDoc = SessionResourceUtils.getQueryResult(request, xmpp, log, db, queryDoc);
		
		updateOperatorElementInCache(request, resultDoc);
		
		/**
		 * Update the session using the result
		 */
     	List<?> resultList = resultDoc.getRootElement().elements();
     	for(Object object:resultList) {
			Element resourceElement = (Element)object;
			String type = MRXMLUtils.getAttributeValue(resourceElement,SessionServiceConst.TYPE_ATTRIBUTE);		
			Element resource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourceElement, "./"+PolicyAdminConstants.RESOURCE));
			String resourceNIDFromDB =MRXMLUtils.getAttributeValue( resource,SessionServiceConst.NID);
			String elementName = MRXMLUtils.getAttributeValue(resourceElement,SessionServiceConst.ELEMENTNAME);
			String resourceElementResourceTitle = MRXMLUtils.getAttributeValue(resourceElement,SessionServiceConst.RESOURCETITLE);
			String  resourceElementResourceNID= MRXMLUtils.getAttributeValue(resourceElement,SessionServiceConst.RESOURCENID,"");
			if(!"".equals(resourceElementResourceTitle) || !"".equals(resourceElementResourceNID)) {		
				String xpath  = "";
				if(!"".equals(resourceElementResourceTitle)) {
					xpath = "//" + elementName + "[@resourceTitle='" + resourceElementResourceTitle + "']";
				} else {
					xpath = "//" + elementName + "[@resourceNID='"+resourceElementResourceNID+"']";
				}
				
				//Update nid and title from db.
				List<?> sessionElementList = MRXMLUtils.getListXpath(sessionDoc, xpath);
				// Multiple elements could have the same title, need to update all of them
				for(Object o:sessionElementList) {
					Element sessionElement = (Element)o;
					Element parent = sessionElement.getParent();
					String sessionElementType = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.TYPE_ATTRIBUTE);
				
					if(type.equals(sessionElementType)) {
						if(SessionServiceConst.ERROR.equals(resource.getName())) {
							SessionServiceUtils.updateSetupError(log, parent,
									new MRException(MediaRoomServiceExceptionCodes.NOTFOUND,
											MRXMLUtils.getValueXpath(resource, ".//data", "") + " not found"));
						}
						else if(	(SessionServiceConst.SRC_PORT.equals(type) ||
										 SessionServiceConst.DST_PORT.equals(type)) &&
									 !isValidSrcAndDestPort(resource, type) ) {
							SessionServiceUtils.updateSetupError(log, parent,
									new MRException(MediaRoomServiceExceptionCodes.RESOURCE_NOT_A_PORT,
											MRXMLUtils.getAttributeValue(resourceElement, SessionServiceConst.RESOURCETITLE)+ " resource not a port"));
						}
						else {
							//replace the nid only if the session element don't have nid
							String sessionELResourceNID = MRXMLUtils.getAttributeValue(sessionElement, SessionServiceConst.RESOURCENID,"");
							if(sessionELResourceNID.isEmpty() && resourceNIDFromDB != null) {
								MRXMLUtils.addAttribute(sessionElement,SessionServiceConst.RESOURCENID, resourceNIDFromDB);
								sessionELResourceNID = resourceNIDFromDB;
							}
										
							//replace the session element resource title of not matching with db.
							String resourceTitleFromDb = MRXMLUtils.getValueXpath(resource, ".//Title", "");
							if(sessionELResourceNID.equals(resourceNIDFromDB) && !resourceElementResourceTitle.equals(resourceTitleFromDb)){
							    MRXMLUtils.addAttribute(sessionElement, SessionServiceConst.RESOURCETITLE, resourceTitleFromDb);
							    System.out.println("DB resource title and port resource title are not matching. Update resource Title");
							}
							
							
							String entityName = MRXMLUtils.getAttributeValueXpath(parent,SessionServiceConst.NAME , "");
							//set the entity names as resource title in case user did not send 
							if(entityName.isEmpty()) {
								MRXMLUtils.setAttributeValue(parent, SessionServiceConst.NAME, resourceTitleFromDb);
							}
							List<String> entityStreamTypeList = MRXMLUtils.getValuesXpath(resource, ".//StreamTypeList/StreamType");
							
							MRXMLUtils.addAttribute(parent, SessionServiceConst.STREAM_TYPE, StringUtils.join(entityStreamTypeList, " "));
							
							//update date created in session element from db response
							String dateCreated = MRXMLUtils.getValueXpath(resource, ".//"+MediaStoreServiceConst.DATE_CREATED, "");
							if(!dateCreated.isEmpty()) {
								MRXMLUtils.addAttribute(sessionElement, SessionServiceConst.DATE_CREATED, dateCreated);
							}
							if(SessionServiceConst.DST_PORT.equals(type)) {
								MRXMLUtils.setAttributeValue(sessionElement, SessionServiceConst.TRICK_PLAY, MRXMLUtils.getValueXpath(resourceElement, ".//Feature[@Name='TrickPlay']", ""));
							}
							
						}
					}
				}
			}
			if(resourceNIDFromDB != null) {
				updateResourceInfoInCache(request, resourceElement, type);	
			}
			// TBD put all resources in the MRRequest cache
		}
     	
        /**
         * Generate the PVRFile elements in the sessionDoc
         */
		List<?> pvrList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = 'SrcPort']/" + SessionServiceConst.PVR+"[@enable = 'true' ]");
		for(Object object: pvrList) {
			Element pvr = ((Element)object);
			Element srcEntity = pvr.getParent();
			String srcEntityId = MRXMLUtils.getAttributeValue(srcEntity,SessionServiceConst.ID);
			
			// Check for shared PVR
			Element srcEntityElement = MRXMLUtils.getElementXpath(sessionDoc, 
					"//Entity[@" + SessionServiceConst.ID + "='" + srcEntityId + "']");
			Element srcPortResource = MRXMLUtils.getElementXpath(srcEntityElement, ".//" + SessionServiceConst.PORT);
			String sourceNID = MRXMLUtils.getAttributeValue(srcPortResource,SessionServiceConst.RESOURCENID);
			String srcResourceTitle = MRXMLUtils.getAttributeValue(srcPortResource, SessionServiceConst.RESOURCETITLE);

			XMPPTransaction.startBlock("getSharedPVRId");
			//update shared pvr info
			SessionMediaRoomUtils.updateSharedPVRInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcEntityElement, sourceNID,pvr);
			XMPPTransaction.endBlock("getSharedPVRId");			
			if(!MRXMLUtils.getAttributeValue(pvr, SessionServiceConst.SESSION_ID,"").isEmpty()) {
				continue;
			}
		
			String dirId = MRXMLUtils.getAttributeValueXpath(srcEntityElement, ".//" + SessionServiceConst.PVR, SessionServiceConst.DIRID, "");
			String srcStreamType = MRXMLUtils.getAttributeValue(srcEntity,SessionServiceConst.STREAM_TYPE);
			String pvrId = srcEntityId + SessionServiceConst.PVR;
			String pvrConnectionId = pvrId + SessionServiceConst.CONNECTION;
			MRXMLUtils.addAttribute(pvr,SessionServiceConst.CONNECTIONID, pvrConnectionId);
			Document pvrDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.PVRINSTANCEXML, log);
			Element destEntity = MRXMLUtils.getElementXpath(pvrDoc, "//" + SessionServiceConst.ENTITY);
            //destEntity.element(SessionServiceConst.FILE).setAttribute("resourceTitle", pvrId);
			Element fileEl = MRXMLUtils.getChild(destEntity, SessionServiceConst.FILE);
			MRXMLUtils.addAttribute(fileEl, SessionServiceConst.RESOURCETITLE, srcResourceTitle);
			MRXMLUtils.addAttribute(fileEl, SessionServiceConst.DIRID, dirId);		
			Element connection = MRXMLUtils.getElementXpath(pvrDoc, "//" + SessionServiceConst.CONNECTION);
			MRXMLUtils.addAttribute(destEntity,SessionServiceConst.ID, pvrId);
			MRXMLUtils.addAttribute(destEntity,SessionServiceConst.STREAM_TYPE, srcStreamType);
			MRXMLUtils.addAttribute(destEntity, "name", pvrId);
			MRXMLUtils.addAttribute(connection,SessionServiceConst.DSTENTITYID, pvrId);
			MRXMLUtils.addAttribute(connection,SessionServiceConst.SRCENTITYID, srcEntityId);
			MRXMLUtils.addAttribute(connection,SessionServiceConst.ID, pvrConnectionId);
			MRXMLUtils.addAttribute(connection,SessionServiceConst.STREAM_TYPE, srcStreamType);
			entityList.add(destEntity.detach());
			connectionList.add(connection.detach());
		}

		/**
       	 *  Update Groups and metadata. Copy user groups to session groups
       	 */
		boolean isGroupPresent = MRXMLUtils.applyXPath(sessionDoc, "//Groups//Tag/@Name!=\'\'");
		if(!isGroupPresent) {
			Document operatordoc = MRXMLUtils.elementToDocument(request.getOperatorGroups());
			Document groupsDoc = MRXMLUtils.getDocument("<Groups></Groups>");
			List<?> tagElements = MRXMLUtils.getListXpath(operatordoc, "//Tag");
			if(tagElements!=null) {
				for(int i=0;i<tagElements.size();i++) {
					Element tagEL = (Element)tagElements.get(i);
					if(tagEL!= null) {
						String tagName = MRXMLUtils.getAttributeValue(tagEL, "Name");
						String tagValue = MRXMLUtils.getAttributeValue(tagEL, "Value");
						if(MRXMLUtils.applyXPath(groupsDoc, "//Groups/Tag/@Name=\'" + tagName + "\' and //Groups/Tag/@Value=\'" + tagValue + "\'")) {
							continue;
						}
						MRXMLUtils.addElement(groupsDoc.getRootElement(), (Element)tagEL.clone());
					}
				}
			}
			MRXMLUtils.removeChild(sessionDoc.getRootElement(), "Groups");
			MRXMLUtils.addElement(sessionDoc.getRootElement(), (Element)groupsDoc.getRootElement().clone());
		} 
		Element groups = MRXMLUtils.getElementXpath(sessionDoc, "//Groups");
		Element metaData = sessionDoc.getRootElement().element("MetaData");
		if(metaData == null) {
            metaData = MRXMLUtils.stringToElement(
	    "<MetaData id='' name=''><Tag Name='none'>none</Tag></MetaData>");
        }

		/**
		 *  Generate the clipIds for all the pvr recording files if not already specified
		 */
		
		List<?> fileList = MRXMLUtils.getListXpath(sessionDoc, "//Entity/File");		
		for(Object object: fileList) {
			Element element = (Element)object;
			Element parent = MRXMLUtils.getParentElement(element);
			String id = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.ID);
			// String type = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.TYPE);
			if(!SessionResourceUtils.isUsedAsDestination(sessionDoc, id)) {
                continue;
            }
			String clipId = MRXMLUtils.getAttributeValue(element, SessionServiceConst.CLIPID, "");
			if("".equals(clipId)){ // If no clipId specified set the CLIPID to be be the entityId
				element.addAttribute(SessionServiceConst.CLIPID, id);
			}
		}


		/** 
		 * Create all the GroupFiles 
		 */
		
		List<?> recGroupFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + 
				SessionServiceConst.GRPFILE + "[@type='" + SessionServiceConst.SYNCREC + "']");		
		for(Object object: recGroupFileList) {
			Element element = (Element)object;
	        // Generate the mediaGroupId for all GroupFiles
	        String mediaGroupId = Utils.getUUID() + "." + sessionConfigNID + "." + sessionId;
			String clipEntityId = MRXMLUtils.getAttributeValue(element,SessionServiceConst.CLIPENTITYID, "");
			if("".equals(clipEntityId)) { // Use the first recording element as the Sync entity id
				clipEntityId = SessionResourceUtils.getSyncClipEntityId(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
				log.addInfo("SessionResourceUtils:updateSessionDoc using first recording for groupsyncrec setting clipEntityId=" + clipEntityId, MRLog.INFO, MRLog.NOTIFY);
				element.addAttribute(SessionServiceConst.CLIPENTITYID, clipEntityId);
			}
			String clipId = "";
			if(!"".equals(clipEntityId)) {
				Element clipEntity = MRXMLUtils.getElementXpath(sessionDoc,
						"//" + SessionServiceConst.ENTITY + "[@" + SessionServiceConst.ID + "='" + 
						                                  clipEntityId + "']");
				clipId = clipEntity.element(SessionServiceConst.FILE).attributeValue(SessionServiceConst.CLIPID);				
			}
	        MRXMLUtils.addAttribute(element,SessionServiceConst.MEDIAGROUPID, mediaGroupId);
	        MRXMLUtils.addAttribute(element,SessionServiceConst.CLIPID, clipId);
	        if(!"".equals(clipId)) { // Create the groupFile only if there is a clipId
				try {
					SessionResourceUtils.createStreamFileResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
							sessionDoc, element,"", now, SessionServiceConst.GRPFILE, groups,  sessionId, sessionConfigNID,"");
				}
				catch(Exception e) {
					e.printStackTrace();
					SessionServiceUtils.updateSetupError(log, element, e);
				}
	        }
		}

		/** 
		 * Create all the PVR and Recording Files 
		 */
		
		//Get recording access xpath if the session has some recording
		String recordAccessXpath = "";		
		List<?> recordingFileList = MRXMLUtils.getListXpath(sessionDoc, "//Entity[@type='"+SessionServiceConst.RECFILE+"']");	
		if(recordingFileList !=null && !recordingFileList.isEmpty()) {
			recordAccessXpath = SessionPolicyUtils.getDirAccessXPath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, PolicyAdminConstants.RECORD_ROLE_ACTION);			
		}
		
		//Get pvr access xpath if the session has some requested pvr
		String pvrAccessXpath = SessionPolicyUtils.getPVRAccessXpath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
				
		for(Object object: fileList) {
			Element element = (Element)object;
			Element parent = MRXMLUtils.getParentElement(element);
			String id = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.ID);
			String type = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.TYPE_ATTRIBUTE);
			if(!SessionResourceUtils.isUsedAsDestination(sessionDoc, id)) {
                continue;
            }
			try {
				//Adding dir support
				String dirID = MRXMLUtils.getAttributeValue(element, SessionServiceConst.DIRID,"");				
				String requestedDirNID = getRequestedDirNID(dirID, sessionDoc);	
				String targetAccessXpath = "";
				if(type.equals(SessionServiceConst.RECFILE)){
					if(recordAccessXpath.equals(PolicyAdminConstants.NULL)){
						throw new MRException(SessionServiceExceptionCodes.DIR_ACCESS_POLICY_DENY,"Dir Access Policy Deny");
					}
					targetAccessXpath = recordAccessXpath;
				}
				
				if(type.equals(SessionServiceConst.PVRFILE)){
					if(pvrAccessXpath.equals(PolicyAdminConstants.NULL)){
						throw new MRException(SessionServiceExceptionCodes.DIR_ACCESS_POLICY_DENY,"Dir Access Policy Deny");
					}
					targetAccessXpath = pvrAccessXpath;
				}
				
				SessionResourceUtils.createStreamFileResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
						sessionDoc, element,requestedDirNID, now, type, groups,  sessionId, sessionConfigNID,targetAccessXpath);
				String clipId = MRXMLUtils.getAttributeValue(element, SessionServiceConst.CLIPID);
				if("".equals(clipId)){ // If no clipId specified set the CLIPID to be be the entityId
					element.addAttribute(SessionServiceConst.CLIPID, id);
				}
			}
			catch(Exception e) {
				e.printStackTrace();
				SessionServiceUtils.updateSetupError(log, parent, e);
			}
		}

		// Update the mediaGroupId and clipId for all the file objects using data read from the db
		for(Object object: fileList) {
			Element element = (Element)object;
			SessionResourceUtils.updateFileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, element);
		}

		// Update the mediaGroupId and clipId for all the group file objects using data read from the db
		List<?> groupFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.GRPFILE );		
		for(Object object: groupFileList) {
			Element element = (Element)object;
			SessionResourceUtils.updateFileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, element);
		}

		// Update the playback syncEntityId
		List<?> plyGroupFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + 
				SessionServiceConst.GRPFILE + "[@type='" + SessionServiceConst.SYNCPLY + "']");		
		for(Object object: plyGroupFileList) {
			Element element = (Element)object;
			String clipId = MRXMLUtils.getAttributeValue(element, SessionServiceConst.CLIPID, "");
			// Find the sourceEntity corresponding to the File which has this clipId
			Element clipFileElement = MRXMLUtils.getElementXpath(sessionDoc,  "//" +
					SessionServiceConst.FILE + "[@" + SessionServiceConst.CLIPID + "='" + clipId + "']");
			if(clipFileElement == null) {
                continue;
            }
			Element parent = MRXMLUtils.getParentElement(clipFileElement);
			String entityType = MRXMLUtils.getAttributeValue(parent, SessionServiceConst.TYPE_ATTRIBUTE, "");
			if(!SessionServiceConst.PLYFILE.equals(entityType)) {
                continue;
            }
			String entityId = MRXMLUtils.getAttributeValue(parent,SessionServiceConst.ID);
			element.addAttribute(SessionServiceConst.SYNCENTITYID, entityId);
			SessionResourceUtils.updateFileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, element);
		}

		// Ensure that every clipFile has a corresponding group file
        if(!fileList.isEmpty()) {
        	for(Object o : fileList) {
		        Element file = (Element)o;
		        String clipMediaGroupId = file.attributeValue(SessionServiceConst.MEDIAGROUPID);
				Element grpFile = MRXMLUtils.getElementXpath(sessionDoc, "//" + 
						SessionServiceConst.GRPFILE + "[@" + SessionServiceConst.MEDIAGROUPID + "='" + clipMediaGroupId + "']");		
				if(grpFile == null) {
                    SessionResourceUtils.addGrpFile(request, xmpp, log, db, sessionDoc, file);
                }
        	}
        }

		//update connection  type and streamType in connection list
		List<?> connList = MRXMLUtils.getListXpath(sessionDoc, "//Connection");
		String type= "";
		String streamType = "";
		Element connEL = null;
		for(Object object:connList){
			connEL = (Element)object;			
			type = getConnectionType(connEL,MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.DSTENTITYID,""),
					MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.SRCENTITYID,""),sessionDoc);
			streamType = getConnectionStreamType(connEL, MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.SRCENTITYID,""),sessionDoc);
			MRXMLUtils.setAttributeValue(connEL, SessionServiceConst.TYPE_ATTRIBUTE, type);
			MRXMLUtils.setAttributeValue(connEL, SessionServiceConst.STREAM_TYPE, streamType);
		}

	}
	
	public static String getConnectionStreamType(Element connEL, String srcEntityID, Document sessionDoc) throws MRException, Exception {
		Element entityEL = null;
		String srcEntityStreamType = null;
		
		entityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityID+"']");
		srcEntityStreamType = MRXMLUtils.getAttributeValue(entityEL, SessionServiceConst.STREAM_TYPE, "");
		
		return srcEntityStreamType;
	}

	public static String getConnectionType(Element connEL,String destEntityID,String srcEntityID, Document sessionDoc) throws MRException,Exception{
		Element entityEL = null;		
	    String entityType = null;
	    
	    entityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+destEntityID+"']");
	    entityType = MRXMLUtils.getAttributeValue(entityEL, SessionServiceConst.TYPE_ATTRIBUTE,"");
	    if(entityType.equals(SessionServiceConst.PVRFILE)){
	    	return SessionServiceConst.PVR; 
	    } 
	    
	    if(entityType.equals(SessionServiceConst.RECFILE)){
	    	return SessionServiceConst.RECORD;
	    }
	    
	    entityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityID+"']");
	    entityType = MRXMLUtils.getAttributeValue(entityEL, SessionServiceConst.TYPE_ATTRIBUTE,"");
	   
	    if(entityType.equals(SessionServiceConst.PLYFILE)){
	    	return SessionServiceConst.PLAYBACK;
	    }
	  
		return SessionServiceConst.VIEW;
	}

	//finds the dir element for given dirId and returns the requested dir nid.
	static private String getRequestedDirNID(String dirID,Document sessionDoc) throws MRException,Exception {
		
		if(dirID.isEmpty()){
			return "";
		}
		String dirXpath = "//" + SessionServiceConst.DIR +"[@"+SessionServiceConst.ID+"='"+dirID+"']";		
		Element dirEl = MRXMLUtils.getElementXpath(sessionDoc,dirXpath);				
		if(dirEl == null) {
            return "";
        }
		return MRXMLUtils.getAttributeValue(dirEl, SessionServiceConst.RESOURCENID,"");
	}
	
    static void addGrpFile(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
    		Document sessionDoc, Element file) throws Exception {
        String clipMediaGroupId = file.attributeValue(SessionServiceConst.MEDIAGROUPID);
        if((clipMediaGroupId == null) || ("".equals(clipMediaGroupId))) {
            return;
        }        
        String[] mediaGroupIdXpath = {
                "//"+MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT+
                "[" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + clipMediaGroupId + "' and Type='"+MediaRoomServiceConst.MEDIAGROUP+"'] "
            };
        
        List<?> groupFileList = MediaStoreServiceUtils.getMediaStreamFileResourceList(mediaGroupIdXpath, request.getUserJID(), log, db);
        if(groupFileList.isEmpty())
         {
            return; // Group not found
        }
        Element groupFileResource = (Element)groupFileList.get(0);
        String resourceNID = groupFileResource.attributeValue(SessionServiceConst.NID);
		Element mediaGroup = MRXMLUtils.getElementXpath(groupFileResource, "//" + MediaRoomServiceConst.MEDIAGROUPID);
		String mediaGroupId = mediaGroup.getText();
		String clipId = file.attributeValue(MediaRoomServiceConst.CLIPID);
		
		Element groupFile = MRXMLUtils.stringToElement("<" + SessionServiceConst.GRPFILE + "/>");
		groupFile.addAttribute(SessionServiceConst.RESOURCENID, resourceNID);
		groupFile.addAttribute(SessionServiceConst.MEDIAGROUPID, mediaGroupId);
		groupFile.addAttribute(SessionServiceConst.CLIPID, clipId);
		Element session = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.SESSION);
		Element grpFileList = session.element(SessionServiceConst.GRPFILELIST);
		if(grpFileList == null) {
			grpFileList = MRXMLUtils.stringToElement("<" + SessionServiceConst.GRPFILELIST + "/>");
			session.add(grpFileList);
		}
		grpFileList.add(groupFile);
    }

	public static void updateFileElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		Element element) throws Exception {
		String clipId = MRXMLUtils.getAttributeValue(element, SessionServiceConst.CLIPID, "");
		String mediaGroupId = MRXMLUtils.getAttributeValue(element, SessionServiceConst.MEDIAGROUPID, "");
		if("".equals(clipId) || "".equals(mediaGroupId)) {
			String resourceNID = MRXMLUtils.getAttributeValue(element, SessionServiceConst.RESOURCENID,"");
			if("".equals(resourceNID))
             {
                return; // No resourceNID 
            }
			Document resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
			// System.out.println(MRXMLUtils.documentToString(resourceDoc));
			Element mediaGroup = MRXMLUtils.getElementXpath(resourceDoc, "//" + MediaRoomServiceConst.MEDIAGROUPID);
			String mediaType = MRXMLUtils.getValueXpath(resourceDoc, "//" + SessionServiceConst.TYPE_ELEMENT,"");
			if( mediaType.equals(MediaRoomServiceConst.MEDIACLIP)){
				String playLength = MRXMLUtils.getValueXpath(resourceDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH,"");
				element.addAttribute(SessionServiceConst.PLAYLENGTH, playLength);
			}
			mediaGroupId = MRXMLUtils.getValue2(mediaGroup,"");
			element.addAttribute(SessionServiceConst.MEDIAGROUPID, mediaGroupId);
			clipId = MRXMLUtils.getAttributeValue(mediaGroup,MediaRoomServiceConst.CLIPID,"");
			element.addAttribute(SessionServiceConst.CLIPID, clipId);
		}
		
	}
	
	/*
	public static void main(String []args) throws Exception {
		String xmlFilePath = "/com/ipvs/test/logfiles/SampleSession.xml";
		Document sessionDoc = MRXMLUtils.loadXMLResource(xmlFilePath, null);
		MRRequest request = MRRequest.createRequest("userJID");
		request.setCache(new MRRequestCache());
		XMPPI xmpp = null;
		MRDB db = null;
		MRLog log = null;
		MRResourceFactoryI resourceFactory = null;
		ServiceAgentFactoryI serviceAgentFactory = null;
		Element sessionConfigElement = (Element)MRXMLUtils.getElementXpath(sessionDoc, "//SessionConfig").clone();
		Document sessionDoc = MRXMLUtils.elementToDocument(sessionConfigElement);
		SessionResourceUtils.updatesessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
		System.out.println(MRXMLUtils.documentToString(sessionDoc));
	}
	*/
	
	public static Element createOperatorQueryElement(MRRequest request) throws MRException,Exception  {
		//add operator query
		Element qelement = MRXMLUtils.stringToElement("<queryitem/>");
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.OPERATOR);
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCEJID, request.getUserJID());
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCETITLE, "");
		MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCENID, "");
		return qelement;
	}
	
	public static void updateOperatorElementInCache(MRRequest request,Document resultDoc) throws MRException,Exception  {
		Element operatorElement = MRXMLUtils.newElement(SessionServiceConst.OPERATOR);
		Element operatorElFromQueryResult = MRXMLUtils.getElementXpath(resultDoc, "//*[@"+SessionServiceConst.TYPE_ATTRIBUTE+"='"+SessionServiceConst.OPERATOR+"']"); 
		operatorElFromQueryResult.detach();
		MRXMLUtils.addElement(operatorElement,(Element)MRXMLUtils.getElement(operatorElFromQueryResult, AMConst.User).detach());
		MRXMLUtils.addElement(operatorElement,(Element)MRXMLUtils.getElement(operatorElFromQueryResult, PolicyAdminConstants.RESOURCE).detach());
		Element operatorResourceEl = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(operatorElFromQueryResult, "./"+PolicyAdminConstants.RESOURCE));
		String operatorResourceNID =MRXMLUtils.getAttributeValue(operatorResourceEl,SessionServiceConst.NID);		
		
		//update operator info and operator asset resource in the cache
		request.setOperatorGroups(operatorElement);		
		if(operatorResourceEl!=null) {				
			Document resourceDoc = MRXMLUtils.elementToDocument(operatorResourceEl);				
			request.getCache().put(operatorResourceNID, resourceDoc);				
		}
	}
	
	
	public static void updateResourceInfoInCache(MRRequest request,Element resourceElement,String type) throws MRException,Exception  {
		Element resource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourceElement, "./"+PolicyAdminConstants.RESOURCE));
		String resourceNIDFromDB = MRXMLUtils.getAttributeValue(resource, SessionServiceConst.NID,"");		
		Document resourceDoc = MRXMLUtils.elementToDocument(resource);
		request.getCache().put(resourceNIDFromDB, resourceDoc);
		
		//if the resource type is  srcport/dstport update src/dst User information in the cache 
		// policies will look for the key and don't do multiple reads in the process of same request				
		if(type.equals(SessionServiceConst.SRC_PORT) || type.equals(SessionServiceConst.DST_PORT)) {
			Element resourceUserElement = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourceElement, "./"+PolicyAdminConstants.USER));
			if(resourceUserElement ==null) {
				resourceUserElement = MRXMLUtils.stringToElement("<"+PolicyAdminConstants.USER+"/>");
			}
			request.getCache().put(resourceNIDFromDB+"."+PolicyAdminConstants.USER, resourceUserElement);
		}	
	}

}
