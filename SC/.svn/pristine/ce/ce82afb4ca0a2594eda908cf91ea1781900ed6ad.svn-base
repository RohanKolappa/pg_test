package com.barco.nimbus.utils.common;

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
}
