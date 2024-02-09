var showingList = [];

var gBaseURL = "";
// gBaseURL = "http://10.51.51.38";

var gAuthorization = "";
var gResource = "";
var gWaitURL = "";
var gClientTemplateTitle = "";
var gServiceVersion = "";

function updateLogin(value) {
	value = Base64.decode(value);
	var data = value.split(" ");
	gBaseURL = document.location.protocol + "//" + data[0];
	gAuthorization = Base64.encode(data[1]+":"+data[2]);
	gResource = data[3];
	var path ="/controller/Resource/Port?selectFormat=BASICURL&sortBy=title&sortBy=title&type=SrcPort&dstPortTitle=" + gResource;	
	sendAPIRequest(path, "Source", handleAPIResponse, "");
	// setTimeout(handleMenuSelected(), 3000);
}

function getFullURL(url) {
	var basePath = document.URL.replace("index.php", "");
	return basePath + "/" + url;
}

function getWaitURL() {
	return gWaitURL;
}

/////////////////////////////////////////////////////////////////
/////////       Logging, Feedback Backend
////////////////////////////////////////////////////////////////
function log(data) {
	display(data);
}

function error(data) {
	alert(data);
}

/////////////////////////////////////////////////////////////////
/////////       REST request backend
////////////////////////////////////////////////////////////////
function sendRESTRequest(request, url, method, data, accept) {
	var async = true; // to prevent blocking execution
	var contentType = "application/json;charset=UTF-8";
	
	url = gBaseURL + url;

	request.open(method, url, async);
	request.setRequestHeader("Content-Type", contentType);
	request.setRequestHeader("Accept", accept);
	request.setRequestHeader("X-Barco-resource", gResource);
	request.setRequestHeader("Authorization", "Basic " + gAuthorization);
	request.send(data);
}

function sendRequest(request, url, method, data) {
	var accept = document.mrform.accept.value;
	sendRESTRequest(request, url, method, data, accept);
}


/////////////////////////////////////////////////////////////////
/////////       Login/Logout backend
////////////////////////////////////////////////////////////////
//---------------------------- utility functions for base64--------------------------------------------
var Base64 = {
// private property
_keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",

// public method for encoding
encode : function (input) {
    var output = "";
    var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
    var i = 0;

    input = Base64._utf8_encode(input);

    while (i < input.length) {

        chr1 = input.charCodeAt(i++);
        chr2 = input.charCodeAt(i++);
        chr3 = input.charCodeAt(i++);

        enc1 = chr1 >> 2;
        enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
        enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
        enc4 = chr3 & 63;

        if (isNaN(chr2)) {
            enc3 = enc4 = 64;
        } else if (isNaN(chr3)) {
            enc4 = 64;
        }

        output = output +
        Base64._keyStr.charAt(enc1) + Base64._keyStr.charAt(enc2) +
        Base64._keyStr.charAt(enc3) + Base64._keyStr.charAt(enc4);

    }

    return output;
},

// private method for UTF-8 encoding
_utf8_encode : function (string) {
    string = string.replace(/\r\n/g,"\n");
    var utftext = "";

    for (var n = 0; n < string.length; n++) {

        var c = string.charCodeAt(n);

        if (c < 128) {
            utftext += String.fromCharCode(c);
        }
        else if((c > 127) && (c < 2048)) {
            utftext += String.fromCharCode((c >> 6) | 192);
            utftext += String.fromCharCode((c & 63) | 128);
        }
        else {
            utftext += String.fromCharCode((c >> 12) | 224);
            utftext += String.fromCharCode(((c >> 6) & 63) | 128);
            utftext += String.fromCharCode((c & 63) | 128);
        }

    }

    return utftext;
},

// public method for decoding
decode : function (input) {
    var output = "";
    var chr1, chr2, chr3;
    var enc1, enc2, enc3, enc4;
    var i = 0;

    input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");

    while (i < input.length) {

        enc1 = Base64._keyStr.indexOf(input.charAt(i++));
        enc2 = Base64._keyStr.indexOf(input.charAt(i++));
        enc3 = Base64._keyStr.indexOf(input.charAt(i++));
        enc4 = Base64._keyStr.indexOf(input.charAt(i++));

        chr1 = (enc1 << 2) | (enc2 >> 4);
        chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
        chr3 = ((enc3 & 3) << 6) | enc4;

        output = output + String.fromCharCode(chr1);

        if (enc3 != 64) {
            output = output + String.fromCharCode(chr2);
        }
        if (enc4 != 64) {
            output = output + String.fromCharCode(chr3);
        }

    }

    output = Base64._utf8_decode(output);

    return output;

},


// private method for UTF-8 decoding
_utf8_decode : function (utftext) {
    var string = "";
    var i = 0;
    var c = c1 = c2 = 0;

    while ( i < utftext.length ) {

        c = utftext.charCodeAt(i);

        if (c < 128) {
            string += String.fromCharCode(c);
            i++;
        }
        else if((c > 191) && (c < 224)) {
            c2 = utftext.charCodeAt(i+1);
            string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
            i += 2;
        }
        else {
            c2 = utftext.charCodeAt(i+1);
            c3 = utftext.charCodeAt(i+2);
            string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
            i += 3;
        }

    }
    return string;
}

}

