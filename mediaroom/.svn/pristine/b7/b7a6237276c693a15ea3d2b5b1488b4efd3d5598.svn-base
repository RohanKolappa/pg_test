package com.ipvs.sessionservice.handler.base;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;

public abstract class GetFileBaseRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
	public static final String MEDIA_FORMAT_STANDARD= "Standard";
	public static final String MEDIA_FORMAT_PROPRIETARY = "Proprietary";
	public static final String MEDIA_FORMAT_BOTH = "Both";

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
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

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String queryFileName,String requestType) throws Exception {

		try {
			Document doc = request.getMessage().getDocument();
			Element getFileElement = MRXMLUtils.getElementXpath(doc, "//"+requestType);

			String whereParentDirNID = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_PARENT_DIR_NID, "");
			String whereSessionConfigNID = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_SESSION_CONFIG_NID, "");
			String whereMetaDataExpr = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_METADATA_EXPR, "");
			String whereState = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_STATE, "");
			String searchTextSubStrIn = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.SEARCH_TEXT_SUB_STR_IN, "");
			String whereTextSubStr = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
			String whereType = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_TYPE, "");
			String whereMediaGroupID = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_MEDIA_GROUP_ID,"");
			String whereNID = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_NID,"");
			String whereTagExpr = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_TAG_EXPR,"");
			String whereDateRange = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_DATE_RANGE,"");
			String detail = MRXMLUtils.getAttributeValue(getFileElement, "selectFormat", "BASIC");
			String whereDownloadState = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_DOWNLOAD_STATE, "");

			List<String> listOfWhereXpath = new ArrayList<String>();
			Hashtable<String, String> customeTokenMappingTable= new Hashtable<String, String>();
			Hashtable<String, String> fileMediaTypeTable = new Hashtable<String, String>();

			//generate common parameters xpath			  
			GetObjectUtils.generateXPath(whereDateRange, whereNID, whereTagExpr, whereTextSubStr,"", listOfWhereXpath, SessionServiceConst.FILE);

			//generate file related xpath which will fall in regular xpath search
			generateFileXPath(listOfWhereXpath,customeTokenMappingTable, whereState, whereSessionConfigNID, whereMediaGroupID,whereType, whereDownloadState, fileMediaTypeTable);

			//generate xpath for whereTextSubString, MetaData and Bookmark Search
			String subStringXpath = generateFileTextsubStrAndMedataDataXPath(listOfWhereXpath, customeTokenMappingTable, whereTextSubStr, searchTextSubStrIn, whereMetaDataExpr,requestType );


			//generate parent dir xpath
			String whereParentDirNIDXPath = "";
			if (!"".equals(whereParentDirNID)) {
				whereParentDirNIDXPath = GetObjectUtils.generateXPath(whereParentDirNID, SessionServiceConst.WHERE_PARENT_DIR_NID, SessionServiceConst.FILE, SessionServiceConst.NID_CONST);
			}		

			String whereXPath = GetObjectUtils.generateWhereXpathFromList(listOfWhereXpath, "and", SessionServiceConst.FILE);

			if(requestType.equals(SessionServiceConst.GET_FILE)) {
				//update xpath in file query
				updateCustomXpath(customeTokenMappingTable, whereXPath, whereParentDirNIDXPath, whereState, whereDownloadState, subStringXpath, fileMediaTypeTable);
				updateBothXpath(customeTokenMappingTable, whereXPath, whereParentDirNIDXPath, whereState, subStringXpath);
				updateBothMediaTypeXpath(customeTokenMappingTable);

			} else {
				//update file xpath for get pvr and get orphan media 			
				updateFileXpath(customeTokenMappingTable, whereXPath, whereParentDirNIDXPath);
			}

			//update sort by
			String sortBy = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.SORT_BY, "");
			String sortByMetaData = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.SORT_BY_METADATA,"");
			String sortByTagName = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.SORT_BY_TAG_NAME,"");

			if (sortBy.equals(SessionServiceConst.PLAY_LENGTH)) {
				sortBy =  MediaStoreServiceConst.PLAY_LENGTH + "/" + SessionServiceConst.NUMBER_CONST;
			} else if (sortBy.equals(SessionServiceConst.TITLE)) {
				sortBy =  MediaStoreServiceConst.INFO_ELEMENT_NAME+"/"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME;
			} else if (sortBy.equals(SessionServiceConst.DATE_CREATED)) {
				sortBy =  MediaStoreServiceConst.DATE_CREATED + "/" + SessionServiceConst.NUMBER_CONST;
			} else if (sortBy.equals(SessionServiceConst.STATE)) {
				sortBy =  AMConst.STATEFLAG;
			} else if (!sortByMetaData.isEmpty()) {
				sortBy =  MediaStoreServiceConst.META_DATA_ELEMENT_NAME+"/"+MediaStoreServiceConst.TAG_ELEMENT_NAME+"[@"+MediaStoreServiceConst.NAME_ATTRIBUTE+"='"+sortByMetaData+"']";
			} else if (!sortByTagName.isEmpty()) {
				sortBy =  MediaStoreServiceConst.GROUPS_EL_NAME+"/"+MediaStoreServiceConst.TAG_ELEMENT_NAME+"[@"+MediaStoreServiceConst.NAME_ATTRIBUTE+"='"+sortByTagName+"']/@"
						+MediaStoreServiceConst.VALUE_ATTRIBUTE;
			} else {
				sortBy =  MediaStoreServiceConst.MEDIA_DIR_SIZE + "/" + SessionServiceConst.NUMBER_CONST;
			} 
			Document responseDoc =  this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getFileElement, SessionServiceConst.FILE,"",sortBy, listOfWhereXpath,  queryFileName, customeTokenMappingTable,null);
			formatFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, getFileElement, responseDoc, detail);
			return responseDoc;
		} catch(MRException me) {
			System.out.println(me.getCode());
			System.out.println(me.getData());
			throw me;
		}
		catch(Exception e) {
			e.printStackTrace();
			throw new MRException(SessionServiceExceptionCodes.OTHER, MRException.getStackTrace(e));
		}
	}
	public void formatFileResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element getFileElement, Document responseDoc,String detail) throws Exception {

		AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
		Element apiKeyTableEl = appServerAgent.getAPIKey();


		String expireTime = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceResourceConst.EXPIRE_TIME, "");
		String authScheme = HttpAuthentication.getAuthScheme(expireTime);

		if(expireTime.isEmpty()) {
			expireTime = MRXMLUtils.getAttributeValueXpath(apiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE, String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
		}
		String dstTitle = getDstTitle(getFileElement, detail, request);
		Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log, xmpp, db, serviceAgentFactory, request, apiKeyTableEl, dstTitle, request.getUserJID(),authScheme);

		String msIPAddress =  MRXMLUtils.getAttributeValue(appResourceInfo, "ipAddress","");
		String dstNID =MRXMLUtils.getAttributeValue(appResourceInfo, "dstNID","");
		String dstTemplateTitle = MRXMLUtils.getAttributeValue(appResourceInfo, "templateTitle","");

		if(!dstNID.isEmpty()) {
			String subscriptionID = MRXMLUtils.getAttributeValue( MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//GetFile"), SessionServiceConst.SUBSCRIPTION_ID,"");
			AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).setDestinationInformation(SessionServiceConst.FILE, request.getUserJID(),
					subscriptionID, dstNID, msIPAddress,dstTemplateTitle,authScheme,expireTime);		    	
		}


		List<?> fileList = MRXMLUtils.getListXpath(responseDoc, "//File");
		if(fileList != null && !fileList.isEmpty()) {


			long now = xmpp.getCurrentTimeMillis();


			String timestamp=(HttpAuthentication.INFINITE_EXPIRY.equals(expireTime))?HttpAuthentication.INFINITE_EXPIRY :String.valueOf(now + (Long.parseLong(expireTime) * 1000));
			String signature = HttpAuthentication.generateSignature(apiKeyTableEl, timestamp, authScheme, request, xmpp, log, db, resourceFactory, serviceAgentFactory);				
			String apiKey = MRXMLUtils.getAttributeValueXpath(apiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");

			String pemXpath = null;

			if(detail.equals(SessionServiceResourceConst.SELECTFORMAT_BASICURL) || detail.equals(SessionServiceResourceConst.SELECTFORMAT_EXTENDEDURL)
					|| detail.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO) || detail.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO)) {			
				// Check policy xpath
				try {
					pemXpath = PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.EXPORT_ROLE_ACTION, AMConst.MEDIA_STREAM_FILE_RESOURCE, request,
							xmpp, log, db, resourceFactory, serviceAgentFactory);								

					if (pemXpath!=null && !pemXpath.isEmpty() && pemXpath.length() > 0)
						pemXpath="//File"+pemXpath;

				} catch (MRException e) {
					// continue with the code if checkPolicyPermissions throws an exception (when pemXpath is null)
					log.addInfo("User doesn't have the permission :" + e.getMessage());
					pemXpath = null;
				} 
			}

			String protocol = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//Header/Protocol", "http");
			for(Object obj:fileList) {
				Element fileEl = (Element)obj;
				String fileNID = MRXMLUtils.getAttributeValue(fileEl, "NID");
				if(detail.equals(SessionServiceResourceConst.SELECTFORMAT_BASICURL) || detail.equals(SessionServiceResourceConst.SELECTFORMAT_EXTENDEDURL)
						|| detail.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO) || detail.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO)) {
					String mediaURL = "";
					//do not check for destNID empty, send mediaURL anyway
					if(MRXMLUtils.getAttributeValue(fileEl, "fileType", "").equals(MediaStoreServiceConst.MEDIA_IMAGE_TYPE)){  
						mediaURL = protocol+"://" + msIPAddress + "/controller/snapshot?mediaID=" + fileNID;
						mediaURL = HttpAuthentication.appendSystemAuthenticationQueryParameters(mediaURL, apiKey, timestamp, authScheme, signature, request.getUserJID(),dstTemplateTitle);

						//append empty value for destination
						if(!dstNID.isEmpty())
							mediaURL = mediaURL.concat("&DstPort.portNID=" + dstNID);
					}

					if(MRXMLUtils.getAttributeValue(fileEl, "fileType", "").equals(MediaRoomServiceConst.MEDIACLIP)){

						mediaURL = protocol+"://" + msIPAddress + "/clients/player?requestName=Playback&SrcFile.fileNID=" + fileNID;
						mediaURL = HttpAuthentication.appendSystemAuthenticationQueryParameters(mediaURL, apiKey, timestamp, authScheme, signature, request.getUserJID(),dstTemplateTitle);

						if (!dstNID.isEmpty())
							mediaURL = mediaURL.concat("&DstPort.portNID=" + dstNID);

						String fallback = "false";
						if(MRXMLUtils.getAttributeValue(fileEl, "hls", "").equals("true"))
							fallback = "true";

						mediaURL = mediaURL.concat("&fallback=" + fallback);
					}

					MRXMLUtils.setAttributeValue(fileEl, SessionServiceResourceConst.MEDIAURL_ATTRIBUTE, mediaURL);

					String thumbnailConfig = MRXMLUtils.getValueXpath(fileEl, ".//ThumbnailConfig/Enable", "");
					if("true".equals(thumbnailConfig) || MRXMLUtils.getAttributeValue(fileEl, "fileType", "").equals(MediaRoomServiceConst.MEDIAIMAGE)) {
						String thumbnailURL = HttpAuthentication.generateThumbnailURL(fileNID, "recording", timestamp, msIPAddress,authScheme, log, serviceAgentFactory, request,db,xmpp,resourceFactory,dstTemplateTitle);
						MRXMLUtils.setAttributeValue(fileEl, SessionServiceResourceConst.THUMBNAILURL_ATTRIBUTE, thumbnailURL);
					}

					List<?> mediaPreparedList = MRXMLUtils.getListXpath(fileEl, ".//Media[@state='Prepared']");
					if(mediaPreparedList != null && !mediaPreparedList.isEmpty()) {

						// Applying permission xpath for downloadURL
						if (pemXpath!=null && (pemXpath.isEmpty() || MRXMLUtils.getListXpath(fileEl, pemXpath).size() > 0)) {

							for (Object object : mediaPreparedList) {
								Element mediaEl = (Element) object;
								String format = MRXMLUtils.getAttributeValue(mediaEl, "format", "");
								String downloadURL = protocol+"://" + msIPAddress + "/controller/File/Download?mediaID=" + fileNID;
								downloadURL = HttpAuthentication.appendSystemAuthenticationQueryParameters(downloadURL, apiKey, timestamp, authScheme, signature, request.getUserJID(),dstTemplateTitle);

								downloadURL+= "&format=" + format.toLowerCase()+"&expireTime="
										+ expireTime+"&persistence=" + SessionServiceResourceConst.PERSISTENCE_PERMANENT
										+ "&type=" + SessionServiceResourceConst.EXPORT_TYPE;
								if(!dstNID.isEmpty())
									downloadURL = downloadURL + "&DstPort.portNID=" + dstNID;
								MRXMLUtils.setAttributeValue(mediaEl, "downloadURL", downloadURL);
								MRXMLUtils.removeChild(fileEl, SessionServiceResourceConst.API_KEY_TABLE);
							}

						}
					}

				}
				MRXMLUtils.removeChild(fileEl, "ThumbnailConfig");
				MRXMLUtils.removeChildren(fileEl, SessionServiceResourceConst.API_KEY_TABLE);
			}
		}

		GetObjectUtils.subscribeUser(getFileElement, responseDoc, SessionServiceConst.FILE, request, serviceAgentFactory);


		/*if(detail.equals(SessionServiceResourceConst.SELECTFORMAT_EXTENDED)) {
			String duplicatecol = MRXMLUtils.getAttributeValue(getFileElement, "duplicate", "");
			if(duplicatecol.equals("true")) {
				for(Object obj:fileList) {
					DuplicateFileColl.getInstance().duplicatecol(request, log, db, xmpp,(Element) obj);
				}
			}
		}*/


		XMPPTransaction.startBlock("fillConnectionStateInfoInResponseDoc");			
		if(detail.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO) || detail.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO) ){
			fillConnectionStateInfoInResponseDoc(responseDoc,request,xmpp,db,log,serviceAgentFactory,resourceFactory, detail, SessionServiceConst.FILE, msIPAddress,authScheme,dstTemplateTitle);
		}			
		XMPPTransaction.endBlock("fillConnectionStateInfoInResponseDoc");


	}

	private void updateFileXpath(Hashtable<String, String> customeTokenMappingTable,
			String whereXPath, String whereParentDirNIDXPath) {
		//StringBuffer xpathBuffer = new StringBuffer();
		StringBuffer xpathbuffer = new StringBuffer();
		if(!whereXPath.isEmpty()) {
			xpathbuffer.append(whereXPath);
		}  	

		if(!whereParentDirNIDXPath.isEmpty()){
			if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");
			xpathbuffer.append(whereParentDirNIDXPath);
		}
		String xpath =  "";

		if(!xpathbuffer.toString().isEmpty()) {
			xpath = " and ("+xpathbuffer.toString() +")";
		}

		customeTokenMappingTable.put(SessionServiceConst._FILEXPATH_,xpath);

	}

	private void updateMediaTypeXpath(StringBuffer xpathbuffer, String whereXPath, String subStringXpath) {
		if(!whereXPath.isEmpty()) {
			/*if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");*/
			xpathbuffer.insert(0, whereXPath + (!xpathbuffer.toString().isEmpty()?" and " : " " ));
			//xpathbuffer.append(whereXPath);
		}
		if(!subStringXpath.isEmpty()){
			if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");
			xpathbuffer.append(subStringXpath);
		}
	}

	private void updateCustomXpath(Hashtable<String, String> customeTokenMappingTable, String whereXPath, String whereParentDirNIDXPath,
			String whereState,	String whereDownloadState, String subStringXpath, Hashtable<String, String> fileMediaTypeTable) {
		String xpath =  "";
		String searchInFiles = customeTokenMappingTable.get(SessionServiceConst._SEARCHINFILES_);

		if (SessionServiceConst.MEDIA_CUSTOM.equals(searchInFiles) ) {
			StringBuffer xpathbuffer = new StringBuffer();
			String orDelimiter = "";
			if (!fileMediaTypeTable.isEmpty()){
				xpathbuffer.append("(");
				for (String value : fileMediaTypeTable.keySet()) {
					xpathbuffer.append(orDelimiter + fileMediaTypeTable.get(value));
					orDelimiter = " or ";
				}
				xpathbuffer.append(")");
			}
			updateMediaTypeXpath(xpathbuffer, whereXPath, subStringXpath);

			if (!fileMediaTypeTable.containsKey(SessionServiceConst.MEDIA_GROUP))
				updateDirPathXpath(xpathbuffer, whereParentDirNIDXPath, whereState);

			// Add whereDownloadState filter if not empty
			if(!xpathbuffer.toString().isEmpty()) {
				String whereDownloadStateXPath = generateWhereDownloadStateXPath(whereDownloadState);
				xpathbuffer.append(whereDownloadStateXPath);
			}
			xpath = xpathbuffer.toString().isEmpty()?"":"[ " + xpathbuffer.toString() + " ]";
		}

		customeTokenMappingTable.put(SessionServiceConst._FILECUSTOMQUERYMEDIAXPATH_,xpath);
	}

	private void updateDirPathXpath(StringBuffer xpathbuffer, String whereParentDirNIDXPath, String whereState) 
	{

		if(!whereParentDirNIDXPath.isEmpty()){
			if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");
			xpathbuffer.append(whereParentDirNIDXPath);
		}

		if(whereState.isEmpty()){ return; }

		if(whereState.equals(SessionServiceConst.READY) || whereState.equals(SessionServiceConst.OFFLINE)) {
			if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");
			xpathbuffer.append(".//ParentMediaDirNID[.=$dirNIDList]");
		} else {
			String whereXPath = GetObjectUtils.generateXPath(whereState, SessionServiceConst.WHERE_STATE, SessionServiceConst.FILE, SessionServiceConst.STATE_CONST);
			xpathbuffer.append(whereXPath);
		}
	}

	private void updateBothXpath(Hashtable<String, String> customeTokenMappingTable, String whereXPath, String whereParentDirXpath,
			String whereState, String subStringXpath) {

		StringBuffer xpathbuffer = new StringBuffer();
		if(!whereXPath.isEmpty()) {
			xpathbuffer.append(whereXPath);
		}
		if(!subStringXpath.isEmpty()){
			if(!xpathbuffer.toString().isEmpty())
				xpathbuffer.append(" and ");
			xpathbuffer.append(subStringXpath);
		}
		String fileBothQueryXpath =  "";

		if(!xpathbuffer.toString().isEmpty()) {
			fileBothQueryXpath = "("+xpathbuffer.toString() +") and ";
		}		

		StringBuffer fileBothQueryParentDirXPathBuffer = new StringBuffer();
		String fileBothQueryParentDirXPath ="";
		updateDirPathXpath(fileBothQueryParentDirXPathBuffer, whereParentDirXpath, whereState);
		if(!fileBothQueryParentDirXPathBuffer.toString().isEmpty()){
			fileBothQueryParentDirXPath = " and ("+fileBothQueryParentDirXPathBuffer.toString() +")";
		}		

		customeTokenMappingTable.put(SessionServiceConst._FILEBOTHQUERYXPATH_, fileBothQueryXpath);
		customeTokenMappingTable.put(SessionServiceConst._FILEBOTHQUERYPARENTXPATH_, fileBothQueryParentDirXPath);
	}

	private void updateBothMediaTypeXpath(Hashtable<String, String> customeTokenMappingTable) {
		String searchInFiles = customeTokenMappingTable.get(SessionServiceConst._SEARCHINFILES_);
		String fileBothMediaTypeXpath = "";
		String mediaWithoutMediaGroup = "";
		if(searchInFiles.equals(SessionServiceConst.BOTH)) {
			fileBothMediaTypeXpath = "and (Info/MediaStoreFileInfo/Type='MediaClip')";
			mediaWithoutMediaGroup = "and .//Type='MediaClip'";
		}
		customeTokenMappingTable.put(SessionServiceConst._FILEBOTHQUERYMEDIATYPEXPATH_, fileBothMediaTypeXpath);
		customeTokenMappingTable.put(SessionServiceConst._MEDIAWITHOUTMEDIAGROUP_, mediaWithoutMediaGroup);
	}

	private String generateFileTextsubStrAndMedataDataXPath(List<String> listOfWhereXpath, Hashtable<String, String> customeTokenMappingTable,String whereTextSubStr, String searchTextSubStrIn,
			String whereMetaDataExpr, String requestType) {


		//update bookmark search string
		String whereBookmarkXpath = "";
		if(!searchTextSubStrIn.isEmpty() && searchTextSubStrIn.contains(SessionServiceConst.SEARCH_TEXT_SUB_STR_IN_BOOKMARK) && !whereTextSubStr.isEmpty()) {
			whereBookmarkXpath = GetObjectUtils.generateXPath(whereTextSubStr, SessionServiceConst.WHERE_TEXT_SUB_STR, SessionServiceConst.BOOKMARK, SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST);
		} 

		String searchInBookmarks = SessionServiceConst.TRUE;
		if(whereBookmarkXpath.isEmpty()) {
			searchInBookmarks = SessionServiceConst.FALSE;
		} else {
			whereBookmarkXpath = "[" + whereBookmarkXpath +"]";
		}

		customeTokenMappingTable.put(SessionServiceConst._SERACHINBOOKMARK_, searchInBookmarks);
		customeTokenMappingTable.put(SessionServiceConst._BOOKMARKXPATH_, whereBookmarkXpath);   	

		//update metadata
		String whereMetaDataExprXPath = ""; 
		if (!"".equals(whereMetaDataExpr)) {
			whereMetaDataExprXPath = GetObjectUtils.generateXPath(whereMetaDataExpr, SessionServiceConst.WHERE_METADATA_EXPR, SessionServiceConst.FILE, "");
		}

		//update text sub string
		String whereTextSubStrXPath = GetObjectUtils.generateXPath(whereTextSubStr, SessionServiceConst.WHERE_TEXT_SUB_STR,searchTextSubStrIn, SessionServiceConst.FILE, SessionServiceConst.WHERE_TEXT_SUBSTRING_CONST);

		if(requestType.equals(SessionServiceConst.GET_FILE)) {
			StringBuffer xpathbuffer = new StringBuffer();
			if(!whereTextSubStr.isEmpty()) {
				xpathbuffer.append("("+whereTextSubStrXPath+")");
			}
			if(!whereMetaDataExpr.isEmpty()){
				if(!xpathbuffer.toString().isEmpty())
					xpathbuffer.append(" or ");
				xpathbuffer.append("("+whereMetaDataExprXPath+")");
			}

			if(!whereBookmarkXpath.isEmpty()){
				if(!xpathbuffer.toString().isEmpty())
					xpathbuffer.append(" or ");
				xpathbuffer.append(" (@NID=$bookmarkFileNIDList) ");
			}
			if(!xpathbuffer.toString().isEmpty()) {
				return "("+xpathbuffer.toString()+")";
			}

		} else {
			if(!whereTextSubStrXPath.isEmpty()) {
				listOfWhereXpath.add(whereTextSubStrXPath);
			}
			if(!whereMetaDataExprXPath.isEmpty()) {
				listOfWhereXpath.add(whereMetaDataExprXPath);
			}    		
		}

		return "";

	}

	private void generateFileXPath(List<String> listOfWhereXpath,
			Hashtable<String, String> customeTokenMappingTable, String whereState, String whereSessionConfigNID,
			String whereMediaGroupID, String whereType, String whereDownloadState, Hashtable<String, String> fileMediaTypeTable) {

		if (!"".equals(whereSessionConfigNID)) {		
			listOfWhereXpath.add(GetObjectUtils.generateXPath(whereSessionConfigNID, SessionServiceConst.WHERE_SESSION_CONFIG_NID, SessionServiceConst.FILE, SessionServiceConst.SESSION_CONFIG_NID_CONST));
		}

		String whereStateEnabled = SessionServiceConst.FALSE;
		String whereStateXpath = "";
		if (!"".equals(whereState)) {	
			whereStateXpath = GetObjectUtils.generateXPath(whereState, SessionServiceConst.WHERE_STATE, SessionServiceConst.FILE, SessionServiceConst.STATE_CONST);
			if(!whereStateXpath.isEmpty()) {
				listOfWhereXpath.add(whereStateXpath);
				if(whereState.equals(SessionServiceConst.READY) || whereState.equals(SessionServiceConst.OFFLINE)) {
					whereStateEnabled = SessionServiceConst.TRUE;
				}
				whereStateXpath = "and ("+whereStateXpath+")";
			}
		}

		String whereDownloadStateXPath = generateWhereDownloadStateXPath(whereDownloadState);

		customeTokenMappingTable.put(SessionServiceConst._WHEREDOWNLOADSTATE_, whereDownloadStateXPath);

		customeTokenMappingTable.put(SessionServiceConst._WHERESTATEENABLED_, whereStateEnabled);
		customeTokenMappingTable.put(SessionServiceConst._WHERESTATE_, whereStateXpath);


		if (!"".equals(whereMediaGroupID)) {		
			listOfWhereXpath.add(GetObjectUtils.generateXPath(whereMediaGroupID, SessionServiceConst.WHERE_MEDIA_GROUP_ID, SessionServiceConst.FILE, SessionServiceConst.MEDIA_GROUP_ID_CONST));
		}

		String whereTypeValue = "(//MediaStoreFileInfo/Type = '_TYPE_')";
		StringTokenizer st = new StringTokenizer(whereType, ",");
		while(st.hasMoreTokens()) {
			String token = st.nextToken().trim();
			fileMediaTypeTable.put(token, whereTypeValue.replace("_TYPE_", token));
		}

		if (fileMediaTypeTable.size() >= SessionServiceResourceConst.FILE_WHERETYPE_ENUMS.values().length || fileMediaTypeTable.isEmpty()) {
			whereType = SessionServiceConst.MEDIA_ALL;
		}
		else {

			if (fileMediaTypeTable.containsKey(SessionServiceConst.MEDIA_CLIP) && fileMediaTypeTable.containsKey(SessionServiceConst.MEDIA_GROUP)) {
				whereType = SessionServiceConst.BOTH;
			} else if ((fileMediaTypeTable.containsKey(SessionServiceConst.MEDIA_GROUP) && fileMediaTypeTable.containsKey(SessionServiceConst.MEDIA_IMAGE)) || fileMediaTypeTable.containsKey("ALL")){
				// Asked for MediaGroups and MediaImages..Invalid pair, Return ALL
				whereType = SessionServiceConst.MEDIA_ALL;
			} else {
				whereType = SessionServiceConst.MEDIA_CUSTOM;
			}
		}	

		customeTokenMappingTable.put(SessionServiceConst._SEARCHINFILES_, whereType);        
	}

	private String generateWhereDownloadStateXPath(String whereDownloadState) {
		String whereDownloadStateXPath = "";
		if (!whereDownloadState.isEmpty()) {
			// Do not filter results if whereDownloadState is not one of the allowed states
			if (whereDownloadState.equals(MediaStoreServiceConst.READY)
					|| whereDownloadState.equals(MediaStoreServiceConst.NOT_READY)
					|| whereDownloadState.equals(MediaStoreServiceConst.BUSY)) {
				String xPathState = (whereDownloadState.equals(MediaStoreServiceConst.READY)) ? MediaStoreServiceConst.PREPARED	: whereDownloadState;
				whereDownloadStateXPath = "and (.//Media/@state='"+xPathState+"')";
			}
		}
		return whereDownloadStateXPath;
	}

