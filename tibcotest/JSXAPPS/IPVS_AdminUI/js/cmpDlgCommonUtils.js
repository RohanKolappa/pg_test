function changeImageState(a_imgName,a_state) {
  PNameSpace.getJSXByName(a_imgName).setState(a_state);
  PNameSpace.getJSXByName(a_imgName).repaint();
}

function changeImagePropertyState(a_imgName,a_state) {
  if( a_state == "1") {
    PNameSpace.getJSXByName(a_imgName).setSrc("images/buttons/checkOn.gif");
    PNameSpace.getJSXByName(a_imgName).repaint();
  } else {
    PNameSpace.getJSXByName(a_imgName).setSrc("images/buttons/checkOff.gif");
    PNameSpace.getJSXByName(a_imgName).repaint();  
  }
}

function chkForExistence(vassetGrpName,vassetGrpValue,a_matrix){
  var objMatrix = PNameSpace.getJSXByName(a_matrix);
  var val = true;

  if (objMatrix) {
    var strXMLCacheId = objMatrix.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordName = objRecordNode.getAttribute("jsxtext");
       // var strRecordGroup = objRecordNode.getAttribute("jsxgroup");
        if((vassetGrpName==strRecordName) ){
          return false;
        }
        else{       
        }
      }
    }
  }
 return val;
}

function chkForOrderExistence(a_OrderValue, a_FieldName, a_matrix, a_Msg){
  var objMatrix = getJsxByName(a_matrix);
  var fieldNameBlkJSX = getJsxByName(a_FieldName);

  if (objMatrix) {
    var strXMLCacheId = objMatrix.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordIndex = objRecordNode.getAttribute("index");
        if (jsx3.util.strEmpty(strRecordIndex)){
          strRecordIndex = objRecordNode.getAttribute("jsxindex");
        }
        if (parseInt(a_OrderValue, 10) == parseInt(strRecordIndex, 10)) {
          alert(a_Msg);
          fieldNameBlkJSX.focus();
          return false;
        }
      }
    }
  }
 return true;
}

function chkForNameExistence(a_NameVal, a_FieldName, a_matrix, a_Msg){
  var objMatrix = getJsxByName(a_matrix);
  var fieldNameBlkJSX = getJsxByName(a_FieldName);

  if (objMatrix) {
    var strXMLCacheId = objMatrix.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordName = objRecordNode.getAttribute("jsxtext");
        if((a_NameVal == strRecordName) ){
          alert(a_Msg);
          fieldNameBlkJSX.focus();
          return false;
        }
      }
    }
  }
 return true;
}

function chkGroupTagValueChanged(a_NID, a_Matrix) {
  var objMatrix = PNameSpace.getJSXByName(a_Matrix);
  var dataNode = topic_GetNIDNode(tCache, a_NID);
  var flag = false;
   
  if ( (objMatrix) && (dataNode != null) ) {
    var strXMLCacheId = objMatrix.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordName = objRecordNode.getAttribute("TagName");
        var strRecordValue = objRecordNode.getAttribute("TagValue");
        var dataGroupTagNodeListIter = dataNode.selectNodeIterator("//Info/Groups/Tag");
        while( dataGroupTagNodeListIter.hasNext() ) {
			    var dataGroupTagNode = dataGroupTagNodeListIter.next();
			    var GroupTagName = dataGroupTagNode.getAttribute("Name");
			    var GroupTagValue = dataGroupTagNode.getAttribute("Value");
			    if( (strRecordName == GroupTagName) && (strRecordValue == GroupTagValue) ) {
			      flag = false;
			      break;
			    } else {
			      flag = true;
			    }
			  }
        if(flag) {
          return true;
        }
      }
    }
  }  
  return false;
}

function changeCheckBoxState(a_CheckBoxName,a_State) {
  //state=0/1 checked =1, unchecked=0
  var chkBlkJSX = getJsxByName(a_CheckBoxName);
  //PNameSpace.getJSXByName(a_CheckBoxName).setChecked(a_State);
  chkBlkJSX.setChecked(a_State);
}

function getTagObjectFilterName(a_TagObjectFilterNID) {
  var TagObjectFilterName = "";

  var filterNameNode = topic_GetNode(tCache, ".//*[@NID='" + a_TagObjectFilterNID + "']");
  if(filterNameNode) {
    TagObjectFilterName = filterNameNode.selectSingleNode(".//Name").getValue();
  }
  return TagObjectFilterName;
}

function getTagDefName(a_TagObjectFilterNID) {
  var TagObjectFilterName = "";

  var filterNameNode = topic_GetNode(tCache, ".//*[@NID='" + a_TagObjectFilterNID + "']");
  if(filterNameNode) {
    TagObjectFilterName = filterNameNode.selectSingleNode(".//TagName").getValue();
  }
  return TagObjectFilterName;
}

function getTagToMatrixElement(a_MatrixName,a_TagObjectType) {

  PNameSpace.getJSXByName(a_MatrixName).resetCacheData();

  var groupTagNameNIDListNode = topic_GetNode(tApp, "//GroupTagNIDList");
  if( groupTagNameNIDListNode != null ) {
    var groupTagNameNIDListIter = groupTagNameNIDListNode.selectNodeIterator(".//ObjectNID");
    while( groupTagNameNIDListIter.hasNext() ) {
      var groupTagNameNID = groupTagNameNIDListIter.next().getValue();
      var groupTagNameNode = topic_GetNode(tCache, "//*[@NID='" + groupTagNameNID + "']");
      var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
      var groupTagPropertyListIter =  groupTagNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
      while( groupTagPropertyListIter.hasNext() ) {
        var groupTagPropertyNode = groupTagPropertyListIter.next();
        var groupTagPropertyType = groupTagPropertyNode.getValue();
        if(groupTagPropertyType == a_TagObjectType){
          var groupTagValueListIter = groupTagNameNode.selectNodeIterator(".//TagValueList/TagValue");
          while( groupTagValueListIter.hasNext() ) {
            var groupTagValueNode = groupTagValueListIter.next();
            var groupTagValue = groupTagValueNode.getValue();
            var o = new Object();
            o.jsxid = jsx3.xml.CDF.getKey() ;
            o.jsxtextTagNameAvailable= groupTagName;
            o.jsxtextTagValueAvailable= groupTagValue;
			   
            PNameSpace.getJSXByName(a_MatrixName).insertRecord(o, null, false); 
          }      
        }

      }   
    }
    PNameSpace.getJSXByName(a_MatrixName).repaintData();
  }
}