function login(username, password, resource, serviceVersion) {
	log("login");
	//var url = "/controller/Service/Login";
	gAuthorization = Base64.encode(username+":"+password);
  gClientTemplateTitle = resource;
  gServiceVersion = serviceVersion;
	gResource = resource+"_"+username;
	doLogin();
//	var method = "POST";
//	var postData = "{\"ServiceLogin\": {\"clientPrivateKey\": \"\",\"clientTemplateTitle\": \"" + resource + "\",\"@clientVersion\": \""+ serviceVersion +"\"	}}";
//	var request = new XMLHttpRequest();	
//	function handler() {     
//		if (this.readyState == 4) {
//			if(this.status == 200) {
//        log("LOGIN 200 OK");
//        display(this.responseText);
//        keep_alive(this);
//        info("Getting Data ...");
//        handleLoginDone();
//      }
//      else if(this.status == 409) {
//        logout();
//        login(username, password, resource, serviceVersion);
//      }
//      else
//        error("LOGIN FAILED code=" + this.status + ":" + this.responseText);
//		}
//	}
//
//	request.onreadystatechange = handler;
//	sendRequest(request, url, method, postData);
//	info("Logging In ...");
}

function doLogin() {
	log("login");
	var url = "/controller/Service/Login";
	var method = "POST";
	var postData = "{\"ServiceLogin\": {\"clientPrivateKey\": \"\",\"clientTemplateTitle\": \"" + gClientTemplateTitle + "\",\"@clientVersion\": \""+ gServiceVersion +"\"	}}";
	var request = new XMLHttpRequest();	
	function handler() {     
		if (this.readyState == 4) {
			if(this.status == 200) {
        log("LOGIN 200 OK");
        display(this.responseText);
        keep_alive(this);
        info("Getting Data ...");
        handleLoginDone();
      }
      else if(this.status == 409) {
        logout(true);
        //login(username, password, resource, serviceVersion);
      }
      else
        error("LOGIN FAILED code=" + this.status + ":" + this.responseText);
		}
	}

	request.onreadystatechange = handler;
	sendRequest(request, url, method, postData);
	info("Logging In ...");
}

function logout(bReLogin) {

	var url = "/controller/Service/Logout";
	var method = "POST";
	var postData = "";
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("LOGOUT 200 OK");
            if(bReLogin)
              doLogin();
            else 
              display(this.responseText);
	        }
			else
				log("ERROR in Logout : " + request.status + ": " + request.responseText);
		}
	}
  
	request.onreadystatechange = handler;
	sendRequest(request, url, method, postData); 	
	info("Please Login");
}

function keep_alive(request) {
	var longpoll = request.getResponseHeader("X-Barco-notification-channel");
	
	longpoll = longpoll.substr(gBaseURL.length);
	
	log("keep_alive URL:" + longpoll);
	if( (longpoll == null) || (longpoll == "") ) {
	log("KEEPALIVE: ENDED");
		return;
	}
	
	var method = "GET";
	var request = new XMLHttpRequest();
   
	function handler() {
		if (this.readyState == 4) {
        	if(this.status == 200) {
	        	handleNotification(this.responseText);
	        	keep_alive(this);
        	}
			else
				error("ERROR in KeepAlive : " + request.status + ": " + request.responseText);
     	}
	}
   
   	request.onreadystatechange = handler;
	sendRequest(request, longpoll, method, ""); 	
}

function sendAPIRequest(path, listType, callback, cbdata) {

	var method = "GET";
	var postData = "";
	var url = path;
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("API 200 OK");
				info("Select to View");
		        callback(this, listType, cbdata);
	        }
        	else
		        error("API FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  	
   	request.onreadystatechange = handler;
	sendRequest(request, url, method, ""); 	
}

function handleNotification(data) {
	display("Notification:" + data);
}

/////////////////////////////////////////////////////////////////
/////////       API calls to add Port with URL and PVR backend
////////////////////////////////////////////////////////////////
function updatePort(request, portNID, title, postData, accept) {
	var url = "/controller/Resource/Port?NID=" + portNID + "&title=" + title;
	var method = "POST";
	sendRESTRequest(request, url, method, postData, accept);
}

function updatePVRInfo(portNID, title, dirNID) {
	var postData = "{\"PortPVRInfo\": {\"@mediaStreamDirResourceNID\": \"" + dirNID + "\",\"@trimLength\": \"60000\",\"@enable\": \"true\"}}";
	var request = new XMLHttpRequest();
	
	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("Port updatePVRInfo 200 OK");
		        handleAddComplete("Source");
	        }
        	else
		        error("Port updatePVRInfo FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  
	var accept = "application/vnd.barco.nimbus.Port-PVRInfo+json";
   	request.onreadystatechange = handler;
	updatePort(request, portNID, title, postData, accept);
}

