/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* An object-oriented class to handle AOP functions defined in aopconfig.xml
*/
jsx3.Class.defineClass('com.ipvs.util.Common', jsx3.lang.Object, null, function(Common, Common_prototype) {

  /** @private @jsxobf-clobber */
  Common._LOG = jsx3.util.Logger.getLogger("com.ipvs.util.Common");

  /**
  * The instance initializer.
  */
  Common_prototype.init = function() {
    this._LOG.info("Common::init");
  }

  /**
  * Asynchronize set and paint a child block to its parent block;
  * Call as com.ipvs.util.Common.asyncSetChild(parentBlock,childBlock); 
  * Return value have to be get as arguments[0]
  * @param cb {JASON} 
  * @return the wrapped function {Function}
  */
  Common.asyncSetChild = jsx3.$Y(function(cb) {
    var parentBlk = cb.args()[0];
    var childBlk = cb.args()[1];
    if (parentBlk.setChild(childBlk)) {
      parentBlk.paintChild(childBlk);
      //childBlk must be passed into 
      var loadChecking = function(childBlk) {
        if (childBlk.getRendered()) {
          //load successful,return childBlk handle
          //get the handle within .when function as: childBlk = arguments[0];
          cb.done(childBlk);
        }
        else {
          var thisObj = this;
          setTimeout(function() { loadChecking.call(thisObj, childBlk); }, 20);
        }
      };
      loadChecking.call(this, childBlk);
    }
  });

  
});