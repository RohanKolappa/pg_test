package com.ipvsserv.nxd.migration.tag.db;

import java.io.File;
import java.util.List;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.migration.DBDocuments;
import com.ipvsserv.nxd.migration.DBMigrationModuleBase;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class XQueryTag extends MigrationTagImpl {
    public XQueryTag() {
        super("XQueryTag");
    }

    public XQueryTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
    	//xquery update is not supported for migration type media 
    	if(root.getScriptGlobals().getMigrationType() == DBMigrationModuleBase.MIGRATION_TYPE_MEDIA){
    		return;
    	}
        ///System.out.println("doMigration " + this.getName());
        String fileName = Dom4jUtils.getAttributeValue(element, "file", "");
        MRLog.debug(this.getName() + " " + fileName);
        boolean relativePath = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "relativePath", ""), true);
        File file = new File(fileName);
        if (relativePath) {
            String filePath = root.getScriptGlobals().getDBMigrationFolder() + File.separator + fileName;
            file = new File(filePath);
        }
        if (!file.exists()) {
            throw new Exception("File does not exists '" + fileName + "'");
        }
        log("XQuery " + fileName);
        String query = CompUtils.readFile(file.getAbsolutePath());
        DBDocuments dBDocuments = root.getMigrationModule().getDBDocuments();
        List<String> domains = dBDocuments.getDomainNames();
        //List<String> domains = db.getDomainNames();
        for (String serviceDomain : domains) {
            root.getScriptGlobals().setServiceDomain(serviceDomain);
            NIDSchemeMDC nodeDomain = new NIDSchemeMDC();
            nodeDomain.setDomain(serviceDomain);
            String xQuery = root.getScriptGlobals().modifyQuery(query);
            db.executeXQueryToUpdate("doMigration", nodeDomain.getCollectionName(), xQuery);
        }
    }
}