function getDirInfo(portNID, title) {
	log("GetDir");
	var method = "GET";
	var url = "/controller/Resource/Dir";
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("GetDir 200 OK");
				var dirObj = JSON.parse(this.responseText);
				var list = dirObj.Resource.Dir;
				log(this.responseText);
				var dirNID = "";
				for(var i = 0; i < list.length; i++) {
					var state = list[i]["@state"];
					if(state == "Ready") {
						dirNID = list[i]["@NID"];
						break;
					}
				}
				if(dirNID != "")
       				updatePVRInfo(portNID, title, dirNID);
        		else
        			log("No Dir found for setting up PVRInfo");
	        }
        	else
		        error("GetDir FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  
	var accept = "application/vnd.barco.nimbus.Dir+json";
	var contentType = "application/json;charset=UTF-8";
   	request.onreadystatechange = handler;
	sendRESTRequest(request, url, method, "", accept);
}
function updateURL(portNID, sdptitle, sdpvalue) {
	var postData = "{\"PortStreamURL\": {\"@streamURL\": \"" + sdpvalue + "\"}}";	
	var request = new XMLHttpRequest();
	
	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("Port updateURL 200 OK");
        		getDirInfo(portNID, sdptitle);
	        }
        	else
		        error("Port updateURL FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  
	var accept = "application/vnd.barco.nimbus.Port-StreamURL+json";
   	request.onreadystatechange = handler;
	updatePort(request, portNID, sdptitle, postData, accept);
}

function handleAddSource(addtitle, addvalue) {

	var url = "/controller/Resource/Device";
	var method = "POST";

	var lines = addvalue.split("\n");
	addvalue = "";
	for(var i = 0; i < lines.length; i++) {
		addvalue = addvalue + lines[i] + "\\n"; 

	} 
			
	var postData = "{\"AddDevice\": {\"Groups\": [],\"@type\": \"RTP-Tx-Device\",\"@title\": \"" + 
					addtitle + 
					"\",\"@password\": \"\",\"@unManaged\": \"true\",\"@applyTagsToPorts\": \"true\" }}";
					
	// var nid = "default.mediastreamsrcportresourcelist.mssprl_62327c54-b653-4c77-8898-22b9e86ff4f6";
	// var addtitle = "224-1-155-1-26";
 	// updateURL(nid, addtitle, addvalue);
 	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("AddDevice 200 OK");
        		display(this.responseText);
				var deviceObj = JSON.parse(this.responseText);
				var nid = deviceObj.Resource.Device.PortList.Port["@NID"];
        		updateURL(nid, addtitle, addvalue);
	        }
        	else
		        error("AddDevice FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  
   	request.onreadystatechange = handler;
	sendRequest(request, url, method, postData);
}



/////////////////////////////////////////////////////////////////
/////////       Adding the Proxy Devices for MS, Node, TFN, DIOR, Web, Layout
////////////////////////////////////////////////////////////////
function handleAddProxyDevice(objectType, addtitle, addvalue) {
	var url = "/controller/Resource/Device";
	var method = "POST";
		
	var postData = "{\"AddDevice\": {\"Groups\": []," + 
					"\"@type\": \"Proxy-Device\"," + 
					"\"@description\": \"" + addvalue + "\"," + 
					"\"@title\": \"" + addtitle + 
					"\",\"@password\": \"\",\"@unManaged\": \"true\",\"@applyTagsToPorts\": \"true\" }}";
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("AddProxyDevice 200 OK");
        		display(this.responseText);
        		handleAddComplete(objectType);
	        }
        	else {
		        if(this.responseText.indexOf("5206"))
		        	handleUpdateProxyDevice(objectType, addtitle, addvalue);
		        else 
			        error("AddProxyDevice FAILED code=" + this.status + ":" + this.responseText);
		    }
		}
	}
  	
   	request.onreadystatechange = handler;
	sendRequest(request, url, method, postData);
}

function handleUpdateProxyDevice(objectType, addtitle, addvalue) {
	var url = "/controller/Resource/Device?title=" + addtitle;
	var method = "POST";
		
	var postData = "{\"DeviceInfo\": {\"@description\": \"" + addvalue + "\",}}";
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("UpdateProxyDevice 200 OK");
        		display(this.responseText);
	        }
        	else {
		        error("UpdateProxyDevice FAILED code=" + this.status + ":" + this.responseText);
		    }
		}
	}
  	
   	request.onreadystatechange = handler;
	var accept = "application/vnd.barco.nimbus.Device-Info+json";
	var contentType = "application/json;charset=UTF-8";
   	request.onreadystatechange = handler;
	sendRESTRequest(request, url, method, postData, accept);
}

