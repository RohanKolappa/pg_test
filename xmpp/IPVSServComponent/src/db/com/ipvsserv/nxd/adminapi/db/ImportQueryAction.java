package com.ipvsserv.nxd.adminapi.db;

import java.io.File;
import java.util.Map;

import org.dom4j.Document;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.migration.DBMigrationFileSystemMediaModule;

public class ImportQueryAction extends DBQueryAction {

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(exportDB.getDomainName());
        String collectionName = requestMDCParams.getCollectionNameSCol();
        File directory = exportDB.getDBImportFolder();
        String serviceVersionMadia = "2.7";//support started at 2.7
        try {
            if(directory==null){
                MRLog.debug("exportDB.getDBImportFolder() " + exportDB.getDBImportFolder());
            }
            File[] fileList = directory.listFiles();
            for (File file : fileList) {
                try {
                    Document doc = Dom4jUtils.getDocument(file);
                    serviceVersionMadia = Dom4jUtils.getAttributeValue(doc.getRootElement(), "serviceVer", "");
                    if (!serviceVersionMadia.equals("")) {
                        MRLog.debug("ImportQueryAction do migration of media at serviceVersion:"+ serviceVersionMadia);
                        break;
                    }
                } catch (Exception ex) {
                    MRLog.debug("file " + file.toString());
                    MRLog.error(ex);
                }
            }
        } catch (Exception ex) {
            MRLog.debug("file directory " + directory);
            MRLog.error(ex);
        }        
        if (!serviceVersionMadia.equals("")) {
            MRLog.debug("ImportQueryAction do migration of media at serviceVersion:"+ serviceVersionMadia);
        }
        DBMigrationFileSystemMediaModule module = new DBMigrationFileSystemMediaModule();
        module.setDBImportFolder(directory.getAbsolutePath());
        module.setServiceVersionMadia(serviceVersionMadia);
        module.initialize(params);
        module.start();
        module.stop();        
        boolean saveToCol = false;
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        DBAdminParams dbadminparams = new DBAdminParams();
        dbadminparams.setSaveToColEnable(saveToCol);
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: importing into DB from  " + directory.getAbsolutePath());
        }
        MRLog.debug("Importing..... START : " + exportDB.getDBImportFolder());
        dbadmin.importdb(directory, collectionName, exportDB.getDomainName(), dbadminparams);
        MRLog.debug("Importing..... DONE : " + exportDB.getDBImportFolder());

        return "DOME";
    }

}
