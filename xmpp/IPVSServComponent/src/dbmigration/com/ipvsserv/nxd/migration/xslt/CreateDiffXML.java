package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class CreateDiffXML {
    public static void main(String[] args) {
        try {
            if (args.length < 3) {
                System.out.println("java com.ipvsserv.nxd.migration.xslt.CreateDiffXML \"fromfolder\"  \"tofolder\"  \"saveTofolder\" ");
                System.exit(0);
            }
            String fromfolder = args[0];
            String tofolder = args[1];
            String saveTofolder = args[2];
            String fromfolder2 = "";
            String tofolder2 = "";

            if (args.length > 4) {
                fromfolder2 = args[3];
                tofolder2 = args[4];
            }
            createXSL(fromfolder, tofolder, saveTofolder, fromfolder2, tofolder2);
        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }

    public static void createXSL(String fromfolder, String tofolder, String saveTofolder, String fromfolder2, String tofolder2) throws Exception {
        CreateDiffXML createDiffXML = new CreateDiffXML();
        createDiffXML.processDiff(fromfolder, tofolder, saveTofolder, fromfolder2, tofolder2);
        createDiffXML.saveDiff();
    }

    //private String fromfolder = "";
    // private String tofolder = "";
    //private String saveTofolder = "";
    private String directoryFromName = "";
    private String directoryToName = "";
    private File directoryFrom = null;
    private File directoryTo = null;
    private File directoryToSave = null;
    private StringBuffer bufNotFoundFiles = new StringBuffer();

    //private String fileRemoveAttr = "";
    //private String fileRemoveElement = "";
    private String fileAttrSum = "";
    private String fileElementSum = "";
    private String fileAllAttrAndEL = "";
    private static boolean devicestatus = false;
    private Hashtable<String, CreateDiffXML.ObjectXSL> objectXSLs = new Hashtable<String, CreateDiffXML.ObjectXSL>();

    public CreateDiffXML() {

    }

    public void processDiff(String fromfolder, String tofolder, String saveTofolder, String fromfolder2, String tofolder2) throws Exception {
        // String source = root.getScriptGlobals().getDBMigrationWorkareaFolder();

        directoryFrom = new File(fromfolder);
        directoryTo = new File(tofolder);
        directoryToSave = new File(saveTofolder);
        CompUtils.deleteFolder(directoryToSave);
        directoryToSave.delete();
        directoryToSave.mkdirs();

        directoryFromName = directoryFrom.getName();
        directoryToName = directoryTo.getName();
        // fileRemoveAttr = directoryToSave.getAbsolutePath() + "/summary_fileRemoveAttrs.txt";
        //fileRemoveElement = directoryToSave.getAbsolutePath() + "/summary_fileRemoveElement.txt";
        fileAttrSum = directoryToSave.getAbsolutePath() + "/Attr_Summary.txt";
        fileElementSum = directoryToSave.getAbsolutePath() + "/Element_Summary.txt";
        fileAllAttrAndEL = directoryToSave.getAbsolutePath() + "/Attr_and_EL_summary.txt";

        findDiffDirectory(directoryFrom, directoryTo, directoryToSave);

        if (!fromfolder2.trim().equals("") && !tofolder2.trim().equals("")) {
            directoryFrom = new File(fromfolder2);
            directoryTo = new File(tofolder2);
            findDiffDirectory(directoryFrom, directoryTo, directoryToSave);
        }

    }

    public void saveDiff() {
        File textDiffDir = new File(directoryToSave, "diff");
        textDiffDir.mkdirs();
        File xslDiffDir = new File(directoryToSave, "xsls");
        xslDiffDir.mkdirs();

        for (String objectName : objectXSLs.keySet()) {
            try {
                CreateDiffXML.ObjectXSL object = objectXSLs.get(objectName);
                File textDiff = new File(textDiffDir, object.rootElementName + ".txt");
                File xslDiff = new File(xslDiffDir, object.rootElementName + ".xsl");

                object.writeXSL(xslDiff.getAbsolutePath(), object.rootElementName, directoryFromName, directoryToName);
                if (object.saveXSL) {
                    object.writeDiff_deletedAttr(textDiff.getAbsolutePath(), object.rootElementName, directoryFromName, directoryToName, true);
                    object.writeDiff_addedAttr(textDiff.getAbsolutePath(), object.rootElementName, directoryFromName, directoryToName, true);
                    object.writeDiff_deletedEL(textDiff.getAbsolutePath(), object.rootElementName, directoryFromName, directoryToName, true);
                    object.writeDiff_addedEL(textDiff.getAbsolutePath(), object.rootElementName, directoryFromName, directoryToName, true);

                    //save to separator
                    CompUtils.writeFile(fileAllAttrAndEL, "\n\n###### " + object.rootElementName + "##  " + object.rootElementName + "## from " + directoryFromName + " to " + directoryToName + "", true);
                    //CompUtils.writeFile(fileRemoveAttr, "\n\n###### " + object.rootElementName + "## " + object.rootElementName + "## from "+directoryFromName + " to "+ directoryToName+"", true);
                    CompUtils.writeFile(fileAttrSum, "\n\n###### " + object.rootElementName + "## " + object.rootElementName + "## from " + directoryFromName + " to " + directoryToName + "", true);
                    //CompUtils.writeFile(fileRemoveElement, "\n\n###### " + object.rootElementName + "## " + object.rootElementName + "## from "+directoryFromName + " to "+ directoryToName+"", true);
                    CompUtils.writeFile(fileElementSum, "\n\n###### " + object.rootElementName + "## " + object.rootElementName + "## from " + directoryFromName + " to " + directoryToName + "", true);

                    //save to global 
                    object.writeDiff_deletedAttr(fileAllAttrAndEL, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_addedAttr(fileAllAttrAndEL, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_deletedEL(fileAllAttrAndEL, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_addedEL(fileAllAttrAndEL, object.rootElementName, directoryFromName, directoryToName, false);

                    object.writeDiff_deletedAttr(fileAttrSum, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_addedAttr(fileAttrSum, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_deletedEL(fileElementSum, object.rootElementName, directoryFromName, directoryToName, false);
                    object.writeDiff_addedEL(fileElementSum, object.rootElementName, directoryFromName, directoryToName, false);

                }

            } catch (Exception e) {
                e.printStackTrace();
            }

        }

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
                debug("creating xsl for  " + directoryTo.getAbsolutePath());
                directoryToSave.getAbsoluteFile().getParentFile().mkdirs();
                //BooleanAssert updateState=new BooleanAssert();
                Document docFrom = null;
                Document docTo = null;
                try {
                    docFrom = Dom4jUtils.getDocument(directoryFrom);
                    docTo = Dom4jUtils.getDocument(directoryTo);
                } catch (Exception ex) {
                }
                if (docFrom == null || docTo == null) {
                    debug("Error " + directoryFrom.getAbsolutePath() + " " + directoryTo.getAbsolutePath());
                    return;
                }
                String rootElementName = docFrom.getRootElement().getName();
                CreateDiffXML.ObjectXSL object = objectXSLs.get(rootElementName);
                if (object == null) {
                    object = new CreateDiffXML.ObjectXSL();
                    object.rootElementName = rootElementName;
                    objectXSLs.put(rootElementName, object);
                }
                //#################### find diff  ####################
                if (devicestatus) {
                    addElementToXSL2(docTo.getRootElement(), docFrom.getRootElement(), "/" + rootElementName, object);
                } else {
                    removeAttrFromXSL(docFrom.getRootElement(), docTo.getRootElement(), "/" + rootElementName, object);
                    addAttrToXSL(docTo.getRootElement(), docFrom.getRootElement(), "/" + rootElementName, object);
                    removeElementFromXSL(docFrom.getRootElement(), docTo.getRootElement(), "/" + rootElementName, object);
                    addElementToXSL(docTo.getRootElement(), docFrom.getRootElement(), "/" + rootElementName, object);
                }
            } catch (Exception ex) {
                debug("Error " + directoryFrom.getAbsolutePath() + " " + directoryTo.getAbsolutePath());
                ex.printStackTrace();
            }
        }
    }

    public boolean excludeAttribute_add(String name) {
        /* if(name.equals("NID") ){
             return true;
         }
         if(name.equals("NIDScheme") ){
             return true;
         }
         if(name.equals("Parse") ){
             return true;
         }
         if(name.equals("KeyElementRelativeXPath") ){
             return true;
         }
         if(name.equals("noNamespaceSchemaLocation") ){
             return true;
         }*/
        if (name.equals("serviceVer")) {
            return true;
        }
        return false;
    }

    public boolean excludeAttribute_del(String name) {
        if (name.equals("NID")) {
            return true;
        }
        if (name.equals("NIDScheme")) {
            return true;
        }
        if (name.equals("Parse")) {
            return true;
        }
        if (name.equals("KeyElementRelativeXPath")) {
            return true;
        }
        if (name.equals("noNamespaceSchemaLocation")) {
            return true;
        }
        /*if(name.equals("serviceVer") ){
            return true;
        }   */
        return false;
    }

    public void removeAttrFromXSL(Element fromEL, Element toEL, String xpath, CreateDiffXML.ObjectXSL object) throws Exception {
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            String valueAttrTo = attrEL.getValue().trim();
            if (toEL.attributeValue(nameAttrTo) == null) {
                if (!excludeAttribute_del(nameAttrTo)) {
                    object.attr_deleted.put(xpath + "/@" + nameAttrTo, valueAttrTo);
                }
            }
        }
        //##loop children    
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                removeAttrFromXSL(toChildEL, fromChildEL, childXpath, object);
            }
        }
    }

    public void removeElementFromXSL(Element fromEL, Element toEL, String xpath, CreateDiffXML.ObjectXSL object) throws Exception {
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                removeElementFromXSL(toChildEL, fromChildEL, childXpath, object);
            } else {
                object.EL_deleted.put(childXpath, toChildEL.asXML());
            }
        }
    }

    public void addAttrToXSL(Element fromEL, Element toEL, String xpath, CreateDiffXML.ObjectXSL object) throws Exception {
        boolean addAttributes = false;
        Hashtable<String, String> chldList = new Hashtable<String, String>();
        List<?> toChildELAttrList = Dom4jUtils.getAttributeList(fromEL);
        for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
            Attribute attrEL = (Attribute) k.next();
            String nameAttrTo = attrEL.getName().trim();
            String valueAttrTo = attrEL.getValue();
            if (toEL.attributeValue(nameAttrTo) == null) {
                if (!excludeAttribute_add(nameAttrTo)) {
                    chldList.put(nameAttrTo, valueAttrTo);
                    addAttributes = true;
                }
            }
        }
        if (addAttributes) {
            object.attr_added.put(xpath, chldList);
        }

        //##loop children    
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                addAttrToXSL(toChildEL, fromChildEL, childXpath, object);
            }
        }
    }

    public void addElementToXSL(Element fromEL, Element toEL, String xpath, CreateDiffXML.ObjectXSL object) throws Exception {
        boolean addChildElments = false;
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        Hashtable<String, String> chldList = new Hashtable<String, String>();
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (fromChildEL != null) {
                addElementToXSL(toChildEL, fromChildEL, childXpath, object);
            } else {
                chldList.put(name, toChildEL.asXML());
                addChildElments = true;
            }
        }
        if (addChildElments) {
            object.EL_added.put(xpath, chldList);
        }
    }

    public void addElementToXSL2(Element fromEL, Element toEL, String xpath, CreateDiffXML.ObjectXSL object) throws Exception {
        boolean addChildElments = false;
        List<?> childListTo = Dom4jUtils.getList(fromEL);
        Hashtable<String, String> chldList = new Hashtable<String, String>();
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(toEL, name);
            String childXpath = xpath + "/" + name;
            if (name.equals("DeviceStatus")) {
                chldList.put(name, toChildEL.asXML());
                addChildElments = true;
            } else {
                if (fromChildEL != null) {
                    addElementToXSL2(toChildEL, fromChildEL, childXpath, object);
                }
            }
        }
        if (addChildElments) {
            object.EL_added.put(xpath, chldList);
        }
    }

    private void debug(String msg) {
        System.out.println(msg);
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

            buftXSLPrefix.append("\n \n");
            buftXSLPrefix.append("\n    <xsl:template  match=\"@serviceVer\">");
            buftXSLPrefix.append("\n    <xsl:attribute name=\"serviceVer\"></xsl:attribute>");
            buftXSLPrefix.append("\n    </xsl:template>");
        }
        return buftXSLPrefix;
    }

    private static StringBuffer getXSLSuffix() {
        StringBuffer buftXSLPrefix = new StringBuffer();
        buftXSLPrefix.append("\n</xsl:stylesheet>");
        return buftXSLPrefix;
    }

    private static class ObjectXSL {
        String rootElementName = "";
        Hashtable<String, String> attr_deleted = new Hashtable<String, String>();
        Hashtable<String, String> EL_deleted = new Hashtable<String, String>();

        Hashtable<String, Hashtable<String, String>> attr_added = new Hashtable<String, Hashtable<String, String>>();
        Hashtable<String, Hashtable<String, String>> EL_added = new Hashtable<String, Hashtable<String, String>>();
        boolean saveXSL = false;

        public void writeXSL(String xslName, String rootElementName, String directoryFromName, String directoryToName) throws Exception {
            String title = "";

            CompUtils.writeFile(xslName, getXSLPrefix().toString(), true);
            if (devicestatus) {
                //add as comment
                if (EL_added.size() > 0) {
                    title = "replace contents of " + rootElementName + "/DeviceStatus";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    //create elements
                    for (String xpath : EL_added.keySet()) {
                        saveXSL = true;
                        Hashtable<String, String> childList = EL_added.get(xpath);
                        if (childList != null) {
                            StringBuffer xslBuf = new StringBuffer();
                            for (String childName : childList.keySet()) {
                                xslBuf.append("\n    <xsl:template  match=\"" + xpath + "/" + childName + "\">");
                                xslBuf.append("\n              " + childList.get(childName));
                                xslBuf.append("\n    </xsl:template>\n");
                            }
                            CompUtils.writeFile(xslName, xslBuf.toString(), true);
                        }
                    }
                }

            } else {

                if (attr_deleted.size() > 0) {
                    title = "DELETE ATTRIBUTES " + rootElementName + " ( were present in '" + directoryFromName + "' but removed from '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    for (String xpath : attr_deleted.keySet()) {
                        CompUtils.writeFile(xslName, "\n   <xsl:template  match=\"" + xpath + "\"/>", true);
                        saveXSL = true;
                    }
                }
                if (attr_added.size() > 0) {
                    title = "Add ATTRIBUTES " + rootElementName + " ( were absent in '" + directoryFromName + "' but added to  '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    for (String xpath : attr_added.keySet()) {
                        saveXSL = true;
                        Hashtable<String, String> attrList = attr_added.get(xpath);
                        if (attrList != null) {
                            StringBuffer xslBuf = new StringBuffer();
                            xslBuf.append("\n    <xsl:template mode=\"add-atts\" match=\"" + xpath + "\">");
                            for (String attrName : attrList.keySet()) {
                                xslBuf.append("\n      <xsl:choose>");
                                xslBuf.append("\n          <xsl:when test=\"@" + attrName + "\">");
                                xslBuf.append("\n          </xsl:when>");
                                xslBuf.append("\n          <xsl:otherwise>");
                                xslBuf.append("\n              <xsl:attribute name=\"" + attrName + "\">" + attrList.get(attrName) + "</xsl:attribute>");
                                xslBuf.append("\n          </xsl:otherwise>");
                                xslBuf.append("\n      </xsl:choose> ");
                            }
                            xslBuf.append("\n    </xsl:template>\n");
                            CompUtils.writeFile(xslName, xslBuf.toString(), true);
                        }
                    }
                }

                if (EL_deleted.size() > 0) {
                    title = "DELETE ELEMENTS " + rootElementName + " ( were present in '" + directoryFromName + "' but removed from '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    for (String xpath : EL_deleted.keySet()) {
                        CompUtils.writeFile(xslName, "\n   <xsl:template  match=\"" + xpath + "\"/>", true);
                        saveXSL = true;
                    }
                }
                //add as comment
                if (EL_added.size() > 0) {
                    title = "Add ELEMENTS " + rootElementName + " ( were absent in '" + directoryFromName + "' but added to  '" + directoryToName + "')";
                    CompUtils.writeFile(xslName, "\n\n\n<!-- " + title + " -->\n", true);
                    //write log for move
                    StringBuffer xslBuf = new StringBuffer();
                    xslBuf.append("\n    <!-- ");
                    for (String xpath : EL_added.keySet()) {
                        saveXSL = true;
                        Hashtable<String, String> childList = EL_added.get(xpath);
                        if (childList != null) {
                            for (String childName : childList.keySet()) {
                                xslBuf.append("\n          " + xpath + "/" + childName);
                            }
                        }
                    }
                    xslBuf.append("\n    -->\n");
                    CompUtils.writeFile(xslName, xslBuf.toString(), true);

                    //create elements
                    for (String xpath : EL_added.keySet()) {
                        saveXSL = true;
                        Hashtable<String, String> childList = EL_added.get(xpath);
                        if (childList != null) {
                            xslBuf = new StringBuffer();
                            xslBuf.append("\n    <xsl:template mode=\"append\" match=\"" + xpath + "\">");
                            for (String childName : childList.keySet()) {
                                xslBuf.append("\n      <xsl:choose>");
                                xslBuf.append("\n          <xsl:when test=\"" + childName + "\">");
                                xslBuf.append("\n          </xsl:when>");
                                xslBuf.append("\n          <xsl:otherwise>");
                                xslBuf.append("\n              " + childList.get(childName));
                                xslBuf.append("\n          </xsl:otherwise>");
                                xslBuf.append("\n      </xsl:choose> ");
                            }
                            xslBuf.append("\n    </xsl:template>\n");
                            CompUtils.writeFile(xslName, xslBuf.toString(), true);
                        }
                    }
                }

            }
            if (!saveXSL) {
                File file = new File(xslName);
                file.delete();
            } else {
                CompUtils.writeFile(xslName, getXSLSuffix().toString(), true);
            }
        }

        public void writeDiff_deletedAttr(String path, String rootElementName, String directoryFromName, String directoryToName, boolean details) throws Exception {
            StringBuffer bufXsl = new StringBuffer();
            if (details) {
                CompUtils.writeFile(path, "\n###### " + rootElementName + "## del attr ##from " + directoryFromName + " to " + directoryToName + "", true);
            }
            for (String xpath : attr_deleted.keySet()) {
                if (details) {
                    //bufXsl.append("\n(DEL_ATTR) " + xpath + " ( " + attr_deleted.get(xpath).replace('\n', ' ') + ")");
                    bufXsl.append("\n(DEL_ATTR) " + xpath + " ( " + attr_deleted.get(xpath) + ")");
                } else {
                    bufXsl.append("\n(DEL_ATTR) " + xpath);
                }
            }
            CompUtils.writeFile(path, bufXsl.toString(), true);
        }

        public void writeDiff_deletedEL(String path, String rootElementName, String directoryFromName, String directoryToName, boolean details) throws Exception {
            StringBuffer bufXsl = new StringBuffer();
            if (details) {
                CompUtils.writeFile(path, "\n###### " + rootElementName + "## del el ##from " + directoryFromName + " to " + directoryToName + "", true);
            }
            for (String xpath : EL_deleted.keySet()) {
                if (details) {
                    // bufXsl.append("\n(DEL_EL) " + xpath + " ( " + EL_deleted.get(xpath).replace('\n', ' ') + ")");
                    bufXsl.append("\n(DEL_EL) " + xpath + " ( " + EL_deleted.get(xpath) + ")");
                } else {
                    bufXsl.append("\n(DEL_EL) " + xpath);
                }
            }
            CompUtils.writeFile(path, bufXsl.toString(), true);
        }

        public void writeDiff_addedAttr(String path, String rootElementName, String directoryFromName, String directoryToName, boolean details) throws Exception {
            StringBuffer bufXsl = new StringBuffer();
            if (details) {
                CompUtils.writeFile(path, "\n###### " + rootElementName + "## add attr ## from " + directoryFromName + " to " + directoryToName + "", true);
            }
            for (String xpath : attr_added.keySet()) {
                Hashtable<String, String> childList = attr_added.get(xpath);
                if (childList != null) {
                    for (String childName : childList.keySet()) {
                        if (details) {
                            //bufXsl.append("\n(ADD_ATTR) " + xpath + "/@" + childName + " ( " + childList.get(childName).replace('\n', ' ') + ")");
                            bufXsl.append("\n(ADD_ATTR) " + xpath + "/@" + childName + " ( " + childList.get(childName) + ")");
                        } else {
                            bufXsl.append("\n(ADD_ATTR) " + xpath + "/@" + childName);
                        }
                    }
                }
            }
            CompUtils.writeFile(path, bufXsl.toString(), true);
        }

        public void writeDiff_addedEL(String path, String rootElementName, String directoryFromName, String directoryToName, boolean details) throws Exception {
            StringBuffer bufXsl = new StringBuffer();
            if (details) {
                CompUtils.writeFile(path, "\n###### " + rootElementName + "## add el ##from " + directoryFromName + " to " + directoryToName + "", true);
            }
            for (String xpath : EL_added.keySet()) {
                Hashtable<String, String> childList = EL_added.get(xpath);
                if (childList != null) {
                    for (String childName : childList.keySet()) {
                        if (details) {
                            //bufXsl.append("\n(ADD_EL) " + xpath + "/" + childName + " ( " + childList.get(childName).replace('\n', ' ') + ")");
                            bufXsl.append("\n(ADD_EL) " + xpath + "/" + childName + " ( " + childList.get(childName) + ")");
                        } else {
                            bufXsl.append("\n(ADD_EL) " + xpath + "/" + childName);
                        }
                    }
                }
            }
            CompUtils.writeFile(path, bufXsl.toString(), true);
        }
    }

}
