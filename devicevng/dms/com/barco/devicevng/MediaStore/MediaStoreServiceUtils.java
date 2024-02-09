package com.barco.devicevng.MediaStore;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class MediaStoreServiceUtils {
	public static boolean checkDiskStatus(Document cmdResultDoc) throws  Exception {
		String status = XMLUtils.getValueXpath(cmdResultDoc, "//Parameter[@name='Status']", "");

		if (Utils.getBooleanValue(status, false) || status.equals("Ok")) {
			return true;
		}

		return false;
	}

	public static Element getDiskEntry(String storePortElementID, Document nasStatus)
			throws  Exception {
		if (storePortElementID.isEmpty() || nasStatus==null) {
			return null;
		}

		Element statusEL = XMLUtils.getElementXpath(nasStatus, "//Parameters[Parameter[(@name='Name' and .='" + storePortElementID + "')]]");

		return statusEL;
	}

	public static Document getDiskStatusDoc(boolean isInternal, Logger log)
			throws Exception {
		if (isInternal) {
			return MediaStoreServiceUtils.getHardDiskStatus();
		} else {
			return MediaStoreServiceUtils.getNASDriveStatus();
		}
	}

	//TODO: Find a way to execute through device admin engine
	public static Document executeCLICommands(String [] cmdArr) {
		try {
			String cmdResult = CommandLineUtils.executeCLICommand(cmdArr);
			//System.out.println("CmdResult:="+cmdResult);
			int index = cmdResult.indexOf("<");
			if (index != -1) {
				cmdResult = cmdResult.substring(index).trim();
				return XMLUtils.stringToDocument(cmdResult);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
		
	}

	public static Document getHardDiskStatus() {
		String[] cmdArr = { MediaStoreServiceConst.GET_DISK_SPACE_CLI_CMD };
		return  executeCLICommands(cmdArr);
    }

	public static Document getNASDriveStatus() {
		String[] cmdArr = { MediaStoreServiceConst.GET_NAS_STATUS_CLI_CMD };
		return executeCLICommands(cmdArr);
	}
	
	    public static Document loadMediaSegmentInfoDocument() throws Exception {
	        Document fileDoc = null;
	        String xmlFilePath = "/com/ipvs/xsd/objectXml/MediaSegmentInfo.xml";
	        fileDoc = XMLUtils.loadXMLResource(xmlFilePath);

	        return fileDoc;
	    }
	    
	public static void writeIndexFile(String prepare_file_folder, String dirNID, String fileNID, String fileUUID, String streamType, String title, String requestTimeStamp)
			throws Exception {
		String content = "<index><record nid='" + fileNID + "' name='" + title + "' id='MediaFile'> " +
				"<Type>MediaClip</Type>" +
				"<Title>" + title +"</Title>" +
				"<ParentMediaDirNID>" + dirNID + "</ParentMediaDirNID>" +
				"<UUID>" + fileUUID + "</UUID>" +
				"<DateCreated>" + requestTimeStamp + "</DateCreated>" +
				"<State></State><Owner/>" +
				"</record></index>";
		File indexFilePath = new File(prepare_file_folder+"/db");
		indexFilePath.mkdirs();
		
		File indexFile = new File(prepare_file_folder+"/db", "_index");
		if (!indexFile.exists())
			indexFile.createNewFile();

		FileWriter fstream = new FileWriter(prepare_file_folder +"/db" + "/_index");
		BufferedWriter out = new BufferedWriter(fstream);
		out.write(content);
		out.close();
	}
	
	public static void writeMediaFile(String filePath, String fileName, String fileContent)
			throws Exception {
		File path = new File(filePath);
		path.mkdirs();
		
		File mFile = new File(filePath, fileName);
		if (!mFile.exists())
			mFile.createNewFile();

		FileWriter fstream = new FileWriter(filePath+"/"+fileName);
		BufferedWriter out = new BufferedWriter(fstream);
		out.write(fileContent);
		out.close();
	}

	/*
	 * ----------- SDP Utility functions start----------------------
	 */

	public static String generateSDPFromSDPTemplate(String sdptemplate,
			String destIP, int destUDPStartPort) throws Exception {
		int videoPort = destUDPStartPort;
		int audioPort = videoPort + 2;
		String sdp = sdptemplate;
		// Replace the values
		sdp = sdp.replace("DESTIP_TOKEN", destIP);
		sdp = sdp.replace("VIDEOPORT_TOKEN", String.valueOf(videoPort));
		sdp = sdp.replace("AUDIOPORT_TOKEN", String.valueOf(audioPort));
		return sdp;
	}

	private static String tokenize(String sdp, String regex, String replacement)
			throws Exception {
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if (!m.find())
			return sdp;
		StringBuffer sb = new StringBuffer();
		m.appendReplacement(sb, replacement);
		while (m.find()) {
			m.appendReplacement(sb, replacement);
		}
		m.appendTail(sb);
		return sb.toString();
	}

	public static String fixsdp(String s) {

		// Insert a new line before "a="
		Matcher m = Pattern.compile("(\\s+)(\\S=)").matcher(s);
		StringBuffer sb = new StringBuffer();
		while (m.find())
			m.appendReplacement(sb, "\n" + m.group(2));
		m.appendTail(sb);

		// Insert a new line before the new line
		s = sb.toString();
		m = Pattern.compile("(\n)").matcher(s);
		sb = new StringBuffer();
		while (m.find())
			m.appendReplacement(sb, "\n" + m.group(1));
		m.appendTail(sb);

		return sb.toString();
	}

	public static String fixSDPProblemWithOlderMedia(String sdp)
			throws Exception {
		// Added for older MPEG4 streams
		sdp = tokenize(sdp, "(c=IN\\s+IP4)\\s+(SERVERIP)", "c=IN IP4 "
				+ "DESTIP_TOKEN");
		sdp = tokenize(sdp, "(m=audio)\\s+(AUDIO_PORT)\\s+", "m=audio "
				+ "AUDIOPORT_TOKEN" + " ");
		sdp = tokenize(sdp, "(m=video)\\s+(VIDEO_PORT)\\s+", "m=video "
				+ "VIDEOPORT_TOKEN" + " ");

		// remove extra tab space
		if (sdp.endsWith("\t")) {
			sdp = sdp.substring(0, sdp.lastIndexOf("\t"));
		}

		sdp = fixsdp(sdp);

		return sdp;
	}

	public static void writeSDPFile(String sdpPath, String sdpContent)
			throws Exception {
		File sdpFile = new File(sdpPath + ".sdp");
		if (!sdpFile.exists())
			sdpFile.createNewFile();

		FileWriter fstream = new FileWriter(sdpPath + ".sdp");
		BufferedWriter out = new BufferedWriter(fstream);
		out.write(sdpContent);
		out.close();
	}

	/*
	 * ----------- SDP Utility functions end----------------------
	 */
}