function handleAddMS(addtitle, addvalue) {
	addvalue = Base64.encode(addvalue);
	// alert(addvalue);
	handleAddProxyDevice("MS", addtitle, "_ms_=" + addvalue);
}

function handleAddNode(addtitle, addvalue) {
	handleAddProxyDevice("Node", addtitle, "_node_=" + addvalue);
}

function handleAddTFN(addtitle, addvalue) {
	addvalue =  Base64.encode(addvalue);
	handleAddProxyDevice("TFN", addtitle, "_tfn_=" + addvalue);
}

function handleAddDIOR(addtitle, addvalue) {
	addvalue =  Base64.encode(addvalue);
	handleAddProxyDevice("DIOR", addtitle, "_dior_=" + addvalue);
}

function handleAddWeb(addtitle, addvalue) {
	addvalue =  Base64.encode(addvalue);
	handleAddProxyDevice("Web", addtitle, "_web_=" + addvalue);
}

function handleAddLayout(addtitle, addvalue) 	{
	addvalue = "layout.php?html=" + Base64.encode(addvalue);
	handleAddProxyDevice("Layout", addtitle, "_layout_=" + addvalue);
}

function handleAddPrivateData(addtitle, addvalue) 	{
	handleAddProxyDevice("PrivateData", addtitle, "_privatedata_=" + addvalue);
}

function handleRename(listType, oldtitle, newtitle) {
	// alert("handleRename: oldtitle=" + oldtitle + " newtitle=" + newtitle);
	var url = "";
	var method = "POST";
	var postData = "";
	var accept = "";
	
	newtitle = newtitle.trim();
	if(newtitle == "") {
		alert("Error: cannot rename to empty title");
		return;
	}
	
	if(listType == "Source") { // Port rename
		url = "/controller/Resource/Port?title=" + oldtitle;
		postData = "{\"PortInfo\": {\"@title\": \"" + newtitle + "\",}}";
		var accept = "application/vnd.barco.nimbus.Port-Info+json";
	}
	else if(listType == "Recording") { // File rename
	}
	else { // Device
	}

	if(url == "") {
		alert("handleRename not supported for :"+ listType);
		return;
	}
	
	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
		        log("Rename 200 OK");
		        handleMenuSelected();
	        }
        	else
		        error("Rename url=" + url + " FAILED code=" + this.status + ":" + this.responseText);
		}
	}
	  
	var request = new XMLHttpRequest();
   	request.onreadystatechange = handler;
	sendRESTRequest(request, url, method, postData, accept);
}



/////////////////////////////////////////////////////////////////
/////////       Layout Launch backend, this has a recursive Callback
////////////////////////////////////////////////////////////////

function getTitle(embedURL) {
	var i = embedURL.indexOf("source.php?");
	if(i == -1)
	  return "";
	var title = embedURL.substring(i + "source.php?".length);
	return title;
}

function getTFNSourceURL(mediaURL) {
    var startIndex = mediaURL.indexOf("Source=") + "Source=".length;
    var endIndex = mediaURL.indexOf("&Display=");
    return sourceURL = mediaURL.substr(startIndex, endIndex - startIndex);
}

function resolveTFNURL(title, tfnURL) {
    // get the source title
    var sourceURL = Base64.decode(getTFNSourceURL(tfnURL));
    var sourcetitle = sourceURL.substring("source.php?".length);
    var expiryTime = "";
    
    // get the URL
	getURL(sourcetitle, getTFNURLCallback, tfnURL, expiryTime);
}

function getTFNURLCallback(mediaURL, tfnURL) {
    // get the source title
    var encodedSourceURL = getTFNSourceURL(tfnURL);
    
    // Set the layoutURL - since that is what share is looking at
    gWaitURL = getFullURL(tfnURL.replace(encodedSourceURL, Base64.encode(mediaURL)));
    // alert(gWaitURL);
}

function resolveSourceURL(title, sourceURL, expiryTime) {
    // get the URL
	getURL(title, getSourceURLCallback, sourceURL, expiryTime);
}

function getSourceURLCallback(mediaURL, sourceURL) {    
    // Set the layoutURL - since that is what share is looking at
    mediaURL = mediaURL + "&confOpt_showControlBar=false";
    gWaitURL = mediaURL;
    // alert(gWaitURL);
}


function resolveLayoutURL(title, mediaURL) {
	var htmlstr = mediaURL.substring("layout.php?html=".length);
	
	// Create an object to hold the html, sourcelist and urllist
	var layoutData = JSON.parse('{ "html":"", "embedlist":[], "urllist":[], "count":0}');
	layoutData.html = Base64.decode(htmlstr);
	var el = document.createElement("div");
	el.innerHTML = layoutData.html;
	var list = el.getElementsByTagName('iframe');
	layoutData.count = 0;
	for(var i = 0; i < list.length; i++) {
		var embedURL = list[i].src;
		if(embedURL.indexOf("source.php?") == -1)
			continue; // not an embedURL
		// layoutData.embedlist[layoutData.count] = embedURL;
		layoutData.embedlist[layoutData.count] = embedURL.substring(embedURL.indexOf("source.php?"));
		layoutData.urllist[layoutData.count] = "";
		layoutData.count++;
	}
	layoutData.count = 0;
	getLayoutURLCallback("", layoutData);
}

