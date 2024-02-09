// globals variables
var btnGetXMLBlkJSX = "";
var btnSaveXMLBlkJSX = "";
var btnClearXMLBlkJSX = "";
var btnGetAllXMLBlkJSX = "";
var btnGetAllCurrentXMLBlkJSX = "";
var selXSDBlkJSX = "";
var txtAreaXMLBlkJSX = "";
var txtXMLNameBlkJSX = "";
var txtFolderNameBlkJSX = "";
var txtRuleFileBlkJSX = "";
var btnFileBrowseBlkJSX = "";
var btnSelectDirBlkJSX = "";
var selXSDVersionBlkJSX = "";
var selAllXSDVersionBlkJSX = "";
var XMLData = null;
var showXMLData = false;
var xsdObjectArr = [];
var ruleFilesArr = [];
var rulesFolderPath = null;
var versionSelected = null;
var IPVSDefualtFilesDir = null;
var debugMode = false;
var TextAreaText = true;

// init is set in the project settings onload
function init() {
  txtRuleFileBlkJSX = xsdTOxml.getJSXByName("txtRuleFile");
  btnFileBrowseBlkJSX = xsdTOxml.getJSXByName("btnFileBrowse");
  btnSelectDirBlkJSX = xsdTOxml.getJSXByName("btnSelectDir");

  btnGetXMLBlkJSX = xsdTOxml.getJSXByName("btnGetXML");
  btnSaveXMLBlkJSX = xsdTOxml.getJSXByName("btnXMLSave");
  btnClearXMLBlkJSX = xsdTOxml.getJSXByName("btnXMLClear");
  btnGetAllXMLBlkJSX = xsdTOxml.getJSXByName("btnGetAllXML");
  btnGetAllCurrentXMLBlkJSX = xsdTOxml.getJSXByName("btnGetAllCurrentXML");
  
  selXSDBlkJSX = xsdTOxml.getJSXByName("selXSD");
  selXSDVersionBlkJSX = xsdTOxml.getJSXByName("selXSDVersion");
  selAllXSDVersionBlkJSX = xsdTOxml.getJSXByName("selAllXSDVersion");   
  
  txtAreaXMLBlkJSX = xsdTOxml.getJSXByName("txtAreaXML");
  txtXMLNameBlkJSX = xsdTOxml.getJSXByName("txtXMLName");
  txtFolderNameBlkJSX = xsdTOxml.getJSXByName("txtFolderName");

  btnFileBrowseBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnFileBrowseBlkJSX.setEvent("doBrowse()", jsx3.gui.Interactive.EXECUTE);
  
  btnSelectDirBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnSelectDirBlkJSX.setEvent("doBrowseDir()", jsx3.gui.Interactive.EXECUTE);
  
  selXSDBlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
  selXSDBlkJSX.setEvent("fillVersionSelect()", jsx3.gui.Interactive.SELECT);
  
  btnGetXMLBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnGetXMLBlkJSX.setEvent("generateXML()", jsx3.gui.Interactive.EXECUTE);
  
  btnClearXMLBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnClearXMLBlkJSX.setEvent("clearXMLArea()", jsx3.gui.Interactive.EXECUTE);
  
  btnSaveXMLBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnSaveXMLBlkJSX.setEvent("saveXML()", jsx3.gui.Interactive.EXECUTE);
  
  btnGetAllXMLBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnGetAllXMLBlkJSX.setEvent("getAllXML()", jsx3.gui.Interactive.EXECUTE);
  
  btnGetAllCurrentXMLBlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
  btnGetAllCurrentXMLBlkJSX.setEvent("getAllCurrentXML()", jsx3.gui.Interactive.EXECUTE);

}

// function called when browse button is clicked. Open browse window
function doBrowse() { 
  jsx3.require("jsx3.net.Form");
  var strFieldName = "txtRuleFile";
  var objFile = null;
  if(!objFile) {
    objFile = new jsx3.net.Form( );
    objFile.addFileUploadField(strFieldName);
    objFile.subscribe(jsx3.net.Form.EVENT_FILE_SELECTED, onFileSelected);
  }
  objFile.promptForFile(strFieldName);
}

// fills the text box with the selected file
function onFileSelected(objEvent) { 
  //alert(objEvent.value);  
  txtRuleFileBlkJSX.setValue(objEvent.value);
  doSelect(); 
}

function doBrowseDir(){
  jsx3.require("jsx3.net.Form");
  var strFieldName = "txtFolderName";
  var objFile = null;
  if(!objFile) {
    objFile = new jsx3.net.Form( );
    objFile.addFileUploadField(strFieldName);
    objFile.subscribe(jsx3.net.Form.EVENT_FILE_SELECTED, onFileSelected1);
  }
  objFile.promptForFile(strFieldName);
}

// fills the text box with the selected file
function onFileSelected1(objEvent) { 
  //alert(objEvent.value);  
  txtFolderNameBlkJSX.setValue(objEvent.value);
  setSaveDirPath();
}

function setSaveDirPath() {
  var FSO;  
  FSO = new ActiveXObject("Scripting.FileSystemObject");
  
  var FilePath = txtFolderNameBlkJSX.getValue();
  
  if(FSO.FileExists(FilePath)) {
   IPVSDefualtFilesDir = FSO.GetParentFolderName(FilePath);
  } else {
    alert("Selected Dir does not exists");
    return false;
  }  
}

// function called when select button is clicked. It will check for the selected file
// If matches then gets the folder path and gets all the files in it.
// Fills the oxsd/bject drop down and also fills the allversion drop down
function doSelect() {
  var FSO;  
  FSO = new ActiveXObject("Scripting.FileSystemObject");
  
  var FolderPath = txtRuleFileBlkJSX.getValue();
  
  if(FSO.FileExists(FolderPath) && FSO.GetFileName(FolderPath) == "PathToRulesDir.xml") {
   rulesFolderPath = FSO.GetParentFolderName(FolderPath);
   //alert(rulesFolderPath);
   getRuleFiles();
   fillXSDSelect();
   fillAllVersionSelect();
  } else {
    alert("Invalid file selected");
    return false;
  }  
}