function getTagDefToSelectElement(a_SelectName,a_FilterName){
  var selectNameBlkJSX = getJsxByName(a_SelectName);
  selectNameBlkJSX.resetCacheData();
  var groupTagNameNIDListNode = topic_GetNode(tApp, "//GroupTagNIDList");
  if( groupTagNameNIDListNode != null ) {
    var groupTagNameNIDListIter = groupTagNameNIDListNode.selectNodeIterator(".//ObjectNID");
    while( groupTagNameNIDListIter.hasNext() ) {
      var groupTagNameNID = groupTagNameNIDListIter.next().getValue();
      var groupTagNameNode = topic_GetNode(tCache, "//*[@NID='" + groupTagNameNID + "']");
      var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
      var groupTagPropertyListIter =     groupTagNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
      while( groupTagPropertyListIter.hasNext() ) {
        var groupTagPropertyNode = groupTagPropertyListIter.next();
        var groupTagPropertyType = groupTagPropertyNode.getValue();
        if(a_FilterName == "ALL") {
          insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
        } else if(a_FilterName == "Resource") {
          if( (groupTagPropertyType == "AssetResource") || 
              (groupTagPropertyType == "MediaStreamPortResource") ||
              (groupTagPropertyType == "MediaStorePortResource") ) {
            insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
        } else if(a_FilterName == "Media") {
          if( (groupTagPropertyType == "MediaStreamFileResource") ||
              (groupTagPropertyType == "MediaStoreDirResource") ) {
            insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
        } else if(groupTagPropertyType == a_FilterName) {
          insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
        }
      }   
    }
  }
    insertElementInSelectBox("SelectGroupTag" , "Select Tag Name", a_SelectName );
    insertElementInSelectBox("DEFAULT_GROUPTAG_NID" , "--Add Tag Name", a_SelectName );
    selectNameBlkJSX.setValue("Select Tag Name");
    selectNameBlkJSX.paint();
}

function insertElementInSelectBox(a_NID, a_Name, a_SelectName) {
  var seleNameBlkJSX = getJsxByName(a_SelectName);
  if( chkForExistenceInSelect(a_Name, a_NID, a_SelectName) ) {
    var o = new Object();
    //o.jsxid = Math.floor(Math.random()*100001) ;
    o.jsxid = a_Name;
    o.NID = a_NID;
    o.jsxtext = a_Name;
			   
    seleNameBlkJSX.insertRecord(o, null, false);
  }
}

function getFilterToSelectElement(a_SelectName,a_FilterName) {
  var selectNameBlkJSX = getJsxByName(a_SelectName);
  selectNameBlkJSX.resetCacheData();
  var filterNameNIDListNode = topic_GetNode(tApp, "//TagObjectFilterNIDList");
  if( filterNameNIDListNode != null ) {
    var filterNameNIDListIter = filterNameNIDListNode.selectNodeIterator(".//ObjectNID");
    while( filterNameNIDListIter.hasNext() ) {
      var filterNameNID = filterNameNIDListIter.next().getValue();
      var filterNameNode = topic_GetNode(tCache, "//*[@NID='" + filterNameNID + "']");
      var filterName = filterNameNode.selectSingleNode(".//Name").getValue();
      var filterPropertyListIter = filterNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
      while( filterPropertyListIter.hasNext() ) {
        var filterPropertyNode = filterPropertyListIter.next();
        var filterPropertyType = filterPropertyNode.getValue();
        if(a_FilterName == "ALL") {
          insertElementInSelectBox(filterNameNID, filterName, a_SelectName);
        } else if(a_FilterName == "Resource") {
          if( (filterPropertyType == "AssetResource") || (filterPropertyType == "MediaStreamPortResource") ) {
            insertElementInSelectBox(filterNameNID, filterName, a_SelectName);
          } 
        } else if(a_FilterName == "ResourceProfile") {
          if( (filterPropertyType == "AssetResource") || (filterPropertyType == "MediaStreamPortResource") || (filterPropertyType == "MediaStreamProfile") ) {
            insertElementInSelectBox(filterNameNID, filterName, a_SelectName);
          }
        } else if(filterPropertyType == a_FilterName) {
          insertElementInSelectBox(filterNameNID, filterName, a_SelectName);
        }   
      }
    }
  }
    insertElementInSelectBox("Select"+a_FilterName , "Select "+a_FilterName + " Filter", a_SelectName );  
    insertElementInSelectBox("DEFAULT_TAGOBJECTFILTER_NID" , "--Add "+a_FilterName + " Filter", a_SelectName );
    selectNameBlkJSX.setValue("Select "+a_FilterName + " Filter");
    selectNameBlkJSX.paint();  
}

function getStreamFilterToSelectElement(a_SelectName) {
//"selCollabStreamFilter"
  var selectNameBlkJSX = getJsxByName(a_SelectName);
  selectNameBlkJSX.resetCacheData();
  var streamNIDListNode = topic_GetNode(tApp, "//MediaStreamFilterNIDList");
  if( streamNIDListNode != null ) {
    var streamNIDListIter = streamNIDListNode.selectNodeIterator(".//ObjectNID");
    while( streamNIDListIter.hasNext() ) {
      var streamNID = streamNIDListIter.next().getValue();
      var streamNode = topic_GetNode(tCache, ".//*[@NID='" + streamNID + "']");
      var streamFilterName = streamNode.selectSingleNode(".//Name").getValue();
      insertElementInSelectBox(streamNID, streamFilterName, a_SelectName );
    }
  }
  insertElementInSelectBox("SelectStreamFilter" , "Select Stream Filter", a_SelectName );  
  insertElementInSelectBox("DEFAULT_MEDIASTREAMFILTER_NID" , "--Add Stream Filter", a_SelectName );
  selectNameBlkJSX.paint();
}



function AddFilterToMatrix(a_SelectNameOne,a_SelectNameTwo,a_Matrix){
  //alert("a_SelectNameOne:"+a_SelectNameOne+",a_SelectNameTwo:"+a_SelectNameTwo+",a_Matrix:"+a_Matrix);
  //PNameSpace.getJSXByName(a_Matrix).resetCacheData();
  var matrixBlkJSX = getJsxByName(a_Matrix);
  if(a_SelectNameOne!=""){
    var RIDSelectOneBlkJSX = getJsxByName(a_SelectNameOne);
    var RIDSelectOne = RIDSelectOneBlkJSX.getValue();
    var mtachResult = RIDSelectOne.search(/Select /);

    if(mtachResult != -1){
      alert("Please "+RIDSelectOne);
      return false;
    } else {
      var SelectOneNode = RIDSelectOneBlkJSX.getRecordNode(RIDSelectOne);
      var FirstFilterName= SelectOneNode.getAttribute("jsxtext");
      var FirstFilterNID = SelectOneNode.getAttribute("NID");
    }
    var RIDSelectTwoBlkJSX = null;
    if(a_SelectNameTwo!=""){
      RIDSelectTwoBlkJSX = getJsxByName(a_SelectNameTwo);
      var RIDSelectTwo = RIDSelectTwoBlkJSX.getValue();
      var mtachResult = RIDSelectTwo.search(/Select /);

      if(mtachResult != -1){
        alert("Please "+RIDSelectTwo);
        return false;
      } else {
        var SelectTwoNode = RIDSelectTwoBlkJSX.getRecordNode(RIDSelectTwo);
        var SecondFilterName= SelectTwoNode.getAttribute("jsxtext");
        var SecondFilterNID = SelectTwoNode.getAttribute("NID");
      }
    }
    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey() ;
    o.jsxtextOneNID = FirstFilterNID;
    o.jsxtextOne = FirstFilterName;
    if(a_SelectNameTwo!=""){
      o.jsxAt="@";
      o.jsxtextTwoNID = SecondFilterNID;
      o.jsxtextTwo = SecondFilterName;
    }
    matrixBlkJSX.insertRecord(o, null, false);
  }
  matrixBlkJSX.repaintData();
  resetSelectBoxValueToDefault(RIDSelectOneBlkJSX);
  resetSelectBoxValueToDefault(RIDSelectTwoBlkJSX);
}


function resetSelectBoxValueToDefault(a_SelectBoxName){
  var objSelect = a_SelectBoxName;
  if(objSelect){
     var strXMLCacheId = objSelect.getXMLId();
     var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
     var recordIter = objXML.selectNodeIterator("//record[@jsxtext]");
       while( recordIter.hasNext()){
         var recordID = recordIter.next().getAttribute("jsxtext");
         var recordName = recordID.search(/Select /);
           if(recordName != -1){
             a_SelectBoxName.setValue(recordID);
           } 
       }
  }
}


function AddRuleKeyToMatrix(a_LHSelectRuleKey,a_LHSelectTagName,a_SelectMatchOperation,a_RHSelectRuleKey,a_RHSelectTagName,a_Matrix){
  //alert("a_SelectNameOne:"+a_SelectNameOne+",a_SelectNameTwo:"+a_SelectNameTwo+",a_Matrix:"+a_Matrix);
  //PNameSpace.getJSXByName(a_Matrix).resetCacheData();
  
  var LHSRuleKey = RuleKeyCheck(a_LHSelectRuleKey);
  if(LHSRuleKey)
    var LHSTagName = RuleKeyCheck(a_LHSelectTagName);
  if(LHSTagName)
    var MatchOperation = RuleKeyCheck(a_SelectMatchOperation);
  if(MatchOperation)
    var RHSRuleKey = RuleKeyCheck(a_RHSelectRuleKey);
  if(RHSRuleKey)
    var RHSTagName = RuleKeyCheck(a_RHSelectTagName);
  if(RHSTagName) {
    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey() ;
    o.jsxtextLHRuleKey = LHSRuleKey;
    o.jsxtextLHTagName = LHSTagName;
    o.jsxtextOperation = MatchOperation;
    o.jsxtextRHRuleKey = RHSRuleKey;
    o.jsxtextRHTagName = RHSTagName;

    PNameSpace.getJSXByName(a_Matrix).insertRecord(o, null, false);
  }

  PNameSpace.getJSXByName(a_Matrix).repaintData();
  resetSelectBoxValueToDefault(PNameSpace.getJSXByName(a_LHSelectRuleKey));
  resetSelectBoxValueToDefault(PNameSpace.getJSXByName(a_LHSelectTagName));
  resetSelectBoxValueToDefault(PNameSpace.getJSXByName(a_SelectMatchOperation));
  resetSelectBoxValueToDefault(PNameSpace.getJSXByName(a_RHSelectRuleKey));
  resetSelectBoxValueToDefault(PNameSpace.getJSXByName(a_RHSelectTagName));
}

function RuleKeyCheck(a_TagName) {
  var SelectTagName = PNameSpace.getJSXByName(a_TagName).getValue();
  var matchResult = SelectTagName.search(/Select /);

  if(matchResult != -1) {
    alert("Please " + SelectTagName);
    return false;
  } else {
    var SelectOneNode = PNameSpace.getJSXByName(a_TagName).getRecordNode(SelectTagName);
    return SelectOneNode.getAttribute("jsxtext");
  }
}

function checkValue(a_Value, a_FieldName, a_Message) {
  if( a_Value == "") {
    alert(a_Message);
    var fieldNameBlkJSX = getJsxByName(a_FieldName);
    fieldNameBlkJSX.focus();
    return false;
  } else {
    return true;
  }
}

function addDatatoMatrix(userFilterBlkJSX, resourceFilterBlkJSX, filterMatrix){
  var userRecord = userFilterBlkJSX.getValue();
  if(userRecord!=""){
    var mtachResult = userRecord.search(/Select /);

    if(mtachResult != -1){
      alert("Please "+userRecord);
      return false;
    } else {
      var SelectOneNode = userFilterBlkJSX.getRecord(userRecord);
      var FirstFilterName= SelectOneNode.jsxtext;
      var FirstFilterNID = SelectOneNode.NID;
    }
    var resourceRecord = resourceFilterBlkJSX.getValue();
    if(resourceRecord!=""){
      var mtachResult = resourceRecord.search(/Select /);

      if(mtachResult != -1){
        alert("Please "+resourceRecord);
        return false;
      } else {
        var SelectTwoNode = resourceFilterBlkJSX.getRecord(resourceRecord);
        var SecondFilterName= SelectTwoNode.jsxtext;
        var SecondFilterNID = SelectTwoNode.NID;
      }
    }
    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey();
    o.jsxtextOneNID = FirstFilterNID;
    o.jsxtextOne = FirstFilterName;
    if(resourceRecord!=""){
      o.jsxAt="@";
      o.jsxtextTwoNID = SecondFilterNID;
      o.jsxtextTwo = SecondFilterName;
    }
    filterMatrix.insertRecord(o, null, false);
  }
  filterMatrix.repaintData();
  userFilterBlkJSX.setValue("Select User Filter");
  resourceFilterBlkJSX.setValue("Select Resource Filter");
}

function SelectRuleKeyCheck(ruleKeyName) {
  var SelectTagName = ruleKeyName.getValue();
  var matchResult = SelectTagName.search(/Select /);
  var rtValue = false;
  if(matchResult != -1) {
    alert("Please " + SelectTagName);
  } else {
    var SelectOneNode = ruleKeyName.getRecordNode(SelectTagName);
    if(SelectOneNode != null){
      rtValue=SelectOneNode.getAttribute("jsxtext");
    }
  }
  return rtValue;
}

function checkAddForNewFilter(recordNode, filterType) {
  var selTagNID = recordNode.getAttribute("NID");
  if( selTagNID == "DEFAULT_TAGOBJECTFILTER_NID") {
    topic_Set(tApp, "AddNewFilterType", filterType);
    dlgEditTagObjectFilter_Create("DEFAULT_TAGOBJECTFILTER_NID");
  }
}

function checkAddForNewTagName(recordNode) {
  var selTagNID = recordNode.getAttribute("NID");
  if( selTagNID == "DEFAULT_GROUPTAG_NID") {
    dlgEditGroupTag_Create("DEFAULT_GROUPTAG_NID");
  }
}

function checkDeviceName(a_Value, a_FieldName, a_Message) {
  if( a_Message == null) {
    a_Message = "The Device Name must be lowercase alphabetic letters and numbers only. \n"+
                "The first character of the Device Name must be an alphabetic letter.";
  }
  
  var strPasswordExp = /^[a-z][a-z0-9]+$/;
  if( !strPasswordExp.test(a_Value)) {
    alert(a_Message);
    getJsxByName(a_FieldName).focus();
    return false;
  } else {
    return true;
  }
}

function checkIntergerValue(a_Value, a_FieldName, a_Message) {
  var fieldNameBlkJSX = getJsxByName(a_FieldName);
  if( a_Value == "") {
    alert(a_Message);
    fieldNameBlkJSX.focus();    
    return false;
  } else if( a_Value.match(/[^0-9]/) ) {
    alert(a_Message);
    fieldNameBlkJSX.focus();    
    return false;
  } else {
    fieldNameBlkJSX.setValue(parseInt(a_Value, 10));
    return true;
  }
//a_Value.match(/[^0-9]/)
}

// set default selected val in select
function SetDefaultInSelect(a_SelectName,FilterName){
  var valStreamSrcUser = PNameSpace.getJSXByName(a_SelectName).getValue().search(/FilterName /);
  if(valStreamSrcUser=="-1"){
    var objSelect = PNameSpace.getJSXByName(a_SelectName);
    if (objSelect) {
      var strXMLCacheId = objSelect.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      if (objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        for (var it = objRecordNodes.iterator();it.hasNext();){
          var objRecordNode = it.next();
          var strRecordName = objRecordNode.getAttribute("jsxtext");
          var strRecordid = objRecordNode.getAttribute("jsxid");
          if(strRecordName == FilterName){
	        PNameSpace.getJSXByName(a_SelectName).setValue(strRecordid);
            return ;
          }
        }
      }
    }
  }
}

function SetTagObjectType(UserInput,responseOutput,tagname){
  if(PNameSpace.getJSXByName(UserInput).getChecked()){
    return tagname;
  } else {
    return null;
  }
}


function SetRecordToMatrixAndSelect(a_ElementNIDOne, a_SelectNameOne , a_ElementNIDTwo , a_SelectNameTwo , a_Matrix ) {
  //chkForDuplicates(a_ElementNIDOne, a_SelectNameOne , a_ElementNIDTwo , a_SelectNameTwo , a_Matrix);/*debuging pending*/
  var FirstFilterName = getTagObjectFilterName(a_ElementNIDOne);

  var o1 = new Object();
  o1.jsxid = jsx3.xml.CDF.getKey() ;
  o1.jsxtextOneNID = a_ElementNIDOne;
  o1.jsxtextOne = FirstFilterName;
  // defined in logic.js
  SetDefaultInSelect(a_SelectNameOne,FirstFilterName);

  if(a_ElementNIDTwo!=""){
    var SecondFilterName = getTagObjectFilterName(a_ElementNIDTwo);
    o1.jsxAt="@";
    o1.jsxtextTwoNID = a_ElementNIDTwo;
    o1.jsxtextTwo = SecondFilterName;
    SetDefaultInSelect(a_SelectNameTwo,SecondFilterName);
  }
	
  PNameSpace.getJSXByName(a_Matrix).insertRecord(o1);
  PNameSpace.getJSXByName(a_Matrix).repaintData();
  
}

function splitNID(a_NID, a_SelectOneName, a_SelectTwoName, a_MatrixName) {
  var arrNID = a_NID.split(',');
  
  if(a_SelectTwoName != "null") {
    for(i=0;i<arrNID.length-1;i++) {
      var arrfilter = arrNID[i].split('@');
      var ElementNIDOne = arrfilter[0];
      var ElementNIDTwo = arrfilter[1];

      SetRecordToMatrixAndSelect(ElementNIDOne, a_SelectOneName , ElementNIDTwo , a_SelectTwoName , a_MatrixName );
    }
  } else {
    for(i=0;i<arrNID.length-1;i++) {
      var ElementNIDOne = arrNID[i];
      SetRecordToMatrixAndSelect(ElementNIDOne, a_SelectOneName , "" , "" , a_MatrixName );
    }
  }

}

function checkCheckBox(a_checkBoxName,responseOutput,responseValue){
  if(PNameSpace.getJSXByName(a_checkBoxName).getChecked()){
    return responseValue;
  } else {
    return false;
  }
}

function getUserRadioButtonValue(){
// radioUser_PolicyTypeAllow
// radioUser_PolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioUser_PolicyTypeAllow","radioUser_PolicyTypeDeny");
  return radioButtonVal;
}

function getUserPolicyRadioButtonValue(){
// radioUserPolicy_PolicyTypeAllow
// radioUserPolicy_PolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioUserPolicy_PolicyTypeAllow","radioUserPolicy_PolicyTypeDeny");
  return radioButtonVal;
}

function getCollabPolicyRadioButtonValue() {
// radioCollabPolicyTypeAllow
// radioCollabPolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioCollabPolicyTypeAllow","radioCollabPolicyTypeDeny");
  return radioButtonVal;
}

function getPolicyAdminPolicyRadioButtonValue() {
// radioPolicyAdminPolicyTypeAllow
// radioPolicyAdminPolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioPolicyAdminPolicyTypeAllow","radioPolicyAdminPolicyTypeDeny");
  return radioButtonVal;
}

function getCollabPolicyStreamFilterSpec() {
// radioCollabPolicyRestrictToList = RestrictToList
// radioCollabPolicySequence = ForceSequence
  return "";
  var radioButtonVal = getRadioButtonValue("radioCollabPolicyRestrictToList","radioCollabPolicySequence");
  if(radioButtonVal) {
    return "RestrictToList";
  } else {
    return "ForceSequence";
  }

}

function getAssetPolicyRadioButtonValue() {
 // radioAssetPolicyTypeAllow
// radioAssetPolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioAssetPolicyTypeAllow","radioAssetPolicyTypeDeny");
  return radioButtonVal;
}

function getMediaStorePolicyRadioButtonValue() {
// radioMediaStorePolicyTypeAllow
// radioMediaStorePolicyTypeDeny
  var radioButtonVal = getRadioButtonValue("radioMediaStorePolicyTypeAllow","radioMediaStorePolicyTypeDeny");
  return radioButtonVal;
}

function getRadioButtonValue(a_RadioOne, a_RadioTwo) {
  if(PNameSpace.getJSXByName(a_RadioOne).getSelected()){
    return true;
  } else if(PNameSpace.getJSXByName(a_RadioTwo).getSelected()){
    return false;
  }
}

function getSingleRadioButtonValue(a_RadioOne) {
  if(PNameSpace.getJSXByName(a_RadioOne).getSelected()){
    return true;
  }
  return false;
}

function groupTagObjectNameValueFromMatrix(a_Type, a_MatrixName) {

  var objMatrix = PNameSpace.getJSXByName(a_MatrixName);
  if (objMatrix) {
    var strXMLCacheId = objMatrix.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var TagName = objRecordNode.getAttribute("jsxtextTagNameSelected");
        var TagValue = objRecordNode.getAttribute("jsxtextTagValueSelected");
        if(a_Type == "TagName"){
          return TagName;
        } else if(a_Type == "TagValue"){
          return TagValue;
        }
      }
    }
  }

}


