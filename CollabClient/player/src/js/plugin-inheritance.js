/* Simple JavaScript Inheritance
 * By John Resig http://ejohn.org/
 * MIT Licensed.
 */
// Inspired by base2 and Prototype
(function(){
  var initializing = false, fnTest = /xyz/.test(function(){xyz;}) ? /\b_super\b/ : /.*/;
 
  // The base Class implementation (does nothing)
  this.Class = function(){};
 
  // Create a new Class that inherits from this class
  Class.extend = function extender(prop) {
    var _super = this.prototype;
   
    // Instantiate a base class (but only create the instance,
    // don't run the init constructor)
    initializing = true;
    var prototype = new this();
    initializing = false;
   
    // Copy the properties over onto the new prototype
    for (var name in prop) {
      // Check if we're overwriting an existing function
      prototype[name] = typeof prop[name] == "function" &&
        typeof _super[name] == "function" && fnTest.test(prop[name]) ?
        (function(name, fn){
          return function() {
            var tmp = this._super;
           
            // Add a new ._super() method that is the same method
            // but on the super-class
            this._super = _super[name];
           
            // The method only need to be bound temporarily, so we
            // remove it when we're done executing
            var ret = fn.apply(this, arguments);        
            this._super = tmp;
           
            return ret;
          };
        })(name, prop[name]) :
        prop[name];
    }
   
    // The dummy class constructor
    function Class() {
      // All construction is actually done in the init method
      if ( !initializing && this.init ) {
        this.pluginName = arguments[2];
        this.init.apply(this, arguments);
      }
    }
   
    // Populate our constructed prototype object
    Class.prototype = prototype;
   
    // Enforce the constructor to be what we expect
    Class.prototype.constructor = Class;
 
    // And make this class extendable
    Class.extend = extender;
   
    Class.prototype.log = function (lev, meth, instance, data) {
      var pluginName = this.pluginName;
      var options = this.options;
      if (options == null || options.divId == null) { options = {divId:""}; }
      // if (pluginName == null) { pluginName = 'UNKNOWN_CONTEXT'; }
      BarcoUtil.logger(lev, options.divId, pluginName, meth,
        instance, data);
    };

    return Class;
  };
})();

/* 
 * Method to register a Class as a jQuery plugin
*/
jQuery.plugin = function(name, object) {
	// Start a plugin
	jQuery.fn[name] = function(options) {
		var args = Array.prototype.slice.call(arguments, 1);
		var methodRtn = undefined;
		var cascadeRtn = this.each(function() {
			var instance = jQuery.data(this, name);
			if (instance) {
				// Call a method name on the plugin instance
				// (Not sure how this works - seems like 'options' should be a string = method name?)
				// (I suppose we could test this - if Object - then call init)
				// (This would presumably allow a re-init of an instance - is this relevant??) 
				methodRtn  = instance[options].apply(instance, args);
			} else {
				// Create a new Class instance (which will also call init) - 'this' refers to the element
				// Save the instance in the element's data store
				instance = jQuery.data(this, name, new object(options, this, name));
			}
		});
		return (methodRtn !== undefined) ? methodRtn : cascadeRtn;
	};
};

/* Combined Usage Pattern:

// MyObject - an object representing a concept/widget
var MyObject = Class.extend({
	init: function(options, elem) {
		// Mix in the passed-in options with the default options
		this.options = jQuery.extend({}, this._defaults, options);

		// Save the element reference, both as a jQuery
		// reference and a normal reference
		this.elem = elem;
		this.$elem = jQuery(elem);

		// Build the DOM's initial structure
		this._build();						
	},
	_defaults: {
		var1: defValue1, 
		var2: defValue2
	},
	_build: function() {
		// create DOM structure as needed
		// use .html or .append as appropriate 
		// - mark added DOM elements with 'myObject' class for later destroy
		var child = jQuery('<h1>'+this.options.var1+'</h1>').addClass('myObject');
		this.$elem.append(child);
		
		// add nested plugins / classes
		this.$elem.myChildPlugin(this.options);
		
		// subscribe to custom events
		this.$elem.bind( "myEvent", function( e ) {
			this.log(bDEBUG, "_build", "event","event");
		});

		this.log(bINFO, "_build", "return", "Done");
	},
	destroy: function(pluginName) {
		// unsubscribe to events
		this.$elem.unbind( "myEvent", function(e){
			this.log(bDEBUG,
			  "destroy", "unbind","unsubscribed to this event");
		});
		
		// clean out DOM structure
		// destroy child plugins / classes
		this.$elem.myChildPlugin('destroy', 'myChildPlugin');
		// destroy self
		this.$elem.find('.myObject').remove();
		this.$elem.removeData(pluginName);
	},
	myMethod: function(msg) {
		// You have direct access to the associated and cached jQuery element
		this.$elem.append('<p>'+msg+'</p>').addClass('myObject');
	}
});
   
// MyExtObject - an object that extends/inherit from MyObject 
var MyExtObject = MyObject.extend({
	init: function(options, elem) {
		// extend the defaults
		this._defaults = jQuery.extend(this._defaults, {
			var3:defValue3
		});
		// call the base class method
		this._super(options, elem);
	},
	_build: function() {
		// This method should override / extend base class
		this._super();
	},
});
   
// expose Objects as jQuery plugins
jQuery.plugin('myObject', MyObject);
jQuery.plugin('myExtObject', MyExtObject);
   
// Now use them
jQuery(document).ready(function() {

	// instantiate and use a myObject via jQuery
	jQuery('#somediv').myPlugin({ var1: 5. divId: 'divId' });
	
	// access the assoc object
	var myObject = jQuery('#somediv').data('myPlugin');
	BarcoUtil.log(myObject.option.var1);
	
	// call a method
	jQuery('#somediv').myPlugin('myMethod', 'Hello');

	// instantiate and use a myObject directly
	var myObject2 = new myObject({ var1: 20 }, '#somediv2');
	BarcoUtil.log(myObject2.options.var1);
	myObject2.myMethod('World');
	
	//Publishing event notifications
	jQuery('#somediv').trigger("myEvent");
	
	//Clean up/destory myObject via jQuery
	jQuery('#somediv').myPlugin('destory', 'myPlugin');
	
	//Clean up/destory myObject directly
	myObject.destory('myPlugin');

});
   
 */

