/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * Skinner - Class definition 
 * - registered as a jQuery plugin
 */
var Skinner = Class.extend({
  init: function(options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    // Init state
    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.cssInsertPos = 0;
    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
  },
  _initSkinValidationRules: function() {
    this.log(bDEBUG, "_initSkinValidationRules", "call", "");
    this._skinValidationRules =
      {global: {
          fontColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]},
          lineColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]},
          lineWidth: {func: this._borderValidate, context: null}
        },
        controlBar: {
          backgroundColor: {func: this._colorValidate, context: ["skinObj", "global", "fontColor"]},
          backgroundImage: {func: this._imageValidate, context: {minWidth: 1, maxWidth: 2880, minHeight: 1, maxHeight: 128}},
          toolTip: {
            fontColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "toolTip", "backgroundColor"]},
            backgroundImage: {func: this._imageValidate, context: {minWidth: 55, maxWidth: 60, minHeight: 20, maxHeight: 32}}
          },
          currentPos: {
            fontColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]}
          },
          scubberBar: {
            borderColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "backgroundColor"]},
            borderWidth: {func: this._borderValidate, context: null},
            progressBarFillColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "backgroundColor"]},
            backgroundColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "borderColor"]}
          },
          scubberHandle: {
            borderColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scubberHandle", "fillColor"]},
            borderWidth: {func: this._borderValidate, context: null},
            fillColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scubberHandle", "borderColor"]}
          },
          duration: {
            fontColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]}
          },
          bookmarkBar: {
            bookmarkIconImageOn: {func: this._imageValidate, context: {minWidth: 8, maxWidth: 16, minHeight: 16, maxHeight: 20}},
            bookmarkIconImageOff: {func: this._imageValidate, context: {minWidth: 8, maxWidth: 16, minHeight: 16, maxHeight: 20}}
          },
          bookmarkPanel: {
            backgroundColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]},
            inputBackgroundColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "bookmarkPanel", "inputTextColor"]},
            inputTextColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "bookmarkPanel", "inputBackgroundColor"]}
          },
          trimEditBar: {
            trimBarInProgressColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "progressBarFillColor"]},
            trimBarAddColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "progressBarFillColor"]},
            trimBarDeleteColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "scrubberBar", "progressBarFillColor"]},
            fontColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "cutlistBar", "backgroundColor"]},
            backgroundColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "cutlistBar", "fontColor"]},
            trimUndoIconImage: {func: this._imageValidate, context: {minWidth: 20, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimCancelImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimCancelImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimStartImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimStartImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimEndImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimEndImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimDoneImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimDoneImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimDeleteImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimDeleteImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveImageDisabled: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveNewImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveNewImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveNewImageDisabled: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveContImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}},
            trimSaveContImageHover: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 120, minHeight: 16, maxHeight: 32}}
          },
          zoomPanel: {
            backgroundColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "backgroundColor"]},
            borderColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "zoomPanel", "backgroundColor"]},
            textColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "zoomPanel", "backgroundColor"]},
            textColorHover: {func: this._colorValidate, context: ["skinObj", "controlBar", "zoomPanel", "backgroundColor"]},
            selectedColor: {func: this._colorValidate, context: ["skinObj", "controlBar", "zoomPanel", "backgroundColor"]},
            notchOffset: {func: this._borderValidate, context: null}
          },
          buttonBar: {
            playStartIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            stopIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            recordIconImageOn: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            recordIconImageOff: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            resumeIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            pauseIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            skipForwardIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            skipBackwardIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            catchupIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            bookmarkAddIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            bookmarkEditIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            bookmarkDeleteIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            trimIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            shareIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            snapShotIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            kbmOnIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            kbmOffIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            zoomIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            nativeResIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            fitWindowIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            fullscreenIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            normalscreenIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            muteIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            unmuteIconImage: {func: this._imageValidate, context: {minWidth: 36, maxWidth: 60, minHeight: 16, maxHeight: 32}},
            pinIconImage: {func: this._imageValidate, context: {minWidth: 18, maxWidth: 24, minHeight: 16, maxHeight: 32}},
            unpinIconImage: {func: this._imageValidate, context: {minWidth: 18, maxWidth: 24, minHeight: 16, maxHeight: 32}}
          }
        }
      };
  },
  _skinCSSRules:
    {global: {
        fontColor: [
          {sel:".tooltip", sty:"color: {{0}};"},
          {sel:".controller-time1", sty:"color: {{0}};"},
          {sel:".controller-time2", sty:"color: {{0}};"},
          {sel:".trimMediaContainer", sty:"color: {{0}};"}
        ],
        lineColor: [
          {sel:".ui-state-default", sty:"border-color: {{0}};"},
          {sel:".ui-widget-content", sty:"border-color: {{0}};"},
          {sel:".ui-widget-header", sty:"border-color: {{0}};"},
          {sel:".ui-widget-content .ui-state-default", sty:"border-color: {{0}};"},
          {sel:".bookmarks-container", sty:"border-color: {{0}};"}
        ],
        lineWidth: [
          {sel:".ui-state-default", sty:"border-width: {{0}};"},
          {sel:".ui-widget-content", sty:"border-width: {{0}};"},
          {sel:".ui-widget-header", sty:"border-width: {{0}};"},
          {sel:".ui-widget-content .ui-state-default", sty:"border-width: {{0}};"},
          {sel:".bookmarks-container", sty:"border-width: {{0}};"}
        ]
      },
      controlBar: {
        backgroundColor: [{sel:".button-overlay", sty:"background-color: {{0}};"}],
        backgroundImage: [
          {sel:".button-overlay", sty:"background-image: {{2}}; background-repeat: repeat;"},
          {sel:".button-overlay", sty:"background-color: transparent;"}],
        toolTip: {
          fontColor: [{sel:".tooltip", sty:"color: {{0}};"}],
          backgroundImage: [{sel:".scrubber_tooltip", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}]
        },
        currentPos: {
          fontColor: [{sel:".controller-time1", sty:"color: {{0}};"}]
        },
        scubberBar: {
          borderColor: [{sel:".ui-widget-content", sty:"border-color: {{0}};"}],
          borderWidth: [{sel:".ui-widget-content", sty:"border-width: {{0}};"}],
          progressBarFillColor: [{sel:".ui-slider-progressbar", sty:"background: {{0}};"}],
          backgroundColor: [{sel:".ui-widget-content", sty:"background: {{0}};"}]
        },
        scubberHandle: {
          borderColor: [{sel:".ui-widget-content .ui-state-default", sty:"border-color: {{0}};"}],
          borderWidth: [{sel:".ui-widget-content .ui-state-default", sty:"border-width: {{0}};"}],
          fillColor: [{sel:".ui-widget-content .ui-state-default", sty:"background: {{0}};"}]
        },
        duration: {
          fontColor: [{sel:".controller-time2", sty:"color: {{0}};"}]
        },
        bookmarkBar: {
          bookmarkIconImageOn: [{sel:".bookmark-on", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          bookmarkIconImageOff: [{sel:".bookmark-off", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}]
        },
        bookmarkPanel: {
          backgroundColor: [{sel:".bookmarkPopup", sty:"background-color: {{0}};"}],
          inputBackgroundColor: [{sel:".bookmarkPopup input", sty:"background-color: {{0}}"}],
          inputTextColor: [{sel:".bookmarkPopup input", sty:"color: {{0}}"}]
        },
        trimEditBar: {
          trimBarInProgressColor: [{sel:".segmentInProgress", sty:"background: {{0}};"}],
          trimBarAddColor: [{sel:".segmentAdd", sty:"background: {{0}};"}],
          trimBarDeleteColor: [{sel:".segmentDelete", sty:"background: {{0}};"}],
          fontColor: [{sel:".trimMediaContainer", sty:"color: {{0}};"}],
          backgroundColor: [{sel:".trimMediaContainer", sty:"background: {{0}};"}],
          trimUndoIconImage: [{sel:".undoTrimBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimCancelImage: [{sel:".cancelTrimBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimCancelImageHover: [{sel:".cancelTrimBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimStartImage: [{sel:".setTrimStartBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimStartImageHover: [{sel:".setTrimStartBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimEndImage: [{sel:".setTrimEndBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimEndImageHover: [{sel:".setTrimEndBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimDoneImage: [{sel:".setTrimDoneBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimDoneImageHover: [{sel:".setTrimDoneBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimDeleteImage: [{sel:".deleteTrimBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimDeleteImageHover: [{sel:".deleteTrimBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveImage: [{sel:".saveTrimBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveImageHover: [{sel:".saveTrimBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveImageDisabled: [{sel:".saveTrimBtn-dsbl", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveNewImage: [{sel:".saveTrimNewFileBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveNewImageHover: [{sel:".saveTrimNewFileBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveNewImageDisabled: [{sel:".saveTrimNewFileBtn-dsbl", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveContImage: [{sel:".saveTrimContinueBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimSaveContImageHover: [{sel:".saveTrimContinueBtn-enabled:hover", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}]
        },
        zoomPanel: {
          backgroundColor: [{sel:".zoomPopup ul", sty:"background-color: {{0}};"},
                            {sel:".zoomPopup li", sty:"background-color: {{0}};"}],
          borderColor: [{sel:".zoomNotch", sty:"border-top: 6px solid {{0}};"},
                        {sel:".zoomPopup ul", sty:"border:solid 1px {{0}};"}],
          textColor: [{sel:".zoomPopup a", sty:"color: {{0}};"}],
          textColorHover: [{sel:".zoomPopup a:hover", sty:"color: {{0}};"}],
          selectedColor: [{sel:".zoomPopup li.zoomItemSelected", sty:"background-color: {{0}};"}],
          notchOffset: [{sel:".zoomNotch", sty:"left: {{0}};"}]
        },
        buttonBar: {
          playStartIconImage: [{sel:".playBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          stopIconImage: [{sel:".stopBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          recordIconImageOn: [{sel:".recordingBtn-on", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          recordIconImageOff: [{sel:".recordingBtn-off", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          resumeIconImage: [{sel:".resumeBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          pauseIconImage: [{sel:".pauseBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          skipForwardIconImage: [{sel:".skipForwardBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          skipBackwardIconImage: [{sel:".skipBackBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          catchupIconImage: [{sel:".catchupBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          bookmarkAddIconImage: [{sel:".addBookmarkBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          bookmarkEditIconImage: [{sel:".editBookmarkBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          bookmarkDeleteIconImage: [{sel:".deleteBookmarkBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          trimIconImage: [{sel:".trimBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          shareIconImage: [{sel:".shareBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          snapShotIconImage: [{sel:".snapBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          kbmOnIconImage: [{sel:".kbmBtn-On-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          kbmOffIconImage: [{sel:".kbmBtn-Off-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          zoomIconImage: [{sel:".zoomBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          nativeResIconImage: [{sel:".resnativeBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          fitWindowIconImage: [{sel:".resfitBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          fullscreenIconImage: [{sel:".fullscreenOnBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          normalscreenIconImage: [{sel:".fullscreenOffBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          muteIconImage: [{sel:".muteBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          unmuteIconImage: [{sel:".unmuteBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          pinIconImage: [{sel:".pinBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}],
          unpinIconImage: [{sel:".unpinBtn-enabled", sty:"width: {{0}}; height: {{1}}; background-image: {{2}};"}]
        }
      }
    },
  _colorValidate: function(color, context, propPath) {
    this.log(bDEBUG, "_colorValidate", "call", "(\"" + color + '\", [' + context + "], [" + propPath + "])");
    var valid = color.match(/^[a-z]{3,20}/i) !== null;
    valid = valid || color.match(/^#[\da-f]{3}$/i) !== null;
    valid = valid || color.match(/^#[\da-f]{6}$/i) !== null;
    valid = valid || color.match(/^rgb(\d{1,3},\d{1,3},\d{1,3})$/) !== null;
    // var valid = valid || color.match(/^rgba(\d{1,3},\d{1,3},\d{1,3},\d{1,3})$/) !== null;
    if (!valid)
      this.log(bWARNING, "_colorValidate", "", propPath + ":" + color + " IS NOT VALID");
    return valid;
  },
  _borderValidate: function(lineWidth, context, propPath) {
    this.log(bDEBUG, "_borderValidate", "call", "(\"" + lineWidth + '\", ' + context + ", [" + propPath + "])");
    if (typeof lineWidth === "number") {
      lineWidth = lineWidth.toString() + "px";
    }
    var valid = lineWidth.match(/^(medium|thin|thick|\d+px)$/i) !== null;
    if (!valid)
      this.log(bWARNING, "_borderValidate", "", propPath + ":" + lineWidth + " IS NOT VALID");
    return valid;
  },
  _imageValidate: function(image, context, propPath) {
    this.log(bDEBUG, "_imageValidate", "call", "(..., [" + propPath + "])");
    var valid = (image.width) && (typeof image.width === "number");
    valid = valid && image.height && (typeof image.height === "number");
    valid = valid && image.image && (typeof image.image === "string");
    if (!valid) {
      this.log(bWARNING, "_imageValidate", "", propPath + " " + "INVALID IMAGE OBJ STRUCTURE");
      return false;
    }
    valid = valid && (image.width >= context.minWidth) && (image.width <= context.maxWidth);
    if (!valid) {
      this.log(bWARNING, "_imageValidate", "", propPath + ",width:" + image.width + " OUTSIDE " + context.minWidth + "-" + context.maxWidth + " RANGE");
      return false;
    }
    valid = valid && (image.height >= context.minHeight) && (image.height <= context.maxHeight);
    if (!valid) {
      this.log(bWARNING, "_imageValidate", "", propPath + ",height:" + image.width + " OUTSIDE " + context.minHeight + "-" + context.maxHeight + " RANGE");
      return false;
    }
    valid = valid && (image.image.match(/^url\(data:image\/(png|jpeg|gif);base64,[\S]+\)$/) !== null);
    if (!valid)
      this.log(bWARNING, "_imageValidate", "", propPath + ",image INVALID BASE-64 ENCODING");
    return valid;
  },
  _build: function() {
    this.log(bDEBUG, "_build", "call", "");
    var customSkin = document.getElementById("customSkin");
    this.styleSheet = customSkin.sheet ? customSkin.sheet : customSkin.styleSheet;
    this._initSkinValidationRules();
    var valid = this._validateSkinObj();
    this.log(bINFO, "_build", "validation", "SKIN OBJ VALID: " + valid);
    if (valid)
      this._buildCustomCSS();
    this.log(bINFO, "_build", "return", "Done");
  },
  _validateProperty: function(prop, obj, propPath) {
    this.log(bDEBUG, "_validateProperty", "call", "(\"" + prop + "\", obj, [" + propPath + "])");
    var propPathFull = propPath.concat(prop);
    var valid = true;
    if (typeof obj === "object" && obj.image === undefined) {
      // Keep walking the skinObj tree until a leaf node is found
      for (var subProp in obj) {
        var subObj = obj[subProp];
        var valid = valid && this._validateProperty(subProp, subObj, propPathFull);
      }
    } else {
      // Look for corresponding validation rule in _skinValidationRules
      var propPathFullLen = propPathFull.length;
      var ruleObj = this._skinValidationRules;
      for (var i = 1; i < propPathFullLen; i++) {
        if (!ruleObj[propPathFull[i]]) {
          this.log(bWARNING, "_validateProperty", "", propPathFull + " NOT AN ALLOWED PROPERTY.")
          return false;
        }
        ruleObj = ruleObj[propPathFull[i]];
      }
      valid = ruleObj.func.call(this, obj, ruleObj.context, propPathFull);
    }
    return valid;
  },
  _validateSkinObj: function() {
    this.log(bDEBUG, "_validateSkinObj", "call", "");
    var valid = this._validateProperty("skinObj", this.options.customSkin.skinObj, []);
    return valid;
  },
  _insertCSSRule: function(selector, styleDef) {
    this.log(bDEBUG, "_insertCSSRule", "call", "(\"" + selector + "\", {" + styleDef + "})");
    if (this.styleSheet.insertRule) {
      this.styleSheet.insertRule(selector + " {" + styleDef + "}", this.cssInsertPos++);
    } else if (this.styleSheet.addRule) {
      this.styleSheet.addRule(selector, styleDef, this.cssInsertPos++);
    }
  },
  _substituteIntoCSSTemplate: function(values, sty) {
    this.log(bDEBUG, "_substituteIntoCSSTemplate", "call", "([" + values + "], " + sty + ")");
    var tmpl = sty;
    var valuesLen = values.length;
    for (var i = 0; i < valuesLen; i++) {
      var pat = new RegExp("\\{\\{" + i.toString() + "\\}\\}","g");
      tmpl = tmpl.replace(pat, values[i]);
    }
    return tmpl;
  },
  _processCSSRule: function(skinObj, ruleObj, propPathFull) {
    this.log(bDEBUG, "_processCSSRule", "call", "(\"" + skinObj + "\", ruleObj, [" + propPathFull + "])");
    var values = [];
    if (typeof skinObj === "string") {
      values.push(skinObj);
    } else if (typeof skinObj === "number") {
      values.push(skinObj.toString() + "px"); // assumes too much
    } else if (typeof skinObj === "object") {
      if (skinObj.image) {
        values.push(skinObj.width);
        values.push(skinObj.height);
        values.push(skinObj.image);
      }
    }
    var ruleCount = ruleObj.length;
    for (var i = 0; i < ruleCount; i++) {
      var rule = ruleObj[i];
      if (rule.sty) {
        var sty = this._substituteIntoCSSTemplate(values, rule.sty);
        this._insertCSSRule(rule.sel, sty);
      } else if (rule.attr) {
      }
    }
  },
  _mapSkinObjPropertyToCSS: function(prop, obj, propPath) {
    this.log(bDEBUG, "_mapSkinObjPropertyToCSS", "call", "(\"" + prop + "\", obj, [" + propPath + "])");
    var propPathFull = propPath.concat(prop);
    if (typeof obj === "object" && obj.image === undefined) {
      // Keep walking the skinObj tree until a leaf node is found
      for (var subProp in obj) {
        var subObj = obj[subProp];
        this._mapSkinObjPropertyToCSS(subProp, subObj, propPathFull);
      }
    } else {
      // Look for corresponding CSS rule in _skinCSSRules
      var propPathFullLen = propPathFull.length;
      var ruleObj = this._skinCSSRules;
      for (var i = 1; i < propPathFullLen; i++) {
        ruleObj = ruleObj[propPathFull[i]];
      }
      this._processCSSRule(obj, ruleObj, propPathFull);
    }
  },
  _buildCustomCSS: function() {
    this.log(bDEBUG, "_buildCustomCSS", "call", "");
    this._mapSkinObjPropertyToCSS("skinObj", this.options.customSkin.skinObj, []);

  },
  destroy: function(pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    this.$elem.removeData(pluginName);
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('skinner', Skinner);