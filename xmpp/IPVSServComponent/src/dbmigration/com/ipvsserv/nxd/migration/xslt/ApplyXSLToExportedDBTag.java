package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.util.List;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;

public class ApplyXSLToExportedDBTag extends MigrationTagImpl {
    private String xsl;
    private String useXSLLib;
    private List<String> listDir;
    private File tempFolder;

    public ApplyXSLToExportedDBTag() {
        super("ApplyXSLToExportedDBTag");
    }

    public ApplyXSLToExportedDBTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        String dbMigrationWorkArea = root.getScriptGlobals().getDBMigrationWorkareaFolder();
        xsl = Dom4jUtils.value(element, "xsl", "");
        useXSLLib = Dom4jUtils.value(element, "useXSLLib", "xalan");
        listDir = Dom4jUtils.values(element, "ObjectType");
        try {
            File directory = new File(dbMigrationWorkArea);
            tempFolder = new File(directory.getParentFile(), "temp");
            tempFolder.mkdirs();
            transform(directory);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            log("End  execute task : " + getName());
        }
    }

    public void transform(File directory) throws Exception {
        File files[] = directory.listFiles();
        for (int i = 0; i < files.length; i++) {
            File srcXmlFile = files[i];
            if (srcXmlFile.isDirectory()) {
                boolean doTransform = false;
                if (IpvsServGlobals.getDomainDBSchema().getDomainName(srcXmlFile.getName()) != null) {
                    doTransform = true;
                } else if (listDir.isEmpty()) {
                    doTransform = true;
                } else if (listDir.contains(srcXmlFile.getName())) {
                    doTransform = true;
                }
                if (doTransform) {
                    transform(srcXmlFile);
                }
            } else {
                try {
                    transform(xsl, srcXmlFile, useXSLLib);
                } catch (Exception ex) {
                    MRLog.error(useXSLLib + " " + xsl + " " + srcXmlFile.getAbsolutePath(), ex);
                }
            }
        }
    }

    public void transform(String xsl, File srcXml, String useXSLLib) throws Exception {
        try {
            File destFile = File.createTempFile(srcXml.getName(), ".xml", tempFolder);
            MRLog.debug(getName() + " transform XALAN " + xsl + " " + srcXml.getAbsolutePath());
            if (useXSLLib.equalsIgnoreCase("XALAN")) {
                XsltTransformUtils.transform_Xalan(xsl, srcXml, destFile);
            } else {
                XsltTransformUtils.transform_xsltproc(xsl, srcXml, destFile);
            }
            CompUtils.copyfile(destFile, srcXml);
            boolean temp = destFile.delete();
            log("transform  " + xsl + " " + srcXml.getAbsolutePath());

        } catch (Exception ex) {
            srcXml.renameTo(srcXml.getAbsoluteFile());
            MRLog.error(xsl + " " + srcXml.getAbsolutePath(), ex);
            throw ex;
        }
    }
}