function serialize(list) {
	var listStr = "";
	for(var i = 0; i < list.length; i++)
		listStr = listStr + list[i] + ",";
	return listStr;
}


function getLayoutURLCallback(mediaURL, layoutData) {
	if(mediaURL != "") { // update the mediaURL in the html
		layoutData.urllist[layoutData.count] = mediaURL;
		layoutData.count++;
	}
	if(layoutData.count < layoutData.embedlist.length) { // Check if there are more embed URLs
		getURL(getTitle(layoutData.embedlist[layoutData.count]), getLayoutURLCallback, layoutData, "");
		return;
	}
	var embedlistStr = "";
	var urllistStr = "";
	for(var i = 0; i < layoutData.embedlist.length; i++) {
		embedlistStr = embedlistStr + Base64.encode(layoutData.embedlist[i]) + ",";
		var url = Base64.encode(layoutData.urllist[i]);
		urllistStr = urllistStr + url + ",";
	}
	
	var layoutURL = "layout.php?html=" + Base64.encode(layoutData.html) + ";" +
		"sourcelist=" + embedlistStr + ";" +
		"urllist=" + urllistStr;
		
	// Setup the share URL for the layoutURL
	var method = "GET";
	var postData = "";
	var url = layoutURL;	
	var request = new XMLHttpRequest();
	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
				var obj = JSON.parse(request.responseText);
				gWaitURL = getFullURL("layout.php?share=" + obj.shareid);
				// alert("layoutURL=" + gWaitURL);
	        }
        	else
		        error("FAILED code=" + this.status + ":" + this.responseText);
		}
	}
   	request.onreadystatechange = handler;
	sendRequest(request, url, method, ""); 	
	
}


/////////////////////////////////////////////////////////////////
/////////       Selector backend
////////////////////////////////////////////////////////////////
function getColor(title, listType) {
	showingTitle = listType + "." + title;

	var color = "lightgray";
	if(showingList.indexOf(showingTitle) >= 0)
		color = "orange";
	return color;
}

function addShowing(title, listType){
	showingTitle = listType + "." + title;
	
	if(showingList.indexOf(showingTitle) < 0)
		showingList.push(showingTitle);
		
	return showingList;
}

function resetShowing() {
	while(showingList.length > 0)
		showingList.pop();
}

function getValue(listType, listElement) {
	var value = "";
	if(listType == "MS") {
		var description = listElement["@description"];
		value = description.substring("_ms_".length + 1);
	}
	else if(listType == "Node") {
		var description = listElement["@description"];
		value = description.substring("_node_".length + 1);
	}
	else if(listType == "TFN") {
		var description = listElement["@description"];
		value = description.substring("_tfn_".length + 1);
		value = "server.php?"+ Base64.decode(value);		
	}
	else if(listType == "DIOR") {
		var description = listElement["@description"];
		value = description.substring("_dior_".length + 1);
	}
	else if(listType == "Web") {
		var description = listElement["@description"];
		value = description.substring("_web_".length + 1);
	}
	else if(listType == "Layout") {
		var description = listElement["@description"];
		value = description.substring("_layout_".length + 1);
	}
	else if(listType == "Source") {
		value = listElement["@mediaURL"];
	}
	else if(listType == "Recording") {
		value = listElement["@mediaURL"];
	}
	else if(listType == "PrivateData") {
		var description = listElement["@description"];
		value = description.substring("_privatedata_".length + 1);
	}
	return value;
}

function convertToList(list) {
	if(list == undefined) // No items in the list
		return new Array(0);
	if((typeof list != undefined) && !(list instanceof Array)) { // Only 1 item in the list
		var newlist = new Array(1);
		newlist[0] = list;
		list = newlist;
	}
	return list;
}



function handleAPIResponse(request, listType, cbdata) {
	display(request.responseText);
	
	var obj = JSON.parse(request.responseText);
	
	
	var list = obj.Resource.Device;
	if(listType == "Recording")
		list = obj.Resource.File;
	else if(listType == "Source")
		list = obj.Resource.Port;

	list = convertToList(list);
	showList(list, listType);
}


/////////////////////////////////////////////////////////////////
/////////       Callbacks to the  UI - Logging
////////////////////////////////////////////////////////////////
function display(data) {
	var response = document.getElementById("response");
	if(response != undefined)
		response.value = data;
}

function info(data) {
	document.getElementById("info").innerHTML = data;
}

