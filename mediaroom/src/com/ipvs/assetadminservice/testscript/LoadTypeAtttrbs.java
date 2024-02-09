package com.ipvs.assetadminservice.testscript;
public class LoadTypeAtttrbs {
  	// common NODE_NAME
	protected String loadNodeType="";
	protected String snXPathID="";
	protected String snSetNodeTextToVar="";
	protected int snIsUpdateTextNode=0;
	protected String snSetNodeToVar="";

	// common NODE_NAME loadNodeType="filePath" 
	protected String snFilePath="";

	//common NODE_NAME loadNodeType="testXmlScript"
	protected String snDbObjectName="";
	protected String snOperationDataType="";
	protected String snAction_suffix="";
	
	//common NODE_NAME loadNodeType="testXmlFile"
	protected String snRequestName="";

	//common NODE_NAME loadNodeType="use_varNode"
	protected String snVarNode="";
	
	//common NODE_NAME loadNodeType="use_respReqID"
	protected String snRespReqID="";
	 ////////////////////////////// common NODE_NAME start ////////////////////////////// 
	public String getLoadNodeType() {
		return loadNodeType;
	}
	public String getSnXPathID() {
		return snXPathID;
	}
	public String getSnSetNodeTextToVar() {
		return snSetNodeTextToVar;
	}
	public int getSnIsUpdateTextNode() {
		return snIsUpdateTextNode;
	}
	public String getSnSetNodeToVar() {
		return snSetNodeToVar;
	}
	public void setLoadNodeType(String loadNodeType) {
		this.loadNodeType = loadNodeType;
	}
	public void setSnXPathID(String snXPathID) {
		this.snXPathID = snXPathID;
	}
	public void setSnSetNodeTextToVar(String snSetNodeTextToVar) {
		this.snSetNodeTextToVar = snSetNodeTextToVar;
	}
	public void setSnIsUpdateTextNode(int snIsUpdateTextNode) {
		this.snIsUpdateTextNode = snIsUpdateTextNode;
	}
	public void setSnSetNodeToVar(String snSetNodeToVar) {
		this.snSetNodeToVar = snSetNodeToVar;
	}
	////////////////////////////// common NODE_NAME end ////////////////////////////// 
	////////////////////////////// common NODE_NAME loadNodeType="filePath"  start ////////////////////////////// 
	public String getSnFilePath() {
		return snFilePath;
	}
	public void setSnFilePath(String snFilePath) {
		this.snFilePath = snFilePath;
	}
	////////////////////////////// common NODE_NAME loadNodeType="filePath"  end ////////////////////////////// 
	////////////////////////////// common NODE_NAME loadNodeType="testXmlScript"  start ////////////////////////////// 
	public String getSnDbObjectName() {
		return snDbObjectName;
	}
	public String getSnOperationDataType() {
		return snOperationDataType;
	}
	public String getSnAction_suffix() {
		return snAction_suffix;
	}
	public void setSnDbObjectName(String snDbObjectName) {
		this.snDbObjectName = snDbObjectName;
	}
	public void setSnOperationDataType(String snOperationDataType) {
		this.snOperationDataType = snOperationDataType;
	}
	public void setSnAction_suffix(String snAction_suffix) {
		this.snAction_suffix = snAction_suffix;
	}
	////////////////////////////// common NODE_NAME loadNodeType="testXmlScript"  end ////////////////////////////// 
	////////////////////////////// common NODE_NAME loadNodeType="testXmlFile"  start ////////////////////////////// 
	public String getSnRequestName() {
		return snRequestName;
	}
	public void setSnRequestName(String snRequestName) {
		this.snRequestName = snRequestName;
	}
	////////////////////////////// common NODE_NAME loadNodeType="testXmlFile"  end ////////////////////////////// 
	////////////////////////////// common NODE_NAME loadNodeType="use_varNode"  start ////////////////////////////// 
	public String getSnVarNode() {
		return snVarNode;
	}
	public String getSnRespReqID() {
		return snRespReqID;
	}
	////////////////////////////// common NODE_NAME loadNodeType="use_varNode"  end ////////////////////////////// 
	////////////////////////////// common NODE_NAME loadNodeType="use_respReqID"  start ////////////////////////////// 
	public void setSnVarNode(String snVarNode) {
		this.snVarNode = snVarNode;
	}
	public void setSnRespReqID(String snRespReqID) {
		this.snRespReqID = snRespReqID;
	}
	////////////////////////////// common NODE_NAME loadNodeType="use_respReqID"  end ////////////////////////////// 
 
}

/*

#<!-- loadNodeType = filePath , testXmlScript ,  testXmlFile , use_varNode , use_respReqID -->
#<NODE_NAME loadNodeType="filePath" snFilePath="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
#<NODE_NAME loadNodeType="testXmlScript" snDbObjectName="" snOperationDataType="" snAction_suffix="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1" />
#<NODE_NAME loadNodeType="testXmlFile" snRequestName="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
#<NODE_NAME loadNodeType="use_varNode"  snVarNode="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1"/>
#<NODE_NAME loadNodeType="use_respReqID"  snRespReqID="" snXPathID="" snSetNodeToVar="" snSetNodeTextToVar="" snIsUpdateTextNode="1">
#<eval_variable  eval_proc="isNotNull" eval_arg="${requestTemplateFile}" setResult="pass/fail"/> 
#</NODE_NAME>


*/