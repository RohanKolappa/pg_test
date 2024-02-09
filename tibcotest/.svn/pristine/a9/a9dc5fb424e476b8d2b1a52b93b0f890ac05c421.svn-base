/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods to handle Class components.
 * Class components are classs defined for a block which sub components.
 */
jsx3.Class.defineInterface("com.ipvs.util.IPVSComponentI", null, function(IPVSComponentI, IPVSComponentI_prototype) {

  /** @private @jsxobf-clobber */
  IPVSComponentI._LOG = jsx3.util.Logger.getLogger(IPVSComponentI.jsxclass.getName());

  /**
  * Displays the Class component.
  */
  IPVSComponentI_prototype.cmpShow = function() {
    this.getParent().setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  /**
  * Hides the Class component.
  */
  IPVSComponentI_prototype.cmpHide = function() {
    this.getParent().setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  /**
  * Converts Media Size bytes to gigabytes.
  */
  IPVSComponentI_prototype.formatMediaFileSize = function(element, cdfkey, matrix, column, rownumber, server) {
    var gigaBytes = 0;
    var bytes = element.innerText;
    if(bytes > 0) {
      var Cbytes = bytes.match(/\d{1,}/);
      gigaBytes = Math.round((Cbytes / 1073741824) * 100) / 100;
    }
    element.innerHTML = gigaBytes;
  }

  /**
  * Converts Media Length milliseconds to hh:mm:ss.
  */
  IPVSComponentI_prototype.formatMediaFileTime = function(element, cdfkey, matrix, column, rownumber, server) {
    element.innerHTML = this.milliSec2HMS(element.innerText);
  }

  IPVSComponentI_prototype.milliSec2HMS = function(ms) {
    function two(x) { return ((x > 9) ? "" : "0") + x }
    function three(x) { return ((x > 99) ? "" : "0") + ((x > 9) ? "" : "0") + x }
    try {
      ms = ms.match(/\d{1,}/);
    } catch(e) {
    }
    var sec = Math.floor(ms / 1000)

    var min = Math.floor(sec / 60)
    sec = sec % 60
    // only ss no ms
    t = two(sec);

    var hr = Math.floor(min / 60)
    min = min % 60
    t = two(min) + ":" + t

    var day = Math.floor(hr / 60)
    hr = hr % 60
    t = two(hr) + ":" + t
    return t;
  }

  /**
  * Unserializes a PHP serialized datatype.
  * @param {string} serializedStr The serialized PHP data
  * @return mixed  The resulting datatype
  */
  IPVSComponentI_prototype.unSerializeStr = function(serializedStr) {
    var result = this._unSerialize(serializedStr);
    return result[0];
  }

  /** @private @jsxobf-clobber */
  IPVSComponentI_prototype._unSerialize = function(serializedStr) {
    var length = 0;
    switch(serializedStr.charAt(0)) {
      /**
      * Array
      */ 
      case 'a':
        length = this._unSerialize_GetLength(serializedStr);
        serializedStr = serializedStr.substr(String(length).length + 4);

        var arr = new Array();
        var key = null;
        var value = null;

        for(var i = 0;i < length;++i) {
          key = this._unSerialize(serializedStr);
          serializedStr = key[1];

          value = this._unSerialize(serializedStr);
          serializedStr = value[1];

          arr[key[0]] = value[0];
        }

        serializedStr = serializedStr.substr(1);
        return [arr, serializedStr];
        break;

      /**
      * Objects
      */ 
      case 'O':
        length = this._unSerialize_GetLength(serializedStr);
        var classname = String(serializedStr.substr(String(length).length + 4, length));

        serializedStr = serializedStr.substr(String(length).length + 6 + length);
        var numProperties = Number(serializedStr.substring(0, serializedStr.indexOf(':')))
        serializedStr = serializedStr.substr(String(numProperties).length + 2);

        var obj = new Object();
        var property = null;
        var value = null;

        for(var i = 0;i < numProperties;++i) {
          key = this._unSerialize(serializedStr);
          serializedStr = key[1];

          // Handle private/protected
          key[0] = key[0].replace(new RegExp('^\x00' + classname + '\x00'), '');
          key[0] = key[0].replace(new RegExp('^\x00\\*\x00'), '');

          value = this._unSerialize(serializedStr);
          serializedStr = value[1];

          obj[key[0]] = value[0];
        }

        serializedStr = serializedStr.substr(1);
        return [obj, serializedStr];
        break;

      /**
      * Strings
      */ 
      case 's':
        length = this._unSerialize_GetLength(serializedStr);
        return [String(serializedStr.substr(String(length).length + 4, length)), serializedStr.substr(String(length).length + 6 + length)];
        break;

      /**
      * Integers and doubles
      */ 
      case 'i':
      case 'd':
        var num = Number(serializedStr.substring(2, serializedStr.indexOf(';')));
        return [num, serializedStr.substr(String(num).length + 3)];
        break;

      /**
      * Booleans
      */ 
      case 'b':
        var bool = (serializedStr.substr(2, 1) == 1);
        return [bool, serializedStr.substr(4)];
        break;

      /**
      * Null
      */ 
      case 'N':
        return [null, serializedStr.substr(2)];
        break;

      /**
      * Unsupported
      */ 
      case 'o':
      case 'r':
      case 'C':
      case 'R':
      case 'U':
        alert('Error: Unsupported PHP data type found!');

        /**
        * Error
        */
      default:
        return [null, null];
        break;
    }
  }

  /** @private @jsxobf-clobber */
  IPVSComponentI_prototype._unSerialize_GetLength = function(strSerialized) {
    strSerialized = strSerialized.substring(2);
    var length = Number(strSerialized.substr(0, strSerialized.indexOf(':')));
    return length
  }


  //  /**
  //   * Subscribes an object or function to a type of event published by the Class components.
  //   * @param strEventId {String}.
  //   * @param objHandler {object/string/function}.
  //   * @param objFunction {function/string}.
  //   * @throws {jsx3.IllegalArgumentException} if objHandler/objFunction are not a valid combination of types.
  //   */
  //  IPVSComponentI_prototype.Subscribe = function(strEventId, objHandler, objFunction) {
  //    this.getServer().subscribe(strEventId, objHandler, objFunction);
  //  }

  //  /**
  //   * Publishes an event to all subscribed Class components.
  //   * @param objEvent {object} the event, should have at least a field 'subject' that is the event id, another common field is 'target' (target will default to this instance)
  //   * @throws {jsx3.IllegalArgumentException} objEvent is not an object with a <code>subject</code> property
  //   */
  //  IPVSComponentI_prototype.Publish = function(objEvent) {
  //    this.getServer().publish(objEvent);
  //  }

});