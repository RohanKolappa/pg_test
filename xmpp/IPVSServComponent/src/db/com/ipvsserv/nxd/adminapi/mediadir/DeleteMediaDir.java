package com.ipvsserv.nxd.adminapi.mediadir;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class DeleteMediaDir extends MediaDirBase {
   
    public DeleteMediaDir() {

    }

    @SuppressWarnings("unchecked")
    public void execute() throws Exception {
        try{
            mediaStreamDirResourceNID = Dom4jUtils.value(Dom4jUtils.element(requestEL, "params"), "mediaStreamDirResourceNID", "");
            MRLog.debug("START "+t0+" Delete mediadir dir='" + mediaStreamDirResourceNID + "'");

            domainName = Dom4jUtils.getAttributeValue(requestEL.getParent(), "domain", "");
            type = Dom4jUtils.getAttributeValue(requestEL.getParent(), "type", "");
            timeout = CompUtils.longValue(Dom4jUtils.getAttributeValue(requestEL.getParent(), "DBCallTimeOut", ""), 900 * 1000);
            
            String scriptFilePath2 = Dom4jUtils.value(requestEL, "scriptFilePath", "deleteDB_mediadir.xml");
            Document doc = getScriptDocument(scriptFilePath2);
            
            zipFolderPath = IpvsServGlobals.getProperty("admindbapi.mediadir_export", "/data/mediadir/export");
            ensureZipFolder(zipFolderPath);        
            //get work area 
            workarea_folder = workareaFolderPath("export",mediaStreamDirResourceNID);
    
    
            Element paramsEL = Dom4jUtils.element(requestEL, "params");
            List<Element> list = (List<Element>) Dom4jUtils.getList(paramsEL);
            for (Element el : list) {
                params.put(el.getName(), Dom4jUtils.value(el, ""));
            }
    
            maxPriority = CompUtils.intValue(Dom4jUtils.value(doc.getRootElement(), "maxPriority", ""), 10);
            invokeActions(null, doc.getRootElement());
        }finally{
            MRLog.debug("DB_ADMIN: DONE "+System.currentTimeMillis()+" (" + (System.currentTimeMillis()-t0) + ")  Delete mediadir dir='" + mediaStreamDirResourceNID + "'");
        }

    }

    
    
}