// Add Button function to move the tag name to selected matrix

function AddTagObjectTypeToMatrix(a_txtTagDefName, a_txtTagDefValue, a_AddToMatrix) {
// "txtUserAddTagDefName"
// "txtUserAddTagDefValue"

  var tagDefName = PNameSpace.getJSXByName(a_txtTagDefName).getValue();
  var tagDefValue = PNameSpace.getJSXByName(a_txtTagDefValue).getValue();


  if( tagDefName == "" ) {
    alert("Please enter Tag Type");
    return false;
  } else if( tagDefValue == "" ) {
    alert("Please enter Tag Value");
    return false;
  } else {
    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey() ;
    o.jsxtextOne = tagDefName;
    o.jsxtextTwo = tagDefValue;

    PNameSpace.getJSXByName(a_AddToMatrix).insertRecord(o, null, false);

    PNameSpace.getJSXByName(a_AddToMatrix).repaintData();
  }
}

function checkAddForTagDefName(a_SelectName) {
  var SelectTagName = PNameSpace.getJSXByName(a_SelectName).getValue();
  /*
  var matchResult = SelectTagName.search(/--Add /);

  if(matchResult != -1) {
    //cmpGroupTagName_btnAddOnClick(); //defined in cmpPolicyGroupTagName.js
    dlgEditGroupTag_Create("DEFAULT_GROUPTAG_NID");
  }
  */
  var selTagNode = PNameSpace.getJSXByName(a_SelectName).getRecordNode(SelectTagName);
  var selTagNID = selTagNode.getAttribute("NID");
  if( selTagNID == "DEFAULT_GROUPTAG_NID") {
    dlgEditGroupTag_Create("DEFAULT_GROUPTAG_NID");
  }
}

