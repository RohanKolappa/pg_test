package com.ipvs.test;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.XPath;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.service.MRXMLUtils;

public class TestXpath {

	public static void main (String[] args){
       try {
    	   int enableLogLevel = MRLog.ERROR;
    	   MRLog log =  MRLog.getInstance(MRLog.CONSOLE);
    	   MRConfig mrconfig = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, "V2D-Tx-Device", log);
    	   
    	   System.out.println("Total Memory "+Runtime.getRuntime().totalMemory());    
	       
    	  // log.addSystemOut(enableLogLevel, MRLog.OK, "1 Free Memory "+Runtime.getRuntime().freeMemory() ,"2");       
	       
    	   Hashtable<String, XPath>  list = new Hashtable<String, XPath> ()	;
	       //log.addSystemOut(enableLogLevel, MRLog.OK, "  createoing  objects "   , "2");
	       for ( int i =0;i<20;i++) {
		       //XPath  xpath_ObjectType = XPath.newInstance("//V2D-Tx-Device/DeviceConfig/SoftwareImageConfig/RootPassword");
		       XPath xpath_ObjectType  = MRXMLUtils.createXPath("//V2D-Tx-Device/DeviceConfig/SoftwareImageConfig/RootPassword");

		       list.put(i+"", xpath_ObjectType);
		       //log.addSystemOut(enableLogLevel, MRLog.OK, "  created object " + i  , "2");
	       }
	       Document doc = mrconfig.getXmlRepresentation(log).getDoc();
	       //log.addSystemOut(enableLogLevel, MRLog.OK, "2 Free Memory"+Runtime.getRuntime().freeMemory() ,"2");       
	       //log.addSystemOut(enableLogLevel, MRLog.OK, " \n\n\n START reading  object value from "   , "2");

	       for ( int i =0;i<20;i++){
 		       XPath  xpath_ObjectType = list.get(i+"");
		       String value =  xpath_ObjectType.valueOf(doc);
		       log.addSystemOut(enableLogLevel, MRLog.OK,i+ "  reading  object value from XPATH"   +i + " valueOf " + value + " "  ,"2");       
	       }	
	       
	       //log.addSystemOut(enableLogLevel, MRLog.OK, "4 Free Memory"+Runtime.getRuntime().freeMemory() ,"2");       
	       //log.addSystemOut(enableLogLevel, MRLog.OK, " \n\n\n START reading  object value from XPATH 1 - times "   , "2");
			    
	       XPath  xpath_ObjectType = list.get(1+"");
	       for ( int i =0;i<20;i++){
		       String value =  xpath_ObjectType.valueOf(doc);
		       log.addSystemOut(enableLogLevel, MRLog.OK,i+ "  reading  object value from 1 - times " + i + " valueOf " + value + " "  ,"2");       
	       }		       
	       
	       //log.addSystemOut(enableLogLevel, MRLog.OK, "5 Free Memory "+Runtime.getRuntime().freeMemory() ,"2");       
	       for ( int i =0;i<20;i++){
	    	   Thread.sleep(2000);
	    	   System.gc();
	       }
	       //log.addSystemOut(enableLogLevel, MRLog.OK, "6 Free Memory "+Runtime.getRuntime().freeMemory() ,"2");       

       }catch(Exception ex){
    	   
       }

	}
}
