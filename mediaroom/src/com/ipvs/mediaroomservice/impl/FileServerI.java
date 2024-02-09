package com.ipvs.mediaroomservice.impl;

import org.dom4j.Document;

import com.ipvs.common.MRException;


public interface FileServerI {	
	 
	  public String createDestDir(Document fileDoc) throws MRException,Exception;
	  public String deleteDestDir(String fileNID) throws MRException,Exception;
	  public void updateDestFileInDB(String destNID,Document destFileDoc) throws MRException,Exception; 
	  public void updateDestFileonDisk(Document fileDoc,String uuid) throws MRException,Exception; 
	  public void updateDestFileStartTC(String fileNID,long currentTC) throws MRException,Exception; 
	  public void updateDestFileEndTC(String fileNID,long currentTC,long bytesWritten) throws MRException,Exception; 
	  public Document getDestFileDoc(String fileNID) throws MRException,Exception;
	  public String updateDestFileStatus(String fileNID,String status) throws Exception ;	  
}
