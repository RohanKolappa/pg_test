if (window.com && com.ipvs.gui)
  delete com.ipvs.gui.Whiteboard;

jsx3.require("jsx3.vector.Block");

jsx3.Class.defineClass("com.ipvs.gui.Whiteboard", jsx3.vector.Block, null, function(Whiteboard, Whiteboard_prototype) {

  /**
  * The instance initializer. Takes the same params as a basic Block constructor
  * @param name {string} a unique name that identifies this object
  * @param left {int} left position (in pixels) of the object relative to its parent container
  * @param top {int} top position (in pixels) of the object relative to its parent container
  * @param width {int} width (in pixels) of the object
  * @param height {int} height (in pixels) of the object
  */
  Whiteboard_prototype.init = function() {
    jsx3.log("Whiteboard: init");
    // Simply call the base class - passing all args
    this.jsxsuper.apply(this, arguments);
  }

  /**
  * Drawing modes / tool defs
  */
  Whiteboard.TOOL_PENCIL = "pencil";
  Whiteboard.TOOL_TEXT = "text";
  Whiteboard.TOOL_OVAL = "oval";
  Whiteboard.TOOL_RECTANGLE = "rectangle";
  Whiteboard.TOOL_ERASER = "eraser";

  /**
  * This is called during the construction.
  * Calls the base class to get the base canvas
  * Appends a transparent background Rectangle to the root vector and sets up to trap MOUSE events on it.
  */
  Whiteboard_prototype.createVector = function() {
    // get the root canvas from the base class    
    var objCanvas = this.jsxsuper.apply(this, arguments);

    // get the size of the base canvas
    var w = objCanvas.getWidth();
    var h = objCanvas.getHeight();
    jsx3.log("Whiteboard: createVector: w=" + w + " h=" + h + "");

    // append a rectangle to fill the whole canvas
    // use fill alpha = 0 to make it transparent
    this._objBackground = new jsx3.vector.Rectangle(0, 0, w, h);
    this._objBackground.setFill(new jsx3.vector.Fill(0xFFFF00, 0));
    objCanvas.appendChild(this._objBackground);

    // set default drawing tool/mode to pencil
    this.setTool(Whiteboard.TOOL_PENCIL);

    // setup handlers for MOUSE events
    this.paintEventHandler(jsx3.gui.Event.MOUSEDOWN, "handleMouseDown", this._objBackground);
    this.paintEventHandler(jsx3.gui.Event.MOUSEMOVE, "handleMouseMove", this._objBackground);
    this.paintEventHandler(jsx3.gui.Event.MOUSEUP, "handleMouseUp", this._objBackground);

    return objCanvas;
  }

  /**
  * Utility function to help resolve the MOUSE event XY relative to the Top-Left of this object
  * Updates 'this.leftX' and 'this.topY' by recursing up thru the DOM tree and adding up all the offsets
  */
  Whiteboard_prototype._getTopLeft = function() {
    this._leftX = 0;
    this._topY = 0;
    var objHTML = this.getRendered();
    if (objHTML) {
      this._leftX = objHTML.offsetLeft;
      this._topY = objHTML.offsetTop;
      var body = document.getElementsByTagName('body')[0];
      while (objHTML.offsetParent && objHTML != body) {
        this._leftX += objHTML.offsetParent.offsetLeft;
        this._topY += objHTML.offsetParent.offsetTop;
        objHTML = objHTML.offsetParent;
      }
      this._leftX += document.body.scrollLeft;
      this._topY += document.body.scrollTop;
    }
  }

  /**
  * on MOUSEDOWN - start drawing
  */
  Whiteboard_prototype.handleMouseDown = function(objEvent) {
    this._getTopLeft();
    var x = objEvent.clientX() - this._leftX;
    var y = objEvent.clientY() - this._topY;
    jsx3.log("Whiteboard: tool: " + this._tool + " handleMouseDown: x=" + x + " y=" + y + "");

    this.startDrawing(x, y);
  }

  /**
  * on MOUSEMOVE - update drawing
  */
  Whiteboard_prototype.handleMouseMove = function(objEvent) {
    var x = objEvent.clientX() - this._leftX;
    var y = objEvent.clientY() - this._topY;
    //    jsx3.log("Whiteboard: tool: "+this._tool+" handleMouseMove: x=" + x + " y=" + y + "");

    // draw slightly 'behind' the cursor to allow the MOUSEUP event to be seen by the background
    this.updateDrawing(x - 10, y - 10);
  }

  /**
  *  on MOUSEUP - stop drawing
  */
  Whiteboard_prototype.handleMouseUp = function(objEvent) {
    var x = objEvent.clientX() - this._leftX;
    var y = objEvent.clientY() - this._topY;
    jsx3.log("Whiteboard: tool: " + this._tool + " handleMouseUp: x=" + x + " y=" + y + "");

    this.stopDrawing(x, y);
  }

  /**
  *  Sets the current drawing tool / mode
  */
  Whiteboard_prototype.setTool = function(selTool) {
    // turn off any drawing in progress
    this.stopDrawing();
    // set the current drawing tool
    this._tool = selTool;
  }

  /**
  * Starts the drawing
  * Pencil Mode: Appends a new 'Shape' to the Vector. 
  */
  Whiteboard_prototype.startDrawing = function(x, y) {
    var objCanvas = this.getCanvas();
    var w = objCanvas.getWidth();
    var h = objCanvas.getHeight();

    if (this.objNewLine == null) {
      switch (this._tool) {
        case Whiteboard.TOOL_PENCIL:
          this.objNewLine = new jsx3.vector.Shape(null, 0, 0, w, h);
          this.objNewLine.pathMoveTo(x, y, false);
          break;
        case Whiteboard.TOOL_OVAL:
          this.objNewLine = new jsx3.vector.Oval(x, y, Math.round(w / 10), Math.round(h / 10));
          break;
        case Whiteboard.TOOL_RECTANGLE:
          this.objNewLine = new jsx3.vector.Rectangle(x, y, Math.round(w / 10), Math.round(h / 10));
          break;
        default:
          break;
      }
      this.objNewLine.setStroke(new jsx3.vector.Stroke(0x990000));
      objCanvas.appendChild(this.objNewLine);

    }
  }

  /**
  * Draws incrementally based on mouse position
  * Pencil Mode: Adds 'pathLineTo' segments to the currently open 'Shape'.
  */
  Whiteboard_prototype.updateDrawing = function(x, y) {
    var objCanvas = this.getCanvas();

    if (this.objNewLine != null) {
      switch (this._tool) {
        case Whiteboard.TOOL_PENCIL:
          this.objNewLine.pathLineTo(x, y, false);
          break;
        case Whiteboard.TOOL_OVAL:
        case Whiteboard.TOOL_RECTANGLE:
          var startX = this.objNewLine.getLeft();
          var startY = this.objNewLine.getTop();
          this.objNewLine.setDimensions(startX, startY,
                                            (x - startX) > 0 ? (x - startX) : 0,
                                            (y - startY) > 0 ? (y - startY) : 0);
          break;
        default:
          break;
      }
      jsx3.vector.updateVector(this.objNewLine, objCanvas);
      this.repaint();
    }
  }

  /**
  *  Turns off drawing to the current 'Shape'
  */
  Whiteboard_prototype.stopDrawing = function() {
    if (this.objNewLine != null) {
      this.objNewLine = null;
    }
  }
});