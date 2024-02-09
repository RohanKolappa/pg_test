package com.ipvsserv.nxd.adminapi.mediafile;

import java.io.File;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsDBUtils;

public class ImportMediaFile extends MediaFileBase {

    public ImportMediaFile() {

    }

    @SuppressWarnings("unchecked")
    public void execute() throws Exception {
        try {
            NID = Dom4jUtils.value(Dom4jUtils.element(requestEL, "params"), "mediaStreamFileResourceNID", IpvsDBUtils.getUUID());
            
            MRLog.debug("START " + t0 + " Import mediafile '" + NID + "'");
            //Element EL = params.getItemData();
            domainName = Dom4jUtils.getAttributeValue(requestEL.getParent(), "domain", "");
            type = Dom4jUtils.getAttributeValue(requestEL.getParent(), "type", "import");
            timeout = CompUtils.longValue(Dom4jUtils.getAttributeValue(requestEL.getParent(), "DBCallTimeOut", ""), 900 * 1000);

            String scriptFilePath2 = Dom4jUtils.value(requestEL, "scriptFilePath", "importDB_mediafile.xml");
            Document doc = getScriptDocument(scriptFilePath2);

            zipFolderPath = IpvsServGlobals.getProperty("admindbapi.mediafile_import", "/data/mediafile/import");
            ensureZipFolder(zipFolderPath);
            //get work area 
            workarea_folder = workareaFolderPath(type.toLowerCase(), NID);
            MRLog.debug(" START " + t0 + " Import mediafile dir='" + workarea_folder + "' file '" + zipFilePath + "'");

            Element paramsEL = Dom4jUtils.element(requestEL, "params");
            List<Element> list = (List<Element>) Dom4jUtils.getList(paramsEL);
            for (Element el : list) {
                params.put(el.getName(), Dom4jUtils.value(el, ""));
            }

            maxPriority = CompUtils.intValue(Dom4jUtils.value(doc.getRootElement(), "maxPriority", ""), 10);
            invokeActions(null, doc.getRootElement());

            //TODO: uncomment the code below
            //CompUtils.deleteDirectory(dir);
            //File file = new File(getZipFilePath());
            //file.deleteOnExit(); 
        } finally {
            try {
                //delete temp folder
                File directory = new File(getWorkarea_folder());
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("Import mediafile - deleting temp files '" + directory.getAbsolutePath() + "' '" + getZipFilePath() + "'");
                }
                /*File file = new File(getZipFilePath());
                if (CompUtils.getOS().equals("windows")) {
                    file.deleteOnExit(); 
                }else{
                    CompUtils.deleteDirectory(directory);
                    file.deleteOnExit(); 
                }*/
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            MRLog.debug("DB_ADMIN:  DONE " + System.currentTimeMillis() + " (" + (System.currentTimeMillis() - t0) + ") Import mediafile  '" + NID + "' dir='" + workarea_folder + "' file '" + zipFilePath + "'");
        }
    }

    public String getDBIndexFilePath() {
        File dir = getDBImportFolder();
        File indexFile = new File(dir.getParentFile(), "_index");
        return indexFile.getAbsolutePath();
        
    }
}
