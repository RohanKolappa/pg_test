package com.ipvs.assetadminservice.testscript;


public interface GenScriptI {
	public String getFilePath();
	public void setFilePath(String filePath) ;
	public String getShortName() ;
	public void setShortName(String shortName);
	public String getScriptFileTagName() ;
	public void getScriptFileTagName(String scriptFileTagName);
	public void save () throws Exception ;
	public String getFileType();
	public void setFileType(String fileType) ;

	public void init( String scriptFileTagName, String filePath ,String shortName , String fileType);
 

}
