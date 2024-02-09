package com.ipvsserv.nxd.adminapi.db;

import java.io.File;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class ExtractZipAction extends DBQueryAction {

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        // /bin/tar -czvf "${temp_folder}/db/msdb.tgz" -C ${temp_folder} ${DB_FOLDER}
        File dir = new File(exportDB.getWorkarea_folder());
        File file = new File(exportDB.getZipFilePath());
        file.getParentFile().mkdirs();
        if (CompUtils.getOS().endsWith("windows")) {
            StringBuffer buf = new StringBuffer();
            String dirNameRoot = CompUtils.windowsToLinuxPath(exportDB.getZipFilePath());
            String[] cmdPSCP = { "pscp", "-pw", "0at3n", "-scp", "-r", "root@10.1.5.165:"+dirNameRoot , exportDB.getZipFilePath()};
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: extracting Zip  '" + exportDB.getZipFilePath() + "' " + CompUtils.joinArray(cmdPSCP, " ") + "" + buf.toString());
            }
            CompUtils.executeProcess(cmdPSCP, buf, true);

            String[] cmd = { "C:/etc/scripts/extract_db_tar.bat", dir.getAbsolutePath(), exportDB.getZipFilePath() ,  file.getName() };
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: extracting Zip  '" + exportDB.getZipFilePath() + "' " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
            }
            File indexFile = new File(dir.getAbsolutePath()+"/_index");
            int retVal = CompUtils.executeProcess(cmd, buf, true);
            if (retVal != 0) {
                int i=0;
                while(!indexFile.exists()){
                    Thread.sleep(5000);
                    retVal = CompUtils.executeProcess(cmd, buf);
                    i++;
                    if(i>20){
                        break;
                    }
                }
                if (!indexFile.exists()) {
                    throw new Exception("Error during create zip. retVal="+retVal+" " +CompUtils.joinArray(cmd, " ") +" "+ buf.toString()  );
                }
            }  
        } else {
            Thread.sleep(200);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("extracting Zip from '" + dir.getAbsolutePath() + "' to '" + exportDB.getZipFilePath() + "'");
            }
            String tarOption = "-xvf";
            if (exportDB.getMediaDirZipType().equalsIgnoreCase("tar")) {
                tarOption = "-xvf";
            } else if (exportDB.getMediaDirZipType().equalsIgnoreCase("tgz")) {
                tarOption = "-xzvf";
            } else {
                tarOption = "-xvf";
            }
            //String[] cmd = { "tar", "-C", dir.getParent(), tarOption, exportDB.getZipFilePath() };
            String[] cmd = { "tar", "-C", dir.getAbsolutePath(), tarOption, exportDB.getZipFilePath() };
            StringBuffer buf = new StringBuffer();
            int retVal = CompUtils.executeProcess(cmd, buf);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: retval=" + retVal + "   extract Zip='" + exportDB.getZipFilePath() + "' cmd=" + CompUtils.joinArray(cmd, " ") + " retval=" + retVal + " ret=" + buf.toString());
            }
            if (retVal != 0) {
                throw new Exception("Error during extract zip. retVal="+retVal+" " +CompUtils.joinArray(cmd, " ") +" "+ buf.toString()  );
            }
            File dbXML = new File(dir.getAbsolutePath() + "/db.xml");
            Document dbDoc = Dom4jUtils.getDocument(dbXML);
            if(dbDoc != null) {
            	Element queryEL = Dom4jUtils.element(exportDB.getRequestEL(), "ExportImportFileListItem");
            	Element internalEL = Dom4jUtils.element(queryEL, "Internal");
            	Dom4jUtils.addAttribute(internalEL, "cliptType", Dom4jUtils.valueXpath(dbDoc, "//Type", ""));
            }
        }
        return "DONE";
    }
    

}
