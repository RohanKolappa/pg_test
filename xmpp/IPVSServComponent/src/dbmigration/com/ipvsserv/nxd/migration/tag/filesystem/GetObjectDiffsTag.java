package com.ipvsserv.nxd.migration.tag.filesystem;

import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.migration.xslt.CreateDiffXML;

public class GetObjectDiffsTag extends MigrationTagImpl {

    public GetObjectDiffsTag() {
        super("GetObjectDiffsTag");
    }

    public GetObjectDiffsTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        // String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();
        String fromfolder = Dom4jUtils.getAttributeValue(element, "fromFolder", "");
        String tofolder = Dom4jUtils.getAttributeValue(element, "toFolder", "");
        String saveTofolder = Dom4jUtils.getAttributeValue(element, "saveToFolder", "");
        log(this.getName() + " " + fromfolder + " to " + tofolder + " save " + saveTofolder);
        CreateDiffXML.createXSL(fromfolder, tofolder, saveTofolder, "", "");

    }

}