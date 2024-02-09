package com.ipvsserv.nxd.migration.tag.db;

import java.io.File;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class RestoreDatabaseTag extends MigrationTagImpl {
    public RestoreDatabaseTag() {
        super("RestoreDatabaseTag");
    }

    public RestoreDatabaseTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        String workarea = root.getScriptGlobals().getDBMigrationWorkareaFolder();

        File directory = new File(workarea);
        log("RestoreDatabase " + directory.getAbsolutePath());
        boolean saveToColEnable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "useObjectFolder", "true"), true);
        boolean saveForDiff = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "saveForDiff", "false"), true);

        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        DBAdminParams params = new DBAdminParams();
        params.setSaveToColEnable(saveToColEnable);
        params.setSaveForDiff(saveForDiff);
        dbadmin.restoredb(directory, params);
    }

}
