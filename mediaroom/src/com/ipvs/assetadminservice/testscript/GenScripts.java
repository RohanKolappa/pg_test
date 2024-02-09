package com.ipvs.assetadminservice.testscript;

public class GenScripts {
}
//public class GenScripts extends AMTestXmlUtil{
//    
//	private String genScriptXmlPath = "/com/ipvs/assetadminservice/testscript/GenScripts.xml";
//	private final static String TESTSCRIPT_ATTRBNAME="name";
//	private Document textScriptdoc=null;
//
//	
//	
//	///////////////////////loadScriptSettings////////////////////
//	//private static int counter = 1;
//	private int counter = 1;
//	// variableslist/setReadParam
// 	private Hashtable<String, String> list_xpath_name = null;
// 	private Hashtable<String, Element> list_xpath_el = null;
//	// script files and xml files
//	private Hashtable<String, String> fileList_name_var= null;
// 	private Hashtable<String, String> fileList_name_Path= null;
// 	private Hashtable<String, GenScriptI> fileList_name_file= null; 
//  
// 	
// 	private String  templatePrefix_file = "";
// 	private String  templateSuffix_file = "";
// 	private String  templatePrefix_load = "";
// 	private String  templateSuffix_load = "";
// 	
//	///////////////////////loadScriptSettings////////////////////
// 	
// 	
// 	
//	///////////////////////processTag_object////////////////////
// 	private Element objectEL = null;
// 	//private Element testScriptEL = null;
// 	private String settings = "";
// 	private String script_settings =""; 
// 	private String shortName =""; 
//   	//private String template =""; 
// 	//private String fileSuffix =""; 
// 	private String dbObjectName ="";
// 	private MRConfig mrConfig = null;
// 	//private Document objectXmldoc = null;;
//	///////////////////////processTag_object////////////////////
//
// 	protected void loadScriptSettings( ){
//		try {
//			counter = 1;
//			templatePrefix_file = "";
//			templateSuffix_file = "";
//			templatePrefix_load = "";
//			templateSuffix_load = "";
//			
//			list_xpath_name = new Hashtable<String, String> ();
//			list_xpath_el = new Hashtable<String, Element> ();
//			// script files and xml files
//			fileList_name_Path= new Hashtable<String, String> ();
//			fileList_name_var= new Hashtable<String, String> ();
//			fileList_name_file = new Hashtable<String, GenScriptI>(); 
//			//fileList_name_xmlScript= new Hashtable<String, GenScriptXml> (); 
//			//fileList_name_exp= new Hashtable<String, GenScriptExp> ();
//	 		
//			/*
//			  <script_settings name="default">
//				  <templatePrefix loadFrom="JAR" file="/com/ipvs/assetadminservice/testscript/AmExpScript_prefix.txt"/>
//				  <templateSuffix loadFrom="JAR" file="/com/ipvs/assetadminservice/testscript/AmExpScript_suffix.txt"/>
//				  <AMTestScriptXML loadFrom="JAR" file="/com/ipvs/test/xml/AMTestScript</TestScriptXML"/>
//			  </script_settings > 			  
//			*/
//			
//			settings = MRXMLUtils.getAttributeValue(objectEL, "settings", "").trim();
// 			script_settings = MRXMLUtils.getAttributeValue(objectEL, "script_settings", "").trim();
// 			
// 			String xpath = "//script_settings[@name='"+script_settings+"']/";
// 			Element templatePrefixEL = MRXMLUtils.getElementXpath(textScriptdoc ,xpath+"/templatePrefix");
//			Element templateSuffixEL = MRXMLUtils.getElementXpath(textScriptdoc.getRootElement(),  xpath+"/templateSuffix");
//			Element AMTestScriptXMLEL = MRXMLUtils.getElementXpath(textScriptdoc.getRootElement(),  xpath+"/AMTestScriptXML");
// 			
// 			templatePrefix_file = MRXMLUtils.getAttributeValue(templatePrefixEL, "file", "");
// 			templateSuffix_file = MRXMLUtils.getAttributeValue(templatePrefixEL, "file", "");
//			templatePrefix_load = MRXMLUtils.getAttributeValue(templateSuffixEL, "loadFrom", "");
// 			templateSuffix_load = MRXMLUtils.getAttributeValue(templateSuffixEL, "loadFrom", "");
// 
// 			String  AMTestScriptXML = MRXMLUtils.getAttributeValue(AMTestScriptXMLEL, "file", "");
//   			this.testXmlPath = AMTestScriptXML;
// 			
// 			loadXml(testXmlPath);
// 			loadSettings(settings);
// 	  
// 
//		}catch(Exception ex){
//			ex.printStackTrace();
//		}
//	} 	
// 	private String[] getArr(String testScript){
// 		if(testScript==null || testScript.trim().equals(""))
// 			return null;
// 		String[] arrTestScriptAttrb = {testScript};
// 		return arrTestScriptAttrb;
// 	}
// 	public void processTag_object(String dbObjectName,  String testScript ) throws Exception{
// 		try {
// 			
// 			//objectEL
//   			String testScriptAttrbName = TESTSCRIPT_ATTRBNAME;
//	 		String[] arrTestScriptAttrb = getArr(testScript);
//	 		
//	 		String xPath ="";
//	 		xPath = xpath_object( dbObjectName);
//	 	 	objectEL = MRXMLUtils.getElementXpath(textScriptdoc, xPath);
//	 		
//	 		xpath_ScriptFile( dbObjectName, testScriptAttrbName, arrTestScriptAttrb);
//	 		//testScriptEL = MRXMLUtils.getElementXpath(textScriptdoc, xPath);
//
//	 	 	
//	 	 	//parse objectEL attribute
// 			this.dbObjectName = MRXMLUtils.getAttributeValue(objectEL, "name", "").trim();
// 			shortName = MRXMLUtils.getAttributeValue(objectEL, "shortName", "").trim();
// 			settings = MRXMLUtils.getAttributeValue(objectEL, "settings", "").trim();
// 			script_settings = MRXMLUtils.getAttributeValue(objectEL, "script_settings", "").trim();
// 			//template = MRXMLUtils.getAttributeValue(objectEL, "template", "").trim();
//			//fileSuffix = MRXMLUtils.getAttributeValue(objectEL, "fileSuffix", "").trim();
//			
//			loadScriptSettings();//call
//
//			//object xml 
//			mrConfig = MRConfig.getInstance(serviceName, dbObjectName, log);
//			//objectXmldoc = mrConfig.getXmlRepresentation(log).getDoc();
//
//			//parse_ObjectXml(objectXmldoc.getRootElement(), shortName, ""  ) ;//call
//
//			//<scriptFile name="scriptFile1" fileSuffix="_DefVars.xml" subfolder="" varName="AR.filepath_vars" />
//			parseTag_ScriptFile();//call
//
//	 	 	processCommon();//call
//	 	 	
//	 	 	
//	 		
// 		} catch(Exception ex) {
// 			ex.printStackTrace();
// 		}
// 	}
// 
//
// 	private void processCommon(){
// 		try {
// 			
// 			String parentXPath = "";
//			/*
// 			<variableList name="variableList1" scriptFile="scriptFile3" />
//			<readParamValues name="readParamValues1" scriptFile="scriptFile1" />
//			<setParamValues name="setParamValues1" loadFromFile="0" scriptFile="scriptFile1" />
//			*/
// 			String scriptFile_variableList = MRXMLUtils.getAttributeValueXpath(objectEL , "xmlScript_settings/variableList[@name='all']/@scriptFile","");
// 			String scriptFile_readParamValues = MRXMLUtils.getAttributeValueXpath(objectEL , "xmlScript_settings/readParamValues[@name='all']/@scriptFile","");
// 			String scriptFile_setParamValues = MRXMLUtils.getAttributeValueXpath(objectEL , "xmlScript_settings/setParamValues[@name='all']/@scriptFile","");
// 			
// 			
// 			
// 			create_commonTags( scriptFile_variableList, scriptFile_readParamValues ,scriptFile_setParamValues ,
// 					mrConfig.getXmlRepresentation(log).getDoc().getRootElement(), shortName, parentXPath);
// 			saveDocs();
//			
//	 		
// 		} catch(Exception ex) {
// 			ex.printStackTrace();
// 		}
// 	}
// 	
// 	 private void saveDocs(){
// 		  try {
//	 		   for (Enumeration<String> e = fileList_name_file.keys() ; e.hasMoreElements() ;) {
//		 			  String key = (String)e.nextElement();
//		 			  GenScriptI genScript =  fileList_name_file.get(key);
//		 			  genScript.save();
//		 	   }
//	 		   
//			} catch (Exception e1) {
//				// TODO Auto-generated catch block
//				e1.printStackTrace();
//			}
// 	 }
//	 private String nextVar(String var){
//	 	String ret=  shortName+""+counter+"."+var;
//	 	counter++;
//	 	return ret;
//	 }
// 	 private String tclVar(String var){
// 		return "${"+var+"}";
// 	 }
// 	 private String setPXpath(String var){
//  		return  "//OperationData"+var;
//  	 }
//	public void create_commonTags(String scriptFile_variableList,String scriptFile_readParamValues,
//			String scriptFile_setParamValues , Element fromEL, String shortName, String parentXPath ) throws Exception {
// 		try {
// 
// 			GenScriptXml scriptXml_readParam = (GenScriptXml)file_Script(scriptFile_readParamValues,"xml");
// 			//GenScriptXml scriptXml_setParam = (GenScriptXml)file_Script(scriptFile_setParamValues,"xml");
// 			GenScriptXml scriptXml_var = (GenScriptXml)file_Script(scriptFile_variableList,"xml");
// 			
// 			//String varName_vars = fileList_name_var.get(scriptFile_variableList);
// 			boolean parse = Utils.booleanValue(MRXMLUtils.getAttributeValue(fromEL, "Parse", "false"), false);
// 			 if(!parse )
// 				return;
// 			
// 			String nameUnique = nextVar( fromEL.getName());
// 			if(parentXPath ==null || parentXPath.trim().equals(""))
// 				parentXPath =  "/" ;
// 			String xPath = parentXPath +"/" + fromEL.getName();
//
// 			String NIDScheme = MRXMLUtils.getAttributeValue(fromEL, IpvsDBUtils.NID_SCHEME, "").trim();
// 			if(NIDScheme.equals(IpvsDBUtils.NID_SCHEME_AUTO)) {
// 				nameUnique = nameUnique +".AUTO";
// 			}
// 			
// 			boolean bHasFreeChildsAttrb = false; //XmlRepresentation.hasFree_Childs_Attrb(fromEL,log);
// 			//TODO: VERIABLELIST add func.shortName for read 
// 			//TODO: snVarNode rootRequest
// 			Element varlistEL = ScriptNode.variableList(nameUnique, xPath);
// 			Element setParamEL = ScriptNode.setParamValues(nameUnique, xPath);
// 			//TODO: ScriptNode.updateNode_filePath(setParamEL, "${"+varName_vars+"}"); //TODO:check if same file
// 			ScriptNode.updateNode_filePath(setParamEL, scriptXml_var.getFilePath()); //TODO:check if same file
// 			//TODO: Element readParamEL = ScriptNode.variableList(nameUnique, xPath);
// 			List<?> freeChilds = XmlRepresentation.getChildEL_PARSE(false , fromEL, log);
// 			for (Iterator<?> i = freeChilds.iterator(); i.hasNext();) {
// 				Element childFromEL = (Element) i.next();
// 				String childxPath = xPath+"/" +fromEL.getName();
// 				String nameUniqueChild = nextVar( childFromEL.getName());
//
// 				//add variable
// 				Element varEL = ScriptNode.var(nameUniqueChild, setPXpath(childxPath) );
// 				MRXMLUtils.addElement(varlistEL, varEL); 
// 				//add set paramvalues
// 				Element varEL2 = ScriptNode.setParamValue( nameUniqueChild, tclVar(nameUniqueChild));
// 				Element setParamELChild =  MRXMLUtils.addElement(setParamEL, varEL2); 
// 				ScriptNode.updateNode_eval(setParamELChild, "isNotNull", tclVar(nameUniqueChild) );
// 				ScriptNode.updateNode_use_varNode(setParamELChild, "rootRequest");
//
// 				bHasFreeChildsAttrb=true;
// 	 		}
// 			if(!bHasFreeChildsAttrb){
// 	  			List<?> attrList = MRXMLUtils.getAttributeList(fromEL);
// 	  			//Vector<String> vec = new Vector<String>();
// 	  			for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
// 	  				Attribute attrEL = (Attribute) i.next();
// 	  				if(!XmlRepresentation.isSchemaAttrib(attrEL.getName())){
// 	  					
// 	  					String childxPath = xPath+"/@" +attrEL.getName();
// 	  	 				String nameUniqueChild = nextVar( attrEL.getName());
//
// 	  					bHasFreeChildsAttrb=true;
// 	  	 				//add variable
// 	  	 				Element varEL = ScriptNode.var(nameUniqueChild, setPXpath(childxPath) );
// 	  	 				MRXMLUtils.addElement(varlistEL, varEL); 
// 	  	 				//add set paramvalues
// 	  	 				Element varEL2 = ScriptNode.setParamValue( nameUniqueChild, tclVar(nameUniqueChild));
// 	  	 				Element setParamELChild =  MRXMLUtils.addElement(setParamEL, varEL2); 
// 	  	 				ScriptNode.updateNode_eval(setParamELChild, "isNotNull", tclVar(nameUniqueChild) );
// 	  	 				ScriptNode.updateNode_use_varNode(setParamELChild, "rootRequest");
// 	  				}
// 	  			}
// 			}
// 			
//  			if(bHasFreeChildsAttrb){
//  				list_xpath_name.put(xPath, nameUnique);
//  				list_xpath_el.put(xPath,  fromEL );
//  				MRXMLUtils.addElement(scriptXml_var.getRoot(), varlistEL);  
//  				MRXMLUtils.addElement(scriptXml_readParam.getRoot(), setParamEL);  
//  				counter++;
//  			}	
//  			
// 			List<?> parseChilds = XmlRepresentation.getChildEL_PARSE(true , fromEL, log);
//  			for (Iterator<?> i = parseChilds.iterator(); i.hasNext();) {
//  				Element childFromEL = (Element) i.next();
//  				create_commonTags( scriptFile_variableList, scriptFile_readParamValues ,scriptFile_setParamValues, childFromEL , shortName, xPath );
//  	 		}
//  			
// 		}catch(Exception ex ){
//  			ex.printStackTrace();
//  		}
// 	} 	 	
//// 	 
////	private void parseTag_variableList () throws Exception {
//// 		//<variableList name="all" varNodeName="AR.filepath_vars" scriptFile="scriptFile1" />
//// 		List<?> childList = MRXMLUtils.getListXpath(objectEL , "xmlScript_settings/variableList");
//// 		for (Iterator<?> i = childList.iterator(); i.hasNext();) {
////			Element fromChildEL = (Element) i.next();
////			String name = MRXMLUtils.getAttributeValue(fromChildEL, "name", "").trim();
////			String scriptFile = MRXMLUtils.getAttributeValue(fromChildEL, "scriptFile", "").trim().replace('\\', '/');
////			if(!fileList_name_Path.containsKey(scriptFile)){
////				System.out.println("scriptFile not defined " + scriptFile );
////				return ;
////			}
////			GenScriptXml scriptXml = file_xmlScript(scriptFile);
////			if(scriptXml ==null) {
////				continue;
////			}
////			
////			Element varlistEL = MRXMLUtils.addElement(scriptXml.getRoot(), ScriptNode.variableList(name));
////			scriptXml.setCurTag(varlistEL);
////			/*
////				<variableList name="all" varNodeName="AR.filepath_vars" scriptFile="scriptFile1" />
////				<variableList name="variableList2" scriptFile="scriptFile1">
////					<variableListXPath   bCopyFreeChilds="false" iCopyChildsToDepth="1"> 
////						//AssetResource/Info/Groups
////					</variableListXPath>
////				</variableList>
////			 */
////	 		List<?> xpathList = MRXMLUtils.getListXpath(objectEL , "common_settings/variableListXPath");
////	 		Document retDoc =null;
////	 		for (Iterator<?> k = xpathList .iterator(); k.hasNext();) {
////	 			Element xpathChildEL = (Element) k.next();
////	 			String xpath = MRXMLUtils.getValue2(xpathChildEL, "").trim();
////				boolean bCopyFreeChilds = AMStringUtil.booleanValue(MRXMLUtils.getAttributeValue(xpathChildEL, "bCopyFreeChilds", "false"),false);
////				int iCopyChildsToDepth =  AMStringUtil.intValue(MRXMLUtils.getAttributeValue(xpathChildEL, "iCopyChildsToDepth", "1"),1);
////				retDoc = mrConfig.getXmlRepresentation(log).getElementFromXpath(bCopyFreeChilds, iCopyChildsToDepth, retDoc, xpath, null, null, log, null, null,null);
////	 		}
////	 		if(retDoc == null){
////	 			retDoc = mrConfig.getXmlRepresentation(log).getDoc();
////	 		}
////			
////			
////			
////			
////		}
//// 	}
// 
//	private void parseTag_ScriptFile ( ) throws Exception {
// 		//String[] arr = new String[0];
// 		List<?> childList = MRXMLUtils.getListXpath(objectEL , "common_settings/scriptFile");
// 		for (Iterator<?> i = childList.iterator(); i.hasNext();) {
//			Element fromChildEL = (Element) i.next();
//			String name = MRXMLUtils.getAttributeValue(fromChildEL, "name", "").trim();
//			String type = MRXMLUtils.getAttributeValue(fromChildEL, "type", "").trim();
//			String fileSuffix2 = MRXMLUtils.getAttributeValue(fromChildEL, "fileSuffix", "").trim().replace('\\', '/');
//			String subfolder = MRXMLUtils.getAttributeValue(fromChildEL, "subfolder", "").trim();
//			String varNameSuffix = MRXMLUtils.getAttributeValue(fromChildEL, "varNameSuffix", "").trim();
//			String filePath = "";
//			StringBuffer filePathBuf = new StringBuffer(testScriptXmlFolder);
//			if(!subfolder.endsWith("")) {
//				if(subfolder.startsWith("/"))
//					filePathBuf.append("/");
//				filePathBuf.append(subfolder);
//			}
//			filePath = ensureFolder( getFullPath(filePathBuf.toString()) ) + "/"+dbObjectName +"" + fileSuffix2 +"." + type; 
//			fileList_name_Path.put(name, filePath);
//			fileList_name_var.put(name, shortName+"."+varNameSuffix);
//			
//			if(type.trim().equals("exp")){
//				GenScriptExp script = new GenScriptExp();
//				script.init(name, filePath, shortName, type);
//				script.setTemplatePrefixFile(templatePrefix_file, templatePrefix_load);
//				script.setTemplateSuffixFile(templateSuffix_file, templateSuffix_load);
//				fileList_name_file.put(name, (GenScriptI) script);
//			} 
//			else {
//				GenScriptXml script = new GenScriptXml();
//				script.init(name, filePath, shortName, type);
//				fileList_name_file.put(name, (GenScriptI) script);
//			}
//			
//		}
// 	}
//	
// 	private String[] parseTag_ObjectList ( String xPath ) throws Exception {
// 		String[] arr = new String[0];
// 		List<?> childList = MRXMLUtils.getListXpath(textScriptdoc.getRootElement() , xPath);
//		Vector<String> vec = new Vector<String>();
// 		for (Iterator<?> i = childList.iterator(); i.hasNext();) {
//			Element fromChildEL = (Element) i.next();
//			String objectName = MRXMLUtils.getAttributeValue(fromChildEL, "name", "").trim();
//			if(!objectName.equals(""))
//			 vec.add(objectName);
//		}
// 		if(vec.size() > 0 ){
// 			arr = new String[vec.size()];
// 			vec.copyInto(arr);
// 		}
//		return arr;	
// 	}
//
//	private static String xpath_object( String dbObjectName){
//		StringBuilder buf = new StringBuilder();
//		buf.append("//object");
//		if(dbObjectName != null && !dbObjectName.trim().equals(""))
//			buf.append("[@name='"+dbObjectName+"']");  
//	 
// 		return buf.toString();
//   	}	
// 	
// 	private static String xpath_ScriptFile( String dbObjectName, String testScriptAttrbName, String[] arrTestScriptAttrb){
//		StringBuilder buf = new StringBuilder();
//		buf.append("//object");
//		if(dbObjectName != null && !dbObjectName.trim().equals(""))
//			buf.append("[@name='"+dbObjectName+"']");  
//		buf.append("/testScript");
//		if(arrTestScriptAttrb != null && arrTestScriptAttrb.length != 0){
//			buf.append("[");
//			boolean bAddOr=false;
//			for(int i=0;i<arrTestScriptAttrb.length;i++){
//				if(!bAddOr){
//					bAddOr=true;
//				} else {
//					buf.append(" or ");	
//				}
//				buf.append("@"+testScriptAttrbName + "='" + arrTestScriptAttrb[i]+"'");					
//			}
//			buf.append("]");
//		}  
// 		return buf.toString();
//   	}	
// 	
//	private GenScriptI file_Script (String name, String type) throws Exception {
//		if(!fileList_name_Path.containsKey(name)){
//			System.out.println("scriptFile not defined " + name );
//			return null;
//		}
//		String filePath = fileList_name_Path.get(name);
//		//GenScriptI doc = null;
//		if(fileList_name_file.containsKey(name)){
//			return fileList_name_file.get(name);
//		} else {
//			if(type.trim().equals("exp")){
//				GenScriptExp script = new GenScriptExp();
//				script.init(name, filePath, shortName, type);
//				script.setTemplatePrefixFile(templatePrefix_file, templatePrefix_load);
//				script.setTemplateSuffixFile(templateSuffix_file, templateSuffix_load);
//				fileList_name_file.put(name, (GenScriptI) script);
//				return script;
//			} 
//			else {
//				GenScriptXml script = new GenScriptXml();
//				script.init(name, filePath, shortName, type);
//				fileList_name_file.put(name, (GenScriptI) script);
//				return script;
//			}
//		}
//		 
// 	} 
//
// 
// 	private void loadScriptXml(String xmlPath) throws Exception {
// 		log = MRLog.getInstance(MRLog.CONSOLE);;
//		xmlPath =xmlPath.replace('\\', '/');
//    	if(xmlPath.startsWith("/") ){
//    		 textScriptdoc =MRXMLUtils.loadXMLResource(xmlPath, log);
//    	}else{
//    		xmlPath = getFullPath(xmlPath);
//    		textScriptdoc  =MRXMLUtils.loadXMLFile(xmlPath, log);
//    	}
//   	}
//
//   	/*
//   	public Hashtable<String, Document> createXmls (boolean bGetDocList, String pdbObjectName, String OperationDataAttrbName, String[] arrOperDataAttrbVals , 
//  			String[] arrFilterActions , String pServiceName, String pxmlOUTPath , String pxmlPath ) {
// 
//  */
//	public  static BufferedWriter getBufferedWriter(String filePath) throws Exception{
//		FileWriter fstream;
//		BufferedWriter out ;
//		fstream = new FileWriter(filePath);
//	    out = new BufferedWriter(fstream);
//		return out;
//	}	
//	public void parse_ObjectXml(Element fromEL, String shortName, String parentXPath ) throws Exception {
// 		try {
// 			
// 			boolean parse = Utils.booleanValue(MRXMLUtils.getAttributeValue(fromEL, "Parse", "false"), false);
// 			if(!parse )
// 				return;
// 			
// 			String nameUnique = shortName+""+counter+"."+fromEL.getName();
// 			String xPath = parentXPath+"/" +fromEL.getName();
//
// 			String NIDScheme = MRXMLUtils.getAttributeValue(fromEL, IpvsDBUtils.NID_SCHEME, "").trim();
// 			if(NIDScheme.equals(IpvsDBUtils.NID_SCHEME_AUTO)) {
// 				nameUnique = nameUnique +".AUTO";
// 			}
// 			
// 			boolean bHasFreeChildsAttrb = XmlRepresentation.hasFree_Childs_Attrb(fromEL,log);
//  			if(bHasFreeChildsAttrb){
//  				list_xpath_name.put(xPath, nameUnique); 
//  				list_xpath_el.put(xPath,  fromEL ); 
//  				counter++;
//  			}	
//  			
// 			List<?> parseChilds = XmlRepresentation.getChildEL_PARSE(true , fromEL, log);
//  			for (Iterator<?> i = parseChilds.iterator(); i.hasNext();) {
//  				Element childFromEL = (Element) i.next();
//	  			parse_ObjectXml( childFromEL , shortName, xPath );
//  	 		}
//  			
// 		}catch(Exception ex ){
//  			ex.printStackTrace();
//  		}
// 	} 	
//	public GenScripts() throws Exception {
//		super();
//	}
//
// 	public void createTestScript(String dbObjectName,String testScript) throws Exception{
//		String serviceName="";
//		String xmlOUTPath="";
// 		createTestScript(dbObjectName, testScript,  serviceName, xmlOUTPath, genScriptXmlPath)	;	
//	}
// 	
//	public void createTestScript(String dbObjectName, String testScript,   String serviceName, String xmlOUTPath , String genScriptXmlPath ) throws Exception{
// 		try {
// 			if(genScriptXmlPath !=null && !genScriptXmlPath.trim().equals(""))
// 				this.genScriptXmlPath = genScriptXmlPath;
// 			this.xmlOUTPath_arg = xmlOUTPath;
// 			this.serviceName_arg = serviceName;
// 			loadScriptXml(this.genScriptXmlPath);
//	 		String xPath = xpath_object( dbObjectName);
// 	 		String[] objectArray = parseTag_ObjectList (  xPath );
// 	 		for(int i=0;i<objectArray.length;i++){
// 	 			processTag_object(objectArray[i],  testScript);
// 	 		}
//	 		
// 		}catch(Exception ex){
// 			ex.printStackTrace();
// 		}
// 	}
//   	public  static void main(String[] args) {
// 		String dbObjectName="";
// 		String testScript="";
// 		String serviceName="";
// 		String xmlOUTPath ="";
// 		String xmlPath ="";
// 		 
// 		try {
// 			GenScripts genScript = new GenScripts();
//			genScript.createTestScript(dbObjectName, testScript, serviceName, xmlOUTPath, xmlPath) ;
//		} catch (Exception e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		System.exit(0);
//	}
//}