function checkAddForFilter(a_SelectName,a_FilterType) {
  var SelectTagName = PNameSpace.getJSXByName(a_SelectName).getValue();
  /*
  var matchResult = SelectTagName.search(/--Add /);
  topic_Set(tApp, "AddNewFilterType", a_FilterType);

  if(matchResult != -1) {
    //cmpFilterName_btnAddOnClick(); //defined in cmpPolicyFilterName.js
    dlgEditTagObjectFilter_Create("DEFAULT_TAGOBJECTFILTER_NID");
  }
  */
  var selTagNode = PNameSpace.getJSXByName(a_SelectName).getRecordNode(SelectTagName);
  var selTagNID = selTagNode.getAttribute("NID");
  if( selTagNID == "DEFAULT_TAGOBJECTFILTER_NID") {
    topic_Set(tApp, "AddNewFilterType", a_FilterType);
    dlgEditTagObjectFilter_Create("DEFAULT_TAGOBJECTFILTER_NID");
  }
}

function checkAddForStreamFilter(a_SelectName) {
  var SelectTagName = PNameSpace.getJSXByName(a_SelectName).getValue();
  /*
  var matchResult = SelectTagName.search(/--Add /);

  if(matchResult != -1) {
    //cmpStream_btnAddOnClick();  //defined in cmpPolicyStream.js
    dlgEditMediaStreamFilter_Create("DEFAULT_MEDIASTREAMFILTER_NID");
  }
  */
  var selTagNode = PNameSpace.getJSXByName(a_SelectName).getRecordNode(SelectTagName);
  var selTagNID = selTagNode.getAttribute("NID");
  if( selTagNID == "DEFAULT_MEDIASTREAMFILTER_NID") {
    dlgEditMediaStreamFilter_Create("DEFAULT_MEDIASTREAMFILTER_NID");
  }
}

