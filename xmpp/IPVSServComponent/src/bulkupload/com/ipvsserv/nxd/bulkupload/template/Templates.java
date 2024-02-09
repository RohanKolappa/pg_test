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
public class Templates {
    private Hashtable<String, Template> templates = new Hashtable<String, Template>();
    
    public Templates(Element el) {
        List<?> list = Dom4jUtils.listXpath(el, "templates/template");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            Template template = new Template(el2);
            templates.put(template.gettemplateId(), template);
        }
    }
    
    public Template gettemplate(String templateId) {
        return templates.get(templateId);
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<templates>");
        for (Template template : templates.values()) {
            buf.append(template.toString());
        }
        buf.append("\n</templates>");
        return buf.toString();
    }
}