function getRuleFiles() {
  var FSO, f, fc;
  FSO = new ActiveXObject("Scripting.FileSystemObject");
  f = FSO.GetFolder(rulesFolderPath);
  fc = new Enumerator(f.files);
  for (; !fc.atEnd(); fc.moveNext())
  {
    ruleFilesArr.push(fc.item().Name);
  }
}

// fill the xsd objects in the drop down
function fillXSDSelect() {
  selXSDBlkJSX.resetCacheData();
  for(var i=0;i<ruleFilesArr.length;i++) {
    var fName = ruleFilesArr[i];
    if( (fName.match("\.xml") != null) && (fName.match("-v") != null) ) {
      var sFName = fName.split("-v",1);
      if(checkInsert(selXSDBlkJSX, sFName)) {
        var o = new Object;
        o.jsxid = sFName;
        o.jsxtext = sFName;
        selXSDBlkJSX.insertRecord(o);
        
        xsdObjectArr.push(sFName);       
      }
    }
  }
  var o = new Object;
  o.jsxid = "Select_ObjecttogenerateXML";
  o.jsxtext = "Select Object to generate XML";
  selXSDBlkJSX.insertRecord(o);   
  selXSDBlkJSX.setValue(o.jsxid);
  selXSDBlkJSX.paint();
}

// check for duplicate in the drop down
function checkInsert(selBlkJSX, checkName) {
  if (selBlkJSX) {
    var strXMLCacheId = selBlkJSX.getXMLId();
    var objXML = xsdTOxml.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordName = objRecordNode.getAttribute("jsxtext");
        if( (checkName == strRecordName) ) {
          return false;
        }
      }
    }
  }
  return true;
}

function fillVersionSelect() {
  versionSelected = null;
  var RuleSelected = selXSDBlkJSX.getValue();
  selXSDVersionBlkJSX.resetCacheData();
  selXSDVersionBlkJSX.setValue("");
  for(var i=0;i<ruleFilesArr.length;i++) {
    var fName = ruleFilesArr[i];
    if( (fName.match("\.xml") != null) && (fName.match("-v") != null) ) {
      var sFName = fName.split("-v");
      if(RuleSelected == sFName[0]) {
        var veName = sFName[1].split("\.xml",1).toString();
        var vName = "";
        var vNameID = veName.replace(/\./g,"_");
        if( veName.match("-Head") != null ){
          vName = veName.split("-Head",1);
        } else {
          vName = veName;
        }
        var o = new Object;
        o.jsxid = vNameID;
        o.jsxtext = vName;
        selXSDVersionBlkJSX.insertRecord(o);     
      }
    }
  }
  var o = new Object;
  o.jsxid = "Select_ObjectVersion";
  o.jsxtext = "Select Object Version";
  selXSDVersionBlkJSX.insertRecord(o);
  selXSDVersionBlkJSX.setValue(o.jsxid);  
  selXSDVersionBlkJSX.paint();  
}

function fillAllVersionSelect() {
  versionSelected = null;
  selAllXSDVersionBlkJSX.resetCacheData();
  for(var i=0;i<ruleFilesArr.length;i++) {
    var fName = ruleFilesArr[i];
    if( (fName.match("\.xml") != null) && (fName.match("-v") != null) ) {
      var sFName = fName.split("-v");
      var veName = sFName[1].split("\.xml",1).toString();
      var vName = "";
      var vNameID = veName.replace(/\./g,"_");;
      if( veName.match("-Head") != null ){
        vName = veName.split("-Head",1);
      } else {
        vName = veName;
      }      
      if(checkInsert(selAllXSDVersionBlkJSX, vName)) {
        var o = new Object;
        o.jsxid = vNameID;
        o.jsxtext = vName;
        selAllXSDVersionBlkJSX.insertRecord(o);     
      }
    }
  }
  var o = new Object;
  o.jsxid = "Select_ObjectVersion";
  o.jsxtext = "Select Object Version";
  selAllXSDVersionBlkJSX.insertRecord(o); 
  selAllXSDVersionBlkJSX.setValue(o.jsxid); 
  selAllXSDVersionBlkJSX.paint();  
}

// this func gets executed when getxml button is clicked
function generateXML() {
  // clears xml area
  clearXMLArea();
  // setting to true so that the XML data is shown in the text area
  showXMLData = true;
  //select rule
  var ObjectName = selXSDBlkJSX.getValue();
  if(debugMode) {
    ObjectName = "MPEGTSMediaStreamPRofile";
  }
  if(ObjectName.match("Select_") != null) {
    alert("Please select xsd/Object");
    return false;
  }
  
  // select version
  var ObjectVersion = selXSDVersionBlkJSX.getValue();
  if(debugMode) {
    ObjectVersion = "1_3-Head";
  }   
  if(ObjectVersion.match("Select_") != null) {
    alert("Please select version");
    return false;  
  }

  versionSelected = ObjectVersion.replace(/_/g,"\.");
  var RuleName = ObjectName +"-v" + ObjectVersion;
  // runs rule to generate xml 
  utilRuleGetOutboundDoc(ObjectName, RuleName);    
}

