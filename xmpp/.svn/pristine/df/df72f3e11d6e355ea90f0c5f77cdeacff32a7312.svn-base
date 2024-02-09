package com.ipvsserv.nxd.bulkupload.template;

import java.util.Hashtable;

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
public class BulkLoadGroupTemplateIterator {
    private TemplateDefination templateDefination = null;
    private Element loadObjectsEL = null;
    private int count = 0;
    private String loadObjectsId = "";
    private String ipvsdomain = "";
    private int position = 0;
    private boolean enable = false;
    
    public BulkLoadGroupTemplateIterator(TemplateDefination templateDefination, Element loadObjectsEL) {
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
    
    public void getPosition(int counter) {
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
    
    //////////////////////////////////////////////////////////////////////////////////
    public Hashtable<String, Element> next() throws Exception {
        position++;
        if (!hasNext()) {
            throw new Exception("reached end of list");
            
        }
        Hashtable<String, Element> tobeSavedMembers = new Hashtable<String, Element>();
        //<loadObjects type="objecttemplate" loadObjectsId='bulkload1.4' templateId='location1' count='5' enable='true'  />       
        boolean enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "enable", "true"), true);
        String loadObjectsId = Dom4jUtils.getAttributeValue(loadObjectsEL, "loadObjectsId", "");
        //String type = Dom4jUtils.getAttributeValue(loadObjectsEL, "type", "");
        int count = CompUtils.intValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "count", "0"), 0);
        String groupTemplateId = Dom4jUtils.getAttributeValue(loadObjectsEL, "groupTemplateId", "");
        
        //check enabled
        if (!enable) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  objecttemplate not enabled  '" + groupTemplateId + "' ");
            return tobeSavedMembers;
        }
        
        //validate groupTemplate defined 
        GroupTemplate groupTemplate = templateDefination.getgroupTemplate(groupTemplateId);
        if (groupTemplate == null) {
            throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  objecttemplate not found  '" + groupTemplateId + "' ");
        }
        
        //process
        MRLog.debug("\nSTART############################# " + position + " processBulkLoadGroupT " + groupTemplateId + " #############################");
        
        for (String groupTemplateMemberId : groupTemplate.getgroupTemplateKeys()) {
            MRLog.debug("groupTemplateMemberId  start " + groupTemplateMemberId);
            
            GroupTemplateMember objecttemplateMember = groupTemplate.getgroupTemplateMember(groupTemplateMemberId);
            Template template = templateDefination.gettemplate(objecttemplateMember.gettemplateId());
            if (template == null) {
                throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  objecttemplate not found  '" + objecttemplateMember.gettemplateId() + "' ");
            }
            String parentNID = ipvsdomain + "." + template.getListName();
            
            //prcess for each objecttemplate
            Document tobeSavedDoc = template.getXml();
            if (tobeSavedDoc == null) {
                throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  tobeSavedDoc is not null '" + objecttemplateMember.gettemplateId() + "' ");
            } else {
                BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), position);
                IpvsDBUtils.applyNID(tobeSavedDoc.getRootElement(), parentNID, position);
                BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), position);
                tobeSavedMembers.put(objecttemplateMember.getgroupTemplateMemberId(), tobeSavedDoc.getRootElement());
            }
            MRLog.debug("groupTemplateMemberId  end " + groupTemplateMemberId);
        }
        //update references
        BulkUploadUtils.updateValueGroup(tobeSavedMembers, groupTemplate);
        MRLog.debug("END############################# " + position + " processBulkLoadGroupT " + groupTemplateId + " #############################");
        return tobeSavedMembers;
    }
    
}