function checkAddForStreamProfile(selNameBlkJSX) {
  var selProfile = selNameBlkJSX.getValue();

  var selProfileRecord = selNameBlkJSX.getRecordNode(selProfile);
  if( selProfileRecord == null ) return;
  var selProfileNID = selProfileRecord.getAttribute("jsxtextNID");
  if( selProfileNID == "DEFAULT_V2D-STREAMPROFILE_NID") {
    dlgEditProfile_Create("DEFAULT_V2D-STREAMPROFILE_NID");
  }
}


function chkForExistenceInSelect(a_SelElementName, a_SelElementNameNID, a_SelectName){
  var objSelect = getJsxByName(a_SelectName);
  if (objSelect) {
    var strXMLCacheId = objSelect.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordName = objRecordNode.getAttribute("jsxtext");
        var strRecordNID = objRecordNode.getAttribute("NID");
        if( (a_SelElementName == strRecordName) ) {
          return false;
        }
      }
    }
  }
  return true;
}


function splitTagName(tagName) {
  var matchResult = tagName.search(/@Name/);
  if( matchResult != -1 ) {
    var nameArr = tagName.split('\'');
    return nameArr[1];  
  } else {
    return tagName;
  }
}

function formatGroupTag(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = splitTagName(element.innerHTML);
}

function covertMilliSecondToHhMmSs(ms) {
  function two(x) {return ((x>9)?"":"0")+x}
  function three(x) {return ((x>99)?"":"0")+((x>9)?"":"0")+x}
  try {
    ms = ms.match(/\d{1,}/);
  } catch(e) {
  }
  var sec = Math.floor(ms/1000)
  ms = ms % 1000
  t = three(ms)

  var min = Math.floor(sec/60)
  sec = sec % 60
  //t = two(sec) + ":" + t
  // only ss no ms
  t = two(sec);

  var hr = Math.floor(min/60)
  min = min % 60
  t = two(min) + ":" + t
  
  var day = Math.floor(hr/60)
  hr = hr % 60
  t = two(hr) + ":" + t
  /*
  t = day + ":" + t
  */
  return t
}

function MediaFileTime(element, cdfkey, matrix, column, rownumber, server) {
  if (jsx3.util.strTrim(element.innerText) == "--") return;
  element.innerHTML = covertMilliSecondToHhMmSs(element.innerText);
}