// function generate the xml takes rule name as input
function utilRuleGetOutboundDoc(ObjectName, RuleName) {
  // sub functions START
  function setNillable(childNode) {
    childNode.setAttribute("jsxSelectMask","Script");
    childNode.setAttribute("jsxTextMask","true");
    childNode.setAttribute("jsxComboMask","nillable");  
  }
  
  function setParseTrue(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"true\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);   
  }
  
  function setParseFalse(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"false\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }
  
  function setNIDSchemeAuto(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"AUTO\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);   
  }
  
  function setNIDSchemeElementName(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"PARENTNID.ELEMENTNAME\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }
  
  function setNIDSchemeKeyVal(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"PARENTNID.KEYVAL\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }  
  
  function setDirtyClean(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"CLEAN\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);   
  }
  
  function setRevision(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"1\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);    
  }
  
  function setVersion(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\""+ versionSelected +"\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);    
  }  

  function setSynchedFalse(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"false\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }
  
  function setQueryable(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"true\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }
  
  function setState(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"OK\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }

  function setListIndex(childNode) {
    var mappingNode = childNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "mappings");
    mappingNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      
    var mappingRecordNode = mappingNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
    mappingRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
    mappingRecordNode.setAttribute("name", "Script");
    mappingRecordNode.setAttribute("value", "setValue(\"1\")\;");
    mappingNode.appendChild(mappingRecordNode);
    
    childNode.appendChild(mappingNode);  
  }
  
  // sub functions END

  // main code STARTS here
  var objService = xsdTOxml.loadResource(RuleName + "_xml");
  if(objService == null) {
    alert("Rule not found : " + RuleName );
    return false;
  }
  var RuleNameDoc = objService.getRulesXML();
 
 // go thru the tibco rule file. editing rule on the fly
 //adding nibble to each element
  var elementIter = RuleNameDoc.selectNodeIterator(".//record");
  while(elementIter.hasNext()) {
    var elementNode = elementIter.next();
    var jsxtext = elementNode.getAttribute("jsxtext"); 
    var type = elementNode.getAttribute("type");
    if((type == "E")) {
      elementNode.setAttribute("jsxComboMask","nillable");
      elementNode.setAttribute("jsxTextMask","true");
      elementNode.setAttribute("jsxSelectMask","Script");

      var restrictionsNode = elementNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "restrictions");
      restrictionsNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
        
      var restrictionsRecordNode = restrictionsNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "record");
      restrictionsRecordNode.setAttribute("jsxid", jsx3.xml.CDF.getKey());
      restrictionsRecordNode.setAttribute("name", "nillable");
      restrictionsRecordNode.setAttribute("value", "true");
      restrictionsNode.appendChild(restrictionsRecordNode);
      
      elementNode.appendChild(restrictionsNode);       
      
    } else if( (type == "A") &&
        ( (jsxtext == "ElementNID") ||
          (jsxtext == "ElementNID-NoParse") ||
          (jsxtext == "Dirty") ||
          (jsxtext == "AutoNID") ||
          (jsxtext == "AutoNID-NoParse") ||
          (jsxtext == "Device-Revision") ||
          (jsxtext == "ListIndex") ||
          (jsxtext == "NoParse") ||
          (jsxtext == "Queryable") ||
          (jsxtext == "State") ||
          (jsxtext == "KeyNID") ||
          (jsxtext == "Version")        
        )) { 
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {   
              case "Rev":
                setNillable(childNode);
                setRevision(childNode);                          
                break;
              case "Version":
                //setVersion(childNode);                          
                break;                                                                                        
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode);
          
        }
        elementParentNode.removeChild(elementNode);  
    } else if( (type == "C") && (jsxtext == "ElementNID") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseTrue(childNode);                           
                break;
              case "NIDScheme":
                setNillable(childNode);
                setNIDSchemeElementName(childNode);                           
                break;     
              case "NID":
                setNillable(childNode);                       
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        } 
        elementParentNode.removeChild(elementNode);   
    } else if( (type == "C") && (jsxtext == "ElementNID-NoParse") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseFalse(childNode);                           
                break;
              case "NIDScheme":
                setNillable(childNode);
                setNIDSchemeElementName(childNode);                           
                break;     
              case "NID":
                setNillable(childNode);                       
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }  
        elementParentNode.removeChild(elementNode);  
    } else if( (type == "C") && (jsxtext == "Dirty") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {   
              case "Dirty":
                setNillable(childNode);
                setDirtyClean(childNode);                           
                break;                                                                            
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }  
        elementParentNode.removeChild(elementNode);  
    } else if( (type == "C") && (jsxtext == "AutoNID") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseTrue(childNode);                           
                break;
              case "NIDScheme":
                setNillable(childNode);
                setNIDSchemeAuto(childNode);                           
                break;     
              case "NID":
                setNillable(childNode);                       
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }  
        elementParentNode.removeChild(elementNode);  
    } else if( (type == "C") && (jsxtext == "AutoNID-NoParse") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseFalse(childNode);                           
                break;
              case "NIDScheme":
                setNillable(childNode);
                setNIDSchemeAuto(childNode);                           
                break;     
              case "NID":
                setNillable(childNode);                       
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }  
        elementParentNode.removeChild(elementNode);  
    } else if( (type == "C") && (jsxtext == "Device-Revision") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {   
              case "Rev":
                setNillable(childNode);
                setRevision(childNode);                          
                break; 
              case "Synched":
                setNillable(childNode);
                setSynchedFalse(childNode);                           
                break;                                                                                        
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        } 
        elementParentNode.removeChild(elementNode);   
    } else if( (type == "C") && (jsxtext == "ListIndex") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "ListIndex":
                setListIndex(childNode);                           
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        } 
        elementParentNode.removeChild(elementNode);    
    } else if( (type == "C") && (jsxtext == "NoParse") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseFalse(childNode);                           
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }   
        elementParentNode.removeChild(elementNode);   
    } else if( (type == "C") && (jsxtext == "Queryable") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Queryable":
                setQueryable(childNode);                           
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }
        elementParentNode.removeChild(elementNode);      
    } else if( (type == "C") && (jsxtext == "State") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "State":
                setState(childNode);                           
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }
        elementParentNode.removeChild(elementNode);      
    }else if( (type == "C") && (jsxtext == "KeyNID") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Parse":
                setNillable(childNode);
                setParseTrue(childNode);                           
                break;
              case "NIDScheme":
                setNillable(childNode);
                setNIDSchemeKeyVal(childNode);                           
                break;     
              case "NID":
                setNillable(childNode);                       
                break;                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        } 
        elementParentNode.removeChild(elementNode);   
    } else if( (type == "C") && (jsxtext == "Version") ) {
        var elementParentNode = elementNode.getParent();
        var childIter = elementNode.getChildIterator(true);
        while(childIter.hasNext()) {
          var childNode = childIter.next().cloneNode(true);
          try {          
            var jsxtextAttr = childNode.getAttribute("jsxtext");
            switch(jsxtextAttr) {
              case "Version":
                //setVersion(childNode);                          
                break;                                                                                         
              default:
                break;
            }
          } catch(e) {
          }
          elementParentNode.insertBefore(childNode,elementNode); 
        }
        elementParentNode.removeChild(elementNode);    
    }
    
  }

  // now runs the edited rule
  var ruleXML1 = objService.getRulesXML();
  objService.setOperation("");
  var ruleXML = objService.getServiceMessage();
  
  // format the generated xml
  var xslfile = xsdTOxml.loadInclude("xml/formatXML.xsl","formatXML","xsl");
  var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
  ruleXML = xslDoc.transform(ruleXML);

  var newDoc = new jsx3.xml.Document();
  newDoc.loadXML(ruleXML);
  // if its V2D-Tx then add V2DCodecVideoLookupTable defaults
  // if its V2D-Rx then add 2DDecoderVideoLookupTable defaults
  if( (ObjectName == "V2D-Tx-Device") ||
      (ObjectName == "V2D-Rx-Device") ||
      (ObjectName == "V2D-XP200-Device") ||
      (ObjectName == "V2D-XP200S-Device") ||
      (ObjectName == "V2D-XP220-Device") ||
      (ObjectName == "V2D-XP220S-Device") ||
      (ObjectName == "V2D-XP100-Device") ||
      (ObjectName == "V2D-XPi-Device") ||
      (ObjectName == "ORB-500-Device")) {
    var defaultVideoLookupXML = "";
    var LookupVal = "";  
    if( (ObjectName == "V2D-Tx-Device") ) {
      defaultVideoLookupXML = "Tx-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( ObjectName == "V2D-Rx-Device" ) {
      defaultVideoLookupXML = "Rx-VideoLookUpTable";
      LookupVal = "V2DDecoderVideoLookupTable"; 
    } else if( (ObjectName == "V2D-XP200-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "V2D-XP200S-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "V2D-XP220-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "V2D-XP220S-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "V2D-XP100-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "V2D-XPi-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    } else if( (ObjectName == "ORB-500-Device") ) {
      defaultVideoLookupXML = "XP200-VideoLookUpTable";
      LookupVal = "V2DCodecVideoLookupTable";
    }            
    var VideoLookupDeafultsDoc = xsdTOxml.loadInclude("xml/" + defaultVideoLookupXML + ".xml", + LookupVal , "xml", true);
   /*
    var CodecLookupNode = newDoc.selectSingleNode(".//" + LookupVal);
    var ParentNode = CodecLookupNode.getParent();
    ParentNode.replaceNode(LookupDeafultsNode, CodecLookupNode); 
   */
    var LookupDeafultsNodeIter = newDoc.selectNodeIterator(".//" + LookupVal);
    if( !LookupDeafultsNodeIter.hasNext() ) {
    } 
    else {
      while( LookupDeafultsNodeIter.hasNext() ) {
        var LookupDeafultsNode = VideoLookupDeafultsDoc.selectSingleNode("//" + LookupVal ).cloneNode(true);
        var CodecLookupNode = LookupDeafultsNodeIter.next();
        var ParentNode = CodecLookupNode.getParent();
        ParentNode.replaceNode(LookupDeafultsNode, CodecLookupNode);
      }
    } 
   
  
  }
  
  if( (ObjectName == "MRSchema") ) {
      var userExistdbXML = "UserListExistDB";
      var userExistVal = "UserList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + userExistdbXML + ".xml", + userExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+userExistVal).cloneNode(true);
      var UserListNode = newDoc.selectSingleNode(".//"+userExistVal);
      var ParentNode = UserListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, UserListNode); 
      var AssetExistdbXML = "AssetResourceListExistDB";
      var AssetExistVal = "AssetResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + AssetExistdbXML + ".xml", + AssetExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+AssetExistVal).cloneNode(true);
      var AssetResourceListNode = newDoc.selectSingleNode(".//"+AssetExistVal);
      var ParentNode = AssetResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, AssetResourceListNode); 
      var DeviceExistdbXML = "DeviceListExistDB";
      var DeviceExistVal = "DeviceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + DeviceExistdbXML + ".xml", + DeviceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+DeviceExistVal).cloneNode(true);
      var DeviceListNode = newDoc.selectSingleNode(".//"+DeviceExistVal);
      var ParentNode = DeviceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, DeviceListNode); 
      var MediaStreamDirResourceExistdbXML = "MediaStreamDirResourceListExistDB";
      var MediaStreamDirResourceExistVal = "MediaStreamDirResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamDirResourceExistdbXML + ".xml", + MediaStreamDirResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamDirResourceExistVal).cloneNode(true);
      var MediaStreamDirResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamDirResourceExistVal);
      var ParentNode = MediaStreamDirResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamDirResourceListNode); 
      var MediaStreamFileResourceExistdbXML = "MediaStreamFileResourceListExistDB";
      var MediaStreamFileResourceExistVal = "MediaStreamFileResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamFileResourceExistdbXML + ".xml", + MediaStreamFileResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamFileResourceExistVal).cloneNode(true);
      var MediaStreamFileResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamFileResourceExistVal);
      var ParentNode = MediaStreamFileResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamFileResourceListNode); 
      var BookmarkExistdbXML = "BookmarkListExistDB";
      var BookmarkExistVal = "BookmarkList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + BookmarkExistdbXML + ".xml", + BookmarkExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+BookmarkExistVal).cloneNode(true);
      var BookmarkListNode = newDoc.selectSingleNode(".//"+BookmarkExistVal);
      var ParentNode = BookmarkListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, BookmarkListNode); 
      var MediaStorePortResourceExistdbXML = "MediaStorePortResourceListExistDB";
      var MediaStorePortResourceExistVal = "MediaStorePortResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStorePortResourceExistdbXML + ".xml", + MediaStorePortResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStorePortResourceExistVal).cloneNode(true);
      var MediaStorePortResourceListNode = newDoc.selectSingleNode(".//"+MediaStorePortResourceExistVal);
      var ParentNode = MediaStorePortResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStorePortResourceListNode); 
      var MediaStreamDstPortResourceExistdbXML = "MediaStreamDstPortResourceListExistDB";
      var MediaStreamDstPortResourceExistVal = "MediaStreamDstPortResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamDstPortResourceExistdbXML + ".xml", + MediaStreamDstPortResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamDstPortResourceExistVal).cloneNode(true);
      var MediaStreamDstPortResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamDstPortResourceExistVal);
      var ParentNode = MediaStreamDstPortResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamDstPortResourceListNode); 
      var MediaStreamSrcPortResourceExistdbXML = "MediaStreamSrcPortResourceListExistDB";
      var MediaStreamSrcPortResourceExistVal = "MediaStreamSrcPortResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamSrcPortResourceExistdbXML + ".xml", + MediaStreamSrcPortResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamSrcPortResourceExistVal).cloneNode(true);
      var MediaStreamSrcPortResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamSrcPortResourceExistVal);
      var ParentNode = MediaStreamSrcPortResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamSrcPortResourceListNode); 
      var MediaStreamRelayPortResourceExistdbXML = "MediaStreamRelayPortResourceListExistDB";
      var MediaStreamRelayPortResourceExistVal = "MediaStreamRelayPortResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamRelayPortResourceExistdbXML + ".xml", + MediaStreamRelayPortResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamRelayPortResourceExistVal).cloneNode(true);
      var MediaStreamRelayPortResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamRelayPortResourceExistVal);
      var ParentNode = MediaStreamRelayPortResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamRelayPortResourceListNode); 
      var MediaStreamIOPortResourceExistdbXML = "MediaStreamIOPortResourceListExistDB";
      var MediaStreamIOPortResourceExistVal = "MediaStreamIOPortResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamIOPortResourceExistdbXML + ".xml", + MediaStreamIOPortResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamIOPortResourceExistVal).cloneNode(true);
      var MediaStreamIOPortResourceListNode = newDoc.selectSingleNode(".//"+MediaStreamIOPortResourceExistVal);
      var ParentNode = MediaStreamIOPortResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamIOPortResourceListNode); 
      var MediaStreamProfileExistdbXML = "MediaStreamProfileListExistDB";
      var MediaStreamProfileExistVal = "MediaStreamProfileList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamProfileExistdbXML + ".xml", + MediaStreamProfileExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamProfileExistVal).cloneNode(true);
      var MediaStreamProfileListNode = newDoc.selectSingleNode(".//"+MediaStreamProfileExistVal);
      var ParentNode = MediaStreamProfileListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamProfileListNode); 
      var ExportImportFileExistdbXML = "ExportImportFileListExistDB";
      var ExportImportFileExistVal = "ExportImportFileList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + ExportImportFileExistdbXML + ".xml", + ExportImportFileExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+ExportImportFileExistVal).cloneNode(true);
      var ExportImportFileListNode = newDoc.selectSingleNode(".//"+ExportImportFileExistVal);
      var ParentNode = ExportImportFileListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, ExportImportFileListNode); 
      var ServiceResourceExistdbXML = "ServiceResourceListExistDB";
      var ServiceResourceExistVal = "ServiceResourceList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + ServiceResourceExistdbXML + ".xml", + ServiceResourceExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+ServiceResourceExistVal).cloneNode(true);
      var ServiceResourceListNode = newDoc.selectSingleNode(".//"+ServiceResourceExistVal);
      var ParentNode = ServiceResourceListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, ServiceResourceListNode); 
      var SessionConfigExistdbXML = "SessionConfigListExistDB";
      var SessionConfigExistVal = "SessionConfigList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + SessionConfigExistdbXML + ".xml", + SessionConfigExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+SessionConfigExistVal).cloneNode(true);
      var SessionConfigListNode = newDoc.selectSingleNode(".//"+SessionConfigExistVal);
      var ParentNode = SessionConfigListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, SessionConfigListNode); 
      var NetworkAddressRuleExistdbXML = "NetworkAddressRuleListExistDB";
      var NetworkAddressRuleExistVal = "NetworkAddressRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + NetworkAddressRuleExistdbXML + ".xml", + NetworkAddressRuleExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+NetworkAddressRuleExistVal).cloneNode(true);
      var NetworkAddressRuleListNode = newDoc.selectSingleNode(".//"+NetworkAddressRuleExistVal);
      var ParentNode = NetworkAddressRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, NetworkAddressRuleListNode); 
      var GroupTagDefExistdbXML = "GroupTagDefListExistDB";
      var GroupTagDefExistVal = "GroupTagDefList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + GroupTagDefExistdbXML + ".xml", + GroupTagDefExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+GroupTagDefExistVal).cloneNode(true);
      var GroupTagDefListNode = newDoc.selectSingleNode(".//"+GroupTagDefExistVal);
      var ParentNode = GroupTagDefListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, GroupTagDefListNode); 
      var TagObjectFilterExistdbXML = "TagObjectFilterListExistDB";
      var TagObjectFilterExistVal = "TagObjectFilterList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + TagObjectFilterExistdbXML + ".xml", + TagObjectFilterExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+TagObjectFilterExistVal).cloneNode(true);
      var TagObjectFilterListNode = newDoc.selectSingleNode(".//"+TagObjectFilterExistVal);
      var ParentNode = TagObjectFilterListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, TagObjectFilterListNode); 
      var ObjectFilterExistdbXML = "ObjectFilterListExistDB";
      var ObjectFilterExistVal = "ObjectFilterList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + ObjectFilterExistdbXML + ".xml", + ObjectFilterExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+ObjectFilterExistVal).cloneNode(true);
      var ObjectFilterListNode = newDoc.selectSingleNode(".//"+ObjectFilterExistVal);
      var ParentNode = ObjectFilterListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, ObjectFilterListNode); 
      var PolicyAdminPolicyExistdbXML = "PolicyAdminPolicyRuleListExistDB";
      var PolicyAdminPolicyExistVal = "PolicyAdminPolicyRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + PolicyAdminPolicyExistdbXML + ".xml", + PolicyAdminPolicyExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+PolicyAdminPolicyExistVal).cloneNode(true);
      var PolicyAdminPolicyRuleListNode = newDoc.selectSingleNode(".//"+PolicyAdminPolicyExistVal);
      var ParentNode = PolicyAdminPolicyRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, PolicyAdminPolicyRuleListNode); 
      var UserAdminPolicyExistdbXML = "UserAdminPolicyRuleListExistDB";
      var UserAdminPolicyExistVal = "UserAdminPolicyRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + UserAdminPolicyExistdbXML + ".xml", + UserAdminPolicyExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+UserAdminPolicyExistVal).cloneNode(true);
      var UserAdminPolicyRuleListNode = newDoc.selectSingleNode(".//"+UserAdminPolicyExistVal);
      var ParentNode = UserAdminPolicyRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, UserAdminPolicyRuleListNode); 
      var AssetAdminPolicyExistdbXML = "AssetAdminPolicyRuleListExistDB";
      var AssetAdminPolicyExistVal = "AssetAdminPolicyRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + AssetAdminPolicyExistdbXML + ".xml", + AssetAdminPolicyExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+AssetAdminPolicyExistVal).cloneNode(true);
      var AssetAdminPolicyRuleListNode = newDoc.selectSingleNode(".//"+AssetAdminPolicyExistVal);
      var ParentNode = AssetAdminPolicyRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, AssetAdminPolicyRuleListNode); 
      var MediaStoreAdminPolicyExistdbXML = "MediaStoreAdminPolicyRuleListExistDB";
      var MediaStoreAdminPolicyExistVal = "MediaStoreAdminPolicyRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStoreAdminPolicyExistdbXML + ".xml", + MediaStoreAdminPolicyExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStoreAdminPolicyExistVal).cloneNode(true);
      var MediaStoreAdminPolicyRuleListNode = newDoc.selectSingleNode(".//"+MediaStoreAdminPolicyExistVal);
      var ParentNode = MediaStoreAdminPolicyRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStoreAdminPolicyRuleListNode); 
      var MediaRoomPolicyRuleExistdbXML = "MediaRoomPolicyRuleListExistDB";
      var MediaRoomPolicyRuleExistVal = "MediaRoomPolicyRuleList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaRoomPolicyRuleExistdbXML + ".xml", + MediaRoomPolicyRuleExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaRoomPolicyRuleExistVal).cloneNode(true);
      var MediaRoomPolicyRuleListNode = newDoc.selectSingleNode(".//"+MediaRoomPolicyRuleExistVal);
      var ParentNode = MediaRoomPolicyRuleListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaRoomPolicyRuleListNode); 
      var MediaStreamFilterExistdbXML = "MediaStreamFilterListExistDB";
      var MediaStreamFilterExistVal = "MediaStreamFilterList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + MediaStreamFilterExistdbXML + ".xml", + MediaStreamFilterExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+MediaStreamFilterExistVal).cloneNode(true);
      var MediaStreamFilterListNode = newDoc.selectSingleNode(".//"+MediaStreamFilterExistVal);
      var ParentNode = MediaStreamFilterListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, MediaStreamFilterListNode); 
      var DisplayProfileExistdbXML = "DisplayProfileListExistDB";
      var DisplayProfileExistVal = "DisplayProfileList";
      var ExistDBDoc = xsdTOxml.loadInclude("xml/" + DisplayProfileExistdbXML + ".xml", + DisplayProfileExistVal , "xml", true);
      var ExistDBDeafultsNode = ExistDBDoc.selectSingleNode("//"+DisplayProfileExistVal).cloneNode(true);
      var DisplayProfileListNode = newDoc.selectSingleNode(".//"+DisplayProfileExistVal);
      var ParentNode = DisplayProfileListNode.getParent();
      ParentNode.replaceNode(ExistDBDeafultsNode, DisplayProfileListNode); 
    } 
  // now set the default values from the array defined in DefaultValues.js
  var defaultValueArr = null;
  if( ObjectName == "AssetAdminPolicyRule") {
      defaultValueArr = AssetAdminPolicyRuleArr;
  } else if( ObjectName == "AssetManagementRequestData") {
      defaultValueArr = AssetManagementRequestDataArr;
  } else if( ObjectName == "AssetManagementResponseData") {
      defaultValueArr = AssetManagementResponseDataArr;
  } else if( ObjectName == "AssetResource") {
      defaultValueArr = AssetResourceArr;
  } else if( ObjectName == "DMS-100-Device") {
      defaultValueArr = DMS100DeviceArr;
  } else if( ObjectName == "DMS-120-Device") {
      defaultValueArr = DMS120DeviceArr;
  } else if( ObjectName == "DMS-200-Device") {
      defaultValueArr = DMS200DeviceArr;
  } else if( ObjectName == "DMS-1000-Device") {
      defaultValueArr = DMS1000DeviceArr;
  } else if( ObjectName == "DMS-1200-Device") {
      defaultValueArr = DMS1200DeviceArr;
  } else if( ObjectName == "DMS-1500-Device") {
      defaultValueArr = DMS1500DeviceArr;
  } else if( ObjectName == "DMS-1600-Device") {
      defaultValueArr = DMS1600DeviceArr;
  } else if( ObjectName == "DMS-1700-Device") {
      defaultValueArr = DMS1700DeviceArr;
  } else if( ObjectName == "DMS-25-Device") {
      defaultValueArr = DMS25DeviceArr;
  } else if( ObjectName == "GroupTagDef") {
      defaultValueArr = GroupTagDefArr;
  } else if( ObjectName == "IPVSClient-Device") {
      defaultValueArr = IPVSClientDeviceArr;
  } else if( ObjectName == "DisplayProfile") {
      defaultValueArr = DisplayProfileArr;
  } else if( ObjectName == "MediaRoomPolicyRule") {
      defaultValueArr = MediaRoomPolicyRuleArr;
  } else if( ObjectName == "MediaRoom") {
      defaultValueArr = MediaRoomArr;
  } else if( ObjectName == "MediaStoreAdminPolicyRule") {
      defaultValueArr = MediaStoreAdminPolicyRuleArr;
  } else if( ObjectName == "MediaStoreFile") {
      defaultValueArr = MediaStoreFileArr;
  } else if( ObjectName == "MediaStorePortResource") {
      defaultValueArr = MediaStorePortResourceArr;
  } else if( ObjectName == "MediaStreamDirResource") {
      defaultValueArr = MediaStreamDirResourceArr;
  } else if( ObjectName == "MediaStreamDstPortResource") {
      defaultValueArr = MediaStreamDstPortResourceArr;
  } else if( ObjectName == "MediaStreamFileResource") {
      defaultValueArr = MediaStreamFileResourceArr;
   } else if( ObjectName == "MediaStreamFilter") {
      defaultValueArr = MediaStreamFilterArr;
   } else if( ObjectName == "MediaStreamIOPortResource") {
      defaultValueArr = MediaStreamIOPortResourceArr;
   } else if( ObjectName == "MediaStreamRelayPortResource") {
      defaultValueArr = MediaStreamRelayPortResourceArr;
   } else if( ObjectName == "MediaStreamSrcPortResource") {
      defaultValueArr = MediaStreamSrcPortResourceArr;
   } else if( ObjectName == "MPEGTSMediaStreamPRofile") {
      defaultValueArr = MPEGTSMediaStreamProfileArr;
   } else if( ObjectName == "MPEGTS-Rx-Device") {
      defaultValueArr = MPEGTSRxDeviceArr;
   } else if( ObjectName == "MPEGTS-Tx-Device") {
      defaultValueArr = MPEGTSTxDeviceArr;
   } else if( ObjectName == "MS-Device") {
      defaultValueArr = MSDeviceArr;
   } else if( ObjectName == "MS-25-Device") {
      defaultValueArr = MS25DeviceArr;
   } else if( ObjectName == "ObjectFilter") {
      defaultValueArr = ObjectFilterArr;
   } else if( ObjectName == "PolicyAdminPolicyRule") {
      defaultValueArr = PolicyAdminPolicyRuleArr;
   } else if( ObjectName == "RTPMediaStreamPRofile") {
      defaultValueArr = RTPMediaStreamProfileArr;
   } else if( ObjectName == "RTP-Rx-Device") {
      defaultValueArr = RTPRxDeviceArr;
   } else if( ObjectName == "RTP-Tx-Device") {
      defaultValueArr = RTPTxDeviceArr;
   } else if( ObjectName == "TagObjectFilter") {
      defaultValueArr = TagObjectFilterArr;
   } else if( ObjectName == "UDPMediaStreamProfile") {
      defaultValueArr = UDPMediaStreamProfileArr;
   } else if( ObjectName == "UDP-Rx-Device") {
      defaultValueArr = UDPRxDeviceArr;
   } else if( ObjectName == "UDP-Tx-Device") {
      defaultValueArr = UDPTxDeviceArr;
   } else if( ObjectName == "UserAdminPolicyRule") {
      defaultValueArr = UserAdminPolicyRuleArr;
   } else if( ObjectName == "User") {
      defaultValueArr = UserArr;
   } else if( ObjectName == "V2DMediaStreamProfile") {
      defaultValueArr = V2DMediaStreamProfileArr;
   } else if( ObjectName == "V2D-Rx-Device") {
      defaultValueArr = V2DRxDeviceArr;
   } else if( ObjectName == "V2D-Tx-Device") {
      defaultValueArr = V2DTxDeviceArr;
   } else if( ObjectName == "V2D-XP200-Device") {
      defaultValueArr = V2DXP200DeviceArr;
   } else if( ObjectName == "V2D-XP200S-Device") {
      defaultValueArr = V2DXP200SDeviceArr;
   } else if( ObjectName == "V2D-XP220-Device") {
      defaultValueArr = V2DXP220DeviceArr;
   } else if( ObjectName == "V2D-XP220S-Device") {
      defaultValueArr = V2DXP220SDeviceArr;
   } else if( ObjectName == "V2D-XP100-Device") {
      defaultValueArr = V2DXP100DeviceArr;
   } else if( ObjectName == "V2D-XPi-Device") {
      defaultValueArr = V2DXPiDeviceArr;
   } else if( ObjectName == "ORB-500-Device") {
      defaultValueArr = ORB500DeviceArr;
   } else if( ObjectName == "MediaStoreInfo") {
      defaultValueArr = MediaStoreInfoArr;
   } else if( ObjectName == "MediaSegmentInfo") {
      defaultValueArr = MediaSegmentInfoArr;
   } else if( ObjectName == "Bookmark") {
      defaultValueArr = BookmarkArr;
   } else if( ObjectName == "CreateCollaborationResquestData") {
      defaultValueArr = CreateCollaborationResquestDataArr;
   } else if( ObjectName == "CreateCollaborationResponseData") {
      defaultValueArr = CreateCollaborationResponseDataArr;
   } else if( ObjectName == "CollaborationConfig") {
      defaultValueArr = CollaborationConfigArr;
   }


