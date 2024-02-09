package com.ipvsserv.nxd.bulkupload.template;

import java.util.ArrayList;
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
public class UpdateElementDef extends UpdateElementXpathDef {
    private List<UpdateElementXpathDef> templateUpdateXpaths = new ArrayList<UpdateElementXpathDef>();
    
    public UpdateElementDef(Element el) {
        super(el);
        if (el != null) {
            xpath = Dom4jUtils.value(el, "").trim();
            if (!xpath.equals("")) templateUpdateXpaths.add(new UpdateElementXpathDef(xpath));
            List<?> list = Dom4jUtils.listXpath(el, "updateElementXpathDef");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el2 = (Element) iterator.next();
                templateUpdateXpaths.add(new UpdateElementXpathDef(el2));
            }
        }
    }
    
    public List<UpdateElementXpathDef> getTemplateUpdateXpaths() {
        return templateUpdateXpaths;
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<updateElementDef ");
        buf.append(toString_attrb());
        buf.append(">");
        buf.append(xpath);
        for (Iterator<UpdateElementXpathDef> iterator = templateUpdateXpaths.iterator(); iterator.hasNext();) {
            UpdateElementXpathDef updateValue = iterator.next();
            buf.append(updateValue.toString());
        }
        buf.append("\n</updateElementDef>");
        return buf.toString();
    }
    
}
