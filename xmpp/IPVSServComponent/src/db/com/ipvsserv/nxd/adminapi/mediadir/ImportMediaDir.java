package com.ipvsserv.nxd.adminapi.mediadir;

import java.io.File;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class ImportMediaDir extends MediaDirBase {
   
    public ImportMediaDir() {

    }

    @SuppressWarnings("unchecked")
    public void execute() throws Exception {
        try {
            mediaStreamDirResourceNID = Dom4jUtils.value(Dom4jUtils.element(requestEL, "params"), "mediaStreamDirResourceNID", "");
            MRLog.debug("START "+t0+" Import mediadir dir='" + mediaStreamDirResourceNID + "'");
            //Element EL = params.getItemData();
            domainName = Dom4jUtils.getAttributeValue(requestEL.getParent(), "domain", "");
            type = Dom4jUtils.getAttributeValue(requestEL.getParent(), "type", "");
            timeout = CompUtils.longValue(Dom4jUtils.getAttributeValue(requestEL.getParent(), "DBCallTimeOut", ""), 900 * 1000);
            
            String scriptFilePath2 = Dom4jUtils.value(requestEL, "scriptFilePath", "importDB_mediadir.xml");
            Document doc = getScriptDocument(scriptFilePath2);
            
            zipFolderPath = IpvsServGlobals.getProperty("admindbapi.mediadir_import", "/data/mediadir/import");
            ensureZipFolder(zipFolderPath);        
            //get work area 
            workarea_folder = workareaFolderPath("import",mediaStreamDirResourceNID);
            MRLog.debug(" START "+t0+" Import mediadir dir='" + workarea_folder + "' file '" + zipFilePath + "'");
    
            
    
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
        }finally{
            try{
                //delete temp folder
                File directory = new File(getWorkarea_folder());
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("Import mediadir - deleting temp files '" + directory.getAbsolutePath() +"' '" + getZipFilePath() +"'" );
                }
                //TODO: uncomment the code below
                CompUtils.deleteDirectory(directory);
                File file = new File(getZipFilePath());
                //todo: for debug only: delete imedeatly
                file.delete(); 
                //file.delete(); 
            }catch(Exception ex){
            }
            MRLog.debug("DB_ADMIN:  DONE "+System.currentTimeMillis()+" (" + (System.currentTimeMillis()-t0) +") Import mediadir dir='" + mediaStreamDirResourceNID + "' dir='" + workarea_folder + "' file '" + zipFilePath + "'");
        }
    }

    
    
}
