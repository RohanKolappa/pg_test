package com.ipvsserv.nxd.bulkupload.template;

import org.dom4j.Element;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class GroupTemplateMember {
    private String groupTemplateMemberId = "";
    private String templateId = "";
    private boolean enable = true;
    
    public GroupTemplateMember(Element el) {
        groupTemplateMemberId = Dom4jUtils.getAttributeValue(el, "groupTemplateMember", "").trim();
        templateId = Dom4jUtils.getAttributeValue(el, "templateId", "").trim();
        enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(el, "enable", "true").trim(), true);
    }
    
    public String getgroupTemplateMemberId() {
        return groupTemplateMemberId;
    }
    
    public String gettemplateId() {
        return templateId;
    }
    
    public boolean isEnable() {
        return enable;
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<groupTemplateMember groupTemplateMemberId='" + groupTemplateMemberId + "' templateId='" + templateId + "'  enable='" + enable + "'/>");
        return buf.toString();
    }
}