/////////////////////////////////////////////////////////////////
/////////       Callbacks to the  UI - Selector
////////////////////////////////////////////////////////////////

function handleLoginDone() {
	handleMenuSelected();
	handleServerMenuSelected();
}

function getListType() {
	// Find out if this are Ports or Files
	var menu = document.getElementById("menu");
	var menuopt = menu.options[menu.selectedIndex];
	return menuopt.innerHTML;
}

function getServerListType() {
	// Find out if this are Ports or Files
	var menu = document.getElementById("servermenu");
	var menuopt = menu.options[menu.selectedIndex];
	return menuopt.innerHTML;
}

function showList(list, listType) {

	var selector = document.getElementById("selector");
	if((listType == "MS") || (listType == "Node") ){
		selector = document.getElementById("serverselector");
	}
	
	var total = 0;
		
	for(var option in selector) {
	 	selector.remove(option);
	}
	
	// Add the ports
	if(list instanceof Array) { // more than 1 port
		for(var i = 0; i < list.length; i++) {
			var opt = document.createElement('option');
			opt.value = getValue(listType, list[i]);
			opt.innerHTML = list[i]["@title"];
			opt.style.backgroundColor= getColor(opt.innerHTML, listType);
			selector.appendChild(opt);
		}
		total = list.length;
	}
	else
		alert("Unable to handle list=" + list);
	
	if((listType == "MS") || (listType == "Node") ){
		// Nothing to update
	}
	else {
		document.getElementById("showing").innerHTML = showingList.length;
		document.getElementById("total").innerHTML = total;
	}
}



/////////////////////////////////////////////////////////////////
/////////       Login/Logout UI
////////////////////////////////////////////////////////////////
function handleOK(username, password, resource, serviceVersion) {
	login(username, password, resource, serviceVersion);
}

function handleCancel() {
	resetSelector();
	resetServerSelector();
	while(showingList.length > 0)
		showingList.pop();
	logout();
}


/////////////////////////////////////////////////////////////////
/////////       Menu UI
////////////////////////////////////////////////////////////////
function handleMenuSelected() {
	var menu = document.getElementById("menu");
	var opt = menu.options[menu.selectedIndex];
	var url = opt.value;
	sendAPIRequest(url, getListType(), handleAPIResponse, "");
	
}

/////////////////////////////////////////////////////////////////
/////////       Add UI
////////////////////////////////////////////////////////////////

function handleAddCallback(objectType, addtitle, addvalue) {

	// Call the appropriate add handler
	if(objectType == "MS")
		handleAddMS(addtitle, addvalue);
	else if(objectType == "Node")
		handleAddNode(addtitle, addvalue);
	else if(objectType == "TFN")
		handleAddTFN(addtitle, addvalue);
	else if(objectType == "DIOR")
		handleAddDIOR(addtitle, addvalue);
	else if(objectType == "Web")
		handleAddWeb(addtitle, addvalue);
	else if(objectType == "Layout")
		handleAddLayout(addtitle, addvalue);
	else if(objectType == "Source")
		handleAddSource(addtitle, addvalue);
}

function handleAddComplete(objectType) {
	// Update the list
	if(getListType() == objectType)
		handleMenuSelected();
	else if(getServerListType() == objectType)
		handleServerMenuSelected();
}

/////////////////////////////////////////////////////////////////
/////////       Selector UI
////////////////////////////////////////////////////////////////

function resetSelector() {
	var selector = document.getElementById("selector");
	for(var option in selector) {
	 	selector.remove(option);
	}	
}

