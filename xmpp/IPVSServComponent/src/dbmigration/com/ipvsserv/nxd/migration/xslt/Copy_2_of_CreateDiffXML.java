package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class Copy_2_of_CreateDiffXML {
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
        Copy_2_of_CreateDiffXML createDiffXML = new Copy_2_of_CreateDiffXML();
        createDiffXML.init(fromfolder, tofolder, saveTofolder);
        createDiffXML.processDiff();
    }

    private String fromfolder = "";
    private String tofolder = "";
    private String saveTofolder = "";
    private String directoryFromName = "";
    private String directoryToName = "";
    private File directoryFrom = null;
    private File directoryTo = null;
    private File directoryToSave = null;
    private StringBuffer bufNotFoundFiles = new StringBuffer();

    private String fileRemoveAttr = "";
    private String fileRemoveElement = "";
    private String fileAddAttr = "";
    private String fileAddElementL = "";
    private String fileAllAttrAndEL = "";

    public Copy_2_of_CreateDiffXML() {

    }

    public void init(String fromfolder, String tofolder, String saveTofolder) throws Exception {
        // String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();
        this.fromfolder = fromfolder;
        this.tofolder = tofolder;
        this.saveTofolder = saveTofolder;
        directoryFrom = new File(fromfolder);
        directoryTo = new File(tofolder);
        directoryToSave = new File(saveTofolder);
        CompUtils.deleteFolder(directoryToSave);
        directoryToSave.delete();
        directoryToSave.mkdirs();

        directoryFromName = directoryFrom.getName();
        directoryToName = directoryTo.getName();
        fileRemoveAttr = directoryToSave.getAbsolutePath() + "/summary_fileRemoveAttrs.txt";
        fileRemoveElement = directoryToSave.getAbsolutePath() + "/summary_fileRemoveElement.txt";
        fileAddAttr = directoryToSave.getAbsolutePath() + "/summary_fileAddAttr.txt";
        fileAddElementL = directoryToSave.getAbsolutePath() + "/summary_fileAddElementL.txt";
        fileAllAttrAndEL = directoryToSave.getAbsolutePath() + "/summary_AllAttrAndEL.txt";

    }

    public void processDiff() throws Exception {

        findDiffDirectory(directoryFrom, directoryTo, directoryToSave);
    }

    private String getSaveXslPath(String folder, String rootName, String prefix, String suffix) {
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

    public void findDiffDirectory(File directoryFrom, File directoryTo, File directoryToSave) throws Exception {
        if (!directoryTo.exists()) {
            debug("File does not exists " + directoryTo.getAbsolutePath());
            bufNotFoundFiles.append("File does not exists " + directoryTo.getAbsolutePath());
            return;
        }
        if (directoryFrom.isDirectory()) {
            //debug("copyDirectory directory " + source.getAbsolutePath());
            directoryToSave.mkdirs();
            File files[] = directoryFrom.listFiles();
            for (int i = 0; i < files.length; i++) {
                File fileFromChild = files[i];
                findDiffDirectory(fileFromChild, new File(directoryTo, fileFromChild.getName()), new File(directoryToSave, fileFromChild.getName()));
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

                String rootElementName = docFrom.getRootElement().getName();

                String xslName = getSaveXslPath(directoryToSave.getParentFile().getAbsolutePath(), docFrom.getRootElement().getName(), "ipvsobject_", ".xsl");
                String diffName = textDiff.getAbsolutePath();

                StringBuffer bufDiff = new StringBuffer();
                StringBuffer bufDiffdetails = new StringBuffer();
                StringBuffer bufXsl = new StringBuffer();

                boolean saveXSL = false;
                CompUtils.writeFile(xslName, getXSLPrefix().toString(), true);
                CompUtils.writeFile(fileAllAttrAndEL, "\n\n\n////// " + rootElementName + "// // " + rootElementName + "// " + rootElementName + "//////////////////////////////////////\n", true);
                //////////////////// removeAttrFromXSL  ////////////////////
                bufDiff = new StringBuffer();
                bufXsl = new StringBuffer();
                removeAttrFromXSL(docFrom.getRootElement(), docTo.getRootElement(), "/" + rootElementName, bufDiff, bufDiffdetails, bufXsl);
                if (bufDiff.length() > 0) {
                    saveXSL = true;
                    String title = "DELETE ATTRIBUTES " + rootElementName + " ( were present in '" + directoryFromName + "' but removed from '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    CompUtils.writeFile(xslName, bufXsl.toString(), true);

                    CompUtils.writeFile(diffName, "\n\n\n////// " + title + " ///\n", true);
                    CompUtils.writeFile(diffName, bufXsl.toString(), true);

                    CompUtils.writeFile(fileRemoveAttr, "\n\n\n////// " + rootElementName + "// del attr // " + rootElementName + "// " + rootElementName + "//////////////////////////////////////\n", true);
                    CompUtils.writeFile(fileRemoveAttr, bufDiff.toString(), true);

                    CompUtils.writeFile(fileAllAttrAndEL, bufDiff.toString(), true);
                }

                ////////////////////addAttrToXSL   ////////////////////
                bufDiff = new StringBuffer();
                bufXsl = new StringBuffer();
                addAttrToXSL(docTo.getRootElement(), docFrom.getRootElement(), "/" + rootElementName, bufDiff, bufDiffdetails, bufXsl);
                if (bufDiff.length() > 0) {
                    saveXSL = true;
                    String title = "ADD ATTRIBUTES " + rootElementName + " ( were absent in '" + directoryFromName + "' but added to  '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    CompUtils.writeFile(xslName, bufXsl.toString(), true);

                    CompUtils.writeFile(diffName, "\n\n\n////// " + title + " ///\n", true);
                    CompUtils.writeFile(diffName, bufXsl.toString(), true);

                    CompUtils.writeFile(fileAddAttr, "\n\n\n////// " + rootElementName + "// add attr// " + rootElementName + "// " + rootElementName + "//////////////////////////////////////\n", true);
                    CompUtils.writeFile(fileAddAttr, bufDiff.toString(), true);

                    CompUtils.writeFile(fileAllAttrAndEL, bufDiff.toString(), true);
                }

                //////////////////// removeElementFromXSL  ////////////////////
                bufDiff = new StringBuffer();
                bufXsl = new StringBuffer();
                removeElementFromXSL(docFrom.getRootElement(), docTo.getRootElement(), "/" + rootElementName, bufDiff, bufDiffdetails, bufXsl);
                if (bufDiff.length() > 0) {
                    saveXSL = true;
                    String title = "DELETE ELEMENTS " + rootElementName + " ( were present in '" + directoryFromName + "' but removed from '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    CompUtils.writeFile(xslName, bufXsl.toString(), true);

                    CompUtils.writeFile(diffName, "\n\n\n////// " + title + " ///\n", true);
                    CompUtils.writeFile(diffName, bufXsl.toString(), true);

                    CompUtils.writeFile(fileRemoveElement, "\n\n\n////// " + rootElementName + "// del el// " + rootElementName + "// " + rootElementName + "//////////////////////////////////////\n", true);
                    CompUtils.writeFile(fileRemoveElement, bufDiff.toString(), true);

                    CompUtils.writeFile(fileAllAttrAndEL, bufDiff.toString(), true);
                }

                ////////////////////addElementToXSL  ////////////////////
                bufDiff = new StringBuffer();
                bufXsl = new StringBuffer();
                addElementToXSL(docTo.getRootElement(), docFrom.getRootElement(), "/" + rootElementName, bufDiff, bufDiffdetails, bufXsl);
                if (bufDiff.length() > 0) {
                    saveXSL = true;
                    String title = "ADD ELEMENTS " + rootElementName + " ( were absent in '" + directoryFromName + "' but added to  '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    CompUtils.writeFile(xslName, bufXsl.toString(), true);

                    CompUtils.writeFile(diffName, "\n\n\n////// " + title + " ///\n", true);
                    CompUtils.writeFile(diffName, bufXsl.toString(), true);

                    CompUtils.writeFile(fileAddElementL, "\n\n\n////// " + rootElementName + "// add el// " + rootElementName + "// " + rootElementName + "//////////////////////////////////////\n", true);
                    CompUtils.writeFile(fileAddElementL, bufDiff.toString(), true);

                    CompUtils.writeFile(fileAllAttrAndEL, bufDiff.toString(), true);
                }

                if (!saveXSL) {
                    File file = new File(xslName);
                    file.delete();
                } else {
                    CompUtils.writeFile(xslName, getXSLSuffix().toString(), true);
                }

            } catch (Exception ex) {
                debug(directoryFrom.getAbsolutePath() + " " + directoryTo.getAbsolutePath());
                ex.printStackTrace();
            }
        }
    }

    public void addElementToXSL(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, StringBuffer bufDiffdetails, StringBuffer xslBuf) throws Exception {
        StringBuffer buf = new StringBuffer();
        boolean addChildElments = false;
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                addElementToXSL(toChildEL, fromChildEL, childXpath, bufDiff, bufDiffdetails, xslBuf);
            } else {
                buf.append("\n      <xsl:choose>");
                buf.append("\n          <xsl:when test=\"" + name + "\">");
                buf.append("\n          </xsl:when>");
                buf.append("\n          <xsl:otherwise>");
                //buf.append("\n          <"+name+"></"+name+">");
                buf.append("\n              " + toChildEL.asXML());
                buf.append("\n          </xsl:otherwise>");
                buf.append("\n      </xsl:choose> ");
                addChildElments = true;
                bufDiff.append("\n (addElementToXSL)" + xpath + "/" + name);
                bufDiffdetails.append("\n " + xpath + "/" + name + " ( " + toChildEL.asXML().replace('\n', ' ') + ")");
            }
        }
        if (addChildElments) {
            xslBuf.append("\n    <xsl:template mode=\"append\" match=\"" + xpath + "\">");
            xslBuf.append("\n      " + buf.toString());
            xslBuf.append("\n\n    </xsl:template>");
        }
    }

    public void removeElementFromXSL(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, StringBuffer bufDiffdetails, StringBuffer xslBuf) throws Exception {
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                removeElementFromXSL(toChildEL, fromChildEL, childXpath, bufDiff, bufDiffdetails, xslBuf);
            } else {
                xslBuf.append("\n   <xsl:template  match=\"" + childXpath + "\"/>");
                bufDiff.append("\n (removeElementFromXSL) " + childXpath);
                bufDiffdetails.append("\n " + childXpath + " (" + toChildEL.asXML().replace('\n', ' ') + ")");
            }
        }
    }

    public void addAttrToXSL(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, StringBuffer bufDiffdetails, StringBuffer xslBuf) throws Exception {
        StringBuffer buf = new StringBuffer();
        boolean addAttributes = false;
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            String valueAttrTo = attrEL.getValue();
            if (toEL.attributeValue(nameAttrTo) == null) {
                buf.append("\n      <xsl:choose>");
                buf.append("\n          <xsl:when test=\"@" + nameAttrTo + "\">");
                buf.append("\n          </xsl:when>");
                buf.append("\n          <xsl:otherwise>");
                buf.append("\n              <xsl:attribute name=\"" + nameAttrTo + "\">" + valueAttrTo + "</xsl:attribute>");
                buf.append("\n          </xsl:otherwise>");
                buf.append("\n      </xsl:choose> ");
                addAttributes = true;
                bufDiff.append("\n (addAttrToXSL) " + xpath + "/@" + nameAttrTo);
                bufDiffdetails.append("\n " + xpath + "/@" + nameAttrTo + " (value=" + valueAttrTo + ")");
            }
        }
        if (addAttributes) {
            xslBuf.append("\n    <xsl:template mode=\"add-atts\" match=\"" + xpath + "\">");
            xslBuf.append("\n      " + buf.toString());
            xslBuf.append("\n\n    </xsl:template>");
        }

        //loop children    
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                addAttrToXSL(toChildEL, fromChildEL, childXpath, bufDiff, bufDiffdetails, xslBuf);
            }
        }
    }

    public void removeAttrFromXSL(Element fromEL, Element toEL, String xpath, StringBuffer bufDiff, StringBuffer bufDiffdetails, StringBuffer xslBuf) throws Exception {
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            if (toEL.attributeValue(nameAttrTo) != null) {
                xslBuf.append("\n  <xsl:template match=\"" + xpath + "/@" + nameAttrTo + "\"/>");
                bufDiff.append("\n (removeAttrFromXSL) " + xpath + "/@" + nameAttrTo);
                bufDiffdetails.append("\n " + xpath + "/@" + nameAttrTo);
            }
        }
        //loop children    
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                removeAttrFromXSL(toChildEL, fromChildEL, childXpath, bufDiff, bufDiffdetails, xslBuf);
            }
        }
    }

    private void debug(String msg) {
        // TODO Auto-generated method stub
        System.out.println(msg);
    }

    private StringBuffer buftXSLPrefix = null;

    private StringBuffer getXSLPrefix() {
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

    private StringBuffer getXSLSuffix() {
        StringBuffer buftXSLPrefix = new StringBuffer();
        buftXSLPrefix.append("\n</xsl:stylesheet>");
        return buftXSLPrefix;
    }

}
