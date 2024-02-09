package com.ipvsserv.nxd.bulkupload.template;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.dom4j.Element;

import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class UpdateXmlDefinations {
    private Hashtable<String, UpdateXmlDefination> updateXmlDefinations = new Hashtable<String, UpdateXmlDefination>();
    
    public UpdateXmlDefinations(Element el) {
        List<?> list = Dom4jUtils.listXpath(el, "updateXmlDefinations/updateXmlDefination");
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el2 = (Element) iterator.next();
            UpdateXmlDefination updateXmlDefination = new UpdateXmlDefination(el2);
            updateXmlDefinations.put(updateXmlDefination.getUpdateXmlDefinationId(), updateXmlDefination);
        }
    }
    
    public UpdateXmlDefination getUpdateXmlDefination(String updateXmlDefinationId) {
        return updateXmlDefinations.get(updateXmlDefinationId);
    }
    
    public Set<String> getUpdateXmlDefinationKeys() {
        return updateXmlDefinations.keySet();
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<UpdateXmlDefinations>");
        for (UpdateXmlDefination templateGroup : updateXmlDefinations.values()) {
            buf.append(templateGroup.toString());
        }
        buf.append("\n</UpdateXmlDefinations>");
        return buf.toString();
    }
}
