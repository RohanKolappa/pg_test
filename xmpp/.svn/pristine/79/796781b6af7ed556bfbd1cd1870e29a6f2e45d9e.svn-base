package com.ipvsserv.nxd.bulkupload.template;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class GroupTemplates {
    private Hashtable<String, GroupTemplate> groupTemplates = new Hashtable<String, GroupTemplate>();
    
    public GroupTemplates(Element el) {
        List<?> list = Dom4jUtils.listXpath(el, "groupTemplates/groupTemplate");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            GroupTemplate groupTemplate = new GroupTemplate(el2);
            groupTemplates.put(groupTemplate.getgroupTemplateId(), groupTemplate);
        }
    }
    
    public GroupTemplate getgroupTemplate(String groupTemplateId) {
        return groupTemplates.get(groupTemplateId);
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<groupTemplates>");
        for (GroupTemplate groupTemplate : groupTemplates.values()) {
            buf.append(groupTemplate.toString());
        }
        buf.append("\n</groupTemplates>");
        return buf.toString();
    }
}
