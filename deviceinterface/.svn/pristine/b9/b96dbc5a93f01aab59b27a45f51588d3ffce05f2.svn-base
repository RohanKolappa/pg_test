package com.barco.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;

public class FileUtils {

	public static final int FILENOTFOUND = 1005;

	public static String readFile(String filename) throws Exception {
		return FileUtils.readFileWithPath("", filename);
	}

	public static String readFileWithPath(String path, String filename) throws Exception {
		InputStream fis = new FileInputStream(new File(path + filename));
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


	public static boolean writeFile(String fname,  String buffer)throws Exception {	
		boolean result=true;
		BufferedWriter output = new BufferedWriter(new FileWriter(new File(fname)));
		output.write(buffer);
		output.close();
		return result;
	}


	//Write the given content to specified file
	public static void writeFileWithPath(String rootDir, String fileName, String content) throws  Exception {
		String filePath = rootDir + "/" + fileName;
		FileUtils.writeFile(filePath, content);
	}


	public static boolean isFileExist(String rootDir,String fileName) throws Exception {
		String filePath = rootDir +"/" +fileName; 
		File objFile = new File(filePath);        
		return objFile.exists();
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
	
	public static boolean deleteFileWithPath(String path, String filename) throws Exception {
		try {
			File file = new File(path + filename);
			if (file.isFile()){
				return file.delete();
			}
		}catch(Exception deleteException){
			
		}
		
		return false;
	}	
	
	public static boolean removeFileOrDir(String fileName) throws Exception {
		File fileObj = new File(fileName);
		if (!fileObj.exists())
			return false;
		return deleteDir(fileObj);
	}
	
	public static boolean deleteDir(File dir) {

		String[] listOfFiles = dir.list();
		if (listOfFiles != null && listOfFiles.length != 0) {
			for (int i = 0; i < listOfFiles.length; i++) {
				if (!deleteDir(new File(dir, listOfFiles[i])))
					return false;
			}
		}
		return dir.delete();
	}
	
	public static void createDirIfNotExist(String dirPath) {
		File dirFile = new File(dirPath);

		if (!dirFile.exists()) {
			dirFile.mkdirs();
		}
	}
	
	public static boolean isDirEmpty(String path) {
		try {
			File file = new File(path);
			System.out.println("Result of isDirEmpty "+(file.isDirectory() && file.list().length == 0 ? true:false)  +"   file.isDirectory() :="+ file.isDirectory() +" file.list().length :="+file.list().length  +" path:="+path);
			return file.isDirectory() && file.list().length == 0 ? true:false;
		}catch(Exception deleteException){
			
		}
		return false;
	}
}
