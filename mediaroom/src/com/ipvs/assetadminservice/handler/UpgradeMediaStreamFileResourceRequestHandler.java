package com.ipvs.assetadminservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;


public class UpgradeMediaStreamFileResourceRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document requestDoc = request.getMessage().getDocument();
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        Document mediaStreamFileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileNID, log, db, request);

        return handleUpgrade(mediaStreamFileDoc, mediaStreamFileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }

    private Document handleUpgrade(Document mediaStreamFileDoc, String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
            String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");

            if (!mediaGroupID.isEmpty()) {
                List<?> listOfMediaStreamFiles = MediaStoreServiceUtils.getReadOnlyMediaStreamFileResourceList(mediaGroupID, request.getUserJID(), log, db);

                if ((listOfMediaStreamFiles != null)) {
                    for (Object object : listOfMediaStreamFiles) {
                        Element upgradeMediaStreamFileEL = (Element) object;
                        Document mediaFileDoc = MRXMLUtils.elementToDocument(upgradeMediaStreamFileEL);
                        MediaStoreServiceUtils.checkMediaStreamFileState(mediaFileDoc);

                        String fileNID = MRXMLUtils.getAttributeValue(upgradeMediaStreamFileEL, MediaStoreServiceConst.NID, "");

                        if (fileNID.equals(mediaStreamFileNID)) {
                            continue;
                        }

                        Document responseDoc = handleUpgrade(mediaFileDoc, fileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

                        if (responseDoc == null) {
                            return null;
                        }
                    }
                }
            }

            handleUpgradeOfGroupAndPlayList(mediaStreamFileDoc, mediaStreamFileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        }else if (SessionServiceResourceConst.FILE_WHERETYPE_ENUMS.contains(mediaType)) {
            if (handleUpgradeOfMediaStreamFile(mediaStreamFileDoc, request, xmpp, log, db, serviceAgentFactory, resourceFactory)) {
                return null;
            }
            
            //upgrade media group if all the clips under media group are upgraded
            String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");

            if (!mediaGroupID.isEmpty()) {
                List<?> listOfMediaStreamFiles = MediaStoreServiceUtils.getReadOnlyMediaStreamFileResourceList(mediaGroupID, request.getUserJID(), log, db);
                if(listOfMediaStreamFiles ==null || listOfMediaStreamFiles.isEmpty()) {
                	Document mediaGroupDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
                	String fileNID = MRXMLUtils.getAttributeValue(mediaGroupDoc.getRootElement(), MediaStoreServiceConst.NID, "");
                	handleUpgradeOfGroupAndPlayList(mediaGroupDoc, fileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                }
            }
            
        } else if (mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            Element playListEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaRoomServiceConst.PLAYLIST);
            Element playListFillterInfoEL = MRXMLUtils.getElementXpath(playListEL, MediaStoreServiceConst.FILLER_INFO);

            if (handleUpgradeOfFillerMediaStreamFile(playListFillterInfoEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory)) {
                return null;
            }

            List<?> playListEntries = MRXMLUtils.getListXpath(playListEL, MediaStoreServiceConst.PLAYLIST_ENTIRES + "/" + MediaStoreServiceConst.PLAYLIST_ENTRY);

            if ((playListEntries != null) && !playListEntries.isEmpty()) {
                Element playListEntryEL = null;
                String playListMediaStreamFileNID = null;
                Element playListEntryFillerInfo = null;

                for (Object object : playListEntries) {
                    playListEntryEL = (Element) object;
                    playListMediaStreamFileNID = MRXMLUtils.getValueXpath(playListEntryEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_NID, "");

                    if (handleUpgradeOfMediaStreamFile(playListMediaStreamFileNID, request, xmpp, log, db, serviceAgentFactory, resourceFactory)) {
                        return null;
                    }

                    playListEntryFillerInfo = MRXMLUtils.getElementXpath(playListEntryEL, MediaStoreServiceConst.FILLER_INFO);

                    if (handleUpgradeOfFillerMediaStreamFile(playListEntryFillerInfo, request, xmpp, log, db, resourceFactory, serviceAgentFactory)) {
                        return null;
                    }
                }
            }

            handleUpgradeOfGroupAndPlayList(mediaStreamFileDoc, mediaStreamFileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        } else {
            throw new MRException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, "Invalid Media Type");
        }

        return request.createResponseData();
    }

    private void handleUpgradeOfGroupAndPlayList(Document mediaStreamFileDoc, String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws MRException, Exception {

    	boolean writePermission = Utils.getBoolean(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, ""), true);

    	if (writePermission) {
    		return;
    	}

    	MediaStoreServiceUtils.updateMediaStreamFilePermissions(mediaStreamFileDoc, mediaStreamFileNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }

    private boolean handleUpgradeOfFillerMediaStreamFile(Element fillerInfoEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String fillerMediaNID = MRXMLUtils.getValueXpath(fillerInfoEL, MediaStoreServiceConst.FILLER_MEDIA_STREAM_FILE_NID, "");

        return handleUpgradeOfMediaStreamFile(fillerMediaNID, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
    }

    private boolean handleUpgradeOfMediaStreamFile(String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        if ((mediaStreamFileNID == null) || mediaStreamFileNID.isEmpty()) {
            return false;
        }

        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
        MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileDoc);

        return handleUpgradeOfMediaStreamFile(mediaStreamFileDoc, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
    }

    private boolean handleUpgradeOfMediaStreamFile(Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        boolean writePermission = Utils.getBoolean(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, ""), true);
        if (writePermission) {
            return false;
        }

        MediaStoreServiceUtils.sendRequestToMediaStoreAgent(AMConst.UPGRADE_MEDIA_STREAM_FILE_REQUEST, mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        return true;
    }
}