//CollaborationConfigArr 


  
  //check ig default array is null
  if(defaultValueArr != null) {
    for(var i=0;i<defaultValueArr.length;i++){
      var objXpathType = defaultValueArr[i].xPathType;
      var objXpath = defaultValueArr[i].xPath;
      var objXpathVal = defaultValueArr[i].xPathValue;
      try{
        if(objXpathType == "Attribute" ) {
          if(objXpath == ".//*//Parameter[@type]" ) {
            var parameterTypeIter = newDoc.selectNodeIterator(objXpath);
            if(!parameterTypeIter.hasNext()) {
            } else {
              while(parameterTypeIter.hasNext()) {
                var parameterTypeIterNode = parameterTypeIter.next();
                parameterTypeIterNode.setAttribute("type","Float");
              }
            }
          } else {
            var objNode = newDoc.selectSingleNode(objXpath);
            objNode.setAttribute(objXpath,objXpathVal);
          }         
        } else if(objXpathType == "SingleElement" ) {
            var objNodeSingleIter = newDoc.selectNodeIterator(objXpath);
            var singleValue = 1;
            while(objNodeSingleIter.hasNext()) {
                var objNodeSingle = objNodeSingleIter.next();
                objNodeSingle.setValue(objXpathVal[singleValue]);
                singleValue++;
            }
        } else {    
          var objNodeIter = newDoc.selectNodeIterator(objXpath);
          if(!objNodeIter.hasNext()) {
          } else {
            while(objNodeIter.hasNext()) {
              var objNodeIterNode = objNodeIter.next();
              objNodeIterNode.setValue(objXpathVal);
            }
          }          
          
        }
      } catch(e) {
        alert("Cannot find Node: Description-> " + e.description + "\n" +
              "Message->" + e.message + "\n" +
              "Name->" + e.name + "\n" + 
              "Error Number->" + e.number);
      }
    }
  }
  
  
  // save the data in the global var 
  XMLData = null;
  XMLData = newDoc.toString();
    
  
  if( showXMLData) {
    // show the xml in the text area
    txtAreaXMLBlkJSX.setValue(XMLData);   
    txtXMLNameBlkJSX.setValue(ObjectName + ".xml");
  }
}

