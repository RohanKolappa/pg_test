package com.ipvs.test;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class DeleteDestFiles {
	private Properties props = new Properties();
	private String propsFile = "DeleteDestFiles.properties";
	private ServiceTestUtil mServiceTestUtil = null;
	private static MRLog log = MRLog.getInstance(MRLog.CONSOLE);
	private static int sumLog = MRLog.ERROR;
	private  String recordingDirectory = "";
	public static final Random wheel = new Random();

	
	public DeleteDestFiles() {
		mServiceTestUtil = new ServiceTestUtil();
		props = new Properties();
		props = loadProperties(props, propsFile);
	}
	public static void main(String[] args) {
		try {
			addSystemOut(sumLog, MRLog.OK, " DeleteDestFiles ------------  start   ", "2");
			DeleteDestFiles mDeleteDestFiles = new DeleteDestFiles();
			if(args.length>1){
				String recordingDirectory = args[1];
				if (!recordingDirectory.equals("")) {
					mDeleteDestFiles.setRecordingDirectory(recordingDirectory);
				}
				String propsFile2 = args[0];
				if (!propsFile2.equals("")) {
					mDeleteDestFiles.setPropsFile(propsFile2);
					mDeleteDestFiles.loadProperties(propsFile2);
				}
				System.out.println(" called "+ propsFile2 +" recordingDirectory: "+recordingDirectory +"\n");	

			}else{
				System.out.println("usage \n");	
				System.out.println("java  com.ipvs.test.DeleteDestFiles DeleteDestFiles.properties recordingDirectory \n");	
				System.exit(1);
			}
			
			/*String propsFile2 = getArgument(args, "-file", "").trim();
			if (!propsFile2.equals("")) {
				mDeleteDestFiles.setPropsFile(propsFile2);
				mDeleteDestFiles.loadProperties(propsFile2);
			}
			String recordingDirectory = getArgument(args, "-dir", "").trim();
			if (!recordingDirectory.equals("")) {
				mDeleteDestFiles.setRecordingDirectory(recordingDirectory);
			}*/
			mDeleteDestFiles.doProcess();
		} catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			addSystemOut(sumLog, MRLog.OK, " DeleteDestFiles ------------  End   ", "2");
		}
		System.exit(0);
	}

	public void doProcess() {
		try {
			if (!login()) {
				return;
			}
			try {
				deleteFiles();
			} catch (Exception e) {
				e.printStackTrace();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}

	}

	// ////////////////////////////////////////////////////////////////////////
	public boolean login() {
		if (!mServiceTestUtil.doLogin_File(props.getProperty("loginXML", ""))) {
			addSystemOut(sumLog, MRLog.OK, "FAILED Login: " + mServiceTestUtil.getLoginResponse(), "2");
			return false;
		}
		if (!mServiceTestUtil.doSetServiceDomain_File(props.getProperty("setDomainXML", ""))) {
			addSystemOut(sumLog, MRLog.OK, "FAILED SetServiceDomain: " + mServiceTestUtil.getLoginResponse(), "2");
			return false;
		}
		return true;
	}
	public boolean deleteFiles() throws Exception{
		try {
			boolean waitforResult = Utils.booleanValue(props.getProperty("read.waitfor.result.synchronized", "true").trim(), true);
			
			String linuxScript = props.getProperty("db_read_files.sh", "db_read_files.sh").trim();
			String recordingDirectory = getRecordingDirectory();
 			executeLinuxScript(linuxScript,recordingDirectory); //recording.directory
			String NIDXpath = props.getProperty("handlerrequest.nid.xpath", "//DeleteRequest/NID").trim();
			
			String path = props.getProperty("db_delete_files.xml", "db_delete_files.xml").trim();
			String delFileRequest = props.getProperty("handler.request.xml", "/com/ipvs/assetadminservice/xml/MediaStreamFileResource_BASE_ALL_DELETE_NID.xml").trim();
			Document docRead = getXML(path);
			Document docRequest = getXML(delFileRequest);
			
			System.out.println("got "+MRXMLUtils.documentToString(docRead));
			List<?> list = MRXMLUtils.getListXpath(docRead.getRootElement(),"//exec");
			for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
				try {
					Element el = (Element) iterator.next();
					String NID = MRXMLUtils.getValue2(el, "").trim();
					if (!NID.equals("")) {
						long startTime = System.currentTimeMillis();
						Element clientdata = MRXMLUtils.getElementXpath(docRequest, "//header/clientdata");
						if (clientdata != null) {
							clientdata.setText(getNextClientData("DeleteFile"));
						}
						Element NIDEL = MRXMLUtils.getElementXpath(docRequest, NIDXpath);
						if (NIDEL != null) {
							NIDEL.setText(NID);
						}
						System.out.println( " calling delete "+NID);
						//System.out.println(NID + " calling "+MRXMLUtils.documentToString(docRequest));
						ServiceTestUtil.RespXML b = mServiceTestUtil.doSend(MRXMLUtils.documentToString(docRequest), " DeleteRequest ", waitforResult);
						System.out.println("done " + NID +" " + b.key + " " + (System.currentTimeMillis() - startTime));
					}
				} catch (Exception ex) {
					ex.printStackTrace();
				}

			}

		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return true;
	}
	public String executeLinuxScript(String scriptFile, String recordingDirectory) throws IOException, InterruptedException {
		BufferedReader stdout = null;
		Process myProcess = null;
		InputStream is = null;
 		String line = "";
		try {
			
			/*
			 db_files_delete.xquery.master=/data/DBDelete/db_files_delete.xquery.master
			 db_files_delete.xquery=/data/DBDelete/db_files_delete.xquery
			 db_files_delete.xquery.replace=<RecordingDir>
			 */
			
			String replaceStr = props.getProperty("db_files_delete.xquery.replace", "<RecordingDir>").trim();
			String masterFile = props.getProperty("db_files_delete.xquery.master", "/data/DBDelete/db_files_delete.xquery.master").trim();
			String xqueryFile = props.getProperty("db_files_delete.xquery", "/data/DBDelete/db_files_delete.xquery").trim();
			String masterFileText = readFromFile(masterFile);
			masterFileText = masterFileText.replace(replaceStr, recordingDirectory.trim() ) ;
			writeToFile(xqueryFile, masterFileText);
			//String cmd = scriptFile +" " + "\""+ recordingDirectory.trim() + "\"";
			String cmd = ""+scriptFile ;//+" " + ""+ recordingDirectory.trim() + "" + "";
			System.out.println("exec " + cmd);
			myProcess = Runtime.getRuntime().exec(cmd);
			is = myProcess.getInputStream();
			stdout = new BufferedReader(new InputStreamReader(is));
			line = stdout.readLine();
			while (line != null) {
				System.out.println(line);
				line = stdout.readLine();
			}
			int exitVal = myProcess.waitFor();
			System.out.println("executeLinuxScript exit value: " + exitVal);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (stdout != null)
					stdout.close();
			} catch (Exception e) {
			}
			try {
				if (is != null)
					is.close();
			} catch (Exception e) {
			}
			try {
				if (myProcess != null) {
					myProcess.destroy();
				}
			} catch (Exception e) {
			}
		}
		return line;
	}

 
    public void writeToFile(String filepath, String text) {
		BufferedWriter out = null;
		try {
			// Create file 
			FileWriter fstream = new FileWriter(filepath);
			out = new BufferedWriter(fstream);
			out.write(text);
			//Close the output stream
			out.close();
		} catch (Exception e) {//Catch exception if any
			System.err.println("Error: " + e.getMessage());
		 } finally {
			try {
				if (out != null)
					out.close();
			} catch (Exception e) {
			}
		}
	}
    public String readFromFile(String filepath) {
		StringBuffer text = new StringBuffer(100);
 		File file = new File(filepath);
		FileInputStream fis = null;
		BufferedReader br  = null;

		try {
			fis = new FileInputStream(file);
			br   = new BufferedReader(new InputStreamReader(fis));
			StringBuilder sb = new StringBuilder();
			String line = null;
			while ((line = br.readLine()) != null) {
				sb.append(line + "\n");
			}			
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (fis != null)
					fis.close();
			} catch (Exception e) {
			}
			try {
				if (br != null)
					br.close();
			} catch (Exception e) {
			}
			 
		}
		return text.toString();
	}
	 

  
	// ////////////////////////////////////////////////////////////////////////

	public void loadProperties(String propsFile) {
		props = loadProperties(props, propsFile);
	}
 	public void setPropsFile(String propsFile) {
		this.propsFile = propsFile;
	}
 	public String getRecordingDirectory() throws Exception{
		if(recordingDirectory==null || recordingDirectory.trim().equals("")){
			System.out.println("recordingDirectory is empty or null...... abort ");
			throw new Exception();
			//System.exit(1);
		}
		return recordingDirectory;
	}
	public void setRecordingDirectory(String recordingDirectory) {
		this.recordingDirectory = recordingDirectory;
	}

 	//////////////////////////////////////////////////////////////////////////
	public static String getArgument(String[] args, String argumentName, String defaultValue) {
		for (int i = 0; i < args.length; i++) {
			String args_i_1 = args[i];
			if (args_i_1.equalsIgnoreCase(argumentName)) {
				if (args.length > i + 1) {
					i = i + 1;
					String args_i_2 = args[i];
					if (!args_i_2.equals(""))
						return args_i_2;
				}
			}
		}
		return defaultValue;
	}

	private static Properties loadProperties(Properties props, String propFile) {
		InputStream in = null;
		try{
			File file = new File(propFile);
			if (!file.exists()) {
				in = DeleteDestFiles.class.getResourceAsStream(propFile);
			} else {
				in = new FileInputStream(new File(propFile));
			}
			props.load(in);

		} catch (FileNotFoundException ex) {
			ex.printStackTrace();
		} catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (Exception ex) {
				}
			}
			in = null;
		}
		return props;
	}

	public static void addSystemOut(int level, int code, String data, String systemOutAndLog) {
		try {
			log.addSystemOut(level, code, data, systemOutAndLog);
			//if( level <= MRLog.ERROR)
			//	logger.error(data);
			//logger2.log(Level.toLevel(level),  data);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	private String getNextClientData(String seed) {
		return seed + "_" + System.currentTimeMillis() + "_" + (Integer.toHexString(wheel.nextInt()));
	}

	private Document getXML(String xml) {
		// String xml="/src/com/ipvs/test/xml/LoginRequest.xml";
		try {
			File file = new File(xml);
			if (file.exists()) {
				return MRXMLUtils.loadXMLFile(xml, MRLog.getInstance(MRLog.CONSOLE));
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			return MRXMLUtils.loadXMLResource(xml, MRLog.getInstance(MRLog.CONSOLE));

		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}
}
