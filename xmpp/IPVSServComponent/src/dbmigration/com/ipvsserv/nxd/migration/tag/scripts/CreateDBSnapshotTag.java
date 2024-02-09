package com.ipvsserv.nxd.migration.tag.scripts;

import java.util.Vector;

import com.ipvsserv.common.config.IpvsServGlobals;
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
public class CreateDBSnapshotTag extends MigrationTagImpl {
    public CreateDBSnapshotTag() {
        super("CreateDBSnapshotTag");
    }

    public CreateDBSnapshotTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        log(" CreateDBSnapshot ");
        restart_db(variables);
        create_snapshot(variables);
    }
    
    public void restart_db(ScriptVariables variables) throws Exception {
        String moduleName = root.getMigrationModule().getIpvsModuleName();
        boolean enable = IpvsServGlobals.getBooleanProperty(moduleName + ".enableRestartOnEachServiceVerMig", false);
        if(!enable ){
            log(" skip restart_db ");
            return ;
        }
        log(" restart_db ");
        Vector<String> vec = new Vector<String>();
        vec.add(DBAdmin.getDBAdminApi().getDbSscript());
        vec.add("restart");
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
    public void create_snapshot(ScriptVariables variables) throws Exception {
        String moduleName = root.getMigrationModule().getIpvsModuleName();
        boolean enable = IpvsServGlobals.getBooleanProperty(moduleName + ".enableCreateSnapshot", false);
        if(!enable ){
            log(" skip create_snapshot ");
            return ;
        }
        log(" create_snapshot ");

        Vector<String> vec = new Vector<String>();
        vec.add(DBAdmin.getDBAdminApi().getDbSscript());
        vec.add("create_snapshot");
        vec.add("migrating_" + root.getScriptGlobals().getServiceVersion());
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
