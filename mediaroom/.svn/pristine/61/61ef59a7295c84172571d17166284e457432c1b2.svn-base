package com.ipvs.test;

import java.io.File;

import com.ipvs.utils.XSLTUtils;

public class RequestHeaderChange {

	 static String [] filePathArray= {"/home/padmaja/work/model/mediaroomXML/com/ipvs/assetadminservice/xml/" ,"/home/padmaja/work/model/mediaroomXML/com/ipvs/assetadminservice/device/xml/"
			 ,"/home/padmaja/work/model/mediaroomXML/com/ipvs/assetadminservice/file/xml/","/home/padmaja/work/model/mediaroomXML/com/ipvs/assetadminservice/port/xml/",
			 "/home/padmaja/work/model/mediaroomXML/com/ipvs/mediaroomservice/xml/","/home/padmaja/work/model/mediaroomXML/com/ipvs/mediastoreservice/xml/",
			 "/home/padmaja/work/model/mediaroomXML/com/ipvs/policyadminservice/xml/","/home/padmaja/work/model/mediaroomXML/com/ipvs/systemservice/xml/"};
	 static String xslFilePath = "/home/padmaja/temp/RequestHeaderChange.xsl";
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		for(int i=0; i<filePathArray.length; i++) {
			File fileObject = new File(filePathArray[i]);
			if(fileObject.isFile()){
	    		continue;
	    	}
	    	File[] fileList = fileObject.listFiles();
	    	for(int j = 0; j < fileList.length; j++) {
	    		File childObj = fileList[j];
	    		if (childObj.isFile()){
	    			String childObjFilePath = childObj.getPath();
	    			try {
						XSLTUtils.executeXSLProc(xslFilePath, childObjFilePath, childObjFilePath);
					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
	    		} else if(childObj.isDirectory()) {
	    			continue;
	    		}
	    	}
		}
		
		
	}

}
