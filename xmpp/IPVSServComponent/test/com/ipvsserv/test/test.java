package com.ipvsserv.test;

import org.dom4j.Document;

import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;

public class test {
    public static void main (String[] args){
        
        try {
            String xmlFilePath="C:/Documents and Settings/pravin/Desktop/abc.xml";
            Document doc= Dom4jUtils.loadXMLFile(xmlFilePath);
            String query = Dom4jUtils.getValueXpath(doc.getRootElement(), "//Query","");
            query = CompUtils.decode(query);
            System.out.print(query);
        } catch (Exception ex) {
            
        }
        
    }
    
}
