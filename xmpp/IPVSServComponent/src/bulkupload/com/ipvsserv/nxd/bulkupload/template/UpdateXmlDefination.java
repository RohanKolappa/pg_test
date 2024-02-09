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
public class UpdateXmlDefination {
    private List<UpdateElementDef> updateElementDefs = new ArrayList<UpdateElementDef>();
    private String updateXmlDefinationId = "";
    
    public UpdateXmlDefination(Element el) {
        updateXmlDefinationId = Dom4jUtils.getAttributeValue(el, "updateXmlDefinationId", "").trim();
        List<?> list = Dom4jUtils.listXpath(el, "updateElementDef");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            updateElementDefs.add(new UpdateElementDef(el2));
        }
        
    }
    
    public String getUpdateXmlDefinationId() {
        return updateXmlDefinationId;
    }
    
    public List<UpdateElementDef> getUpdateElementDefValues() {
        return updateElementDefs;
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<UpdateXmlDefinations updateXmlDefinationId='" + updateXmlDefinationId + "'>");
        for (Iterator<UpdateElementDef> iterator = updateElementDefs.iterator(); iterator.hasNext();) {
            UpdateElementDef updateValue = iterator.next();
            buf.append(updateValue.toString());
        }
        buf.append("\n</UpdateXmlDefinations>");
        return buf.toString();
    }
}
