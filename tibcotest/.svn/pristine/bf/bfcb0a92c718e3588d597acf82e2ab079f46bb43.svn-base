/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* An object-oriented class to handle AOP functions defined in aopconfig.xml
*/
jsx3.Class.defineClass('com.ipvs.util.AOPHandler', jsx3.lang.Object, null, function(AOPHandler, AOPHandler_prototype) {

  /** @private @jsxobf-clobber */
  AOPHandler._LOG = jsx3.util.Logger.getLogger("com.ipvs.util.AOPHandler");
  AOPHandler_prototype.aopCfgNode = null;

  /** {String} @private */
  AOPHandler._AOPCfgXMLFile = "aop/aopconfig.xml";

  /**
  * The instance initializer.
  */
  AOPHandler_prototype.init = function() {
    AOPHandler._LOG.info("AOPHandler::init");
  }

  /**
  * Method to handle AOP advice defined in aopconfig.xml;
  * This is triggered by hot key Alt+Ctrl+Shift+A;
  * Dynamic load AOP configuration every hot key hit; 
  */
  AOPHandler_prototype.handleAdvice = function() {
    //reload AOP config file asynchronously once triggered by of Alt+Ctrl+Shift+A
    AOPHandler_prototype._ASYNC_loadAOPConfig().when(function(){
      var aopCfgNode = arguments[0];
      AOPHandler._LOG.info("AOPHandler::handleAdvice:aopCfgNode=" + aopCfgNode.getXML());
      if (aopCfgNode) {
          //load external AOP functions (defined in js files) which is separated by ','
          var aopFiles = aopCfgNode.getAttribute("aopfile");
          var aopFilesArr = aopFiles.split(/,/);
          var rtValArr = new Array(aopFilesArr.length);
          var rtTotalVal = null;
          for (var i = 0; i < aopFilesArr.length; i++) {
            //load external Javascript file (patch functions) when loading success
            if (i==0){
              rtTotalVal = AOPHandler_prototype._ASYNC_loadAOPCodeFile(aopFilesArr[i]);
            }
            else {
              rtTotalVal = rtTotalVal.and(AOPHandler_prototype._ASYNC_loadAOPCodeFile(aopFilesArr[i]));
            }
          }
          AOPHandler_prototype.aopCfgNode = aopCfgNode;
          AOPHandler._LOG.info("AOPHandler::handleAdvice:has " + rtValArr.length + " aopfile(s).");
          //after loading success, process pointcuts defined in aopconfig.xml
          rtTotalVal.when(AOPHandler_prototype.processAOPConfig);
        }
        else {
          AOPHandler._LOG.error("AOPHandler::handleAdvice: There is no 'aopconfig' node.");
        }
    });

  }

  /**
  * Go through all tgtclass nodes in aopconfig, asynchronous loading js class 
  * Loading success,process 'tgtclass' node.
  */
  AOPHandler_prototype.processAOPConfig = function() {
    var aopTgtclassNodeIter = AOPHandler_prototype.aopCfgNode.selectNodeIterator(".//tgtclass");
    if (aopTgtclassNodeIter) {
      //loop all tgtclass
      while (aopTgtclassNodeIter.hasNext()) {
        var aopTgtclassNode = aopTgtclassNodeIter.next();
 
        //tgtclass attribute
        var className = aopTgtclassNode.getAttribute("name");
        var asynRtVal = AOPHandler_prototype._ASYNC_requireJSClass(className, aopTgtclassNode);
        asynRtVal.when(function() {
          //Asynchronously pass initial context variables
          var lcClassName = arguments[0][0];
          var lcAopTgtclassNode = arguments[0][1];
          AOPHandler_prototype.processAOPTgtClass(lcClassName,lcAopTgtclassNode);
        });
      }
    }
    else {
      AOPHandler._LOG.error("AOPHandler::processAOPConfig:aopconfig doesn't have any 'tgtclass' child.");
    }
  }

  /**
  * Go through each tgtmethod node in a tgtclass, require js class defined in tgtclass. 
  * When requiring done,create one pointcut per tgtmethod, and apply attributes to it.
  */
  AOPHandler_prototype.processAOPTgtClass = function(className,aopTgtclassNode) {
    //'method' attributes
    var tgtmethodNodeIter = aopTgtclassNode.selectNodeIterator(".//tgtmethod");
    if (tgtmethodNodeIter) {
      while (tgtmethodNodeIter.hasNext()) {
        //one pointcut per method
        var tgtMethodNode = tgtmethodNodeIter.next();
        var pcName = tgtMethodNode.getAttribute("pcname");
        var aopEnable = tgtMethodNode.getAttribute("enable");
        if (aopEnable == 'false') {
          AOPHandler_prototype.deletePCByName(pcName);
          continue;
        }
        var methodName = tgtMethodNode.getAttribute("name");
        var aopType = tgtMethodNode.getAttribute("aoptype");
        var aopFnc = tgtMethodNode.getAttribute("aopfnc");
        AOPHandler._LOG.info("AOPHandler::processAOPTgtClass:calling addAndApplyPointcut: tgtclass(" + className + "):pcName:(" + pcName + "):aopFnc:(" + aopFnc + ")");
        //add & apply a pointcut by parameters
        AOPHandler_prototype.addAndApplyPointcut(pcName, className, methodName, aopType, aopFnc);
      }
    }
    else {
      AOPHandler._LOG.error("AOPHandler::processAOPTgtClass:aopconfig tgtclass(" + className + ") doesn't have 'tgtmethod' node.");
    }
  }

  /**
  * Create pointcut and applyt attributes to it.
  * @param pcName {String} the name of a pointcut.
  * @param className {String} the name of a class to be loaded.
  * @param methodName {String} the name of a method in above class.
  * @param aopType {String} the type of pointcut, including:'before','after','around'.
  * @param aopFnc {String} the function defined in external js file used to be excuted in AOP.
  */
  AOPHandler_prototype.addAndApplyPointcut = function(pcName, className, methodName, aopType, aopFnc) {
    //add a pointcut by pcname
    AOPHandler_prototype.addPCByName(pcName, { classes: className, methods: methodName });
    try {
      switch (aopType) {
        case "before":
          AOPHandler_prototype.addPCBefore(pcName, aopFnc);
          break;
        case "after":
          AOPHandler_prototype.addPCAfter(pcName, aopFnc);
          break;
        case "around":
          AOPHandler_prototype.addPCAround(pcName, aopFnc);
          break;
        default:
          //default value "after"
          AOPHandler_prototype.addPCAfter(pcName, aopFnc);
          break;
      }
    }
    catch (e) {
      AOPHandler._LOG.error("AOPHandler::addAndApplyPointcut: " + e.description);
      AOPHandler._LOG.error("AOPHandler::addAndApplyPointcut:Please check function (" + aopFnc + ") in external javascript file; pointcut (" + pcName + ") will not work.");
    }
  }

  /**
  * Asynchronous function for loading AOP configuration file.
  */
  AOPHandler_prototype._ASYNC_loadAOPConfig = jsx3.$Y(function(cb) {
    PNameSpace.unloadInclude(AOPHandler._AOPCfgXMLFile);
    //load configuration file
    var aopCfgData = loadXMLFile(AOPHandler._AOPCfgXMLFile);
    //get AOP config node
    var aopCfgNode = aopCfgData.getRootNode();
    cb.done(aopCfgNode);
  });

  /**
  * Asynchronous function for requiring js class.
  */
  AOPHandler_prototype._ASYNC_requireJSClass = jsx3.$Y(function(cb) {
    jsx3.require(cb.args()[0]);
    AOPHandler._LOG.info("AOPHandler::_ASYNC_requireJSClass:loading JS file '" + cb.args()[0] + "'.");
    cb.done(cb.args()); 
  });

  /**
  * Asynchronous function for loading external js file(s) which includ AOP functions.
  */
  AOPHandler_prototype._ASYNC_loadAOPCodeFile = jsx3.$Y(function(cb) {
    var aopFile = cb.args()[0];
    //try to remove existing script
    PNameSpace.unloadInclude(aopFile);
    var aopFileURI = "jsxuser:///" + aopFile;
    var u = jsx3.net.URI.valueOf(aopFileURI);
    var r = jsx3.net.Request.open("GET", u, true);
    r.subscribe("*", function() {
      if (r.getStatus() == 200) {
        AOPHandler._LOG.info("AOPHandler::_ASYNC_loadAOPCodeFile:filename=" + aopFile + " load successful.");
        cb.done(jsx3.eval(r.getResponseText()));
      } else {
        AOPHandler._LOG.info("AOPHandler::_ASYNC_loadAOPCodeFile:fail to load " + aopFile + " reason:status=" + r.getStatus());
      }
    });
    r.send();
  });


  /**
  * Set an AOP function before pointcut 'pcName'.
  * @param pcName {String} the name of a pointcut.
  * @param aopFnc {Function} the AOP function to be excuted before the pointcut.
  */
  AOPHandler_prototype.addPCBefore = function(pcName, aopFnc) {
    AOPHandler._LOG.info("AOPHandler::addPCBefore:pcName (" + pcName + "):aop function (" + aopFnc + ").");
    jsx3.AOP.before(pcName, eval(aopFnc));
  }

  /**
  * Set an AOP function after pointcut 'pcName'.
  * @param pcName {String} the name of a pointcut.
  * @param aopFnc {Function} the AOP function to be excuted after the pointcut.
  */
  AOPHandler_prototype.addPCAfter = function(pcName, aopFnc) {
    AOPHandler._LOG.info("AOPHandler::addPCAfter:pcName (" + pcName + "):aop function (" + aopFnc + ").");
    jsx3.AOP.after(pcName, eval(aopFnc));
  }

  /**
  * Set an AOP function around pointcut 'pcName'.
  * @param pcName {String} the name of a pointcut.
  * @param aopFnc {Function} the AOP function to be excuted around the pointcut.
  */
  AOPHandler_prototype.addPCAround = function(pcName, aopFnc) {
    AOPHandler._LOG.info("AOPHandler::addPCAround:pcName (" + pcName + "):aop function (" + aopFnc + ").");
    jsx3.AOP.around(pcName, eval(aopFnc));
  }

  /**
  * Add a pointcut by name.
  * @param pcName {String} the name of a pointcut.
  * @param pcJsonObj {String} the Json object defined as {classes:className,methods:methodName}.
  */
  AOPHandler_prototype.addPCByName = function(pcName, pcJsonObj) {
    AOPHandler._LOG.info("AOPHandler::addPCByName:Create a new pointcut (" + pcName + ") with classname:" + pcJsonObj.classes + " and method:" + pcJsonObj.methods);
    jsx3.AOP.pc(pcName, pcJsonObj);
  }

  /**
  * Delete a pointcut by name.
  * @param pcName {String} the name of a pointcut.
  */
  AOPHandler_prototype.deletePCByName = function(pcName) {
    AOPHandler._LOG.info("AOPHandler::deletePCByName:delete a pointcut (" + pcName + ").");
    jsx3.AOP.pcrem(pcName);
  }

});