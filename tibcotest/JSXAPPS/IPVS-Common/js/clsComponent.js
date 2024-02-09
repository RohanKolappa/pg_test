//To release a deadlock caused by GI 'jsx3.sleep' bug: in a try-catch-final function,
//the final part never excute even if try block throw an exception
function jsxSleepQueueMonitor(sleepDelayTime) {
  this.lastSleepTo = null;
  this.intervalID = null;
  this.delay = sleepDelayTime ? sleepDelayTime : 1000;
  this.start = function() {
    this.intervalID = window.setInterval(function() {
      if (jsx3._SLEEP_TO != null) {
        if (jsx3._SLEEP_TO == this.lastSleepTo) {
          log.error("jsxSleepQueueMonitor::jsx3 sleep queue is blocked:resetting.");
          jsx3._SLEEP_QUEUE = [];
          jsx3._SLEEP_TO = null;
        }
        else {
          this.lastSleepTo = jsx3._SLEEP_TO;
        }
      }
    }, this.delay);
  }
  this.stop = function() {
    if (this.intervalID != null) {
      clearInterval(this.intervalID);
    }
  }
}

//---------------------------------------
function EventListner ( TopicName) {
  this.TopicName = TopicName;
  this.PubSubList = [];

  topic_Subscribe(TopicName, this, "eventHandler");

  this.destroy = function()
  {
    topic_UnSubscribe(TopicName, this, "eventHandler");
  }

  this.subscribe = function(EventVar, EventVal, Context, Function)
  {
    var Event = new Object;
    Event.Id = jsx3.xml.CDF.getKey();
    Event.Var = EventVar;
    Event.Val = EventVal;
    Event.Context = Context;
    Event.Function = Function;
    this.PubSubList.push(Event);
    return Event.Id;
  }
  
  this.unsubscribe = function(EventVar, EventId) {
    for(var i=0; i<this.PubSubList.length; i++) {
      var Event = this.PubSubList[i];
      if( Event.Var == EventVar ) {
        if( (EventId == null) || (EventId == Event.Id) ) {
          this.PubSubList.splice(i, 1);
          return;
        }
      }
    }
  }

  this.eventHandler = function(objPub) {
    if (objPub) {
      var TopicName = objPub.subject;
      var EventVar = objPub.stateVar;
      var EventVal = objPub.stateVal;
      for (var i = 0; i < this.PubSubList.length; i++) {
        var Event = this.PubSubList[i];
        if (Event.Var == EventVar &&
            (Event.Val == null || Event.Val == EventVal)) {
          var TgtContextBlkName = Event.Context.BlkName != null ? Event.Context.BlkName : (this.BlkName != null ? this.BlkName : this);
          var TgtFunctionName = "Unknown";
          log.debug("EVENT: SUB: Topic: " + this.TopicName + " Var: " + EventVar + " Val: " + EventVal
                                                           + " Target: " + TgtContextBlkName + "::" + TgtFunctionName);
          if (Event.Context != null) {
            (Event.Function).call(Event.Context, EventVar, EventVal);
          }
          else {
            (Event.Function).call(this, EventVar, EventVal);
          }
        }
      }
    }
  }
}
//---------------------------------------

function Component (BlkName, EventArr, SelEvent, MapRuleArr) {
  // base vars
  if( BlkName != null ) {
    if( this.BlkName == null ) this.BlkName = BlkName;
    // get JSX handle from blkname
    if( this.BlkJSX == null ) {
      // allow an array to specify hierarchy
      // CAVEAT: Not all javascript strings are 'String'
      // when in doubt - caller should use array form of input
      if( typeof this.BlkName == 'string' ) {
        this.BlkJSX = PNameSpace.getJSXByName(BlkName);
      }
      else {
        var blkNameArr = this.BlkName;
        for( var i=0; i<blkNameArr.length; i++ ) {
          if( typeof blkNameArr[i] != 'string' ) {
            this.BlkJSX = blkNameArr[i];
          }
          else {
            if( this.BlkJSX == null ) {
              this.BlkJSX = PNameSpace.getJSXByName(blkNameArr[i]);
            }
            else {
              this.BlkJSX = this.BlkJSX.getDescendantOfName(blkNameArr[i], false, false);
            }
          }
        }
      }
    }
  }
  if( EventArr != null ) {
    this.EventArr = EventArr;
    this.EventState = [];
    this.eventHandlerArr = new Object;  
  }
  if( SelEvent != null ) {
    this.SelEvent = SelEvent;
  }
  if( MapRuleArr != null ) {
    this.MapRuleArr = MapRuleArr;
  }
  this.autoHide = null;
  this.autoHideTimerId = null;
  this.previousAutoHideTimerId = null;

// base init function
  this.init = function() {
    this.CmpViewState = "NONE";
    if (this.BlkJSX != null && this.BlkJSX.clearXmlData != null) {
      //this.BlkJSX.clearXmlData();
      this.PaintState = false;
    }
    // setup app event handler
    if (this.EventArr != null) {
      for (var i = 0; i < this.EventArr.length; i++) {
        var Event = this.EventArr[i];
        if (Event.action == "APPEVENT" || Event.action == "REQEVENT") {
          Event.topic = tApp;
        }
        if (Event.action != "FNCCALL") {
          if (Event.action == "CLSEVENT") {
            Event.eventcls.subscribe(Event.eventsub, this, this.clsShow);
            this.EventState[i] = Event.eventcls.getStateParam(Event.eventvar);
          }
          else {
            if (this.eventHandlerArr[Event.topic] == null) {
              this.eventHandlerArr[Event.topic] = new EventListner(Event.topic);
            }
            this.eventHandlerArr[Event.topic].subscribe(Event.eventvar, Event.eventval, this, this.show);
            this.EventState[i] = topic_Get(Event.topic, Event.eventvar);
          }
        }
      }
    }
    if (this.SelEvent != null) {
      if (this.SelEvent.action == "APPEVENT" || this.SelEvent.action == "REQEVENT") {
        this.SelEvent.topic = tApp;
      }
    }
    if (this.MenuEvent != null) {
      if (this.MenuEvent.action == "APPEVENT" || this.MenuEvent.action == "REQEVENT") {
        this.MenuEvent.topic = tApp;
      }
    }
    // skin the component
    this.customizeSkin();
  }

  // customize the skin label or images
  this.customizeSkin = function() {
  }
  
  // base destroy function
  this.destroy = function() {
    if(this.autoHideTimerId){
      clearTimeout(this.autoHideTimerId);
    }
    if( this.EventArr != null ) {
      for(var j=0; j<this.EventArr.length; j++) {
        var Event = this.EventArr[j];
        if( Event.action == "CLSEVENT" ) {
          Event.eventcls.unsubscribe(Event.eventsub, this, this.clsShow);
        }
      }
    }
    if( this.eventHandlerArr == null ) return;
    for( var EventTopic in this.eventHandlerArr ) {
      this.eventHandlerArr[EventTopic].destroy();
      delete this.eventHandlerArr[EventTopic];
    }
  }
  
// set map rule info
  this.setMapRuleData = function(rule, direction, datatype, datasrc) {
    if( this.MapRuleArr == null ) this.MapRuleArr = new Object;
    if( this.MapRuleArr[rule] == null ) {
      this.MapRuleArr[rule] = new Object;
    }
    this.MapRuleArr[rule][direction] = new Object;
    this.MapRuleArr[rule][direction].datatype = datatype;
    this.MapRuleArr[rule][direction].datasrc = datasrc;
  }
  
  //to handle show function from class event
  this.clsShow = function(objEvent) {
    var EventVar = objEvent.stateVar;
    var EventVal = objEvent.stateVal;
    this.show(EventVar, EventVal);
  }
  
// base show function
  this.show = function(EventVar, EventVal) {
    // repaint if necessary
    if( this.repaintCheck() == true ) {
      this.repaint(EventVar, EventVal);
    }
    // set display state
    if( this.BlkJSX == null ) return;
    //check if the parent is hidden
    // if hidden then dont change child display state
    function checkBlkJSXDisplay(blkJSX) {
      if (blkJSX.getDisplay!=null && blkJSX.getDisplay() == "none")
      {
        return true;
      }
      return false;
    }
    // finds the parent state
    var HiddenParentBlkJSX = this.BlkJSX.findAncestor(checkBlkJSXDisplay, false);    
    if( (this.CmpViewState != "VISIBLE") &&
        (HiddenParentBlkJSX == null) ) {
      if(this.BlkJSX.setDisplay != null) {
        this.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
      this.CmpViewState = "VISIBLE";
    }
    var THISCLASS = this;
    // process auto hide if specified
    if (this.autoHide) {
      // if non-zero auto hide - (re)start a hide timer
      if( this.autoHide != 0) {
        if(this.autoHideTimerId){
          this.previousAutoHideTimerId = this.autoHideTimerId;
        }
        this.autoHideTimerId = window.setTimeout(function() {
                                            THISCLASS.hide();
                                          }, this.autoHide);
        //clear previouse after creating a new one, keep this sequence
        if(this.previousAutoHideTimerId){
          clearTimeout(this.previousAutoHideTimerId);
        }
      }
      // if 0 - turn off any currently running hide timer
      else {
        if(this.autoHideTimerId){
          clearTimeout(this.autoHideTimerId);
        }
      }    
    }
  }
// base hide function
  this.hide = function() {
    if( this.BlkJSX == null ) return;
    if( this.CmpViewState != "HIDDEN" ) {
      if(this.BlkJSX.setDisplay != null) {
        this.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }
      this.CmpViewState = "HIDDEN";
    }     
  }

// base repaint check function - check if paint needed
  this.repaintCheck = function() {
    var RepaintFlag = false;
    // check if not painted - if so start with repaint = true
    if( this.PaintState == null || this.PaintState == false ) {
      RepaintFlag = true;
    }
    // check for valid/changed event/data state
    if( this.EventArr == null ) return RepaintFlag;
    var CurrEventState = [];
    for(var i=0; i<this.EventArr.length; i++) {
      var Event = this.EventArr[i];
      if( Event.action == "CLSEVENT" ) {
        CurrEventState[i] = Event.eventcls.getStateParam(Event.eventvar);
      }
      else {
        CurrEventState[i] = topic_Get(Event.topic, Event.eventvar);
      }
      // if if event is of req type
      if( Event.action == "REQEVENT" ) {
        // check if there is a request assoc with it
        var cmpReqObj = cmpReqObj_Get(Event.eventvar);
        // which has never been fired
        if( cmpReqObj != null && cmpReqObj.listreqstate == null ) {
          // if yes fire it off
          jsx3.sleep((cmpReqObj.req.sendRequest()), null, this);
        }
      }
      // if any one of the state vars is not valid - can't paint
      //if( CurrEventState[i] == null || CurrEventState[i] == "NONE" ) {
      if( CurrEventState[i] == null ) {
        return false;
      }
      // if it is active - check for change from last repaint
      if( CurrEventState[i] != this.EventState[i] ) {
        if( CurrEventState[i] == "INVALID" ) {
//          this.PaintState = "mask";
//          RepaintFlag = true;
          RepaintFlag = false;
        }
        else {
          RepaintFlag = true;
        }
      } 
    }
    // all state vars are valid here - save curr state - if we are going to repaint 
    if( RepaintFlag == true ) {
      this.EventState = CurrEventState;
    }
    return RepaintFlag;
  }
  
  // base repaint function
  this.repaint = function() {
   // run all inbound map rules -as needed
    if( this.MapRuleArr != null ) {
      for(var rule in this.MapRuleArr) {
        var objRule = this.MapRuleArr[rule];
        if( objRule.IN == null ) continue;
        try {
          var ruleDoc = objRule.IN.datasrc;
          if( objRule.IN.datatype == "FILE" ) {
            ruleDoc = PNameSpace.loadInclude("jsxuser:///" + objRule.IN.datasrc, "FILE_" + objRule.IN.datasrc, "xml", true);
          }
          else if( objRule.IN.datatype == "DOC" ) {
            ruleDoc = objRule.IN.datasrc;
          }
          else if( objRule.IN.datatype == "NID" ) {
            var objectNode = topic_GetNode(tCache, "//*[@NID='" + objRule.IN.datasrc + "']");
            ruleDoc = new jsx3.xml.Document().loadXML(objectNode.toString()); 
          }
          utilRuleSetInboundDoc.call(this, rule, ruleDoc);
        }
        catch(e) {
          handleFatalError( "Internal Error: Inbound Map failed: Component: " + this.BlkJSX + "\n" +
                    "Map Rule: " + rule + "\n" +
                    "Map Rule Datasrc: " + objRule.IN.datasrc);
        }
      }
      this.postpaint();
    }
  }
  
// base post paint function
  this.postpaint = function() {
    if( this.BlkJSX == null ) return;
    this.BlkJSX.repaint();
    this.PaintState = true;
  }
  
// base select event function
  this.onSelect = function(SelVal) {
    this.doCmpEvent(this.SelEvent, this.selRecord, SelVal);
    //HACK: resize window for Firefox2.0 to display GI components
//    firefoxVerFlag = firefox20WindowResize(firefoxVersion, firefoxVerFlag);
  }
  
// base right click event function
  this.onMenu = function(MenuVal) {
    this.doCmpEvent(this.MenuEvent, this.menuRecord, MenuVal);
    // return true to allow menu display
    return true;
  }

  this.doCmpEvent = function(Event, Record, Val) {
    if (Event == null) return;
    if (Event.action == null) return;
    // call/fire event
    if (Event.action != "FNCCALL") {
      var eventval = Event.eventval;
        if (Event.eventval == "DATE") {
          eventval = newServerDate();
        }
        else if (Record != null && Val != "MULTI-ROWS") {
          eventval = Record[Event.eventval];
        }
        else if (Val != null) {
          eventval = Val;
        }
        topic_Publish(Event.topic, Event.eventvar, eventval);
      }
    else if (Event.action == "FNCCALL") {
      //jsx3.sleep((Event.fnc), null, Event.context);
      if (Event.context != null) {
        (Event.fnc).call(Event.context, Val);
      }
      else {
        (Event.fnc).call(this, Val);
      }
    }
  }   
}

//---------------------------------------
/*
SampleComponent.prototype = new Component();
function SampleComponent (BlkName) {
// setup inheritence and call base constructor
  SampleComponent.BaseClass = SampleComponent.prototype;
  SampleComponent.prototype.constructor = SampleComponent;
  // extended constructor code here
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName);
    // extension logic here
  }
  // extend a base func
  this.show = function() {
    SampleComponent.BaseClass.show.call(this);
    // extension logic here
  }
}
*/

// define blk just to repaint
//---------------------------------------
CmpBlock.prototype = new Component();
function CmpBlock (BlkName) {
// setup inheritence and call base constructor
  CmpBlock.BaseClass = CmpBlock.prototype;
  CmpBlock.prototype.constructor = CmpBlock;
  // extended constructor code here
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName);
    // extension logic here
  }
  // force repaint
  this.repaintCheck = function() {
    return true;  
  }
  // over ride repaint
  this.repaint = function() {
    this.postpaint();
  }
}

//---------------------------------------
CmpContainer.prototype = new Component();
function CmpContainer (BlkName, SubCmpArr, EventArr, onDestroyEvent) {
// setup inheritence and call base constructor
  CmpContainer.BaseClass = CmpContainer.prototype;
  CmpContainer.prototype.constructor = CmpContainer;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, onDestroyEvent);
    this.SubCmpArr = SubCmpArr;
  }
            
  // extend init
  this.init = function() {
    CmpContainer.BaseClass.init.call(this);
    for(var i=0; i < this.SubCmpArr.length; i++) {
      if( this.SubCmpArr[i].cmp == null ) continue;
      this.SubCmpArr[i].cmp.init();
    }
  }
  // extend destroy
  this.destroy = function() {
    if(this.SubCmpArr == null) return;
    for(var i=0; i < this.SubCmpArr.length; i++) {
      if( this.SubCmpArr[i].cmp == null ) continue;
      this.SubCmpArr[i].cmp.destroy();
      this.SubCmpArr[i].cmp = null;
      delete this.SubCmpArr[i];
    }
    delete this.SubCmpArr;
    // call the onDestroyEvent before final destruction
    if( this.SelEvent != null ) {
      this.onSelect();
    }    
    CmpContainer.BaseClass.destroy.call(this);
  }
  // extend show
  this.show = function (EventVar, EventVal) {
    // do base show for self (potentially recursive)
    CmpContainer.BaseClass.show.call(this, EventVar, EventVal);
    var ThisObj = null
    if (this.SubCmpArr == null) return;
    // do show/hide for all children
    for (var i = 0; i < this.SubCmpArr.length; i++) {
      if (this.SubCmpArr[i].cmp == null) continue;
      // this line for help with debug
      var BlkName = this.SubCmpArr[i].cmp.BlkName;
      // if there is a sel clause - eval it and show / hide based on that 
      if (this.SubCmpArr[i].sel == null || eval(this.SubCmpArr[i].sel)) {
        // create cmp if needed
        if (this.SubCmpArr[i].onDemandFnc != null) {
          this.SubCmpArr[i].cmp = this.SubCmpArr[i].onDemandFnc.call(this);
          this.SubCmpArr[i].cmp.init();
          this.SubCmpArr[i].onDemandFnc = null;
        }
        this.SubCmpArr[i].cmp.show();
        if (this.SubCmpArr[i].onDemandPub != null) {
          ThisObj = this.SubCmpArr[i].onDemandPub;
          jsx3.sleep(function () {
            if (ThisObj != null) {
              eval(ThisObj);
            }
          }, null, this);
        }
      }
      else {
        this.SubCmpArr[i].cmp.hide();
      }
    }
  }
  // extend hide
  this.hide = function() {
    for(var i=0; i < this.SubCmpArr.length; i++) {
      if( this.SubCmpArr[i].cmp == null ) continue;
      this.SubCmpArr[i].cmp.hide();
    }
    CmpContainer.BaseClass.hide.call(this);
  }
}

//---------------------------------------
CmpLabel.prototype = new Component();                                 
function CmpLabel(BlkName, Visibility, EventArr, SelEvent, DefaultTxt) {
  CmpLabel.BaseClass = CmpLabel.prototype;
  CmpLabel.prototype.constructor = CmpLabel;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  if( Visibility != null ) {
    this.Visibility = Visibility;
  }
  this.Txt = null;
  this.DefaultTxt = DefaultTxt;
  // extend init
  this.init = function() {
    CmpLabel.BaseClass.init.call(this);
    if( this.Visibility == "HIDDEN" ) {
      this.BlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN,true);
    }
    // setup the select event handler  
    if( this.SelEvent != null ) {
      // GI bug - block components do not fire events if removeEvent is called
      //        - in addition some script code has to be specified for this event in the IDE
      // this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXCLICK);
      this.BlkJSX.subscribe(jsx3.gui.Interactive.JSXCLICK, this, "onSelect");
    }
    //instead of destroy just setting default text
    if( this.DefaultTxt != null ) {
      this.BlkJSX.setText(this.DefaultTxt, true);
    }
  }
  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.JSXCLICK, this);
    CmpLabel.BaseClass.destroy.call(this);
  }
  this.show = function(EventVar, EventVal) {
    if( this.Visibility != "HIDDEN" ) {
      this.BlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYVISIBLE,true);
    }
    CmpLabel.BaseClass.show.call(this, EventVar, EventVal);
  }
  this.hide = function() {
    this.BlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN,true);
    CmpLabel.BaseClass.hide.call(this);
  }
  this.repaintCheck = function() {
    return true;
  }
  this.repaint = function(EventVar, EventVal) {
    this.Txt = EventVal;
    if( this.Txt != null ) {
      this.BlkJSX.setText(this.Txt, true);
    }
    this.postpaint();
  }
  // extend onselect 
  this.onSelect = function(Val) {
    CmpLabel.BaseClass.onSelect.call(this, Val);
  } 
}

//---------------------------------------
CmpLabelErrorTip.prototype = new CmpLabel();                                 
function CmpLabelErrorTip(BlkName, Visibility, EventArr, SelEvent, DefaultTxt) {
  CmpLabelErrorTip.BaseClass = CmpLabelErrorTip.prototype;
  CmpLabelErrorTip.prototype.constructor = CmpLabelErrorTip;
  if( BlkName != null ) {
    CmpLabel.prototype.constructor.call(this, BlkName, Visibility, EventArr, SelEvent, DefaultTxt);
  }
  
  this.init = function() {
    CmpLabelErrorTip.BaseClass.init.call(this);
    this.BlkJSX.setColor("#ffffff");
    this.BlkJSX.setTip("");
    //instead of destroy just setting default text
    if( this.DefaultTxt != null ) {
      this.BlkJSX.setText(this.DefaultTxt, true);
    }
  }
  
  this.repaint = function(EventVar, EventVal) {
    //Checking ErrorStatus to display tooltip
    if( EventVar == "ERRORSTATUS" ) {
      if( EventVal != "NONE" ) {
        this.BlkJSX.setColor("#FF8040");
        var TipMsg = "";
        if(this.BlkJSX.getTip() == ""){
          TipMsg = EventVal ;
        }else{
          TipMsg = this.BlkJSX.getTip() + "\n" + EventVal;
        }
        this.BlkJSX.setTip(TipMsg);
      }
      else {
        //if ErrorStatus is NONE then remove tooltip
        if(this.BlkJSX.getColor() == "#FF8040"){
          this.BlkJSX.setColor("#ffffff");
        }
        this.BlkJSX.setTip("");
      }
    }
    else {
      if( EventVar == "ConnectionStatusMsg" || EventVar == "CollaborationStatusMsg") {
        if( EventVal == "CONNECTED" ) {
          this.BlkJSX.setColor("#749a80");
          this.BlkJSX.setTip("");
        }else{
           if(this.BlkJSX.getTip() != "" && EventVal != "DENIED"){
             this.BlkJSX.setColor("#FF8040");
           }
           else{
             this.BlkJSX.setColor("#ffffff");
           }
        }
      }
      CmpLabelErrorTip.BaseClass.repaint.call(this, EventVar, EventVal);
    }
    this.postpaint();
  }
}

//---------------------------------------
CmpLabelTxtStream.prototype = new CmpLabel();                                 
function CmpLabelTxtStream(BlkName, Visibility, EventArr, SelEvent, DefaultTxt, DialogFile, BlkDialogText) {
  CmpLabelTxtStream.BaseClass = CmpLabelTxtStream.prototype;
  CmpLabelTxtStream.prototype.constructor = CmpLabelTxtStream;
  if( BlkName != null ) {
    CmpLabel.prototype.constructor.call(this, BlkName, Visibility, EventArr, SelEvent, DefaultTxt);
  }
  this.DialogFile = DialogFile;
  this.BlkDialogText = BlkDialogText;
  this.TxtStream = "";
  this.dlgTextStreamBlkJSX = null;
  this.blkTextStreamBlkJSX = null;
  
  // extend init
  this.init = function() {
    CmpLabelTxtStream.BaseClass.init.call(this);
    // setup the select event handler  
    // turn off click sub
    //this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.JSXCLICK, this);
    // add double click sub
    //this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXDOUBLECLICK);
    //this.BlkJSX.setEvent('eval("' + this.onSelect+ '();")', jsx3.gui.Interactive.JSXDOUBLECLICK);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.JSXDOUBLECLICK, this, "onSelect");
  }
  
  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.JSXDOUBLECLICK, this);
    this.destroyDlg();
    CmpLabelTxtStream.BaseClass.destroy.call(this);
  }
  this.destroyDlg = function() {
    if( this.dlgTextStreamBlkJSX != null ) {
      this.dlgTextStreamBlkJSX .unsubscribe(jsx3.gui.Interactive.DESTROY, this);
      this.dlgTextStreamBlkJSX = null;
    }
    this.blkTextStreamBlkJSX = null;
    return true;
  }
  this.repaint = function(EventVar, EventVal) {
    if( EventVal != null ) {
      // concat eventval to current label txt
      this.TxtStream += EventVal;
      // chop everything but the last line
      var Txt = this.TxtStream.replace(/.*\n/g, "");
      // call base class to do the rest
      CmpLabelTxtStream.BaseClass.repaint.call(this, EventVar, Txt);
    }
    // update dialog if open
    if( this.dlgTextStreamBlkJSX != null ) {
      this.blkTextStreamBlkJSX.setText(this.TxtStream.replace(/\n/g, "<br>"), true);
    }
  }
  this.onSelect = function() {
    // launch / focus dialog
    if( this.dlgTextStreamBlkJSX != null ) {
      this.dlgTextStreamBlkJSX.focus();
    }
    else {
      this.dlgTextStreamBlkJSX = this.BlkJSX.getAncestorOfType(jsx3.gui.Window).getRootBlock().load(this.DialogFile);
      this.dlgTextStreamBlkJSX.subscribe(jsx3.gui.Interactive.DESTROY, this, "destroyDlg");
      this.blkTextStreamBlkJSX = getJsxByName([this.dlgTextStreamBlkJSX, this.BlkDialogText]);
      this.repaint();
    } 
    CmpLabelTxtStream.BaseClass.onSelect.call(this, this.TxtStream);
  } 
}
//---------------------------------------
CmpLabelHighlight.prototype = new CmpLabel();                                 
function CmpLabelHighlight(BlkName, Visibility, EventArr, SelEvent) {
  CmpLabelHighlight.BaseClass = CmpLabelHighlight.prototype;
  CmpLabelHighlight.prototype.constructor = CmpLabelHighlight;
  if( BlkName != null ) {
    CmpLabel.prototype.constructor.call(this, BlkName, Visibility, EventArr, SelEvent);
  }

  this.repaint = function(EventVar, EventVal) {
    if(this.SelEvent.eventvar == EventVar){
      if( this.SelEvent.eventval != EventVal ) {
        this.BlkJSX.setClassName("").repaint();
      } else {
//        this.BlkJSX.setCSSOverride("color:#fff;font-weight:bold;background-color:#aaa;").repaint();
        this.BlkJSX.setClassName("navHighlight").repaint();
      }
      this.postpaint();    
    }
  }
  this.onSelect = function() {
    CmpLabel.BaseClass.onSelect.call(this, null);
  }   
}

//---------------------------------------  
CmpTxtBox.prototype = new Component();                                 
function CmpTxtBox(BlkName, cmpVal, AutoFocus, EventArr, SelEvent,ChangeEvent) {
  CmpTxtBox.BaseClass = CmpTxtBox.prototype;
  CmpTxtBox.prototype.constructor = CmpTxtBox;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    this.AutoFocus = AutoFocus;
    this.ChangeEvent = ChangeEvent;
  }
  var ThisClass = this;
  // extend init
  this.init = function() {
    CmpTxtBox.BaseClass.init.call(this);
    // clear any initial selection
    this.BlkJSX.setValue(cmpVal);
    //FocusEvent
    if(this.SelEvent != null) {
      if(this.ChangeEvent == true){
        this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXCHANGE);
        this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.JSXCHANGE);    
      } 
      else{
        this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
        this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.JSXFOCUS);
      }  
      this.BlkJSX.onSelect = this.onSelect;
      this.BlkJSX.repaint();
    }
  }
  this.onSelect = function(objEvent) {
    if (ThisClass.SelEvent != null) {
      CmpTxtBox.BaseClass.onSelect.call(ThisClass);
    }
  }
  
  this.show = function() {
    CmpTxtBox.BaseClass.show.call(this, null);
    if (this.AutoFocus == true) {
      this.BlkJSX.focus();
    }
  }
  // override hide - do nothing
  this.hide = function() {}
  // override repaint
  this.repaint = function() {}
}

//---------------------------------------  
CmpTxtBoxWithEnterKey.prototype = new Component();                                 
function CmpTxtBoxWithEnterKey(BlkName, cmpVal, AutoFocus, EventArr, SelEvent, onFocusVal) {
  CmpTxtBoxWithEnterKey.BaseClass = CmpTxtBoxWithEnterKey.prototype;
  CmpTxtBoxWithEnterKey.prototype.constructor = CmpTxtBoxWithEnterKey;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    this.AutoFocus = AutoFocus;
    this.onFocusVal = onFocusVal;
  }
  var ThisClass = this;
  // extend init
  this.init = function() {
    CmpTxtBoxWithEnterKey.BaseClass.init.call(this);
    // clear any initial selection
    this.BlkJSX.setValue(cmpVal);
  }
  this.show = function() {
    CmpTxtBoxWithEnterKey.BaseClass.show.call(this, null);
    if (this.AutoFocus == true) {
      this.BlkJSX.focus();
    }
    if (this.SelEvent != null) {
      this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
      this.BlkJSX.setEvent("this.onFocus();", jsx3.gui.Interactive.JSXFOCUS);   
      this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXBLUR);
      this.BlkJSX.setEvent("this.onBlur();", jsx3.gui.Interactive.JSXBLUR);     
    }
    this.BlkJSX.onFocus = this.onFocus;
    this.BlkJSX.onBlur = this.onBlur;
    this.BlkJSX.repaint();
  }

  this.onFocus = function (objEvent) {
    if (ThisClass.SelEvent != null) {
      if (ThisClass.onFocusVal != null && ThisClass.BlkJSX.getValue() == cmpVal) {
        ThisClass.BlkJSX.setValue(ThisClass.onFocusVal);
      }
      jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(ThisClass.onSelect));
      jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, eval(ThisClass.onSelect));
      ThisClass.BlkJSX.setSelection();
    }
  }
  
  this.onBlur = function(objEvent) {
    if (ThisClass.SelEvent != null) {
      jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(ThisClass.onSelect));
    }
  }

  this.onSelect = function(objEvent) {
    if (objEvent.event.enterKey()) {
      if (ThisClass.BlkJSX.focus()) {
        if (ThisClass.SelEvent != null) {
          CmpTxtBoxWithEnterKey.BaseClass.onSelect.call(ThisClass);
        }
      }
    }
  }
  // override hide - do nothing
  this.hide = function() {
    if (this.SelEvent != null) {
      jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(this.onSelect));
    }
  }
  // override repaint
  this.repaint = function() {}
}

//---------------------------------------  
CmpTxtBoxWithBlur.prototype = new Component();                                 
function CmpTxtBoxWithBlur(BlkName, cmpVal, EventArr, SelEvent) {
  CmpTxtBoxWithBlur.BaseClass = CmpTxtBoxWithBlur.prototype;
  CmpTxtBoxWithBlur.prototype.constructor = CmpTxtBoxWithBlur;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  var ThisClass = this;
  // extend init
  this.init = function() {
    CmpTxtBoxWithBlur.BaseClass.init.call(this);
    // clear any initial selection
    this.BlkJSX.setValue(cmpVal);
    if (this.SelEvent != null) {
      this.BlkJSX.subscribe(jsx3.gui.Interactive.JSXBLUR, this, "onBlur"); 
    }    
  }
  this.show = function() {
    CmpTxtBoxWithBlur.BaseClass.show.call(this, null);
  }
  this.onBlur = function(objEvent) {
    CmpTxtBoxWithBlur.BaseClass.onSelect.call(ThisClass);
  }
  // override repaint
  this.repaint = function() {}
  
}
//---------------------------------------  
CmpButton.prototype = new Component();
function CmpButton (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpButton.BaseClass = CmpButton.prototype;
  CmpButton.prototype.constructor = CmpButton;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpButton.BaseClass.init.call(this);
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.EXECUTE, this, "onSelect");
  }

  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.EXECUTE, this);
    CmpButton.BaseClass.destroy.call(this);
  }

  this.repaint = function(EventVar, EventVal) {
    //if( this.BlkJSX.instanceOf(jsx3.gui.Button) ) {
    if( this.BlkJSX.setEnabled != null ) {
      if( EventVal != "NONE" ) {
        this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      } else {
        this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
    }
    this.postpaint();
  }
  
  this.onSelect = function() {
    CmpButton.BaseClass.onSelect.call(this, null);
  } 
}

//---------------------------------------  
CmpCheckBox.prototype = new Component();
function CmpCheckBox (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpCheckBox.BaseClass = CmpCheckBox.prototype;
  CmpCheckBox.prototype.constructor = CmpCheckBox;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpCheckBox.BaseClass.init.call(this);
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.TOGGLE);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.TOGGLE, this, "onSelect");
    if(this.SelEvent != null){
      if(this.SelEvent.eventval == true){
        this.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);  
      }
      else{
        this.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
      }
    }
  }
  this.onSelect = function() {
    CmpCheckBox.BaseClass.onSelect.call(this, null);
  }
  this.repaint = function() {
    if(this.SelEvent.eventval != null ) {
      if(eval(this.SelEvent.eventval) == true){
        this.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }else{
        this.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
      }
    }
  }
}

//---------------------------------------  
CmpTab.prototype = new Component();
function CmpTab (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpTab.BaseClass = CmpTab.prototype;
  CmpTab.prototype.constructor = CmpTab;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpTab.BaseClass.init.call(this);
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SHOW);
    this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.SHOW);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
  }
  
  this.onSelect = function() {
    CmpTab.BaseClass.onSelect.call(ThisClass);
  }

}

//---------------------------------------  
CmpRadioButton.prototype = new Component();
function CmpRadioButton (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpRadioButton.BaseClass = CmpRadioButton.prototype;
  CmpRadioButton.prototype.constructor = CmpRadioButton;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpRadioButton.BaseClass.init.call(this);
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.SELECT);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
  }
  
  this.onSelect = function() {
    CmpRadioButton.BaseClass.onSelect.call(ThisClass);
  }

}

//---------------------------------------  
CmpButtonPanel.prototype = new Component();
function CmpButtonPanel (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpButtonPanel.BaseClass = CmpButtonPanel.prototype;
  CmpButtonPanel.prototype.constructor = CmpButtonPanel;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  this.docSrcXML = null;
  
  // extend init
  this.init = function() {
    var SELECTION_SINGLE = 1;
    var SELECTION_MULTI = 2;
    CmpButtonPanel.BaseClass.init.call(this);	
	  if (this.allowMultiSelection == true) {
      this.BlkJSX.setSelectionModel(SELECTION_MULTI);        
    }
    else {
      this.BlkJSX.setSelectionModel(SELECTION_SINGLE);
    }
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");
  }
  
  this.repaint = function() {
    if( this.docSrcXML ) this.BlkJSX.setSourceXML(this.docSrcXML);
    this.postpaint();
  }
  
  this.onResponse = function(docXformResp) {
    this.docSrcXML = docXformResp;
    this.repaint();
  }
  
  // override show
  this.show = function () {
    this.CmpViewState = "VISIBLE";
    this.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.postpaint();
  }

  this.onSelect = function() {
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpButtonPanel.BaseClass.onSelect.call(this);
  }
}

//---------------------------------------  
CmpCanvasEditorWindowPanel.prototype = new Component();
function CmpCanvasEditorWindowPanel (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpCanvasEditorWindowPanel.BaseClass = CmpCanvasEditorWindowPanel.prototype;
  CmpCanvasEditorWindowPanel.prototype.constructor = CmpCanvasEditorWindowPanel;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  this.docSrcXML = null;
  
  // extend init
  this.init = function() {
    CmpCanvasEditorWindowPanel.BaseClass.init.call(this);	
	  if (this.allowMultiSelection == true) {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_MULTI_BLOCK);         
    }
    else {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_BLOCK);
    }
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.WindowFullScreen, this, "onWindowFullScreen");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.WindowOneToOne, this, "onWindowOneToOne");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.WindowMute, this, "onWindowMute");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.WindowVisible, this, "onWindowVisible");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.WindowKbm, this, "onWindowKbm");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasEditorWindowPanel.DeleteBlock, this, "onDeleteBlock");
  }
  
  this.repaint = function() {
    if( this.docSrcXML ) this.BlkJSX.setSourceXML(this.docSrcXML);
    this.postpaint();
  }
  
  this.onResponse = function(docXformResp) {
    this.docSrcXML = docXformResp;
    this.repaint();
  }
  
  this.onSelect = function() {
    this.SelEvent = SelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
  
  this.onWindowFullScreen = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "WindowFullScreen";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
  
  this.onWindowOneToOne = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "WindowOneToOne";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
  
  this.onWindowMute = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "WindowMute";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
  
  this.onWindowVisible = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "WindowVisible";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }

  this.onWindowKbm = function () {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action = "TOPICEVENT";
    newSelEvent.eventval = "jsxid";
    newSelEvent.eventvar = "WindowKbm";
    newSelEvent.topic = topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if (this.selRecordID) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
  
  this.onDeleteBlock = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "WindowDelete";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvasEditorWindowPanel.BaseClass.onSelect.call(this);
  }
}

//--------------------------------------
CmpCanvas.prototype = new Component();
function CmpCanvas (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpCanvas.BaseClass = CmpCanvas.prototype;
  CmpCanvas.prototype.constructor = CmpCanvas;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  this.docSrcXML = null;
  
  // extend init
  this.init = function() {
    var SELECTION_SINGLE = 1;
    var SELECTION_MULTI = 2;
    CmpCanvas.BaseClass.init.call(this);	
	  if (this.allowMultiSelection == true) {
      this.BlkJSX.setSelectionModel(SELECTION_MULTI);        
    }
    else {
      this.BlkJSX.setSelectionModel(SELECTION_SINGLE);
    }
    if (this.allowToggle == true) {
      this.BlkJSX.setSelectionType(com.ipvs.gui.CanvasInterface.SELECTION_TOGGLE);         
    }
    else {
      this.BlkJSX.setSelectionType(com.ipvs.gui.CanvasInterface.SELECTION_NORMAL);
    }
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");
    this.BlkJSX.subscribe(com.ipvs.gui.CanvasInterface.SelectWindow, this, "onSelectWindow");
  }
  
  this.repaint = function() {
    if( this.docSrcXML ) this.BlkJSX.setSourceXML(this.docSrcXML);
    this.postpaint();
  }

  this.onResponse = function (docXformResp) {
    this.docSrcXML = docXformResp;
    if (this.selRecordID) {
      var selRecord = docXformResp.selectSingleNode(".//record[@jsxid='" + this.selRecordID + "']");
      if (!jsx3.util.strEmpty(selRecord)) {
        selRecord.setAttribute("jsxselected","1");
      }
    }
    this.onSelect();
    this.repaint();
  }
  
  this.setBlkJSXSourceXML = function(docSrcXML) {
    if( docSrcXML ) this.BlkJSX.setSourceXML(docSrcXML);
  }
    
  this.onSelect = function() {
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpCanvas.BaseClass.onSelect.call(this);
  }
  
  this.onSelectWindow = function() {
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getWindow(this.selRecordID);
    CmpCanvas.BaseClass.onSelect.call(this);
  }
}


//---------------------------------------  
CmpEndPointPanel.prototype = new Component();
function CmpEndPointPanel (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
 CmpEndPointPanel.BaseClass = CmpEndPointPanel.prototype;
 CmpEndPointPanel.prototype.constructor = CmpEndPointPanel;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);  
  }
  this.docSrcXML = null;
  
  // extend init
  this.init = function() {
    CmpEndPointPanel.BaseClass.init.call(this);
     if (this.allowMultiSelection == true) {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_MULTI_BLOCK);        
    }
    else {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_BLOCK);
    }
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");
    this.BlkJSX.subscribe(com.ipvs.gui.EndPointPanel.DeleteBlock, this, "onDeleteBlock");
    this.BlkJSX.subscribe(com.ipvs.gui.EndPointPanel.Windowing, this, "onWindowing");
    this.BlkJSX.subscribe(com.ipvs.gui.EndPointPanel.SelectConnection , this, "onSelectConnection");
    this.BlkJSX.subscribe(com.ipvs.gui.EndPointPanel.DeleteConnection , this, "onDeleteConnection");
    this.BlkJSX.subscribe(com.ipvs.gui.EndPointPanel.SessionInfo , this, "onSessionInfo");    
  }
  
  this.repaint = function() {
    if( this.docSrcXML ) this.BlkJSX.setSourceXML(this.docSrcXML);
    this.postpaint();
  }
  
  this.onResponse = function(docXformResp) {
    this.docSrcXML = docXformResp;
    this.repaint();
  }
  
  this.onSelect = function() {
    this.SelEvent = SelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }

  this.onWindowing = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "Windowing";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }
    
  this.onDeleteBlock = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT";
    newSelEvent.eventval ="jsxid";
    newSelEvent.eventvar = "DeleteTarget";
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }
    
  this.onSessionInfo = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT"
    newSelEvent.eventval ="jsxsessionid";
    newSelEvent.eventvar ="SessionInfo"
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;         
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getSession(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }
  
  this.onSelectConnection = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT"
    newSelEvent.eventval ="jsxconnectionid";
    newSelEvent.eventvar ="SelectConnection"
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getConnection(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }

  this.onDeleteConnection = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT"
    newSelEvent.eventval ="jsxconnectionid";
    newSelEvent.eventvar ="DeleteConnection"
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getConnection(this.selRecordID);
    CmpEndPointPanel.BaseClass.onSelect.call(this);
  }
   
}


//---------------------------------------  
CmpSessionSrcPanel.prototype = new Component();
function CmpSessionSrcPanel (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
 CmpSessionSrcPanel.BaseClass = CmpSessionSrcPanel.prototype;
 CmpSessionSrcPanel.prototype.constructor = CmpSessionSrcPanel;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);  
  }
  this.docSrcXML = null;
  
  // extend init
  this.init = function() {
    CmpSessionSrcPanel.BaseClass.init.call(this);
     if (this.allowMultiSelection == true) {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_MULTI_BLOCK);        
    }
    else {
      this.BlkJSX.setSelectionModel(com.ipvs.gui.BlockPanel.SELECTION_BLOCK);
    }
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");
    this.BlkJSX.subscribe(com.ipvs.gui.SessionSrcPanel.DeleteConnection , this, "onDeleteConnection"); 
  }
  
  this.repaint = function() {
    if( this.docSrcXML ) this.BlkJSX.setSourceXML(this.docSrcXML);
    this.postpaint();
  }
  
  this.onResponse = function(docXformResp) {
    this.docSrcXML = docXformResp;
    this.repaint();
  }
  
  this.setBlkJSXSourceXML = function(docXformResp) {
    if( docXformResp ) this.BlkJSX.setSourceXML(docXformResp);
  }
    
  this.onSelect = function() {
    this.SelEvent = SelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
    CmpSessionSrcPanel.BaseClass.onSelect.call(this);
  }

  this.onDeleteConnection = function() {
    var topic = this.SelEvent.topic;
    var newSelEvent = new Object();
    newSelEvent.action ="TOPICEVENT"
    newSelEvent.eventval ="jsxconnectionid";
    newSelEvent.eventvar ="DeleteConnection"
    newSelEvent.topic =topic;
    this.SelEvent = newSelEvent;
    this.selRecordID = this.BlkJSX.getValue();
    if( this.selRecordID ) this.selRecord = this.BlkJSX.getConnection(this.selRecordID);
    CmpSessionSrcPanel.BaseClass.onSelect.call(this);
  }
   
}



//---------------------------------------  
CmpMenu.prototype = new Component();
function CmpMenu (BlkName, MenuItemList, ObjectNID, MenuItemXPath, MtxObjectList, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpMenu.BaseClass = CmpMenu.prototype;
  CmpMenu.prototype.constructor = CmpMenu;
  // stash value of this in local var for access from onselect function
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  this.MenuItemList = MenuItemList;
  this.ObjectNID = ObjectNID;
  this.MenuItemXPath = MenuItemXPath;
  this.MtxObjectList = MtxObjectList;
  
  // extend init
  this.init = function() {
    CmpMenu.BaseClass.init.call(this);
    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
    this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.EXECUTE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
  }
  
  // extend repaint check
  this.repaintCheck = function () {
    // return true if object NID changed
    if (this.ObjectNID != null) {
      if (this.ObjectNID != this.LastObjectNID) {
        this.LastObjectNID = this.ObjectNID;
        return true;
      }
    }
    // else do the basic checking
    var ret = true; // CmpMenu.BaseClass.repaintCheck.call(this);
    return ret;
  }
  
  // override repaint
  this.repaint = function (EventVar, EventVal) {
    // This selectedObjectType will get the MenuList from the this.MenuItemList
    var selectedObjectType = EventVal;
    var MenuItemList = [];
    var DynamicItemList = [];
    if (this.MenuItemXPath != null && this.MtxObjectList != null) {
      if (jsx3.util.strEmpty(selectedObjectType)) return;
      // get list of commands and add them to the menu
      if (this.BlkJSX.clearXmlData != null) {
        this.BlkJSX.clearXmlData();
      }
      else if (this.BlkJSX.resetData != null) {
        this.BlkJSX.resetData();
      }

      var defMenuItemXPath = this.MenuItemXPath;
      var menuItemXPath = defMenuItemXPath.replace("_DEVICETYPE_", selectedObjectType);
      // get menu items from object node and paint
      var objectNode = topic_GetNode(tCache, menuItemXPath);
      if (objectNode == null) return false;

      var objCmdIter = objectNode.selectNodeIterator("./record");
      while (objCmdIter.hasNext()) {
        var objRecord = objCmdIter.next();
        var itemName = objRecord.getAttribute("jsxtext");
        var showDivider = objRecord.getAttribute("jsxShowAfterDivider");
        var parentID = objRecord.getAttribute("jsxParentID");
        var hasChildren = objRecord.getAttribute("jsxHasChildren");
        var showOffline = objRecord.getAttribute("jsxShowOffline");

        if (this.MtxObjectList.selRecord.assetStatus == "Offline" && showOffline == "yes") {
          var o = new Object();
          o.jsxid = (hasChildren == "yes") ? objRecord.getAttribute("jsxid") : jsx3.xml.CDF.getKey();
          o.jsxtext = (hasChildren == "yes") ? itemName : itemName + "...";
          o.jsxdivider = showDivider;
          this.BlkJSX.insertRecord(o, parentID, false);
        }
        else if(this.MtxObjectList.selRecord.assetStatus == "Offline" && showOffline == "no") {
        }
        else {
          var o = new Object();
          o.jsxid = (hasChildren == "yes") ? objRecord.getAttribute("jsxid") : jsx3.xml.CDF.getKey();
          o.jsxtext = (hasChildren == "yes") ? itemName : itemName + "...";
          o.jsxdivider = showDivider;
          this.BlkJSX.insertRecord(o, parentID, false);
        }

      }
    }
    else if (this.MenuItemList != null) {
      // paint list directly
      if (this.BlkJSX.clearXmlData != null) {
        this.BlkJSX.clearXmlData();
      }
      else if (this.BlkJSX.resetData != null) {
        this.BlkJSX.resetData();
      }
      for (var iter = 0; iter < this.MenuItemList.length; iter++) {
        var menuObjectType = this.MenuItemList[iter].ObjectType;
        if (menuObjectType == selectedObjectType) {
          MenuItemList = this.MenuItemList[iter].MenuListArr;
          DynamicItemList = this.MenuItemList[iter].DynamicListArr;
        }
      }
      var tempArr = [];
      for (var i = 0; i < MenuItemList.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        var menuName = MenuItemList[i];
        var showDivider = menuName.match(/--/);
        var showGroup = menuName.match(/,/);
        if (showDivider) {
          menuName = menuName.replace(/--/, "");
          o.jsxdivider = "1";
        }
        tempArr.push({ "Type": menuName, "ID": o.jsxid });
        var parentID = null;
        if (showGroup) {
          grpVal = menuName.split(/,/);
          menuName = grpVal[1];
          var GroupName = grpVal[0];
          for (var j = 0; j < tempArr.length; j++) {
            if (tempArr[j].Type == GroupName) {
              parentID = tempArr[j].ID;
              break;
            }
          }

        }
        o.jsxtext = menuName;
        this.BlkJSX.insertRecord(o, parentID, false);
      }

      if (DynamicItemList) {
        for (var i = 0; i < DynamicItemList.length; i++) {
          var o = new Object();
          o.jsxid = jsx3.xml.CDF.getKey();
          var menuName = eval(DynamicItemList[i]);
          var showDivider = menuName.match(/--/);
          var showImg = menuName.match(/:/);
          if (showDivider) {
            menuName = menuName.replace(/--/, "");
            o.jsxdivider = "1";
          }
          if (showImg) {
            var dArr = menuName.split(":");
            menuName = dArr[0];
            o.jsximg = dArr[1];
          }
          tempArr.push({ "Type": menuName, "ID": o.jsxid });

          o.jsxtext = menuName;
          this.BlkJSX.insertRecord(o, null, false);
        }
      }

    }
    this.postpaint();
  }
  
  this.onSelect = function() {
    ThisClass.selRecordID = ThisClass.BlkJSX.getValue();
    if( ThisClass.selRecordID != null ) {     
      ThisClass.selRecord = ThisClass.BlkJSX.getRecord(ThisClass.selRecordID);
      CmpMenu.BaseClass.onSelect.call(ThisClass);
    }
  }

}

//---------------------------------------  
CmpImageButton.prototype = new Component();
function CmpImageButton (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpImageButton.BaseClass = CmpImageButton.prototype;
  CmpImageButton.prototype.constructor = CmpImageButton;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpImageButton.BaseClass.init.call(this);
    // setup the select event handler
    // is this event TOGGLE if the btn type is toggle?
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.EXECUTE, this, "onSelect");
  }

  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.EXECUTE, this);
    CmpImageButton.BaseClass.destroy.call(this);
  }
  // override show
  this.show = function() {
    if( this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.postpaint();
  }
  // override hide
  this.hide = function() {
    if( this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.postpaint();
  }
  // extend onselect
  this.onSelect = function() {
    // if it is a toggle button - toggle the state
    // do we need to do this - or is it managed automatically?
    /*
    if( this.BlkJSX.getType() == jsx3.gui.ImageButton.TYPE_TOGGLE ) {
      if( this.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_ON ) {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      else {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
    }
    */
    CmpImageButton.BaseClass.onSelect.call(this);
  }

  this.toggle = function() {
    if( this.BlkJSX.getType() == jsx3.gui.ImageButton.TYPE_TOGGLE ) {
      if( this.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_ON ) {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      else {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
      this.onSelect();
    }
  }
}

//Image button to behave like button
//---------------------------------------  
CmpImageButtonButton.prototype = new Component();
function CmpImageButtonButton(BlkName, EventArr, SelEvent) {
  // setup inheritence and call base constructor
  CmpImageButtonButton.BaseClass = CmpImageButtonButton.prototype;
  CmpImageButtonButton.prototype.constructor = CmpImageButtonButton;
  if (BlkName != null) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
  CmpImageButtonButton.BaseClass.init.call(this);
    // setup the select event handler
    // is this event TOGGLE if the btn type is toggle?
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.EXECUTE);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.EXECUTE, this, "onSelect");
  }

  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.EXECUTE, this);
    CmpImageButtonButton.BaseClass.destroy.call(this);
  }
  
  // override repaint
  this.repaint = function(EventVar, EventVal) {
    //if( this.BlkJSX.instanceOf(jsx3.gui.Button) ) {
    if (this.BlkJSX.setEnabled != null) {
      if (EventVal != "NONE") {
        this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      } else {
        this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
    }
    this.postpaint();
  }

  // extend onselect
  this.onSelect = function() {
    CmpImageButton.BaseClass.onSelect.call(this);
  }

  this.toggle = function() {
    if (this.BlkJSX.getType() == jsx3.gui.ImageButton.TYPE_TOGGLE) {
      if (this.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_ON) {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      else {
        this.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
      this.onSelect();
    }
  }
}

//---------------------------------------
CmpList.prototype = new Component();
function CmpList (BlkName, ObjectList, RecordArr, EventArr, SelEvent, MenuEvent, SpyGlassEventFnc, setCanDrag) {
// setup inheritence and call base constructor
  CmpList.BaseClass = CmpList.prototype;
  CmpList.prototype.constructor = CmpList;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    this.ObjectList = ObjectList;
    this.RecordArr = RecordArr;
    this.selRecord = null;   
    this.selRecordID = null;
    this.MenuEvent = MenuEvent;
    this.menuRecordID = null;
    this.menuRecord = null;
    this.handleScroll = null;
    this.SpyGlassEventFnc = SpyGlassEventFnc;
    this.setCanDrag = setCanDrag;
    this.MultiRowsSelected = false;
    this.rowDeselection = false;
    this.HScrollPos = 0;
  }
      
  var ThisClass = this;
  
  // extend init
  this.init = function() {
    CmpList.BaseClass.init.call(this);
    // clear out any hard coded data
    this.BlkJSX.setXMLString(null);
    
    // clear any initial selection
    this.BlkJSX.setValue(null);
    this.MultiRowsSelected = false;
    // setup the selection model for matrix
    if (!jsx3.gui.Matrix) { jsx3.require("jsx3.gui.Matrix"); }
    if( this.BlkJSX.instanceOf(jsx3.gui.Matrix) ) {
      if( this.allowMultiSelection == true ) {
        this.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_MULTI_ROW);
        this.MultiRowsSelected = true;
      }
      else {
        this.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
      }
      this.BlkJSX.setFocusStyle("font-weight:normal");
    }

    // setup the select event handler  
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.SELECT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.SELECT, this, "onSelect");

    // check for any formathandler to be set
    // matrix column Name and CDF attribute name are same
    if( this.RecordArr != null ) {
      for( var colName in this.RecordArr ) {
        if( this.RecordArr[colName].objxpath != null ) {
          if( this.RecordArr[colName].formathandler != null ) {
            var colObj = this.BlkJSX.getDescendantOfName(colName, false, false);
            //var fname = "FncName";
            var formatHandlerFnc = this.RecordArr[colName].formathandler["FncName"];
            colObj.setFormatHandler(formatHandlerFnc);
          }
        }
      }
    }
        
    // setup the menu event handler if needed
    if( this.MenuEvent != null ) {
      this.BlkJSX.removeEvent(jsx3.gui.Interactive.MENU);
      this.BlkJSX.setEvent("this.onMenu(strRECORDID);", jsx3.gui.Interactive.MENU);
      // the above event will fire in the context of the blk - so we setup its onMenu to point to ours
      this.BlkJSX.onMenu = this.onMenu;
    }
    //Spyglass
    if(this.SpyGlassEventFnc != null){
      this.BlkJSX.removeEvent(jsx3.gui.Interactive.SPYGLASS);
      this.BlkJSX.setEvent('eval("' + this.SpyGlassEventFnc+ '(this, strRECORDID);")', jsx3.gui.Interactive.SPYGLASS);
    }
    // setCanDrag
    if(this.setCanDrag == true) {
      this.BlkJSX.setCanDrag(jsx3.Boolean.TRUE);
    } 
    else {
      this.BlkJSX.setCanDrag(jsx3.Boolean.FALSE);
    }
  }

  this.clearRefresh = function() {
    if( this.ObjectRefreshArr != null ) {
      for( var AgentObj in this.ObjectRefreshArr ) {
        for( var AgentJID in this.ObjectRefreshArr[AgentObj] ) {
          clearAgentObjRequest(AgentJID, AgentObj)
        }
      }
    }
  }

  this.destroy = function() {
    // clear refresh
    this.clearRefresh();
    this.CellRefreshArr = null;
    this.ObjectRefreshArr = null;    
    
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.SELECT, this);
    //TBD clear out subscriptions for row data topic events

    //Clear out the data painted in component
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.repaint();
    CmpList.BaseClass.destroy.call(this);
  }
  
  // extend show
  this.show = function(EventVar, EventVal) {
    CmpList.BaseClass.show.call(this, EventVar, EventVal);
    // setup timers for object refresh arr

    var BlkJSXDisplayState = this.BlkJSX.getDisplay();
    if( BlkJSXDisplayState == "none") return;
    if( this.ObjectRefreshArr != null ) {
      for( var AgentObj in this.ObjectRefreshArr ) {
        for( var AgentJID in this.ObjectRefreshArr[AgentObj] ) {
          if ( this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID == null ) {
            var Interval = this.ObjectRefreshArr[AgentObj][AgentJID].RefreshInterval;
            var status = this.ObjectRefreshArr[AgentObj][AgentJID].Status;
            Interval = parseInt(Interval)*1000;
            this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID = setAgentReqInterval(Interval, AgentJID, AgentObj, status);
            //send request anyway dont wait for the 1 request to be send after the interval
            sendAgentObjRequest(AgentJID, AgentObj, status);
          }
          if( arrAgentObjectInfo[AgentObj].reqAgentJID != "AppServerAgentJID" ) {
            var AgentObjectReqSuccessEventVar = arrAgentObjectInfo[AgentObj].successevent.eventvar;
            var AgentObjectReqSuccessEventVal = arrAgentObjectInfo[AgentObj].successevent.eventval;
            
            var AgentObjectReqTopic = AgentJID;
            if( this.eventHandlerArr[AgentObjectReqTopic] == null ) {
              this.eventHandlerArr[AgentObjectReqTopic] = new EventListner( AgentObjectReqTopic );
              //subscription should be set once
              var refreshRowFnc = new refreshRow(this, AgentJID);
              this.eventHandlerArr[AgentObjectReqTopic].subscribe(AgentObjectReqSuccessEventVar, 
                                                                  null,
                                                                  refreshRowFnc, refreshRowFnc.refreshCol);            
            }
          }
        }
      }
    }
    //set the refresh for the pagination matrix
    if( (!jsx3.util.strEmpty(this.RefreshInterval)) &&
        (jsx3.util.strEmpty(this.IntervalID)) ) {
      this.IntervalID = setInterval(this.sendRequest, this.RefreshIntervalTime);  
    }   
    if(this.CmpViewState == "VISIBLE"){
      if( this.BlkJSX.instanceOf(jsx3.gui.Matrix) ) {
        this.BlkJSX.getParent().removeEvent(jsx3.gui.Interactive.AFTER_RESIZE_VIEW);
        this.BlkJSX.getParent().subscribe(jsx3.gui.Interactive.AFTER_RESIZE_VIEW, this, "handleResize");
      }
    }
    this.setScrollPosition();   
  }
  
  // override hide - do nothing
  this.hide = function() {
    var BlkJSXDisplayState = this.BlkJSX.getDisplay();
    if( BlkJSXDisplayState == "none") return;
    // clear timers for object refresh arr
    if( this.ObjectRefreshArr != null ) {
      for( var AgentObj in this.ObjectRefreshArr ) {
        for( var AgentJID in this.ObjectRefreshArr[AgentObj] ) {
          if ( this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID != null ) {
            clearAgentReqInterval(this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID);
            this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID = null;
          }
        }
      }
    }
    // clear the refresh interval
      if( (!jsx3.util.strEmpty(this.RefreshInterval)) &&
          (!jsx3.util.strEmpty(this.IntervalID)) ) {
      clearInterval(this.IntervalID);
      this.IntervalID = null;      
    }
    this.BlkJSX.getParent().unsubscribe(jsx3.gui.Interactive.AFTER_RESIZE_VIEW, this);
    this.getScrollPosition();
    CmpList.BaseClass.hide.call(this); 
  }
  
  // override repaint
  this.repaint = function() {
    if( this.ObjectList == null ) return;
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // iter over object list
    for(var i=0; i<this.ObjectList.length; i++) {
      if( this.ObjectList[i] == null ) continue;
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      this.BlkJSX.insertRecord(o, null, false);
      this.objToBlkJSXRecord(this.ObjectList[i], o.jsxid);
    }
    this.postpaint();
    this.onSelect();
  }

  // override post paint function
  this.postpaint = function() {
    if( this.BlkJSX == null ) return;
    this.handleScrollbars(); 
    this.BlkJSX.repaint();    
    this.PaintState = true;
  }

  this.getScrollPosition = function() {
    if( this.BlkJSX.instanceOf(jsx3.gui.Matrix) ) {
      this.HScrollPos = this.BlkJSX.getScrollTop();
    }
  }
  
  this.setScrollPosition = function() {
    if( this.BlkJSX.instanceOf(jsx3.gui.Matrix) ) {
      this.BlkJSX.setScrollTop(this.HScrollPos);
    }   
  }
  
  // On Resize of the Browser window this class is triggered
  this.handleResize = function(){
    ThisClass.BlkJSX.repaint();
    ThisClass.handleScrollbars();
    ThisClass.BlkJSX.repaint();
  }

  this.handleScrollbars = function(){
    if( this.BlkJSX.instanceOf(jsx3.gui.Matrix) ) {
      try{
        this.setScrollPosition();
        var objHeight = this.BlkJSX.getRendered().offsetHeight;
        var rowHeight = this.BlkJSX.getRowHeight(jsx3.gui.Matrix.DEFAULT_ROW_HEIGHT);
        var headerHeight = this.BlkJSX.getHeaderHeight(jsx3.gui.Matrix.DEFAULT_HEADER_HEIGHT);
        var availableHeight = objHeight - headerHeight;
        var noOfRows = this.BlkJSX.getSortedIds().length;
        var requiredHeight = noOfRows * rowHeight;
        this.BlkJSX.setSuppressVScroller(jsx3.Boolean.FALSE, true);
        if( availableHeight > requiredHeight ) {
          if( this.handleScroll != false ){
            //this.BlkJSX.setSuppressVScroller(jsx3.Boolean.TRUE, true);
          }
        }
      }catch(e){
        log.error("clsComponent::CmpList:handleScrollbars " + e.message);
      }
    }
  }

  this.objToBlkJSXRecord = function(obj, RecordID) {
    var o = this.BlkJSX.getRecordNode(RecordID);
    // for each object iter over list columns
    for( var colName in this.RecordArr ) {
      var colObjectProp = this.RecordArr[colName];
      o.setAttribute(colName,obj[colObjectProp]);

    }       
  }

  this.deSelect = function(resetMode) {
    this.selRecordID = null;
    if( (this.SelEvent == null) || 
        (this.SelEvent.persist == null) || 
        (this.SelEvent.persist.noreset != true) || 
        (resetMode == true) ) { 
      this.selRecord = null;
    }
    if(this.BlkJSX.deselectAllRecords != null) {
      this.BlkJSX.deselectAllRecords();
    }
  }

  // extend onselect - save selected record  
  this.onSelect = function() {
    function setSelection(SelMatchStr) {
      if( SelMatchStr != null ) {
        var selMatchRecords = this.BlkJSX.getXML().selectNodes(SelMatchStr);
        var selectedIDsArr = new Array();
        for (var it = selMatchRecords.iterator();it.hasNext();){
          var selMatchRecord = it.next();
          if( selMatchRecord != null ) {
            selectedIDsArr.push(selMatchRecord.getAttribute("jsxid"));  
          }
        }
        if( selectedIDsArr.length > 0 ) {
          //Selection-model = multi-row; Multi rows selected
          if( this.MultiRowsSelected == true ) {
            this.selRecordID = selectedIDsArr;
            this.BlkJSX.setValue(this.selRecordID);
            this.selRecord = this.BlkJSX.getSelectedNodes();
          }
          //Selection-model = multi-row; Single row selected
          else if( this.allowMultiSelection == true ) {
            this.selRecordID = selectedIDsArr;
            this.BlkJSX.setValue(this.selRecordID);
            this.selRecord = this.BlkJSX.getRecord(this.selRecordID[0]);
          }
          //Selection-model = single-row;
          else {
            this.selRecordID = selectedIDsArr[0];
            this.BlkJSX.setValue(this.selRecordID);
            this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
          }
          return true;
        }
      }
      this.deSelect();
      return false;
    }
    function doSelectEvent() {
      var Val = null;
      if( this.selRecordID == null ) {
        Val = "NONE";
      }
      else if( this.MultiRowsSelected == true && this.selRecordID.length != 0 ) {
        Val = "MULTI-ROWS";
      }
      
      CmpList.BaseClass.onSelect.call(this, Val);      
    }
    //@selRecordID -- is array if list selection model is multi-row otherwise it is string
    this.selRecordID = this.BlkJSX.getValue();

    // if not selected - persist/init it
    if( this.selRecordID == null || ( this.allowMultiSelection == true && this.selRecordID.length == 0) ) {
      // check if a persist(+init) clause has been specified on the select event
      if( this.SelEvent != null && this.SelEvent.persist != null ) {
        var colVal = null;
        var SelMatchStr = null;
        // if there is already a selected record (ie - persiting an existing selection)
        // unless a "reset" has been specified - in that case ignore selection and reset to initval
        if( this.selRecord != null ) {
          if( this.SelEvent.persist.reset == true && this.SelEvent.persist.initval != null) {
            // use value from initval
            colVal = eval(this.SelEvent.persist.initval);
          }
          else{
            if( this.MultiRowsSelected == true ) {
              // use value from previous selection
              for (var it = this.selRecord.iterator();it.hasNext();){
                var objRecordNode = it.next();
                if( colVal == null ) {
                  colVal = "'" + objRecordNode.getAttribute(this.SelEvent.persist.col) + "'";
                }
                else {
                  colVal = colVal + " or @" + this.SelEvent.persist.col + "='" + objRecordNode.getAttribute(this.SelEvent.persist.col) + "'";
                }
              }
            }
            else {
              colVal = "'" + this.selRecord[this.SelEvent.persist.col] + "'";
            }
          }
          SelMatchStr = "//record[@" + this.SelEvent.persist.col + "=" + colVal + "]";
          var persist = setSelection.call(this, SelMatchStr);
          if( persist != true && this.SelEvent.persist.noreset != true ) {
            doSelectEvent.call(this);
          }
        }
        // else if initing the selection - do it based on the initval spec
        else {
          if( this.SelEvent.persist.initval == true ) {
            if( this.SelEvent.persist.initxpath != null ) {
              SelMatchStr = "//record[@" + this.SelEvent.persist.initxpath +"]";
            }
            else {
              colVal = 1;
              SelMatchStr = "//record[@" + this.SelEvent.persist.col + "][" + colVal +"]";
            }
          }
          else if( this.SelEvent.persist.initval == "SORT" ) {
            var arrRecordId =  this.BlkJSX.getSortedIds();
            colVal = arrRecordId[0];
            SelMatchStr = "//record[@" + "jsxid" + "='" + colVal + "']";
          }
          else if( this.SelEvent.persist.initval != null ) {
            colVal = eval(this.SelEvent.persist.initval);
            SelMatchStr = "//record[@" + this.SelEvent.persist.col + "='" + colVal + "']";
          }
          else {
            SelMatchStr = null;
          }
          setSelection.call(this, SelMatchStr);
          doSelectEvent.call(this);
        }
      }
      else {
        setSelection.call(this, null);
        doSelectEvent.call(this);
      }
    }
    else {
      //Selection-model = multi-row;
      if( this.allowMultiSelection == true ) {
        //Selection-model = multi-row; Single row selected
        if( this.selRecordID.length == 1 ) {
          this.MultiRowsSelected = false;
          this.selRecord = this.BlkJSX.getRecord(this.selRecordID[0]);
        }
        //Selection-model = multi-row; Multi rows selected
        else {
          this.MultiRowsSelected = true;
          this.selRecord = this.BlkJSX.getSelectedNodes();
        }
      }
      //Selection-model = single-row;
      else {
        this.MultiRowsSelected = false;
        this.selRecord = this.BlkJSX.getRecord(this.selRecordID);
      }
      
      doSelectEvent.call(this);
    }    
  }
  // handle right click
  this.onMenu = function(RecordID) {
    var ret = false;
    // checking if record has been selected. Show menu only when record is selected
    if(ThisClass.selRecordID == RecordID) {
      // get the current object and fire menu event
      ThisClass.menuRecordID = RecordID;
      if( ThisClass.menuRecordID != null ) {     
        ThisClass.menuRecord = ThisClass.BlkJSX.getRecord(ThisClass.menuRecordID);
        ret = CmpList.BaseClass.onMenu.call(ThisClass, ThisClass.menuRecordID);
      }
    }
    return ret;
  } 
  
  // redraw row/column cell 
  // function used in setInterval() in the show/hide of the blk
  refreshRow = function(ThisClass, AgentJID) {
    this.AgentJID = AgentJID;
    this.ThisClass = ThisClass;
    
    this.refreshCol = function(EventVar, EventVal) {
      if( this.ThisClass.CellRefreshArr != null ) {
        // getting the cdf doc for the blkjsx (ie matrix)
        var strXMLCacheId = this.ThisClass.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
        for( var AgentJID in this.ThisClass.CellRefreshArr ) {
          if( AgentJID == this.AgentJID ) {
            for( var ColName in this.ThisClass.CellRefreshArr[AgentJID] ) {
              var AgentObj = this.ThisClass.CellRefreshArr[AgentJID][ColName].AgentObj;
              var cellNode = null;
              var cellValue = "-";
              if( AgentObj == EventVar ) {
                var ColObj = this.ThisClass.CellRefreshArr[AgentJID][ColName].ColObj;
                // get xpath for the col and then value from the topic, then set to the record
                var ColXPath = this.ThisClass.CellRefreshArr[AgentJID][ColName].ColObjXPath;
                var RowID = this.ThisClass.CellRefreshArr[AgentJID][ColName].RowID;
                try {
                  cellNode = topic_GetNode(AgentJID, ColXPath);
                  if( !jsx3.util.strEmpty(cellNode) ){
                    cellValue = cellNode.getValue();
                  }
                } catch(e) {
                }
                if( objXML ) {
                  var objRecordNode = objXML.selectSingleNode("//record[@jsxid='" + RowID + "']");
                  if( !jsx3.util.strEmpty(objRecordNode) ){
                    if(ColObj.jsxvaluetemplate == "@image" && cellValue == "-"){
                    }
                    else {
                      objRecordNode.setAttribute(ColName, cellValue);
                    }
                  }
                }
                if(ColObj.jsxvaluetemplate == "@image" && cellValue != "-"){
                  this.ThisClass.BlkJSX.redrawRecord(RowID,jsx3.xml.CDF.UPDATE);
                }
                else {
                  this.ThisClass.BlkJSX.redrawCell(RowID,ColObj ,"");
                }
              }
            }
          }
        }
      }
    }    
  }  
}

//---------------------------------------
// this can get cleaned out by changing all refs to just CmpList
CmpMenuList.prototype = new CmpList();
function CmpMenuList (BlkName, ObjectList, RecordArr, EventArr, SelEvent, MenuEvent) {
// setup inheritence and call base constructor
  CmpMenuList.BaseClass = CmpMenuList.prototype;
  CmpMenuList.prototype.constructor = CmpMenuList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, ObjectList, RecordArr, EventArr, SelEvent, MenuEvent);
  }
}

//---------------------------------------
CmpStaticList.prototype = new CmpList();
function CmpStaticList (BlkName, ObjectListXMLURL, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpStaticList.BaseClass = CmpStaticList.prototype;
  CmpStaticList.prototype.constructor = CmpStaticList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent);
    this.ObjectListXMLURL = ObjectListXMLURL;
  }
  
  this.init = function() {
    CmpStaticList.BaseClass.init.call(this);
    this.BlkJSX.resetCacheData();
    this.BlkJSX.setXmlAsync(false);
    this.BlkJSX.setXMLURL(this.ObjectListXMLURL);
  }

  // redefine show
  this.repaint = function() {
    if( this.SelEvent.persist != null ){
      this.BlkJSX.setValue(null);
    }
    this.onSelect();
  }

}

//---------------------------------------
CmpSelect.prototype = new CmpList();
function CmpSelect (BlkName, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpSelect.BaseClass = CmpSelect.prototype;
  CmpSelect.prototype.constructor = CmpSelect;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent);
  }
  
  this.init = function() {
    CmpSelect.BaseClass.init.call(this);
    this.BlkJSX.resetCacheData();
  }

  // redefine show
  this.repaint = function() {
    if( this.SelEvent.persist != null ){
      this.BlkJSX.setValue(null);
    }
    this.onSelect();
  }

}

//---------------------------------------
CmpNIDGroupTagNameList.prototype = new CmpList();
function CmpNIDGroupTagNameList (BlkName, ObjectNIDGroupTree, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpNIDGroupTagNameList.BaseClass = CmpNIDGroupTagNameList.prototype;
  CmpNIDGroupTagNameList.prototype.constructor = CmpNIDGroupTagNameList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent);
    this.ObjectNIDGroupTree = ObjectNIDGroupTree;
  }
  
  // extend show - 
  this.show = function(EventVar, EventVal) {
    log.debug("Show: SUB: Topic: " + this.ObjectNIDGroupTree + " Var: " + EventVar + " Val: " + EventVal);
    CmpNIDGroupTagNameList.BaseClass.show.call(this, EventVar, EventVal);
  }
    
  // redefine show
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null; 
    // get group tree node
    var objectNIDGroupTreeNode = topic_GetNode(tApp, "//" + this.ObjectNIDGroupTree);
    var treeNodeClone  = objectNIDGroupTreeNode.cloneNode(true);
    log.debug("Repaint: SUB: Topic: " + this.ObjectNIDGroupTree + " Val: " + treeNodeClone.toString());    
    var objectGroupTagNameListIter = objectNIDGroupTreeNode.selectNodeIterator(".//ObjectNIDTree/*");
    while( objectGroupTagNameListIter.hasNext() ) {
      var groupNode = objectGroupTagNameListIter.next();
      var GroupName = groupNode.getAttribute("Name");
      var o = new Object();
      // use groupID for jsxid
      o.jsxid = jsx3.xml.CDF.getKey();;
      o.jsxtext = GroupName;
      this.BlkJSX.insertRecord(o, null, false);
    }
    this.postpaint();
    this.onSelect();
  }
}

//---------------------------------------
CmpNIDGroupTagValueList.prototype = new CmpList();
function CmpNIDGroupTagValueList(BlkName, ObjectNIDGroupTree, GroupTagName, EventArr, SelEvent, ListType, TreeRootIconImage, TreeLeafIconImage) {
// setup inheritence and call base constructor
  CmpNIDGroupTagValueList.BaseClass = CmpNIDGroupTagValueList.prototype;
  CmpNIDGroupTagValueList.prototype.constructor = CmpNIDGroupTagValueList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent);
    this.ObjectNIDGroupTree = ObjectNIDGroupTree;
    this.GroupTagName = GroupTagName;
    this.ListType = ListType;
  }
  
  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }

    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
 
    var ParentID = null;
    var ParentName = null;
    // insert the ALL group
    if( this.ListType != "SIMPLELIST" ) {
      var o = new Object;
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxtext = "_ALL_";
      // setup parent node if tree mode
      if( this.ListType == "TREE" ) {
        o.jsxpath = o.jsxtext;
        o.jsxopen = "1";
        if (TreeRootIconImage != null) {
            o.jsximg = TreeRootIconImage;
        } else {
          o.jsximg = "images/icon_7.gif";
        }

        ParentID = o.jsxid;

        this.BlkJSX.insertRecord(o, "jsxroot", false);
      } else {
        this.BlkJSX.insertRecord(o, null, false);
      }
    }

    // get group tree node
    var objectNIDGroupTreeNode = topic_GetNode(tApp, "//" + this.ObjectNIDGroupTree);
    var GroupTagName = "_ALL_";
    if( this.GroupTagName != null ) {
      GroupTagName = topic_Get(tApp, this.GroupTagName);
    }
    var objectGroupTagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree");
    if( GroupTagName != "_ALL_" ) {
      objectGroupTagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree/Group[@Name='" + GroupTagName + "']");
    }
    if( objectGroupTagNameNode != null ) {
      this.addChildRecords(objectGroupTagNameNode, ParentID, ParentName);         
   }
  
    // repaint
    this.postpaint();
    this.onSelect();  
    var strXMLCacheId = this.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);    

  }
  
  // helper function to recurse thru the records
  this.addChildRecords = function(groupNode, ParentID, ParentName) {
    var LeafNode = true;
    var groupListIter = groupNode.selectNodeIterator("./*");
    while( groupListIter.hasNext() ) {
      groupNode = groupListIter.next();
      var NodeName = groupNode.getNodeName();
      if( NodeName == "ObjectNID" ) continue;
      var GroupName = groupNode.getAttribute("Name");
      if( this.ListType == "SIMPLELIST" ) {
        if( GroupName == "_ALL_" || GroupName == "_unassigned_" ) {
          continue;
        }
      }
      var GroupPath = GroupName;
      if( ParentName != null ) {
        GroupPath = ParentName + "/" + GroupName;
      }
      var GroupID = jsx3.xml.CDF.getKey();
      LeafNode = false;
      var o = new Object;
      if( this.ListType == "TREE" ) {
        o.jsxid = GroupID;
        o.jsxtext = GroupName;
        o.jsxpath = GroupPath;
        if (TreeLeafIconImage != null) {
            o.jsximg = TreeLeafIconImage;
        } else {
            o.jsximg = "images/icon_7.gif";
        }

        this.BlkJSX.insertRecord(o, ParentID, true);
        this.addChildRecords(groupNode, GroupID, GroupPath);   
      }
      else {
        if( this.addChildRecords(groupNode, GroupID, GroupPath) == true ) {
          o.jsxid = GroupID;
          o.jsxtext = GroupPath;
          this.BlkJSX.insertRecord(o, null, false);
        }
      }
    }
    return LeafNode;
  }
}

//---------------------------------------
CmpNIDValueList.prototype = new CmpList();
function CmpNIDValueList(BlkName, ObjectNIDList, ParentNIDXPath, RecordArr, EventArr, SelEvent, SpyGlassEventFnc, setCanDrag, TreeRootIconImage, TreeLeafIconImage)
{
// setup inheritence and call base constructor
  CmpNIDValueList.BaseClass = CmpNIDValueList.prototype;
  CmpNIDValueList.prototype.constructor = CmpNIDValueList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent, null, SpyGlassEventFnc, setCanDrag);
    this.ObjectNIDList = ObjectNIDList;
    this.ParentNIDXPath = ParentNIDXPath;
  }

  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    
    var ParentID = null;
    // insert the ALL group
    if( this.ParentNIDXPath != null ) {
      var o = new Object;
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxtext = "/";
      o.jsxpath = o.jsxtext;
      o.jsxopen = "1";
      o.jsxparentnid = null;
      if (TreeRootIconImage != null)
      {
        o.jsximg = TreeRootIconImage;
      } else
      {
        o.jsximg = "images/icon_7.gif";
      }
      ParentID = o.jsxid;

      this.BlkJSX.insertRecord(o, "jsxroot", true);
    }
    // get object list node
    var objectNIDListNode = topic_GetNode(tApp, "//" + this.ObjectNIDList);
    
    if( objectNIDListNode != null ) {
      this.addChildRecords(objectNIDListNode, ParentID);
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }  
  
  this.DoneList = [];
  this.recordCount = 0;  
  
  // helper function to recurse thru the records
  this.addChildRecords = function(listNode, ParentID) {
    var countListIter = listNode.selectNodeIterator("./*");
    while( countListIter.hasNext() ) {
      countListNode = countListIter.next();
      var ObjectNID = countListNode.getValue();
      
      this.recordCount++;
    }
    
    this.recordParentDirNID = null;
    
    while( this.DoneList.length != this.recordCount ) { //
      var countListIter = listNode.selectNodeIterator("./*");
      while( countListIter.hasNext() ) {
        countListNode = countListIter.next();
        var ObjectNID = countListNode.getValue();
        
        if( this.ParentNIDXPath == null) { //display simplelist
          var pNID = null;
          this.insertData(pNID, ObjectNID);
        } else { // display tree
          if( this.checkPainted(ObjectNID) ) {
            this.recordParentDirNID = this.getParentDirNID(ObjectNID);
            if( (this.recordParentDirNID != "") && (this.recordParentDirNID != "null") && (this.recordParentDirNID != "NONE") ) {
              var recordDirNID = this.getRecordParentID(this.recordParentDirNID);
              if(recordDirNID != null) {
                this.insertData(recordDirNID,ObjectNID);
              } else {
                var checkrecordParentDirNID = this.checkRecordParentDirNID(listNode, this.recordParentDirNID);
                // if the parentMediadir does not exists in the dir list the leave it
                if(!checkrecordParentDirNID){ 
                  this.DoneList.push(ObjectNID);
                }
              }          
            } else {
              var recordParentNID = ParentID;
              this.insertData(recordParentNID,ObjectNID); 
            }
          }
        }
      }
    }
    this.DoneList = [];
    this.recordCount = 0; 
  } 
  
  this.checkRecordParentDirNID = function(listNode, recordDirNID) {
    var countListIter = listNode.selectNodeIterator("./*");
    while( countListIter.hasNext() ) {
      countListNode = countListIter.next();
      var ObjectNID = countListNode.getValue();
      if( ObjectNID == recordDirNID) {
       return true;
      }
    }
    return false;
  }
  
  this.checkPainted = function(ObjectNID) {
    for(var i=0; i< this.DoneList.length; i++) {
      if( ObjectNID == this.DoneList[i]) {
        return false;
      } 
    }
    return true;
  }
  
  
  this.getParentDirNID = function(ObjectNID) {
    var objectNode = topic_GetNIDNode(tCache, ObjectNID);
    var PXPath = (this.ParentNIDXPath);
    var parentDirNode = objectNode.selectSingleNode(PXPath);
    var parentDirNID = parentDirNode.getValue();
    return parentDirNID;
  }
  
  this.getRecordParentID = function(parentMediaDirNID) {
    var strXMLCacheId = this.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordParentNID = objRecordNode.getAttribute("jsxparentnid");
        var strRecordNID = objRecordNode.getAttribute("jsxid");
        if(strRecordNID == parentMediaDirNID){
          return strRecordNID;
        }
      }
    }
    return null;
  }

  this.getRecordParentName = function(parentMediaDirNID) {
    var strXMLCacheId = this.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var strRecordName = "";
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        var strRecordParentNID = objRecordNode.getAttribute("jsxparentnid");
        var strRecordNID = objRecordNode.getAttribute("jsxid");
        if(strRecordNID == parentMediaDirNID){
          strRecordName = objRecordNode.getAttribute("jsxtext");
          return strRecordName;
        }
      }
    }
    return strRecordName;
  }
  this.insertData = function(ParentID, ObjectNID) {
    var objectNode = topic_GetNIDNode(tCache, ObjectNID);
    var o = new Object();
    // use objectNID for jsxid
    o.jsxid = ObjectNID;
    // iter over columns for each record
    for( var colName in RecordArr ) {
      var colValXPath = null;
      if( RecordArr[colName].objxpath != null ) {
        colValXPath = eval(RecordArr[colName].objxpath);
      }
      else {
        colValXPath = eval(RecordArr[colName]);
      }
      var objValNode = objectNode.selectSingleNode(colValXPath);
      if( objValNode != null ) {
        var colVal = objValNode.getValue();
        if( RecordArr[colName].lookuparr != null ) {
          // gets the value from the array, which will be displayed in UI
          colVal = RecordArr[colName].lookuparr[colVal];
        }
        o[colName] = colVal;
        if(colName == "jsxtext"){
          var GroupName = colVal;
        }
      }
    }
    if( this.ParentNIDXPath != null) {
      if (TreeLeafIconImage != null)
      {
        o.jsximg = TreeLeafIconImage;
      } else
      {
        o.jsximg = "images/icon_7.gif";
      }
      var GroupPath = GroupName;
      if( ParentID != null ) {
        var parentName = this.getRecordParentName(ParentID);
        if(parentName == "/") {
          GroupPath = "/" + GroupName;
        } else {
          GroupPath =  parentName + "/" + GroupName;
        }
      }
      o.jsxpath = GroupPath;      
    }

    this.BlkJSX.insertRecord(o, ParentID, true);  
    this.DoneList.push(ObjectNID);
  }
  
}

//---------------------------------------
CmpNIDList.prototype = new CmpList();
function CmpNIDList (BlkName, ObjectNIDGroupTree, GroupTagName, GroupTagValue, RecordArr, EventArr, SelEvent, SpyGlassEventFnc) {
// setup inheritence and call base constructor
  CmpNIDList.BaseClass = CmpNIDList.prototype;
  CmpNIDList.prototype.constructor = CmpNIDList;

  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent, null, SpyGlassEventFnc);
    this.ObjectNIDGroupTree = ObjectNIDGroupTree;
    this.GroupTagName = GroupTagName;
    this.GroupTagValue = GroupTagValue;
  }

  // redefine repaint
  this.repaint = function() {
    //clear rowid array
    this.BlkJSX.RowIDArr = [];
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // get group tree node
    var objectNIDGroupTreeNode = topic_GetNode(tApp, "//" + this.ObjectNIDGroupTree);
    var GroupTagNameStr = "_ALL_";
    if( this.GroupTagName != null ) {
      var topicName = tApp;
      var groupTagName = this.GroupTagName;
      if( this.GroupTagName.match(/:/) ) {
        var topicName = this.GroupTagName.replace(/:\w*/, "");
        var groupTagName = this.GroupTagName.replace(/^.*:/, "");
      }
      GroupTagNameStr = topic_Get(topicName, groupTagName);
    }
    //var objectGroupTagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree");
    var objectGroupTagNameNode = objectNIDGroupTreeNode;
    if( GroupTagNameStr != "_ALL_" ) {
      var tagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree/Group[@Name='" + GroupTagNameStr + "']");
      if( tagNameNode != null ) {
        objectGroupTagNameNode = tagNameNode;
      }
    }
    var GroupTagValueStr = "_ALL_";
    if( this.GroupTagValue != null ) {
      var topicName = tApp;
      var groupTagValue = this.GroupTagValue;
      if( this.GroupTagValue.match(/:/) ) {
        var topicName = this.GroupTagValue.replace(/:\w*/, "");
        var groupTagValue = this.GroupTagValue.replace(/^.*:/, "");
      }
      GroupTagValueStr = topic_Get(topicName, groupTagValue);
    }
    var objectGroupTagValueNode  = objectGroupTagNameNode;
    if( GroupTagValueStr != "_ALL_" ) {
      var GroupTagValueArr = GroupTagValueStr.split("/");
      for( var i=0; i<GroupTagValueArr.length; i++ ) {
        GroupTagValueStr = GroupTagValueArr[i];
        objectGroupTagValueNode = objectGroupTagValueNode.selectSingleNode(".//Group[@Name='" + GroupTagValueStr + "']");
        if( objectGroupTagValueNode == null ) {
           break;
        }
      }    
    }
    if( objectGroupTagValueNode != null ) {
      this.addChildRecords(objectGroupTagValueNode);
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }
  
  // helper function to recurse thru the records
  this.addChildRecords = function(groupNode) {
    //var objectListIter = groupNode.selectNodeIterator("./*");
    var objectListIter = groupNode.selectNodeIterator(".//ObjectNID");
    while( objectListIter.hasNext() ) {
      groupNode = objectListIter.next();
      var NodeName = groupNode.getNodeName();
      if( NodeName != "ObjectNID" ) {
        this.addChildRecords(groupNode);
        continue;
      }
      var ObjectNID = groupNode.getValue();
      var o = this.getChildRecord(ObjectNID);
      if( o != null ) {
        this.BlkJSX.insertRecord(o, null, false);
      }
    }
  }

  // helper function to fill a record from obj array
  this.getChildRecord = function(ObjectNID) {
      var objectNode = topic_GetNIDNode(tCache, ObjectNID);
    if( objectNode == null ) return null;
      var o = new Object();
      // use objectNID for jsxid
      o.jsxid = ObjectNID;
      // iter over columns for each record
      for( var colName in RecordArr ) {
        var colValXPath = null;
        if( RecordArr[colName].objxpath != null ) {
          colValXPath = eval(RecordArr[colName].objxpath);
        }
        else {
          colValXPath = eval(RecordArr[colName]);
        }
        var attributeXPath = colValXPath.search(/\[@/);
        var objValNode = objectNode.selectNodes(colValXPath);
        if (objValNode != null) {
          var colVal = "";
          var numberOfNodes = objValNode.getLength();
          if (numberOfNodes > 1) {
            colVal = RecordArr[colName].lookuparr["MULTI"];
          }
          else{
            if( !jsx3.util.strEmpty(objValNode.get(0)) ) {
              colVal = objValNode.get(0).getValue();
              if(attributeXPath != -1 ){
                var attributeName = colValXPath.replace(/.*\[@(.*)\]/,"$1")
                colVal = objValNode.get(0).getAttribute(attributeName);
              }
              if( RecordArr[colName].lookuparr != null ) {
                // gets the value from the array, which will be displayed in UI
                colVal = RecordArr[colName].lookuparr[colVal];
              }
            }
          }
          o[colName] = colVal;
        }
      }
    return o;
  }

  // these functions will add/update/delete just individual NID records
  this.addRecord = function(ObjectNID) {
    var o = this.getChildRecord(ObjectNID);
    if (o != null) {
      this.BlkJSX.insertRecord(o);
      this.handleScrollbars();
    }
  }
  this.updateRecord = function(ObjectNID) {
    var o = this.getChildRecord(ObjectNID);
    if (o != null) {
      this.BlkJSX.insertRecord(o);
      this.handleScrollbars();
    }
  }
  this.deleteRecord = function(ObjectNID) {
    this.BlkJSX.deleteRecord(ObjectNID);
    this.handleScrollbars();
    this.selRecord = null;
    this.onSelect();
  }
}

//---------------------------------------
CmpXPathNIDList.prototype = new CmpList();
function CmpXPathNIDList (BlkName, ObjectNIDList, ObjectXPath, RecordArr, EventArr, SelEvent, SpyGlassEventFnc) {
// setup inheritence and call base constructor
  CmpXPathNIDList.BaseClass = CmpXPathNIDList.prototype;
  CmpXPathNIDList.prototype.constructor = CmpXPathNIDList;

  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent, null, SpyGlassEventFnc);
    this.ObjectNIDList = ObjectNIDList;
    this.ObjectXPath = ObjectXPath;
  }

  // redefine repaint
  this.repaint = function() {
    //clear rowid array
    this.BlkJSX.RowIDArr = [];
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // get group tree node
    var objectNIDListNode = topic_GetNode(tApp, "//" + this.ObjectNIDList);

    if( objectNIDListNode != null ) {
      this.addChildRecords(objectNIDListNode);
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }
  
  // helper function to recurse thru the records
  this.addChildRecords = function(nidListNode) {
    var objectListIter = nidListNode.selectNodeIterator(".//ObjectNID");
    while( objectListIter.hasNext() ) {
      nidNode = objectListIter.next();
      var ObjectNID = nidNode.getValue();
      var o = this.getChildRecord(ObjectNID);
      if( o != null ) {
        this.BlkJSX.insertRecord(o, null, false);
      }
    }
  }

  // helper function to fill a record from obj array
  this.getChildRecord = function(ObjectNID) {
      var objectNode = topic_GetNIDNode(tCache, ObjectNID);
    if( objectNode == null ) return null;
    //get the node for the specified XPATH else continue
    if( !jsx3.util.strEmpty(this.ObjectXPath) ) {
      var objectXPathNode = objectNode.selectSingleNode(this.ObjectXPath);
      if( objectXPathNode == null ) return null;
    }
    var o = new Object();
    // use objectNID for jsxid
    o.jsxid = ObjectNID;
    // iter over columns for each record
    for( var colName in RecordArr ) {
      var colValXPath = null;
      if( RecordArr[colName].objxpath != null ) {
        colValXPath = eval(RecordArr[colName].objxpath);
      }
      else {
        colValXPath = eval(RecordArr[colName]);
      }
      var objValNode = objectNode.selectSingleNode(colValXPath);
      if( objValNode != null ) {
        var colVal = objValNode.getValue();
        if( RecordArr[colName].lookuparr != null ) {
          // gets the value from the array, which will be displayed in UI
          colVal = RecordArr[colName].lookuparr[colVal];
        }
        o[colName] = colVal;
      }
    }
    return o;
  }

  // these functions will add/update/delete just individual NID records
  this.addRecord = function(ObjectNID) {
    var o = this.getChildRecord(ObjectNID);
    if (o != null) {
      this.BlkJSX.insertRecord(o);
      this.handleScrollbars();
    }
  }
  this.updateRecord = function(ObjectNID) {
    var o = this.getChildRecord(ObjectNID);
    if (o != null) {
      this.BlkJSX.insertRecord(o);
      this.handleScrollbars();
    }
  }
  this.deleteRecord = function(ObjectNID) {
    this.BlkJSX.deleteRecord(ObjectNID);
    this.handleScrollbars();
    this.selRecord = null;
    this.onSelect();
  }
}

//---------------------------------------
CmpMultiNIDList.prototype = new CmpList();
function CmpMultiNIDList (BlkName, ObjectNIDGroupTree, GroupTagName, GroupTagValue, ObjectInfoArr, RecordArr, EventArr, SelEvent, SpyGlassEventFnc) {
// setup inheritence and call base constructor
  CmpMultiNIDList.BaseClass = CmpMultiNIDList.prototype;
  CmpMultiNIDList.prototype.constructor = CmpMultiNIDList;

  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent, null, SpyGlassEventFnc);
    this.ObjectNIDGroupTree = ObjectNIDGroupTree;
    this.GroupTagName = GroupTagName;
    this.GroupTagValue = GroupTagValue;
    this.ObjectInfoArr = ObjectInfoArr;
  }
  
  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    
    //clear refresh arrays
    this.clearRefresh();
    this.CellRefreshArr = new Object;
    this.ObjectRefreshArr = new Object;
    
    // get group tree node
    var objectNIDGroupTreeNode = topic_GetNode(tApp, "//" + this.ObjectNIDGroupTree);
    var GroupTagNameStr = "_ALL_";
    if( this.GroupTagName != null ) {
      GroupTagNameStr = topic_Get(tApp, this.GroupTagName);
    }
    //var objectGroupTagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree");
    var objectGroupTagNameNode = objectNIDGroupTreeNode;
    if( GroupTagNameStr != "_ALL_" ) {
      var tagNameNode = objectNIDGroupTreeNode.selectSingleNode(".//ObjectNIDTree/Group[@Name='" + GroupTagNameStr + "']");
      if( tagNameNode != null ) {
        objectGroupTagNameNode = tagNameNode;
      }
    }
    var GroupTagValueStr = "_ALL_";
    if( this.GroupTagValue != null ) {
      GroupTagValueStr = topic_Get(tApp, this.GroupTagValue);
    }
    var objectGroupTagValueNode  = objectGroupTagNameNode;
    if( GroupTagValueStr != "_ALL_" ) {
      var GroupTagValueArr = GroupTagValueStr.split("/");
      for( var i=0; i<GroupTagValueArr.length; i++ ) {
        GroupTagValueStr = GroupTagValueArr[i];
        objectGroupTagValueNode = objectGroupTagValueNode.selectSingleNode(".//Group[@Name='" + GroupTagValueStr + "']");
        if( objectGroupTagValueNode == null ) {
           break;
        }
      }    
    }
    if( objectGroupTagValueNode != null ) {
      this.addChildRecords(objectGroupTagValueNode);
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }
  
  // helper function to recurse thru the records
  this.addChildRecords = function(groupNode) {
    //var objectListIter = groupNode.selectNodeIterator("./*");
    var objectListIter = groupNode.selectNodeIterator(".//ObjectNID");
    while( objectListIter.hasNext() ) {
      groupNode = objectListIter.next();
      var NodeName = groupNode.getNodeName();
      if( NodeName != "ObjectNID" ) {
        this.addChildRecords(groupNode);
        continue;
      }
      var ObjectNID = groupNode.getValue();
      // init new row record - use objectNID for jsxid
      var o = new Object();
      o.jsxid = ObjectNID;
  
     // get object nodes with data for this row
      var ObjectNodeArr = [];
      var ObjAgentJID = null;
      for( var i=0; i<this.ObjectInfoArr.length; i++ ) {
        var objInfo = this.ObjectInfoArr[i];
        var objCacheXPath = objInfo.xpath;
        objCacheXPath = objCacheXPath.replace(/\$NID/, ObjectNID);
        // if regular service object - get object node from tCache
        if( objInfo.type != null ) {
          var objectNode = topic_GetNode(tCache, objCacheXPath);
          ObjectNodeArr.push({"Type":objInfo.type, "Node":objectNode});
        }
        // if agent object node - get object node from AgentJID specifc topic
        else if( objInfo.agentobjtype != null ) {
          ObjAgentJID = null;
          var ObjAgentCacheRoot = arrAgentObjectInfo[objInfo.agentobjtype];
          var objAgentJIDNode = topic_GetNode(tCache, objCacheXPath);
          ObjAgentJID = objAgentJIDNode.getValue();
          var objectNode = topic_GetNode(ObjAgentJID, "//"+ ObjAgentCacheRoot);
          ObjectNodeArr.push({"Type":objInfo.agentobjtype, "Node":objectNode, "RefreshInterval":objInfo.refreshInterval});
          // check /add objecttype + agentJID in refresh list
          // get refresh interval from the cache
          var RefreshInterval = topic_Get(tApp,objInfo.refreshInterval);          
          if( RefreshInterval != null ) {
            if( this.ObjectRefreshArr[objInfo.agentobjtype] == null ) {
              this.ObjectRefreshArr[objInfo.agentobjtype] = new Object;
            }
            this.ObjectRefreshArr[objInfo.agentobjtype][ObjAgentJID] = {"RefreshInterval":RefreshInterval,
                                                                        "XPath":null,
                                                                        "IntervalID":null};
          }
        }    
      }

      // iter over columns for each record
      for( var colName in RecordArr ) {
        var colValObjectType = RecordArr[colName].objtype;
        var colValObjectXPath = eval(RecordArr[colName].objxpath);
        var objValNode = null;
        for(var i=0;i<ObjectNodeArr.length; i++) {
          // find matching object type
          if( ObjectNodeArr[i].Type == colValObjectType) {
            // get the col data node from the resp obj node
            try {
              objValNode = ObjectNodeArr[i].Node.selectSingleNode(colValObjectXPath);
            } catch(e) {
            }
            // check if this data refreshes - if so add this cell to refresh list
            if( ObjectNodeArr[i].RefreshInterval != null ) {
              if( this.CellRefreshArr[ObjAgentJID] == null ) this.CellRefreshArr[ObjAgentJID] = new Object;
              var colObj = this.BlkJSX.getDescendantOfName(colName, false, false);
              this.CellRefreshArr[ObjAgentJID][colName] = {"RowID":o.jsxid,
                                                           "AgentObj":colValObjectType,
                                                           "ColObj":colObj,
                                                           "ColObjXPath":colValObjectXPath};
            }
            break;
          }
        }
        // set col data in matrix record - do lookup arr if needed
        if( objValNode != null ) {
          var colVal = objValNode.getValue();
          if( RecordArr[colName].lookuparr != null ) {
            // gets the value from the array, which will be displayed in UI
            colVal = RecordArr[colName].lookuparr[colVal];
          }
          o[colName] = colVal;
        } else {
          o[colName] = "-";
        }
        
      }
      this.BlkJSX.insertRecord(o, null, false);
    }
  }
}

//---------------------------------------
CmpNIDRefreshList.prototype = new CmpList();
function CmpNIDRefreshList (BlkName, ObjectInfoArr, RecordArr, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpNIDRefreshList.BaseClass = CmpNIDRefreshList.prototype;
  CmpNIDRefreshList.prototype.constructor = CmpNIDRefreshList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent);
    this.ObjectInfoArr = ObjectInfoArr;
  }
  
  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    this.BlkJSX.clearXmlData();
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    
    //clear refresh arrays
    this.clearRefresh();
    this.CellRefreshArr = new Object;
    this.ObjectRefreshArr = new Object;    

    var ObjectNodeArr = [];
    var ObjAgentJID = null;
    var objectNIDNode = null;
    var ObjAgentJID = null;
    var objectNode = null;
    var ElementXPath = null;
    for( var i=0; i<this.ObjectInfoArr.length; i++ ) {
      var objInfo = this.ObjectInfoArr[i];
      // if regular service object - get object node from tCache
      if( objInfo.type == "appobjtype" ) {
        ObjAgentJID = topic_Get(tApp, "AppServerAgentJID");
        // get refresh interval from the cache
        var RefreshInterval = topic_Get(tApp,objInfo.refreshInterval);          
        if( RefreshInterval != null ) {        
          if( this.ObjectRefreshArr[objInfo.objtype] == null ) {
            this.ObjectRefreshArr[objInfo.objtype] = new Object;
          }
          this.ObjectRefreshArr[objInfo.objtype][ObjAgentJID] = {"RefreshInterval":RefreshInterval,
                                                                 "XPath":objInfo.xpath,
                                                                 "IntervalID":null,
                                                                 "Status":"Ready"
                                                                };
        }
      }
    }

    // setup the request Most probable this the first time request
    if( this.ObjectRefreshArr != null ) {
      for( var AgentObj in this.ObjectRefreshArr ) {
        for( var AgentJID in this.ObjectRefreshArr[AgentObj] ) {
          if ( this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID == null ) {
            var Interval = this.ObjectRefreshArr[AgentObj][AgentJID].RefreshInterval;
            var status = this.ObjectRefreshArr[AgentObj][AgentJID].Status;
            Interval = parseInt(Interval)*1000;
            this.ObjectRefreshArr[AgentObj][AgentJID].IntervalID = setAgentReqInterval(Interval, AgentJID, AgentObj, status );
            //send request anyway dont wait for the 1 request to be send after the interval
            sendAgentObjRequest(AgentJID, AgentObj, status);
          }
          var AgentObjectReqSuccessEventVar = arrAgentObjectInfo[AgentObj].successevent.eventvar;
          var AgentObjectReqSuccessEventVal = arrAgentObjectInfo[AgentObj].successevent.eventval;
          
          var AgentObjectReqTopic = AgentJID;
          if( this.eventHandlerArr[AgentObjectReqTopic] == null ) {
            this.eventHandlerArr[AgentObjectReqTopic] = new EventListner( AgentObjectReqTopic );
            //subscription should be set once
            var CacheNodeIterXPath = this.ObjectRefreshArr[AgentObj][AgentJID].XPath;
            var refreshMatrixFnc = new refreshMatrix(this, AgentObj, AgentJID, CacheNodeIterXPath, this.RecordArr);
            this.eventHandlerArr[AgentObjectReqTopic].subscribe(AgentObjectReqSuccessEventVar, 
                                                                null,
                                                                refreshMatrixFnc, refreshMatrixFnc.refreshMtx);            
          }

        }
      }
    }      

    // repaint
    this.postpaint();
    this.onSelect();  
  }
       
  // redraw matrix 
  // function used in setInterval() in the show/hide of the blk
  refreshMatrix = function(ThisClass, AgentObj, AgentJID, CacheNodeIterXPath, RecordArr) {
    this.ThisClass = ThisClass;
    this.AgentObj = AgentObj;
    this.AgentJID = AgentJID;
    this.CacheNodeIterXPath = CacheNodeIterXPath;
    this.RecordArr = RecordArr;
    
    this.refreshMtx = function(EventVar, EventVal) {
      // helper function
      function insertDataInMatrix(BlkJSX, RecordArr, cacheNode, RecordPaintTime) {
        var elementNode = cacheNode;
        var o = new Object();
        for( var colName in RecordArr ) {
          var colValObjectType = RecordArr[colName].objtype;
          var colValObjectXPath = eval(RecordArr[colName].objxpath);
          var objValNode = null;
          try {
            objValNode = elementNode.selectSingleNode(colValObjectXPath);
          } catch(e) {
          }

          // set col data in matrix record - do lookup arr if needed
          if( objValNode != null ) {
            var colVal = objValNode.getValue();
            if( RecordArr[colName].lookuparr != null ) {
              // gets the value from the array, which will be displayed in UI
              colVal = RecordArr[colName].lookuparr[colVal];
            }
            o[colName] = colVal;
          } else {
            o[colName] = "-";
          }
        }
        o["RecordPaintTime"] = RecordPaintTime;
        BlkJSX.insertRecord(o, null, false);       
      } // end helper insertDataInMatrix
      
      //helper function
      function refreshDataInMatrix(BlkJSX, RecordArr, recordNode, cacheNode, RecordPaintTime) {
        var elementNode = cacheNode;
        var NID = recordNode.getAttribute("jsxid");
             
        var strXMLCacheId = BlkJSX.getXMLId();
        var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);      
        if(objXML2) {
          var objRecordNodes = objXML2.selectNodes("//record");
          for (var it = objRecordNodes.iterator();it.hasNext();){
            var RecordNode = it.next();
            var RecordNID = RecordNode.getAttribute("jsxid");
            if(NID == RecordNID) {
              for( var colName in RecordArr ) {
                var ColObj = BlkJSX.getDescendantOfName(colName, false, false);
                var colValObjectType = RecordArr[colName].objtype;
                var colValObjectXPath = eval(RecordArr[colName].objxpath);
                var objValNode = null;
                try {
                  objValNode = elementNode.selectSingleNode(colValObjectXPath);
                } catch(e) {
                }

                // set col data in matrix record - do lookup arr if needed
                if( objValNode != null ) {
                  var colVal = objValNode.getValue();
                  if( RecordArr[colName].lookuparr != null ) {
                    // gets the value from the array, which will be displayed in UI
                    colVal = RecordArr[colName].lookuparr[colVal];
                  }
                  RecordNode.setAttribute(colName, colVal);
                  BlkJSX.redrawCell(NID,ColObj ,"");
                }
              }
              RecordNode.setAttribute("RecordPaintTime", RecordPaintTime);             
            }
          }
        }        
      } // end helper refreshDataInMatrix
      
      //helper function
      function cleanUPMatrix(BlkJSX, RecordPaintTime) {
        // getting the cdf doc for the blkjsx (ie matrix)
        var strXMLCacheId = BlkJSX.getXMLId();
        var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);      
        if(objXML2) {
          var objRecordNodes = objXML2.selectNodes("//record");
          for (var it = objRecordNodes.iterator();it.hasNext();){
            var RecordNode = it.next();
            var CloneoRecordNode = RecordNode.cloneNode(true);
            var RecordPaintedTime = CloneoRecordNode.getAttribute("RecordPaintTime");
            if( RecordPaintedTime < RecordPaintTime) {
              var selRecordID = CloneoRecordNode.getAttribute("jsxid");
              BlkJSX.deleteRecord(selRecordID,true);
            }
          }      
        }      
      } // end helper cleanUPMatrix
      
      // start main code
      this.ThisClass.BlkJSX.setValue(null); 
      this.ThisClass.selRecordID = null;
      var MtxRecordArr = [];
      var CacheRecordArr = [];
      var RecordPaintTime = new Date().getTime();    
      // getting the cdf doc for the blkjsx (ie matrix)
      var strXMLCacheId = this.ThisClass.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId); 
      if(objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        for (var it = objRecordNodes.iterator();it.hasNext();){
          var RecordNode = it.next();
          var CloneoRecordNode = RecordNode.cloneNode(true);
          var strRecordNID = CloneoRecordNode.getAttribute("jsxid");
          MtxRecordArr.push({"NID":strRecordNID, "DataNode":CloneoRecordNode});
        }      
      }
      var MtxRecordArrLength = MtxRecordArr.length;
      
      var agentObjtopicCacheRoot = arrAgentObjectInfo[this.AgentObj].cacheRoot;
      var agentObjtopicNode = topic_GetNode(this.AgentJID, "//"+agentObjtopicCacheRoot);
      if( agentObjtopicNode != null ) {
        var elementListIter = agentObjtopicNode.selectNodeIterator(this.CacheNodeIterXPath);
        while( elementListIter.hasNext() ) {
          var elementNode = elementListIter.next();
          var CloneElementNode = elementNode.cloneNode(true);
          var elementNID = CloneElementNode.getAttribute("NID");
          CacheRecordArr.push({"NID":elementNID, "DataNode":CloneElementNode});
        }
      }      
      var CacheRecordArrLength = CacheRecordArr.length;
      
      // if cachedata is empty then reset the matrix data
      if( CacheRecordArrLength == 0 ) {
        this.ThisClass.BlkJSX.resetCacheData();
        this.ThisClass.postpaint(this.ThisClass);        
      } else {
        // if cache has data and matrix is empty then insert all the cache data
        var RepaintReq = false;
        if(MtxRecordArrLength == 0) {
          for(var CRAL=0;CRAL<CacheRecordArr.length;CRAL++) {
            var cacheNode = CacheRecordArr[CRAL].DataNode;
            insertDataInMatrix(this.ThisClass.BlkJSX, this.RecordArr, cacheNode, RecordPaintTime);
          }
          RepaintReq = true;
          //this.ThisClass.postpaint(this.ThisClass);        
        } else { 
          // if cache has data and matrix has data then update the cdf of the matrix
          for(var CRAL=0; CRAL < CacheRecordArr.length; CRAL++) {
            var CacheNID = CacheRecordArr[CRAL].NID;
            var cacheNode = CacheRecordArr[CRAL].DataNode.cloneNode(true);
            var flag = false;
            for(var MRAL=0; MRAL < MtxRecordArr.length; MRAL++) {
              var MtxRecordNID = MtxRecordArr[MRAL].NID;
              //if data present then update data
              if( CacheNID == MtxRecordNID) {
                flag = true;
                var recordNode = MtxRecordArr[MRAL].DataNode.cloneNode(true);
                refreshDataInMatrix(this.ThisClass.BlkJSX, this.RecordArr, recordNode, cacheNode, RecordPaintTime);
                break;
              }
            }
            //if cache data not in the matrix then insert it
            if(!flag){
              insertDataInMatrix(this.ThisClass.BlkJSX, this.RecordArr, cacheNode, RecordPaintTime);
              RepaintReq = true;  
            }
          }
        }
        if(MtxRecordArrLength != CacheRecordArrLength) {
          RepaintReq = true;
        }        
        if(RepaintReq) {
          cleanUPMatrix(this.ThisClass.BlkJSX, RecordPaintTime);
          this.ThisClass.postpaint(this.ThisClass);
        }              
      } // end if
      this.ThisClass.onSelect.call(this.ThisClass);  
    } // end this.refreshmatrix 
    
  } // end  refreshmatrix  
  
  
} // end class

//---------------------------------------
CmpNIDElementRefreshList.prototype = new CmpList();
function CmpNIDElementRefreshList (BlkName, ObjectNID, ElementXPath, RecordArr, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpNIDElementRefreshList.BaseClass = CmpNIDElementRefreshList.prototype;
  CmpNIDElementRefreshList.prototype.constructor = CmpNIDElementRefreshList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent);
    this.ObjectNID = ObjectNID;
    this.ElementXPath = ElementXPath;
  }
   
      
  // redefine repaint
  this.repaint = function() {
    var ObjAgentJID = topic_Get(tApp, "AppServerAgentJID");
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // get group tree node
    var objectNIDNode = topic_GetNode(tApp, "//" + this.ObjectNID);
    var objectNID = objectNIDNode.getValue();
    var objectNode = topic_GetNIDNode(ObjAgentJID, objectNID);
    if( objectNode != null ) {
      var elementListIter = objectNode.selectNodeIterator(this.ElementXPath);
      while( elementListIter.hasNext() ) {
        var elementNode = elementListIter.next();
        var o = new Object();
        // use objectNID for jsxid
        o.jsxid = jsx3.xml.CDF.getKey();
        // iter over columns for each record
        for( var colName in this.RecordArr ) {
          var colValXPath = null;
          if( this.RecordArr[colName].objxpath != null ) {
            colValXPath = eval(this.RecordArr[colName].objxpath);
          }
          else {
            colValXPath = eval(this.RecordArr[colName]);
          }
          var objValNode = elementNode.selectSingleNode(colValXPath);
          if( objValNode != null ) {
            var colVal = objValNode.getValue();
            if( this.RecordArr[colName].lookuparr != null ) {
              // gets the value from the array, which will be displayed in UI
              colVal = this.RecordArr[colName].lookuparr[colVal];
            }
            o[colName] = colVal;
          }
        }
        if( colVal != "") {
          this.BlkJSX.insertRecord(o, null, false); 
        }
      }
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }
}

//---------------------------------------
CmpNIDElementList.prototype = new CmpList();
function CmpNIDElementList (BlkName, ObjectNID, ElementXPath, RecordArr, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpNIDElementList.BaseClass = CmpNIDElementList.prototype;
  CmpNIDElementList.prototype.constructor = CmpNIDElementList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, RecordArr, EventArr, SelEvent);
    this.ObjectNID = ObjectNID;
    this.ElementXPath = ElementXPath;
  }
  
  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // get group tree node
    var objectNIDNode = topic_GetNode(tApp, "//" + this.ObjectNID);
    var objectNID = objectNIDNode.getValue();
    var objectNode = topic_GetNIDNode(tCache, objectNID);
    if( objectNode != null ) {
      var elementListIter = objectNode.selectNodeIterator(this.ElementXPath);
      while( elementListIter.hasNext() ) {
        var elementNode = elementListIter.next();
        var o = new Object();
        // use objectNID for jsxid
        o.jsxid = jsx3.xml.CDF.getKey();
        // iter over columns for each record
        for( var colName in this.RecordArr ) {
          var colValXPath = null;
          if( this.RecordArr[colName].objxpath != null ) {
            colValXPath = eval(this.RecordArr[colName].objxpath);
          }
          else {
            colValXPath = eval(this.RecordArr[colName]);
          }
          var objValNode = elementNode.selectSingleNode(colValXPath);
          if( objValNode != null ) {
            var colVal = objValNode.getValue();
            if( this.RecordArr[colName].lookuparr != null ) {
              // gets the value from the array, which will be displayed in UI
              colVal = this.RecordArr[colName].lookuparr[colVal];
            }
            o[colName] = colVal;
          }
        }
        if( colVal != "") {
          this.BlkJSX.insertRecord(o, null, false); 
        }
      }
    }
    // repaint
    this.postpaint();
    this.onSelect();  
  }
}

//---------------------------------------
CmpNIDCustomList.prototype = new CmpList();
function CmpNIDCustomList (BlkName, RepaintFunction, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpNIDCustomList.BaseClass = CmpNIDCustomList.prototype;
  CmpNIDCustomList.prototype.constructor = CmpNIDCustomList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent);
    this.RepaintFunction = RepaintFunction;
  }
  
  // redefine repaint
  this.repaint = function() {
    // clear curent data view
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.BlkJSX.setValue(null); 
    this.selRecordID = null;
    // calling custom repaint function defined in the cmp js file
    this.RepaintFunction.call(this);
    // repaint
    this.postpaint();
    this.onSelect();  
  }
}

//---------------------------------------
CmpController.prototype = new Component();
function CmpController(BlkName, InstanceID, EventArr)
{
// setup inheritence and call base constructor
  CmpController.BaseClass = CmpController.prototype;
  CmpController.prototype.constructor = CmpController;
  
  // stash value of this in local var for access from timer function
  var ThisClass = this;
  this.currSessionID = "NONE";
      
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr);
    this.InstanceID = InstanceID;
  }

  this.init = function() {
    CmpController.BaseClass.init.call(this);
    agtCollaborationCtrl.registerController(this.InstanceID);
  }
  this.destroy = function() {
    agtCollaborationCtrl.unregisterController(this.InstanceID);
    CmpController.BaseClass.destroy.call(this);
  }

  this.repaint = function(EventVar, EventVal) {
    if(EventVar == "SessionID") {
      // get mediaroom node from collab/ ctl state
      var SessionID = EventVal;
      if(SessionID != this.currSessionID) {
        if(this.currSessionID != "NONE") {
          // if changing media room, leave collaboration
          agtCollaborationCtrl.leaveSession(this.InstanceID);
        }
        // stash new mediaroom nid
        this.currSessionID = SessionID;
        // join with the dest agent
        if(SessionID != "NONE") {
          agtCollaborationCtrl.joinSession(this.InstanceID, SessionID);
        }
      }
    }
    else {
      var Action = EventVal;
      try {
        switch (Action) {
          case "START":
            agtCollaborationCtrl.startSession(this.InstanceID, this.nodeSession.cloneNode(true));
            break;
          case "STOP":
            agtCollaborationCtrl.stopSession(this.InstanceID);
            break;
          case "PAUSE":
            agtCollaborationCtrl.pause(this.InstanceID);
            break;
          case "RESUME":
            agtCollaborationCtrl.resume(this.InstanceID);
            break;
          case "RESUMEFROMMEDIATC":
            //agtCollaborationCtrl.pvrAction(this.InstanceID, "ResumeFromMediaTC"); ??
            break;
          case "RESUMEFROMBOOKMARK":
            agtCollaborationCtrl.resumeFromBookmark(this.InstanceID);
            break;
          case "CATCHUP":
            agtCollaborationCtrl.catchup(this.InstanceID);
            break;
          case "SKIPBACK":
            agtCollaborationCtrl.skipback(this.InstanceID);
            break;
          case "SKIPFWD":
            agtCollaborationCtrl.skipfwd(this.InstanceID);
            break;
          case "STARTRECORDING":
            agtCollaborationCtrl.startRecording(this.InstanceID);
            break;
          case "STOPRECORDING":
            alert("TBD: StopRecording not supported in this release");
            //agtCollaborationCtrl.stopRecording(this.InstanceID);
            break;
          case "PAUSERECORDING":
            agtCollaborationCtrl.pauseRecording(this.InstanceID);
            break;
          case "RESUMERECORDING":
            agtCollaborationCtrl.resumeRecording(this.InstanceID);
            break;
          case "NEWBOOKMARK":
            agtCollaborationCtrl.newBookmark(this.InstanceID);
            break;
          case "ADDBOOKMARK":
            agtCollaborationCtrl.addBookmark(this.InstanceID);
            break;
          case "EDITBOOKMARK":
            agtCollaborationCtrl.editBookmark(this.InstanceID);
            break;
          case "DELETEBOOKMARK":
            agtCollaborationCtrl.deleteBookmark(this.InstanceID);
            break;
          case "VIEW":
          case "PLAYBACK":
            agtCollaborationCtrl.viewOrPlayBack(this.InstanceID, objReq);
            break;
//          case "SETWINDOWCONTENT":
//            agtCollaborationCtrl.setWindowContent(this.InstanceID, objReq);
//            break;
          case "SWITCHSRC":
            agtCollaborationCtrl.switchSrc(this.InstanceID, objReq);
            break;
          default:
            break;
        }
      }
      catch(e) {
        //TBD -- ??
        log.error("Controller Agent Error: " + e.getMessage());
        alert(e.getMessage());
      }
    }
  }
}
//---------------------------------------
CmpPlayer.prototype = new Component();
function CmpPlayer(BlkName, InstanceID, EventArr, LoadEvent, PlayerTopic) {
  // setup inheritence and call base constructor
  CmpPlayer.BaseClass = CmpPlayer.prototype;
  CmpPlayer.prototype.constructor = CmpPlayer;

  // stash value of this in local var for access from timer function
  var ThisClass = this;

  if (BlkName != null) {
    Component.prototype.constructor.call(this, BlkName, EventArr, LoadEvent);
    this.InstanceID = InstanceID;
  }
  // extend init
  this.init = function() {
    CmpPlayer.BaseClass.init.call(this);
    // init App Vars
    this.EnableKbm = 0;
    this.FullScreenMode = false;
    this.NativeMode = false;
    // stash initial size of container
    this.gLeft = this.BlkJSX.getLeft();
    this.gTop = this.BlkJSX.getTop();
    this.gWidth = this.BlkJSX.getWidth();
    this.gHeight = this.BlkJSX.getHeight();
    // stash initial position of dividers
    if (this.HeaderDividerBlkJSX != null) this.gHeaderRows = this.HeaderDividerBlkJSX.getRows();
    if (this.CenterDividerBlkJSX != null) this.gCenterCols = this.CenterDividerBlkJSX.getCols();
    // init the player size object defaults
    this.playerSize = new Object;
    //get a default border setting:'8'
    this.playerSize.border = topic_Get(tApp, "IPVSPlayerDefaultBorder");
    this.playerSize.aspectRatio = topic_Get(tApp, "IPVSPlayerDefaultAspectRatio");
    this.Fps = topic_Get(tApp, "IPVSPlayerDefaultFps");
    this.currentFps = this.Fps; 

    // auto pause on hide
    this.AutoPauseOnHide = false;

    // init player load vars
    this.IPVideoPlayer = null;
    this.lastAction = null;

    //+ register an avcodec port instance (on the current Port) with the dest stream agent
    var DestNID = topic_Get(this.InstanceID, "DestNID");
    var PortID = dsaPortGet(DestNID);
    this.PortInstance = dsaPortInstanceAdd(PortID, this.InstanceID);
    this.ResourceInstance = null;
    this.playerTopic = PlayerTopic;
  }

  // extend destroy for CmpPlayer
  this.destroy = function() {
    if (this.IPVideoPlayer != null) {
      // stop the player
      this.IPVideoPlayer.stop();
      this.deleteResourceInstance();
      this.IPVideoPlayer.destroy();
      this.IPVideoPlayer = null;
    }
    // delete the port instance
    dsaPortInstanceDelete(this.InstanceID);
    // clear event handler arr
    this.eventHandlerArr = null;
    // do base destroy
    CmpPlayer.BaseClass.destroy.call(this);
  }

  // extend show - to handle player events
  this.show = function (EventVar, EventVal) {
    // do base show
    CmpPlayer.BaseClass.show.call(this, EventVar, EventVal);
    if (this.IPVideoPlayer == null) {
      this.IPVideoPlayer = com.ipvs.gui.MediaPlayer.getPlayerInstance(this.InstanceID, this.BlkJSX);
      this.IPVideoPlayer.load().when(function () {
        log.info(" clsComponent:CmpPlayer:show:loaded:ThisClass.IPVideoPlayer= " + ThisClass.IPVideoPlayer + ": ThisClass.SelEvent=" + ThisClass.SelEvent + ".");
        if (ThisClass.IPVideoPlayer != null) {
          // Check if the Aspect ratio is changed then keep new Aspect ratio
          var IPVSPlayerDefaultAspectRatio = topic_Get(tApp, "IPVSPlayerDefaultAspectRatio");
          if (ThisClass.playerSize.aspectRatio != IPVSPlayerDefaultAspectRatio) {
            ThisClass.playerSize.aspectRatio = IPVSPlayerDefaultAspectRatio;
          }

          //? Check if the Fps is changed then set it to the player
          var IPVSPlayerDefaultFps = topic_Get(tApp, "IPVSPlayerDefaultFps");
          if (ThisClass.Fps != IPVSPlayerDefaultFps) {
            ThisClass.Fps = IPVSPlayerDefaultFps;
            ThisClass.IPVideoPlayer.setFps(ThisClass.Fps);
          }
          if (ThisClass.SelEvent != null) {
            // setup a timer to check for load completion
            CmpPlayer.BaseClass.onSelect.call(ThisClass);
          }
          ThisClass.IPVideoPlayer.setTopic(ThisClass.playerTopic);
        }
        //publish once player load 
        if (topic_Get(tApp, "PlayerLoaded") == "NONE") {
          topic_Publish(tApp, "PlayerLoaded", "TRUE");
        }
        
      });
    }
  }

  // extend hide
  this.hide = function() {
    // pause the player if auto pause when hide mode
    if (this.AutoPauseOnHide == true) {
      topic_Publish(tPly, "PlayerEventRequest", "PAUSE");
    }
    // call base class
    CmpPlayer.BaseClass.hide.call(this);
  }

  //add a resource instance along with the assoc SS instances for this stream type
  this.addResourceInstance = function(StreamType) {
    if (StreamType == null) {
      return;
    }
    this.ResourceInstance = dsaResourceInstanceAdd(this.PortInstance, StreamType);
  }

  //get ip port info from Rx stream instance for this resource instance and return
  this.getResourceInstanceIpPort = function() {
    return dsaResourceInstanceGetRxMediaIpPort(this.ResourceInstance);
  }

  this.bindResourceInstance = function(StreamNID) {
    if (StreamNID == null) {
      return;
    }
    //has been removed from resource instance - so no need to bind there
    /*
    if( this.ResourceInstance != null ) {
    //add a dsa instance binding this streamnid to this port resource instance
    dsaResourceInstanceBind(this.ResourceInstance, StreamNID);
    }
    */
    // should probably save as local var and remove dependence on tInstance
    this.StreamNID = StreamNID;
  }

  this.deleteResourceInstance = function() {
    if (this.ResourceInstance != null) {
      // remove the dsa instance for this streamnid
      dsaResourceInstanceDelete(this.ResourceInstance);
      this.StreamNID = null;
    }
  }

  this.doAction = function(Action, ActionData) {
    switch (Action) {
      case "Start":
        // get URL and start player
        var StreamURL = ActionData;
        if (StreamURL == null || StreamURL == "") {
          StreamURL = dsaResourceInstanceGetPlayerURL(this.ResourceInstance);
        }
        else {
          // update our idea of FPS (based on what came in the URL)
          if (StreamURL.match(/fps=/)) {
            // save the final/changed fps value in the component for later use
            var streamFps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");

            // check prefs to see if there is a Fps setting (0 = use original value from stream)
            var Fps = topic_Get(tApp, "IPVSPlayerDefaultFps");
            // if not 0 AND less than streamURL's - substitute setting value in StreamURL
            if (Fps != 0 && Fps < streamFps) {
              StreamURL = StreamURL.replace(/fps=\d+/, "fps=" + Fps);
            }
            // save the final/changed fps value in the component for later use
            this.Fps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");
          }
          if (!this.ResourceInstance) {
            if (StreamURL.match(/^v2d/)) streamType = "V2D";
            if (StreamURL.match(/^udp/)) streamType = "MPEGTS";
            if (StreamURL.match(/^http/)) streamType = "RTP";
            // addResourceInstance(streamType);
            this.addResourceInstance(streamType);
          }
          dsaResourceInstanceSetPlayerURL(this.ResourceInstance, StreamURL);
        }
        if (StreamURL == null || StreamURL == "") return;
        // make sure player has loaded
        if (this.IPVideoPlayer == null) {
          this.lastAction = Action;
          break;
        }

        // start player
        this.IPVideoPlayer.start(StreamURL);

        //enable Remote button
        if (topic_Get(this.InstanceID, "EnableRemote") != null) {
          var EnableKbm = 0;
          if (StreamURL.match(/enablekbm=/)) {
            EnableKbm = StreamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");
            switch (EnableKbm) {
              case "false":
                EnableKbm = 0;
                break;
              case "true":
                EnableKbm = 1;
                break;
              default:
                EnableKbm = 0;
                break;
            }
          }
          topic_Publish(this.InstanceID, "EnableRemote", EnableKbm);
        }
        break;
      case "Stop":
        // make sure player has loaded
        if (this.IPVideoPlayer == null) {
          this.lastAction = Action;
          break;
        }

        // stop player
        this.IPVideoPlayer.stop();
        break;
      case "Pause":
        // make sure player has loaded
        if (this.IPVideoPlayer == null) {
          this.lastAction = Action;
          break;
        }

        this.IPVideoPlayer.pause();
        break;
      case "Resume":
      case "ResumeFrom":
      case "ResumeFromMediaTC":
        // make sure player has loaded
        if (this.IPVideoPlayer == null) {
          this.lastAction = Action;
          break;
        }
        this.IPVideoPlayer.resume();
        break;
      case "Catchup":
        // make sure player has loaded
        if (this.IPVideoPlayer == null) {
          this.lastAction = Action;
          break;
        }

        this.IPVideoPlayer.catchUp();
        break;
      case "SkipBackward":
        //passing 0 and let stream server decide 'seconds'
        this.IPVideoPlayer.skipBackward();
        break;
      case "SkipForward":
        this.IPVideoPlayer.skipForward();
        break;
      case "NativesizeOn":
        // set player to show native size video
        this.NativeMode = true;
        this.IPVideoPlayer.setNativeSize();
        break;
      case "NativesizeOff":
        // set player to show native size video
        this.NativeMode = false;
        this.IPVideoPlayer.setScaleToWindow();
        break;
      case "MuteOn":
        this.IPVideoPlayer.mute(true);
        break;
      case "MuteOff":
        this.IPVideoPlayer.mute(false);
        break;
      case "RemoteOn":
        this.IPVideoPlayer.setLocalKBMSState(1);
        this.IPVideoPlayer.setFocus();
        break;
      case "RemoteOff":
        this.IPVideoPlayer.setLocalKBMSState(0);
        break;
      case "FullScreenOn":
        this.IPVideoPlayer.IPVideoPlayer.style.border = 0;
        // call player to toggle the browser state
        this.IPVideoPlayer.toggleDisplay();
        // set top level dims to hide everything else
        this.FullScreenMode = true;
        break;
      case "FullScreenOff":
        this.IPVideoPlayer.IPVideoPlayer.style.border = topic_Get(tApp, "IPVSPlayerDefaultBorder");
        // call player to toggle the browser state
        this.IPVideoPlayer.toggleDisplay();
        this.FullScreenMode = false;
        break;
      default:
        break;
    }
  }

  this.setTrickMode = function (bolTrickMode) {
    if (this.IPVideoPlayer != null) {
      this.IPVideoPlayer.setTrickMode(bolTrickMode);
    }
  }

  //this make each player can have its own FPS, if pass null restore to default value
  this.setFPS = function(fps) {
    if (fps != null) {
      if (fps != this.currentFps) {
        this.IPVideoPlayer.setFps(fps);
        this.currentFps = fps;
      }
    }
    else {
      this.IPVideoPlayer.setFps(this.Fps);
      this.currentFps = this.Fps;
    }
  }
}
//---------------------------------------  
CmpSlider.prototype = new Component();
function CmpSlider (BlkName, InstanceID, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpSlider.BaseClass = CmpSlider.prototype;
  CmpSlider.prototype.constructor = CmpSlider;
  // stash value of this in local var for access from timer function
  var ThisClass = this;
  
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    // link to the player instance this slider is tied to
    this.InstanceID = InstanceID;
    // time and offset of start of slider bar
    // startmediatime is used as a flag to control repaint execution
    this.StartMediaTime = null;
    this.SliderStartOffset = null;
    // last known time and offset of media
    this.LastMediaTime = null;
    this.LastMediaOffset = null;
    // mode and scale of time since last known media offset
    this.TimeMode = null;
    this.TimeScale = null;
    this.TimeLength = null;
    // current media offset
    this.CurrMediaOffset = null;
    this.LiveStartMediaTime = null;
    // PVR media offsets
    this.PVRStartMediaOffset = null;
    this.LiveCurrMediaOffset = null;
    // custom offset for starting file media at non-zero offset
    this.StreamStartOffset = null;
    // pointer to interval timer for continuous update
    this.timerHandle = null;
    // pointers to the gui componenets that need update
    this.blkProgress = null;
    this.lblTimeCode = null;
    this.lblTimeLength = null;
  }
  
  // extend init
  this.init = function () {
    if (!jsx3.util.strEmpty(jsx3.gui.Slider.BRIDGE_EVENTS) ) {
      jsx3.gui.Slider.BRIDGE_EVENTS[jsx3.gui.Event.MOUSEWHEEL] = false;
    }
    else {
      jsx3.gui.Slider.yg[jsx3.gui.Event.MOUSEWHEEL] = false;
    }
      
    CmpSlider.BaseClass.init.call(this);
    // setup the select event handler
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.CHANGE);
    this.BlkJSX.setEvent("this.onSelect(fpVALUE,fpPREVIOUS);", jsx3.gui.Interactive.CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
    // setup the select event handler
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.INCR_CHANGE);
    this.BlkJSX.setEvent("this.onDrag(fpVALUE);", jsx3.gui.Interactive.INCR_CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onDrag = this.onDrag;
    
    // removing the mouse wheel event
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXMOUSEWHEEL);
    
    // init the offset and position
    this.initMediaOffset();
    this.updateView();
  }
  this.destroy = function() {
    this.repaint("PlayerCtrlState", "Stop");
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.INCR_CHANGE, this);
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.CHANGE, this);
    CmpSlider.BaseClass.destroy.call(this);
  }
  // extend show
  this.show = function(EventVar, EventVal) {
    CmpSlider.BaseClass.show.call(this, EventVar, EventVal);
    if( this.TimeMode != "NONE" && this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.BlkJSX.setHandleImage("images/slider/handleEnabled.gif");
    this.postpaint();
  }
  // override hide
  this.hide = function() {
    if( this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.BlkJSX.setHandleImage("images/slider/handleDisabled.gif");
    this.postpaint();
  }
  // override repaintCheck
  this.repaintCheck = function() {
    return true;
  }
  //check mediaOffset before set it, to ensure this.StartMediaTime not null for join mode
  this.checkAndSetMediaOffset = function() {
    //bug fix 20696
    if(this.timerHandle ==null ){
      this.startMediaOffset();
    }
    this.setMediaOffset();
    this.TimeScale = topic_Get(this.InstanceID, "TimeScale");
  }
  // override repaint
  this.repaint = function(EventVar, EventVal) {
    if( EventVar == "PlayerCtrlState" || EventVar == "ControllerState" ) {
      // process action
      var Action = EventVal;   
      switch(Action) {
        case "Start":
          this.startMediaOffset();
          break;
        case "Stop":
          this.initMediaOffset();
          break;
        case "Pause":
          this.checkAndSetMediaOffset();
          break;
        case "Resume":
          this.checkAndSetMediaOffset();
          break;
        case "ResumeFrom":
          this.checkAndSetMediaOffset();
          break;          
        case "ResumeFromMediaTC":
          this.checkAndSetMediaOffset();
          break;
        case "Catchup":
          this.checkAndSetMediaOffset();
          break;
        case "SkipBackward":
          // TBD - do we need to check the bounds of offset?
          this.checkAndSetMediaOffset();
          break;
        case "SkipForward":
          this.checkAndSetMediaOffset();
          // TBD - do we need to check the bounds of offset?
          break;
        case "TBD-FASTFWD":
          this.checkAndSetMediaOffset();
          break;
        case "TBD-REWIND":
          this.checkAndSetMediaOffset();
          break;
        default:
          this.updateMediaOffset();
          break;
      }
      this.updateView(); 
    }
  }
  this.initMediaOffset = function() {
    // default time mode = live
    this.TimeMode = "NONE";
    // default time length = 1 hour
    this.TimeLength = DefaultTimeLength;//3600000;
    this.TrimLength = DefaultTimeLength;
    // default timescale is 1:1 play
    this.TimeScale = 1;
    // default stream start offset
    this.StreamStartOffset = 0;

    this.StartMediaTime = null;
    this.LiveStartMediaTime = null;
    this.PVRStartMediaTime = null;
    this.LastMediaTime = null;
    this.LastMediaOffset = 0;

    this.SliderStartOffset = 0;
    this.CurrMediaOffset = 0;
  }

  this.startMediaOffset = function() {
    // default time mode = live
    this.TimeMode = topic_Get(this.InstanceID, "TimeMode");
    if( this.TimeMode == null || this.TimeMode == "" ) {
      this.TimeMode = "NONE";
    }
    // default time length = 1 hour
    this.TimeLength = topic_Get(this.InstanceID, "PlayLength");
    if (jsx3.util.strEmpty(this.TimeLength)){
      this.TimeLength = DefaultTimeLength;
    }
    this.TrimLength = topic_Get(this.InstanceID, "TrimLength");
    if (jsx3.util.strEmpty(this.TrimLength)){
      this.TrimLength = DefaultTimeLength;
    }

    var TimeLengthStr = sec2HMS(this.TimeLength);

    if( this.lblTimeLength != null ) {
      this.lblTimeLength.BlkJSX.setText(TimeLengthStr);
      this.lblTimeLength.BlkJSX.repaint();
    }
    // default timescale is 1:1 play
    this.TimeScale = topic_Get(this.InstanceID, "TimeScale");
    if( this.TimeScale == null || this.TimeScale == "" ) {
      this.TimeScale = 1;
    }
    this.StreamStartOffset = topic_Get(this.InstanceID, "StreamStartOffset");
    if( this.StreamStartOffset == null || this.StreamStartOffset == "" ) {
      this.StreamStartOffset = 0;
    }
    var CurrDate = newServerDate();
    // assume we are starting now
    //and media offset 0 (unless mode is LIVE)
    // in LIVE mode Media Offset = MediaTime
    this.StartMediaTime = topic_Get(this.InstanceID, "startWallClock");
    if( this.StartMediaTime == null || this.StartMediaTime == "") {
      this.StartMediaTime = CurrDate.getTime();
    }
    else if(this.StartMediaTime > CurrDate.getTime()){
      this.StartMediaTime = CurrDate.getTime();
    }
    // get the last media time and offset
    this.LastMediaTime = topic_Get(this.InstanceID, "lastActionWallClock");
    if( this.LastMediaTime == null || this.LastMediaTime == "") {
      this.LastMediaTime = CurrDate.getTime();
    }
    else if(this.LastMediaTime > CurrDate.getTime()){
      this.LastMediaTime = CurrDate.getTime();
    }    
    this.LastMediaOffset = topic_Get(this.InstanceID, "ActionMediaOffset");
    if( this.LastMediaOffset == null || this.LastMediaOffset == "") {
      this.LastMediaOffset = this.LastMediaTime - this.StartMediaTime;
    }
//    if( this.TimeMode == "LIVE" ) {
//      this.LastMediaOffset = this.LastMediaTime;
//    }
    // done with start - rest is handled by update
    this.updateMediaOffset();
  }

  this.setMediaOffset = function() {
    // get curr media offset from the offset that came with the action event
    var CurrDate = newServerDate();
    var ActionMediaOffset = parseInt(topic_Get(this.InstanceID, "ActionMediaOffset"));
    if( ActionMediaOffset != null && (! isNaN(ActionMediaOffset)) ) {
      this.CurrMediaOffset = ActionMediaOffset;
      if( this.TimeMode == "PVR" ) {
        if( this.CurrMediaOffset < (this.PVRStartMediaTime - this.StartMediaTime) ) {
          this.CurrMediaOffset = this.PVRStartMediaTime - this.StartMediaTime;
        }
      }
    }
    else {
      this.updateMediaOffset();
    }
    // record curr media offset/time as last media offset/time
    // set timescale
    this.TimeScale = topic_Get(this.InstanceID, "TimeScale");
    this.CurrMediaOffset = this.CurrMediaOffset + ((CurrDate.getTime() - parseInt(topic_Get(this.InstanceID, "lastActionWallClock"))) * this.TimeScale);
    this.LastMediaOffset = this.CurrMediaOffset;
    this.LastMediaTime = CurrDate.getTime();
  }

  this.updateMediaOffset = function() {
    if(this.StartMediaTime != null) {
      var CurrDate = newServerDate();
      // calc media offset where live is
      this.LiveCurrMediaOffset = CurrDate.getTime() - this.StartMediaTime;
      // calc curr media offset (in milliseconds) based on milliseconds since media time and timescale (ie play mode)  
      this.CurrMediaOffset = Math.round(parseInt(this.LastMediaOffset) + (parseInt(CurrDate.getTime()) - parseInt(this.LastMediaTime)) * this.TimeScale);
      // by default slider starts at offset 0
      this.SliderStartOffset = 0;
      // if time mode is still NONE
      if(this.TimeMode == "NONE") {
        // calc the slider start offset
        this.SliderStartOffset = this.LiveCurrMediaOffset;
      }
      // if in LIVE mode 
      if(this.TimeMode == "LIVE") {
        // if time mode just changed to LIVE - init LiveStartTime
        if(this.LiveStartMediaTime == null) {
          //this.LiveStartMediaTime = CurrDate.getTime();
          this.LiveStartMediaTime = this.StartMediaTime;
          this.LastMediaTime = CurrDate.getTime();
          this.LastMediaOffset = this.CurrMediaOffset;
        }
        // calc slider start offset
        this.SliderStartOffset = this.LiveCurrMediaOffset -this.TimeLength;
      }
      // if in PVR mode
      if(this.TimeMode == "PVR") {
        // if time mode just changed to PVR - init PVRStartTime
        if(this.PVRStartMediaTime == null) {
          //this.PVRStartMediaTime = CurrDate.getTime();
          this.PVRStartMediaTime = this.StartMediaTime;
          this.LastMediaTime = CurrDate.getTime();
          this.LastMediaOffset = this.CurrMediaOffset;
        }
        // calc the start of the PVR bar fill
        this.PVRStartMediaOffset = this.LiveCurrMediaOffset - this.TrimLength;
        // adjust if trimming has kicked in
        if(this.PVRStartMediaOffset < (this.PVRStartMediaTime - this.StartMediaTime)) {
          this.PVRStartMediaOffset = this.PVRStartMediaTime - this.StartMediaTime;
        }
        // calc the slider start offset
        this.SliderStartOffset = this.LiveCurrMediaOffset - this.TimeLength;
        if( this.SliderStartOffset < 0){
          this.SliderStartOffset = 0;
        }
        // adjust if paused for longer than trim
        if(this.SliderStartOffset > this.CurrMediaOffset) {
          this.SliderStartOffset = this.CurrMediaOffset;
        }
      }
    }
  }
  
  this.updateView = function() {
    // update view of all time related things
    this.updateSliderHandle();
    this.updateProgressBar();
    this.updateMediaOffsetString(this.CurrMediaOffset);
    // make sure increment timer is stopped/started as needed
    if( this.StartMediaTime == null ) {
      this.incrementStop();
    }
    else {
      this.incrementStart();
    }  
  }
  this.updateSliderHandle = function() {
    var Position = 0;
    //We are getting unexpected exception "Microsoft Jscript runtime error:object required".
    //Not able to find exact reason for this exception
    //To work around this issue using try/catch.
    try {
      if( this.StartMediaTime != null ) {
        var Position = Math.round( (parseInt(this.CurrMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100) );
        // check for bounds and set the position
        if (isNaN(Position)) Position = 100;
        if( Position > 100 ) Position = 100;
        if( Position < 0 ) Position = 0;
      }
      // Slider mark position --
      this.BlkJSX.setValue(Position);
    }catch(e){
      log.warn("CmpSlider: updateSliderHandle: " + e.description);
    }
  }
  this.updateProgressBar = function() {
    var ProgressStart = 0;
    var ProgressSize = 0;
    var MaxProgressSize = 100;
    var UserTrimLength = this.TrimLength;
    
    if(this.StartMediaTime != null) {
      if(this.TimeMode == "PVR" && this.PVRStartMediaOffset != null) {
        MaxProgressSize = Math.round( parseInt(MaxProgressSize)* parseInt(UserTrimLength) / this.TimeLength);
        ProgressStart = Math.round((parseInt(this.PVRStartMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100));
        if (isNaN(ProgressStart)) ProgressStart = 100;
        if(ProgressStart > 100) ProgressStart = 100;
        ProgressSize = Math.round((parseInt(this.LiveCurrMediaOffset) - parseInt(this.PVRStartMediaOffset)) / (this.TimeLength / 100));
        if (isNaN(ProgressSize) || ProgressSize > MaxProgressSize) ProgressSize = MaxProgressSize;
        
        if(ProgressStart + parseInt(ProgressSize) > 100) ProgressSize = 100 - ProgressStart;
      }
      else if(this.TimeMode == "PLAYBACK") {
        ProgressStart = 0;
        ProgressSize = 100;
      }
    }
    // Slider progress block size -- 
    this.blkProgress.BlkJSX.setLeft(ProgressStart + "%");
    this.blkProgress.BlkJSX.setWidth(ProgressSize + "%", true);
  }
  this.updateMediaOffsetString = function(Offset) {
    var MediaOffsetStr = "";

    if(this.StartMediaTime != null) {
      // Slider Tooltip & Time Label -- set curr offset time as tool tip
      if(this.TimeMode == "NONE") {
        Offset = Offset - this.SliderStartOffset;
      }
      if(this.TimeMode == "LIVE") {
        Offset = Offset - (this.LiveStartMediaTime - parseInt(this.StartMediaTime));
      }
      if(this.TimeMode == "PVR") {
        Offset = Offset - (this.PVRStartMediaTime - parseInt(this.StartMediaTime));
      }
      if (this.TimeMode == "PLAYBACK" && Offset > this.TimeLength) {
        Offset = this.TimeLength;
      }
      MediaOffsetStr = sec2HMS(Offset);
    }
    // slider tool tip
    this.BlkJSX.setTip("");
    // time label
    if(this.lblTimeCode != null) {
      this.lblTimeCode.BlkJSX.setText(MediaOffsetStr);
      this.lblTimeCode.BlkJSX.repaint();
    }
  }
  
  // extend onselect - this will handle user moving the slider position
  this.onSelect = function(fpVALUE, fpPREVIOUS) {
    // in LIVE mode return false to reset the hanldle position
    if( ThisClass.TimeMode == "LIVE" ) {
      return false; 
    }
    else {
      // calc the target media offset based on the current position
      var TgtOffset = parseInt(ThisClass.SliderStartOffset) + Math.round( fpVALUE * (ThisClass.TimeLength / 100) );
      ThisClass.goToOffset.call(ThisClass, TgtOffset);
      // return true to allow the handle to be left in the new position
      return true;
    }
  }
  this.onDrag = function(fpVALUE) {
    // in LIVE mode - do nothing
    if( ThisClass.TimeMode == "LIVE" ) {
      return;
    }
    else {
      // turn off position update
      ThisClass.incrementStop.call(ThisClass);
      // calc the target media offset based on the current position
      var TgtOffset = parseInt(ThisClass.SliderStartOffset) + Math.round( fpVALUE * (ThisClass.TimeLength / 100) );
      // update the display string to show what the media offset would be
      ThisClass.updateMediaOffsetString.call(ThisClass, TgtOffset);
    }
  }
  
  this.goToOffset = function(TgtOffset) {
    // re calc current offset - for accuracy
    this.incrementStop();
    this.updateMediaOffset();
    // calc the offset from current position
    //HACK: if the Tgt Offset is 0 - the user is trying to get to the start
    // - probably the request will fail - because we will be off by a few milliseconds
    // - the right fix is for the middleware to truncate negative offsets to zero
    // - for other offsets reduce the TgtOffset by 2 seconds to improve usability for now
    if( TgtOffset > 2000 ) {
      TgtOffset = parseInt(TgtOffset) - 2000;
    }
    else {
      TgtOffset = 0;
    }
    var Offset = TgtOffset - this.CurrMediaOffset;
    // fire event which will result in skip fwd/back by this offset
    CmpSlider.BaseClass.onSelect.call(this, Offset);
  }
  
  // setup interval timer to call increment function every length/100 times
  this.incrementStart = function() {
    if( this.timerHandle == null ) {
      // slider has 100 discrete positions
      //this.timerHandle = setInterval( this.increment, (this.TimeLength / 100) );
      this.timerHandle = setInterval( this.increment, 1000 );
    }
  }
  // stop interval timer
  this.incrementStop = function() {
    //HACK:There is an issue if we use multiple timers.
    //To handle that removing following check
    //if( this.timerHandle != null ) {
      clearInterval( this.timerHandle );
      this.timerHandle = null;
    //}
  }
  // this function is called in a global context (due to setInterval)
  // the value of "this" in here is not our "this" - so we use the special var ThisClass instead
  this.increment = function() {
    //ThisClass.repaint.call(ThisClass);
    ThisClass.updateMediaOffset.call(ThisClass);
    ThisClass.updateView.call(ThisClass); 
  }
}

//---------------------------------------  
CmpBookmarks.prototype = new Component();
function CmpBookmarks (BlkName, InstanceID, MediaNIDEvalStr, EventArr, SelEvent) {
// setup inheritence and call base constructor
  CmpBookmarks.BaseClass = CmpBookmarks.prototype;
  CmpBookmarks.prototype.constructor = CmpBookmarks;
  // stash value of this in local var for access from timer function
  var ThisClass = this;
  
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    this.MediaNIDEvalStr = MediaNIDEvalStr;
    this.MediaStartTC = null;
    this.MediaEndTC = null;
    this.CurrentTimeTC = null;
    this.MediaPlayLength = null;
    this.MediaOffset = null;
    this.LastBookmarkSelectedBlkJSX = null;
    this.BookmarkSelectedTitle = null;
    this.InstanceID = InstanceID;
    this.PrevMediaNID = null;
  }
  // extend init
  this.init = function() {
    CmpBookmarks.BaseClass.init.call(this);
    // get handle to the sample mark and setup onselect handler
    this.MarkBlkJSX = this.BlkJSX.getDescendantOfName("blkMark", false, false);
    // we want it to call our onSelect function
    // but the fnc call will exec in the context of the GI block
    // so later (after cloning) we will need to set the "onSelect" property in its context to point to our "onSelect" fnc
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.MarkBlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.JSXCLICK);
    // get handle to list area and clear it - just in case
    this.MarkListBlkJSX = this.BlkJSX.getDescendantOfName("blkMarkList", false, false);
    this.MarkListBlkJSX.removeChildren();
  }
  // extend show
  this.show = function(EventVar, EventVal) {
    if( this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    CmpBookmarks.BaseClass.show.call(this);
  }
  // override hide
  this.hide = function() {
    if( this.BlkJSX.setEnabled != null ) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.postpaint();
  }
  // override repaint
  this.repaint = function(EventVar, EventVal) {
    // clear any existing bookmarks
    this.MarkListBlkJSX.removeChildren();
    // get media NID
    var MediaNID = eval(this.MediaNIDEvalStr);

    // get the startTC and stop TC from the media node
    if( (MediaNID == null) || (MediaNID == "") || (MediaNID == "NONE")) {
      if( this.PrevMediaNID != null ) {
        this.LastBookmarkSelectedBlkJSX = null;
        this.clearBookmarks(this.PrevMediaNID);
        this.PrevMediaNID = null;
      }
      return;
    }

    if( this.PrevMediaNID != MediaNID ) {
      if( this.PrevMediaNID != null ) {
        this.clearBookmarks(this.PrevMediaNID);
      }
      this.PrevMediaNID = MediaNID;
      this.getBookmarks(MediaNID);
    }
    else {
      var MediaPlayLength = topic_Get(this.InstanceID, "PlayLength");
      var SrcMediaFileTrackNode = topic_GetNode(this.InstanceID, "//SrcMediaFileTrack");
      if( SrcMediaFileTrackNode == null ) return;
      var mediaBookmarkListNode = topic_GetNode(tCache, "//BookmarkList");
      if( mediaBookmarkListNode == null ) return;
      // for each bookmark
      var nodeBookmarkIter = mediaBookmarkListNode.selectNodeIterator("./*[.//MediaStreamFileResourceNID='" + MediaNID + "']");
      while( nodeBookmarkIter.hasNext() ) {
        var nodeBookmark = nodeBookmarkIter.next();
        // get bookmark details
        var BookmarkNID = nodeBookmark.getAttribute("NID");
        var BookmarkAtTC = nodeBookmark.selectSingleNode(".//AtTC").getValue();
        var BookmarkComment = nodeBookmark.selectSingleNode(".//Comment").getValue();
          // find mediasegemnt in which it falls
          var BookmarkOffset = null;
          // get first segment values to handle bookmarks created with a TC that is before the start of even the first media segement
          var firstSegmentStartTC = null;
          var firstSegmentEndTC = null;
          var firstSegmentMediaOffset = null;
          var nodeMediaSegmentIter = SrcMediaFileTrackNode.selectNodeIterator("//SrcMediaFileTrack//MediaTrackItem//MediaSegmentInfo");
          while( nodeMediaSegmentIter.hasNext() ) {
            var nodeMediaSegment = nodeMediaSegmentIter.next();
            var MediaOffset = nodeMediaSegment.selectSingleNode(".//MediaOffset").getValue();
            var MediaStartTC = nodeMediaSegment.selectSingleNode(".//StartTC").getValue();
            var MediaEndTC = nodeMediaSegment.selectSingleNode(".//EndTC").getValue();
            if( firstSegmentStartTC == null || MediaStartTC < firstSegmentStartTC ) {
              firstSegmentStartTC = MediaStartTC;
              firstSegmentEndTC = MediaEndTC;
              firstSegmentMediaOffset = MediaOffset;
            }
            if( BookmarkAtTC >= MediaStartTC &&  BookmarkAtTC <= MediaEndTC ) {
              BookmarkOffset = parseInt(MediaOffset) + parseInt(BookmarkAtTC - MediaStartTC);
              break;
            }
          }

          //if bookmark is in gaps then display at the end of previous segment.
          if( BookmarkOffset == null ) {
            var prevSegmentStartTC = null;
            var prevSegmentEndTC = null;
            var prevSegmentMediaOffset = null;
            var nodeMediaSegmentIter = SrcMediaFileTrackNode.selectNodeIterator("//SrcMediaFileTrack//MediaTrackItem//MediaSegmentInfo");
            while( nodeMediaSegmentIter.hasNext() ) {
              var nodeMediaSegment = nodeMediaSegmentIter.next();
              var segmentMediaOffset = nodeMediaSegment.selectSingleNode(".//MediaOffset").getValue();
              var segmentMediaStartTC = nodeMediaSegment.selectSingleNode(".//StartTC").getValue();
              var segmentMediaEndTC = nodeMediaSegment.selectSingleNode(".//EndTC").getValue();
              if( prevSegmentEndTC != null & BookmarkAtTC > prevSegmentEndTC &&  BookmarkAtTC < segmentMediaStartTC ) {
                BookmarkOffset = parseInt(prevSegmentMediaOffset) + parseInt(prevSegmentEndTC - prevSegmentStartTC);
                break;
              }
              else {
                //check if the bookmark is after segment then update prevSegment values;
                if( BookmarkAtTC > segmentMediaEndTC ) {
                  if( prevSegmentEndTC == null || segmentMediaEndTC > prevSegmentEndTC ) {
                    prevSegmentStartTC = segmentMediaStartTC;
                    prevSegmentEndTC = segmentMediaEndTC;
                    prevSegmentMediaOffset = segmentMediaOffset;
                  }
                }
              }
            }
          }

          //handle bookmarks created with a TC that is before the start of even the first media segement
          if( BookmarkOffset == null ) {
            if( BookmarkAtTC < firstSegmentStartTC ) {
              BookmarkOffset = firstSegmentMediaOffset;
            }
          }

        if( BookmarkOffset != null ){
          var BookmarkPosition = Math.round( BookmarkOffset / (MediaPlayLength /100) );
          // check value for bounds
          if( BookmarkPosition > 100 ) BookmarkPosition = 100;
          if( BookmarkPosition < 0 ) BookmarkPosition = 0;
          // clone a new bookmark and make it a child of the bookmark list block
          var newBookMarkBlkJSX = this.MarkBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
          this.MarkListBlkJSX.adoptChild(newBookMarkBlkJSX, false);
          // set a "onSelect" property = the curr instance onSelect function to call on click
          newBookMarkBlkJSX.onSelect = this.onSelect;
          // set a MarkID property = the bookmark ID so we can get it back when selected
          newBookMarkBlkJSX.MarkID = BookmarkNID;
          // set its position
          newBookMarkBlkJSX.setDimensions(BookmarkPosition+'%', 0, 8, '100%', true);
          // set its visibility
          newBookMarkBlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYVISIBLE, true);
          // set the comment as tooltip
          newBookMarkBlkJSX.setTip(BookmarkComment);
          // also save the offset value for later retrieval
          newBookMarkBlkJSX.MarkOffset = BookmarkOffset;
          if( this.LastBookmarkSelectedBlkJSX != null ) {
            if( newBookMarkBlkJSX.MarkID == this.LastBookmarkSelectedBlkJSX.MarkID){
              newBookMarkBlkJSX.setBackgroundColor("#336666", true);
              this.LastBookmarkSelectedBlkJSX = newBookMarkBlkJSX;
            }
          }
        }
      }  
      this.MarkListBlkJSX.repaint();
    }
  }

  this.getBookmarks = function() {
    var NIDforGapInfo = topic_Get(this.InstanceID, "NIDForGapInfo");
    if( NIDforGapInfo != null && NIDforGapInfo != "") {
      cmpReq_GetSrcMediaFileTrack(this.InstanceID, NIDforGapInfo);
    }
  }

  this.clearBookmarks = function(MediaNID) {
    trimBookmarkCacheNode(MediaNID);
  }
  
  // extend onselect - this will handle selection of bookmark for delete
  // this function executes in the context of the GI component (not "this")
  this.onSelect = function() {
    // mark this bookmark as the currently selected one
    ThisClass.markSelected.call(ThisClass, this);
    // get the Offset and move slider to it
   // if( this.MarkOffset != null ) {
    //  ThisClass.blkSlider.goToOffset.call(ThisClass.blkSlider, this.MarkOffset);
    //}
  }
  
  // change the highlighting to reflect selected bookmark and fire select event
  this.markSelected = function(BookmarkSelectedBlkJSX) {
    // un highlight the last selection
    if( this.LastBookmarkSelectedBlkJSX != null ) {
      this.LastBookmarkSelectedBlkJSX.setBackgroundColor("#9898a5", true);
    }
    // highlight new selection
    BookmarkSelectedBlkJSX.setBackgroundColor("#336666", true);
    // save as the last selection - for next time
    this.LastBookmarkSelectedBlkJSX = BookmarkSelectedBlkJSX;
    // set the new bookmark text
    // get comment from Bookmark node
    var nodeBookmark = topic_GetNIDNode(tCache, BookmarkSelectedBlkJSX.MarkID);
    if( nodeBookmark == null ) return;
    this.BookmarkSelectedTitle = nodeBookmark.selectSingleNode(".//Comment").getValue();
    // fire the select event with the current mark id
    if( this.SelEvent != null ) {
      // get the markID for publishing the event
      this.SelEvent.eventval = BookmarkSelectedBlkJSX.MarkID;
      CmpBookmarks.BaseClass.onSelect.call(this, BookmarkSelectedBlkJSX.MarkID);
    }
  }

  this.autoBookmarkHighlight = function(CurrSliderPosition) {
    // get list of bookmarks - find the nearest one
    var bookmarkBlkJSXList = this.MarkListBlkJSX.getChildren();
    var bookmarkSelection = null;
    var BookmarkDelta = 100;
    for(var i=0;i<bookmarkBlkJSXList.length;i++) {
      // for each bookmark - find its position (left dimension - without the % sign)
      var BookmarkDimensions = bookmarkBlkJSXList[i].getDimensions();
      var BookmarkPosition = BookmarkDimensions[0].replace("%", "");
      // compare with current slider position
      if( BookmarkPosition <= CurrSliderPosition &&
          (CurrSliderPosition - BookmarkPosition) <= BookmarkDelta ) {
        bookmarkSelection = bookmarkBlkJSXList[i];
        BookmarkDelta = CurrSliderPosition - BookmarkPosition;
      }
    }
    // if nearest bookmark has changed
    if( bookmarkSelection != null && 
        ThisClass.LastBookmarkSelectedBlkJSX != bookmarkSelection ) {
      // mark this bookmark as the currently selected one
      ThisClass.markSelected.call(ThisClass, bookmarkSelection);
    }
  }
  
}

//------------------------------------------
CmpIFrame.prototype = new Component();                                 
function CmpIFrame(BlkName, IFrameURL, EventArr) {
  CmpIFrame.BaseClass = CmpIFrame.prototype;
  CmpIFrame.prototype.constructor = CmpIFrame;
  if( BlkName != null ) {
    this.IFrameURL = IFrameURL;
    Component.prototype.constructor.call(this, BlkName, EventArr);
  }
  // extend init
  this.init = function() {
    CmpIFrame.BaseClass.init.call(this);
  }
  // override repaint
  this.repaint = function() {
    this.BlkJSX.setSrc(eval(this.IFrameURL));
    this.BlkJSX.repaint();
  }
} 

//---------------------------------------  
CmpSpinner.prototype = new Component();
function CmpSpinner(BlkName, EventArr, SelEvent, MaskBgColor) {  
  CmpSpinner.BaseClass = CmpSpinner.prototype;
  CmpSpinner.prototype.constructor = CmpSpinner;
  if(BlkName != null) {
    this.MaskBgColor = MaskBgColor;
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);    
  }
  // extend init
  this.init = function() {
    CmpSpinner.BaseClass.init.call(this);
    this.ImgSpinnerBlkJSX = this.BlkJSX.getDescendantOfName("imgSpinnerBG", false, false); 
  }
  //Override repaint
  this.repaint = function(EventVar, EventVal) {       
    if(EventVal == "SHOW") {            
      this.bWidth = this.BlkJSX.getWidth();
      this.bHeight = this.BlkJSX.getHeight();
      this.bLeft = this.bWidth/2;
      this.bTop = this.bHeight/2;       
      this.ImgSpinnerBlkJSX.setLeft(this.bLeft, true);
      this.ImgSpinnerBlkJSX.setTop(this.bTop, true);
      this.ImgSpinnerBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);      
      this.BlkJSX.repaint();
      this.BlkJSX.showMask("");
      this.BlkJSX.getLastChild().setCursor("default", true);
      this.BlkJSX.getLastChild().setBackgroundColor(this.MaskBgColor, true); 
    }
    else {
      if(EventVal == "HIDE") {
        this.BlkJSX.hideMask("");
        this.ImgSpinnerBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.BlkJSX.repaint();
      }
    }
  }
}

//---------------------------------------
CmpBlockButton.prototype = new Component();
function CmpBlockButton(BlkName, EventArr, SelEvent, showCSSRule, hideCSSRule) {
  // setup inheritence and call base constructor
  CmpBlockButton.BaseClass = CmpBlockButton.prototype;
  CmpBlockButton.prototype.constructor = CmpBlockButton;
  var ThisClass = this;
  if (BlkName != null) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    this.showCSSRule = showCSSRule;
    this.hideCSSRule = hideCSSRule;
  }
  this.Selected = false;
  
  // extend init
  this.init = function() {
    CmpBlockButton.BaseClass.init.call(this);
    
    // setup the select event handler
    //this.BlkJSX.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.BlkJSX.setEvent("this.onSelect();", jsx3.gui.Interactive.JSXCLICK);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
    
    this.enable= true; 
    // setup the mouseover / blur handler
    //this.BlkJSX.setCanSpy(jsx3.Boolean.TRUE);
    //this.BlkJSX.subscribe(jsx3.gui.Interactive.SPYGLASS, this, "onMouseOver");
    //this.BlkJSX.subscribe(jsx3.gui.Interactive.JSXBLUR, this, "onBlur");
  }

  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.JSXCLICK, this);
    //this.BlkJSX.unsubscribe(jsx3.gui.Interactive.SPYGLASS, this);
    //this.BlkJSX.unsubscribe(jsx3.gui.Interactive.JSXBLUR, this);
    CmpBlockButton.BaseClass.destroy.call(this);
  }

  this.show = function() {
    /*
    if (this.BlkJSX.setEnabled != null) {
      if (EventVal != "NONE") {
          this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      } else {
          this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
    }
    */
    if (this.showCSSRule != null) {
      this.BlkJSX.setClassName(this.showCSSRule);
      if(ThisClass.enable ==  false){
        this.BlkJSX.setCursor("not-allowed",true);
      } 
      else{
        this.BlkJSX.setCursor("pointer",true);
      }
    }
    //show this button even without CSS rule
    this.BlkJSX.repaint();
  }
  
  this.hide = function() {
    if (hideCSSRule) {
      this.BlkJSX.setClassName(this.hideCSSRule).repaint();
      if(ThisClass.enable ==  false){
        this.BlkJSX.setCursor("not-allowed",true);
      }
      else{
        this.BlkJSX.setCursor("pointer",true);
      }
    }
  }

  this.onSelect = function() {
    if(ThisClass.enable ==  true){
    CmpBlockButton.BaseClass.onSelect.call(ThisClass, null);
  }
  }  
 
/*
  this.onMouseOver = function() {
    if (selCSSRule != null) {
      this.previousCSS = this.BlkJSX.getClassName();
      this.BlkJSX.setClassName(selCSSRule).repaint();
    }
  }
  this.onBlur = function() {
    if (this.previousCSS != null) {
      this.BlkJSX.setClassName(this.previousCSS).repaint(); 
    }
  }
*/    
}

// Window class - extends Container - makes sure there is only one of each name
CmpAppWindow.prototype = new CmpContainer();
function CmpAppWindow(WinName, WinEventListner, WinXML, WinParams, SubCmpArr, EventArr, onDestroyEvent)
{
   // return the registered instance to avoid multiple objects for the same window
  if (CmpAppWindow[WinName] != null) {
    return CmpAppWindow[WinName];
  }
  // if not register a new Instance
  CmpAppWindow[WinName] = this;
  // stash win name / topic
  this.tWin = WinName;
  this.eventWin = WinEventListner;
  // stash win params
  this.WinParams = WinParams;
  // create an app window
  this.BlkJSX = PNameSpace.getAppWindow(WinName);
  if (this.BlkJSX == null) {
    this.BlkJSX = PNameSpace.createAppWindow(WinName);
    // load the XML in it
    var rootBlock = this.BlkJSX.getRootBlock();
    var childBlkJSX = rootBlock.loadAndCache(WinXML);
    rootBlock.setChild(childBlkJSX);
  }
  // set its properties
  if (this.WinParams.WinTitle) this.BlkJSX.setTitle(this.WinParams.WinTitle);
  if (this.WinParams.WinHeight) { 
    var lcWinHeight = this.WinParams.WinHeight > screen.availHeight ? screen.availHeight : this.WinParams.WinHeight;
    this.BlkJSX.setHeight(lcWinHeight);
  }
  if (this.WinParams.WinWidth) { 
    var lcWinWidth = this.WinParams.WinWidth > screen.availWidth ? screen.availWidth : this.WinParams.WinWidth;
    this.BlkJSX.setWidth(lcWinWidth);
  }
  if (this.WinParams.WinResizable) this.BlkJSX.setResizable(this.WinParams.WinResizable);
  if (this.WinParams.WinScrollable) this.BlkJSX.setScrollable(this.WinParams.WinScrollable);
  // more properties settings
  this.BlkJSX.setDependent(true);

  // setup inheritence
  CmpAppWindow.BaseClass = CmpAppWindow.prototype;
  CmpAppWindow.prototype.constructor = CmpAppWindow;
  // call base class constructor
  CmpContainer.prototype.constructor.call(this, WinName, SubCmpArr, EventArr, onDestroyEvent);
  
  // cleanup before close
  this.onClose = function() {
    //destroy any dialogs on top of this window if they are exist
    if (jsx3.gui.Dialog){
      try {
        var dialogBoxArr = this.BlkJSX.getDescendantsOfType(jsx3.gui.Dialog, false); 
        var dlgNameArr = new Object();
        for (var i = 0; i < dialogBoxArr.length; i++) {
          var lcDlgName = dialogBoxArr[i].getName();
          if(lcDlgName in dlgNameArr)continue;
          dlgNameArr[lcDlgName]=lcDlgName;
          dialogBoxArr[i].doClose(); 
        }
      } catch (e) {
        log.warn("clsComponent::CmpAppWindow - closing dialogs on top of this Window:" + e.description);
      }
    }
    // unsubscribe from the windowclosing event
    this.BlkJSX.unsubscribe(jsx3.gui.Window.WILL_CLOSE, this);
    //There is an issue with IE9 to access expandos bug#52002
    //While opening window we are passing information with opener
    //Remove that info while closing.
    var winName = this.BlkJSX.getId().replace(/\./g, "");
    if (!jsx3.util.strEmpty(window[winName])) window[winName] = null;
    //the onDestroyEvent may include topic_Publish,so it has to be called before any destruction
    if (this.SelEvent != null) {
      CmpAppWindow.BaseClass.onSelect.call(this, onDestroyEvent);
    }
    // destroy ourself
    this.destroy();
  }
  
  // destroy all window components
  this.destroy = function () {
    // call base class destory
    // -- turn off event processing in Container base class
    this.SelEvent = null;
    //clear root block hotkeys
    this.BlkJSX.getRootBlock().clearHotKeys();
    CmpAppWindow.BaseClass.destroy.call(this);
    // deref the app window
    this.BlkJSX.getParent().removeChild(this.BlkJSX);
    this.BlkJSX = null;
    // clear reference to self
    CmpAppWindow[this.tWin] = null;
    delete CmpAppWindow[this.tWin];
  }

  // close window - same as user clicking X
  this.close = function() {
    //check if this window no closed then
    if (this.BlkJSX != null) {
      this.BlkJSX.close();
//      this.onClose();
    }
  }

  this.onOpen = function () {
    this.BlkJSX.unsubscribeAll(jsx3.gui.Window.DID_OPEN);
    // setup subscription to destroy it before close
    this.BlkJSX.unsubscribe(jsx3.gui.Window.WILL_CLOSE, this.onClose);
    this.BlkJSX.subscribe(jsx3.gui.Window.WILL_CLOSE, this, this.onClose);
    // now create sub-components
    if (this.createSubCmp != null) {
      this.createSubCmp();
    }
    var thisBlkJSXParentBlock = this.BlkJSX.getParent();
    var thisBlkJSXBlock = thisBlkJSXParentBlock.getChild(this.BlkJSX.getName());
    thisBlkJSXBlock.focus();
    
    //regist hot key to get rendered HTML source code
    var intKey = jsx3.gui.HotKey.keyDownCharToCode("=");
    this.BlkJSX.getRootBlock().registerHotKey(promptUserToHtmlSrcLog, intKey, false, true, true);
  }

  this.open = function () {
    // make sure its open
    if (!this.BlkJSX.isOpen()) {
      this.BlkJSX.subscribe(jsx3.gui.Window.DID_OPEN, this, this.onOpen);
      //pass this value to me.opener._BlkJSX of jsx3.gui.window.html
      //window._BlkJSX = this.BlkJSX;
      //There is an issue with IE9 to access expandos bug#52002
      //While opening window we are passing information with opener
      window[this.BlkJSX.getId().replace(/\./g, "")] = this.BlkJSX;
      this.BlkJSX.open();
      //this.winOpen.call(this.BlkJSX);
    }
    else {
      this.onOpen();
    }
  }

  this.asyncOpen = jsx3.$Y(function (cb) {
    // make sure its open
    this.open();
    cb.done();
  });

  /** 
  * 'winOpen' is a wrapper of 'open' to support fullscreen window
  * this is copied from window.js - with changes for passing parameters to open
  * call to this function should be in the context of 'this.BlkJSX'
  **/
//  this.winOpen = function () {
// 
//    if (this._jsxwindow != null && !this._jsxwindow.closed) {
//      throw new Exception(jsx3._msg("gui.win.op", this));
//    } else {
//      // open the window
//      // apparently IE doesn't like this name to contain a period
//      var uri = jsx3.util.strEndsWith(jsx3.app.Browser.getLocation().getPath().toLowerCase(), ".xhtml") ?
//        "jsx3.gui.window.xhtml" : "jsx3.gui.window.html";
//      var winParamStr = "directories=no,location=no,menubar=no,status=no,personalbar=no,titlebar=no,toolbar=no," +
//        "width=" + this.getWidth() + "," +
//        "height=" + this.getHeight() + "," +
//        "resizable=" + (this.isResizable() ? "yes" : "no") + "," +
//          "scrollbars=" + (this.isScrollable() ? "yes" : "no");

//      //fullscreen window settings
//      if (WinParams.WinFullscreen) {
//        winParamStr = WinParams.WinParamStr;
//      }

//      var w = window.open(uri, this.getId().replace(/\./g, ""), winParamStr);

//      if (w != null) {
//        w["_jsxwindow"] = this;
//        this._jsxwindow = w;
//        this._jsxopener = window;
//        return true;
//      } else {
//        return false;
//      }
//    }
//  };

  this.show = function() {
    //bug fix 12790,closing wiondow before fully loaded,just return
    if (jsx3.util.strEmpty(this.BlkJSX)) return; 
    // make sure its open and focused
    if( ! this.BlkJSX.isOpen() ) {
      this.open();
    }
    else {
      this.BlkJSX.focus();
    }
    // then call base class show
    CmpAppWindow.BaseClass.show.call(this);
  }
}


//------------------------------------------
CmpBlkDropTarget.prototype = new Component();                                 
function CmpBlkDropTarget(BlkName, EventArr, SelEvent) {
  CmpBlkDropTarget.BaseClass = CmpBlkDropTarget.prototype;
  CmpBlkDropTarget.prototype.constructor = CmpBlkDropTarget;
  var ThisClass = this;
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function() {
    CmpBlkDropTarget.BaseClass.init.call(this);
    if(this.SelEvent != null) {
      this.BlkJSX.setCanDrop(jsx3.Boolean.TRUE);
      this.BlkJSX.removeEvent(jsx3.gui.Interactive.DROP);
      this.BlkJSX.setEvent("this.OnDrop(strDRAGID,objSOURCE,strDRAGTYPE)", jsx3.gui.Interactive.DROP);
      this.BlkJSX.OnDrop = this.OnDrop;
    }    
  }
  // override repaint
  this.repaint = function() {
    this.BlkJSX.repaint();
  }
  
  this.OnDrop = function(strDRAGID,objSOURCE,strDRAGTYPE) {
    var Val = new Object;
    Val.strDRAGID = strDRAGID;
    Val.objSOURCE = objSOURCE;
    Val.strDRAGTYPE = strDRAGTYPE;
    jsx3.sleep(function() {
      CmpBlkDropTarget.BaseClass.onSelect.call(ThisClass,Val);
    }, null, this);    
  }
}

/**
 * sliderMaxVal:maximum slide value of the slider;
 * currentWindowBlkJSX:a window of confirmation box to attatch;
 * confirmStr:the message to be displayed in the window; if null then no confirmation;
 * 
 **/
CmpSimpleSlider.prototype = new Component();
function CmpSimpleSlider(BlkName, sliderMaxVal,lblTxtBlkJSX, EventArr, SelEvent){
  // setup inheritence and call base constructor
  CmpSimpleSlider.BaseClass = CmpSimpleSlider.prototype;
  CmpSimpleSlider.prototype.constructor = CmpSimpleSlider;
  // stash value of this in local var for access from onSelect function
  var ThisClass = this;

  if (BlkName != null) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
  }
  // extend init
  this.init = function () {
    if (!jsx3.util.strEmpty(jsx3.gui.Slider.BRIDGE_EVENTS) ) {
      jsx3.gui.Slider.BRIDGE_EVENTS[jsx3.gui.Event.MOUSEWHEEL] = false;
    }
    else {
      jsx3.gui.Slider.yg[jsx3.gui.Event.MOUSEWHEEL] = false;
    } 
    CmpSimpleSlider.BaseClass.init.call(this);

    // setup the select event handler
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.CHANGE);
    this.BlkJSX.setEvent("this.onSelect(fpVALUE,fpPREVIOUS);", jsx3.gui.Interactive.CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onSelect = this.onSelect;
    // setup the select event handler
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.INCR_CHANGE);
    this.BlkJSX.setEvent("this.onDrag(fpVALUE);", jsx3.gui.Interactive.INCR_CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.BlkJSX.onDrag = this.onDrag;
    // init the offset and position
    this.setOffset(0);
  }
  this.destroy = function() {
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.INCR_CHANGE, this);
    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.CHANGE, this);
    CmpSimpleSlider.BaseClass.destroy.call(this);
  }
  // extend show
  this.show = function(EventVar, EventVal) {
    CmpSimpleSlider.BaseClass.show.call(this, EventVar, EventVal);
    if (this.BlkJSX.setEnabled != null)
    {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.BlkJSX.setHandleImage("images/slider/handleEnabled.gif");
  }
  // override hide
  this.hide = function() {
    if (this.BlkJSX.setEnabled != null) {
      this.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.BlkJSX.setHandleImage("images/slider/handleDisabled.gif");
  }
  // override repaintCheck
  this.repaintCheck = function() {
    return true;
  }
 
  // extend onselect - this will handle user moving the slider position
  this.onSelect = function(fpVALUE, fpPREVIOUS) {
    // calc the target offset based on the current position
    var TgtOffset = Math.round(fpVALUE * (sliderMaxVal / 100));
    ThisClass.setTxtOffset.call(ThisClass,TgtOffset);
    ThisClass.setTipOffset.call(ThisClass, TgtOffset);
    CmpSimpleSlider.BaseClass.onSelect.call(ThisClass, TgtOffset);
    return true;
  }

  this.onDrag = function(fpVALUE) {
    // calc the target offset based on the current position
    var TgtOffset = Math.round(fpVALUE * (sliderMaxVal / 100));
    ThisClass.setTipOffset.call(ThisClass, TgtOffset);
  }

  //change slider handler,tip,and text box view
  this.setOffset = function(TgtOffset) {
    if (TgtOffset!=null){
      // change slider view
      var fpVALUE = Math.round(TgtOffset / sliderMaxVal * 100);
      this.BlkJSX.setValue(fpVALUE);
      // change tip
      this.setTipOffset(TgtOffset);
    }
    else {
      // change tip
      this.setTipOffset("Default");
    }
  }

  //change tip view
  this.setTipOffset = function(TgtOffset) {
    // change tip
    if (TgtOffset == 0) TgtOffset = '0';
    this.BlkJSX.setTip(TgtOffset.toString());
  }

  //change text box view
  this.setTxtOffset = function(TgtOffset) {
    // change text box view
    if (TgtOffset == 0) TgtOffset = '0';
    lblTxtBlkJSX.setText(TgtOffset);
    lblTxtBlkJSX.repaint();
  }
}


//--------------------------------------- 
//this component is extended from CmpContainer
//SubCompNamesArr is the names of navigation button and name of label in order of First, Prev, Next, Last and lblName
CmpPageNavButtonsContainer.prototype = new CmpContainer();
function CmpPageNavButtonsContainer(BlkName, SubCompNamesArr, MtxObjectList, EventArr, SelEvent) {
  // setup inheritence and call base constructor
  CmpPageNavButtonsContainer.BaseClass = CmpPageNavButtonsContainer.prototype;
  CmpPageNavButtonsContainer.prototype.constructor = CmpPageNavButtonsContainer;
  if( BlkName != null ) {
    var SubCompNamesArr = SubCompNamesArr;
    this.MtxObjectList = MtxObjectList;

    this.cmpBtnFirst = new Component();
    if( SubCompNamesArr[0] != null ) {
      this.cmpBtnFirst = new CmpImageButton(SubCompNamesArr[0],
                                            null,
                                            {action:"FNCCALL", context:this,
                                              fnc:function() {
                                                    this.MtxObjectList.StartFrom = 1;  
                                                    // repaint the matrix                                                                                             
                                                    this.MtxObjectList.doRepaint("PAGINATION");
                                                }
                                            });
    }

    this.cmpBtnPrev = new Component();
    if( SubCompNamesArr[1] != null ) {
      this.cmpBtnPrev = new CmpImageButton(SubCompNamesArr[1],
                                           null,
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  var prevStartFrom = parseInt(this.MtxObjectList.StartFrom);
                                                  var countToFetch = parseInt(this.MtxObjectList.CountToFetch);
                                                  var objectCount = parseInt(this.MtxObjectList.ObjectCount);
                                                  
                                                  var nextStartFrom = prevStartFrom - countToFetch;
                                                  if( nextStartFrom <= 0){
                                                    nextStartFrom = 1;
                                                  }
                                                  this.MtxObjectList.StartFrom = nextStartFrom ;
                                                  // repaint the matrix               
                                                  this.MtxObjectList.doRepaint("PAGINATION");
                                                }
                                           });
    }

    this.cmpBtnNext = new Component();
    if( SubCompNamesArr[2] != null ) {
      this.cmpBtnNext = new CmpImageButton(SubCompNamesArr[2],
                                           null,
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  var prevStartFrom = parseInt(this.MtxObjectList.StartFrom);
                                                  var countToFetch = parseInt(this.MtxObjectList.CountToFetch);
                                                  var objectCount = parseInt(this.MtxObjectList.ObjectCount);
                                                  
                                                  var nextStartFrom = prevStartFrom + countToFetch;
                                                  if( nextStartFrom > objectCount){
                                                    nextStartFrom = objectCount;
                                                  }
                                                  this.MtxObjectList.StartFrom = nextStartFrom ;
                                                  // repaint the matrix
                                                  this.MtxObjectList.doRepaint("PAGINATION");
                                                }
                                           });
    }
    
    this.cmpBtnLast = new Component();
    if( SubCompNamesArr[3] != null ) {
      this.cmpBtnLast = new CmpImageButton(SubCompNamesArr[3],
                                           null,
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  var prevStartFrom = parseInt(this.MtxObjectList.StartFrom);
                                                  var countToFetch = parseInt(this.MtxObjectList.CountToFetch);
                                                  var objectCount = parseInt(this.MtxObjectList.ObjectCount);
                                                  
                                                  // Modulus (%) operator returns only the remainder. means last number of records to fetch
                                                  var lastCountToFetch = objectCount % countToFetch;
                                                  if(lastCountToFetch == 0){
                                                    lastCountToFetch = countToFetch;
                                                  }
                                                  var nextStartFrom = objectCount - lastCountToFetch + 1;
                                                  this.MtxObjectList.StartFrom = nextStartFrom ;
                                                  // repaint the matrix                                         
                                                  this.MtxObjectList.doRepaint("PAGINATION");
                                                }
                                           });
    }

    this.cmpLblRecordInfo = new Component();
    if( SubCompNamesArr[4] != null ) {
      this.cmpLblRecordInfo = new CmpLabel(SubCompNamesArr[4], null, null, null, "");
    }
    
    this.cmpTxtCurrentRecordNumber = new Component();
    if( SubCompNamesArr[5] != null ) {
      this.cmpTxtCurrentRecordNumber = new CmpTxtBoxWithEnterKey(SubCompNamesArr[5], null, null,null, 
                                                                {action:"FNCCALL", context:this, 
                                                                 fnc:function() {
                                                                        jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(this.cmpTxtCurrentRecordNumber.onSelect));
                                                                        var prevStartFrom = parseInt(this.MtxObjectList.StartFrom);
                                                                        var countToFetch = parseInt(this.MtxObjectList.CountToFetch);
                                                                        var objectCount = parseInt(this.MtxObjectList.ObjectCount);
                                                                        if(objectCount < countToFetch) {
                                                                          countToFetch = objectCount; 
                                                                        }
                                                                        var nextStartFrom = this.cmpTxtCurrentRecordNumber.BlkJSX.getValue();
                                                                        var validateNumber = true;
                                                                        if (jsx3.util.strEmpty(nextStartFrom) || (!jsx3.util.strEmpty(nextStartFrom) && /\D/.test(nextStartFrom)) || isNaN(nextStartFrom)) {
                                                                           validateNumber = false;
                                                                        }
                                                                        else{
                                                                          nextStartFrom = parseInt(nextStartFrom,10);
                                                                          if(nextStartFrom > objectCount || nextStartFrom <= 0){
                                                                            validateNumber = false;
                                                                          }
                                                                        }
                                                                        
                                                                        if(!validateNumber){
                                                                          if(!jsx3.util.strEmpty(jsx3.gui.Window)){
                                                                            windowAlert(this.BlkJSX, "Please enter numeric value between 1 and "+objectCount);
                                                                          }
                                                                          else{
                                                                            alert("Please enter numeric value between 1 and "+objectCount);
                                                                          }
                                                                          return false;
                                                                        }
                                                                        this.MtxObjectList.StartFrom = nextStartFrom ;
                                                                          
                                                                        this.MtxObjectList.doRepaint("PAGINATION");
                                                                     }
                                                                });
    }
    
    this.cmpBtnRefresh = new Component();
    if( SubCompNamesArr[6] != null ) {
      this.cmpBtnRefresh = new CmpImageButton(SubCompNamesArr[6],
                                              null,
                                              {action:"FNCCALL", context:this,
                                               fnc:function() {
                                                     // repaint the matrix                                                
                                                     this.MtxObjectList.doRepaint("REFRESH");
                                                   }
                                              });
    }

    this.cmpBtnRefreshSpinner = new Component();
    if( SubCompNamesArr[7] != null ) {
      this.cmpBtnRefreshSpinner = new CmpImageButton(SubCompNamesArr[7],
                                                      null,
                                                      {action:"FNCCALL", context:this,
                                                       fnc:function() {
                                                             // cancel the request
                                                             this.MtxObjectList.cmpReq.endRequest();
                                                              // hide the refreshspinner
                                                              this.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                              // show the refresh
                                                              this.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                                              if( this.cmpBtnRefreshNotify.BlkJSX ) {
                                                                this.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                              }                                                            
                                                           }
                                                      });
    }
    
    this.cmpBtnRefreshNotify = new Component();
    if( SubCompNamesArr[8] != null ) {
      this.cmpBtnRefreshNotify = new CmpImageButton(SubCompNamesArr[8],
                                              null,
                                              {action:"FNCCALL", context:this,
                                               fnc:function() {
                                                     // repaint the matrix                                                
                                                     this.MtxObjectList.doRepaint("REFRESH");
                                                   }
                                              });
    }
    
    var SubCmpArr = [{cmp:this.cmpBtnFirst, sel:"parseInt(this.MtxObjectList.StartFrom) > 1"},
                     {cmp:this.cmpBtnPrev, sel:"parseInt(this.MtxObjectList.StartFrom) > 1"},
                     {cmp:this.cmpBtnNext, sel:"(parseInt(this.MtxObjectList.StartFrom)+ parseInt(this.MtxObjectList.CountToFetch)) <= parseInt(this.MtxObjectList.ObjectCount)"},
                     {cmp:this.cmpBtnLast, sel:"(parseInt(this.MtxObjectList.StartFrom)+ parseInt(this.MtxObjectList.CountToFetch)) <= parseInt(this.MtxObjectList.ObjectCount)"},
                     {cmp:this.cmpLblRecordInfo},
                     {cmp:this.cmpTxtCurrentRecordNumber},
                     {cmp:this.cmpBtnRefresh},
                     {cmp:this.cmpBtnRefreshSpinner},
                     {cmp:this.cmpBtnRefreshNotify}
                    ];

    CmpContainer.prototype.constructor.call(this, BlkName, SubCmpArr, EventArr);
  }

  this.init = function() {
    CmpPageNavButtonsContainer.BaseClass.init.call(this);
  }

  this.show = function(EventVar, EventVal) {
    CmpPageNavButtonsContainer.BaseClass.show.call(this);
    
    if( EventVal == null ) return;
    
    if(EventVal == "LOADING"){
      // Show the refreshspinner
      this.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      // Hide the refresh
      this.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      if( this.cmpBtnRefreshNotify.BlkJSX ) {
        this.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }  
    }
    else if( EventVal == "DIRTY" ) {
      // Show the refreshNotify
      if( this.cmpBtnRefreshNotify.BlkJSX ) {
        this.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
      // Hide the refresh
      this.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);  
    }
    else {
      // hide the refreshspinner
      this.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      // show the refresh
      this.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      if( this.cmpBtnRefreshNotify.BlkJSX ) {
        this.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }      
    }
                                                
    var ListStartFrom = parseInt(this.MtxObjectList.StartFrom);
    var ListTotalDBCount = parseInt(this.MtxObjectList.ObjectCount);
    var ListDefaultFetchCount = parseInt(this.MtxObjectList.CountToFetch);
    
    var NextListStartFrom = ListStartFrom + ListDefaultFetchCount;
    var lblStart = ListStartFrom;
    var lblEnd = ListTotalDBCount;
    
    this.cmpBtnFirst.hide();
    this.cmpBtnPrev.hide();
    this.cmpBtnNext.hide();
    this.cmpBtnLast.hide();
    this.cmpLblRecordInfo.show();

    if( ListStartFrom > 1 ) {
      this.cmpBtnFirst.show();
      this.cmpBtnPrev.show();
    }
    if( NextListStartFrom <= ListTotalDBCount) {
      this.cmpBtnNext.show();
      this.cmpBtnLast.show();
      lblEnd = NextListStartFrom - 1;
    }

    var startLblMessage = "";
    if(this.cmpTxtCurrentRecordNumber.BlkJSX) {
      if( ListTotalDBCount == 0 ) {
        ListStartFrom = 0;
      }
      this.cmpTxtCurrentRecordNumber.BlkJSX.setValue(ListStartFrom);
      this.cmpTxtCurrentRecordNumber.show();    
    }
    else {
      startLblMessage = ListStartFrom;
      if( ListTotalDBCount == 0 ) {
        startLblMessage = 0;
      }
    }

    if(this.cmpLblRecordInfo.BlkJSX) {    
      var lblMessage = startLblMessage + " to " + lblEnd + " of " + ListTotalDBCount;
      this.cmpLblRecordInfo.BlkJSX.setText(lblMessage);
      this.cmpLblRecordInfo.BlkJSX.repaint();
    }

  } // end this.show
  
} // end CmpPageNavButtonsContainer

//---------------------------------------
CmpCustomPaginationList.prototype = new CmpList();
function CmpCustomPaginationList (BlkName, EventArr, SelEvent, FormatHandlerArr, SpyGlassEventFnc,XSLFile) {
// setup inheritence and call base constructor
  CmpCustomPaginationList.BaseClass = CmpCustomPaginationList.prototype;
  CmpCustomPaginationList.prototype.constructor = CmpCustomPaginationList;
  if( BlkName != null ) {
    CmpList.prototype.constructor.call(this, BlkName, null, null, EventArr, SelEvent, null, SpyGlassEventFnc);
    this.FormatHandlerArr = FormatHandlerArr;
  }
  //this.reqParams = [];
  var ThisClass = this;
  // extend init
  this.init = function() {
    CmpCustomPaginationList.BaseClass.init.call(this);
    // clear out any hard coded data
    this.BlkJSX.setXMLString(null);
    // clear any initial selection
    this.BlkJSX.setValue(null);
    this.responseXML = null;

   //apply XML transformers
    if (XSLFile) this.BlkJSX.setXMLTransformers(XSLFile);

    // check for any formathandler to be set
    // matrix column Name and CDF attribute name are same
    if( this.FormatHandlerArr != null ) {
      for( var colName in this.FormatHandlerArr ) {
        if( this.FormatHandlerArr[colName].ColumnName != null ) {
          var columnName = this.FormatHandlerArr[colName].ColumnName;
          var colObj = this.BlkJSX.getDescendantOfName(columnName, false, false);
          //var fname = "FncName";
          var formatHandlerFnc = this.FormatHandlerArr[colName].FncName;
          colObj.setFormatHandler(formatHandlerFnc);
        }
      }
    }
    this.NotificationTimerID = null;
    this.NotificationNIDArr = new Array();
    this.WhereNIDArr = new Array();
    // init the data req
    this.cmpReq.reqParams = [];
    if( ! jsx3.util.strEmpty(this.columnCmpReq) ) {
      this.columnCmpReq.reqParams = [];
    }
    if( ! jsx3.util.strEmpty(this.filterTagCmpReq) ) {
      this.filterTagCmpReq.reqParams = [];
    }
    
    //set event to handle sorting
    this.BlkJSX.removeEvent(jsx3.gui.Interactive.AFTER_SORT);
    this.BlkJSX.subscribe(jsx3.gui.Interactive.AFTER_SORT, this, "onSortReq");
    //handlers are zero based to get paginated list
    this.ResetToStart(1,50,0);
    
    this.nameSpace = PNameSpace.getEnv("NAMESPACE");
    
    // check if the component need to maintain the column list
    this.getMtxColumn();

  } // end init
  
  this.show = function(EventVar, EventVal) {
    if (jsx3.util.strEmpty(this.columnDataState)) {
      this.setMtxColumn();
    }
    CmpCustomPaginationList.BaseClass.show.call(this, EventVar, EventVal);
    if( EventVal == null ) return;
  }
  
  this.destroy = function() {
    this.responseXML = null;
    this.cmpReq.reqParams = null;
    this.cmpReq = null;
    if( ! jsx3.util.strEmpty(this.columnCmpReq) ) {
      this.columnMtxDoc = null;
      this.columnCmpReq.reqParams = null;
      this.columnDataState = null;
    }
    
    if( ! jsx3.util.strEmpty(this.cmpItemByNIDReq) ) {
      this.cmpItemByNIDReq.reqParams = null;
      this.cmpItemByNIDReq.eventType = null;
    }

    if( ! jsx3.util.strEmpty(this.NotificationTimerID) ) {
      clearTimeout(this.NotificationTimerID);
      this.NotificationTimerID = null;
    }
    this.NotificationNIDArr = null;
    this.WhereNIDArr = null;     

    this.BlkJSX.unsubscribe(jsx3.gui.Interactive.AFTER_SORT, this);
    CmpCustomPaginationList.BaseClass.destroy.call(this);
  }
  
  this.doRepaint = function(mode) {
    if( mode == "PAGINATION" ) {
      this.HScrollPos = 0;
    }
    else if( mode == "REFRESH" ) {
      this.getScrollPosition();
    }
    this.repaint("GetDataState", newServerDate());
  }
  // redefine repaint
  this.repaint = function(EventVar, EventVal) {
      if (!jsx3.util.strEmpty(EventVal)) {
          // check if dynamic column data is required
          if ((jsx3.util.strEmpty(this.columnDataState)) &&
          (!jsx3.util.strEmpty(this.columnMtxDoc))) {
              this.getColumnsFromDB();
          }
          else {
              // check if filterdoc is set
              if ((jsx3.util.strEmpty(this.filterTagDataState)) &&
            (!jsx3.util.strEmpty(this.filterTagDoc))) {
                  this.getFilterTagDoc();
              }

              // calling custom repaint function defined in the cmp js file
              // fill in list req params
              var SortPath = "";
              if (this.BlkJSX.getSortPath != null) {
                  SortPath = this.BlkJSX.getSortPath();
              }
              var SortByXpath = "";
              var SortByMetaData = "";
              if (!jsx3.util.strEmpty(this.SortPathArr)) {
                // this will give regular columns sort name
                SortByXpath = this.SortPathArr[SortPath]; 
                if (jsx3.util.strEmpty(SortByXpath)) {
                  // there are other 2 types of columns jsxTagValue and jsxMetaData
                  // check for these one 
                  var searchMetaData = SortPath.search(/jsxMetaData/);
                  if( searchMetaData != -1 ){
                    var SortByMetaData = SortPath.replace(/jsxMetaData/, "");
                    this.cmpReq.reqParams[this.cmpReqParamArr["SortByMetaData"]] = SortByMetaData;
                    SortByXpath = "";
                  }
                  else {
                    this.cmpReq.reqParams[this.cmpReqParamArr["SortByMetaData"]] = SortByMetaData;
                    var searchTagValue = SortPath.search(/jsxTagValue/);
                    if (searchTagValue == -1) {
                        SortByXpath = "$b/@NID";
                    }
                    else {
                        SortByXpath = this.SortPathArr["jsxTagValue"];
                        var tagName = SortPath.replace(/jsxTagValue/, "");
                        SortByXpath = SortByXpath.replace(/_TAGNAME_/, tagName);
                    }                  
                  }

                }
              }
              this.cmpReq.reqParams[this.cmpReqParamArr["SortByXpath"]] = SortByXpath;

              var SortDirection = null;
              if (this.BlkJSX.getSortDirection != null) {
                  SortDirection = this.BlkJSX.getSortDirection();
              }
              if (jsx3.util.strEmpty(SortDirection)) {
                  SortDirection = "ascending";
              }
              this.cmpReq.reqParams[this.cmpReqParamArr["SortOrder"]] = SortDirection;
              this.cmpReq.reqParams[this.cmpReqParamArr["ObjectCount"]] = this.ObjectCount;
              this.cmpReq.reqParams[this.cmpReqParamArr["StartFrom"]] = this.StartFrom;
              this.cmpReq.reqParams[this.cmpReqParamArr["CountToFetch"]] = this.CountToFetch;
              

              // publish the event to show the spinner
              this.doCmpEvent(this.DataUpdatedEvent, null, "LOADING");
              // send the request
              //this.cmpReq.sendRequest();  
              this.sendRequest();

              // check if any refreshInterval is set If yes then set the setInterval for the matrix
              // cmpList show() will also check and do the setInterval if this.IntervalID =null
              // because hise will set this.IntervalID =null
              if (!jsx3.util.strEmpty(this.RefreshInterval)) {
                  if (!jsx3.util.strEmpty(this.IntervalID)) {
                      clearInterval(this.IntervalID);
                      this.IntervalID = null;
                  }
                  var Interval = topic_Get(this.RefreshInterval.topic, this.RefreshInterval.topicVar);
                  this.RefreshIntervalTime = parseInt(Interval) * 1000
                  this.IntervalID = setInterval(this.sendRequest, this.RefreshIntervalTime);
              }
          }
      }
  }

  this.sendRequest = function(){
    ThisClass.cmpReq.sendRequest();
  }

  this.onResponse = function (respCode, respVal) {
    if (respCode == "SUCCESS" && respVal != null) {
      // clear curent data view
      this.clearXMLData();
      this.BlkJSX.setValue(null);
      this.selRecordID = null;
      var strXMLCacheId = this.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(respVal.toString());
      // save the retunr value (result of all transforms and return it at the end for caller to use
      var docXformResp = this.BlkJSX.setSourceXML(objXML);
      // if  RefreshData and RefreshColCell is set the create
      if ((!jsx3.util.strEmpty(this.RefreshData)) &&
          (!jsx3.util.strEmpty(this.RefreshColArr))) {
        //clear refresh array          
        this.clearRefresh();
        this.CellRefreshArr = new Object;
        this.ObjectRefreshArr = new Object;
        this.initRefresh(respVal);
      }
      // just to avoid click/select while painting
      this.BlkJSX.getParent().showMask("Loading...");
      this.postpaint();
      var THISCLASS = this;
      jsx3.sleep(function () { THISCLASS.hideMenuUILoading(); }, null, this);
      this.onSelect();
      this.updatePaginationBlock();

      return docXformResp;
    }
    // this is to repaint if there is an error in the response.
    this.postpaint();
    this.onSelect();
    this.updatePaginationBlock();
  }
  
  this.initRefresh = function(respVal) {
    var refreshInfo = this.RefreshData;
    if (refreshInfo.type == "agentobjtype") {
      var ObjAgentCacheRoot = arrAgentObjectInfo[refreshInfo.objtype];
          // ier over records
      var recordtListIter = respVal.selectNodeIterator(".//record");
      while (recordtListIter.hasNext()) {
        var recordNode = recordtListIter.next();
        var ObjAgentJID = recordNode.getAttribute(refreshInfo.agentJID);
        var RowID = recordNode.getAttribute("jsxid");
        var Status = recordNode.getAttribute(refreshInfo.status);
        var objName = recordNode.getAttribute("jsxtext");
        //Remove any referencees for same device.
        //i.e in case device went offline and comes back to online
        if( !jsx3.util.strEmpty(this.ObjectRefreshArr[refreshInfo.objtype]) && 
            !jsx3.util.strEmpty(this.ObjectRefreshArr[refreshInfo.objtype][ObjAgentJID]) ) {
          clearAgentObjRequest(ObjAgentJID, refreshInfo.objtype);
          delete this.ObjectRefreshArr[refreshInfo.objtype][ObjAgentJID];
          this.ObjectRefreshArr[refreshInfo.objtype][ObjAgentJID] = null;
        }
        if( !jsx3.util.strEmpty(this.CellRefreshArr[ObjAgentJID]) ) {
          delete this.CellRefreshArr[ObjAgentJID];
          this.CellRefreshArr[ObjAgentJID] = null;
        }

        if (jsx3.util.strEmpty(ObjAgentJID)) {
          log.error(" 'onresponse()' NO status request for device:'" + objName + "' AgentJID is empty , status: " + Status);
          continue;
        }

        if (this.ObjectRefreshArr[refreshInfo.objtype] == null) {
          this.ObjectRefreshArr[refreshInfo.objtype] = new Object;
        }
        //set up the ObjectRefreshArr array
        this.ObjectRefreshArr[refreshInfo.objtype][ObjAgentJID] = { "RefreshInterval": refreshInfo.refreshInterval,
          "Status": Status,
          "XPath": refreshInfo.xpath,
          "IntervalID": null
        };


        // set up the CellRefreshArr array
        for (var col in this.RefreshColArr) {
          var colName = this.RefreshColArr[col].objColumnName;
          var colValObjectXPath = eval(this.RefreshColArr[col].objXPath);
          var colValObjectType = this.RefreshColArr[col].objtype;
          if (this.CellRefreshArr[ObjAgentJID] == null) {
            this.CellRefreshArr[ObjAgentJID] = new Object;
          }
          var colObj = this.BlkJSX.getDescendantOfName(colName, false, false);
          this.CellRefreshArr[ObjAgentJID][colName] = { "RowID": RowID,
            "AgentObj": colValObjectType,
            "ColObj": colObj,
            "ColObjXPath": colValObjectXPath
          };
        }
      }
    }
    this.show();
  }
  // this func when called will deselect the record after select
  this.deSelectAfterSelect = function(){
    this.getScrollPosition();
    // just deselect all - now that the onfocus has been patched - this works as well
    this.BlkJSX.deselectAllRecords();
    this.postpaint();    
  } // end deSelectAfterSelect

  // add to the matrix 'tr' xsl to change the css onmouse over/out
  CmpCustomPaginationList.ADDMOUSEOVEREVENT_TEMPLATE = new jsx3.xml.Document().loadXML(
                                              '<xsl:if xmlns:xsl="http://www.w3.org/1999/XSL/Transform" test="@jsxunselectable!=\'1\' and $jsx_1 and $jsx_1!=\'\'">\n' +
                                              '  <xsl:attribute name="onmouseover"><xsl:text>this.className=\'</xsl:text><xsl:value-of select="$jsx_1"/><xsl:text>\'</xsl:text></xsl:attribute>\n' +
                                              '  <xsl:attribute name="onmouseout"><xsl:text>this.className=\'\'</xsl:text></xsl:attribute>\n' +
                                              '</xsl:if>\n');
  CmpCustomPaginationList.ADDMOUSEDOWNEVENT_TEMPLATE = new jsx3.xml.Document().loadXML(
                                              '<xsl:if xmlns:xsl="http://www.w3.org/1999/XSL/Transform" test="@jsxunselectable!=\'1\' and $jsx_2 and $jsx_2!=\'\'">\n' +
                                              '  <xsl:attribute name="onmousedown"><xsl:text>this.className=\'</xsl:text><xsl:value-of select="$jsx_2"/><xsl:text>\'</xsl:text></xsl:attribute>\n' +
                                              '  <xsl:attribute name="onmouseup"><xsl:text>this.className=\'\'</xsl:text></xsl:attribute>\n' +
                                              '</xsl:if>\n');
  // patch the getXSL method - to insert the mouse over/out handling to the tr element
  this.BlkJSXgetXSL = this.BlkJSX.getXSL;
  this.BlkJSX.getXSL = function() {
    // call the original method to get the orig xsl doc
    var docXSL = ThisClass.BlkJSXgetXSL.apply(ThisClass.BlkJSX, arguments);
    if( arguments[0]!=true && ! docXSL.selectSingleNode("//xsl:attribute[@name='onmouseover']") ) {
      // clone the incoming  doc - (to work around some problem re: read-only doc modification)
      var docXSL = docXSL.cloneDocument();
      // look for 'tr' elements that spec style
      var iterNodeTRStyle = docXSL.selectNodeIterator("//tr[@style]");
      // inside each - append a copy of the mouse event template
      while( iterNodeTRStyle.hasNext() ) {
        var nodeTRStyle = iterNodeTRStyle.next();
        var objXSLSnippet = CmpCustomPaginationList.ADDMOUSEOVEREVENT_TEMPLATE.getRootNode().cloneNode(true);
        nodeTRStyle.insertBefore(objXSLSnippet, nodeTRStyle.getFirstChild());
        var objXSLSnippet = CmpCustomPaginationList.ADDMOUSEDOWNEVENT_TEMPLATE.getRootNode().cloneNode(true);
        nodeTRStyle.insertBefore(objXSLSnippet, nodeTRStyle.getFirstChild());
      }
    }
    // look for +/- 'td' element and remove onfocus/onblur
//    var nodeTDOnFocus = docXSL.selectSingleNode(".//td[.//div/*/@name='ui_controller']");
//    if(nodeTDOnFocus){
//      nodeTDOnFocus.removeAttribute("onfocus");
//      nodeTDOnFocus.removeAttribute("onblur");
//    }
    
    /*Removing onfocus event on matrix cells.Only onclick called when user clicks a row.
    Changed to support the event bubbling modle in FF and MSIE */    
    if (ThisClass.rowDeselection == true){
      var iterTDOnFocus = docXSL.selectNodeIterator(".//td[@onfocus]");
      while(iterTDOnFocus.hasNext()){
        var nodeTDOnFocus = iterTDOnFocus.next();
        nodeTDOnFocus.removeAttribute("onfocus");
        nodeTDOnFocus.removeAttribute("onblur");
      }
    }
    // look for 'td' 'icon' element and remove it
    var nodeTDIcon = docXSL.selectSingleNode(".//td[@jsxtype='icon']");
    if(nodeTDIcon){
      nodeTDIcon.getParent().removeChild(nodeTDIcon);
    }
    // return modified doc
    return docXSL;
  }
  
  // extend onselect 
  this.onSelect = function(objEvent) {
    CmpCustomPaginationList.BaseClass.onSelect.call(this);
  }

  this.handleNotification = function(objEvent) {
    if( objEvent == null ) return;
    var eventType = objEvent.type;
    if( eventType == "add" ) {
      this.markDirty();
      return;
      // not addingin the list, just making it dirty
//      if( parseInt(this.ObjectCount) >= parseInt(this.CountToFetch) ) {
//        return;
//      }     
    }
    else if( eventType == "replace" ) {
      var ItemNode = this.BlkJSX.getRecord(objEvent.nodeID);
      if( jsx3.util.strEmpty(ItemNode) ) {
        return;
      }
    }
    else if( eventType == "delete" ) {
      var ItemNode = this.BlkJSX.getRecord(objEvent.nodeID);
      if( !jsx3.util.strEmpty(ItemNode) ) {
        this.BlkJSX.deleteRecord(objEvent.nodeID);
      }
      this.markDirty();
      return;
    }
    if( !jsx3.util.strEmpty(this.NotificationNIDArr) ) {
        this.NotificationNIDArr.push(objEvent.nodeID);
        if( jsx3.util.strEmpty(this.NotificationTimerID) ) {
          this.NotificationTimerID = setTimeout(this.sendReqForNotificationNIDs, 2000);
      }
    }
  }
  this.markDirty = function() {
    this.doCmpEvent(this.DataUpdatedEvent, null, "DIRTY");
  }

  this.sendReqForNotificationNIDs = function() {
    ThisClass.NotificationTimerID = null;
    if (jsx3.util.strEmpty(ThisClass.cmpItemByNIDReqFnc)) return;
    if( ThisClass.NotificationNIDArr.length > 0 ) {
      ThisClass.WhereNIDArr = ThisClass.NotificationNIDArr;
      ThisClass.NotificationNIDArr = new Array();

      ThisClass.cmpItemByNIDReq = ThisClass.cmpItemByNIDReqFnc.call();
      ThisClass.cmpItemByNIDReq.reqParams = [];
      ThisClass.cmpItemByNIDReq.reqParams[ThisClass.cmpReqParamArr["SortByXpath"]] = "$b/@NID";
      ThisClass.cmpItemByNIDReq.reqParams[ThisClass.cmpReqParamArr["SortOrder"]] = "ascending";
      ThisClass.cmpItemByNIDReq.reqParams[ThisClass.cmpReqParamArr["ObjectCount"]] = 0;
      ThisClass.cmpItemByNIDReq.reqParams[ThisClass.cmpReqParamArr["StartFrom"]] = 1;
      ThisClass.cmpItemByNIDReq.reqParams[ThisClass.cmpReqParamArr["CountToFetch"]] = 5000;
      ThisClass.cmpItemByNIDReq.sendRequest();
    }
  }
  
  this.handleResponseForNotificationNIDs = function(respVal, xslFileName, xslParam, xslParamVal) {
    var xslUriRes = null;
    var UriRes = this.BlkJSX.getUriResolver();
    if( jsx3.util.strEmpty(UriRes) ) return;
    xslUriRes = UriRes.resolveURI("xsl/" + xslFileName +".xsl");
    if( jsx3.util.strEmpty(xslUriRes) ) return;

    var XSLFile = PNameSpace.loadInclude(xslUriRes, xslFileName + "_Update", "xsl");
    if( XSLFile ) {
      var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
      if( !jsx3.util.strEmpty(xslParam) ) {
        XSLDoc.setParam(xslParam, xslParamVal);
      }
      var updatedPortDoc = XSLDoc.transformToObject(respVal);
      this.onResponseUpdateRecord("SUCCESS", updatedPortDoc);
    }
  }
  this.onResponseUpdateRecord = function (respCode, respVal) {
    if (respCode != "SUCCESS") return;
    var dataNode = respVal.cloneNode(true);
    var recordNodeIterartor = respVal.selectNodeIterator(".//record");
    while (recordNodeIterartor.hasNext()) {
      var recordNode = recordNodeIterartor.next();
      var nodeId = recordNode.getAttribute("jsxid");
      var orgRecordNode = this.BlkJSX.getRecordNode(nodeId); 
      //check if the record is there or not.
      if (!jsx3.util.strEmpty(orgRecordNode)) {
        var isRecordSelected = null;
        if (!jsx3.util.strEmpty(orgRecordNode)) {
          isRecordSelected = orgRecordNode.getAttribute("jsxselected");
        }
        if (!jsx3.util.strEmpty(isRecordSelected)) {
          recordNode.setAttribute("jsxselected", "1");
        }
        this.BlkJSX.insertRecordNode(recordNode);
        //To Fix Bug#47402, #47458 and #50280
        //this.onSelect();
      }
    }
    if( (!jsx3.util.strEmpty(this.RefreshData)) &&
        (!jsx3.util.strEmpty(this.RefreshColArr))) {
      this.initRefresh(dataNode);
    }
  }

  this.onResponseUpdateXML = function(respCode, respVal) {
    var responseXML = this.responseXML;
    if( jsx3.util.strEmpty(responseXML) ) return;
    var newNodeIter = respVal.selectNodeIterator(".//Port");
    while( newNodeIter.hasNext() ) {
      var newNode = newNodeIter.next();
      var nodeId = newNode.getAttribute("NID");
      var oldNode = responseXML.selectSingleNode(".//Port[@NID='" + nodeId + "']");
      if( !jsx3.util.strEmpty(oldNode) ) {
        responseXML.replaceNode(newNode, oldNode);
      }
      else {
        responseXML.appendChild(newNode);
      }
    }
    this.responseXML = responseXML;
  }

  this.getResponseXML = function() {
    return this.responseXML;
  }
     // To Paint Button view for Media list
/*  this.onResponseButtonView = function(respCode, respVal, window, eventListener, repaintFlag) {
      var winMediaList = window;
      var tMediaList = eventListener;
      if (respCode == "SUCCESS" && respVal != null) {
          if (repaintFlag) {
              if (this.cmpReq.req.ObjectList_Type == "SrcPort") {
                  var sourceResponse = respVal;
                  var portIterartor = sourceResponse.selectNodeIterator(".//Port");
                  while (portIterartor.hasNext()) {
                      var sourceDataInfo = portIterartor.next();
                      //Send to clone function
                      var cmpBlkConnectionSourceBlk = cmpBlkSource_Create(winMediaList, sourceDataInfo, tMediaList);
                  }
              }
              else if (this.cmpReq.req.ObjectList_Type == "DstPort") {
                  var dstResponse = respVal;
                  var portIterartor = dstResponse.selectNodeIterator(".//Port");
                  while (portIterartor.hasNext()) {
                      var dstDataInfo = portIterartor.next();
                      //Send to clone function
                      var cmpBlkConnectionDstBlk = cmpBlkDst_Create(winMediaList, dstDataInfo, tMediaList);
                  }
              }
          }
      }
  } */
  
  
  this.onRefreshResponse = function(respCode, respVal) {
    // helper function
    function insertDataInMatrix(BlkJSX, cacheNode, RecordPaintTime) {
	    var elementNode = cacheNode;
	    var o = new Object();
	    for( var col in RecordArr ) {
	      var colName = RecordArr[col].objColumnName
	      var colValObjectXPath = eval(RecordArr[col].objXPath);
	      var objValNode = null;
	      try {
		      objValNode = elementNode.selectSingleNode(colValObjectXPath);
	      } catch(e) {
	      }

	      // set col data in matrix record - do lookup arr if needed
	      o[colName] = "-";
	      if( objValNode != null ) {
		      var colVal = objValNode.getValue();
		      o[colName] = colVal;
	      } 
	    }
	    o["RecordPaintTime"] = RecordPaintTime;
	    BlkJSX.insertRecord(o, null, false);       
    } // end helper insertDataInMatrix
    
    //helper function
    function refreshDataInMatrix(BlkJSX, RecordArr, recordNode, cacheNode, RecordPaintTime) {
	    var elementNode = cacheNode;
	    var ID = recordNode.getAttribute("jsxid");
    		 
	    var strXMLCacheId = BlkJSX.getXMLId();
	    var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);      
	    if(objXML2) {
	      var objRecordNodes = objXML2.selectNodes("//record");
	      for (var it = objRecordNodes.iterator();it.hasNext();){
		      var RecordNode = it.next();
		      var RecordID = RecordNode.getAttribute("jsxid");
		      if(ID == RecordID) {
		        for( var col in RecordArr ) {
			        var colName = RecordArr[col].objColumnName;
			        var ColObj = BlkJSX.getDescendantOfName(colName, false, false);
			        var colValObjectXPath = eval(RecordArr[col].objXPath);
			        var objValNode = null;
			        try {
			          objValNode = elementNode.selectSingleNode(colValObjectXPath);
			        } catch(e) {
			        }

			        // set col data in matrix record - do lookup arr if needed
			        if( objValNode != null ) {
			          var colVal = objValNode.getValue();
			          RecordNode.setAttribute(colName, colVal);
			          BlkJSX.redrawCell(ID,ColObj ,"");
			        }
		        }
		        RecordNode.setAttribute("RecordPaintTime", RecordPaintTime);             
		      }
	      }
	    }        
    } // end helper refreshDataInMatrix
    
    //helper function
    function cleanUPMatrix(BlkJSX, RecordPaintTime) {
	    // getting the cdf doc for the blkjsx (ie matrix)
	    var strXMLCacheId = BlkJSX.getXMLId();
	    var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);      
	    if(objXML2) {
	      var objRecordNodes = objXML2.selectNodes("//record");
	      for (var it = objRecordNodes.iterator();it.hasNext();){
		      var RecordNode = it.next();
		      var CloneoRecordNode = RecordNode.cloneNode(true);
		      var RecordPaintedTime = CloneoRecordNode.getAttribute("RecordPaintTime");
		      if( RecordPaintedTime < RecordPaintTime) {
		        var selRecordID = CloneoRecordNode.getAttribute("jsxid");
		        BlkJSX.deleteRecord(selRecordID,true);
		      }
	      }      
	    }      
    } // end helper cleanUPMatrix
    
    // start main code
    ThisClass.BlkJSX.setValue(null); 
    ThisClass.selRecordID = null;
    var MtxRecordArr = [];
    var CacheRecordArr = [];
    var RecordArr = ThisClass.RecordArr;

    var RecordPaintTime = new Date().getTime();    
    // getting the cdf doc for the blkjsx (ie matrix)
    var strXMLCacheId = ThisClass.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId); 
    if(objXML) {
	    var objRecordNodes = objXML.selectNodes("//record");
	    for (var it = objRecordNodes.iterator();it.hasNext();){
	      var RecordNode = it.next();
	      var CloneoRecordNode = RecordNode.cloneNode(true);
	      var strRecordID = CloneoRecordNode.getAttribute("jsxid");
	      MtxRecordArr.push({"ID":strRecordID, "DataNode":CloneoRecordNode});
	    }      
    }
    var MtxRecordArrLength = MtxRecordArr.length;
    
	  var elementListIter = respVal.selectNodeIterator(ThisClass.RefreshInterval.DataNodeIteratorXPath);
	  while( elementListIter.hasNext() ) {
		  var elementNode = elementListIter.next();
		  var CloneElementNode = elementNode.cloneNode(true);
		  var elementID = CloneElementNode.getAttribute("id");
		  CacheRecordArr.push({"ID":elementID, "DataNode":CloneElementNode});
	  }
       
    var CacheRecordArrLength = CacheRecordArr.length;
    
    // if cachedata is empty then reset the matrix data
    if( CacheRecordArrLength == 0 ) {
	    ThisClass.BlkJSX.resetCacheData();
	    ThisClass.postpaint(ThisClass);        
    } else {
	    // if cache has data and matrix is empty then insert all the cache data
	    var RepaintReq = false;
	    if(MtxRecordArrLength == 0) {
	      for(var CRAL=0;CRAL<CacheRecordArr.length;CRAL++) {
		      var cacheNode = CacheRecordArr[CRAL].DataNode;
		      insertDataInMatrix(ThisClass.BlkJSX, cacheNode, RecordPaintTime);
	      }
	      RepaintReq = true;
	      //ThisClass.postpaint(ThisClass);        
	    } else { 
	      // if cache has data and matrix has data then update the cdf of the matrix
	      for(var CRAL=0; CRAL < CacheRecordArr.length; CRAL++) {
		      var CacheID = CacheRecordArr[CRAL].ID;
		      var cacheNode = CacheRecordArr[CRAL].DataNode.cloneNode(true);
		      var flag = false;
		      for(var MRAL=0; MRAL < MtxRecordArr.length; MRAL++) {
		        var MtxRecordID = MtxRecordArr[MRAL].ID;
		        //if data present then update data
		        if( CacheID == MtxRecordID) {
			        flag = true;
			        var recordNode = MtxRecordArr[MRAL].DataNode.cloneNode(true);
			        refreshDataInMatrix(ThisClass.BlkJSX, RecordArr, recordNode, cacheNode, RecordPaintTime);
			        break;
		        }
		      }
		      //if cache data not in the matrix then insert it
		      if(!flag){
		        insertDataInMatrix(ThisClass.BlkJSX, cacheNode, RecordPaintTime);
		        RepaintReq = true;  
		      }
	      }
	    }
	    if(MtxRecordArrLength != CacheRecordArrLength) {
	      RepaintReq = true;
	    }        
	    if(RepaintReq) {
	      cleanUPMatrix(ThisClass.BlkJSX, RecordPaintTime);
	      ThisClass.postpaint();
	    }              
    } // end if
    
    ThisClass.onSelect();
    ThisClass.updatePaginationBlock();
    
    var THISCLASS = ThisClass;
    jsx3.sleep(function () {
      THISCLASS.hideMenuUILoading();
    }, null, this);

    
  } // end onRefreshResponse

  this.hideMenuUILoading = function () {
    this.BlkJSX.getParent().hideMask("");
    var hideblkJSX = this.BlkJSX.getAncestorOfName("blkMenuUI");
    if (!jsx3.util.strEmpty(hideblkJSX)) {
      hideblkJSX.getParent().hideMask("");
    }
  }

  this.updatePaginationBlock = function() {
    ThisClass.ObjectCount = ThisClass.cmpReq.reqParams[ThisClass.cmpReqParamArr["ObjectCount"]];
    ThisClass.StartFrom = ThisClass.cmpReq.reqParams[ThisClass.cmpReqParamArr["StartFrom"]];
    ThisClass.CountToFetch = ThisClass.cmpReq.reqParams[ThisClass.cmpReqParamArr["CountToFetch"]];
    
    ThisClass.doCmpEvent(ThisClass.DataUpdatedEvent); 
  }

  this.onResponseErrorHandle = function (requestName, respCode, respVal) {
    this.hideMenuUILoading();

    if (!jsx3.util.strEmpty(respVal)) {
      this.postpaint();
      this.onSelect();
      this.updatePaginationBlock();
      try{
        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
        var ErrorMsg = ""+requestName+" Request failed \n";
        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
        ErrorMsg += "Error code: " + respCode;
        if(!jsx3.util.strEmpty(jsx3.gui.Window)){
          windowAlert(this.BlkJSX, ErrorMsg);
        }
        else{
          alert(ErrorMsg);
        }
      }
      catch(e){
      }
      return false;
    }
    // this is to repaint if there is an error in the response.
    this.postpaint();
    this.onSelect();
    this.updatePaginationBlock();
  }
     
  this.onSortReq = function() {
    this.clearXMLData();
    this.doRepaint();
  }
  
  this.ResetToStart = function(StartFrom, CountToFetch, ObjectCount) {
    this.StartFrom = StartFrom;
    this.CountToFetch = CountToFetch;
    this.ObjectCount = ObjectCount;
    //this.clearXMLData();
  }
  
  this.clearXMLData = function() {
    if(this.BlkJSX.clearXmlData != null){
      this.BlkJSX.clearXmlData();
    }
    else if(this.BlkJSX.resetData != null){
      this.BlkJSX.resetData();
    }
    this.postpaint();  
    this.doCmpEvent(this.DataUpdatedEvent);
  }
  
  this.getMtxColumn = function() {
    if( jsx3.util.strEmpty(this.columnMtxDoc) ) return;
    
    this.columnMtxDoc.removeChildren();
    var columnListNode = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/" + this.screenName, null);
    log.info("\nclsComponent::CmpCustomPaginationList:getMtxColumn:: " + this.screenName + " :: saved columnListNode: " + columnListNode + "\n");
    //if there is no saved column list then get from the matrix
    if( jsx3.util.strEmpty(columnListNode) ) {
      // get the colunmns of the matrix
      var colArr = this.BlkJSX.getDescendantsOfType(jsx3.gui.Matrix.Column, false);
      for(var i=0; i < colArr.length; i++ ) {
        var colObj = colArr[i];
        var jsxid = colObj.jsxname;
        // look for only static columns, dynamic columns will have jsxTagValue/jsxMetaData prefix
        var searchResult = jsxid.search(/jsxTagValue/);
        var searchResultMetaData = jsxid.search(/jsxMetaData/);
        var searchResultHidden = jsxid.search(/jsxHidden/);
        var searchResultImage = jsxid.search(/jsximg/);
        var searchResultMediaFileImgage = jsxid.search(/jsxMediaFileImg/);
        if( (searchResultMetaData == -1 && searchResult == -1 && searchResultHidden == -1 && searchResultImage == -1 && searchResultMediaFileImgage == -1) ) {
          var o = new Object;
          o.jsxid = jsxid;
          o.jsxColumnName = colObj.jsxtext;
          var colDisplay = colObj.getDisplayIndex();
          o.jsxChkColumn = '1';
          if( jsx3.util.strEmpty(colDisplay) ) {
            o.jsxChkColumn = '0';
          }
            
          var recordNode = topic_Set(tApp, "record", null); 
          recordNode.setAttribute("jsxid",o.jsxid);
          recordNode.setAttribute("jsxColumnName",o.jsxColumnName);
          recordNode.setAttribute("jsxChkColumn",o.jsxChkColumn);                    
          this.columnMtxDoc.appendChild(recordNode);
        }
      }
    }
    // save the saved column list in the matrix column doc
    else{
      var colDataNode = new jsx3.xml.Document().loadXML(columnListNode);
      var colNodes = colDataNode.selectNodes("//data/*");
      for(var it = colNodes.iterator(); it.hasNext(); ) {
        var colDataNode = it.next();
        this.columnMtxDoc.appendChild(colDataNode);
      }      
    } 
  }

  this.setMtxColumn = function () {
    if (jsx3.util.strEmpty(this.columnMtxDoc)) return;

    var objXML = this.columnMtxDoc;

    // go thru the columns in the objXML. if column not found then create it
    // do show/hide as well
    var objRecordNodes = objXML.selectNodes("//record");
    var objBefore = null;
    for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
      var objRecordNode = it.next();
      var colName = objRecordNode.getAttribute("jsxid");
      colName = colName.split(' ').join('');
      var colHeaderName = objRecordNode.getAttribute("jsxColumnName");
      var ColDisplay = objRecordNode.getAttribute("jsxChkColumn");
      var colCanSort = objRecordNode.getAttribute("jsxColumnSortable");
      var colObj = this.BlkJSX.getDescendantOfName(colName, false, false);
      if (this.InsertBeforeColumnName == colName) {
        objBefore = colObj;
      }
      if (jsx3.util.strEmpty(colObj)) {
        //colObj = this.BlkJSX.loadAndCache("JSXAPPS/IPVS_AdminUI/xml/defaults/AssetColumn.xml", false);
        colObj = this.BlkJSX.load("JSXAPPS/IPVS_AdminUI/xml/defaults/AssetColumn.xml");
        colObj.setName(colName, true);
        colObj.setPath(colName, true);
        colObj.setText(colHeaderName, true);
        colObj.setCanSort(colCanSort);
        colObj.setCellPadding("2 2 2 2", true);
      }
      //colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, false); 
      if (ColDisplay == '1' && colObj.getDisplay() != "") {
        colObj.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
      else if (ColDisplay == '0' && colObj.getDisplay() != "none") {
        colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }
//      else {
//        colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
//      }
    }
    //this.postpaint();
  }     
    
  
  this.getColumnsFromDB = function() {
    this.getScrollPosition();
    // check if the component need to maintain the column list
    if( jsx3.util.strEmpty(this.columnMtxDoc) ) return;
    this.columnCmpReq.reqParams[this.cmpReqParamArr["SortByXpath"]] = "";

    this.columnCmpReq.reqParams[this.cmpReqParamArr["SortOrder"]] = "ascending";
    this.columnCmpReq.reqParams[this.cmpReqParamArr["ObjectCount"]] = 0;
    this.columnCmpReq.reqParams[this.cmpReqParamArr["StartFrom"]] = 1;
    this.columnCmpReq.reqParams[this.cmpReqParamArr["CountToFetch"]] = 5000;
    // send the request
    this.columnCmpReq.sendRequest(this.CacheValuesArr);    
    
  } // end

  this.setColumnsFromDB = function (respCode, respVal) {
    if (respCode == "SUCCESS" && respVal != null) {
      this.getMtxColumn();
      var colDoc = this.columnMtxDoc;
      var checkNodes = respVal.cloneNode(true);
      var colNodes = respVal.selectNodes("//data/*");
      var noAutoDisplayTags = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/" + this.screenName + "/@noAutoDisplayTags", "false");
      log.info("clsComponent::CmpCustomPaginationList:setColumnsFromDB::noAutoDisplayTags: " + noAutoDisplayTags);
      for (var it = colNodes.iterator(); it.hasNext(); ) {
        var colDataNode = it.next();
        var colDataid = colDataNode.getAttribute("jsxid");
        var colDocRecord = colDoc.selectSingleNode(".//record[@jsxid='" + colDataid + "']");
        if (jsx3.util.strEmpty(colDocRecord)) {
          if (noAutoDisplayTags == "true") {
            colDataNode.setAttribute("jsxChkColumn", "0");
          }
          colDoc.appendChild(colDataNode);
        }
      }
      // if there are tag columns in the doc and the user deleted it  so it wont be in the new get response. so now clean up the old doc
      // go thru the columnMtxDoc for TagValues and see if it is in the response. If not then remove it.
      var objRecordNodes = colDoc.selectNodes("//record");
      var objBefore = null;
      for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
        var objRecordNode = it.next();
        var jsxid = objRecordNode.getAttribute("jsxid");
        // look for only static columns, dynamic columns will have jsxTagValue/jsxMetaData prefix
        var searchResult = jsxid.search(/jsxTagValue/);
        if (searchResult != -1 && jsxid != "jsximg" && jsxid != "jsxMediaFileImg") {
          var recordTagValue = checkNodes.selectSingleNode(".//record[@jsxid='" + jsxid + "']");
          if (jsx3.util.strEmpty(recordTagValue)) {
            colDoc.removeChild(objRecordNode);
            var colName = objRecordNode.getAttribute("jsxColumnName");
            var colObj = this.BlkJSX.getDescendantOfName(jsxid, false, false);
            if (!jsx3.util.strEmpty(colObj)) {
              colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
            }
          }
        }
      }



      //var colDoc2 = this.columnMtxDoc;
      this.columnMtxDoc = colDoc;
      //var colDoc3 = this.columnMtxDoc;
      this.columnDataState = newServerDate();
      this.doCmpEvent(this.ColumnDataUpdatedEvent);
    }
  }        // end
  
  this.getFilterTagDoc = function() {
        // check if the component need to maintain the filterTagDoc
    if( jsx3.util.strEmpty(this.filterTagDoc) ) return;

    // publish the event to show the spinner
    this.doCmpEvent(this.filterTagDataUpdatedEvent, null, "LOADING");

    this.filterTagCmpReq.reqParams[this.cmpReqParamArr["SortByXpath"]] = "";
    this.filterTagCmpReq.reqParams[this.cmpReqParamArr["SortOrder"]] = "ascending";
    this.filterTagCmpReq.reqParams[this.cmpReqParamArr["ObjectCount"]] = 0;
    this.filterTagCmpReq.reqParams[this.cmpReqParamArr["StartFrom"]] = 1;
    this.filterTagCmpReq.reqParams[this.cmpReqParamArr["CountToFetch"]] = 5000;
    // send the request
    this.filterTagCmpReq.sendRequest(this.CacheValuesArr);    
    
  } // end 
    
  this.setFilterTagDoc = function(respCode, respVal) {
    if(respCode == "SUCCESS" && respVal != null) {
      var filterTagDoc = this.filterTagDoc;
      filterTagDoc.removeChildren();
      var filterTagNodes = respVal.selectNodes("//data/tag");
      for(var it = filterTagNodes.iterator(); it.hasNext(); ) {
       var filterTagDataNode = it.next();
       filterTagDoc.appendChild(filterTagDataNode);
      }
      this.filterTagDataState = newServerDate();
      this.doCmpEvent(this.filterTagDataUpdatedEvent);      
    }
  } // end
  
} // end CmpCustomPaginationList


//--------------------------------------- 
//this component is extended from CmpContainer
//SubCompNamesArr is the names of buttons and matrix in order customize, matrix, apply, cancel
cmpCustomizeContainer.prototype = new CmpContainer();
function cmpCustomizeContainer(BlkName, SubCompNamesArr, MtxObjectList, InstanceID, BlkHideEvent, BlkApplyEvent, MtxEventArr, EventArr, SelEvent) {
  // setup inheritence and call base constructor
  cmpCustomizeContainer.BaseClass = cmpCustomizeContainer.prototype;
  cmpCustomizeContainer.prototype.constructor = cmpCustomizeContainer;
  if( BlkName != null ) {
    this.SubCompNamesArr = SubCompNamesArr;
    this.MtxObjectList = MtxObjectList;
    this.InstanceID = InstanceID;
    this.BlkHideEvent = BlkHideEvent;
    this.BlkApplyEvent = BlkApplyEvent;
    this.MtxEventArr = MtxEventArr;
    var ThisClass = this;
    this.cmpMtxCustomize = new Component(this.SubCompNamesArr[0], this.MtxEventArr);
    this.cmpMtxCustomize.repaint = function() {                                    
                                     var strXMLCacheId = this.BlkJSX.getXMLId();
                                     var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                     var cloneColumnMtxDoc  = ThisClass.MtxObjectList.columnMtxDoc.cloneNode(true);
                                      var columnMtxNodes = cloneColumnMtxDoc.selectNodes("//record");
                                      var colDataDoc = new jsx3.xml.CDF.Document.newDocument();
                                      for(var it = columnMtxNodes.iterator(); it.hasNext(); ) {
                                        var colDataNode = it.next();
                                        var metaDataNode = colDataNode.getAttribute("jsxColumn");
                                        if( jsx3.util.strEmpty(metaDataNode) ){
                                          colDataDoc.appendChild(colDataNode);
                                        } 
                                      }                                 
                                     objXML.loadXML(colDataDoc.toString());
                                     // handle scrolbar
                                    var objHeight = this.BlkJSX.getRendered().offsetHeight;
                                    var rowHeight = this.BlkJSX.getRowHeight(jsx3.gui.Matrix.DEFAULT_ROW_HEIGHT);
                                    var headerHeight = this.BlkJSX.getHeaderHeight(jsx3.gui.Matrix.DEFAULT_HEADER_HEIGHT);
                                    var availableHeight = objHeight - headerHeight;
                                    var noOfRows = this.BlkJSX.getSortedIds().length;
                                    var requiredHeight = noOfRows * rowHeight;
                                    this.BlkJSX.setSuppressVScroller(jsx3.Boolean.FALSE);
                                    if( availableHeight > requiredHeight ) {
                                      this.BlkJSX.setSuppressVScroller(jsx3.Boolean.TRUE);
                                    }                                     
                                     
                                     this.postpaint();    
                                   }    
    
    this.cmpBtnCustomizeApply = new CmpButton(this.SubCompNamesArr[1],
                                              null,
                                              this.BlkApplyEvent
                                             );
                                               
    this.cmpBtnCustomizeCancel = new CmpButton(this.SubCompNamesArr[2],
                                               null,
                                               this.BlkHideEvent);
    
    this.cmpBtnCustomizeRefresh = new CmpButton(this.SubCompNamesArr[3],
                                                null,
                                                {action:"FNCCALL", context:this,
                                                 fnc:function() {
                                                       this.MtxObjectList.getColumnsFromDB();
                                                     }
                                                });
                                                
    this.cmpBtnCustomizeClose = new CmpButton(this.SubCompNamesArr[4],
                                               null,
                                               this.BlkHideEvent);
      
    this.cmpMtxMetaData = new CmpBlock(this.SubCompNamesArr[5], this.MtxEventArr);
    this.cmpMtxMetaData.repaint = function () {
                                    var strXMLCacheId = this.BlkJSX.getXMLId();
                                    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                    var cloneColumnMtxDoc = ThisClass.MtxObjectList.columnMtxDoc.cloneNode(true);
                                    //go over the metadata in the master doc and put the metadata in the new doc
                                    var metaDataRecordIter = cloneColumnMtxDoc.selectNodeIterator(".//record[@jsxColumn='MetaData']");
                                    var metaDataDoc = new jsx3.xml.CDF.Document.newDocument();
                                    while (metaDataRecordIter.hasNext()) {
                                      var metaDataRecord = metaDataRecordIter.next();
                                      metaDataDoc.appendChild(metaDataRecord);
                                    }

                                    //go ver the new doc and the metadata matrix
                                    //record not found in the new doc append in the master doc
                                    var recordIter = objXML.selectNodeIterator(".//record");
                                    while (recordIter.hasNext()) {
                                      var record = recordIter.next();
                                      var colName = record.getAttribute("jsxColumnName");
                                      //check in the master doc
                                      var recPresent = metaDataDoc.selectSingleNode(".//record[@jsxColumn='MetaData' and @jsxColumnName='" + colName + "']");
                                      if (jsx3.util.strEmpty(recPresent)) {
                                        colName = colName.split(' ').join('');
                                        var recJSXId = "jsxMetaData" + colName;
                                        record.setAttribute("jsxid", recJSXId);
                                        record.removeAttribute("jsxselected");
                                        metaDataDoc.appendChild(record);
                                      }
                                    }

                                    objXML.loadXML(metaDataDoc.toString());
                                    // handle scrolbar
                                    var objHeight = this.BlkJSX.getRendered().offsetHeight;
                                    var rowHeight = this.BlkJSX.getRowHeight(jsx3.gui.Matrix.DEFAULT_ROW_HEIGHT);
                                    var headerHeight = this.BlkJSX.getHeaderHeight(jsx3.gui.Matrix.DEFAULT_HEADER_HEIGHT);
                                    var availableHeight = objHeight - headerHeight;
                                    var noOfRows = this.BlkJSX.getSortedIds().length;
                                    var requiredHeight = noOfRows * rowHeight;
                                    this.BlkJSX.setSuppressVScroller(jsx3.Boolean.FALSE);
                                    if (availableHeight > requiredHeight) {
                                      this.BlkJSX.setSuppressVScroller(jsx3.Boolean.TRUE);
                                    }

                                    this.postpaint();
                                  }  
                                   
    this.cmpBtnAddMetaData = new CmpButton(this.SubCompNamesArr[6],
                                              null,
                                              { action: "FNCCALL", context: this,
                                                fnc: function() {
                                                  var o = new Object;
                                                  o.jsxid = jsx3.xml.CDF.getKey();
                                                  o.jsxChkColumn = "1";
                                                  o.jsxColumnSortable = "1";
                                                  o.jsxColumnName = "";
                                                  o.jsxColumn = "MetaData";
                                                  var objMatrix = this.cmpBtnAddMetaData.MetaDataMtxBlkJSX;
                                                  var objMatrixSortedIds = objMatrix.getSortedIds();
                                                  if (objMatrixSortedIds.length == 0) {
                                                    objMatrix.insertRecord(o, true);
                                                  }
                                                  else {
                                                    objMatrix.insertRecordBefore(o, objMatrixSortedIds[0], true);
                                                  }
                                                }
                                              });
                                              
    this.cmpBtnAddMetaData.MetaDataMtxBlkJSX = this.cmpMtxMetaData.BlkJSX;

    this.cmpBtnDeleteMetaData = new CmpButton(this.SubCompNamesArr[7],
                                              null,
                                              { action: "FNCCALL", context: this,
                                                fnc: function() {
                                                  var objMatrix = this.cmpBtnDeleteMetaData.MetaDataMtxBlkJSX;
                                                  var selRecordID = objMatrix.getValue();
                                                  var selRecordNode = objMatrix.getRecordNode(selRecordID);
                                                  if ((selRecordID != null) && (selRecordNode != null)) {
                                                    var dirtyBitVal = selRecordNode.getAttribute("jsxDirtyBit");
                                                    objMatrix.deleteRecord(selRecordID, true);
                                                  } else {
                                                    alert("Please select MetaData to delete");
                                                    return false;
                                                  } 
                                                }
                                              });
                                              
    this.cmpBtnDeleteMetaData.MetaDataMtxBlkJSX = this.cmpMtxMetaData.BlkJSX;
    
    this.cmpCustomizeLayout = new Component(this.SubCompNamesArr[8]);
    
    this.cmpBlkMetaData = new Component(this.SubCompNamesArr[9]);
    this.ShowHideMetaData = this.SubCompNamesArr[10];
    
    this.cmpCustomizeLayout.BlkJSX.setRows("26,30,*,1,26,10,30", true);
    if(this.ShowHideMetaData == "ShowMetaData"){
      this.cmpCustomizeLayout.BlkJSX.setRows("26,30,*,*,26,10,30", true);
    }
    
  
    var SubCmpArr = [{cmp:this.cmpBtnCustomize},
                     {cmp:this.cmpMtxCustomize},
                     {cmp:this.cmpBtnCustomizeApply},
                     {cmp:this.cmpBtnCustomizeCancel},
                     {cmp:this.cmpBtnCustomizeRefresh},
                     {cmp:this.cmpBtnCustomizeClose},
                     { cmp: this.cmpMtxMetaData},
                     { cmp: this.cmpBtnAddMetaData},
                     { cmp: this.cmpBtnDeleteMetaData}
                    ];

    CmpContainer.prototype.constructor.call(this, BlkName, SubCmpArr, EventArr);
  }

  this.init = function() {
    cmpCustomizeContainer.BaseClass.init.call(this);
  }

  this.show = function(EventVar, EventVal) {
    cmpCustomizeContainer.BaseClass.show.call(this, EventVar, EventVal);
    
    if( EventVal == null ) return;
                                                
  }
  

 
} // end cmpCustomizeContainer



//--------------------------------------- 
//this component is extended from CmpContainer
//SubCompNamesArr is the names of buttons and matrix in order customize, matrix, apply, cancel
cmpFilterContainer.prototype = new CmpContainer();
function cmpFilterContainer(BlkName, SubCompNamesArr, MtxObjectList, InstanceID, BlkHideEvent, BlkApplyEvent, BlkEventArr, EventArr, SelEvent) {
  // setup inheritence and call base constructor
  cmpFilterContainer.BaseClass = cmpFilterContainer.prototype;
  cmpFilterContainer.prototype.constructor = cmpFilterContainer;
  if( BlkName != null ) {
    var ThisClass = this;
    var SubCompNamesArr = SubCompNamesArr;
    this.MtxObjectList = MtxObjectList;
    this.InstanceID = InstanceID;
    this.BlkHideEvent = BlkHideEvent;
    this.BlkApplyEvent = BlkApplyEvent;
    this.BlkEventArr = BlkEventArr;
      
                                        
    this.cmpTagBlkJSX = getJsxByName(SubCompNamesArr[0]);
    
    this.cmpTagList = new Component(SubCompNamesArr[1], ThisClass.BlkEventArr);
    this.cmpTagList.MtxObjectListFilterTagDoc = this.MtxObjectList.filterTagDoc;
    this.cmpTagList.repaint = function (EventVar, EventVal) {
                                if (EventVal == "LOADING") {
                                  // Show the refreshspinner
                                  ThisClass.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                  // Hide the refresh
                                  ThisClass.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  if (ThisClass.cmpBtnRefreshNotify.BlkJSX) {
                                    ThisClass.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  }
                                  return;
                                }
                                else if (EventVal == "DIRTY") {
                                  // Show the refreshNotify
                                  if (ThisClass.cmpBtnRefreshNotify.BlkJSX) {
                                    ThisClass.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                  }
                                  // Hide the refresh
                                  ThisClass.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  ThisClass.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  return;
                                }
                                else {
                                  // hide the refreshspinner
                                  ThisClass.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  // show the refresh
                                  ThisClass.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                  if (ThisClass.cmpBtnRefreshNotify.BlkJSX) {
                                    ThisClass.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  }
                                }

                                //delete the previous tag blocks 
                                var cmpBlkTagListChildren = ThisClass.cmpTagList.BlkJSX.getChildren().clone();
                                var arrLength = cmpBlkTagListChildren.length;
                                for (var i = 0; i < arrLength; i++) {
                                  var cmpBlkTagChild = cmpBlkTagListChildren[i];
                                  var cmpBlkTagChildName = cmpBlkTagChild.getName();
                                  if (cmpBlkTagChildName.match(/blkTagOrg_/)) {
                                    ThisClass.cmpTagList.BlkJSX.removeChild(cmpBlkTagChild);
                                  }
                                }

                                var tagNodes = this.MtxObjectListFilterTagDoc.selectNodes("//data/tag");
                                for (var it = tagNodes.iterator(); it.hasNext(); ) {
                                  var filterTagDataNode = it.next();
                                  var tagName = filterTagDataNode.selectSingleNode(".//tagName").getValue();
                                  var tagValueDoc = filterTagDataNode.selectSingleNode(".//tagValue/data");

                                  var newTagBlkJSX = ThisClass.cmpTagBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
                                  newTagBlkJSX.setName("blkTagOrg_" + tagName);
                                  ThisClass.cmpTagList.BlkJSX.adoptChild(newTagBlkJSX, false);
                                  // show the block
                                  newTagBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);

                                  var lblTagNameBlkJSX = getJsxByName([newTagBlkJSX, "lblTagName"]);
                                  lblTagNameBlkJSX.setText(tagName, false);

                                  var mtxTagValueBlkJSX = getJsxByName([newTagBlkJSX, "mtxTagValues"]);
                                  var strXMLCacheId = mtxTagValueBlkJSX.getXMLId();
                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                  if (!jsx3.util.strEmpty(objXML)) {
                                    delete objXML;
                                  }
                                  var newMtxTagValueDoc = new jsx3.xml.CDF.Document.newDocument();
                                  PNameSpace.getCache().setDocument(strXMLCacheId, newMtxTagValueDoc);
                                  var objXML1 = PNameSpace.getCache().getDocument(strXMLCacheId);
                                  objXML1.loadXML(tagValueDoc.toString());
                                  mtxTagValueBlkJSX.repaintData();
                                }
                                this.postpaint();

                              }

    this.cmpTagList.getTextSubStr = function(){
                                         var txtSubStr = ThisClass.cmpTxtNameSearch.BlkJSX.getValue();
                                         return txtSubStr;
                                       }     
                                                                                                       
    this.cmpTagList.getNameDescXPath = function(){
                                         var namedescXPath = "";
                                         var nameXPath = "";
                                         var descXPath = "";
                                         var metadataXPath = "";
                                         
                                         var textNameSearch = ThisClass.cmpTxtNameSearch.BlkJSX.getValue();
                                         var chkName = ThisClass.cmpChkName.BlkJSX.getChecked();
                                         // if should be removed oce we move to the getprofile api call. since we are using the read query so this hack has to be in.
                                         if (ThisClass.MtxObjectList.screenName == 'ConnectionProfile' && !chkName && !jsx3.util.strEmpty(textNameSearch) ) {
                                           chkName = 1;
                                         }
                                         if(chkName) {
                                           nameXPath = "( (matches( upper-case(//Title) , upper-case(\"" + textNameSearch + "\") )) or (matches( upper-case(//Name) , upper-case(\"" + textNameSearch + "\") )) )";
                                         }
                                         var chkDesc = ThisClass.cmpChkDescription.BlkJSX.getChecked();
                                         // if should be removed oce we move to the getprofile api call. since we are using the read query so this hack has to be in.
                                         if (ThisClass.MtxObjectList.screenName == 'ConnectionProfile' && !chkDesc && !jsx3.util.strEmpty(textNameSearch) ) {
                                           chkDesc = 1;
                                         }
                                         if(chkDesc) {
                                           descXPath += "(matches( upper-case(//Description) , upper-case(\"" + textNameSearch + "\") ))";  
                                         }
                                         var chkMetadata = ThisClass.cmpChkMetaData.BlkJSX.getChecked();
                                         if(chkMetadata){
                                           metadataXPath += "(matches( //MetaData/Tag , \"" + textNameSearch + "\" ))";
                                         }

                                         if( chkName && chkDesc && chkMetadata ) {
                                           namedescXPath += "("+ nameXPath + " or "+ descXPath + " or "+ metadataXPath + ")";
                                         }
                                         else if( chkName && chkDesc && !chkMetadata ){
                                           namedescXPath += "("+ nameXPath + " or "+ descXPath + ")";
                                         }
                                         else if( chkName && !chkDesc && !chkMetadata ){
                                           namedescXPath += "("+ nameXPath + ")";
                                         }
                                         else if( !chkName && chkDesc && chkMetadata ){
                                           namedescXPath += "("+ descXPath + " or "+ metadataXPath + ")";
                                         }
                                         else if( !chkName && !chkDesc && chkMetadata ){
                                           namedescXPath += "("+ metadataXPath + ")";
                                         }
                                         else if( !chkName && chkDesc && !chkMetadata ){
                                           namedescXPath += "("+ descXPath + ")";
                                         }
                                                                                  
                                         return namedescXPath;   
                                      }
                                      
    this.cmpTagList.getMtxTagValuesXPath = function(){
                                             var tagFilterValues = "";
                                             var groupTagArr = new Object;
                                             var objParent = ThisClass.cmpTagList.BlkJSX.getParent();
                                             // find all descendant of type matrixes
                                             var mtxArr = objParent.getDescendantsOfType(jsx3.gui.Matrix, false);
                                             for(var i=0; i < mtxArr.length; i++ ) {
                                               var mtxBlkJSX = mtxArr[i];
                                               var mtxName = mtxBlkJSX.getName();
                                               if(mtxName != "mtxTagValues") continue; 
                                                
                                               var selectedNodes = mtxBlkJSX.getSelectedNodes();
                                               var iter=0;
                                               var tagName = null;
                                               var tagValue = "";
                                               for (var it = selectedNodes.iterator(); it.hasNext(); ) {
                                                 var tagRecordNode = it.next();
                                                 tagName = tagRecordNode.getAttribute("jsxTagName");
                                                 if(iter == 0) {
                                                   tagValue += "'" + tagRecordNode.getAttribute("jsxTagValue") + "'";
                                                 }
                                                 else {
                                                   tagValue += ", '" + tagRecordNode.getAttribute("jsxTagValue") + "'";
                                                 }
                                                 iter++;
                                               }
                                               if(iter > 0) {
                                                 if(groupTagArr[tagName] == null ) groupTagArr[tagName] = new Object;
                                                 groupTagArr[tagName] = {"tagValue":tagValue};
                                               }
                                             }
                                             var tagiter=0;
                                             var xPath = "";
                                             for( var tagName in  groupTagArr ) {
                                               var tagValue = groupTagArr[tagName].tagValue;
                                               if( tagiter == 0 ) {
                                                 xPath = "(//Tag[@Name='"+ tagName +"' and @Value=("+tagValue+")])";
                                               }
                                               else {
                                                 xPath += " and (//Tag[@Name='"+ tagName +"' and @Value=("+tagValue+")])";
                                               }
                                               tagiter++;
                                             }
                                             if(tagiter > 0 ) {
                                               tagFilterValues = "(" + xPath + ")";
                                             }                                        

                                             return tagFilterValues;                                        
                                           }                       
                                                        
    this.cmpTagList.getWhereXPath = function(){
                                     var whereXPath = "";

                                     var namedescXPath = ThisClass.cmpTagList.getNameDescXPath();
                                     var groupTagXPath = ThisClass.cmpTagList.getMtxTagValuesXPath();
                                     
                                     if( (!jsx3.util.strEmpty(namedescXPath)) &&
                                         (!jsx3.util.strEmpty(groupTagXPath)) ){
                                       whereXPath = "[" + namedescXPath + " and " + groupTagXPath + "]";
                                     }
                                     else if( (jsx3.util.strEmpty(namedescXPath)) &&
                                              (!jsx3.util.strEmpty(groupTagXPath)) ){
                                       whereXPath = "[" + groupTagXPath + "]";                                                           
                                     }
                                     else if( (!jsx3.util.strEmpty(namedescXPath)) &&
                                              (jsx3.util.strEmpty(groupTagXPath)) ){
                                       whereXPath = "[" + namedescXPath + "]";         
                                    }
                                      return whereXPath;
                                    }
                                    
    this.cmpTagList.getAPITagExpr = function(){
                                             var tagFilterValues = "";
                                             var groupTagArr = new Object;
                                             var objParent = ThisClass.cmpTagList.BlkJSX.getParent();
                                             // find all descendant of type matrixes
                                             var mtxArr = objParent.getDescendantsOfType(jsx3.gui.Matrix, false);
                                             for(var i=0; i < mtxArr.length; i++ ) {
                                               var mtxBlkJSX = mtxArr[i];
                                               var mtxName = mtxBlkJSX.getName();
                                               if(mtxName != "mtxTagValues") continue; 
                                                
                                               var selectedNodes = mtxBlkJSX.getSelectedNodes();
                                               var iter=0;
                                               var tagName = null;
                                               var tagValue = "";
                                               for (var it = selectedNodes.iterator(); it.hasNext(); ) {
                                                 var tagRecordNode = it.next();
                                                 tagName = tagRecordNode.getAttribute("jsxTagName");
                                                 if(iter == 0) {
                                                   tagValue += "'" + tagRecordNode.getAttribute("jsxTagValue") + "'";
                                                 }
                                                 else {
                                                   tagValue += ", '" + tagRecordNode.getAttribute("jsxTagValue") + "'";
                                                 }
                                                 iter++;
                                               }
                                               if(iter > 0) {
                                                 if(groupTagArr[tagName] == null ) groupTagArr[tagName] = new Object;
                                                 groupTagArr[tagName] = {"tagValue":tagValue};
                                               }
                                             }
                                             var tagiter=0;
                                             var xPath = "";
                                             for( var tagName in  groupTagArr ) {
                                               var tagValue = groupTagArr[tagName].tagValue;
                                               if( tagiter == 0 ) {
                                                 xPath = "Name='" + tagName +"' and Value=" + tagValue + "";
                                               }
                                               else {
                                                 xPath += " , Name='" + tagName + "' and Value=" + tagValue + "";
                                               }
                                               tagiter++;
                                             }
                                             if(tagiter > 0 ) {
                                               tagFilterValues = xPath;
                                             }                                        

                                             return tagFilterValues;  
                                    }

    this.cmpTagList.getChkBookmark = function () {
                                      var chkBookmark = ThisClass.cmpChkBookMark.BlkJSX.getChecked();
                                      return chkBookmark;
                                    }                                    
    this.cmpTagList.getChkName = function(){
                                          var chkName = ThisClass.cmpChkName.BlkJSX.getChecked();
                                          return chkName;
                                       }
    this.cmpTagList.getChkDescription = function(){
                                        var chkDescription = ThisClass.cmpChkDescription.BlkJSX.getChecked();
                                        return chkDescription;
                                       }
    this.cmpTagList.getChkMetaData = function(){
                                         var chkBookmark = ThisClass.cmpChkMetaData.BlkJSX.getChecked();                 
                                         return chkBookmark;
                                       }
                                                                                                                                                            
    this.cmpTxtNameSearch = new CmpTxtBox(SubCompNamesArr[2],null,null,null);
    
    this.cmpChkName = new CmpCheckBox(SubCompNamesArr[3],null,
                                      {action:"FNCCALL", 
                                        context:this, 
                                        fnc:function() {
                                        }
                                      }    
                                     );
    
    this.cmpChkDescription = new CmpCheckBox(SubCompNamesArr[4],null,
                                              {action:"FNCCALL", 
                                                context:this, 
                                                fnc:function() {
                                                }
                                              }                                               
                                            );
                                         
    this.cmpBtnFilterCancel = new CmpButton(SubCompNamesArr[5],
                                               null,
                                               this.BlkHideEvent);                                                  
                                               
    this.cmpBtnFilterApply = new CmpButton(SubCompNamesArr[6],
                                              null,
                                              this.BlkApplyEvent
                                             );

    this.cmpBtnClearAll = new CmpButton(SubCompNamesArr[7],
                                        null,
                                        { action: "FNCCALL", context: this,
                                            fnc: function () {
                                                ThisClass.cmpBtnClearAll.clearAllSelection();
                                            }
                                        });

    this.cmpBtnClearAll.clearAllSelection = function () {
                                                //cleat text box
                                                ThisClass.cmpTxtNameSearch.BlkJSX.setValue("", true);
                                                // check Name
                                                ThisClass.cmpChkName.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
                                                // check description
                                                ThisClass.cmpChkDescription.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
                                                // check Bookmark
                                                ThisClass.cmpChkBookMark.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
                                                // check metadata
                                                ThisClass.cmpChkMetaData.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);

                                                // deselectAllrecords in the Matrixes
                                                var objParent = ThisClass.cmpTagList.BlkJSX.getParent();
                                                // find all descendant of type matrixes
                                                var mtxArr = objParent.getDescendantsOfType(jsx3.gui.Matrix, false);
                                                for (var i = 0; i < mtxArr.length; i++) {
                                                    var mtxBlkJSX = mtxArr[i];
                                                    var mtxName = mtxBlkJSX.getName();
                                                    if (mtxName != "mtxTagValues") continue;

                                                    if (mtxBlkJSX.deselectAllRecords != null) {
                                                        mtxBlkJSX.deselectAllRecords();
                                                    }
                                                }                                            
                                            }
                                                                                                       
    this.cmpBtnClose = new CmpButton(SubCompNamesArr[8],
                                     null,
                                     this.BlkHideEvent);

    this.cmpBtnRefresh = new Component();
    if (SubCompNamesArr[9] != null) {
      this.cmpBtnRefresh = new CmpImageButton(SubCompNamesArr[9],
                                              null,
                                              { action: "FNCCALL", context: this,
                                                fnc: function () {
                                                  // repaint the matrix                                                
                                                  this.MtxObjectList.getFilterTagDoc();
                                                }
                                              });
    }

    this.cmpBtnRefreshSpinner = new Component();
    if (SubCompNamesArr[10] != null) {
      this.cmpBtnRefreshSpinner = new CmpImageButton(SubCompNamesArr[10],
                                                      null,
                                                      { action: "FNCCALL", context: this,
                                                        fnc: function () {
                                                          // cancel the request
                                                          this.MtxObjectList.filterTagCmpReq.endRequest();
                                                          // hide the refreshspinner
                                                          this.cmpBtnRefreshSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                          // show the refresh
                                                          this.cmpBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                                          if (this.cmpBtnRefreshNotify.BlkJSX) {
                                                            this.cmpBtnRefreshNotify.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                          }
                                                        }
                                                      });
    }

    this.cmpBtnRefreshNotify = new Component();
    if (SubCompNamesArr[11] != null) {
      this.cmpBtnRefreshNotify = new CmpImageButton(SubCompNamesArr[11],
                                              null,
                                              { action: "FNCCALL", context: this,
                                                fnc: function () {
                                                  // repaint the matrix                                                
                                                  this.MtxObjectList.getFilterTagDoc();
                                                }
                                              });
    }
    this.cmpChkBookMark = new CmpCheckBox(SubCompNamesArr[12],null,
                                      {action:"FNCCALL", 
                                        context:this, 
                                        fnc:function() {
                                        }
                                      }    
                                     );

    this.cmpChkMetaData = new CmpCheckBox(SubCompNamesArr[13],null,
                                      {action:"FNCCALL", 
                                        context:this, 
                                        fnc:function() {
                                        }
                                      }    
                                     );
                                                                          
    var SubCmpArr = [{ cmp: this.cmpTagList },
                     { cmp: this.cmpTag },
                     { cmp: this.cmpTxtNameSearch },
                     { cmp: this.cmpChkName },
                     { cmp: this.cmpChkDescription, sel:"this.MtxObjectList.screenName != 'User'" },
                     { cmp: this.cmpBtnFilterCancel },
                     { cmp: this.cmpBtnFilterApply },
                     { cmp: this.cmpBtnClearAll },
                     { cmp: this.cmpBtnClose },
                     { cmp: this.cmpBtnRefresh },
                     { cmp: this.cmpBtnRefreshSpinner },
                     { cmp: this.cmpBtnRefreshNotify },
                     { cmp: this.cmpChkBookMark, sel:"this.MtxObjectList.screenName == 'MediaStreamFile'"},
                     { cmp: this.cmpChkMetaData, sel:"this.MtxObjectList.screenName == 'MediaStreamFile'"}
                    ];

    CmpContainer.prototype.constructor.call(this, BlkName, SubCmpArr, EventArr);
  }

  this.init = function() {
    cmpFilterContainer.BaseClass.init.call(this);
  }

  this.show = function(EventVar, EventVal) {
    cmpFilterContainer.BaseClass.show.call(this, EventVar, EventVal);
    if (EventVal == null) return;                                        
  }

  this.markDirty = function () {
    this.MtxObjectList.doCmpEvent(this.MtxObjectList.filterTagDataUpdatedEvent, null, "DIRTY");
  }

  this.handleNotification = function (objEvent) {
    if (objEvent == null) return;
    var eventType = objEvent.type;
    if (eventType == "add") {
      this.markDirty();
      return;
    }
    else if (eventType == "replace") {
      this.markDirty();
      return;
    }
    else if (eventType == "delete") {
      this.markDirty();
      return;
    }

  }
 
} // end cmpFilterContainer

//--------------------------------------- 
//this component is extended from CmpComponent
CmpMenuBlock.prototype = new Component();
function CmpMenuBlock(BlkName, MenuLaunchBtnBlkName, MenuOKBtnBlkName, MenuCancelBtnBlkName, EventArr,CancelFnc, SelEvent) {
  // setup inheritence and call base constructor
  CmpMenuBlock.BaseClass = CmpMenuBlock.prototype;
  CmpMenuBlock.prototype.constructor = CmpMenuBlock;
  if (BlkName != null) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);

    this.cmpBtnOk = new CmpButton([this.BlkJSX, MenuOKBtnBlkName],
                                  null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function() {
                                      this.onSelect();
                                      this.hide();
                                    }
                                  }
                                 );
   this.cmpBtnCancel = new CmpButton([this.BlkJSX, MenuCancelBtnBlkName],
                                  null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function() {
                                      CancelFnc.call(this);
                                      this.hide();
                                    }
                                  }
                                 );
    // view mode toggle button is not belone to blkName, but to winMediaPlayer/BlkName[0]
    this.cmpBtnLaunch = new CmpButton(MenuLaunchBtnBlkName,
                                  null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function() {
                                      this.show();
                                    }
                                  }
                                 );
    }
    this.init = function() {
      this.cmpBtnOk.init();
      this.cmpBtnCancel.init();
      this.cmpBtnLaunch.init();
      CmpMenuBlock.BaseClass.init.call(this);
    }
} 

//------------------Canvas Edit--------
//this component is extended from CmpContainer
//SubCompNamesArr is the names of canvas display, window list and name of txt boxes in order of id, left, top, width and height.
CmpCanvasEditorContainer.prototype = new CmpContainer();
function CmpCanvasEditorContainer(BlkName, SubCompNamesArr,EventArr, SelEvent,windowBlkJSX, editCanvasType) {
  // setup inheritence and call base constructor
  CmpCanvasEditorContainer.BaseClass = CmpCanvasEditorContainer.prototype;
  CmpCanvasEditorContainer.prototype.constructor = CmpCanvasEditorContainer;
  var ThisClass = this;
  this.editCanvasType = editCanvasType;
  var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS-Common/xsl/Window.xsl","WindowPanel","xsl");
  var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
  this.xslDoc = xslDoc;
  //set the editCanvasType in the XML thru the xsl
  this.xslDoc.setParam("CanvasEditTypeArgument", this.editCanvasType);
  
 
  var windowString = '<Window id="1" enable="" content="" jsxselected="1" ><ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId="" vres="" hres="" srcKbmCtrl=""></ConnectionContent><ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent><Properties zIndex="100" border="" vpos="350" hpos="550" vres="400" hres="500" aspect="" transparency="0" layoutId="" layoutPos="" visibility="true" maximize="false" fps="15" scale="fitWindow" mute="false" volume="50%" remoteKbmCtrl="false" scrollHPos="0" scrollVPos="0"></Properties><SubCanvas></SubCanvas></Window>';
  var windowDoc = new jsx3.xml.Document();
  var windowNode = windowDoc.loadXML(windowString.toString());
  this.windowNode = windowNode;
 
  var tCanvasEditor = "CanvasEditor_"+jsx3.xml.CDF.getKey();
  var tCanvasEditorDoc = null;
  tCanvasEditorDoc = topic_Create(tCanvasEditor);
  var eventCanvasEditor = new EventListner(tCanvasEditor); 
  topic_Set(tCanvasEditor,"CurrentWindow","NONE");
  topic_Set(tCanvasEditor,"CurrentCanvas","NONE");
  topic_Set(tCanvasEditor,"WindowMode","CANVAS");
  topic_Set(tCanvasEditor,"PropertiesOrScroll","PROPERTIES");
  log.info("INIT CmpCanvasEditorContainer tCanvasEditor::"+tCanvasEditor + "\n");
  
  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    var SubCompNamesArr = SubCompNamesArr;
  
    this.cmpCanvas = new Component();
    if( SubCompNamesArr[0] != null ) {
      this.cmpCanvas = new CmpCanvas(SubCompNamesArr[0],
                                                   null,
                                                   {action: "TOPICEVENT", topic: tCanvasEditor, eventvar: "SelectWindow", eventval: "id" });
    }
    this.masterCDF = new Component();
    if( SubCompNamesArr[1] != null ) {
     this.masterCDF = new Component(SubCompNamesArr[1],null,null,null);
    }

    this.cmpWindowList = new Component();
    if( SubCompNamesArr[2] != null ) {
     this.cmpWindowList = new CmpCanvasEditorWindowPanel(SubCompNamesArr[2],
                                                   null,
                                                   {action: "TOPICEVENT", topic: tCanvasEditor, eventvar: "SelectListWindow", eventval: "jsxid" });
    }

    this.cmpTxtWindowId = new Component();
    if( SubCompNamesArr[3] != null ) {
      this.cmpTxtWindowId = new CmpTxtBoxWithBlur(SubCompNamesArr[3], null, null,  
                                                  {action:"FNCCALL", 
                                                     context:this, 
                                                     fnc:function() {
                                                          var prevValue = this.cmpTxtWindowId.BlkJSX.jsxvalue;
                                                          var winId =  jsx3.util.strTrim(this.cmpTxtWindowId.BlkJSX.getValue()); 
                                                          if( jsx3.util.strEmpty(winId) ){
                                                            if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                              windowAlert(windowBlkJSX,"Window ID cannot be empty/blank.");
                                                            }
                                                            else{
                                                              alert("Window ID cannot be empty/blank.");
                                                            } 
                                                            this.cmpTxtWindowId.BlkJSX.setValue(prevValue);
                                                            this.cmpTxtWindowId.BlkJSX.repaint();  
                                                            this.cmpTxtWindowId.BlkJSX.focus();                                               
                                                            return;
                                                          }
                                                          var selectedNode =  this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']");;
                                                          var selectedId = selectedNode.getAttribute("jsxid");
                                                          //this is to avoid already exist id alert.
                                                          if( selectedId == winId ){
                                                            return;
                                                          }
                                                          
                                                          var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                          var existingWindowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+winId+"']");
                                                          if(!jsx3.util.strEmpty(existingWindowNode)){
                                                            if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                              windowAlert(windowBlkJSX,"Window ID already exists.Please enter a different ID");
                                                            }
                                                            else{
                                                              alert("Window ID already exists.Please enter a different ID");
                                                            }
                                                            this.cmpTxtWindowId.BlkJSX.setValue(selectedId);
                                                            this.cmpTxtWindowId.BlkJSX.repaint();
                                                            return;
                                                          }
                                                          if(!jsx3.util.strEmpty(selectedId)){
                                                            selectedNode.setAttribute('jsxtext',winId);
                                                            selectedNode.setAttribute('jsxid',winId);
                                                            var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedId+"']");
                                                            windowNode.setAttribute("id",winId);
                                                            topic_Set(tCanvasEditor,"CurrentWindow",winId);
                                                            this.cmpWindowList.BlkJSX.repaint();
                                                            this.cmpCanvas.repaint();
                                                          }
                                                         }
                                                      });   
    }
    
   this.cmpTxtWindowLeft = new Component();
    if( SubCompNamesArr[4] != null ) {
      this.cmpTxtWindowLeft = new CmpTxtBoxWithBlur(SubCompNamesArr[4], null, null, 
                                                    { action: "FNCCALL",
                                                      context: this,
                                                      fnc: function () {
                                                        var winleft = this.cmpTxtWindowLeft.BlkJSX.getValue();
                                                        var canvasWidth = this.cmpTxtDisplayWidth.BlkJSX.getValue();
                                                        if (this.isNumberOrPercentage(winleft, canvasWidth) == false) {
                                                          this.cmpTxtWindowLeft.BlkJSX.setValue("0");
                                                          this.cmpTxtWindowLeft.BlkJSX.repaint();
                                                          this.cmpTxtWindowLeft.BlkJSX.focus();
                                                          return;
                                                        }
                                                        if (jsx3.util.strEmpty(winleft)) {
                                                          winleft = 0;
                                                          this.cmpTxtWindowLeft.BlkJSX.setValue(winleft);
                                                          this.cmpTxtWindowLeft.BlkJSX.repaint();
                                                        }
                                                        var selectedNode = this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']"); ;
                                                        var selectedId = selectedNode.getAttribute("jsxid");
                                                        if (!jsx3.util.strEmpty(selectedId)) {
                                                          selectedNode.setAttribute('hpos', winleft);
                                                          var masterXML = this.cmpCanvas.BlkJSX.getXML();
                                                          var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='" + selectedId + "']");
                                                          var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                          propertiesNode.setAttribute("hpos", winleft);
                                                          this.cmpCanvas.repaint();
                                                        }
                                                      }
                                                    });   
    }
    
   this.cmpTxtWindowTop = new Component();
    if( SubCompNamesArr[5] != null ) {
      this.cmpTxtWindowTop = new CmpTxtBoxWithBlur(SubCompNamesArr[5], null, null,  
                                                  {action:"FNCCALL", 
                                                   context:this, 
                                                   fnc:function() {
                                                          var wintop = this.cmpTxtWindowTop.BlkJSX.getValue();
                                                          var canvasHeight = this.cmpTxtDisplayHeight.BlkJSX.getValue();
                                                          if (this.isNumberOrPercentage(wintop, canvasHeight) == false) {
                                                            this.cmpTxtWindowTop.BlkJSX.setValue("0");
                                                            this.cmpTxtWindowTop.BlkJSX.repaint();
                                                            this.cmpTxtWindowTop.BlkJSX.focus();
                                                            return;
                                                          }
                                                          if (jsx3.util.strEmpty(wintop)) {
                                                            wintop = 0;
                                                            this.cmpTxtWindowTop.BlkJSX.setValue(wintop);
                                                            this.cmpTxtWindowTop.BlkJSX.repaint();
                                                          } 
                                                          var selectedNode =  this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']");;
                                                          var selectedId = selectedNode.getAttribute("jsxid");
                                                          if(!jsx3.util.strEmpty(selectedId)){
                                                            selectedNode.setAttribute('vpos',wintop);
                                                            var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                            var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedId+"']");
                                                            var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                            propertiesNode.setAttribute("vpos",wintop);
                                                            this.cmpCanvas.repaint();
                                                          }
                                                       }
                                                    });   
    }
    
   this.cmpTxtWindowWidth = new Component();
    if( SubCompNamesArr[6] != null ) {
      this.cmpTxtWindowWidth = new CmpTxtBoxWithBlur(SubCompNamesArr[6], null, null, 
                                                    { action: "FNCCALL",
                                                      context: this,
                                                      fnc: function () {
                                                        var winwidth = this.cmpTxtWindowWidth.BlkJSX.getValue();
                                                        var canvasWidth = this.cmpTxtDisplayWidth.BlkJSX.getValue();
                                                        if (this.isNumberOrPercentage(winwidth, canvasWidth) == false) {
                                                          this.cmpTxtWindowWidth.BlkJSX.setValue(canvasWidth);
                                                          this.cmpTxtWindowWidth.BlkJSX.repaint(); 
                                                          this.cmpTxtWindowWidth.BlkJSX.focus();
                                                          return;
                                                        }
                                                        if (jsx3.util.strEmpty(winwidth)) {
                                                          winwidth = 0;
                                                          this.cmpTxtWindowWidth.BlkJSX.setValue(winwidth);
                                                          this.cmpTxtWindowWidth.BlkJSX.repaint();
                                                        }
                                                        var selectedNode = this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']"); ;
                                                        var selectedId = selectedNode.getAttribute("jsxid");
                                                        if (!jsx3.util.strEmpty(selectedId)) {
                                                          selectedNode.setAttribute('hres', winwidth);
                                                          var masterXML = this.cmpCanvas.BlkJSX.getXML();
                                                          var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='" + selectedId + "']");
                                                          var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                          propertiesNode.setAttribute("hres", winwidth);
                                                          this.cmpCanvas.repaint();
                                                        }
                                                      }
                                                    });      
    }
    
   this.cmpTxtWindowHeight = new Component();
    if( SubCompNamesArr[7] != null ) {
      this.cmpTxtWindowHeight = new CmpTxtBoxWithBlur(SubCompNamesArr[7], null, null, 
                                                    {action:"FNCCALL", 
                                                     context:this, 
                                                     fnc:function() {
                                                         var winheight = this.cmpTxtWindowHeight.BlkJSX.getValue();
                                                         var canvasHeight = this.cmpTxtDisplayHeight.BlkJSX.getValue();
                                                         if (this.isNumberOrPercentage(winheight, canvasHeight) == false) {
                                                            this.cmpTxtWindowHeight.BlkJSX.setValue(canvasHeight);
                                                            this.cmpTxtWindowHeight.BlkJSX.repaint();
                                                            this.cmpTxtWindowHeight.BlkJSX.focus();
                                                            return;
                                                          }
                                                          if (jsx3.util.strEmpty(winheight)) {
                                                            winheight = 0;
                                                            this.cmpTxtWindowHeight.BlkJSX.setValue(winheight);
                                                            this.cmpTxtWindowHeight.BlkJSX.repaint();
                                                          }
                                                          var selectedNode =  this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']");;
                                                          var selectedId = selectedNode.getAttribute("jsxid");
                                                          if(!jsx3.util.strEmpty(selectedId)){
                                                            selectedNode.setAttribute('vres',winheight);
                                                            var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                            var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedId+"']");
                                                            var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                            propertiesNode.setAttribute("vres",winheight);
                                                            this.cmpCanvas.repaint();
                                                          }
                                                         }
                                                      });      
    }
    
  this.cmpTxtWindowTransparency = new Component();
    if( SubCompNamesArr[8] != null ) {
      this.cmpTxtWindowTransparency = new CmpTxtBoxWithBlur(SubCompNamesArr[8], null, null,  
                                                            {action:"FNCCALL", 
                                                             context:this, 
                                                             fnc:function() {
                                                                  var winTransparency =  jsx3.util.strTrim(this.cmpTxtWindowTransparency.BlkJSX.getValue()); 
                                                                  if(isNaN(winTransparency)){
                                                                    if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                                       windowAlert(windowBlkJSX,"Please enter numeric opacity value");
                                                                    }
                                                                    else{
                                                                      alert("Please enter numeric opacity value");
                                                                    }
                                                                    this.cmpTxtWindowTransparency.BlkJSX.setValue("");
                                                                    this.cmpTxtWindowTransparency.BlkJSX.repaint();
                                                                    return;
                                                                  }
                                                                  var selectedNode =  this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']");;
                                                                  var selectedId = selectedNode.getAttribute("jsxid");
                                                                  if(!jsx3.util.strEmpty(selectedId)){                      
                                                                    selectedNode.setAttribute('transparency',winTransparency);
                                                                    var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                                    var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedId+"']");
                                                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                                    propertiesNode.setAttribute("transparency",winTransparency);
                                                                    this.cmpCanvas.repaint();
                                                                  }
                                                                 }
                                                              });      
    }    
    
  this.cmpTxtWindowZIndex = new Component();
    if( SubCompNamesArr[9] != null ) {
      this.cmpTxtWindowZIndex = new CmpTxtBoxWithBlur(SubCompNamesArr[9], null, null, 
                                                      { action: "FNCCALL",
                                                        context: this,
                                                        fnc: function () {
                                                          var winZIndex = jsx3.util.strTrim(this.cmpTxtWindowZIndex.BlkJSX.getValue());
                                                          if ( this.isNegativeNumber(winZIndex) ) {
                                                            if (!jsx3.util.strEmpty(windowBlkJSX)) {
                                                              windowAlert(windowBlkJSX, "Please enter valid z-index value(Only positive integers)");
                                                            }
                                                            else {
                                                              alert("Please enter valid z-index value(Only positive integers)");
                                                            }
                                                            this.cmpTxtWindowZIndex.BlkJSX.setValue("100");
                                                            this.cmpTxtWindowZIndex.BlkJSX.repaint();
                                                            this.cmpTxtWindowZIndex.BlkJSX.focus();
                                                            return;
                                                          }
                                                          if (jsx3.util.strEmpty(winZIndex)) {
                                                            winZIndex = 0;
                                                            this.cmpTxtWindowZIndex.BlkJSX.setValue(winZIndex);
                                                            this.cmpTxtWindowZIndex.BlkJSX.repaint();
                                                          }
                                                          var selectedNode = this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']"); ;
                                                          var selectedId = selectedNode.getAttribute("jsxid");
                                                          if (!jsx3.util.strEmpty(selectedId)) {
                                                            selectedNode.setAttribute('zIndex', winZIndex);
                                                            var masterXML = this.cmpCanvas.BlkJSX.getXML();
                                                            var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='" + selectedId + "']");
                                                            var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                            propertiesNode.setAttribute("zIndex", winZIndex);
                                                            this.cmpCanvas.repaint();
                                                          }
                                                        }
                                                      });      
    }    
    
  this.cmpBtnAddWindow = new Component();
    if( SubCompNamesArr[10] != null ) {
      this.cmpBtnAddWindow = new CmpImageButton(SubCompNamesArr[10],
                                                null,
                                                { action: "FNCCALL",
                                                   context: this,
                                                   fnc: function() {
                                                     //Create Default Key
                                                     this.cmpCanvas.BlkJSX.deselectAllWindows();
                                                     var newjsxid = this.getNewWindowId();
                                                     var newWindow = this.windowNode.cloneNode(true);
                                                     newWindow.setAttribute("id",newjsxid);

                                                     this.cmpTxtWindowId.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                     //Add Default Node to Canvas
                                                     var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                     var canvasNode = masterXML.selectSingleNode(".//record/Canvas");
                                                     canvasNode.appendChild(newWindow);
                                                     this.cmpCanvas.setBlkJSXSourceXML(masterXML);
                                                     //Process Default Node to Matrix data
//                                                     this.cmpWindowList.BlkJSX.clearXmlData();
                                                     var windowXML = this.xslDoc.transform(masterXML);
                                                     var strXMLCacheId =this.cmpWindowList.BlkJSX.getXMLId();
                                                     var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                     objXML.loadXML(windowXML);
                                                     var windowNode = objXML.selectSingleNode(".//record[@jsxid='"+newjsxid+"']");
                                                     windowNode.setAttribute("jsxselected","1");
                                                     this.masterCDF.BlkJSX.setCDFId(newjsxid);
                                                     
                                                     var selectedWindowNode = this.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='"+newjsxid+"']"); 
                                                     if(selectedWindowNode!=null){
                                                        topic_Set(tCanvasEditor,"CurrentWindow",newjsxid);
                                                        var currentCanvasId = this.getCanvasId();
                                                        topic_Set(tCanvasEditor,"CurrentCanvas",currentCanvasId);
                                                     }
                                                     //Paint Canvas Matrix
                                                     this.cmpCanvas.repaint();
                                                     this.cmpWindowList.BlkJSX.repaint();
                                                                                                        
                                                   }
                                                });

   } 
   
  this.cmpTxtDisplayWidth = new Component();
    if( SubCompNamesArr[11] != null ) {
      this.cmpTxtDisplayWidth = new CmpTxtBoxWithBlur(SubCompNamesArr[11], null, null,  
                                                    {action:"FNCCALL", 
                                                     context:this, 
                                                     fnc:function() {
                                                          var canvasWidth =  this.cmpTxtDisplayWidth.BlkJSX.getValue(); 
                                                          var message = "Please enter Canvas resolution in px";
                                                          if(this.isNegativeNumber(canvasWidth)){
                                                            if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                              windowAlert(windowBlkJSX, message);
                                                            }
                                                            else{
                                                              alert(message);
                                                            }
                                                            this.cmpTxtDisplayWidth.BlkJSX.setValue("1280");
                                                            this.cmpTxtDisplayWidth.BlkJSX.repaint();
                                                            return;
                                                          }
                                                          if (jsx3.util.strEmpty(canvasWidth)) {
                                                            if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                              windowAlert(windowBlkJSX, message);
                                                            }
                                                            else{
                                                              alert(message);
                                                            }
                                                            canvasWidth = 1280;
                                                            this.cmpTxtDisplayWidth.BlkJSX.setValue(canvasWidth);
                                                            this.cmpTxtDisplayWidth.BlkJSX.repaint();
                                                            this.cmpTxtDisplayWidth.BlkJSX.focus();
                                                          }
                                                          var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                          var canvasNode = masterXML.selectSingleNode(".//record/Canvas");
                                                          canvasNode.setAttribute("hres",canvasWidth);
                                                          //Paint Canvas Matrix
                                                          this.cmpCanvas.repaint();
                                                         }
                                                      });      
    }
    
  this.cmpTxtDisplayHeight = new Component();
    if( SubCompNamesArr[12] != null ) {
      this.cmpTxtDisplayHeight = new CmpTxtBoxWithBlur(SubCompNamesArr[12], null, null, 
                                                      {action:"FNCCALL", 
                                                       context:this, 
                                                       fnc:function() {
                                                            var canvasHeight =  this.cmpTxtDisplayHeight.BlkJSX.getValue(); 
                                                            var message = "Please enter Canvas resolution in px";
                                                            if(this.isNegativeNumber(canvasHeight)){
                                                             if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                                   windowAlert(windowBlkJSX, message);
                                                              }
                                                              else{
                                                                alert(message);
                                                              }
                                                              this.cmpTxtDisplayHeight.BlkJSX.setValue("1024");
                                                              this.cmpTxtDisplayHeight.BlkJSX.repaint();
                                                              return;
                                                            }
                                                            if (jsx3.util.strEmpty(canvasHeight)) {
                                                              if(!jsx3.util.strEmpty(windowBlkJSX)){
                                                                   windowAlert(windowBlkJSX, message);
                                                              }
                                                              else{
                                                                alert(message);
                                                              }
                                                              canvasHeight = 1024;
                                                              this.cmpTxtDisplayHeight.BlkJSX.setValue(canvasHeight);
                                                              this.cmpTxtDisplayHeight.BlkJSX.repaint();
                                                              this.cmpTxtDisplayHeight.BlkJSX.focus();
                                                            }
                                                            var masterXML =  this.cmpCanvas.BlkJSX.getXML();
                                                            var canvasNode = masterXML.selectSingleNode(".//record/Canvas");
                                                            canvasNode.setAttribute("vres",canvasHeight);
                                                            //Paint Canvas Matrix
                                                            this.cmpCanvas.repaint();
                                                           }
                                                        });      
    }
   
   this.cmpChkWindowBorder = new Component();
    if( SubCompNamesArr[13] != null ) {
      this.cmpChkWindowBorder = new CmpCheckBox(SubCompNamesArr[13],
                                                null,
                                                {action:"FNCCALL", 
                                                 context:null, 
                                                 fnc:function() {
                                                    var layoutNode =ThisClass.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Layout");
                                                    var checked = ThisClass.cmpChkWindowBorder.BlkJSX.getChecked();
                                                    if(checked == jsx3.gui.CheckBox.CHECKED ){ 
                                                      layoutNode.setAttribute("border","show");
                                                    }
                                                    else{
                                                      layoutNode.setAttribute("border","hide");
                                                    }
                                                 }
                                                }
                                               );
     }
    
   this.cmpChkWindowHeader = new Component();
    if( SubCompNamesArr[14] != null ) {
      this.cmpChkWindowHeader = new CmpCheckBox(SubCompNamesArr[14],
                                                null,
                                                {action:"FNCCALL", 
                                                 context:null, 
                                                 fnc:function() {
                                                    var layoutNode =ThisClass.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Layout");
                                                    var checked = ThisClass.cmpChkWindowHeader.BlkJSX.getChecked();
                                                    if(checked == jsx3.gui.CheckBox.CHECKED ){ 
                                                      layoutNode.setAttribute("header","show");
                                                    }
                                                    else{
                                                      layoutNode.setAttribute("header","hide");
                                                    }
                                                 }
                                                }
                                               );
     } 
   this.cmpBlkFullScreen = new Component();
    if( SubCompNamesArr[15] != null ) {
     this.cmpBlkFullScreen = new CmpCanvas(SubCompNamesArr[15],
                                           null,
                                           null);

    }
    
    this.cmpFullScreenCtr = new Component();
     if( SubCompNamesArr[16] != null ) {
     this.cmpFullScreenCtr = new CmpContainer(SubCompNamesArr[16],
                                            [{cmp:this.cmpBlkFullScreen}
                                            ],
                                            null,
                                            null );  
     
    }
    
    this.cmpCanvasCtr = new Component();
     if( SubCompNamesArr[17] != null ) {
     this.cmpCanvasCtr = new CmpContainer(SubCompNamesArr[17],
                                            [{cmp:this.cmpCanvas}
                                            ],
                                            null,
                                            null );  
    }
    
    this.cmpCanvasBlkCtr = new Component();
     if( SubCompNamesArr[18] != null ) {
      this.cmpCanvasBlkCtr = new CmpContainer(SubCompNamesArr[18],
                                            [{cmp:this.cmpCanvasCtr,
                                              sel:"topic_Get('"+tCanvasEditor+"', 'WindowMode') == 'CANVAS'"},
                                             {cmp:this.cmpFullScreenCtr,
                                              sel:"topic_Get('"+tCanvasEditor+"', 'WindowMode') == 'FULLSCREEN'"}
                                            ],
                                            [{action: "TOPICEVENT", topic: tCanvasEditor, eventvar:"WindowMode", eventval: null }],
                                            null );  
     }
    
    this.cmpSelCanvasLayout = new Component();
     if( SubCompNamesArr[19] != null ) {
     this.cmpSelCanvasLayout = new CmpSelect(SubCompNamesArr[19],
                                              null,
                                              { persist: { col: "jsxid", initval: false },
                                                action: "TOPICEVENT", topic: tCanvasEditor, eventvar: "CanvasLayoutSelected", eventval: "jsxid"
                                              }
                                             );    
    }
    
    
  this.chkWindowOneToOneAspect = new Component();
    if( SubCompNamesArr[20] != null ) {
      this.chkWindowOneToOneAspect = new CmpCheckBox(SubCompNamesArr[20],
                                                      null,
                                                      {action:"FNCCALL", 
                                                       context:null, 
                                                       fnc:function() {
                                                          var checked = ThisClass.chkWindowOneToOneAspect.BlkJSX.getChecked();
                                                          if(checked == jsx3.gui.CheckBox.CHECKED ){ 
                                                            topic_Publish(tCanvasEditor,"ScrollBtn","SHOW");
                                                          }
                                                          else{
                                                            topic_Publish(tCanvasEditor,"ScrollBtn","HIDE");
                                                          }
                                                       }
                                                      }
                                                     );
     } 
    
    this.cmpBtnScrollBar = new Component();
      if( SubCompNamesArr[21] != null ) {
        this.cmpBtnScrollBar = new CmpButton(SubCompNamesArr[21],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                ThisClass.SetOneToOneVideoPropeties();
                                                topic_Publish(tCanvasEditor,"PropertiesOrScroll","SCROLL");
                                              }
                                             }
                                            );
    }  
    
    this.cmpBtnBack = new Component();
      if( SubCompNamesArr[22] != null ) {
        this.cmpBtnBack = new CmpButton(SubCompNamesArr[22],
                                         null, 
                                         {action:"FNCCALL", 
                                          context:null, 
                                          fnc:function() {
                                              topic_Publish(tCanvasEditor,"PropertiesOrScroll","PROPERTIES");
                                            var absolutePositionWindow =ThisClass.cmpBlkWindow.BlkJSX.getAbsolutePosition();
                                            var absolutePositionVideo =ThisClass.cmpBlkVideo.BlkJSX.getAbsolutePosition();
                                            var leftShift = absolutePositionWindow.L - absolutePositionVideo.L ;
                                            var topShift = absolutePositionWindow.T - absolutePositionVideo.T ;
                                          }
                                         }
                                        );
    }  
    
    this.cmpPropertiesCtr = new Component();
     if( SubCompNamesArr[23] != null ) {
     this.cmpPropertiesCtr = new CmpContainer(SubCompNamesArr[23],
                                              [{cmp:this.cmpTxtWindowId},
                                               {cmp:this.cmpTxtWindowLeft},
                                               {cmp:this.cmpTxtWindowTop},
                                               {cmp:this.cmpTxtWindowWidth},
                                               {cmp:this.cmpTxtWindowHeight},
                                               {cmp:this.cmpTxtWindowTransparency},
                                               {cmp:this.cmpTxtWindowZIndex},
                                               {cmp:this.chkWindowOneToOneAspect},
                                               {cmp:this.cmpBtnScrollBar,
                                                sel:"topic_Get('"+tCanvasEditor+"', 'ScrollBtn') == 'SHOW'"}
                                              ],
                                              [{action: "TOPICEVENT", topic: tCanvasEditor, eventvar:"ScrollBtn", eventval: null }],
                                              null );  
    }
    
    this.cmpScrollCtr = new Component();
     if( SubCompNamesArr[24] != null ) {
     this.cmpScrollCtr = new CmpContainer(SubCompNamesArr[24],
                                            [{cmp:this.cmpBlkWindow},
                                             {cmp:this.cmpBlkVideo},
                                             {cmp:this.cmpBtnBack}
                                            ],
                                            null,
                                            null );  
    }  
    
   this.cmpBlkWindow = new Component();
    if( SubCompNamesArr[25] != null ) {
     this.cmpBlkWindow = new Component(SubCompNamesArr[25],null,null,null);
    }
   this.windowWidth = this.cmpBlkWindow.BlkJSX.getWidth();   
   this.windowHeight = this.cmpBlkWindow.BlkJSX.getHeight();
   
   this.cmpBlkVideo = new Component();
    if( SubCompNamesArr[26] != null ) {
     this.cmpBlkVideo = new Component(SubCompNamesArr[26],null,null,null);
    }  
      
  this.cmpBlkProperties = new Component();
   if( SubCompNamesArr[27] != null ) {
    this.cmpBlkProperties = new CmpContainer( SubCompNamesArr[27],
                                              [{cmp: this.masterCDF},
                                               {cmp:this.cmpWindowList},
                                               {cmp:this.cmpBtnAddWindow},
                                               {cmp:this.cmpPropertiesCtr,
                                                sel:"topic_Get('"+tCanvasEditor+"', 'PropertiesOrScroll') == 'PROPERTIES'"},
                                               {cmp:this.cmpScrollCtr,
                                                sel:"topic_Get('"+tCanvasEditor+"', 'PropertiesOrScroll') == 'SCROLL'"}
                                              ],
                                              [{action: "TOPICEVENT", topic: tCanvasEditor, eventvar:"PropertiesOrScroll", eventval: null }],
                                              null );  
   } 
    var SubCmpArr = [{cmp: this.cmpCanvasBlkCtr},
                     {cmp:this.cmpTxtDisplayWidth},
                     {cmp:this.cmpTxtDisplayHeight},
//                     {cmp:this.cmpChkWindowBorder},
//                     {cmp:this.cmpChkWindowHeader},
                     {cmp:this.cmpBlkProperties}
                    ];

    CmpContainer.prototype.constructor.call(this, BlkName, SubCmpArr, EventArr);
  }

  this.init = function() {
    CmpCanvasEditorContainer.BaseClass.init.call(this);
    var xmlId = "master_xml_"+jsx3.xml.CDF.getKey();
    this.masterCDF.BlkJSX.setXMLId(xmlId);
    this.cmpWindowList.BlkJSX.setXMLId(xmlId);
  }

  this.show = function() {
    CmpCanvasEditorContainer.BaseClass.show.call(this);
  } // end this.show
  
  // function to Add first new available Win+i form id 
  this.getNewWindowId = function(){
    var objXML = this.cmpCanvas.BlkJSX.getXML();
    var i =1;
    var newWindowId = "Win" + i;
    var windowNode = objXML.selectSingleNode(".//record/Canvas/Window[@id='"+newWindowId+"']");
    while(!jsx3.util.strEmpty(windowNode)){
      i++;
      newWindowId = "Win" + i;
      windowNode = objXML.selectSingleNode(".//record/Canvas/Window[@id='"+newWindowId+"']");
    }
    return newWindowId;
  }// end getNewWindowId

 this.setFullScreen = function(objXML) {
    var newObjXML = objXML.cloneDocument();
    this.cmpBlkFullScreen.setBlkJSXSourceXML(newObjXML);
    var canvasNode= this.cmpBlkFullScreen.BlkJSX.getXML().selectSingleNode(".//record/Canvas");
    var windowNode =canvasNode.selectSingleNode(".//Window");
    var windowIterator =canvasNode.selectNodeIterator(".//Window");
    while(windowIterator.hasNext()){
        var node = windowIterator.next();
        canvasNode.removeChild(node);
    }
    windowNode.setAttribute("jsxselected","1");
    windowNode.selectSingleNode("//Properties").setAttribute("hres","100%");
    windowNode.selectSingleNode("//Properties").setAttribute("vres","100%");
    windowNode.selectSingleNode("//Properties").setAttribute("hpos","0");
    windowNode.selectSingleNode("//Properties").setAttribute("vpos","0");
    canvasNode.appendChild(windowNode);
    this.cmpBlkFullScreen.repaint();
  } 
 
  this.setSelLayoutData = function(objXML) {
    var newObjXML = objXML.cloneDocument();
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS-Common/xsl/CanvasLayout.xsl","CanvasLayout","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var layoutXML = xslDoc.transform(newObjXML);
    var objLayoutXML = new jsx3.xml.Document().loadXML(layoutXML); 
    this.cmpSelCanvasLayout.BlkJSX.setSourceXML(objLayoutXML);
    var id = this.cmpSelCanvasLayout.BlkJSX.getXML().selectSingleNode(".//record").getAttribute("jsxid");
    if(!jsx3.util.strEmpty(id)){
      this.cmpSelCanvasLayout.BlkJSX.setValue(id);
    }
  }

  this.onResponse = function(respCode, respVal) {
    if (respCode == "SUCCESS" && respVal != null) {
      //XML attached to Canvas Editor
      var strXMLCacheId = this.cmpCanvas.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      var masterXML = objXML.loadXML(respVal);
      this.cmpCanvas.setBlkJSXSourceXML(masterXML);
      this.cmpCanvas.BlkJSX.deselectAllWindows();
      //      this.setFullScreen(masterXML);
      var canvasNode = masterXML.selectSingleNode(".//record/Canvas");
      var canvasHeight = canvasNode.getAttribute("vres");
      var canvasWidth = canvasNode.getAttribute("hres");
      this.cmpTxtDisplayHeight.BlkJSX.setValue(canvasHeight)
      this.cmpTxtDisplayWidth.BlkJSX.setValue(canvasWidth);
      this.setSelLayoutData(masterXML);
      //Process master XML to feed the window Matrix
      var windowXML = this.xslDoc.transform(masterXML);

      //    this.cmpWindowList.BlkJSX.clearXmlData();
      var objWinXML = new jsx3.xml.Document().loadXML(windowXML);
      this.cmpWindowList.onResponse(objWinXML);
      var currentCanvasId = topic_Get(tCanvasEditor, "CurrentCanvas");
      if (currentCanvasId != canvasNode.getAttribute("id")) {
        topic_Set(tCanvasEditor, "CurrentWindow", "NONE");
      }
      //Mark the last selected window, if nothing is selcted select the first window.
      var currentWindow = topic_Get(tCanvasEditor, "CurrentWindow");
      if (!jsx3.util.strEmpty(currentWindow) && currentWindow != "NONE") {
        var selectedWindowNode = canvasNode.selectSingleNode(".//Window[@id='" + currentWindow + "']");
      }
      else {
        var selectedWindowNode = canvasNode.selectSingleNode(".//Window");
      }
      if (!jsx3.util.strEmpty(selectedWindowNode)) {
        var windowId = selectedWindowNode.getAttribute("id");
        selectedWindowNode.setAttribute("jsxselected", "1");
        this.cmpCanvas.selRecord = this.cmpCanvas.BlkJSX.getWindow(windowId);
        topic_Publish(tCanvasEditor, "SelectWindow", windowId);
      }
      this.cmpCanvas.repaint();
      this.cmpWindowList.BlkJSX.repaint();
      topic_Publish(tCanvasEditor, "WindowMode", "CANVAS");
      //Attach the processed windowXML to WindowList subcomponent of Canvas Editor
      //      this.show();
    }
  } 
 
  this.getCanvasXML = function(){
    this.cmpCanvas.BlkJSX.deselectAllWindows();
    this.SetScrollPositions();
    var strXMLCacheId = this.cmpCanvas.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId).cloneNode(true); 
    return objXML;
  }
  
  this.getCanvasId = function(){
    var canvasId="";
    var canvasNode = this.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas");
    if(!jsx3.util.strEmpty(canvasNode)){
     canvasId = canvasNode.getAttribute("id");
    }
    return canvasId;
  }
  
  this.clearPropertiesTextBoxes = function(){
    this.cmpTxtWindowId.BlkJSX.setValue("");
    this.cmpTxtWindowLeft.BlkJSX.setValue("");
    this.cmpTxtWindowTop.BlkJSX.setValue("");
    this.cmpTxtWindowWidth.BlkJSX.setValue("");
    this.cmpTxtWindowHeight.BlkJSX.setValue(""); 
    this.cmpTxtWindowTransparency.BlkJSX.setValue("");
    this.cmpTxtWindowZIndex.BlkJSX.setValue("");  
  } 

  this.isNegativeNumber = function(winZIndex){
    if(isNaN(winZIndex) || winZIndex < 0){
      return true;
    }
    return false;
  }

  this.isNumberOrPercentage = function (dimension, compareValue) {
    dimension = jsx3.util.strTrim(dimension);
    var ispercentage = dimension.substr(dimension.length-1,1);
    if( ispercentage == "%"){
      var splitResult =dimension.split("%");
      dimension = splitResult[0];
      if (isNaN(dimension)) {
        if (!jsx3.util.strEmpty(windowBlkJSX)) {
          windowAlert(windowBlkJSX, "Please enter valid dimensions in px or %");
        }
        else {
          alert("Please enter valid dimensions in px or %");
        }
        return false;
      }
      else {
        if (dimension < 0 || dimension >100) {
          if (!jsx3.util.strEmpty(windowBlkJSX)) {
            windowAlert(windowBlkJSX, "Please enter dimension between 0-100%");
          }
          else {
            alert("Please enter dimension between 0-100%");
          }
          return false;
        }
        else return true;
      }
    }

    if(isNaN(dimension)){
     if(!jsx3.util.strEmpty(windowBlkJSX)){
       windowAlert(windowBlkJSX,"Please enter valid dimensions in px or %");
      }
      else{
        alert("Please enter valid dimensions in px or %");
      }
      return false;
    }
    else{
      var canvasHeight = this.cmpTxtDisplayHeight.BlkJSX.getValue();
      var canvasWidth = this.cmpTxtDisplayWidth.BlkJSX.getValue();

      if (jsx3.util.strEmpty(dimension)) {
        if (!jsx3.util.strEmpty(windowBlkJSX)) {
          windowAlert(windowBlkJSX, "Please enter non negative dimensions in px or %");
        }
        else {
          alert("Please enter non negative dimensions in px or %");
        }
        return false;
      }
      else if ( dimension < 0 ) {
        if(!jsx3.util.strEmpty(windowBlkJSX)){
          windowAlert(windowBlkJSX,"Please enter non negative dimensions in px or %");
        }
        else{
          alert("Please enter non negative dimensions in px or %");
        }
        return false;
      }
      else if (parseInt(dimension, 10) > parseInt(compareValue, 10) ) {
        if (!jsx3.util.strEmpty(windowBlkJSX)) {
          windowAlert(windowBlkJSX, "Please enter dimensions less than the Canvas resolution " + canvasWidth + "x" + canvasHeight);
        }
        else {
          alert("Please enter dimensions less than the Canvas resolution " + canvasWidth + "x" + canvasHeight);
        }
        return false;
      }
      else return true;
    }
  }
  
  this.SetOneToOneVideoPropeties = function(){
    var selectedWindow =topic_Get(tCanvasEditor,"SelectWindow"); 
    var canvasNode = ThisClass.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas");
    var windowNode = canvasNode.selectSingleNode(".//Window[@id='"+selectedWindow+"']");
    var windowWidth = ThisClass.cmpCanvas.BlkJSX.getCanvasWindowWidth(windowNode,canvasNode);
    var windowHeight = ThisClass.cmpCanvas.BlkJSX.getCanvasWindowHeight(windowNode,canvasNode);
    this.SetOneToOneWindowDimensions(windowWidth,windowHeight);
    this.SetOneToOneVideoDimensions(canvasNode,windowNode);
    
  }

  this.SetOneToOneWindowDimensions = function(windowWidth,windowHeight){
    this.scaleFactor= 1;
    while(windowWidth >= this.windowWidth || windowHeight >= this.windowHeight){
     this.scaleFactor= this.scaleFactor - 0.1 ;
     windowWidth = parseInt(windowWidth*this.scaleFactor);
     windowHeight = parseInt(windowHeight*this.scaleFactor);
    }
    ThisClass.cmpBlkWindow.BlkJSX.setWidth(windowWidth,true);
    ThisClass.cmpBlkWindow.BlkJSX.setHeight(windowHeight,true);
    ThisClass.cmpBlkWindow.repaint();
  }
  
  this.SetOneToOneVideoDimensions = function(canvasNode,windowNode){
    var ConnectionContent = windowNode.selectSingleNode(".//ConnectionContent");
    var videoWidth =ConnectionContent.getAttribute("hres");
    var videoHeight =ConnectionContent.getAttribute("vres");
    if(!jsx3.util.strEmpty(videoWidth)){
     this.SetOneToOneVideoWidth(canvasNode,windowNode,videoWidth);
    }
    if(!jsx3.util.strEmpty(videoHeight)){
      this.SetOneToOneVideoHeight(canvasNode,windowNode,videoHeight);
    }
  }
  
  
  this.SetOneToOneVideoWidth = function(canvasNode,windowNode,videoWidth){
//    var orgWindowWidth = windowNode.selectSingleNode(".//Properties").getAttribute("hres'");   
//    var windowWidth = ThisClass.cmpCanvas.BlkJSX.getCanvasWindowWidth(windowNode,canvasNode);
//    var widthRatio = orgWindowWidth/windowWidth;
//    var newVideoWidth = parseInt(videoWidth/widthRatio);
    ThisClass.cmpBlkVideo.BlkJSX.setWidth(videoWidth,true);
  }

  this.SetOneToOneVideoHeight = function(canvasNode,windowNode,videoHeight){
//    var orgWindowHeight = windowNode.selectSingleNode(".//Properties").getAttribute("vres'");
//    var windowHeight = ThisClass.cmpCanvas.BlkJSX.getCanvasWindowHeight(windowNode,canvasNode);
//    var heightRatio = orgWindowHeight/windowHeight;
//    var newVideoHeight = parseInt(videoHeight/heightRatio);
    ThisClass.cmpBlkVideo.BlkJSX.setHeight(videoHeight,true);
  } 
  
  this.SetScrollPositions = function(){
    if(topic_Get(tCanvasEditor,"PropertiesOrScroll") =="SCROLL"){
      var absolutePositionWindow =ThisClass.cmpBlkWindow.BlkJSX.getAbsolutePosition();
      var absolutePositionVideo =ThisClass.cmpBlkVideo.BlkJSX.getAbsolutePosition();
      var leftShift = absolutePositionWindow.L - absolutePositionVideo.L ;
      var topShift = absolutePositionWindow.T - absolutePositionVideo.T ;
      var selectedWindow =topic_Get(tCanvasEditor,"SelectWindow"); 
      var windowNode = ThisClass.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='"+selectedWindow+"']");
      var propertiesNode = windowNode.selectSingleNode(".//Properties");
      propertiesNode.setAttribute("scrollHPos",leftShift);
      propertiesNode.setAttribute("scrollVPos",topShift);
    }
  }
  
  eventCanvasEditor.subscribe("SelectWindow",null, this,
                                function(eventVar, eventVal) {
                                  if(this.SelEvent != null){
                                    if(this.cmpCanvas.selRecord != null){
                                      this.selRecordID = eventVal;
                                      this.selRecord = this.cmpCanvas.selRecord;
                                    }
                                    CmpCanvasEditorContainer.BaseClass.onSelect.call(this);
                                  }
                                  var windowId = eventVal;
                                  var selectedWindowNode = this.cmpCanvas.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='"+windowId+"']"); 
                                  if(selectedWindowNode!=null){
                                    topic_Set(tCanvasEditor,"CurrentWindow",windowId);
                                    var currentCanvasId = this.getCanvasId();
                                    topic_Set(tCanvasEditor,"CurrentCanvas",currentCanvasId);
                                    var connId = selectedWindowNode.selectSingleNode(".//ConnectionContent").getAttribute("connId");
                                    if(!jsx3.util.strEmpty(connId)){
                                      this.cmpTxtWindowId.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                    }
                                    else{
                                      this.cmpTxtWindowId.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                    }
                                  }
                                  this.cmpWindowList.BlkJSX.deselectAllRecords();
                                  var winRecord =this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='"+windowId+"']");
                                  winRecord.setAttribute("jsxselected","1");
                                  this.cmpWindowList.BlkJSX.repaint();
                                  this.masterCDF.BlkJSX.setCDFId(windowId);
                                }
                              );
                              
  eventCanvasEditor.subscribe("SelectListWindow",null, this,
                                  function (eventVar, eventVal) {
                                    var selectedId = eventVal;
                                    ThisClass.masterCDF.BlkJSX.setCDFId(selectedId);
                                    var masterXML =  ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedId+"']");
                                    ThisClass.cmpCanvas.BlkJSX.selectWindow(selectedId);
                                    this.cmpCanvas.selRecord = ThisClass.cmpCanvas.BlkJSX.getWindow(selectedId);
                                    if (this.SelEvent != null) {
                                      if (this.cmpCanvas.selRecord != null) {
                                        this.selRecordID = eventVal;
                                        this.selRecord = this.cmpCanvas.selRecord;
                                      }
                                      CmpCanvasEditorContainer.BaseClass.onSelect.call(this);
                                    }

                                    var connId = windowNode.selectSingleNode(".//ConnectionContent").getAttribute("connId");
                                    if(!jsx3.util.strEmpty(connId)){
                                      ThisClass.cmpTxtWindowId.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                    }
                                    else{
                                      ThisClass.cmpTxtWindowId.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                    }
                                    topic_Set(tCanvasEditor,"CurrentWindow",selectedId);
                                    this.cmpWindowList.repaint();
                                  }
                                );  
                                                          
  eventCanvasEditor.subscribe("WindowFullScreen",null, this,
                                  function(eventVar, eventVal) {
                                    var objRecordId = eventVal; 
                                    //log.info("Subscribe WindowFullScreen eventVal:" + eventVal);
                                    var masterXML = ThisClass.cmpCanvas.BlkJSX.getXML();

                                    //Only one of the window can have Maximize button so thats why setting all other to false.
                                    // deselect all other full screen in the window list
                                    var windowListIter = this.cmpWindowList.BlkJSX.getXML().selectNodeIterator(".//record[@jsxid !='" + objRecordId + "']");
                                    while (windowListIter.hasNext()) {
                                      var windowRecord = windowListIter.next();
                                      windowRecord.setAttribute("maximize", "false");
                                    }
                                    // deselect all other full screen in the masterXml list
                                    var recordIter = masterXML.selectNodeIterator(".//record/Canvas/Window[@id !='" + objRecordId + "']");
                                    while (recordIter.hasNext()) {
                                      var record = recordIter.next();
                                      var propertiesNode = record.selectSingleNode(".//Properties");
                                      propertiesNode.setAttribute("maximize", "false");
                                    }

                                    var winRecord = this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='"+objRecordId+"']");
                                    var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+objRecordId+"']");
                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                    var fullScreen = winRecord.getAttribute("maximize");
                                    if(fullScreen == "true"){
  //                                    topic_Publish(tCanvasEditor,"WindowMode","CANVAS");
                                      winRecord.setAttribute("maximize","false");
                                      propertiesNode.setAttribute("maximize","false");
                                    }
                                    else {
  //                                    this.cmpBlkFullScreen.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window").setAttribute("id",objRecordId);
  //                                    this.cmpBlkFullScreen.repaint();
  //                                    topic_Publish(tCanvasEditor,"WindowMode","FULLSCREEN");
                                      winRecord.setAttribute("maximize","true");
                                      propertiesNode.setAttribute("maximize","true");
                                    }
                                    //log.info("Subscribe WindowFullScreen winRecord:" + winRecord);
                                    if(!jsx3.util.strEmpty(winRecord.getAttribute("jsxselected")))
                                    this.cmpWindowList.repaint();
                                    this.cmpCanvas.repaint();  
                                  }
                                );
                              
  eventCanvasEditor.subscribe("WindowOneToOne",null, this,
                                  function(eventVar, eventVal) {
                                    var objRecordId = eventVal; 
                                    //log.info("Subscribe WindowOneToOne eventVal:" + eventVal);
                                    var winRecord =this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='"+objRecordId+"']");
                                    var masterXML =  ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode =masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+objRecordId+"']");
                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                    var scale = winRecord.getAttribute("scale");
                                    if(scale == "fitWindow" ){
                                      winRecord.setAttribute("scale","fitNativeSize");
                                      propertiesNode.setAttribute("scale","fitNativeSize");
                                    }
                                    else {
                                      winRecord.setAttribute("scale","fitWindow");
                                      propertiesNode.setAttribute("scale","fitWindow");
                                    }
                                    if(!jsx3.util.strEmpty(winRecord.getAttribute("jsxselected")))
                                    this.cmpWindowList.repaint();
                                    this.cmpCanvas.repaint(); 
                                  }
                                );
                              
  eventCanvasEditor.subscribe("WindowMute",null, this,
                                  function(eventVar, eventVal) {
                                    var objRecordId = eventVal; 
                                    //log.info("Subscribe WindowMute eventVal:" + eventVal);
                                    var winRecord =this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='"+objRecordId+"']");
                                    var masterXML =  ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode =masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+objRecordId+"']");
                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                    var mute = winRecord.getAttribute("mute");
                                    if(mute =="true"){
                                      winRecord.setAttribute("mute","false");
                                      propertiesNode.setAttribute("mute","false");
                                    }
                                    else {
                                      winRecord.setAttribute("mute","true");
                                      propertiesNode.setAttribute("mute","true");
                                    }
                                    if(!jsx3.util.strEmpty(winRecord.getAttribute("jsxselected")))
                                    this.cmpWindowList.repaint();
                                    this.cmpCanvas.repaint();     
                                  }
                                );
                              
  eventCanvasEditor.subscribe("WindowVisible",null, this,
                                  function(eventVar, eventVal) {
                                    var objRecordId = eventVal;
                                    //log.info("Subscribe WindowVisible eventVal:" + eventVal);
                                    var winRecord =this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='"+objRecordId+"']"); 
                                    var masterXML =  ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode =masterXML.selectSingleNode(".//record/Canvas/Window[@id ='"+objRecordId+"']");
                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                    var visibility = winRecord.getAttribute("visibility");
                                    if(visibility == "true"){
                                      winRecord.setAttribute("visibility","false");
                                      propertiesNode.setAttribute("visibility","false");
                                    }
                                    else {
                                      winRecord.setAttribute("visibility","true");
                                      propertiesNode.setAttribute("visibility","true");
                                    }
                                    if(!jsx3.util.strEmpty(winRecord.getAttribute("jsxselected")))
                                    this.cmpWindowList.repaint();
                                    this.cmpCanvas.repaint();      
                                  }
                                );

   eventCanvasEditor.subscribe("WindowKbm", null, this,
                                  function (eventVar, eventVal) {
                                    var objRecordId = eventVal;
                                    //log.info("Subscribe WindowKbm eventVal:" + eventVal);
                                    var winRecord = this.cmpWindowList.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" + objRecordId + "']");
                                    var masterXML = ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='" + objRecordId + "']");
                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                    var remoteKbmCtrl = winRecord.getAttribute("remoteKbmCtrl");
                                    if (remoteKbmCtrl == "true") {
                                      winRecord.setAttribute("remoteKbmCtrl", "false");
                                      propertiesNode.setAttribute("remoteKbmCtrl", "false");
                                    }
                                    else {
                                      winRecord.setAttribute("remoteKbmCtrl", "true");
                                      propertiesNode.setAttribute("remoteKbmCtrl", "true");
                                    }
                                    if (!jsx3.util.strEmpty(winRecord.getAttribute("jsxselected")))
                                    this.cmpWindowList.repaint();
                                    this.cmpCanvas.repaint();
                                  }
                                );

 eventCanvasEditor.subscribe("WindowDelete", null, this,
                                  function(eventVar, eventVal) {
                                    var recordIdArray = ThisClass.cmpWindowList.BlkJSX.getRecordIds();
                                    if (recordIdArray.length <= 1) {
                                      if (!jsx3.util.strEmpty(windowBlkJSX)) {
                                        windowAlert(windowBlkJSX, "Cannot have a empty Canvas");
                                      }
                                      else {
                                        alert("Cannot have a empty Canvas");
                                      }
                                      return;
                                    }
                                    var objRecordId = eventVal;
                                    //log.info("Subscribe WindowDelete eventVal:" + eventVal);
                                    var ObjRecord = ThisClass.cmpWindowList.BlkJSX.getRecord(objRecordId);
                                    
                                    var masterXML = ThisClass.cmpCanvas.BlkJSX.getXML();
                                    var windowNode = masterXML.selectSingleNode(".//record/Canvas/Window[@id ='" + objRecordId + "']");

                                    //checking if any connection in the window. If its there then alert user.
                                    var connId = windowNode.selectSingleNode(".//ConnectionContent").getAttribute("connId");
                                    if (!jsx3.util.strEmpty(connId)) {
                                      var alertWindowDeleteMsg = "You cannot delete a window with an active connection.\nTo remove the window you must first delete the connection.";
                                      if (!jsx3.util.strEmpty(windowBlkJSX)) {
                                        windowAlert(windowBlkJSX, alertWindowDeleteMsg);
                                      }
                                      else {
                                        alert(alertWindowDeleteMsg);
                                      }
                                      return;
                                    }

                                    ThisClass.cmpWindowList.BlkJSX.deleteRecord(objRecordId, false);
                                    
                                    var parentNode = windowNode.getParent();
                                    parentNode.removeChild(windowNode);
                                    // check if the deleted window is selected then clear the window properties ewction
                                    if( objRecordId == topic_Get(tCanvasEditor, "CurrentWindow") ){
                                      topic_Set(tCanvasEditor, "CurrentWindow", "NONE");                                    
                                      ThisClass.cmpTxtWindowId.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowLeft.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowTop.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowWidth.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowHeight.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowTransparency.BlkJSX.setValue("");
                                      ThisClass.cmpTxtWindowZIndex.BlkJSX.setValue("");                                    
                                    }

                                    ThisClass.cmpWindowList.repaint();
                                    ThisClass.cmpCanvas.repaint();

                                  }
                                );
} // end CmpCanvasEditorContainer

//------------------PVR TrimLength--------
CmpPVRTrimLengthContainer.prototype = new CmpContainer();
function CmpPVRTrimLengthContainer(BlkName, SubCompNamesArr, EventArr, SelEvent, windowBlkJSX, winCollaboration) {
  CmpPVRTrimLengthContainer.BaseClass = CmpPVRTrimLengthContainer.prototype;
  CmpPVRTrimLengthContainer.prototype.constructor = CmpPVRTrimLengthContainer;
  var ThisClass = this;

  if( BlkName != null ) {
    Component.prototype.constructor.call(this, BlkName, EventArr, SelEvent);
    var SubCompNamesArr = SubCompNamesArr;
    
    this.chkEnablePVR = new Component();
    if( SubCompNamesArr[0] != null ) {
      this.chkEnablePVR = new CmpCheckBox(SubCompNamesArr[0],
                                          null,
                                          {action:"FNCCALL", 
                                           context:null, 
                                           fnc:function() {
                                              var checked = ThisClass.chkEnablePVR.BlkJSX.getChecked();
                                              if(checked == jsx3.gui.CheckBox.CHECKED ){ 
                                                if(!jsx3.util.strEmpty(winCollaboration)){
                                                  var trimLength = ThisClass.getEnablePVRTrimLength();
                                                  if(trimLength != false){
                                                    winCollaboration.sessionConfig.setPVREnable_PVRTrimLength(true, trimLength);
                                                  }
                                                }
                                                ThisClass.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                ThisClass.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                              }
                                              else{
                                                if(!jsx3.util.strEmpty(winCollaboration)){
                                                  winCollaboration.sessionConfig.setPVREnable(false);
                                                }
                                                ThisClass.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                ThisClass.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                              }
                                              if(jsx3.util.strEmpty(winCollaboration)){
                                                this.doCmpEvent(ThisClass.DataUpdatedEvent);
                                              }
                                           }
                                          }
                                         );
     }
     
    this.cmpTxtSessionHrPVRTrimLength = new Component();
    if( SubCompNamesArr[1] != null ) {
      this.cmpTxtSessionHrPVRTrimLength = new CmpTxtBoxWithBlur(SubCompNamesArr[1], null, null, 
                                                                   { action: "FNCCALL",
                                                                     context: this,
                                                                     fnc: function () {
                                                                       this.validatePVRTrimLength();
                                                                     }
                                                                   });   
    }

    this.cmpTxtSessionMinPVRTrimLength = new Component();
    if( SubCompNamesArr[2] != null ) {
      this.cmpTxtSessionMinPVRTrimLength = new CmpTxtBoxWithBlur(SubCompNamesArr[2], null, null, 
                                                                    { action: "FNCCALL",
                                                                      context: this,
                                                                      fnc: function () {
                                                                        var trimLength = this.validatePVRTrimLength();
                                                                        if(trimLength != false){
                                                                          this.minutesConversiontoHours();
                                                                        }
                                                                      }
                                                                    });   
    }


    var SubCmpArr = [{cmp: this.chkEnablePVR},
                     {cmp:this.cmpTxtSessionHrPVRTrimLength},
                     {cmp:this.cmpTxtSessionMinPVRTrimLength}
                    ];

    CmpContainer.prototype.constructor.call(this, BlkName, SubCmpArr, EventArr);
    }

    this.init = function() {
      CmpPVRTrimLengthContainer.BaseClass.init.call(this);
    }

    this.show = function() {
      CmpPVRTrimLengthContainer.BaseClass.show.call(this);
    }
    
    this.onResponse = function(respCode, respVal) {
      if (respCode == "SUCCESS" && respVal != null) {
        if(!jsx3.util.strEmpty(winCollaboration)){
          var trimLength = respVal.getPVRTrimLength();
          this.setPVRTrimLengthValues(trimLength);

          // get the PVR INFO
          var enablePVR = respVal.getPVREnable();
          this.setPVREnable(enablePVR);
        }
      }
    }
    
    this.minutesConversiontoHours = function(){
      var trimLength = this.cmpTxtSessionMinPVRTrimLength.BlkJSX.getValue();
      var trimLengthMin = parseInt(trimLength, 10);
      if(trimLengthMin < 60){
        this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue(trimLengthMin);
      }
      else{
        trimLengthHr = parseInt(trimLength, 10) / 60;
        trimLengthHr = Math.floor(trimLengthHr)
        if(trimLengthHr >= 1 && trimLengthHr <= 24){
          var pvrHours = "";
          var oldPVRHours = this.cmpTxtSessionHrPVRTrimLength.BlkJSX.getValue();
          if(!jsx3.util.strEmpty(oldPVRHours)){
            pvrHours = parseInt(oldPVRHours, 10) + parseInt(trimLengthHr, 10);
          }
          else{
            pvrHours = trimLengthHr;
            this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(trimLengthHr);
          }
          if(pvrHours > 24 ){
            var messageHrsTxt = "Max PVR Trim Length is 24 Hours";
            if(!jsx3.util.strEmpty(windowBlkJSX)){
              windowAlert(windowBlkJSX, messageHrsTxt);
            }
            else{
              alert(messageHrsTxt);
            }
            this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(oldPVRHours);
            this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue("0");
            this.cmpTxtSessionMinPVRTrimLength.BlkJSX.focus();
            return false;
          }
          else{
            var trimLengthMin = parseInt(trimLength, 10) - (parseInt(trimLengthHr, 10) * 60);
            trimLengthMin = parseInt(trimLengthMin, 10);
            if(pvrHours == 24 ){
              if(trimLengthMin > 0){
                var messageHrsTxt = "Max PVR Trim Length is 24 Hours";
                if(!jsx3.util.strEmpty(windowBlkJSX)){
                  windowAlert(windowBlkJSX, messageHrsTxt);
                }
                else{
                  alert(messageHrsTxt);
                }
                this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(pvrHours);
                this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue("0");
                this.cmpTxtSessionMinPVRTrimLength.BlkJSX.focus();
                return false;
              }
              else{
                this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(pvrHours);
                this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue(trimLengthMin);
              }
            }
            else{
              this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(pvrHours);
              this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue(trimLengthMin);
            }
          }
        }
      }
    }

    this.validatePVRTrimLength = function() {
      var trimLengthMinBlkJSX = this.cmpTxtSessionMinPVRTrimLength.BlkJSX;
      var trimLengthHrBlkJSX = this.cmpTxtSessionHrPVRTrimLength.BlkJSX;
      var trimLengthMin = trimLengthMinBlkJSX.getValue();
      var trimLengthHr = trimLengthHrBlkJSX.getValue();
      var message = "Please enter numeric value in minutes between 1 and 1440 for PVR Trim Length";
      var hrMessage = "Please enter numeric value in hours between 0 and 24 for PVR Trim Length";
      var messageMinTxt = "Min PVR Trim Length is 1 Minute";
      var messageHrsTxt = "Max PVR Trim Length is 24 Hours";
      if ((jsx3.util.strEmpty(trimLengthMin) || parseInt(trimLengthMin, 10) == 0) && (parseInt(trimLengthHr, 10) == 0 || jsx3.util.strEmpty(trimLengthHr))) {
        if(!jsx3.util.strEmpty(windowBlkJSX)){
         windowAlert(windowBlkJSX, messageMinTxt);
        }
        else{
          alert(messageMinTxt);
        }
        trimLengthMinBlkJSX.setValue("1");
        trimLengthMinBlkJSX.focus();    
        return false;
      }
      else if (trimLengthMin.match(/[^0-9]/)) {
        if(!jsx3.util.strEmpty(windowBlkJSX)){
         windowAlert(windowBlkJSX, message);
        }
        else{
          alert(message);
        }
        trimLengthMinBlkJSX.setValue("1");
        trimLengthMinBlkJSX.focus();
        return false;
      }
      else if(parseInt(trimLengthMin) > 1440 || parseInt(trimLengthMin) < 0){
        if(parseInt(trimLengthHr) > 0){
          if(!jsx3.util.strEmpty(windowBlkJSX)){
           windowAlert(windowBlkJSX, messageHrsTxt);
          }
          else{
            alert(messageHrsTxt);
          }
          var oldPVRHours = trimLengthHrBlkJSX.getValue();
          trimLengthHrBlkJSX.setValue(oldPVRHours);
          trimLengthMinBlkJSX.setValue("0");
          trimLengthMinBlkJSX.focus();
          return false;
        }
        else{
          if(!jsx3.util.strEmpty(windowBlkJSX)){
           windowAlert(windowBlkJSX, message);
          }
          else{
            alert(message);
          }
          trimLengthMinBlkJSX.setValue("1");
          trimLengthMinBlkJSX.focus();
          return false;
        }
      }
      if (trimLengthHr.match(/[^0-9]/)) {
        if(!jsx3.util.strEmpty(windowBlkJSX)){
         windowAlert(windowBlkJSX, hrMessage);
        }
        else{
          alert(hrMessage);
        }
        trimLengthHrBlkJSX.setValue("0");
        trimLengthHrBlkJSX.focus();
        return false;
      }
      else if (!jsx3.util.strEmpty(trimLengthHr) && (parseInt(trimLengthHr, 10) > 24 || parseInt(trimLengthHr, 10) < 0)) {
        if(!jsx3.util.strEmpty(windowBlkJSX)){
         windowAlert(windowBlkJSX, messageHrsTxt);
        }
        else{
          alert(messageHrsTxt);
        }
        trimLengthHrBlkJSX.setValue("0");
        trimLengthMinBlkJSX.setValue("1");
        trimLengthHrBlkJSX.focus();    
        return false;
      }
      else if (!jsx3.util.strEmpty(trimLengthHr) && parseInt(trimLengthHr, 10) == 24) {
        if (!jsx3.util.strEmpty(trimLengthMin) && parseInt(trimLengthMin, 10) > 0) {
           if(!jsx3.util.strEmpty(windowBlkJSX)){
            windowAlert(windowBlkJSX, messageHrsTxt);
           }
           else{
             alert(messageHrsTxt);
           }
           trimLengthMinBlkJSX.setValue("0");
           trimLengthMinBlkJSX.focus();    
           return false;
        }
      }
       if (jsx3.util.strEmpty(trimLengthMin) && !jsx3.util.strEmpty(trimLengthHr) && parseInt(trimLengthHr, 10) > 0) {
         trimLengthMin = 0;
      }
      else{
        trimLengthMin = parseInt(trimLengthMin, 10) * 60 * 1000;
      }
      if(!jsx3.util.strEmpty(trimLengthHr)){
        trimLengthHr = parseInt(trimLengthHr, 10) * 60 * 60 * 1000;
      }
      else{
        trimLengthHr = 0;
      }
      var trimLength = parseInt(trimLengthMin, 10) + parseInt(trimLengthHr, 10);
      return trimLength;
    }

    this.setPVRTrimLengthValues = function(trimLength){
      if( jsx3.util.strEmpty(trimLength) ){
        this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue("0");
        this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue("1");
      }
      else{
        trimLengthMin = parseInt(trimLength, 10) / (60 * 1000);
        if(trimLengthMin < 60){
          this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue(trimLengthMin);
        }
        else{
          trimLengthHr = parseInt(trimLength, 10) / (60 * 60 * 1000);
          trimLengthHr = Math.floor(trimLengthHr)
          if(trimLengthHr >= 1 && trimLengthHr <= 24){
            this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue(trimLengthHr);
            var trimLengthMin = parseInt(trimLength, 10) - (parseInt(trimLengthHr, 10) * (60 * 60 * 1000));
            trimLengthMin = parseInt(trimLengthMin, 10) / (60 * 1000);
            this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue(trimLengthMin);
          }
        }
      }
    }

    this.setPVREnable = function(enablePVR){
      if (enablePVR == "true") {
        this.chkEnablePVR.BlkJSX.setChecked("1");
        this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      } else {
        this.chkEnablePVR.BlkJSX.setChecked("0");
        this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
    }

    this.getEnablePVRTrimLength = function(){
      var trimLength = "";
      var trimLengthMin = this.cmpTxtSessionMinPVRTrimLength.BlkJSX.getValue();
      var trimLengthHr = this.cmpTxtSessionHrPVRTrimLength.BlkJSX.getValue();
      if(jsx3.util.strEmpty(trimLengthMin) && jsx3.util.strEmpty(trimLengthHr)){
        trimLength = 60 * 60 * 1000;
      }
      else{
        if(jsx3.util.strEmpty(trimLengthHr)){
          trimLength = parseInt(trimLengthMin, 10) * 60 * 1000;
        }
        else{
          if(!jsx3.util.strEmpty(trimLengthMin)){
            trimLengthMin = parseInt(trimLengthMin, 10) * 60 * 1000;
          }
          else{
            trimLengthMin = 0;
          }
          trimLengthHr = parseInt(trimLengthHr, 10) * 60 * 60 * 1000;
          trimLength = parseInt(trimLengthMin, 10) + parseInt(trimLengthHr, 10);
        }
      }
      if(trimLength > 86400000){
        this.validatePVRTrimLength();
      }
      return trimLength;
    }

    this.getDisablePVRTrimLength = function(){
      this.chkEnablePVR.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
}