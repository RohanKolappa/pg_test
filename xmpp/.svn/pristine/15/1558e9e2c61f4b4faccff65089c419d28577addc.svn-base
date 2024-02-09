package com.ipvsserv.nxd.migration.tag.scripts;

import java.util.Vector;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class BackupDBTag extends MigrationTagImpl {
    public BackupDBTag() {
        super("BackupDBTag");
    }

    public BackupDBTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        String path = root.getScriptGlobals().getDBBackupPath();
        log(this.getName() + " export dbmigration " + path);
        Vector<String> vec = new Vector<String>();
        vec.add(DBAdmin.getDBAdminApi().getDbSscript());
        vec.add("export");
        vec.add(path);
        vec.add("dbmigration");
        vec.add("fullpath");
        String[] cmdArgs = new String[vec.size()];
        vec.copyInto(cmdArgs);
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        exitVal = CompUtils.executeProcess(cmdArgs, buf);

        if (exitVal != 0) {
            //script should not return non zero value.
            throw new Exception("ERROR "+CompUtils.joinArray(cmdArgs, " ")+" "+buf.toString());
        }
    }
}
