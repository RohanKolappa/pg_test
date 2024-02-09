package com.ipvs.assetadminservice.testscript;

import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;

public class ScriptNode {
	/*
	 <var name=""/>
	 <set_variable name=""/>
	 <eval_variable name="" eval_proc="isNotNull" eval_arg="${requestTemplateFile}" setResult="pass/fail" doExist="1/0"/> 
	 */

	public static Element var( String name){
		return createNode(ScXmlConst.var, name);
	}
	public static Element var( String name, String originalXpath){
		return  createNode_xpath( ScXmlConst.var ,   name,   originalXpath);
	}	
	public static Element set_variable( String name){
		return createNode(ScXmlConst.set_variable, name);
	}
	public static Element eval_variable( String name, String eval_proc, String eval_arg, String setResult, int  doExist){
		Element scriptNode = null;
		try {
			scriptNode = createNode(ScXmlConst.eval_variable, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.eval_proc,eval_proc);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.eval_arg,eval_arg);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.setResult,setResult);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.doExist,doExist+"");
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;		
	}
	
	/*
	 <executeSeq name="Add_RouteTable" userid=""/>
     <executeRequestAction name="" pauseAfterMiliSec="1" checkResponse="case_name_pass_1" testResult="must_success" userid=""/>

	 <RequestAction name="" userid=""/>
	 <execute name="" userid=""/>
	 */
	public static Element executeSeq( String name){
		return createNode(ScXmlConst.executeSeq, name);
	}
	public static Element executeRequestAction( String name, String pauseAfterMiliSec, String checkResponse, String testResult) {
		Element scriptNode = null;
		try {
			scriptNode = createNode(ScXmlConst.executeRequestAction, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.pauseAfterMiliSec,pauseAfterMiliSec);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.checkResponse,checkResponse);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.testResult,testResult);
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;			
	}
	public static Element requestAction( String name){
		return createNode(ScXmlConst.RequestAction, name);
	}
	public static Element execute( String name){
		return createNode(ScXmlConst.execute, name);
	}
	
	/*
	<sendRequest  name="" userid="" set_varResRootNode="" set_varResReqID="" NODE_NAME />

	<!-- action =  appendElement / replaceElement / deleteElement / cloneElement /  cloneElement -->	
	<modifyElement  name="" modify_action="" modify_varNode="" modify_varNode_xPathID="" NODE_NAME />
	*/	
	public static Element sendRequest( String name, String set_varResRootNode, String set_varResReqID) {
		Element scriptNode = null;
		try {
			scriptNode = createNode(ScXmlConst.sendRequest, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.set_varResRootNode,set_varResRootNode);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.set_varResReqID,set_varResReqID);
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;			
	}
	public static Element modifyElement( String name, String modify_action, String modify_varNode, String modify_varNode_xPathID) {
		Element scriptNode = null;
		try {
			scriptNode = createNode(ScXmlConst.modifyElement, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.modify_action,modify_action);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.modify_varNode,modify_varNode);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.modify_varNode_xPathID,modify_varNode_xPathID);
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;			
	}
	/*
	<setParamValue name="" /> 
	<readParamValue name="" /> 
	 */
	public static Element setParamValue( String name , String variableName){
		Element scriptNode = null;
		try {
 			// common NODE_NAME
			scriptNode = MRXMLUtils.addElement(ScXmlConst.setParamValue, variableName);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.name,name);
			 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;		
	 
	}
	public static Element readParamValue( String name){
		return createNode(ScXmlConst.readParamValue, name);
	}
	/*
	 <variableList name=""/>
	 <setParamValues name="" /> 
	 <readParamValues name="" /> 
	 <call_variableList name=""/>
	 <import_variableList name="" filePath="" /> 
	 */
	public static Element variableList( String name, String originalXpath){
 		return  createNode_xpath( ScXmlConst.variableList ,   name,   originalXpath);
	}
	public static Element createNode_xpath(String tagName, String name, String originalXpath){
		Element scriptNode = null;
		try {
 			// common NODE_NAME
			scriptNode = createNode(tagName, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.originalXpath,originalXpath);
			 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}	
	public static Element setParamValues( String name, String originalXpath){
 		return  createNode_xpath( ScXmlConst.setParamValues ,   name,   originalXpath);
	}
	public static Element setParamValues( String name){
		return createNode(ScXmlConst.setParamValues, name);
	}	
	public static Element readParamValues( String name){
		return createNode(ScXmlConst.readParamValues, name);
	}
	public static Element call_variableList( String name){
		return createNode(ScXmlConst.call_variableList, name);
	}
	public static Element import_variableList( String name){
		return createNode(ScXmlConst.import_variableList, name);
	}
	
	/*
     <import_executeRequestAction name="" filePath=""/>
	 <import_RequestAction name="" filePath="" /> 
	 <call_execute name=""/>
	 <import_execute name="" filePath="" /> 
	*/
	public static Element import_executeRequestAction( String name, String filePath){
		return import_NODE(ScXmlConst.import_executeRequestAction, name,filePath);
	}
	public static Element import_RequestAction( String name){
		return createNode(ScXmlConst.import_RequestAction, name);
	}
	public static Element call_execute( String name){
		return createNode(ScXmlConst.call_execute, name);
	}
	public static Element import_execute( String name){
		return createNode(ScXmlConst.import_execute, name);
	}
	/*
	 <call_modifyElement name=""/>
	 <import_modifyElement name="" filePath="" /> 
	 <call_setParamValues name=""/>
	 <import_setParamValues name="" filePath="" /> 
	 <call_readParamValues name=""/>
	 <import_readParamValues name="" filePath="" /> 
	 */
	public static Element call_modifyElement( String name){
		return createNode(ScXmlConst.call_modifyElement, name);
	}
	public static Element import_modifyElement( String name){
		return createNode(ScXmlConst.import_modifyElement, name);
	}
	public static Element call_setParamValues( String name){
		return createNode(ScXmlConst.call_setParamValues, name);
	}
	public static Element import_setParamValues( String name){
		return createNode(ScXmlConst.import_setParamValues, name);
	}
	public static Element call_readParamValues( String name){
		return createNode(ScXmlConst.call_readParamValues, name);
	}	
	public static Element import_readParamValues( String name){
		return createNode(ScXmlConst.import_readParamValues, name);
	}
 	
	///////////////////// commmon //////////////////////////////////
	public static Element createNode(String tagName, String name){
		Element scriptNode = null;
		try {
 			// common NODE_NAME
			scriptNode = MRXMLUtils.addElement(tagName);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.name,name);
			 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}
	public static Element import_NODE( String tagName, String name, String filePath ){
		Element scriptNode = null;
		try {
			scriptNode = createNode(tagName, name);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.filePath,filePath);
			 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;		
	}
	///////////////////////////////////////// update ///////////////////////////////////////// 
	public static Element updateNode_eval(Element scriptNode, String eval_proc, String eval_arg ){
		try {
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.eval_proc,eval_proc);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.eval_arg,eval_arg);
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}
	
	//TODO: 
	public static Element updateNode_LoadType(Element scriptNode, String loadNodeType , String snXPathID, String snSetNodeTextToVar, String snIsUpdateTextNode, String snSetNodeToVar){
		try {
			// common NODE_NAME
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.loadNodeType, loadNodeType);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snXPathID,snXPathID);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeTextToVar,snSetNodeTextToVar);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snIsUpdateTextNode,snIsUpdateTextNode+"");
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeToVar,snSetNodeToVar);
	 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}	
	
	
	
	public static Element updateNode_use_varNode(Element scriptNode, String snVarNode){
		String snXPathID="";
		String snSetNodeTextToVar="";
		String snIsUpdateTextNode="";
		String snSetNodeToVar="";
		return updateNode_use_varNode(scriptNode, snVarNode, snXPathID, snSetNodeTextToVar, snIsUpdateTextNode, snSetNodeToVar);
	}	
	public static Element updateNode_use_varNode(Element scriptNode, String snVarNode, String snXPathID, String snSetNodeTextToVar, String snIsUpdateTextNode, String snSetNodeToVar){
		try {
			// common NODE_NAME
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.loadNodeType, ScXmlConst.LOAD_TYPE_use_varNode);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snVarNode, snVarNode);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snXPathID,snXPathID);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeTextToVar,snSetNodeTextToVar);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snIsUpdateTextNode,snIsUpdateTextNode+"");
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeToVar,snSetNodeToVar);
	 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}	

	public static Element updateNode_filePath(Element scriptNode, String snFilePath){
		String snXPathID="";
		String snSetNodeTextToVar="";
		String snIsUpdateTextNode="";
		String snSetNodeToVar="";
		return updateNode_filePath(scriptNode, snFilePath, snXPathID, snSetNodeTextToVar, snIsUpdateTextNode, snSetNodeToVar);
	}
	public static Element updateNode_filePath(Element scriptNode, String snFilePath, String snXPathID, String snSetNodeTextToVar, String snIsUpdateTextNode, String snSetNodeToVar){
		try {
			// common NODE_NAME
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.loadNodeType, ScXmlConst.LOAD_TYPE_filePath);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snFilePath, snFilePath);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snXPathID,snXPathID);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeTextToVar,snSetNodeTextToVar);
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snIsUpdateTextNode,snIsUpdateTextNode+"");
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeToVar,snSetNodeToVar);
	 
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}	
	
	public static Element updateNode_LoadType(Element scriptNode, LoadTypeAtttrbs loadTypeAtttrbs){
		try {
 			
			// common NODE_NAME
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.loadNodeType, loadTypeAtttrbs.getLoadNodeType());
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snXPathID,loadTypeAtttrbs.getSnXPathID());
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeTextToVar,loadTypeAtttrbs.getSnSetNodeTextToVar());
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snIsUpdateTextNode,loadTypeAtttrbs.getSnIsUpdateTextNode()+"");
			MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snSetNodeToVar,loadTypeAtttrbs.getSnSetNodeToVar());
			
			String loadNodeType = loadTypeAtttrbs.getLoadNodeType();
			
			// common NODE_NAME loadNodeType="filePath" 
			if(loadNodeType.equalsIgnoreCase(ScXmlConst.LOAD_TYPE_filePath)){
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snFilePath,loadTypeAtttrbs.getSnFilePath());
			
			//common NODE_NAME loadNodeType="testXmlScript"
			} else if(loadNodeType.equalsIgnoreCase(ScXmlConst.LOAD_TYPE_testXmlScript)){
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snDbObjectName,loadTypeAtttrbs.getSnDbObjectName());
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snOperationDataType,loadTypeAtttrbs.getSnOperationDataType());
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snAction_suffix,loadTypeAtttrbs.getSnAction_suffix());
			
			//common NODE_NAME loadNodeType="testXmlFile"
			} else if(loadNodeType.equalsIgnoreCase(ScXmlConst.LOAD_TYPE_testXmlFile)){
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snRequestName,loadTypeAtttrbs.getSnRequestName());
			
			//common NODE_NAME loadNodeType="use_varNode"
			} else if(loadNodeType.equalsIgnoreCase(ScXmlConst.LOAD_TYPE_use_varNode)){
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snVarNode,loadTypeAtttrbs.getSnVarNode());
			
			//common NODE_NAME loadNodeType="use_respReqID"
			} else if(loadNodeType.equalsIgnoreCase(ScXmlConst.LOAD_TYPE_use_respReqID)){
				MRXMLUtils.addAttribute(scriptNode, ScXmlConst.snVarNode,loadTypeAtttrbs.getSnVarNode());
			}
		} catch (Exception e) {
			e.printStackTrace();
		}		
		return scriptNode;
	}
	///////////////////////////////////////// update ///////////////////////////////////////// 

}
/*

<!-- loadNodeType = filePath , testXmlScript ,  testXmlFile , use_varNode , use_respReqID -->
<NODE_NAME loadNodeType="filePath" snFilePath="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
<NODE_NAME loadNodeType="testXmlScript" snDbObjectName="" snOperationDataType="" snAction_suffix="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1" />
<NODE_NAME loadNodeType="testXmlFile" snRequestName="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
<NODE_NAME loadNodeType="use_varNode"  snVarNode="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
<NODE_NAME loadNodeType="use_respReqID"  snRespReqID="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1">
	<eval_variable name="" eval_proc="isNotNull" eval_arg="${requestTemplateFile}" setResult="pass/fail" doExist="1/0"/> 
</NODE_NAME>


 <var name=""/>
 <set_variable name="" NODE_NAME />
 <eval_variable name="" eval_proc="isNotNull" eval_arg="${requestTemplateFile}" setResult="pass/fail" doExist="1/0"/> 
 
 
<executeSeq name="Add_RouteTable" userid=""/>
<executeRequestAction name="" pauseAfterMiliSec="1" checkResponse="case_name_pass_1" testResult="must_success" userid=""/>
<import_executeRequestAction name="" filepath=""/>
<RequestAction name="" userid=""/>
<execute name="" userid=""/>

<!-- action =  appendElement / replaceElement / deleteElement / cloneElement /  cloneElement -->	
<modifyElement  name="" modify_action="" modify_varNode="" modify_varNode_xPathID="" NODE_NAME />
<sendRequest  name="" userid="" set_varResRootNode="" set_varResReqID="" NODE_NAME />

<setParamValue name="" /> 
<readParamValue name="" /> 
  
 
 
 /////////// list elements ///////////   
 <variableList name=""/>
 <setParamValues name="" /> 
 <readParamValues name="" /> 
 <call_variableList name=""/>
 <import_variableList name="" filePath="" /> 
 <import_RequestAction name="" filePath="" /> 
 <call_execute name=""/>
 <import_execute name="" filePath="" /> 
 <call_modifyElement name=""/>
 <import_modifyElement name="" filePath="" /> 
 <call_setParamValues name=""/>
 <import_setParamValues name="" filePath="" /> 
 <call_readParamValues name=""/>
 <import_readParamValues name="" filePath="" /> 

 /// executeSeq  
 <executeSeq name="Add_RouteTable">
    <executeRequestAction name="Get Asset Given Title" pauseAfterMiliSec="1" checkResponse="case_name_pass_1" testResult="must_success" />
 </executeSeq>
 /// child where xPathID attrib is there 
 <format_ATTRIB_NAME xPathID="">
	<formatArgument/>
 </format_ATTRIB_NAME> 
 
 
 ///////////////common childs to all///////////////	
 <var name=""/>
 <set_variable name=""/>
 <eval_variable name=""/>
 <variableList name=""/>
 <call_variableList name=""/>
 <import_variableList name="" filePath="" /> 

 
 
*/