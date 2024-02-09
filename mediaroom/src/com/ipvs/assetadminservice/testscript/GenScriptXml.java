package com.ipvs.assetadminservice.testscript;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;

public class GenScriptXml implements GenScriptI {
	private Document doc = null;
	private Element root = null;
	private Element curTag= null;
	
	//private Element exceuteReqTag= null;
	private Element exceuteReqTag= null;
	
//	private Element variableList_clsmain = null;

	public GenScriptXml (){
		initDoc () ;
	}
 
	private void initDoc () {
		try {
			doc = MRXMLUtils.getDocument();
			root = MRXMLUtils.addElement(doc, "Script");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void save () throws Exception{
		MRXMLUtils.saveDocument(filePath, doc);
	}
	
	
	////////////////////////
 	public Document getDoc() {
		return doc;
	}
	public Element getRoot() {
		return root;
	}
	public Element getCurTag() {
		return curTag;
	}
	public Element getExceuteReqTag() {
		return exceuteReqTag;
	}
	public void setDoc(Document doc) {
		this.doc = doc;
	}
	public void setRoot(Element root) {
		this.root = root;
	}
	public void setCurTag(Element curTag) {
		this.curTag = curTag;
	}
	public void setExceuteReqTag(Element exceuteReqTag) {
		this.exceuteReqTag = exceuteReqTag;
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
