package com.ipvsserv.nxd.bulkupload.template;

import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class GroupCreateReference {
    private String fromXpath = "";
    private String fromXpath_templateMemberId = "";
    private String setXpath = "";
    private String setXpath_templateMemberId = "";
    
    public GroupCreateReference(Element el) {
        Element fromXpathEL = el.element("fromXpath");
        Element setXpathEL = el.element("setXpath");
        if (fromXpathEL != null) {
            fromXpath_templateMemberId = Dom4jUtils.getAttributeValue(fromXpathEL, "groupTemplateMemberId", "").trim();
            fromXpath = fromXpathEL.getTextTrim();
        }
        if (setXpathEL != null) {
            setXpath_templateMemberId = Dom4jUtils.getAttributeValue(setXpathEL, "groupTemplateMemberId", "").trim();
            setXpath = setXpathEL.getTextTrim();
        }
    }
    
    public String getFromXpath() {
        return fromXpath;
    }
    
    public String getFromXpath_templateMemberId() {
        return fromXpath_templateMemberId;
    }
    
    public String getSetXpath() {
        return setXpath;
    }
    
    public String getSetXpath_templateMemberId() {
        return setXpath_templateMemberId;
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<groupCreateReference>");
        buf.append("<fromXpath groupTemplateMemberId='" + fromXpath_templateMemberId + "'>" + fromXpath + "</fromXpath>");
        buf.append("<setXpath groupTemplateMemberId='" + setXpath_templateMemberId + ">" + setXpath + "</setXpath>");
        buf.append("\n</groupCreateReference>");
        return buf.toString();
    }
}