function clearXMLArea() {
  txtAreaXMLBlkJSX.setValue("");
  txtXMLNameBlkJSX.setValue();
  XMLData = null;
}

// this function is called when clicked on the save xml button
function saveXML() {
  try {
    if(showXMLData) {
      if( XMLData == null) {
        alert("Please generate XML Data");
        return false;
      }
    }

    //check where to save   dir is selected
    if( checkSaveDir() != true ) return;
      
    var WshShell = new ActiveXObject("WScript.Shell");
    // gets the desktop path
    var strDesktop = WshShell.SpecialFolders("Desktop");
    //gets the file name
    var logFileName = txtXMLNameBlkJSX.getValue();
    if( logFileName == "MediaRoomMRSchema.xml") {
      logFileName = "MRSchema.xml";
    }
    var FSO = new ActiveXObject("Scripting.FileSystemObject");
    var fileData = "";
    if(TextAreaText){
      fileData = txtAreaXMLBlkJSX.getValue();
    }else{
      fileData = XMLData;
    }
    // writes file to the folder
    var theFile = FSO.OpenTextFile( IPVSDefualtFilesDir + '\\' + logFileName, 2, true );
    theFile.write( fileData);
    theFile.close();
    if(showXMLData) {
      alert("XML file '" +IPVSDefualtFilesDir +"\\"+ logFileName + "' successfully saved");
    }
  }
  catch(e) {
    alert("Writing XML failed : " + e.description);
  }  
}