function handleMediaSelected() {
	var selector = document.getElementById("selector");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined)
		return;
	var mediaURL = opt.value;
	if(mediaURL == undefined)
		return;
	var title = opt.innerHTML;
	opt.style.backgroundColor="orange";
	opt.selected = false;

	var listType = getListType();	

	var modeselector = document.getElementById("mode");
	var opt = modeselector.options[modeselector.selectedIndex];

	if(listType == "Web") {
		mediaURL =  Base64.decode(mediaURL);
	}

	// handle RENAME
	if(opt.value == "rename") {
		if(listType != "Source") { // Only Port rename supported
			alert("Rename not supported for listType:" + listType);
			return;
		}
		var newtitle = prompt("Enter new title", title);
		handleRename(listType, title, newtitle);
		return;
	}
	
	// handle EMBED
	if(opt.value == "embed") {
		if(listType == "Source") {
			prompt("Embed tag for " + title, getFullURL("/source.php?" + title));
		}
		else if(listType == "Recording") {
			prompt("Embed tag for " + title, getFullURL("/source.php?Recording." + title));
		}
		else if(listType == "Layout") {
			prompt("Embed tag for " + title, getFullURL("/source.php?Layout." + title));
		}
		else if(listType == "TFN") {
			prompt("Embed tag for " + title, getFullURL("/source.php?TFN." + title));
		}
		else if(listType == "Web") {
			prompt("Embed tag for " + title, mediaURL);
		}
		else
			alert("Embed tag not supported for :" + listType);
		return;
	}
		
	// handle SHARE
	if(opt.value == "share") {
		if(listType == "Layout") { // Need to wait for the full layoutURL to get generated
			gWaitURL = "";
			resolveLayoutURL(title, mediaURL);
			mediaURL = "share.php?WAIT";
		}
		else if(listType == "TFN") {
			gWaitURL = "";
			
			// NOTE *** : Do not share the pull URL, only the push URL, hence just share the source
			// The code to share the pull also works and has been tested - MIYE
			// resolveTFNURL(title, mediaURL);
			var sourceURL = Base64.decode(getTFNSourceURL(mediaURL));
		    title = sourceURL.substring("source.php?".length);	
			resolveSourceURL(title, mediaURL, "");		    
			mediaURL = "share.php?WAIT";
		}
		else {
			mediaURL = "share.php?" + Base64.encode(mediaURL);
		}
		window.open(mediaURL, "Share", "height=300,width=400, resizable=1, scrollbars=yes");
		return;
	}

	if(opt.value == "publish") { // Only supported for sources
		if(listType == "TFN") { // Find the source associated with it
			listType = "Source";
			var sourceURL = Base64.decode(getTFNSourceURL(mediaURL));
		    title = sourceURL.substring("source.php?".length);	
		}		
		if(listType == "Source") {
			gWaitURL = "";
      var expiryTime = document.getElementById("txtExpTime").value * 3600;
			resolveSourceURL(title, mediaURL, expiryTime);
			mediaURL = "share.php?PUBLISH";
			window.open(mediaURL, title, "height=300,width=400, resizable=1, scrollbars=yes");
		}
		else {
			alert("Publish tag not supported for :" + listType);
		}
		return;
	}

	// handle VIEW
	var embedURL = "";
	if(listType == "Source") {
		embedURL = "source.php?" + title;
	}
	else if(listType == "Recording") {
		embedURL = "source.php?Recording." + title;
	}
	else if(listType == "Layout") {
		embedURL = "source.php?Layout." + title;
	}
	else if( (listType == "TFN") || (listType == "DIOR") ){
		winoptions = "height=420,width=400, resizable=1, scrollbars=yes";
		// embedURL = "source.php?TFN." + title;
		embedURL = mediaURL;
	}
	
	
	openWindow(mediaURL, title, "height=400,width=400, resizable=1, scrollbars=yes, location=no", embedURL);

	var sList = addShowing(title, listType);
	document.getElementById("showing").innerHTML = sList.length;
}

function handleModeSelected() {
  var modeselector = document.getElementById("mode");
	var opt = modeselector.options[modeselector.selectedIndex];
  if(opt.value == "publish") {
    document.getElementById("spanVisited").style.display = "none";
    document.getElementById("spanExp").style.display = "block";    
  }
  else {
    document.getElementById("spanExp").style.display = "none";
    document.getElementById("spanVisited").style.display = "block";
  }
}
/////////////////////////////////////////////////////////////////
/////////       Server Selector UI
////////////////////////////////////////////////////////////////

function resetServerSelector() {
	var selector = document.getElementById("serverselector");
	if(selector == undefined)
		return;
	for(var option in selector) {
	 	selector.remove(option);
	}	
}

function handleServerMenuSelected() {
	var menu = document.getElementById("servermenu");
	if(menu == null)
		return;
	var opt = menu.options[menu.selectedIndex];
	var url = opt.value;
	sendAPIRequest(url, opt.innerHTML, handleAPIResponse, "");
}

function handleServerSelected() {
	var selector = document.getElementById("serverselector");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined)
		return;
	var serverURL = opt.value;
	// alert("serverURL=" + serverURL);
	if(serverURL == undefined)
		return;
	var title = opt.innerHTML;
	opt.style.backgroundColor="orange";
	opt.selected = false;
	
	var menu = document.getElementById("servermenu");
	var opt = menu.options[menu.selectedIndex];
	var serverListType = opt.innerHTML;
	
	var winoptions = "height=400,width=400, resizable=1, scrollbars=yes";
	
	if(serverListType == "MS") {
		var msURL = document.URL.replace("https", "http");
		serverURL = msURL + "?" + serverURL;
		winoptions = "height=400,width=310, resizable=1, scrollbars=yes";
	}
	
	openWindow(serverURL, title, "height=400,width=310, resizable=1, scrollbars=yes", "");

}

/////////////////////////////////////////////////////////////////
/////////       Add related operations
////////////////////////////////////////////////////////////////

<!-- Used to populate the list of currently showing sources -->
function handleAdd(listType) {
    var addURL = "add.php?" + listType;
    if(listType == "Layout") {
    	addURL = addURL  + "=";
    	for(var i = 0; i < showingList.length; i++)
    		addURL = addURL + showingList[i] + ",";
    }
    
    if( (listType == "TFN") || (listType == "DIOR") ) {
    	addURL = "server.php?" + listType;
    }
    
    window.open(addURL, "add", "height=400,width=400, resizable=1, scrollbars=yes", "");
}

