package com.ipvsserv.nxd.adminapi.mediafile;

import java.io.File;
import java.util.Hashtable;
import java.util.Map;

import org.apache.commons.io.FileUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.db.DBQueryAction;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;

public class UpdateMediaResourceFilesAction extends DBQueryAction{
	
	String ipvscomphome = "";
    private static final String MEDIASTREAMFILERESOURCE_OBJECT = "MediaStreamFileResource";
    private static final String MEDIASTREAMFILERESOURCE_OBJECT_PATH = "/home/onair/objectXml";
    private static final String UPDATE_MEDIA_RESOURCE_XSL = "/conf/xmls/mediafile/update_media_resource.xsl";
    private static final String MEDIACLIP_TYPE = "MediaClip";
    private static final String PVRMEDIACLIP_TYPE = "PVRMediaClip";

	@Override
	public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException,DBExecutionException, InterruptedException, Exception {
		
		Element queryEL = Dom4jUtils.element(exportDB.getRequestEL(), "ExportImportFileListItem");
        Element internalEL = Dom4jUtils.element(queryEL, "Internal");
        
        String mediaFileType = Dom4jUtils.getAttributeValue(internalEL, "cliptType", "");
        String parentMediaDirNID = Dom4jUtils.getAttributeValue(internalEL, "dirNID", "");
        
        if(mediaFileType.equalsIgnoreCase(PVRMEDIACLIP_TYPE)) {
        	Hashtable<String, String> parameters = new Hashtable<String, String>();
        	String startOffset = Dom4jUtils.getAttributeValue(internalEL, "startOffset", "");
        	String endOffset = Dom4jUtils.getAttributeValue(internalEL, "endOffset", "");
        	if(!startOffset.isEmpty() && !endOffset.isEmpty())
        		parameters.put("playLength", String.valueOf(Integer.valueOf(endOffset) - Integer.valueOf(startOffset)));
        	
        	File directory = exportDB.getDBImportFolder();
            
            parameters.put("parentMediaDirNID", parentMediaDirNID);
            parameters.put("type", MEDIACLIP_TYPE);
           
            File[] list = directory.listFiles();
            for (File file : list) {
            	Document doc = null;
            	Element el = null;
            	try {
            		doc = Dom4jUtils.getDocument(file);
            		el = doc.getRootElement();
            	} catch (Exception ex) {
            	}
            	if (el == null) {
            		continue;
            	}

            	String object_type = el.getName();
            	String nid = Dom4jUtils.getAttributeValue(el, "NID", "").trim();
            	NIDSchemeMDC node = new NIDSchemeMDC();
            	node.parse(nid);
            	String file_type = Dom4jUtils.getValueXpath(el, "//MediaStoreFileInfo/Type", "");
            	
            	if(!file_type.isEmpty() && file_type.equalsIgnoreCase(PVRMEDIACLIP_TYPE) && object_type.equals(MEDIASTREAMFILERESOURCE_OBJECT)) {
            		File resultFile = new File(directory, "tmpMediaFile");
            		FileUtils.copyFileToDirectory(new File(MEDIASTREAMFILERESOURCE_OBJECT_PATH + "/" + MEDIASTREAMFILERESOURCE_OBJECT + ".xml"), directory);
            		File srcFile = new File(directory, MEDIASTREAMFILERESOURCE_OBJECT + ".xml");
            		parameters.put("objectFilePath", srcFile.getAbsolutePath());
            		
            		ipvscomphome = IpvsServGlobals.getHomeDirectory();
            		String xsl = ipvscomphome + UPDATE_MEDIA_RESOURCE_XSL;
            		XsltTransformUtils.transform_Xalan(xsl, file, resultFile, parameters);
            		file.delete();
            		srcFile.delete();
            		resultFile.renameTo(new File(directory, node.getDocUri()));
            	}
            }
        }
        
		return null;
	}

}
