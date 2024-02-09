package com.ipvsserv.common.config;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.concurrent.ConcurrentHashMap;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class XMLProperties {
    
    private File file;
    private Document document;
    
    public static final String MULTI_EL_ATTR_SEP = "@";
    public static final String ID_PREFIX = "id_";
    public static final String ID_ATTRIBUTE = "ipvsid";
    public static final String MULTI_ELEMENT_NAME = "multiElement";
    
    private Map<String, String> propertyCache = new HashMap<String, String>();
    private boolean enableSaveXMLProperties=false;
    
    
    public XMLProperties(String fileName) throws IOException {
        this(new File(fileName));
    }
    
    public XMLProperties(InputStream in) throws IOException {
        Reader reader = new BufferedReader(new InputStreamReader(in));
        buildDoc(reader);
    }
    
    public XMLProperties(File file) throws IOException {
        this.file = file;
        if (!file.exists()) {
            // Attempt to recover from this error case by seeing if the
            // tmp file exists. It's possible that the rename of the
            // tmp file failed the last time Jive was running,
            // but that it exists now.
            File tempFile;
            tempFile = new File(file.getParentFile(), file.getName() + ".tmp");
            if (tempFile.exists()) {
                MRLog.error("WARNING: " + file.getName() + " was not found, but temp file from " + "previous write operation was. Attempting automatic recovery." + " Please check file for data consistency.");
                tempFile.renameTo(file);
            }
            // There isn't a possible way to recover from the file not
            // being there, so throw an error.
            else {
                throw new FileNotFoundException("XML properties file does not exist: " + file.getName());
            }
        }
        // Check read and write privs.
        if (!file.canRead()) {
            throw new IOException("XML properties file must be readable: " + file.getName());
        }
        if (!file.canWrite()) {
            throw new IOException("XML properties file must be writable: " + file.getName());
        }
        
        FileReader reader = new FileReader(file);
        buildDoc(reader);
    }
    
    private void buildDoc(Reader in) throws IOException {
        try {
            SAXReader xmlReader = new SAXReader();
            document = xmlReader.read(in);
        } catch (Exception ex) {
            MRLog.error(ex);
            throw new IOException(ex.getMessage());
        } finally {
            if (in != null) {
                in.close();
            }
        }
    }
    //////////////////////////////////// added pravin ///////////////////
    public void setElement(Element el) {
        if(el==null)
            return;
        Element exist= getElement(el.getName());
        if(exist!=null)
            exist.detach();
        Element root = document.getRootElement();
        Dom4jUtils.addElement(root, (Element)el.clone());
    }
    
    public Element getElementXpath(String propName) {
        return Dom4jUtils.getElementXpath(document.getRootElement(), propName);
    }
    
    public Element getElement(String nameToResolve) {
        return getResolvedElement(nameToResolve);
    }
    
    private Element getResolvedElement(String nameToResolve) {
        String[] propName = parseXMLPropertyName(nameToResolve);
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return null.
                return null;
            }
        }
        return element;
    }
    
    private Element getResolvedElement(Element pelement, String propName) {
        Element element = null;
        if (propName.indexOf(MULTI_EL_ATTR_SEP) != -1) {
            String id = "";
            String elementName = propName.substring(0, propName.indexOf(MULTI_EL_ATTR_SEP));
            String idValue = propName.substring(propName.indexOf(MULTI_EL_ATTR_SEP) + 1);
            List<?> list = pelement.elements(elementName);
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                id = el.attributeValue(ID_ATTRIBUTE);
                if (id != null && !id.trim().equals("") && id.equalsIgnoreCase(idValue)) {
                    element = el;
                    break;
                }
            }
        } else {
            element = pelement.element(propName);
        }
        return element;
    }
    
    private Element setResolvedElement(Element pelement, String propName) {
        Element element = null;
        if (propName.indexOf(MULTI_EL_ATTR_SEP) != -1) {
            element = getResolvedElement(pelement, propName);
            if (element == null) {
                String elementName = propName.substring(0, propName.indexOf(MULTI_EL_ATTR_SEP));
                String idValue = propName.substring(propName.indexOf(MULTI_EL_ATTR_SEP));
                element = pelement.addElement(elementName);
                element.addAttribute(ID_ATTRIBUTE, idValue);
            }
        } else {
            element = pelement.element(propName);
            if (element == null) {
                element = pelement.addElement(propName);
            }
        }
        return element;
    }
    
    private String[] getResolvedElementNames(Element pelement) {
        List children = pelement.elements();
        int childCount = children.size();
        String[] childrenNames = new String[childCount];
        for (int i = 0; i < childCount; i++) {
            Element el = (Element) children.get(i);
            String elementName = el.getName();
            String idValue = el.attributeValue(ID_ATTRIBUTE);
            String childrenName = "";
            if (idValue != null && idValue.trim().equals("")) {
                childrenName = elementName + MULTI_EL_ATTR_SEP + idValue;
            } else {
                childrenName = elementName;
            }
            childrenNames[i] = childrenName;
        }
        return childrenNames;
    }
    
    /*public void fillModuleParams(String name, Map<String,String> params , String propPrefix) {
        String[] propName = parsePropertyName(name);
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return null.
                return ;
            }
        }
        fillModuleParams(propPrefix, element, params );
    }*/

    public void fillModuleParams(String moduleName, Map<String, String> params) {
        /*String[] propName = parsePropertyName(moduleName);
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return null.
                return;
            }
        }*/
        Element element = getResolvedElement(moduleName);
        fillModuleParams("", element, params);
    }
    
    private void fillModuleParams(String propPrefix, Element pelement, Map<String, String> params) {
        if (pelement == null) return;
        String key = "";
        if (propPrefix == null || propPrefix.equals("")) {
            key = pelement.getName();
        } else {
            key = propPrefix + "." + pelement.getName();
        }
        String value = pelement.getTextTrim();
        if (value != null && !value.trim().equals("")) params.put(key, value);
        List children = pelement.elements();
        int childCount = children.size();
        for (int i = 0; i < childCount; i++) {
            Element el = (Element) children.get(i);
            fillModuleParams(key, el, params);
        }
    }
    
    public Map<String, Map<String, String>> getModules() {
        return getModules("modules.priorityLevels", "modules", "module");
    }
    
    public Map<String, Map<String, String>> getModules(String priorityLevelsELName, String modulesElName, String moduleElName) {
        /*
        String[] propName = parsePropertyName(modulesElName);
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return null.
                return null;
            }
        }
        */
        Element element = getResolvedElement(modulesElName);
        if (element == null) return new LinkedHashMap<String, Map<String, String>>();
        ;
        return getModules(element, priorityLevelsELName, modulesElName, moduleElName);
    }
    
    public Map<String, Map<String, String>> getModulesXpath(String priorityLevelsELName, String modulesElName, String moduleElName) {
        Element element = Dom4jUtils.getElementXpath(document.getRootElement(), modulesElName);
        if (element == null) return null;
        return getModules(element, priorityLevelsELName, modulesElName, moduleElName);
    }
    
    public Map<String, Map<String, String>> getModules(Element element, String priorityLevelsELName, String modulesElName, String moduleElName) {
        
        int priorityLevels = CompUtils.intValue(getProperty(priorityLevelsELName), 20);
        
        Map<String, Map<String, String>> modules = new LinkedHashMap<String, Map<String, String>>();
        Iterator iter = element.elementIterator(moduleElName);//"module");
        while (iter.hasNext()) {
            Element elModule = (Element) iter.next();
            boolean moduleEnable = CompUtils.booleanValue(elModule.attributeValue("enable"), false);
            if (!moduleEnable) continue;
            String moduleName = elModule.attributeValue("name");
            int priority = CompUtils.intValue(elModule.attributeValue("priority"), priorityLevels);
            Map<String, String> params = new ConcurrentHashMap<String, String>();
            //if(moduleEnable) {
            String className = elModule.elementTextTrim("class");
            if(className==null){
                className="";
            }
            params.put("class", className);
            params.put("name", moduleName);
            params.put("enable", moduleEnable + "");
            params.put("priority", priority + "");
            params.put("xpath", modulesElName + "/"+elModule.getName()+"[@name='"+moduleName+"']");
            modules.put(moduleName, params);
            
            Iterator iterParams = elModule.elementIterator("param");
            while (iterParams.hasNext()) {
                Element elParam = (Element) iterParams.next();
                String paramName = elParam.attributeValue("name");
                String paramValue = elParam.getTextTrim();
                if (paramName != null && paramValue != null) {
                    params.put(paramName.trim(), paramValue.trim());
                }
            }
            modules.put(moduleName, params);
            //}
            
        }
        return modules;
    }
    
    public Map<String, String> getParams(String name) {
        Map<String, String> params = new LinkedHashMap<String, String>();
        Element element = getResolvedElement(name);
        if (element == null) {
            return params;
        }
        if (element != null) {
            Iterator iter = element.elementIterator("param");
            while (iter.hasNext()) {
                Element elModule = (Element) iter.next();
                String paramName = elModule.attributeValue("name");
                String paramValue = elModule.getTextTrim();
                params.put(paramName, paramValue);
                
            }
        }
        return params;
    }
    
    //////////////////////////////////// added pravin ///////////////////
    public synchronized String getProperty(String name) {
        String value = propertyCache.get(name);
        if (value != null) {
            return value;
        }
        /*
        String[] propName = parsePropertyName(name);
        // Search for this property by traversing down the XML heirarchy.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return null.
                return null;
            }
        }*/
        Element element = getResolvedElement(name);
        if (element == null) {
            return null;
        }
        // At this point, we found a matching property, so return its value.
        // Empty strings are returned as null.
        value = element.getTextTrim();
        if ("".equals(value)) {
            return null;
        } else {
            // Add to cache so that getting property next time is fast.
            propertyCache.put(name, value);
            //MRLog.info("getXMLProperty '" + name + "' = '" + value + "'");
            return value;
        }
    }
    
    public String[] getProperties(String nameToResolve) {
        /* String[] propName = parsePropertyName(nameToResolve);
         // Search for this property by traversing down the XML heirarchy,
         // stopping one short.
         Element element = document.getRootElement();
         for (int i = 0; i < propName.length - 1; i++) {
             element = getResolvedElement(element, propName[i]);
             //element = element.element(propName[i]);
             if (element == null) {
                 // This node doesn't match this part of the property name which
                 // indicates this property doesn't exist so return empty array.
                 return new String[] {};
             }
         }*/
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return new String[] {};
        }
        
        String[] propName = parseXMLPropertyName(nameToResolve);
        // We found matching property, return names of children.
        //Iterator iter = element.elementIterator(propName[propName.length - 1]);
        element = getResolvedElement(element, propName[propName.length - 1]);
        Iterator iter = element.elementIterator();
        List<String> props = new ArrayList<String>();
        String value;
        while (iter.hasNext()) {
            // Empty strings are skipped.
            value = ((Element) iter.next()).getTextTrim();
            if (!"".equals(value)) {
                props.add(value);
            }
        }
        String[] childrenNames = new String[props.size()];
        return props.toArray(childrenNames);
    }
    public String[] getValues(String nameToResolve) {
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return new String[] {};
        }
        Element parent= element.getParent();
        Iterator<?> iter = parent.elementIterator(element.getName());
        List<String> props = new ArrayList<String>();
        String value;
        while (iter.hasNext()) {
            // Empty strings are skipped.
            value = ((Element) iter.next()).getTextTrim();
            if (!"".equals(value)) {
                props.add(value);
            }
        }
        String[] childrenNames = new String[props.size()];
        return props.toArray(childrenNames);
    }
    
    public Iterator getChildProperties(String nameToResolve) {
        /*String[] propName = parsePropertyName(name);
        // Search for this property by traversing down the XML heirarchy,
        // stopping one short.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length - 1; i++) {
            element = getResolvedElement(element, propName[i]);
            //element = element.element(propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return empty array.
                return Collections.EMPTY_LIST.iterator();
            }
        }*/
        ArrayList<String> props = new ArrayList<String>();
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return props.iterator();
        }
        String[] propName = parseXMLPropertyName(nameToResolve);
        // We found matching property, return values of the children.
        element = getResolvedElement(element, propName[propName.length - 1]);
        Iterator iter = element.elementIterator();
        //Iterator iter = element.elementIterator(propName[propName.length - 1]);
        while (iter.hasNext()) {
            props.add(((Element) iter.next()).getText());
        }
        return props.iterator();
    }
    
    public String getAttribute(String nameToResolve, String attribute) {
        if (nameToResolve == null || attribute == null) {
            return null;
        }
        /*String[] propName = parsePropertyName(name);
        // Search for this property by traversing down the XML heirarchy.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            //String child = propName[i];
            //element = element.element(child);
            element = getResolvedElement(element, propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return empty array.
                break;
            }
        }*/
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return null;
        }
        if (element != null) {
            // Get its attribute values
            return element.attributeValue(attribute);
        }
        return null;
    }
    
    public void setProperties(String nameToResolve, List<String> values) {
        /*String[] propName = parsePropertyName(nameToResolve);
        // Search for this property by traversing down the XML heirarchy,
        // stopping one short.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length - 1; i++) {
            // If we don't find this part of the property in the XML heirarchy
            // we add it as a new node
            //if (element.element(propName[i]) == null) {
            //    element.addElement(propName[i]);
            //}
            //element = element.element(propName[i]);
            element = setResolvedElement(element, propName[i]);
        }*/
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return;
        }
        String[] propName = parseXMLPropertyName(nameToResolve);
        // We found matching property, clear all children.
        List<Element> toRemove = new ArrayList<Element>();
        element = getResolvedElement(element, propName[propName.length - 1]);
        Iterator<?> iter = element.elementIterator();
        //String childName = propName[propName.length - 1];
        //Iterator iter = element.elementIterator(childName);
        while (iter.hasNext()) {
            toRemove.add((Element)iter.next());
        }
        for (iter = toRemove.iterator(); iter.hasNext();) {
            element.remove((Element) iter.next());
        }
        // Add the new children.
        for (String value : values) {
            //Element childElement = element.addElement(childName);
            Element childElement = setResolvedElement(element, propName[propName.length - 1]);
            if (value.startsWith("<![CDATA[")) {
                childElement.addCDATA(value.substring(9, value.length() - 3));
            } else {
                childElement.setText(value);
            }
        }
        saveXMLProperties();
        
        // Generate event.
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("value", values);
        PropertyEventDispatcher.dispatchEvent(nameToResolve, PropertyEventDispatcher.EventType.xml_property_set, params);
    }
    
    public String[] getChildrenProperties(String nameToResolve) {
        /*String[] propName = parsePropertyName(parent);
        // Search for this property by traversing down the XML heirarchy.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            element = getResolvedElement(element, propName[i]);
            //element = element.element(propName[i]);
            if (element == null) {
                // This node doesn't match this part of the property name which
                // indicates this property doesn't exist so return empty array.
                return new String[] {};
            }
        }*/
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return new String[] {};
        }
        // We found matching property, return names of children.
        //List children = element.elements();
        //int childCount = children.size();
        //String[] childrenNames = new String[childCount];
        //for (int i = 0; i < childCount; i++) {
        //    childrenNames[i] = ((Element)children.get(i)).getName();
        //}
        //return childrenNames;
        return getResolvedElementNames(element);
    }
    
    public synchronized void setXMLProperty(String name, String value) {
        if (name == null) {
            return;
        }
        if (value == null) {
            value = "";
        }
        
        // Set cache correctly with prop name and value.
        propertyCache.put(name, value);
        
        String[] propName = parseXMLPropertyName(name);
        // Search for this property by traversing down the XML heirarchy.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length; i++) {
            // If we don't find this part of the property in the XML heirarchy
            // we add it as a new node
            //if (element.element(propName[i]) == null) {
            //    element.addElement(propName[i]);
            //}
            //element = element.element(propName[i]);
            element = setResolvedElement(element, propName[i]);
        }
        if (element == null) return;
        // Set the value of the property in this node.
        if (value.startsWith("<![CDATA[")) {
            element.addCDATA(value.substring(9, value.length() - 3));
        } else {
            element.setText(value);
        }
        // Write the XML properties to disk
        saveXMLProperties();
        
        // Generate event.
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("value", value);
        PropertyEventDispatcher.dispatchEvent(name, PropertyEventDispatcher.EventType.xml_property_set, params);
    }
    
    public synchronized void deleteXMLProperty(String nameToResolve) {
        // Remove property from cache.
        propertyCache.remove(nameToResolve);
        
        /*String[] propName = parsePropertyName(nameToResolve);
        // Search for this property by traversing down the XML heirarchy.
        Element element = document.getRootElement();
        for (int i = 0; i < propName.length - 1; i++) {
            //element = element.element(propName[i]);
            element = getResolvedElement(element, propName[i]);
            // Can't find the property so return.
            if (element == null) {
                return;
            }
        }*/
        Element element = getResolvedElement(nameToResolve);
        if (element == null) {
            return;
        }
        String[] propName = parseXMLPropertyName(nameToResolve);
        // Found the correct element to remove, so remove it...
        //element.remove(element.element(propName[propName.length - 1]));
        element = getResolvedElement(element, propName[propName.length - 1]);
        element.getParent().remove(element);
        // .. then write to disk.
        saveXMLProperties();
        
        // Generate event.
        PropertyEventDispatcher.dispatchEvent(nameToResolve, PropertyEventDispatcher.EventType.xml_property_deleted, new HashMap<String, Object>());
    }
    
 
    private synchronized void saveXMLProperties() {
        if(!enableSaveXMLProperties)
            return ;
        boolean error = false;
        // Write data out to a temporary file first.
        File tempFile = null;
        Writer writer = null;
        try {
            tempFile = new File(file.getParentFile(), file.getName() + ".tmp");
            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(tempFile)));
            OutputFormat prettyPrinter = OutputFormat.createPrettyPrint();
            XMLWriter xmlWriter = new XMLWriter(writer, prettyPrinter);
            xmlWriter.write(document);
        } catch (Exception ex) {
            MRLog.error(ex);
            // There were errors so abort replacing the old property file.
            error = true;
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                    error = true;
                }
            }
        }
        
        // No errors occured, so delete the main file.
        if (!error) {
            // Delete the old file so we can replace it.
            if (!file.delete()) {
                MRLog.error("Error deleting property file: " + file.getAbsolutePath());
                return;
            }
            // Copy new contents to the file.
            try {
                CompUtils.copy(tempFile, file);
            } catch (Exception ex) {
                MRLog.error(ex);
                // There were errors so abort replacing the old property file.
                error = true;
            }
            // If no errors, delete the temp file.
            if (!error) {
                tempFile.delete();
            }
        }
    }
    
    private String[] parseXMLPropertyName(String name) {
        List<String> propName = new ArrayList<String>(5);
        // Use a StringTokenizer to tokenize the property name.
        StringTokenizer tokenizer = new StringTokenizer(name, ".");
        while (tokenizer.hasMoreTokens()) {
            propName.add(tokenizer.nextToken());
        }
        return propName.toArray(new String[propName.size()]);
    }
    
    public void setXMLProperties(Map<String, String> propertyMap) {
        for (String propertyName : propertyMap.keySet()) {
            String propertyValue = propertyMap.get(propertyName);
            setXMLProperty(propertyName, propertyValue);
        }
    }
    
   
}