function handleAddCurrentServerListType() {
	var listType = getServerListType();
	handleAdd(listType);
}

function handleAddCurrentListType() {
	var listType = getListType();
	if(listType == "Recording") {
		alert("Add Recording not supported");
		return;
	}
	handleAdd(listType);
}

/////////////////////////////////////////////////////////////////
/////////  Window Manager Hook - added to support external Window Managers e.g ows 
////////////////////////////////////////////////////////////////
// Used to support desktop window manager, specific browsers and external window managers
function openWindow(url, title, options, embedURL) {
	var wintitle = title.replace(/\W*/g, '');
	if(window.parent.handleSelected && (typeof(window.parent.handleSelected) === "function") )
		window.parent.handleSelected(url, wintitle);
	else { // Normal case look for the embedURL
		if(embedURL != "")
			url = embedURL;
		window.open(url, wintitle, options);
	}
}

/////////////////////////////////////////////////////////////////
/////////  get URL Hook - added to support external web sites 
////////////////////////////////////////////////////////////////
function getURL(title, callback, cbdata, expiryTime) {

	// Add the expiry option option if this is for a user in the system
	var expiryParam = "";
	if(expiryTime && expiryTime != "") // Used to generate the shareURL
		expiryParam = "&expireTime=" + expiryTime;
	  
	// Default is source
	var listType = "Source";
	var path ="/controller/Resource/Port?selectFormat=BASICURL&type=SrcPort"+ expiryParam + "&dstPortTitle=" + gResource + "&whereTextSubStr=" + title;
	if(title.substring(0, "Source.".length) == "Source.") {
		title = title.substring("Source.".length);
		listType = "Source";
		path ="/controller/Resource/Port?selectFormat=BASICURL&type=SrcPort"+ expiryParam + "&dstPortTitle=" + gResource + "&whereTextSubStr=" + title;
	}	
	else if(title.substring(0, "Recording.".length) == "Recording.") {
		title = title.substring("Recording.".length);
		listType = "Recording";
		path ="/controller/Resource/File?selectFormat=BASICURL"+ expiryParam + "&dstPortTitle=" + gResource + "&whereTextSubStr=" + title;
	}
	else if(title.substring(0, "Layout.".length) == "Layout.") {
		title = title.substring("Layout.".length);
		listType = "Layout";
		path ="/controller/Resource/Device?whereTextSubStr=_layout_";
	}
	else if(title.substring(0, "TFN.".length) == "TFN.") {
		title = title.substring("TFN.".length);
		listType = "TFN";
		path ="/controller/Resource/Device?whereTextSubStr=_tfn_";
	}
	else if(title.substring(0, "PrivateData.".length) == "PrivateData.") {
		title = title.substring("PrivateData.".length);
		listType = "PrivateData";
		path ="/controller/Resource/Device?whereTextSubStr=_privatedata_";
	}

	// Add the recording option if this is for a user in the system
	var options = "";
//	if((cbdata == "") && (listType == "Source")) // Coming from source.php					
	if(listType == "Source") // Coming from source.php					
		options = "&confOpt_showRecordingControls=true";
	
	var method = "GET";
	var postData = "";
	var url = path;
	
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
				var obj = JSON.parse(request.responseText);
				var	list = obj.Resource.Device;
				if(listType == "Recording")
					list = obj.Resource.File;
				if(listType == "Source")
					list = obj.Resource.Port;
				list = convertToList(list);
				var url = "";
				for(var i = 0; i < list.length; i++) {
					if(title == list[i]["@title"]) {
						url = getValue(listType, list[i]);
						url = url + options;
						// alert(url);
						break;
					}
				}
				callback(url, cbdata);
	        }
        	else
		        error("API FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  	
   	request.onreadystatechange = handler;
	sendRequest(request, url, method, ""); 	
}

function getSources(callback) {
	var path ="/controller/Resource/Port?selectFormat=BASICURL&sortBy=title&type=SrcPort&dstPortTitle=" + gResource;	
	sendAPIRequest(path, "Source", getSourcesCB, callback);
}

function getSourcesCB(request, listType, cbdata) {
	display(request.responseText);
	var obj = JSON.parse(request.responseText);
	var	list = obj.Resource.Port;
	list = convertToList(list);
	cbdata(list);
}

function putPrivateData(data) {
	handleAddPrivateData(gResource + "-privatedata", Base64.encode(data));
}

function getPrivateData(callback) {
	getURL("PrivateData." + gResource + "-privatedata", getPrivateDataCB, callback, "");
}

function getPrivateDataCB(data, cbdata) {
	data = Base64.decode(data);
	cbdata(data);
}

function testCB(data) {
	alert("testCB: " + data);
}