function getAllXML() {
  //clear xml text area
  clearXMLArea();
  showXMLData = false;
  versionSelected = null;
  TextAreaText = false;
  //check where to save   dir is selected
  if( checkSaveDir() != true ) return; //CTBR
    
  // select version
  var ObjectVersion = selAllXSDVersionBlkJSX.getValue();
  //ObjectVersion = "1.1";
  if(ObjectVersion.match("Select_") != null) {
    alert("Please select version");
    return false;  
  }
  versionSelected = ObjectVersion.replace(/_/g,"\.");
    
  // go thru xsd array to generate xml for each xsd
  for(var i=0;i<xsdObjectArr.length;i++) { // CTBR
    //get rule
    var ObjectName = xsdObjectArr[i];  // CTBR
    //var ObjectName = "ObjectFilter";
    
    // check rule exist for that version
    var RuleName = ObjectName +"-v"+ ObjectVersion;
    if(checkRuleNameExist(RuleName)) {
      // set msg in the text area
      var msg = txtAreaXMLBlkJSX.getValue();
      msg += ObjectName + " generating XML ...\n ";
      txtAreaXMLBlkJSX.setValue(msg);    
      // set the file name
      txtXMLNameBlkJSX.setValue(ObjectName + ".xml");
      // runs rule to generate xml 
      utilRuleGetOutboundDoc(ObjectName, RuleName);
      // run save file
      saveXML();
      // set msg in the text area
      msg = txtAreaXMLBlkJSX.getValue();
      msg += ObjectName + " XML file saved \n\n";
      txtAreaXMLBlkJSX.setValue(msg);
    }
  } //CTBR
}


