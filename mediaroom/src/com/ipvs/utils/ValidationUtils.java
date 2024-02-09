package com.ipvs.utils;

import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;


public class ValidationUtils {
	
	public static void containsSpecialCharacters(String value) throws Exception {		  		
		 Pattern pattern = Pattern.compile("[^a-zA-Z0-9:._-]");
		 Matcher matcher = pattern.matcher(value);		 
		 if (matcher.find()) {
			 throw new MRException(AMExceptionCodes.VALID_ERROR_SPECIAL_CHARACTERS_IN_NAME, "Object name doesn't allow special characters");
	   	 } 	   	 
	 }
	
	public static void containsSpecialCharacters(String value, String regex) throws Exception {		
		 Pattern pattern = Pattern.compile(regex);
		 Matcher matcher = pattern.matcher(value);		 
		 if (matcher.find()) {
			 throw new MRException(AMExceptionCodes.VALID_ERROR_SPECIAL_CHARACTERS_IN_NAME, "Object name doesn't allow special characters");
	   	 } 	   	 
	 }
	
	public static void containsSpecialCharactersInUserName(String value) throws Exception {		  		
		 Pattern pattern = Pattern.compile("[^a-zA-Z0-9._-]");
		 Matcher matcher = pattern.matcher(value);		 
		 if (matcher.find()) {
			 throw new MRException(AMExceptionCodes.VALID_ERROR_SPECIAL_CHARACTERS_IN_NAME, "Object name doesn't allow special characters");
	   	 } 	   	 
	 }
	
	public static void isCorrectFormatAndRange(String value) throws Exception {		  		
		Pattern pattern = Pattern.compile("[^0-9,]");
		Matcher matcher = pattern.matcher(value);		 
		if (matcher.find()) {
			throw new MRException(AMExceptionCodes.VALID_ERROR_SPECIAL_CHARACTERS_IN_EXPORT_SCHEDULE, "'Export Schedule' doesn't allow special characters");
	   	} 	   	 
		 
		StringTokenizer token = new StringTokenizer(value, ",");
		while (token.hasMoreElements()) {
			int number = Integer.parseInt(token.nextToken());
			if (number < 0 || number > 23) {
				throw new MRException(AMExceptionCodes.VALID_ERROR_EXPORT_SCHEDULE_RANGE, "'Export Schedule' is out of range. Please enter a value between 0 and 23");
			}
		}
		
	 }
	
	public static void isDuplicateName(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, String value) throws Exception {		  		
		
		String NID = "";
		String listName = "";
		String childXPath = "";
		String[] sortByXPath = null;
		String sortOrder = "";
		String selectXML = "";
		Element appendToParentElement = null;
		String[] arrWhereXPath = {"//Name='" + value + "'"};
		
		Element dbResponseEl = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXPath, arrWhereXPath, sortByXPath, 
				sortOrder, selectXML, appendToParentElement);
		if (dbResponseEl != null) {
			throw new MRException(AMExceptionCodes.VALID_ERROR_DUPLICATE, "Duplicate report name not allowed. Please provide a different name");
		} 	   	 
	 }
	
	public static void containsWhiteSpaces(String value, String dbObjectName, MRLog log) throws Exception {		  		
		if (value.indexOf(" ") != -1) {
            log.addInfo(" " + dbObjectName + ": Invalid Title ='" + value + "', Spaces are not allowed.", MRLog.ERROR, MRLog.ERROR);
            throw new MRException(AMExceptionCodes.VALID_ERROR_SPACE_IN_NAME, MRXMLUtils.generateErrorXML("value", value));
        }	   	 
	 }
	
	public static String escapeCurlyBraces(String value) throws Exception {
		String result = value;
		
		if(value.indexOf("{") >= 0)
			result = value.replace("{", "{{");
		
		if(value.indexOf("}") >= 0)
			result = result.replace("}", "}}");
		
		return result;
			
    }
	
	public static void validateCutListOffset(String value, long playlength) throws Exception {		  		
		 Pattern pattern = Pattern.compile("^(\\d{1,9}:\\d{1,9},)*$");
		 value= value.endsWith(",")?value:value+",";
		 Matcher matcher = pattern.matcher(value);	
		 if(matcher.matches()){
			 String[] inputarr= value.split(",");
	    	  int previousendOffset = 0;
	    	  for (int i = 0; i < inputarr.length; i++) {
				int startOffset =Integer.parseInt(inputarr[i].split(":")[0]);
				int endOffset =Integer.parseInt(inputarr[i].split(":")[1]);
				if( (startOffset !=0 && previousendOffset >= startOffset) || startOffset  >= endOffset)
				{
					throw new MRException(AMExceptionCodes.VALID_ERROR_OFFSET, "Offset combination is not valid.");
				}else if (startOffset > playlength || endOffset > playlength){
					throw new MRException(AMExceptionCodes.VALID_ERROR_OFFSET, "Offset cannot exceed the playlength of media.");
				}
				else
					previousendOffset = endOffset;
			}
			 
		 }
		 else
		   throw new MRException(AMExceptionCodes.VALID_ERROR_OFFSET, "Offsets Not Valid.");
	 }
}
