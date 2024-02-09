package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class CopyOfCreateDiffXML {
    public static void main(String[] args) {
        try {
            if (args.length < 3) {
                System.out.println("java com.ipvsserv.nxd.migration.xslt.CreateDiffXML \"fromfolder\"  \"tofolder\"  \"saveTofolder\" ");
                System.exit(0);
            }
            String fromfolder = args[0];
            String tofolder = args[1];
            String saveTofolder = args[2];
            createXSL(fromfolder, tofolder, saveTofolder);
        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }

    public static void createXSL(String fromfolder, String tofolder, String saveTofolder) throws Exception {
        // String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();

        File directoryFrom = new File(fromfolder);
        File directoryTo = new File(tofolder);
        File directoryToSave = new File(saveTofolder);
        CompUtils.deleteFolder(directoryToSave);
        directoryToSave.delete();
        directoryToSave.mkdirs();

        String directoryFromName = directoryFrom.getName();
        String directoryToName = directoryTo.getName();
        //String destination = file.getParentFile().getAbsolutePath() + File.separator + toSubFolder;
        //debug(this.getName() + " " +source +" to "+ destination);
        //CompUtils.copyDirectory(source, destination);
        StringBuffer bufDiffDirectory = new StringBuffer();
        findDiffDirectory(directoryFrom, directoryTo, directoryToSave, bufDiffDirectory, directoryFromName, directoryToName);

    }

    private static String getSaveXslPath(String folder, String rootName, String prefix, String suffix) {
        int counter = 1;
        File dir = new File(folder);
        File xlsDir = new File(dir, "xsls");
        xlsDir.mkdirs();
        String save_path2 = xlsDir.getAbsolutePath() + "/" + prefix + rootName + ".xsl";
        File file = new File(save_path2);
        while (file.exists()) {
            save_path2 = folder + "/" + prefix + rootName + "_" + counter + ".xsl";
            file = new File(save_path2);
            counter++;
        }
        return file.getAbsolutePath();
    }

    public static void findDiffDirectory(File directoryFrom, File directoryTo, File directoryToSave, StringBuffer bufDiffDirectory, String directoryFromName, String directoryToName) throws Exception {
        if (!directoryTo.exists()) {
            debug("File does not exists " + directoryTo.getAbsolutePath());
            bufDiffDirectory.append("File does not exists " + directoryTo.getAbsolutePath());
            return;
        }
        if (directoryFrom.isDirectory()) {
            //debug("copyDirectory directory " + source.getAbsolutePath());
            directoryToSave.mkdirs();
            File files[] = directoryFrom.listFiles();
            for (int i = 0; i < files.length; i++) {
                File fileFromChild = files[i];
                findDiffDirectory(fileFromChild, new File(directoryTo, fileFromChild.getName()), new File(directoryToSave, fileFromChild.getName()), bufDiffDirectory, directoryFromName, directoryToName);
            }
        } else {

            try {
                if (!directoryFrom.getName().endsWith(".xml")) {
                    return;
                }

                File textDiffDir = new File(directoryToSave.getParentFile(), "diff");
                textDiffDir.mkdirs();
                File textDiff = new File(textDiffDir, directoryToSave.getName() + ".txt");

                directoryToSave.getAbsoluteFile().getParentFile().mkdirs();
                //BooleanAssert updateState=new BooleanAssert();
                Document docFrom = Dom4jUtils.getDocument(directoryFrom);
                Document docTo = Dom4jUtils.getDocument(directoryTo);

                StringBuffer bufTextDiff = new StringBuffer(getXSLPrefix());
                bufTextDiff.append("\n\n\n/////////////////////more EL/ATTR  in '" + directoryFromName + "' compared to -->'" + directoryToName + "'////////////////////\n");
                findDiff(docFrom.getRootElement(), docTo.getRootElement(), "/" + docFrom.getRootElement().getName(), bufTextDiff);
                bufTextDiff.append("\n\n\n/////////////////////more EL/ATTR in '" + directoryToName + "' compared to -->'" + directoryFromName + "'////////////////////\n");
                findDiff(docTo.getRootElement(), docFrom.getRootElement(), "/" + docFrom.getRootElement().getName(), bufTextDiff);
                CompUtils.writeFile(textDiff.getAbsolutePath(), bufTextDiff.toString());

                StringBuffer bufDiffDelete = new StringBuffer();
                StringBuffer bufDiffAdd = new StringBuffer();
                createXsl(docFrom.getRootElement(), docTo.getRootElement(), "/" + docFrom.getRootElement().getName(), bufDiffDelete, false);
                createXsl(docTo.getRootElement(), docFrom.getRootElement(), "/" + docFrom.getRootElement().getName(), bufDiffAdd, true);

                if (bufDiffDelete.length() > 0 || bufDiffAdd.length() > 0) {
                    StringBuffer bufDiff = new StringBuffer(getXSLPrefix());
                    bufDiff.append("\n\n\n<!-- DELETE ( were present in '" + directoryFromName + "' but removed from '" + directoryToName + "') -->\n");
                    bufDiff.append(bufDiffDelete);
                    bufDiff.append("\n\n\n<!-- ADD ( were absent in '" + directoryFromName + "' but added to  '" + directoryToName + "') -->\n");
                    bufDiff.append(bufDiffAdd);
                    bufDiff.append(getXSLSuffix());
                    String xslName = getSaveXslPath(directoryToSave.getParentFile().getAbsolutePath(), docFrom.getRootElement().getName(), "ipvsobject_", ".xsl");
                    CompUtils.writeFile(xslName, bufDiff.toString());
                }

            } catch (Exception ex) {
                debug(directoryFrom.getAbsolutePath() + " " + directoryTo.getAbsolutePath());
                ex.printStackTrace();
            }
        }
    }

    private static void debug(String msg) {
        // TODO Auto-generated method stub
        System.out.println(msg);
    }

    public static void createXsl(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, boolean append) throws Exception {
        createXsl_attr(fromEL, toEL, xpath, bufDiff, append);
        createXsl_element(fromEL, toEL, xpath, bufDiff, append);
    }

    public static void createXsl_attr(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, boolean append) throws Exception {
        boolean addAttributes = false;
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        StringBuffer buf = new StringBuffer();
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            String valueAttrTo = attrEL.getValue();
            if (toEL.attributeValue(nameAttrTo) == null) {
                if (append) {
                    buf.append("\n      <xsl:choose>");
                    buf.append("\n          <xsl:when test=\"@" + nameAttrTo + "\">");
                    buf.append("\n          </xsl:when>");
                    buf.append("\n          <xsl:otherwise>");
                    buf.append("\n              <xsl:attribute name=\"" + nameAttrTo + "\">" + valueAttrTo + "</xsl:attribute>");
                    buf.append("\n          </xsl:otherwise>");
                    buf.append("\n      </xsl:choose> ");
                    addAttributes = true;
                } else {
                    bufDiff.append("\n    <xsl:template  match=\"" + xpath + "/@" + nameAttrTo + "\"/>");
                }
            }
        }
        if (addAttributes) {
            if (append) {
                bufDiff.append("\n    <xsl:template mode=\"add-atts\" match=\"" + xpath + "\">");
                bufDiff.append("\n      " + buf.toString());
                bufDiff.append("\n\n    </xsl:template>");
            }
        }
    }

    public static void createXsl_element(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, boolean append) throws Exception {
        StringBuffer buf = new StringBuffer();
        boolean addChildElments = false;
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        //////////// handle childrens 
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                createXsl(toChildEL, fromChildEL, childXpath, bufDiff, append);
            } else {

                if (append) {
                    buf.append("\n      <xsl:choose>");
                    buf.append("\n          <xsl:when test=\"" + name + "\">");
                    buf.append("\n          </xsl:when>");
                    buf.append("\n          <xsl:otherwise>");
                    //buf.append("\n          <"+name+"></"+name+">");
                    buf.append("\n              " + toChildEL.asXML());
                    buf.append("\n          </xsl:otherwise>");
                    buf.append("\n      </xsl:choose> ");
                    addChildElments = true;
                } else {
                    bufDiff.append("\n    <xsl:template  match=\"" + childXpath + "\"/>");
                }
            }
        }
        if (addChildElments) {
            if (append) {
                bufDiff.append("\n    <xsl:template mode=\"append\" match=\"" + xpath + "\">");
                bufDiff.append("\n      " + buf.toString());
                bufDiff.append("\n\n    </xsl:template>");
            }
        }
    }

    public static void findDiff(Element toEL, Element fromEL, String xpath, StringBuffer bufDiff) throws Exception {
        boolean bFound = false;
        List<?> childListFrom = Dom4jUtils.getList(fromEL);
        List<?> childListTo = Dom4jUtils.getList(toEL);
        if (childListFrom.size() != childListTo.size()) {
            //updateState.setDifferent(true);
        }
        String valueTo = Dom4jUtils.value(toEL, "").trim();
        String valueFrom = Dom4jUtils.value(fromEL, "").trim();
        if (!valueTo.equals(valueFrom)) {
            //updateState.setDifferent(true);
        }
        // compare attributes
        List<?> fromChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(toEL);
        /* if (fromChildELAttrList.size() != toChildELAttrList.size()) {
             updateState.setDifferent(true);
         }*/
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            String valAttrTo = attrEL.getValue();
            String valAttrFrom = Dom4jUtils.getAttributeValue(fromEL, nameAttrTo, "").trim();
            /*if (!valAttrFrom.equals(valAttrTo.trim())) {
                updateState.setDifferent(true);
            }*/
            if (fromEL.attributeValue(nameAttrTo) == null) {
                if (!bFound) {
                    bufDiff.append("\n");
                    bFound = true;
                }
                bufDiff.append("\n" + xpath + "/@" + nameAttrTo);
                debug("not found " + xpath + "/@" + nameAttrTo);
            }
        }

        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(fromEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                findDiff(toChildEL, fromChildEL, childXpath, bufDiff);
            } else {
                // updateState.setDifferent(true);
                if (!bFound) {
                    bufDiff.append("\n");
                    bFound = true;
                }
                bufDiff.append("\n\n" + childXpath);
                bufDiff.append("\n\n/////////////////////////===> aditional Element '" + childXpath + "' start ==> \n " + toChildEL.asXML() + "\n /////////////////////////<=== aditional Element '" + childXpath + "' end <==\n\n");
                debug("not found " + childXpath);
            }

        }
    }

    private static StringBuffer buftXSLPrefix = null;

    private static StringBuffer getXSLPrefix() {
        if (buftXSLPrefix == null) {
            buftXSLPrefix = new StringBuffer();
            buftXSLPrefix.append("\n<xsl:stylesheet version=\"1.0\"");
            buftXSLPrefix.append("\n    xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">");
            buftXSLPrefix.append("\n    <!-- Here's the boilerplate code -->");
            buftXSLPrefix.append("\n    <!-- By default, copy all nodes unchanged -->");
            buftXSLPrefix.append("\n    <xsl:template match=\"@* | node()\">");
            buftXSLPrefix.append("\n    <xsl:apply-templates mode=\"before\" select=\".\" />");
            buftXSLPrefix.append("\n    <xsl:copy>");
            buftXSLPrefix.append("\n        <xsl:apply-templates select=\"@*\" />");
            buftXSLPrefix.append("\n        <xsl:apply-templates mode=\"add-atts\" select=\".\" />");
            buftXSLPrefix.append("\n        <xsl:apply-templates mode=\"insert\" select=\".\" />");
            buftXSLPrefix.append("\n        <xsl:apply-templates />");
            buftXSLPrefix.append("\n        <xsl:apply-templates mode=\"append\" select=\".\" />");
            buftXSLPrefix.append("\n    </xsl:copy>");
            buftXSLPrefix.append("\n    <xsl:apply-templates mode=\"after\" select=\".\" />");
            buftXSLPrefix.append("\n    </xsl:template>");
            buftXSLPrefix.append("\n    <!-- By default, don't add anything -->");
            buftXSLPrefix.append("\n    <xsl:template mode=\"add-atts\" match=\"*\" />");
            buftXSLPrefix.append("\n    <xsl:template mode=\"insert\" match=\"*\" />");
            buftXSLPrefix.append("\n    <xsl:template mode=\"append\" match=\"*\" />");
            buftXSLPrefix.append("\n    <xsl:template mode=\"before\" match=\"@* | node()\" />");
            buftXSLPrefix.append("\n    <xsl:template mode=\"after\" match=\"@* | node()\" />");
        }
        return buftXSLPrefix;
    }

    private static StringBuffer getXSLSuffix() {
        StringBuffer buftXSLPrefix = new StringBuffer();
        buftXSLPrefix.append("\n</xsl:stylesheet>");
        return buftXSLPrefix;
    }
}