function getAllCurrentXML() {
  //clear xml text area
  clearXMLArea();
  showXMLData = false;
  versionSelected = null;
  TextAreaText = false;  
  //check where to save   dir is selected
  if( checkSaveDir() != true ) return;
      
  for(var i=0;i<ruleFilesArr.length;i++) {
    var fName = ruleFilesArr[i];
    if( (fName.match("\.xml") != null) && (fName.match("-Head") != null) ) {
      var sFName = fName.split("\.xml",1);
      var ObjectName = sFName.toString().split("-v",1);
      var RuleName = sFName.toString().replace(/\./g,"_");
      // set msg in the text area
      var msg = txtAreaXMLBlkJSX.getValue();
      msg += ObjectName + " generating XML ...\n ";
      txtAreaXMLBlkJSX.setValue(msg);    
      // set the file name
      txtXMLNameBlkJSX.setValue(ObjectName + ".xml");   
      // runs rule to generate xml
      utilRuleGetOutboundDoc(ObjectName, RuleName);
      // run save file
      saveXML();
      // set msg in the text area
      msg = txtAreaXMLBlkJSX.getValue();
      msg += ObjectName + " XML file saved \n\n";
      txtAreaXMLBlkJSX.setValue(msg);         
    }
  }
    
}

// Utility Functions ************************************************************

function checkSaveDir() {
  if( (IPVSDefualtFilesDir == null) || 
      (IPVSDefualtFilesDir == "") ) {
    alert("Please specify the path where you want to save the XML.");
    return false;
  } else {
    return true;
  }
}

function checkRuleNameExist(RuleName) {
  var RuleFile = RuleName + ".xml";
  RuleFile = RuleFile.replace(/_/g,"\.");
  for(var i=0;i<ruleFilesArr.length;i++) {
    var fName = ruleFilesArr[i];
    if( (fName.match("\.xml") != null) ) {
      if( RuleFile == fName ) {
        return true;
      }   
    }
  }
  return false;
}
