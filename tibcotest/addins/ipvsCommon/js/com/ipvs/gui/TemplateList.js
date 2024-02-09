/*



(function(ButtonPanel,ButtonPanel_prototype) {

jsx3.gui.Template.unregisterLibrary(ButtonPanel.jsxclass.getName());

ButtonPanel.jsxclass._JSXFLUID = false;

ButtonPanel.jsxclass._JSXRECALC = false;

ButtonPanel.jsxclass._JSXTEMPLATE_INITED = true;

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onScroll",function() {
return "onScroll";
},{type:"event",name:"onscroll"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onClick",function() {
return "onClick";
},{type:"event",name:"onclick"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onKeyDown",function() {
return "onKeyDown";
},{type:"event",name:"onkeydown"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","buttonid",function() {
return this.buttonid;
},{type:"attribute",name:"id",triggers:["buttonid"]});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onMouseOver",function() {
return "onMouseOver";
},{type:"event",name:"onmouseover"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onMouseOut",function() {
return "onMouseOut";
},{type:"event",name:"onmouseout"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onMouseDown",function() {
return "onMouseDown";
},{type:"event",name:"onmousedown"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","onMouseUp",function() {
return "onMouseUp";
},{type:"event",name:"onmouseup"});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","buttonimg",function() {
return this.buttonimg;
},{type:"attribute",name:"src",triggers:["buttonimg"]});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","buttondevpresence",function() {
return this.buttondevpresence;
},{type:"attribute",name:"src",triggers:["buttondevpresence"]});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","buttondevstate",function() {
return this.buttondevstate;
},{type:"attribute",name:"src",triggers:["buttondevstate"]});

jsx3.gui.Template.addResolver("com.ipvs.gui.ButtonPanel","buttondevtype",function() {
return this.buttondevtype;
},{type:"attribute",name:"src",triggers:["buttondevtype"]});

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$id",jsx3.gui.Template.getResolver("jsx3.gui.Template","$id"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$margin",jsx3.gui.Template.getResolver("jsx3.gui.Template","$margin"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$height",jsx3.gui.Template.getResolver("jsx3.gui.Template","$height"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$width",jsx3.gui.Template.getResolver("jsx3.gui.Template","$width"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$top",jsx3.gui.Template.getResolver("jsx3.gui.Template","$top"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$left",jsx3.gui.Template.getResolver("jsx3.gui.Template","$left"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","jsx3.gui.Template","$position",jsx3.gui.Template.getResolver("jsx3.gui.Template","$position"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onScroll",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onScroll"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onClick",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onClick"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onKeyDown",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onKeyDown"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","buttonid",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","buttonid"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onMouseOver",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onMouseOver"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onMouseOut",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onMouseOut"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onMouseDown",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onMouseDown"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","onMouseUp",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","onMouseUp"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","buttonimg",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","buttonimg"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","buttondevpresence",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","buttondevpresence"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","buttondevstate",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","buttondevstate"));

jsx3.gui.Template._registerLocalResolver("com.ipvs.gui.ButtonPanel","com.ipvs.gui.ButtonPanel","buttondevtype",jsx3.gui.Template.getResolver("com.ipvs.gui.ButtonPanel","buttondevtype"));

ButtonPanel_prototype._paint =function(_jsxpaintprofile, _jsxboxprofile) {
try {
var MODE = 'paint';var _output = [];var jsxdrawspacetarget;var jsxdrawspace = {};
_output.push(_jsxboxprofile["/inlinebox"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div"].paintOpen(_jsxpaintprofile,this));
var a1 = this.getIterableRecords();
var i2 = -1;
_jsxforcecascade = typeof(_jsxforcecascade) == 'undefined' ? 1 : _jsxforcecascade;
while(a1.hasNext()) {jsxdrawspace.target = a1.next();
i2++;

var buttonid = _jsxpaintprofile["buttonid"] = this.getId() + '_' + jsxdrawspace.target.getAttribute('jsxid');
var buttontarget = _jsxpaintprofile["buttontarget"] = jsxdrawspace.target;
var buttonimg = _jsxpaintprofile["buttonimg"] = this.getCurrentImageURL(jsxdrawspace.target.getAttribute('jsxselected'));
var buttondevpresence = _jsxpaintprofile["buttondevpresence"] = jsxdrawspace.target.getAttribute('jsxdevpresence') || ButtonPanel.BUTTON_PRESENCE_DEFAULT;
var buttondevtitle = _jsxpaintprofile["buttondevtitle"] = jsxdrawspace.target.getAttribute('jsxtext') || ButtonPanel.BUTTON_TITLE_DEFAULT;
var buttondevstate = _jsxpaintprofile["buttondevstate"] = jsxdrawspace.target.getAttribute('jsxdevstate') || ButtonPanel.BUTTON_STATE_DEFAULT;
var buttondevtype = _jsxpaintprofile["buttondevtype"] = jsxdrawspace.target.getAttribute('jsxdevtype') || ButtonPanel.BUTTON_TYPE_DEFAULT;
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/img"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/img"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span/img"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span/img"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[2]"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[2]"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]/img"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]/img"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]/img"].paintOpen(_jsxpaintprofile,this));
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]/img"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div/span"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span/div"].paintClose());
_output.push(_jsxboxprofile["/inlinebox/div/for-each/span"].paintClose());
}
_output.push(_jsxboxprofile["/inlinebox/div"].paintClose());
_output.push(_jsxboxprofile["/inlinebox"].paintClose());
return _output.join('');
} catch(e) { jsx3.log("There is a JavaScript error in the '_paint' method related to code generated by the template definition for the object, "+ this + ":\n" + e + "\nFor more information on this error, precompile the template and insert log statements where appropriate (see: jsx3.gui.Template.precompile).");}
};

ButtonPanel_prototype._createBoxProfile = function(_jsxdrawspace, _jsxpaintprofile) {
try {
var MODE = 'create';var _jsxboxprofile = {};var jsxdrawspacetarget;
var jsxbox;
var _drawspace = jsxdrawspace = jsx3.clone(_jsxdrawspace);
jsxbox = _jsxboxprofile["/inlinebox"] = new jsx3.gui.Template.Box("/inlinebox");
jsxbox.addAttribute("$id");
jsxbox.setStaticAtts('');
jsxbox.addProperty("tagname","inlinebox",false);
jsxbox.addProperty("margin",_jsxpaintprofile["$margin"],"$margin");
jsxbox.addProperty("height",_jsxpaintprofile["$height"],"$height");
jsxbox.addProperty("width",_jsxpaintprofile["$width"],"$width");
jsxbox.addProperty("top",_jsxpaintprofile["$top"],"$top");
jsxbox.addProperty("left",_jsxpaintprofile["$left"],"$left");
jsxbox.addProperty("position",_jsxpaintprofile["$position"],"$position");
jsxbox.setNodeDepth(0);
jsxbox.setNodePath("");
var _drawspace1 = jsxdrawspace = jsxbox.compile(_drawspace).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div"] = new jsx3.gui.Template.Box("/inlinebox/div");
jsxbox.setStaticStyles("overflow:auto;");
jsxbox.setName("buttonpanel");
jsxbox.addEvent("onScroll");
jsxbox.addEvent("onClick");
jsxbox.addEvent("onKeyDown");
jsxbox.setStaticAtts('jsxtype="buttonpanel" ');
jsxbox.addProperty("tagname","div",false);
jsxbox.addProperty("height",jsxdrawspace.parentheight,false);
jsxbox.addProperty("width",jsxdrawspace.parentwidth,false);
jsxbox.addProperty("top",0,false);
jsxbox.addProperty("left",0,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(1);
jsxbox.setNodePath("0");
var _drawspace2 = jsxdrawspace = jsxbox.compile(_drawspace1).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span");
jsxbox.setStaticStyles("cursor:pointer;");
jsxbox.addAttribute("buttonid");
jsxbox.setName("button");
jsxbox.addEvent("onMouseOver");
jsxbox.addEvent("onMouseOut");
jsxbox.addEvent("onMouseDown");
jsxbox.addEvent("onMouseUp");
jsxbox.setStaticAtts('jsxtype="button" title="button tip" ');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("height",40,false);
jsxbox.addProperty("width",130,false);
jsxbox.addProperty("position","relative",false);
jsxbox.setNodeDepth(2);
jsxbox.setNodePath("0/0/0");
var _drawspace3 = jsxdrawspace = jsxbox.compile(_drawspace2).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div");
jsxbox.setStaticAtts('');
jsxbox.addProperty("tagname","div",false);
jsxbox.addProperty("height",40,false);
jsxbox.addProperty("position","relative",false);
jsxbox.setNodeDepth(3);
jsxbox.setNodePath("0/0/0/0");
var _drawspace4 = jsxdrawspace = jsxbox.compile(_drawspace3).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/img"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/img");
jsxbox.addAttribute("buttonimg");
jsxbox.setStaticAtts('width="130" height="40" ');
jsxbox.addProperty("tagname","img",false);
jsxbox.addProperty("empty",true,false);
jsxbox.setNodeDepth(4);
jsxbox.setNodePath("0/0/0/0/0");
var _drawspace5 = jsxdrawspace = jsxbox.compile(_drawspace4).getClientDrawspace();
var jsxdrawspace = _drawspace4;
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span");
jsxbox.setStaticAtts('');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("top",0,false);
jsxbox.addProperty("left",0,false);
jsxbox.addProperty("height",40,false);
jsxbox.addProperty("width",130,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(4);
jsxbox.setNodePath("0/0/0/0/1");
var _drawspace5 = jsxdrawspace = jsxbox.compile(_drawspace4).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span");
jsxbox.setStaticStyles("overflow:hidden;");
jsxbox.setName("presence");
jsxbox.setStaticAtts('jsxtype="presence" ');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("top",3,false);
jsxbox.addProperty("left",2,false);
jsxbox.addProperty("height",16,false);
jsxbox.addProperty("width",16,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(5);
jsxbox.setNodePath("0/0/0/0/1/0");
var _drawspace6 = jsxdrawspace = jsxbox.compile(_drawspace5).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span/img"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span/img");
jsxbox.addAttribute("buttondevpresence");
jsxbox.setStaticAtts('width="16" height="16" ');
jsxbox.addProperty("tagname","img",false);
jsxbox.addProperty("empty",true,false);
jsxbox.setNodeDepth(6);
jsxbox.setNodePath("0/0/0/0/1/0/0");
var _drawspace7 = jsxdrawspace = jsxbox.compile(_drawspace6).getClientDrawspace();
var jsxdrawspace = _drawspace6;
var jsxdrawspace = _drawspace5;
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[2]"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span[2]");
jsxbox.setStaticStyles("font-size:12px;overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis;");
jsxbox.setName("title");
jsxbox.setStaticAtts('jsxtype="title" ');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("top",3,false);
jsxbox.addProperty("left",21,false);
jsxbox.addProperty("width",106,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(5);
jsxbox.setNodePath("0/0/0/0/1/1");
var _drawspace6 = jsxdrawspace = jsxbox.compile(_drawspace5).getClientDrawspace();
var jsxdrawspace = _drawspace5;
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span[3]");
jsxbox.setStaticStyles("overflow:hidden;");
jsxbox.setName("state");
jsxbox.setStaticAtts('jsxtype="state" ');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("top",22,false);
jsxbox.addProperty("left",3,false);
jsxbox.addProperty("height",16,false);
jsxbox.addProperty("width",16,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(5);
jsxbox.setNodePath("0/0/0/0/1/2");
var _drawspace6 = jsxdrawspace = jsxbox.compile(_drawspace5).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[3]/img"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span[3]/img");
jsxbox.addAttribute("buttondevstate");
jsxbox.setStaticAtts('width="16" height="16" ');
jsxbox.addProperty("tagname","img",false);
jsxbox.addProperty("empty",true,false);
jsxbox.setNodeDepth(6);
jsxbox.setNodePath("0/0/0/0/1/2/0");
var _drawspace7 = jsxdrawspace = jsxbox.compile(_drawspace6).getClientDrawspace();
var jsxdrawspace = _drawspace6;
var jsxdrawspace = _drawspace5;
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span[4]");
jsxbox.setStaticStyles("overflow:hidden;");
jsxbox.setName("type");
jsxbox.setStaticAtts('jsxtype="type" ');
jsxbox.addProperty("tagname","span",false);
jsxbox.addProperty("top",22,false);
jsxbox.addProperty("left",22,false);
jsxbox.addProperty("height",16,false);
jsxbox.addProperty("width",35,false);
jsxbox.addProperty("position","absolute",false);
jsxbox.setNodeDepth(5);
jsxbox.setNodePath("0/0/0/0/1/3");
var _drawspace6 = jsxdrawspace = jsxbox.compile(_drawspace5).getClientDrawspace();
jsxbox = _jsxboxprofile["/inlinebox/div/for-each/span/div/span/span[4]/img"] = new jsx3.gui.Template.Box("/inlinebox/div/for-each/span/div/span/span[4]/img");
jsxbox.addAttribute("buttondevtype");
jsxbox.setStaticAtts('width="35" height="16" ');
jsxbox.addProperty("tagname","img",false);
jsxbox.addProperty("empty",true,false);
jsxbox.setNodeDepth(6);
jsxbox.setNodePath("0/0/0/0/1/3/0");
var _drawspace7 = jsxdrawspace = jsxbox.compile(_drawspace6).getClientDrawspace();
var jsxdrawspace = _drawspace6;
var jsxdrawspace = _drawspace5;
var jsxdrawspace = _drawspace4;
var jsxdrawspace = _drawspace3;
var jsxdrawspace = _drawspace2;
var jsxdrawspace = _drawspace1;
var jsxdrawspace = _drawspace;
return _jsxboxprofile;
} catch(e) { jsx3.log("There is a JavaScript error in the '_createBoxProfile' method related to code generated by the template definition for the object, "+ this + ":\n" + e + "\nFor more information on this error, precompile the template and insert log statements where appropriate (see: jsx3.gui.Template.precompile).");}
};

ButtonPanel_prototype._updateBoxProfileImpl = function(_jsxdrawspace, _jsxpaintprofile, _jsxupdatequeue, _jsxobjgui, _jsxforcecascade) {
try {
var MODE = 'update';var _drawspace = jsxdrawspace = _jsxdrawspace;
var _objgui = _jsxobjgui;var jsxdrawspacetarget;
var jxbox, jsxcascade;
jsxbox = this.getBoxByName("/inlinebox");
jsxbox.updateProperty("margin",_jsxpaintprofile["$margin"],true,jsxdrawspace);
jsxbox.updateProperty("height",_jsxpaintprofile["$height"],true,jsxdrawspace);
jsxbox.updateProperty("width",_jsxpaintprofile["$width"],true,jsxdrawspace);
jsxbox.updateProperty("top",_jsxpaintprofile["$top"],true,jsxdrawspace);
jsxbox.updateProperty("left",_jsxpaintprofile["$left"],true,jsxdrawspace);
jsxbox.updateProperty("position",_jsxpaintprofile["$position"],true,jsxdrawspace);
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui1 = _objgui;
var _drawspace1 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div");
var _objgui1 = (_objgui1) ? jsx3.html.DOM.selectSingleElm(_objgui1,"0") : null;
jsxbox.updateProperty("height",jsxdrawspace.parentheight,false,jsxdrawspace);
jsxbox.updateProperty("width",jsxdrawspace.parentwidth,false,jsxdrawspace);
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui1,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui2 = _objgui1;
var _drawspace2 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var a1 = this.getIterableRecords();
var i2 = -1;
_jsxforcecascade = typeof(_jsxforcecascade) == 'undefined' ? 1 : _jsxforcecascade;
while(a1.hasNext()) {jsxdrawspace.target = a1.next();
i2++;

var buttonid = _jsxpaintprofile["buttonid"] = this.getId() + '_' + jsxdrawspace.target.getAttribute('jsxid');
var buttontarget = _jsxpaintprofile["buttontarget"] = jsxdrawspace.target;
var buttonimg = _jsxpaintprofile["buttonimg"] = this.getCurrentImageURL(jsxdrawspace.target.getAttribute('jsxselected'));
var buttondevpresence = _jsxpaintprofile["buttondevpresence"] = jsxdrawspace.target.getAttribute('jsxdevpresence') || ButtonPanel.BUTTON_PRESENCE_DEFAULT;
var buttondevtitle = _jsxpaintprofile["buttondevtitle"] = jsxdrawspace.target.getAttribute('jsxtext') || ButtonPanel.BUTTON_TITLE_DEFAULT;
var buttondevstate = _jsxpaintprofile["buttondevstate"] = jsxdrawspace.target.getAttribute('jsxdevstate') || ButtonPanel.BUTTON_STATE_DEFAULT;
var buttondevtype = _jsxpaintprofile["buttondevtype"] = jsxdrawspace.target.getAttribute('jsxdevtype') || ButtonPanel.BUTTON_TYPE_DEFAULT;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span");
var _objgui2 = (_objgui2) ? jsx3.html.DOM.selectSingleElm(_objgui2,i2) : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui2,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui3 = _objgui2;
var _drawspace3 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div");
var _objgui3 = (_objgui3) ? jsx3.html.DOM.selectSingleElm(_objgui3,"0") : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui3,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui4 = _objgui3;
var _drawspace4 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/img");
var _objgui4 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"0") : null;
jsxbox.recalculate(jsxdrawspace,_objgui4,_jsxforcecascade);
var _drawspace5 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var jsxdrawspace = jsx3.clone(_drawspace4);
var _objgui4 = _objgui3;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span");
var _objgui4 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"1") : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui4,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui5 = _objgui4;
var _drawspace5 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span");
var _objgui5 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"0") : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui5,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui6 = _objgui5;
var _drawspace6 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span/img");
var _objgui6 = (_objgui6) ? jsx3.html.DOM.selectSingleElm(_objgui6,"0") : null;
jsxbox.recalculate(jsxdrawspace,_objgui6,_jsxforcecascade);
var _drawspace7 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var jsxdrawspace = jsx3.clone(_drawspace6);
var _objgui6 = _objgui5;
}
var jsxdrawspace = jsx3.clone(_drawspace5);
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[2]");
var _objgui5 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"1") : null;
jsxbox.recalculate(jsxdrawspace,_objgui5,_jsxforcecascade);
var _drawspace6 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var jsxdrawspace = jsx3.clone(_drawspace5);
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[3]");
var _objgui5 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"2") : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui5,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui6 = _objgui5;
var _drawspace6 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[3]/img");
var _objgui6 = (_objgui6) ? jsx3.html.DOM.selectSingleElm(_objgui6,"0") : null;
jsxbox.recalculate(jsxdrawspace,_objgui6,_jsxforcecascade);
var _drawspace7 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var jsxdrawspace = jsx3.clone(_drawspace6);
var _objgui6 = _objgui5;
}
var jsxdrawspace = jsx3.clone(_drawspace5);
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[4]");
var _objgui5 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"3") : null;
if((jsxcascade = jsxbox.recalculate(jsxdrawspace,_objgui5,_jsxforcecascade)) && (jsxcascade.w||jsxcascade.h||_jsxforcecascade)) {
var _objgui6 = _objgui5;
var _drawspace6 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[4]/img");
var _objgui6 = (_objgui6) ? jsx3.html.DOM.selectSingleElm(_objgui6,"0") : null;
jsxbox.recalculate(jsxdrawspace,_objgui6,_jsxforcecascade);
var _drawspace7 = jsxbox.getClientDrawspace();
var jsxdrawspace = jsxbox.getClientDrawspace();
var jsxdrawspace = jsx3.clone(_drawspace6);
var _objgui6 = _objgui5;
}
var jsxdrawspace = jsx3.clone(_drawspace5);
var _objgui5 = _objgui4;
}
var jsxdrawspace = jsx3.clone(_drawspace4);
var _objgui4 = _objgui3;
}
var jsxdrawspace = jsx3.clone(_drawspace3);
var _objgui3 = _objgui2;
}
var jsxdrawspace = jsx3.clone(_drawspace2);
var _objgui2 = _objgui1;
}
}
var jsxdrawspace = jsx3.clone(_drawspace1);
var _objgui1 = _objgui;
}
} catch(e) { jsx3.log("There is a JavaScript error in the '_updateBoxProfileImpl' method related to code generated by the template definition for the object, "+ this + ":\n" + e + "\nFor more information on this error, precompile the template and insert log statements where appropriate (see: jsx3.gui.Template.precompile).");}
};

ButtonPanel_prototype._getClientDimensions = function(_jsxdrawspace, _jsxpaintprofile, _targetchild, _jsxboxprofile) {
try {
var MODE = 'dimension';var jsxbox;var jsxdrawspacetarget;
return jsx3.gui.Template.getEmptyDrawspace();
} catch(e) { jsx3.log("There is a JavaScript error in the '_getClientDimensions' method related to code generated by the template definition for the object, "+ this + ":\n" + e + "\nFor more information on this error, precompile the template and insert log statements where appropriate (see: jsx3.gui.Template.precompile).");}
};

ButtonPanel_prototype._paintChild = function(_targetchild, _jsxpaintprofile) {
try {
var MODE = "paintchild";
var jsxdrawspace = {};
var _jsxobjgui = this.getRendered();
var _objgui = _jsxobjgui;
var jsxdrawspacetarget;
var jxbox;
jsxbox = this.getBoxByName("/inlinebox");
var _objgui = _objgui;
jsxbox = this.getBoxByName("/inlinebox/div");
var _objgui1 = (_objgui) ? jsx3.html.DOM.selectSingleElm(_objgui,"0") : null;
var buttonid = _jsxpaintprofile["buttonid"] = this.getId() + '_' + jsxdrawspace.target.getAttribute('jsxid');
var buttontarget = _jsxpaintprofile["buttontarget"] = jsxdrawspace.target;
var buttonimg = _jsxpaintprofile["buttonimg"] = this.getCurrentImageURL(jsxdrawspace.target.getAttribute('jsxselected'));
var buttondevpresence = _jsxpaintprofile["buttondevpresence"] = jsxdrawspace.target.getAttribute('jsxdevpresence') || ButtonPanel.BUTTON_PRESENCE_DEFAULT;
var buttondevtitle = _jsxpaintprofile["buttondevtitle"] = jsxdrawspace.target.getAttribute('jsxtext') || ButtonPanel.BUTTON_TITLE_DEFAULT;
var buttondevstate = _jsxpaintprofile["buttondevstate"] = jsxdrawspace.target.getAttribute('jsxdevstate') || ButtonPanel.BUTTON_STATE_DEFAULT;
var buttondevtype = _jsxpaintprofile["buttondevtype"] = jsxdrawspace.target.getAttribute('jsxdevtype') || ButtonPanel.BUTTON_TYPE_DEFAULT;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span");
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div");
var _objgui3 = (_objgui2) ? jsx3.html.DOM.selectSingleElm(_objgui2,"0") : null;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/img");
var _objgui4 = (_objgui3) ? jsx3.html.DOM.selectSingleElm(_objgui3,"0") : null;
var _objgui4 = _objgui3;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span");
var _objgui4 = (_objgui3) ? jsx3.html.DOM.selectSingleElm(_objgui3,"1") : null;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span");
var _objgui5 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"0") : null;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span/img");
var _objgui6 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"0") : null;
var _objgui6 = _objgui5;
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[2]");
var _objgui5 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"1") : null;
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[3]");
var _objgui5 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"2") : null;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[3]/img");
var _objgui6 = (_objgui5) ? jsx3.html.DOM.selectSingleElm(_objgui5,"0") : null;
var _objgui6 = _objgui5;
var _objgui5 = _objgui4;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[4]");
var _objgui5 = (_objgui4) ? jsx3.html.DOM.selectSingleElm(_objgui4,"3") : null;
jsxbox = this.getBoxByName("/inlinebox/div/for-each/span/div/span/span[4]/img");
} catch(e) { jsx3.log("There is a JavaScript error in the '_paintChild' method related to code generated by the template definition for the object, "+ this + ":\n" + e + "\nFor more information on this error, precompile the template and insert log statements where appropriate (see: jsx3.gui.Template.precompile).");}
};

ButtonPanel_prototype.onbeforeinit = function() {
;
};

ButtonPanel_prototype.oninit = function() {
;
};

ButtonPanel_prototype.onbeforepaint = function() {
;
};

ButtonPanel_prototype.onpaint = function() {
;
};

ButtonPanel_prototype.onbeforeresize = function() {
;
};

ButtonPanel_prototype.onresize = function() {
;
};

ButtonPanel_prototype.onbeforeresizechild = function() {
;
};

})(ButtonPanel,ButtonPanel_prototype);











================================================================================================
jsx3.require("jsx3.gui.Template","jsx3.xml.Cacheable","jsx3.xml.CDF");

jsx3.lang.Class.defineClass("my.Grid",jsx3.gui.Template.Block,[jsx3.xml.Cacheable,jsx3.xml.CDF],function(GRID,grid) {

  //init
  grid.init = function(strName) {
    this.jsxsuper(strName);
  };

 // defaults
  grid.columns = {};
  grid.columns.items = [{attribute:"jsxtext",caption:"Text",width:"*"}];

 // template xml
  grid.getTemplateXML = function() {
    return ['',
    '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">' ,
    '  <model>' ,
    '    <var id="mask_value" triggers="beforeedit">return this. getTargetProfile().value;</var>' ,
    '    <var id="mask_left" triggers="beforeedit">return isNaN(this. getTargetProfile().L) ? -100 : this. getTargetProfile().L;</var>' ,
    '    <var id="mask_top" triggers="beforeedit">return isNaN(this. getTargetProfile().T) ? -18 : this. getTargetProfile().T;</var>' ,
    '    <var id="mask_width" triggers="beforeedit">return this. getTargetProfile().W || 100;</var>' ,
    '    <var id="mask_height" triggers="beforeedit">return this. getTargetProfile().H || 18;</var>' ,
    '    <var id="mask_display" triggers="beforeedit afteredit">return this. getTargetProfile().display || "none";</var>' ,
    '  </model>' ,
    '  <template dom="static">' ,
    '    <inlinebox style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">' ,
           //scrollable data rows
    '      <div u:id="datarows" style="position:absolute;left:0px;top:20px;width:100%;height:100%-20px;overflow:auto;z-index:1;" onscroll="{onscroll}">',
                          //textbox edit mask
    '        <input u:id="txt_editmask" type="text" class="" value="{mask_value}" onkeydown="{onkeydown}" onblur="{onblur}" ',
    '          style="position:absolute;padding:3 0 0 1;background-color:#ffffff;z-index:2;color:blue;left:{mask_left};top:{mask_top};width:{mask_width};height:{mask_height};font-family:{$fontname};font-size:{$fontsize};display:{mask_display};border:solid 1px gray;" />',
                          //exit the template early if the mask is targeting a cell; when that happens, merely show the mask above and exit early
    '        <if test="this._getTargetProfile().target"><return/></if>' ,
    '        <table u:protected="true" u:id="datarows_table" index="0" onmousedown="{onmousedown}" style="position:absolute;left:0px;top:0px;width:100%;font-size:{$fontsize};font-family:{$fontname};table-layout:fixed;" cellspacing="0" border="0"><tbody u:id="tb_b" u:protected="true">' ,
    '          <for-each select="this.getIterableRecords()">' ,
    '            <var id="rowid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '            <var id="rowtarget">$$target</var>',
    '            <tr id="{rowid}" u:id="tr_b" u:protected="true">',
    '              <for-each select="this.getIterableColumns()">' ,
    '                <var id="celltext">rowtarget.getAttribute($$target.attribute) || "&amp;#160;"</var>',
    '                <var id="columnwidth">$$target.width</var>',
    '                <td u:id="td_b" u:protected="true" style="width:{columnwidth};padding:3px;border-right:solid 1px lightblue;border-bottom:solid 1px lightblue;"><text>{celltext}</text></td>',
    '              </for-each>',
    '            </tr>',
    '          </for-each>',
    '        </tbody></table>',
    '      </div>',
            //fixed header row
    '      <div u:id="header" style="position:absolute;left:0px;top:0px;width:100%;height:20px;border:{myborder|solid 1px gray};z-index:2;background-color:{headerbg};color:{headercolor};overflow:hidden;">',
    '        <table u:protected="true" u:id="header_tbl" style="position:absolute;left:0px;top:0px;width:100%;font-size:{$fontsize};font-weight:bold;table-layout:fixed;" cellpadding="3" cellspacing="0" border="0"><tbody u:id="tb_h" u:protected="true">' ,
    '          <tr u:id="tr_h" u:protected="true">',
    '            <for-each select="this.getIterableColumns()">' ,
    '              <var id="headertext">$$target.caption</var>',
    '              <var id="columnwidth">$$target.width</var>',
    '              <td u:id="td_h" u:protected="true" style="width:{columnwidth};white-space:nowrap;text-align:center;">',
    '                <text>{headertext}</text>',
    '              </td>',
    '            </for-each>',
    '          </tr>',
    '        </tbody></table>',
    '      </div>',
    '    </inlinebox>' ,
    '  </template>' ,
    '</transform>'].join("");
  };

  // 6) define the CONTROLLER functions
  grid.onmousedown = function(objEvent,objGUI) {
    this.onbeforeedit(objEvent,objEvent.srcElement().parentNode,objEvent.srcElement());
  };

 grid.onscroll = function(objEvent,objGUI) {
    //called when the data rows are scrolled
    this.getRenderedBox("header_tbl").style.left = -objGUI.scrollLeft + "px";
  };

 grid.onblur = function(objEvent,objGUI) {
    //handle the primitive event (onblur) with the prototype (instance) event
    this.onafteredit(objEvent,objGUI);
  };

 grid.onkeydown = function(objEvent,objGUI) {
    var objCell = this._getTargetProfile().target;
    var intCellIndex = objCell.cellIndex
    var objRow;

    if((objEvent.enterKey() && !objEvent.shiftKey()) || objEvent.downArrow())  {
      //commit the value; advance edit to the next cell down
      this.onafteredit(objEvent,objGUI);

      //if the current row isn't the last row, apply edit mask to the next row down
      objRow = (objCell.parentNode != objCell.parentNode.parentNode.lastChild) ?
        objCell.parentNode.nextSibling : objCell.parentNode.parentNode.firstChild;
    } else if(objEvent.upArrow() || (objEvent.enterKey() && objEvent.shiftKey())) {
      //commit the value; advance edit to the next cell up
      this.onafteredit(objEvent,objGUI);

      //if the current row isn't the first row, apply edit mask to the next row up;
      objRow = (objCell.parentNode != objCell.parentNode.parentNode.firstChild) ?
        objCell.parentNode.previousSibling : objCell.parentNode.parentNode.lastChild;
    } else if(objEvent.rightArrow())  {
      //commit the value; advance edit to the next cell down
      this.onafteredit(objEvent,objGUI);
      var objRow = objCell.parentNode;
      intCellIndex = objRow.lastChild == objCell ? 0 : intCellIndex+1;
    } else if(objEvent.leftArrow()) {
      //commit the value; advance edit to the next cell up
      this.onafteredit(objEvent,objGUI);
      var objRow = objCell.parentNode;
      intCellIndex = objRow.firstChild==objCell ?objRow.lastChild.cellIndex :
        intCellIndex-1;
    }

    //begin the edit session for the target cell
    if(objRow)
      this.onbeforeedit(objEvent,objRow,objRow.childNodes[intCellIndex]);
  };

 grid._getTargetProfile = function() {
    //when an edit event happens, a target profile is created that describes the context
    return this._jsxtargetprofile || {};
  };

 grid._setTargetProfile = function(objP) {
    //when an edit event happens, a target profile is created that describes the context
    this._jsxtargetprofile = objP;
  };


  grid.onbeforeedit = function(objEvent,objRow,objCell) {
    //use a sleep delay to stop repeated clicks and key strokes from taxing performance
    jsx3.sleep(function() {
        this._onbeforeedit(objEvent,objRow,objRow.childNodes[objCell.cellIndex]);
    },"my.GRID",this);
  };

 grid._onbeforeedit = function(objEvent,objRow,objCell) {
    //get the id for the row that was clicked
    var strCdfId = objRow.getAttribute("id").substr(this.getId().length+1);
    var strAttName = this.columns.items[objCell.cellIndex].attribute;
    var strValue = this.getRecordNode(strCdfId).getAttribute(strAttName);

    //allow the before-edit event to be cancelled
    var objReturn = this.doEvent(jsx3.gui.Interactive.BEFORE_EDIT,
        {objEVENT:objEvent,strID:strCdfId,objCELL:objCell,strVALUE:strValue});
    if(objReturn !== false) {
      //determine information about the target cell being edited (left, top, etc)
      var objThis = this.getRendered(objRow);
      var objDataContainer = this.getRenderedBox("datarows",objThis);
      var objMask = this.getRenderedBox("txt_editmask",objThis);

      //query the system for the location of the target table cell
      var objPos = jsx3.html.getRelativePosition(objDataContainer,objCell);
      //when running on firefox, builds earlier than 3.6.1 have a bug
      if(!(jsx3.getVersion() < "3.6.1" && /fx/.test(jsx3.CLASS_LOADER.getType()))) {
        objPos.L = objPos.L + objDataContainer.scrollLeft;
        objPos.T = objPos.T + objDataContainer.scrollTop;
      }

      objPos.value = strValue || "";
      objPos.display = "block";
      objPos.target = objCell;
      objPos.id = strCdfId;
      objPos.attribute = strAttName;

      //cache the information about the target
      this._setTargetProfile(objPos);
      this.syncProperty("beforeedit",true);
      //give cursor focus to the edit mask (the text input)
      objMask.focus();
    }
  };

 grid.onafteredit = function(objEvent,objGUI) {
    //get the profile object
    var objP = this._getTargetProfile();

    //get the new value entered by the user
    var objReturn;
    if(objP.value != objGUI.value) {
      //allow the edit to be cancelled/modified
      objReturn = this.doEvent(jsx3.gui.Interactive.AFTER_EDIT,
          {objEVENT:objEvent,strID:objP.id,objCELL:objP.target,
          strVALUE:objGUI.value});
      if(objReturn !== false) {
        //update the data model
        this.insertRecordProperty(objP.id,objP.attribute,objGUI.value,false);

        //update the view
        objP.target.innerHTML = jsx3.util.strEmpty(objGUI.value) ?
            "&#160;" : objGUI.value;
      }
    }

    //reset the mask (hide it)
    this._setTargetProfile({value:""});
    this.syncProperty("afteredit",true);

    //fire the final commit event (not cancellable)
    if(objReturn !== false)
      this.doEvent(jsx3.gui.Interactive.AFTER_COMMIT,
         {objEVENT:objEvent,strID:objP.id,objCELL:objP.target,
         strVALUE:\(objReturn != null && objReturn.strVALUE ?
         objReturn.strVALUE : objGUI.value)});
  };

 grid.setColumns = function(arrColumns) {
    //call to change the columns to render for the table. The schema is as follows.:
    this.columns.items = arrColumns;
    //the GI serializer only saves scalar types
    this.encodeColumns(arrColumns);
    this.repaint();
  };

 grid._getColumns = function(arrColumns) {
    return this._columns || "";
  };

 grid.encodeColumns = function(arrColumns) {
    //serialize the columns array as a string so the GI class serializer
    var a = ;
    for(var i=0;i<arrColumns.length;i++) {
      var cur = arrColumns[i];
      a.push("{attribute:\"" + cur.attribute + "\",caption:" +
        jsx3.util.strEscapeJSON(cur.caption) + ",width:\"" + cur.width + "\"}");
    }
    this._columns = "[" + a.join(",") + "]";
  };

 grid.onAfterAssemble = function() {
    //when a serialized object is restored in GI (deserialized), this method is called;
    if(!jsx3.util.strEmpty(this._columns))
      this.columns.items = jsx3.eval(this._columns);
  };

 grid.getIterableRecords = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectNodeIterator("//record") :
    (new jsx3.util.List()).iterator();
  };

  grid.getIterableColumns = function() {
    return new jsx3.util.List(this.columns.items.length ?
    this.columns.items : ).iterator();
  };

});

*/