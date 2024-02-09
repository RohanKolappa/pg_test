package com.ipvsserv.nxd.adminapi.mediafile;

import java.io.File;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class ExportMediaFile extends MediaFileBase {
    
    public ExportMediaFile() {

    }

    @SuppressWarnings("unchecked")
    public void execute() throws Exception {
        try {
            NID = Dom4jUtils.value(Dom4jUtils.element(requestEL, "params"), "exportMediaStreamFileResourceNID", "");
            MRLog.debug("START " + t0 + " Export mediafile '" + NID + "'");
            domainName = Dom4jUtils.getAttributeValue(requestEL.getParent(), "domain", "");
            type = Dom4jUtils.getAttributeValue(requestEL.getParent(), "type", "export");
            timeout = CompUtils.longValue(Dom4jUtils.getAttributeValue(requestEL.getParent(), "DBCallTimeOut", ""), 900 * 1000);

            String scriptFilePath2 = Dom4jUtils.value(requestEL, "scriptFilePath", "exportDB_mediafile.xml");
            Document doc = getScriptDocument(scriptFilePath2);

            zipFolderPath = IpvsServGlobals.getProperty("admindbapi.mediafile_export", "/data/mediadir/export");
            ensureZipFolder(zipFolderPath);
            //get work area 
            workarea_folder = workareaFolderPath(type.toLowerCase(), NID);

            Element paramsEL = Dom4jUtils.element(requestEL, "params");
            List<Element> list = (List<Element>) Dom4jUtils.getList(paramsEL);
            for (Element el : list) {
                params.put(el.getName(), Dom4jUtils.value(el, ""));
            }
            //add queryEL NID to _index
            Element queryEL = Dom4jUtils.element(requestEL, "ExportImportFileListItem");
            String exportImportFileListItem=Dom4jUtils.getAttributeValue(queryEL, "NID", "");
            params.put("exportImportFileListItem", exportImportFileListItem);
            
            maxPriority = CompUtils.intValue(Dom4jUtils.value(doc.getRootElement(), "maxPriority", ""), 10);
            invokeActions(null, doc.getRootElement());

        } finally {
            try {
                File directory = new File(getWorkarea_folder());
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("Export mediafile - deleting temp files '" + directory.getAbsolutePath() + "' '" + getZipFilePath() + "'");
                }
                if (CompUtils.getOS().equals("windows")) {
                }else{
                    CompUtils.deleteDirectory(directory);
                }
                
            } catch (Exception ex) {
            }

            MRLog.debug("DB_ADMIN: DONE " + System.currentTimeMillis() + " (" + (System.currentTimeMillis() - t0) + ") Export mediafile '" + NID + "' dir='" + workarea_folder + "' file '" + zipFilePath + "'");
        }
    }

    /*
        <ExportMediaStreamFileRequestData>      
                <File NID='' title='' dateCreated='' />
                <File NID='' title='' dateCreated='' >
                    <FileList>
                        <File NID='' title='' dateCreated='' />
                    </FileList> 
                </File>
        </ExportMediaStreamFileRequestData>
      
     */

}