function convertBytesToMegaBytes(bytes){
  var megaBytes = 0;
  if(bytes > 0) {
    var Cbytes = bytes.match(/\d{1,}/);
    megaBytes = Math.round(Cbytes/1048576);
  }
  return megaBytes;
}

//1GB = 1073741824 bytes
//convert storage units from bytes to GB ans round it for 2 digits after decimal
function convertBytesToGigaBytes(bytes){
  var gigaBytes = 0;
  if(bytes > 0) {
    var Cbytes = bytes.match(/\d{1,}/);
    gigaBytes = Math.round((Cbytes/1073741824)*100)/100;
  }
  return gigaBytes;
}

function MediaFileSize(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = convertBytesToMegaBytes(element.innerText);
}

      
function cmpDlgEditAssetSetDirtyBitValue(objBlkJSX) {
  var objParent = objBlkJSX.getParent();
  while( objParent != null ) {
    var objBlkJSXName = objBlkJSX.getName();
    var objParentName = objParent.getName();
      
    var ObjParentInstanceOf = objParent.getInstanceOf();
    if( (ObjParentInstanceOf == "jsx3.gui.Matrix.Column") &&
        (objBlkJSXName == objParentName ) ) {
      var objParentAncestorType = objParent.getAncestorOfType(jsx3.gui.Matrix);
      var selRecordID = objParentAncestorType.getValue();
      if( !jsx3.util.strEmpty(selRecordID) ) {
        var selRecordNode = objParentAncestorType.getRecordNode(selRecordID);
        if( !jsx3.util.strEmpty(selRecordNode) ) {
          var oldDirtyBit = selRecordNode.getAttribute("jsxDirtyBit");
          if( (oldDirtyBit == "CLEAN") || 
              (oldDirtyBit == "") ) {
            selRecordNode.setAttribute("jsxDirtyBit","UPDATELIST");
            objParentAncestorType.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
          }
        }
      }
    }
    
    var txtDirtyBitName = "txtDirtyBit" + objParentName;
    var objDirtyBit = objParent.getDescendantOfName(txtDirtyBitName, false, false);
    if( objDirtyBit != null ) {
      objDirtyBit.setValue("MODIFY");
    }
    
    objParent = objParent.getParent();
  }  
}

  function cmpDlgEditTagObjectFilter_TagNameSelect() {
    var SelectTagName = PNameSpace.getJSXByName("selPolicyFilterResource").getValue();
    var selTagNode = PNameSpace.getJSXByName("selPolicyFilterResource").getRecordNode(SelectTagName);
    var selTagNID = selTagNode.getAttribute("NID");
    if( selTagNID == "DEFAULT_GROUPTAG_NID") {
      dlgEditGroupTag_Create("DEFAULT_GROUPTAG_NID");
    } else {
      topic_Publish(tApp, "FilterTagDefNameSelected",selTagNID);
    }
  }
  
  function cmpProgram_btnStopOnCLick() {
    var roomNID = topic_Get(tApp, "ProgramSelected");
    if(roomNID != null) {
      var agree = confirm('Are you sure you want to delete this mediaroom? ');
      if(agree) {    
        var reqDeleteMediaRoom = new ReqDeleteMediaRoom(roomNID,
                                                        {action:"FNCCALL", context:this,
                                                         fnc:function(respCode, respVal){
                                                                      if(respCode == "SUCCESS" && respVal != null) {
                                                                        topic_Publish(tApp, "ProgramSelected",null);
                                                                      } else {
                                                                         var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                         var ErrorMsg = "Error: Mediaroom delete request failed \n";
                                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                         ErrorMsg += "Error code: " + respCode;
                                                                         alert(ErrorMsg);
                                                                      }
                                                                     }
                                                        }
                                                       );
        reqDeleteMediaRoom.sendRequest(this);        
      }    
    } else {
      alert("Please select mediaroom to delete");
      return false;
    }    
  }

  //function set the 1st value in the select to be selected
  function cmpSelect_Select1Value(a_SelectBoxName) {
    var selRecordID = "";
    var blkJSX = PNameSpace.getJSXByName(a_SelectBoxName);
    selRecordID = blkJSX.getValue();
    if( (selRecordID == null) || (selRecordID == "") ) {
      if (blkJSX) {
        var strXMLCacheId = blkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        if (objXML) {
          var objRecordNodes = objXML.selectNodes("//record");
          for (var it = objRecordNodes.iterator();it.hasNext();){
            var objRecordNode = it.next();
            var strRecordName = objRecordNode.getAttribute("jsxtext");
            var strRecordid = objRecordNode.getAttribute("jsxid");
	          blkJSX.setValue(strRecordid);
	          break;
	        }
        }
      }
      blkJSX.paint();         
    }
  }
  
  // function fill select with the device name and mediastoreportresource nid
  function cmpMediaSelMediaStoreNID_Repaint(a_SelectBoxName, mediaStorePortResourceNID) {
    //var selRecordID = null;
    PNameSpace.getJSXByName(a_SelectBoxName).resetCacheData();
    var assetResourceNIDListNode = topic_GetNode(tApp, "//AssetNIDList");
    if( assetResourceNIDListNode != null ) {
      var deviceListNode = getObjectCacheListNode("Device");
      var assetResourceNIDListIter = assetResourceNIDListNode.selectNodeIterator(".//ObjectNID");
      while( assetResourceNIDListIter.hasNext() ) {
        var assetResourceNID = assetResourceNIDListIter.next().getValue();
        var assetResourceNode = topic_GetNode(tCache, ".//*[@NID='" + assetResourceNID + "']");
        var assetType = assetResourceNode.selectSingleNode(".//Info/Type").getValue();
        var deviceTypeListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml");
        var dmsDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='DigitalMediaServer']/record[@jsxid='"+ assetType +"']");
        // get the MediastoreNID for the DMS's only
        if( !jsx3.util.strEmpty(dmsDeviceType) ) {
          var assetName = assetResourceNode.selectSingleNode(".//Info/Title").getValue();
          var AssetType = assetResourceNode.selectSingleNode(".//Info/Type").getValue();
          
          var DeviceWhereXPath = "[AssetResourceNID='" + assetResourceNID + "']";
          var deviceNode = deviceListNode.selectSingleNode("./" + AssetType + DeviceWhereXPath);
          if( deviceNode == null ) {
            cmpMediaGetDevice(a_SelectBoxName, assetName, AssetType, DeviceWhereXPath, mediaStorePortResourceNID);
          } else {
            var DeviceNID = deviceNode.getAttribute("NID");
            var mediaStorePortNID = deviceNode.selectSingleNode(".//ResourcePointers/MediaStorePortResourceNID/NIDValue").getValue();
            // if root dir selected then display all the devices otherwise
            // display only the selected mediaStorePortResourceNID devices
            if( mediaStorePortResourceNID == "DEFAULT") {
              cmpMediaInsertMediaStoreNID(a_SelectBoxName, mediaStorePortNID, assetName);
            } else if( mediaStorePortResourceNID == mediaStorePortNID) {
              cmpMediaInsertMediaStoreNID(a_SelectBoxName, mediaStorePortNID, assetName);
            }
          }
          
        }
      }
      PNameSpace.getJSXByName(a_SelectBoxName).paint();
      
    } 
    
    function cmpMediaGetDevice(a_SelectBoxName, assetName, AssetType, DeviceWhereXPath, mediaStorePortResourceNID) {

      var reqDeviceData = new ReqGetObjectWhere(AssetType, DeviceWhereXPath,
                                                  {action:"FNCCALL", context:this, 
                                                     fnc:function(respCode, respVal) {
                                                           if( respCode == "SUCCESS" && respVal != null ) {
                                                             var deviceListNode = getObjectCacheListNode("Device");
                                                             var deviceNode = deviceListNode.selectSingleNode("./" + AssetType + DeviceWhereXPath);
                                                             if( deviceNode != null ) {
                                                               var DeviceNID = deviceNode.getAttribute("NID");
                                                               var mediaStorePortNID = deviceNode.selectSingleNode(".//ResourcePointers/MediaStorePortResourceNID/NIDValue").getValue();
                                                               if( mediaStorePortResourceNID == "DEFAULT" ) {
                                                                 cmpMediaInsertMediaStoreNID(a_SelectBoxName,mediaStorePortNID, assetName);
                                                               }
                                                               else if( mediaStorePortResourceNID == mediaStorePortNID ) {
                                                                 cmpMediaInsertMediaStoreNID(a_SelectBoxName,mediaStorePortNID, assetName);   
                                                               }
                                                             } else {
                                                               alert("Internal error: Not able to find device media store port resources");
                                                             }
                                                           } else {
                                                             var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                             var ErrorMsg = "Error: Get MediaStorePort get data request failed \n";
                                                             ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                             ErrorMsg += "Error code: " + respCode;
                                                             alert(ErrorMsg);                                                           
                                                           }
                                                         }
                                                  } 
                                               );
      reqDeviceData.sendRequest();
    } // end cmpMediaGetDevice
    
    function cmpMediaInsertMediaStoreNID(a_SelectBoxName, mediaStorePortNID, assetName) {
      var o = new Object();
      o.jsxid = mediaStorePortNID;
      o.jsxtext = assetName;
      //selRecordID = o.jsxid;
      PNameSpace.getJSXByName(a_SelectBoxName).insertRecord(o, null, true);
      /*
      if(selRecordID != null) {
        PNameSpace.getJSXByName(a_SelectBoxName).setValue(selRecordID);
      }
      */
    }
  } //end  cmpMediaSelMediaStoreNID_Repaint
  

