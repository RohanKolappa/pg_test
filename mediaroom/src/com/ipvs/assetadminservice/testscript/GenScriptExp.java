package com.ipvs.assetadminservice.testscript;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;

import com.ipvs.common.service.MRDB;

public class GenScriptExp implements GenScriptI {
	private String  templatePrefixFile = "";
 	private String  templateSuffixFile = "";
	//private String  templatePrefixLoad = "";
 	//private String  templateSuffixLoad = "";
	
	private BufferedWriter out =null;
 	private ArrayList<String> list_name_var = null;
 	private StringBuffer  script = new StringBuffer();
	public GenScriptExp(){
		list_name_var = new ArrayList<String> ();
	}
 
	public void setVariable(String line) {
		list_name_var.add(line);
	}
	public void writeScriptLine(String value) {
		script.append(value+"\n");
	}	
	public void save(){
		try {
			open();
			write_Prefix();
			write_variables();
			write(script.toString());
			write_Sufix();
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	private void write_variables(){
		try {
 			for (Iterator<String> i = list_name_var.iterator(); i.hasNext();) {
				String line = i.next();
				write(line+"\n");
 			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}	
	////////
	public String getTemplatePrefixFile() {
		return templatePrefixFile;
	}
	public String getTemplateSuffixFile() {
		return templateSuffixFile;
	}
	public void setTemplatePrefixFile(String templatePrefixFile, String templatePrefixLoad) {
		this.templatePrefixFile = templatePrefixFile;
		//this.templatePrefixLoad = templatePrefixLoad;
	}
	public void setTemplateSuffixFile(String templateSuffixFile, String templateSuffixLoad) {
		this.templateSuffixFile = templateSuffixFile;
		//this.templateSuffixLoad = templateSuffixLoad;
	}
	//////////////////////
	private void write_Prefix(){
		try {
			write ( getFileText(templatePrefixFile));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	private void write_Sufix(){
		try {
			write ( getFileText(templateSuffixFile));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}	
	private void open()  {
		FileWriter fstream=null;
		try {
			filePath = getResourceFilePath(filePath);
			fstream = new FileWriter(filePath);
			out = new BufferedWriter(fstream);
		} catch (Exception e) {
			e.printStackTrace();
		}
		 
	}

	private void write(String line){
         try {
        	 
			out.write(line);
			out.flush();
		} catch (IOException e) {
 			e.printStackTrace();
		}
         
	}
	public void close(){
		if(out!=null){
			try {
				out.flush();
				out.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		out=null;
	}	
	 
	
	///////////////
	public static String getResourceFilePath(String xmlFilePath ) {
		xmlFilePath=xmlFilePath.replace('\\', '/');
		if (!xmlFilePath.startsWith("/") )  
			 return xmlFilePath;
		try {
 			URL url = MRDB.class.getResource(xmlFilePath);
			return url.getFile();
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return null;
		 
	}	
	public static  String getFileText(String filePath){
		filePath = getResourceFilePath(filePath);
	    StringBuffer contents = new StringBuffer();
	    try {
	      BufferedReader input =  new BufferedReader(new FileReader(filePath));
	      try {
	        String line = null; //not declared within while loop
	        while (( line = input.readLine()) != null){
	          contents.append(line);
	          contents.append(System.getProperty("line.separator"));
	        }
	      }
	      finally {
	        input.close();
	      }
	    }
	    catch (IOException ex){
	      ex.printStackTrace();
	    }
	    return contents.toString();
	 }
	///////////////
	private String filePath = null;
	private String scriptFileTagName="";
	private String shortName="";
	//private String fileType="";
	public void init( String scriptFileTagName, String filePath ,String shortName , String fileType){
		this.filePath = filePath ;
		this.scriptFileTagName=scriptFileTagName;
		this.shortName=shortName;
		//this.fileType=fileType;
	}

	public String getScriptFileTagName() {
		return scriptFileTagName;
	}
	public void getScriptFileTagName(String scriptFileTagName) {
		this.scriptFileTagName=scriptFileTagName;
	}
	public String getShortName() {
		return shortName;
	}
	public void setShortName(String shortName) {
		this.shortName = shortName;
	}
	public String getFilePath() {
		return filePath;
	}
	public void setFilePath(String filePath) {
		this.filePath = filePath;
	}
	public String getFileType() {
		return filePath;
	}
	public void setFileType(String fileType) {
		//this.fileType = fileType;
	}
	 
}
 