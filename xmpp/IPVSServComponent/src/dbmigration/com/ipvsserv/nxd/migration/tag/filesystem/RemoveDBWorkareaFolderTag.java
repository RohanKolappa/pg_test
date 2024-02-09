package com.ipvsserv.nxd.migration.tag.filesystem;

import java.io.File;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class RemoveDBWorkareaFolderTag extends MigrationTagImpl {
    public RemoveDBWorkareaFolderTag() {
        super("RemoveDBWorkareaFolderTag");
    }

    public RemoveDBWorkareaFolderTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();
        log(this.getName() + " " + source);
        CompUtils.deleteDirectory(new File(source).getParentFile());
    }

}
