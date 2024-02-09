package com.ipvsserv.nxd.bulkupload.template;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsDBUtils;
import com.ipvsserv.nxd.bulkupload.BulkUploadUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class BulkLoadTemplateIterator {
    private TemplateDefination templateDefination = null;
    private Element loadObjectsEL = null;
    private int count = 0;
    private String loadObjectsId = "";
    private String ipvsdomain = "";
    private int position = 0;
    private boolean enable = false;
    
    public BulkLoadTemplateIterator(TemplateDefination templateDefination, Element loadObjectsEL) {
        this.templateDefination = templateDefination;
        this.loadObjectsEL = loadObjectsEL;
        this.count = CompUtils.intValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "count", count + ""), count);
        ;
        this.ipvsdomain = Dom4jUtils.getAttributeValue(loadObjectsEL, "ipvsdomain", ipvsdomain);
        this.enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "enable", "true"), true);
        this.loadObjectsId = Dom4jUtils.getAttributeValue(loadObjectsEL, "loadObjectsId", "");
    }
    
    ////////////////////////////////////
    public TemplateDefination getTemplateDefination() {
        return templateDefination;
    }
    
    public void setTemplateDefination(TemplateDefination templateDefination) {
        this.templateDefination = templateDefination;
    }
    
    public String getloadObjectsId() {
        return loadObjectsId;
    }
    
    public void setloadObjectsId(String loadObjectsId) {
        this.loadObjectsId = loadObjectsId;
    }
    
    public Element getloadObjectsEL() {
        return loadObjectsEL;
    }
    
    public void setloadObjectsEL(Element loadObjectsEL) {
        this.loadObjectsEL = loadObjectsEL;
    }
    
    public int getCount() {
        return count;
    }
    
    public void setCount(int count) {
        this.count = count;
    }
    
    public String getIpvsdomain() {
        return ipvsdomain;
    }
    
    public void setIpvsdomain(String ipvsdomain) {
        this.ipvsdomain = ipvsdomain;
    }
    
    public int getPosition() {
        return position;
    }
    
    public void setPosition(int counter) {
        position = counter;
    }
    
    public boolean isEnable() {
        return enable;
    }
    
    public void setEnable(boolean enable) {
        this.enable = enable;
    }
    
    //////////////////////////////////////////////////////////////////////
    
    public boolean hasNext() {
        if (position >= count) return false;
        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public Element next() throws Exception {
        
        String templateId = Dom4jUtils.getAttributeValue(loadObjectsEL, "templateId", "");
        //if not enable skip
        if (!enable) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  is not enabled '" + templateId + "' ");
            return null;
        }
        //validate
        Template objecttemplate = templateDefination.gettemplate(templateId);
        if (objecttemplate == null) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  objecttemplate not found  '" + templateId + "' ");
            return null;
        }
        String parentNID = ipvsdomain + "." + objecttemplate.getListName();
         
        MRLog.debug("\nSTART############################# " + position + " processBulkLoadT " + templateId + " #############################");
        Document tobeSavedDoc = objecttemplate.getXml();
        if (tobeSavedDoc == null) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  tobeSavedDoc is not null '" + templateId + "' ");
        } else {
            BulkUploadUtils.updateValue2(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), objecttemplate.getUpdateXmlDefinations(), position);
            IpvsDBUtils.applyNID(tobeSavedDoc.getRootElement(), parentNID, position);
            BulkUploadUtils.updateValue2(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), objecttemplate.getUpdateXmlDefinations(), position);
        }
        position++;
        return tobeSavedDoc.getRootElement();
    }
    
}
