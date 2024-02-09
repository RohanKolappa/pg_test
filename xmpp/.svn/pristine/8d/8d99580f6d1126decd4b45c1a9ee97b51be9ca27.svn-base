package com.ipvsserv.nxd.migration.tag.db;

import java.io.File;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class ExportDatabaseTag extends MigrationTagImpl {
    public ExportDatabaseTag() {
        super("ExportDatabaseTag");
    }

    public ExportDatabaseTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        String workarea = root.getScriptGlobals().getDBMigrationWorkareaFolder();
        File directory = new File(workarea);
        boolean saveToColEnable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "useObjectFolder", "true"), true);
        boolean saveForDiff = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "saveForDiff", "false"), true);

        log("ExportDatabase " + directory.getAbsolutePath());
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        DBAdminParams params = new DBAdminParams();
        params.setSaveForDiff(saveForDiff);
        params.setSaveToColEnable(saveToColEnable);

        dbadmin.backupdb(directory, params);
    }

}
