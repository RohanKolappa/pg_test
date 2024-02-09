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
public class UpdateElementXpathDef {
    protected String type = "";
    protected int seed = 0;
    protected String prefix = "";
    protected String suffix = "";
    protected int startNumber = 0;
    protected String value = "";
    protected boolean keepOriginalText = true;
    protected String xpath = "";
    protected String fromXpath = "";
    
    public UpdateElementXpathDef(String xpath) {
        
        this.xpath = xpath;
    }
    
    public UpdateElementXpathDef(Element el) {
        
        type = Dom4jUtils.getAttributeValue(el, "type", "").trim();
        seed = CompUtils.intValue(Dom4jUtils.getAttributeValue(el, "seed", "0").trim(), 0);
        prefix = Dom4jUtils.getAttributeValue(el, "prefix", "").trim();
        fromXpath = Dom4jUtils.getAttributeValue(el, "fromXpath", "").trim();
        suffix = Dom4jUtils.getAttributeValue(el, "suffix", "").trim();
        keepOriginalText = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(el, "keepOriginalText", "false").trim(), false);
        startNumber = CompUtils.intValue(Dom4jUtils.getAttributeValue(el, "startNumber", "0").trim(), 0);
        value = Dom4jUtils.getAttributeValue(el, "value", "").trim();
        xpath = Dom4jUtils.value(el, "").trim();
    }
    
    public String getType() {
        return type;
    }
    
    public int getSeed() {
        return seed;
    }
    
    public String getPrefix() {
        return prefix;
    }
    
    public String getSuffix() {
        return suffix;
    }
    
    public int getStartNumber() {
        return startNumber;
    }
    
    public String getValue() {
        return value;
    }
    
    public boolean isKeepOriginalText() {
        return keepOriginalText;
    }
    
    public String getXpath() {
        return xpath;
    }
    
    public String getFromXpath() {
        return fromXpath;
    }
    
    protected String toString_attrb() {
        StringBuffer buf = new StringBuffer();
        if (type != null && !type.trim().equals("")) {
            buf.append(" type='" + type + "'");
        }
        if (seed > 0){
            buf.append(" seed='" + seed + "'");
        }
        if (startNumber > 0){
            buf.append(" startNumber='" + startNumber + "'");
        }
        if (prefix != null && !prefix.trim().equals("")) {
            buf.append(" prefix='" + prefix + "'");
        }
        if (suffix != null && !suffix.trim().equals("")){
            buf.append(" suffix='" + suffix + "'");
        }
        if (value != null && !value.trim().equals("")){
            buf.append(" value='" + value + "'");
        }
        if (keepOriginalText) {
            buf.append(" keepOriginalText='" + keepOriginalText + "'");
        }
        if (fromXpath != null && !fromXpath.trim().equals("")) {
            buf.append(" fromXpath='" + fromXpath + "'");
        }
        
        return buf.toString();
    }
    
    public String toString() {
        StringBuffer buf = new StringBuffer();
        buf.append("\n<templateUpdateXpath ");
        buf.append(toString_attrb());
        buf.append(">");
        buf.append(xpath);
        buf.append("\n</templateUpdateXpath>");
        return buf.toString();
    }
}