/*	private boolean isFilePrepared(String fileNID, String format, Element fileEl,  MRLog log) throws Exception{    	
		//Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(fileNID, request, log, db);
		if (format.equalsIgnoreCase(MEDIA_FORMAT_STANDARD)) {
			return fileFormatCheck(fileEl, MEDIA_FORMAT_STANDARD, log);  
		}else if (format.equalsIgnoreCase(MEDIA_FORMAT_PROPRIETARY)) {
			return fileFormatCheck(fileEl, MEDIA_FORMAT_PROPRIETARY, log);  
		}else{
			throw new MRException(SessionServiceExceptionCodes.OTHER, "format not supported");
		}

	}

	private boolean fileFormatCheck(Element fileEl, String mediaFormat, MRLog log) throws Exception {
		Element mediaElement =MRXMLUtils.getElementXpath(fileEl, "//Media[@format='"+mediaFormat+"']");		
		String stateFlag = MRXMLUtils.getAttributeValue(mediaElement, "state", "");
		log.addInfo("isFilePrepared: " + stateFlag + " " + stateFlag);
		if (!stateFlag.equalsIgnoreCase("Prepared") || stateFlag.isEmpty())
			return false;		
		else 
			return true;
	}*/
	
	public Document getFileAlternativeForWhereNID(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,String whereNID,String details) throws Exception {

		log.addInfo("PERFORMANCE: getFileAlternativeForWhereNID Start");
		Document doc = request.getMessage().getDocument();
		Element getFileElement = MRXMLUtils.getElementXpath(doc, "//GetFile");

		
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.FILE +"Alternative"+ SessionServiceConst.XQUERYXML;
		
		if(whereNID.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Alternative query only works with whereNID. Please provide whereNID");
		}
		
		String objectQuery = FileUtils.readFileResource(xqueryFileName, request.getClass());
		objectQuery = objectQuery.replace("_XPATH_", "@NID='"+whereNID+"'");
		objectQuery = objectQuery.replace("_SELECTFORMAT_", details);
		Document resultDoc = GetObjectUtils.executeQuery(objectQuery, SessionServiceConst.FILE, new ArrayList<String>(), details,"", request, xmpp, log, db, serviceAgentFactory, resourceFactory, true);
	
      	formatFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, getFileElement, resultDoc, details);
    	String requestName = getFileElement.getName();
		Element responseEL = MRXMLUtils.newElement(requestName);
		if ((resultDoc != null)) {
			responseEL = resultDoc.getRootElement();
			responseEL.setName(requestName);
		}
		log.addInfo("PERFORMANCE: getFileAlternativeForWhereNID END");
		
		return request.createResponseData(responseEL);
	}

	
		
}

