package com.ipvs.test;

import java.io.File;

import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.FileUtils;

public class XSDTest {

  
	public static void main(String[] args) {
	    if(args.length == 0) {
	    	System.out.println("Usage: java com.ipvs.test.XSDTest <xmlfilename> <xsdfilename>");
	    	System.exit(1);
	    }

	    String xsdURL = args[1];

		try {
			String xml = FileUtils.readFile(new File(args[0]));
			MRXMLUtils.getDocument(xml, xsdURL);
			System.out.println("Document " + args[0] + " is OK");
		}
		catch(Exception e) {
			e.printStackTrace();
		    StackTraceElement[] st = e.getStackTrace();
		    for(int i = 0; i < st.length; i++) {
		      System.out.println(st[i].toString());
		      if(i > 5)
		          break;
		    }
		    System.out.println(e.toString());
		}
	}
}
