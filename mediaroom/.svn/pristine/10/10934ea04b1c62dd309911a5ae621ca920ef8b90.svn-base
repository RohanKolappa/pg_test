package com.ipvs.common.db;

public class ServiceRequestActionFilter {
    public static final char FILTER_NID = 'A';
    public static final char FILTER_ORDER_BY_XPATH = 'B';
    public static final char FILTER_WHERE_XPATH = 'C';
    public static final char FILTER_CURSOR = 'E';
    public static final char FILTER_SELECT_XML = 'F';    
    public static final char FILTER_OPERATION_DATA = 'G';
    public static final char FILTER_CHILD_XPATH_CONTEXT = 'H';    

    
    private StringBuffer buf = null;

    public ServiceRequestActionFilter() {
        reset();
    }
    
    public void reset() {
        buf = new StringBuffer();
    }
    
    public String getFilter() {
        return buf.toString();
    }
    
    public void setFilter(String filter) {
        buf = new StringBuffer();
        buf.append(filter);
    }
    
    public boolean isElemenEnabled(char c) {
        if (buf.indexOf(c + "") != -1) return true;
        return false;
    }
    
    public void enableElement(char c, boolean bEnable) {
        if (bEnable) { //enable
            if (buf.indexOf(c + "") == -1) buf.append(c);
        } else {
            if (buf.indexOf(c + "") != -1) { //remove
                String filter = buf.toString();
                filter = filter.replace(c, ' ');
                buf = new StringBuffer();
                buf.append(filter);
            }
        }
    }
    
    /// nid enable 
    public boolean isNIDEnabled() {
        return isElemenEnabled(FILTER_NID);
    }
    
    public void enableNID(boolean bEnable) {
        enableElement(FILTER_NID, bEnable);
    }
    
    /// Where XPath enable 
    public boolean isWhereXPathEnabled() {
        return isElemenEnabled(FILTER_WHERE_XPATH);
    }
    
    public void enableWhereXPath(boolean bEnable) {
        enableElement(FILTER_WHERE_XPATH, bEnable);
    }
    
    /// SortBy XPath enable 
    public boolean isSortByXPathEnabled() {
        return isElemenEnabled(FILTER_ORDER_BY_XPATH);
    }
    
    public void enableSortByXPath(boolean bEnable) {
        enableElement(FILTER_ORDER_BY_XPATH, bEnable);
    }
    
    //cursor enable
    public boolean isCursorEnabled() {
        return isElemenEnabled(FILTER_CURSOR);
    }
    
    public void enableCurso(boolean bEnable) {
        enableElement(FILTER_CURSOR, bEnable);
    }
    
    //Operation Data enable
    public boolean isOperationDataEnabled() {
        return isElemenEnabled(FILTER_OPERATION_DATA);
    }
    
    public void enableOperationData(boolean bEnable) {
        enableElement(FILTER_OPERATION_DATA, bEnable);
    }
    
    //Select XML enable
    public boolean isSelectXMLEnabled() {
        return isElemenEnabled(FILTER_SELECT_XML);
    }
    
    public void enableSelectXML(boolean bEnable) {
        enableElement(FILTER_SELECT_XML, bEnable);
    }
    
}
