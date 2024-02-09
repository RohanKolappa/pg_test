package com.ipvsserv.nxd.migration.tag.db;

import java.io.File;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.migration.DBMigrationModuleBase;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class ImportXmlsIntoDBTag extends MigrationTagImpl {
    public ImportXmlsIntoDBTag() {
        super("ImportXmlsIntoDatabaseTag");
    }

    public ImportXmlsIntoDBTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception { 
    	//xquery update is not supported for migration type media 
    	if(root.getScriptGlobals().getMigrationType() == DBMigrationModuleBase.MIGRATION_TYPE_MEDIA){
    		return;
    	}

    	String useApplyXSLFolder = Dom4jUtils.getAttributeValue(element, "useXSLApplyFolder", "false");
    	String workarea = root.getScriptGlobals().getDBMigrationWorkareaFolder();
    	if("true".equals(useApplyXSLFolder)) {
    		workarea = root.getMigrationModule().getXslApplyFolder();
    	}
        File directory = new File(workarea);
        log("ImportXmlsIntoDBTag " + directory.getAbsolutePath());
        boolean saveToColEnable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "useObjectFolder", "true"), true);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        DBAdminParams params = new DBAdminParams();
        params.setSaveToColEnable(saveToColEnable);
        dbadmin.importdb(directory, params);
    }

}
