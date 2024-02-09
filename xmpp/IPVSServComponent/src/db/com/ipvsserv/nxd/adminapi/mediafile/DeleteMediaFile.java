package com.ipvsserv.nxd.adminapi.mediafile;

import java.util.List;
import java.util.Vector;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class DeleteMediaFile extends MediaFileBase {
   
    public DeleteMediaFile() {

    }
    
    @SuppressWarnings("unchecked")
    public void execute() throws Exception {
        try {
            NID = Dom4jUtils.value(Dom4jUtils.element(requestEL, "params"), "mediaStreamFileResourceNID", "");
            List<String> listNIDS = Dom4jUtils.values(Dom4jUtils.element(requestEL, "params"), "mediaStreamFileResourceNID");
            Vector<String> vec = new Vector<String>();
            for (String nid : listNIDS) {
                if(!nid.trim().equals("")){
                    vec.add("'"+nid+"'");
                }
            }
            String[] array= new String[vec.size()];
            vec.copyInto(array);
            NID = CompUtils.joinArray(array, ",");
            MRLog.debug("START " + t0 + " Delete mediafile \"" + NID + "\" ");
            domainName = Dom4jUtils.getAttributeValue(requestEL.getParent(), "domain", "");
            type = Dom4jUtils.getAttributeValue(requestEL.getParent(), "type", "export");
            timeout = CompUtils.longValue(Dom4jUtils.getAttributeValue(requestEL.getParent(), "DBCallTimeOut", ""), 900 * 1000);

            String scriptFilePath2 = Dom4jUtils.value(requestEL, "scriptFilePath", "deleteDB_mediafile.xml");
            Document doc = getScriptDocument(scriptFilePath2);

            zipFolderPath = IpvsServGlobals.getProperty("admindbapi.mediafile_export", "/data/mediadir/export");
            //ensureZipFolder(zipFolderPath);
            //get work area 
            workarea_folder = workareaFolderPath(type.toLowerCase(), "delete");

            Element paramsEL = Dom4jUtils.element(requestEL, "params");
            List<Element> list = (List<Element>) Dom4jUtils.getList(paramsEL);
            for (Element el : list) {
                //todo: do it for all elements 
                if(el.getName().equals("mediaStreamFileResourceNID")){
                    params.put(el.getName(), NID);
                }else{
                    params.put(el.getName(), Dom4jUtils.value(el, ""));
                }
            }

            maxPriority = CompUtils.intValue(Dom4jUtils.value(doc.getRootElement(), "maxPriority", ""), 10);
            invokeActions(null, doc.getRootElement());

        } finally {
            MRLog.debug("DB_ADMIN: DONE " + System.currentTimeMillis() + " (" + (System.currentTimeMillis() - t0) + ") Delete mediafile \"" + NID + "\" ");
        }

    }
}
