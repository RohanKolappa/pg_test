package com.ipvs.utils;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Vector;

import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class XSLTUtils {
	
	
	//temporary files for translation
	protected static final String SRC_FILE_PATH = "/tmp/src.xml";
	protected static final String DST_FILE_PATH = "/tmp/dst.xml";
	protected static final String XSL_FILE_PATH ="/tmp/translate.xsl";

	public static synchronized Element executeTranslateCommand(String xslFileName,Element srcEL,MRLog log) throws MRException,Exception {
		try {
			FileUtils.writeFile(SRC_FILE_PATH, srcEL.asXML());
			String xslFileString  = FileUtils.readFileResource(xslFileName, log.getClass());
			FileUtils.writeFile(XSL_FILE_PATH, xslFileString);
			executeXSLProc(XSL_FILE_PATH , SRC_FILE_PATH, DST_FILE_PATH);
			String translatedData = FileUtils.readFile(DST_FILE_PATH);
			return MRXMLUtils.stringToElement(translatedData);
		} catch(MRException exp){
			exp.printStackTrace();
			throw new MRException(exp.getCode(), exp.getData());
		} catch(Exception exp){
			exp.printStackTrace();
			throw new MRException(SessionServiceExceptionCodes.TRANSLATION_FAILED, "Translation Failed " + exp.getMessage());
		} finally {
            FileUtils.deleteFile(SRC_FILE_PATH);
            FileUtils.deleteFile(DST_FILE_PATH);
            FileUtils.deleteFile(XSL_FILE_PATH);
		}	
	}
    public static void executeXSLProc(String xslFilePath, String srcFilePath, String dstFilePath) throws Exception {
    	Vector<String> vec = new Vector<String>();
    	vec.add("xsltproc"); 
    	vec.add("-o"); 
    	vec.add(dstFilePath);
    	vec.add(xslFilePath);
    	vec.add(srcFilePath);    	
    	String[] cmdArgs = new String[vec.size()]; vec.copyInto(cmdArgs); 
     	Runtime runtime = Runtime.getRuntime();
    	Process proc = runtime.exec(cmdArgs);
    	

    	if (proc.waitFor() != 0) {
    		InputStream inputstream = proc.getErrorStream();
            InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
            BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

            StringBuffer sb = new StringBuffer();
         
            String line = null;

            while ((line = bufferedreader.readLine()) != null) {
                sb.append(line.trim());
                sb.append("\n");
            }
            
            System.out.println("result of xsl proc :"+sb.toString());

    		throw new MRException(SessionServiceExceptionCodes.EXECUTE_XSL_PROC_FAILED, "Execute XSL proc Failed  "+proc.exitValue());
    	} 
    }
    


}


