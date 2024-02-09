package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
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
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class AddDestFileRequestHandler implements MRRequestHandlerI {
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

    // @SuppressWarnings("deprecation")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String mediaFileRequestXML = "MediaStreamFileResource_BASE_ALL_ADD.xml";
        Document mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);

        Document requestDoc = request.getMessage().getDocument();
        Element dirGroups = MRXMLUtils.newElement(MediaStoreServiceConst.GROUPS_EL_NAME);
        Element dirPermissions = MRXMLUtils.newElement(MediaStoreServiceConst.PERMISSION_EL_NAME);

        String clipType = "";
        String parentDirNID = null;
        Element metaDataEL = null;
        Element groupEL = null;
        Element permissionEL = null;
        String dirTitle = null;
        String dirDescription = null;
        String fileTitle = null;
        String mediaGroupFileResourceNID = null;
        String fileDescription = null;
        //String storePortNID = null;

        long xmpp_date = xmpp.getCurrentTimeMillis();
        String userJID = request.getUserJID();
        int serverindex = userJID.indexOf("/");
        String username = null;

        if (serverindex > 0) {
            username = userJID.substring(0, serverindex);
        }

        try {
            parentDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, "");
           // storePortNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STORE_PORT_RESOURCE_NID, "");

            if (parentDirNID.isEmpty()) {
                throw new MRException(MediaRoomServiceExceptionCodes.PARENT_MEDIA_DIR_MISSING, "Parent Media Dir NID Missing");
            }
           
            clipType = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.CLIP_TYPE_EL_NAME, "");
            
            if(clipType.equals(MediaRoomServiceConst.PVRMEDIACLIP)){
            	parentDirNID = MediaStoreServiceUtils.getPVRDirNID(parentDirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);            	
            }

            /**
             * Read the Directory document from data base and get the dir groups, title, description and permissions
             */
            Document dirDoc = null;

            dirDoc = MRXMLDBUtils.readElement(request, log, db, parentDirNID);

            if (dirDoc != null) {
                dirGroups = MRXMLUtils.getElementXpath(dirDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
                dirTitle = MRXMLUtils.getValueXpath(dirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");
                dirDescription = MRXMLUtils.getValueXpath(dirDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, "");
                dirPermissions = MRXMLUtils.getElementXpath(dirDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME);
            }

            metaDataEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
        
            /*if (clipType.equals(MediaRoomServiceConst.MEDIACLIP) || clipType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
                if (storePortNID.isEmpty()) {
                    throw new MRException(MediaRoomServiceExceptionCodes.MEDIA_STORE_PORT_NID_MISSING, "Media Store Port NID Missing. ClipType:=" + clipType);
                }
            }*/

            groupEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
            permissionEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME);

            mediaGroupFileResourceNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_FILE_RESOURCE_NID_EL_NAME, "");
            fileTitle = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.FILE_TITLE_EL_NAME, "");
            fileDescription = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.FILE_DESCRIPTION_EL_NAME, "");

            /**
             *  Title, Description, Permissions and groups are optional element.
             *  If request XML does not have those values set the dir values
             */
            if (fileTitle.isEmpty()) {
                fileTitle = dirTitle;
            }

            if (fileDescription.isEmpty()) {
                fileDescription = dirDescription;
            }

            if (groupEL == null) {
                groupEL = dirGroups;
            }

            if (permissionEL == null) {
                permissionEL = dirPermissions;
            }

            /**
             * Set the media Stream File Info. Like Groups,Parent Dir NID, Store Port NID and Permissions
             */

            //set UUID
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.UUID, Utils.getUUID());

            //if it is new file in the data base then set the parent dir NID as NONE.
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, parentDirNID);

            //set the state as not ready.           
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.NOT_READY);

            //set date created and modified values
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, String.valueOf(xmpp_date));
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, String.valueOf(xmpp_date));

            //set the owner user name as logged in user name
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.OWNER_EL_NAME, username);

            Element infoEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);

            //set the Media Stream File Groups 
            Element fileGroupEL = MRXMLUtils.getElement(infoEL, MediaStoreServiceConst.GROUPS_EL_NAME);

            if ((fileGroupEL != null) && (groupEL != null)) {
                MRXMLUtils.replaceContent(fileGroupEL, groupEL);
            } else if (groupEL != null) {
                MRXMLUtils.addElement(infoEL, (Element) groupEL.clone());
            } else if (fileGroupEL != null) {
                MRXMLUtils.removeContent(fileGroupEL);
            }

            //set the permission 
            Element filePermissionEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME);

            if ((filePermissionEL != null) && (permissionEL != null)) {
                MRXMLUtils.replaceContent(filePermissionEL, permissionEL);
            } else if (permissionEL != null) {
                MRXMLUtils.addElement(MRXMLUtils.getElementXpath(infoEL, MediaStoreServiceConst.PROPERTIES_EL_NAME), (Element) permissionEL.clone());
            }

            /**
             * Set the media store file info (Common info between media stream file and media store file)
             */
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, fileDescription);

            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, fileTitle);

            // Find the mediaGroupID
            String mediaGroupID = "";

            if (!"".equals(mediaGroupFileResourceNID)) {
                Document groupFileDoc = MRXMLDBUtils.readElement(request, log, db, mediaGroupFileResourceNID);

                if (groupFileDoc != null) {
                    mediaGroupID = MRXMLUtils.getValueXpath(groupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaRoomServiceConst.MEDIAGROUPID, "");
                }
            }

            // Set the Media Type
            if (clipType.equalsIgnoreCase(MediaRoomServiceConst.MEDIACLIP)) {
                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, MediaRoomServiceConst.MEDIACLIP);
            } else if (clipType.equalsIgnoreCase(MediaRoomServiceConst.PVRMEDIACLIP)) {
                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, MediaRoomServiceConst.PVRMEDIACLIP);

                if (infoEL != null) {
                    MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PVR_TRIM_LENGTH_EL_NAME, MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
                }
            } else if (clipType.equalsIgnoreCase(MediaRoomServiceConst.MEDIAGROUP)) {
                //set the state as ready.           
                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.READY);

                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, MediaRoomServiceConst.MEDIAGROUP);
                mediaGroupID = Utils.getUUID();
            } else if (clipType.equalsIgnoreCase(MediaRoomServiceConst.MEDIAIMAGE)){
            	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, MediaRoomServiceConst.MEDIAIMAGE);
            }else {
                throw new MRException(MediaRoomServiceExceptionCodes.CLIPTYPE_NOT_FOUND, clipType);
            }

            // Set the MediaGroupID
            if (infoEL != null) {
                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaRoomServiceConst.MEDIAGROUPID, mediaGroupID);
            }

            //set the meta Data
            Element mediaStoreFileInfoEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT);

            if (mediaStoreFileInfoEL != null) {
                Element fileMetaDataEL = mediaStoreFileInfoEL.element(MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

                if ((fileMetaDataEL != null) && (metaDataEL != null)) {
                    MRXMLUtils.replaceContent(fileMetaDataEL, metaDataEL);                    
                } else if (metaDataEL != null) {
                    MRXMLUtils.addElement(mediaStoreFileInfoEL, (Element) metaDataEL.clone());
                } else if (fileMetaDataEL != null) {
                    MRXMLUtils.removeContent(fileMetaDataEL);
                }
            }

            String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
            MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, mediaType);
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, 0 + "");

            // Build the object
            Element mediaStreamFileEL = (Element)mediaStreamFileDoc.getRootElement().detach();

            return MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.ASSETADMINSERVICE, mediaFileRequestXML, null, null, mediaStreamFileEL, "Add");
        } catch (MRException exp) {
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaRoomServiceExceptionCodes.ADD_DEST_FILE_FAILED, exp.toString());
        }
    }
}
