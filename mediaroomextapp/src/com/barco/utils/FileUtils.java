package com.barco.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;


public class FileUtils {
	
	public static String readFile(File file)throws Exception {
		//MRLog.getInstance(MRLog.DAEMON).addInfo("Read File Start ="+file.getName() ,MRLog.INFO,MRLog.NOTIFY);
		StringBuffer contents = new StringBuffer();
		BufferedReader input = new BufferedReader(new FileReader(file));
		String line = null;
		while( (line = input.readLine()) != null) {
			contents.append(line);
			contents.append("\n");
		}
		if(input!=null) {input.close();};
		//MRLog.getInstance(MRLog.DAEMON).addInfo("Read File End ="+file.getName() ,MRLog.INFO,MRLog.NOTIFY);
		
		return contents.toString();
	}
	
	public static boolean writeFile(String fname,  String buffer)throws Exception {
		//MRLog.getInstance(MRLog.DAEMON).addInfo("Write File Start ="+fname ,MRLog.INFO,MRLog.NOTIFY);
		
		boolean result=true;
		BufferedWriter output = new BufferedWriter(new FileWriter(new File(fname)));
		output.write(buffer);
		output.close();
		//MRLog.getInstance(MRLog.DAEMON).addInfo("Write File End ="+fname ,MRLog.INFO,MRLog.NOTIFY);
		
		return result;
	}
	
	public static boolean createFile(String fname) throws Exception
	{
		boolean result = true;		
		File file = new File(fname);
    	file.createNewFile();
    	
    	return result;
	}
	
	public static boolean deleteFile(String fname)throws Exception {
		boolean result=true;
		File df = new File(fname);
		result = df.delete();
		df=null;
		return result;
	}
	
	public static boolean deleteFile(String rootPath,String fname)throws Exception {
		String filePath = rootPath +"/" +fname;
		return deleteFile(filePath);
	}
	
	 public static boolean isFileExist(String rootDir,String fileName) throws Exception {
    	String filePath = rootDir +"/" +fileName; 
        File objFile = new File(filePath);        
        return objFile.exists();
	 }
	   
	public static String readFile(String propFile) throws Exception {
        try {
            return readFile(new FileInputStream(new File(propFile)));
        } catch (Exception ex) {
            throw new Exception("File not found or invalid." + propFile + " " + ex.getMessage());
        }
    }
    
    public static String readFileResource(String propFile, Class c) throws Exception {
        try {
            return readFile(c.getResourceAsStream(propFile));
        } catch (Exception ex) {
        	ex.printStackTrace();
            throw new Exception("File not found or invalid." + propFile + " " + ex.getMessage());
        }
    }
    
    public static String readFileURL(URL url) throws Exception {
        try {
            if (url == null) throw new Exception("File url is invalid.");
            return readFile(url.openStream());
        } catch (Exception ex) {
            throw new Exception("File not found or invalid." + url.getPath() + " " + ex.getMessage());
        }
    }
    
    private static String readFile(InputStream fis) throws Exception {
        BufferedReader reader=null;
        StringBuffer buf = new StringBuffer();
        try {
            String lineSeparator=System.getProperty("line.separator","\n");
            // Here BufferedInputStream is added for fast reading.
            reader = new BufferedReader(new InputStreamReader(fis));
            String line;
            while((line = reader.readLine()) != null) {
                buf.append(line );
                buf.append(lineSeparator);
            }            
            
        } catch (Exception ex) {
        	ex.printStackTrace();
            throw new Exception("readFile error ." + " " + ex.getMessage());
        } finally {
            // dispose all the resources after using them.
            try {
                fis.close();
            } catch (Exception e) {}
            try {
                reader.close();
            } catch (Exception e) {}
             
        }
        return buf.toString();
    }	 
    
    
    public static long getDirectorySize(File fileObject) throws Exception{
    	if(fileObject.isFile()){
    		fileObject.length();
    	}
    	File[] fileList = fileObject.listFiles();
    	long dirSize = 0;
    	for(int i = 0; i < fileList.length; i++) {
    		File childObj = fileList[i];
    		if (childObj.isFile()){
    			dirSize += childObj.length();
    		} else if(childObj.isDirectory()) {
    			dirSize += getDirectorySize(childObj);
    		}
    	}
    	return dirSize;
    }
    
    public static void createFileDir(String rootDirPath ,String dirName) throws Exception {
    	File rootDirFileObject  = new File(rootDirPath);
    	File  dirobject = new File(rootDirFileObject,dirName);
    	if(dirobject != null){
    		dirobject.mkdir();
    	}
    }
    
    public static void createDirIfNotExist(String dirPath){		
		File dirFile = new File(dirPath);

		if(!dirFile.exists()){
			dirFile.mkdirs();
		}
	}	
    
    //renaming  file is not working. TBD: Need to debug more
    public static boolean moveDir(String fromDir,String toDir) throws IOException,Exception {
    	File fromDirFile = new File(fromDir);
    	fromDirFile.setWritable(true);
    	if(!fromDirFile.exists() || !fromDirFile.isDirectory()){
    		System.out.println("File Not Exist ="+fromDir);
    		return false;
    	}
    
    	if(!fromDirFile.renameTo(new File(toDir))){
    		System.out.println("Renaming a file failed");
    		return false;
    	}
    	return true;
    }
   
    public static  boolean removeFileOrDir(String fileName) throws Exception {
    	File fileObj = new File(fileName);
    	if(!fileObj.exists())
    		 return false;
    	return deleteDir(fileObj);
	}
    
    public static boolean deleteDir(File dir) {
       
    	String [] listOfFiles = dir.list();
		if(listOfFiles != null && listOfFiles.length !=0 ) {
    		for(int i=0; i<listOfFiles.length;i++){
    			if(!deleteDir(new File(dir,listOfFiles[i])))
    				return false;
    		}
	    }
		return dir.delete();
    }
    
    public static synchronized void appendToFile(String fname, String buffer) throws Exception
    {
    	BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(new File(fname), true));
    	PrintWriter output = new PrintWriter(bufferedWriter);
		output.println(buffer);
		output.close();
    }
    
}
