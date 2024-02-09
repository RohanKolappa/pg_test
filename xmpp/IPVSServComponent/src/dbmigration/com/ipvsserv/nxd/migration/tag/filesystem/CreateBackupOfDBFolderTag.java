package com.ipvsserv.nxd.migration.tag.filesystem;

import java.io.File;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;

public class CreateBackupOfDBFolderTag extends MigrationTagImpl {
    public CreateBackupOfDBFolderTag() {
        super("CreateBackupOfDBFolderTag");
    }

    public CreateBackupOfDBFolderTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        //System.out.println("doMigration " + this.getName());
        String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();

        String toSubFolder = Dom4jUtils.getAttributeValue(element, "toFolder", root.getScriptGlobals().getPreMigrationDBFolder());
        File file = new File(source);
        String destination = file.getParentFile().getAbsolutePath() + File.separator + toSubFolder;
        log(this.getName() + " " + source + " to " + destination);

        CompUtils.copyDirectory(source, destination);
    }

}
