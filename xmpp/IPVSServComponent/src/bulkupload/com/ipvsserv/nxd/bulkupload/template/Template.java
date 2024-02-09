package com.ipvsserv.nxd.bulkupload.template;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class Template {
    private Document doc = null;
    private String templateId = "";
    private String listname = "";
    private String objectXmlResource = "";
    private String objectXmlFile = "";
    private List<String> updateXmlDefinations = new ArrayList<String>();
    
    public Template(Element el) {
        
        templateId = Dom4jUtils.getAttributeValue(el, "templateId", "").trim();
        listname = Dom4jUtils.getAttributeValue(el, "listName", "").trim();
        objectXmlResource = Dom4jUtils.getAttributeValue(el, "objectXmlResource", "").trim();
        objectXmlFile = Dom4jUtils.getAttributeValue(el, "objectXmlFile", "").trim();
        String updateXmlDefinationId = Dom4jUtils.getAttributeValue(el, "updateXmlDefinationId", "").trim();
        if (!updateXmlDefinationId.equals("")) {
            updateXmlDefinations.add(updateXmlDefinationId);
        }
        if (objectXmlResource != null && !objectXmlResource.trim().equals("")) {
            try {
                doc = Dom4jUtils.loadXMLResource(objectXmlResource);
            } catch (Exception ex) {
                MRLog.error("ERROR ObjectXml not found templateId='" + templateId + "' '" + objectXmlResource + "'");
                MRLog.error(ex);
            }
        }
        if (doc == null) {
            File file = new File(objectXmlFile);
            if(!file.exists()){
                file = new File(IpvsServGlobals.getHomeDirectory()+File.separator+objectXmlFile);
                if(!file.exists()){
                    file = new File(IpvsServGlobals.getHomeDirectory()+File.separator+   "testXmls" + File.separator+objectXmlFile);
                }
            }
            //load xml///////////
            if(file.exists()){
                try {
                    doc = Dom4jUtils.loadXMLFile(file.getAbsolutePath());
                } catch (Exception ex) {
                    MRLog.error("ERROR ObjectXml not found templateId='" + templateId + "' '" + objectXmlResource + "'");
                    MRLog.error(ex);
                }
               
            }
            
        }        
       
        if (doc == null) {
            MRLog.error("ERROR ObjectXml not found templateId='" + templateId + "' " + objectXmlFile +"  "+ objectXmlResource + "");
        }
        List<?> list = Dom4jUtils.listXpath(el, "updateXml");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            updateXmlDefinationId = Dom4jUtils.getAttributeValue(el2, "updateXmlDefinationId", "").trim();
            if (updateXmlDefinationId.equals("")){
                updateXmlDefinations.add(updateXmlDefinationId);
            }
            
        }
    }
    
    public List<String> getUpdateXmlDefinations() {
        return updateXmlDefinations;
    }
    
    public String gettemplateId() {
        return templateId;
    }
    
    public String getListName() {
        return listname;
    }
    
    public Document getDoc() {
        return doc;
    }
    
    public Document getXml() throws Exception {
        if (doc == null) {
            throw new Exception("SKIP.  templateId is not loaded '" + templateId + "' " + objectXmlResource +" " + objectXmlFile );
        }
        //MRLog.debug("start.  templateId loading '" + templateId + "' ");
        Document doc2 = Dom4jUtils.getDocument();
        doc2.add((Element) doc.getRootElement().clone());
        Element element = doc2.getRootElement();
        return doc2;
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<template templateId='" + templateId + "' listName='" + listname + "' objectXmlFile='" + objectXmlFile + "' objectXmlResource='" + objectXmlResource + "'>");
        
        buf.append("\n</template>");
        return buf.toString();
    }
    
}