function cleanDirtyBitColumn(element, cdfkey, matrix, column, rownumber, server) {
  var matchResult = element.innerHTML.search(/CLEAN/);

  if(matchResult != -1) {
    element.innerHTML = "";
  } 
}

function validatePassword(a_Password , a_FieldName, a_Message) {
  if( a_Message == null) {
    a_Message = "Password format should be minimum 8 characters, at least 1 uppercase, 1 lowercase, 1 numeric";
  }
  
  var strPasswordExp = /^.*(?=.{8,})(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).*$/;
  if( !strPasswordExp.test(a_Password)) {
    alert(a_Message);
    getJsxByName(a_FieldName).focus();
    return false;
  } else {
    return true;
  }
}

function validateVerifyPassword(a_Password, a_VerifyPassword, a_FieldName, a_Message) {
  if( a_Message == null) {
    a_Message = "Passwords do not match. Please enter again";
  }
  
  if( a_Password == a_VerifyPassword ) {
    return true;
  } else {
      alert(a_Message);
      getJsxByName(a_FieldName).focus();
      return false;
  }

}

function getClock(epoch) {
  var time = new Date(epoch);
  var hr = time.getHours();
  var min = time.getMinutes();
  var sec = time.getSeconds();
  var ampm = " PM"
  if (hr < 12){
    ampm = " AM"
    }
  if (hr > 12){
    hr -= 12
    }
  if(hr < 10){
    hr = " " + hr
    }
  if(min < 10){
    min = "0" + min
    }
  if(sec < 10){
    sec = "0" + sec
    } 
  var retTime = hr + ":" + min + ":" + sec ;
  return retTime;
}

function calculateDuration(matrix, cdfkey) {
  var selRecord = matrix.getRecord(cdfkey);
  var creationTime = selRecord.jsxCreationTime;

  var currentTime = new Date().getTime();
  var deltaTime = topic_Get(tApp, "DeltaServerLocalTime");
  var serverTime = parseInt(currentTime) - parseInt(deltaTime);

  var duarationTime = 0;
  if( parseInt(serverTime) > parseInt(creationTime) ) {
    duarationTime = parseInt(serverTime) - parseInt(creationTime);
  } else {
    duarationTime = parseInt(creationTime) - parseInt(serverTime);
  } 
  var duration = covertMilliSecondToHhMmSs(parseInt(duarationTime)); 
  //duration = "00:00:00";
  return duration;
}

function showDuration(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = calculateDuration(matrix, cdfkey);
}

function setCreatedTimeCodeToDateTime(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = getDateTimeFromTimeCode(element.innerText);
}

function GetspyGlassConatinerDIV(a_left, a_top, a_width, a_height) {
  var lef = a_left + "px";
  var top = a_top + "px";
  var wid = a_width + "px";
  var hei = a_height + "px";
  var spyGlassConatinerDIV = '<div style="top:'+ top +';left:'+ lef +';width:600px;height:200px;border: solid 2px #e5a902;">';
  return spyGlassConatinerDIV;
}


//var spyGlassConatinerDIV = '<div style="width:500px;height:200px;border: solid 2px #e5a902;">';
var spyGlassConatinerDIVClose = '</DIV>';
var spyGlassStartDIV = '<div style="overflow:auto;border:solid 2px #ffffff;background-image:url(images/panels/policyDetailsBG.jpg);padding:0px 8px 10px 8px">';
var spyGlassStartDIVClose = '</DIV>';
var spyGlassTableDIV = '<DIV style="float:left; margin:8px 8px 8px 8px; border: solid 1px #cccccc; #ccc;background-image:url(images/panels/H4BG.png);background-repeat:repeat-x;">';
var spyGlassTableDIVClose = '</DIV>';
var spyGlassHeaderDIV = '<DIV style="height:17px;padding:0px 0px 0px 5px;color: #666666; font-family:Geneva, Arial, Helvetica, sans-serif;font-size: 11px; font-weight:bold;">';
var spyGlassHeaderDIVClose = '</DIV>';
var spyGlassRowContainerDIV = '<DIV style="background-color:#f5f5f5;border-top: solid 1px #cccccc;font-size:10px;font-weight:bold;padding:4px 4px 4px 4px;">';
var spyGlassRowContainerDIVClose = '</DIV>';
var spyGlassRowEnabledSpan = '<span>';
var spyGlassRowEnabledSpanClose = '</span>';
var spyGlassRowDisabledSpan = '<span style="color:#cccccc;">';
var spyGlassRowDisabledSpanClose = '</span>';
var spyGlassSpanNewLine = '<br />';


