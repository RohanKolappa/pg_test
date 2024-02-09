package com.ipvsserv.nxd.bulkupload.template;

import org.dom4j.Document;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class TemplateDefination {
    private String templateDefinationXmlPath = "templateDefination.xml";
    private Document doc = null;
    private GroupTemplates groupTemplates = null;
    private Templates templates = null;
    private UpdateXmlDefinations updateXmlDefinations = null;
    
    public TemplateDefination() {

    }
    
    public void loadXML() throws Exception {
        doc = Dom4jUtils.loadXMLFile(templateDefinationXmlPath);
        loadXML(doc);
    }
    
    public void loadXML(String xml) throws Exception {
        doc = Dom4jUtils.stringToDocument(xml);
        loadXML(doc);
    }
    
    public void loadXML(Document doc) throws Exception {
        this.doc = doc;
        groupTemplates = new GroupTemplates(doc.getRootElement());
        templates = new Templates(doc.getRootElement());
        updateXmlDefinations = new UpdateXmlDefinations(doc.getRootElement());
    }
    
    public Document getDoc() {
        return doc;
    }
    
    public void setDoc(Document doc) {
        this.doc = doc;
    }
    
    public void setTemplateDefinationXmlPath(String templateDefinationXmlPath) {
        this.templateDefinationXmlPath = templateDefinationXmlPath;
    }
    
    public String getTemplateDefinationXmlPath() {
        return templateDefinationXmlPath;
    }
    
    /*
    public groupTemplates getgroupTemplates() {
        return groupTemplates;
    }
    public templates gettemplates() {
        return templates;
    }
    
    */
    public UpdateXmlDefinations getUpdateXmlDefinations() {
        return updateXmlDefinations;
    }
    
    public GroupTemplate getgroupTemplate(String groupTemplateId) {
        return groupTemplates.getgroupTemplate(groupTemplateId);
    }
    
    public Template gettemplate(String templateId) {
        return templates.gettemplate(templateId);
    }
    
    public UpdateXmlDefination getUpdateXmlDefination(String updateXmlDefinationId) {
        return updateXmlDefinations.getUpdateXmlDefination(updateXmlDefinationId);
    }
    
}
