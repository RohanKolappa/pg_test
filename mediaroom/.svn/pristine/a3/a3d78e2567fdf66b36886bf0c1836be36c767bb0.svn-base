package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
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
import com.ipvs.utils.Utils;

public class GetExportImportFileRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
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
     
	private String[] getSortByRequestParam(String requestParam) {
		String[] ret = requestParam.split(",");
		if (!requestParam.isEmpty() && ret.length > 0) {
			for (int i = 0; i < ret.length; i++) {
				ret[i] = "$VAR/@" + ret[i];
			}
		}
		return ret;
	}

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

        //get request document
        Document requestDoc = request.getMessage().getDocument();

        //create response document
        //Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());
        StringBuffer buf = new StringBuffer();
        buf.append("<GetExportImportFileResponseData>");
        buf.append("<ObjectType>" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE + "</ObjectType>");
        buf.append("<GetResponse><ResponseXML/></GetResponse>");
        buf.append("</GetExportImportFileResponseData>");
        Document docResp = MRXMLUtils.stringToDocument(buf.toString());
        Element respRoot = MRXMLUtils.getElementXpath(docResp, "//ResponseXML");
        //get element from request
        Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//GetExportImportFile");// + respRoot.getName());
        if (reqEL == null) {
            //todo:MRException
            throw new Exception("Invalid request " + respRoot.getName());
        }

        //get request params 
        int startFrom = Utils.intValue(MRXMLUtils.getAttributeValue(reqEL, "startFrom", "0").trim(), 0);
        int countToFetch = Utils.intValue(MRXMLUtils.getAttributeValue(reqEL, "countToFetch", "10").trim(), 10);

        String sortBy = MRXMLUtils.getAttributeValue(reqEL, "sortBy", "priority").trim();
        String sortOrder = MRXMLUtils.getAttributeValue(reqEL, "sortOrder", "descending").trim();

        //get request params : where clause
        String whereDateCreated = FileSMUtils.getWhereRequestParam("@dateCreated", MRXMLUtils.getAttributeValue(reqEL, "whereDateCreated", "").trim());
        String whereStateDescription = FileSMUtils.getWhereRequestParam("@state_description", MRXMLUtils.getAttributeValue(reqEL, "whereStateDescription", "").trim());
        String whereState = FileSMUtils.getWhereRequestParam("@state", MRXMLUtils.getAttributeValue(reqEL, "whereState", "").trim());
        String whereOwnerJID = FileSMUtils.getWhereRequestParam("@ownerJID", MRXMLUtils.getAttributeValue(reqEL, "whereOwnerJID", "").trim());
        String whereType = FileSMUtils.getWhereRequestParam("@type", MRXMLUtils.getAttributeValue(reqEL, "whereType", "").trim());
        String wherePriority = FileSMUtils.getWhereRequestParam("@priority", MRXMLUtils.getAttributeValue(reqEL, "wherePriority", "").trim());
        String whereNID = FileSMUtils.getWhereRequestParam("@NID", MRXMLUtils.getAttributeValue(reqEL, "whereNID", "").trim());

        //get Object MRConfig
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { whereDateCreated, whereState, whereOwnerJID, whereType, wherePriority, whereNID, whereStateDescription };
        String[] sortByXpath = getSortByRequestParam(sortBy);
        //String sortOrder="";
        String selectXML = "";

        //do db read
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, respRoot, startFrom, countToFetch);

        //return response    
        return respRoot.getDocument();

    }

}
/*
<GetExportImportFile 
            startFrom="0" 
            countToFetch="10"
            sortBy="" 
            sortOrder="Ascending" 
            whereDateCreated="" 
            whereState=""
            whereOwnerJID="" 
            whereType="" 
            wherePriority="" 
            whereNID="" />
              
1. AddExportFileRequestHandler ( add one or more export files to queue)
2. AddImportFileRequestHandler ( add one or more import files to queue) 
3. DeleteExportImportFileRequestHandler ( delete one or more export/import files.
               it can use some where clause too...like date, title, createdByUserJID.. 
               it can not delete current one in progress... 
               to delete current one in progress ...first suspend and than delete )
4. GetExportImportFileStatusRequestHandler ( get one or more with pagination list )
5. ResumeExportImportFileRequestHandler ( resume system wide  : 
                   which may be suspended earlier 
                   or system may be rebooted)
6. SuspendExportImportFileRequestHandler ( suspend system wide :  
                   temporarily suspend all import/export activity)
7. UpdateExportImportFileRequestHandler 
           (   Update one record -- 
               like 
               priority = low, medium, high ( 1 to 3)
               toJID="" toFilePath="" ...etc
               or any other element in db queue xml )
<!--
export:    state= scheduled, exportFile_startNext, exportFile_dbExport, exportFile_dbFileTransfer,
       exportFile_tar, exportFile_fileTransfer, export_done  

import:    state= scheduled, importFile_startNext, importFile_fileTransfer, importFile_untar, 
   importFile_dbFileTransfer, importFile_dbImport, importFile_done  
-->

*************************************************************************** 
   Object :: ExportImportFileListItem  
*************************************************************************** 
<ExportImportFileListItem serviceVer="" Ver="1.6" Rev="1" NID="" parentNID=""
       type="import/export" 
       priority="1 to 3 :  low/medium/high" 
       state="scheduled" 
       ownerJID="" 
       dateCreated="" 
       queue_state="not_started/error/running/error_resume" 
   >
   <!-- 
       export parameters 
           -object level = type=export priority=1 to 3 (low/high/medium)
           -internal- file nid or title/dateCreated
           -external- folderPath
           -external- filePath

       import parameters 
           -object level = type=import priority=1 to 3 (low/high/medium)
           -internal- dir nid
           -external- folderPath
           -external- filePath
   -->
    <internal
       JID=""
       dirNID="" 
       tempFolderPath="blob_folder/../export or blob_folder/../import"
       mediaFileFolderPath="blob_folder"
       fileName="UUID.tgz"
       fileNID="file NID" 
       title="title"
       dateCreated="dateCreated" 
    />
    <external
       JID="" 
       folderPath="client_destFolder"
       fileName="clientFileName.tgz"
     />
   
   <error>
       <prev_state/>
       <error_state/>
       <error_message errorCode="">exception</error_message>
   </error>
</ExportImportFileListItem>            

*/