//moved to common area
/*function getJsxByName(BlkName) {
  var BlkJSX = null;
  if( BlkName != null ) {
    if( BlkName == null ) BlkName = BlkName;
    // get JSX handle from blkname
    if( BlkJSX == null ) {
      // allow an array to specify hierarchy
      // CAVEAT: Not all javascript strings are 'String'
      // when in doubt - caller should use array form of input
      if( typeof BlkName == 'string' ) {
        BlkJSX = PNameSpace.getJSXByName(BlkName);
      }
      else {
        var blkNameArr = BlkName;
        for( var i=0; i<blkNameArr.length; i++ ) {
          if( typeof blkNameArr[i] != 'string' ) {
            BlkJSX = blkNameArr[i];
          }
          else {
            if( BlkJSX == null ) {
              BlkJSX = PNameSpace.getJSXByName(blkNameArr[i]);
            }
            else {
              BlkJSX = BlkJSX.getDescendantOfName(blkNameArr[i], false, false);
            }
          }
        }
      }
    }
    return BlkJSX;
  }
}*/

function deleteEmptyGroupTag(dataNode) {
  if( jsx3.util.strEmpty(dataNode) ) return;
  var TagXPath = ".//*/Tag[@Name='' and @Value='']";
  var emptyGroupTagNodes = dataNode.selectNodes(TagXPath);
  for (var it = emptyGroupTagNodes.iterator();it.hasNext();){
    var emptyGroupTagNode = it.next();  
    var parentTagNode = emptyGroupTagNode.getParent();
    parentTagNode.removeChild(emptyGroupTagNode);
  }
}

function deleteEmptyTableConfig(dataNode) {
  try {
    var hostEntryXPath = ".//HostEntry[.//Name='' and .//IPAddress='']";
    var hostNodes = dataNode.selectNodes(hostEntryXPath);
    for (var it = hostNodes.iterator();it.hasNext();){
      var hostNode = it.next();    
      var parentnameNode = hostNode.getParent();
      parentnameNode.removeChild(hostNode);
    }
    
    var natEntryXPath = ".//NATZoneEntry[.//Name='' and .//IPAddress='']";
    var natNodes = dataNode.selectNodes(natEntryXPath);
    for (var it = natNodes.iterator();it.hasNext();){
      var natNode = it.next();     
      var parentnameNode = natNode.getParent();
      parentnameNode.removeChild(natNode);
    }
  }
  catch(e){
  }
}

function closeDialogIfExists(dialogName) {
  if (jsx3.gui.Dialog){
    try {
      var dialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
      for (var i = 0; i < dialogBoxArr.length; i++) {
        var lcDlgName = dialogBoxArr[i].getName();
        var windowBarTitle = dialogBoxArr[i].getDescendantOfName("windowBar").getText();
        if( lcDlgName == dialogName || windowBarTitle == dialogName) {
          //If we use dialogBoxArr[i].doClose(), dialog is closing but it is not calling destory method
          dialogBoxArr[i].getDescendantOfName("btnDialogCloseX").doExecute();
        }
      }
    } catch (e) {
      log.warn("Dialog error:" + e.description);
    }
  }
}


// Checking if Any Change has been made to the Group Tags 
function changeInGroupTags(objNode,mtxGroupTagListBlkJSX){
  if( objNode != null) {
    var orgGroupNode = null;
    var nodeName =  objNode.getNodeName();
    if( nodeName == "Groups" ){
      orgGroupNode = objNode;
    }
    else {
      orgGroupNode = objNode.selectSingleNode(".//Info/Groups");
    }
    var recordIds = mtxGroupTagListBlkJSX.getRecordIds();
    var tagItr = orgGroupNode.selectNodeIterator(".//Tag");
    var countOrgTag = 0;
    while(tagItr.hasNext()){
      countOrgTag ++;
      tagItr.next();
    }
    //If number of Tags differ to Original Tags on Device  
    if(countOrgTag != recordIds.length){
      return true ;
    }
    else{
      var change = false;
      for( var i=0 ; i < recordIds.length ; i++){
        var id = recordIds[i];
        var record = mtxGroupTagListBlkJSX.getRecord(id);
        var tagName = record.jsxTagName;
        var tagValue = record.jsxTagValue;
        var orgTag = null;
        orgTag = orgGroupNode.selectSingleNode(".//Tag[@Name='"+ tagName +"' and @Value='" + tagValue +"']");
        //If number of Tags are the same but values are mismatched to the original.
        if(jsx3.util.strEmpty(orgTag)){
          change = true;
          break;
        }
      }
      return change ;
    }
  }
}

//Update Read policy if all others are marked
function updateReadPolicy(parentBlk, chkBox1, chkBox2, chkBox3, chkBox4, chkBox5){
  var chk1 = getJsxByName([parentBlk, chkBox1]).getChecked();
  var chk2 = getJsxByName([parentBlk, chkBox2]).getChecked();
  var chk3 = getJsxByName([parentBlk, chkBox3]).getChecked();
  var chk4 = getJsxByName([parentBlk, chkBox4]).getChecked();
  var chk5 = false;
  if( !jsx3.util.strEmpty(chkBox5) ){
    chk5 = getJsxByName([parentBlk, chkBox5]).getChecked();
  }
  if(chk1 || chk2 || chk3 || chk5 ){
    getJsxByName([parentBlk, chkBox4]).setChecked(jsx3.gui.CheckBox.CHECKED);
  }
  else{
    getJsxByName([parentBlk, chkBox4]).setChecked(jsx3.gui.CheckBox.UNCHECKED);
  }
}

//check if atleast one of the permissions is selected for policy to exist
function checkPermissionSelect(parentBlk,  alertMessage, chkBoxArray){
  var i = 0;
  while(i < chkBoxArray.length){
    var blk = parentBlk.getDescendantOfName(chkBoxArray[i]);
    var chk1 = blk.getChecked();
    if(chk1 == jsx3.gui.CheckBox.UNCHECKED){
      i++;
    }
    else{
      return true;
    }
  }
  alert(alertMessage);
  return false;  
}