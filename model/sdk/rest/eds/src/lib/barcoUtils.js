
var service = "2.34.14";

var protocol = location.protocol;

if(window.opener != null){
var resource = window.opener.document.getElementById('input_apiKey1').value;
console.log("resource: " + resource);
}

 
 console.log("service: " + service);

function login(serverip, username, password) {

 

 	var url = protocol +"//" + serverip + "/controller/Service/Login";
	console.log("BarcoUtils Login serverip: " + serverip +" username: " + username+" password " + password);

	var method = "POST";

	var postData = "{\"ServiceLogin\": {\"clientPrivateKey\": \"\",\"clientTemplateTitle\": \"" + resource + "\",\"@clientVersion\": \""+ service +"\"	}}";

	

	var async = true;

	

	var request = new XMLHttpRequest();

	

	request.onload = function () {

	   var status = request.status; // HTTP response status, e.g., 200 for "200 OK"

	   var data = request.responseText; // Returned data, e.g., an HTML document.

     var resp = request.resonseXML;

     

     if (request.readyState == 4) {

        if(request.status == 200) {

          console.log("RECEIVED 200 OK for LOGIN. SENDING LONG POLL");

          

          var longpoll = request.getResponseHeader("X-Barco-notification-channel");

          document.getElementById("url").value = longpoll;

          console.log("LONG POLL: " + longpoll);

          //keep_alive(longpoll, username, password, resource);

        }else if (request.status == 409){
		 alert ("Duplicate Login! "+ data+ "! Go ahead!");
		}else {
		 alert ("Login Failure! " + data);
		}

     }

	}

  

  request.onerror = function () {

     console.log("ERROR in Login : " + request.status + ": " + request.responseText);
     alert ("Login Failure!Please check connectivity with MS.");
     //document.getElementById("response").value = request.responseText;

  }

	

	request.open(method, url, async);

	

  request = addHeaders(request, username, password, resource);

  

  request.send(postData);

}





function logout(serverip, username, password, resource) {



  var url = protocol +"//" + serverip + "/controller/Service/Logout";

	var method = "POST";

	var postData = "";

	var async = true;

	

	var request = new XMLHttpRequest();



	request.onload = function () {

     var status = request.status; 

     var data = request.responseText;

     var resp = request.resonseXML;

     

     if (request.readyState == 4) {

        if(request.status == 200) {

          console.log(data);

        }

     }

	}

  

  request.onerror = function () {

     console.log("ERROR in Logout : " + request.status + ": " + request.responseText);

     document.getElementById("response").value = request.responseText;

  }

	

	request.open(method, url, async);

  request = addHeaders(request, username, password, resource);

 	request.send(postData);

}



function sendRequest(username, password, serverip, resource) {



  var url = document.mrform.url.value;
console.log("url->"+url);

	var method = document.mrform.requestType.value;

	var async = true;



  var postData = "";

  if(method == "POST" || method == "PUT") {

      postData = document.mrform.request.value;

  }

    

  var request = new XMLHttpRequest();



	request.onload = function () {

     var status = request.status; 

     var data = request.responseText;

     var resp = request.resonseXML;

     

     if (request.readyState == 4) {

        if(request.status == 200) {

          console.log(data);

          document.getElementById("response").value = data;

        }

     }

	}

  

  request.onerror = function () {

     console.log("ERROR in request: " + request.status + ": " + request.responseText);

     document.getElementById("response").value = request.responseText;

  }

	

	request.open(method, url, async);

  request = addHeaders(request, username, password, resource);

 	request.send(postData);

  

}



function keep_alive(longpoll, username, password, resource) {

   var method = "GET";

   var async = true;

   console.log("KEEPALIVE: " + longpoll);

   if(longpoll == null) {

      console.log("KEEPALIVE: ENDED");

      return;

   }

   var request = new XMLHttpRequest();

   

   request.onload = function () {

      if (request.readyState == 4) {

        if(request.status == 200) {

          //document.getElementById("notification").value += "\n------------------------ Notification------------------------"

          document.getElementById("notification").value += "\n" + request.responseText;

          //document.getElementById("notification").value += "\n------------------------ End Notification---------------------"

          

          var longpoll = request.getResponseHeader("X-Barco-notification-channel");

          //console.log("KEEPALIVE, CALLBACK WITH: " + longpoll);

          keep_alive(longpoll, username, password, resource);

        }

     }

   }

   

   request.onerror = function () {

     console.log("KEEPALIVE ERROR: " + longpoll);

   }

   request.open(method, longpoll, async);

   request = addHeaders(request, username, password, resource);

   

   request.send();

}



function addHeaders(request, username, password,resource) {
//console.log("addHeaders, addHeaders:application/json ");

   request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

	 request.setRequestHeader("Accept", "application/json");

	 request.setRequestHeader("X-Barco-resource", resource+"_"+username);

   request.setRequestHeader("Authorization", "Basic " + Base64.encode(username+":"+password));

   return request;

}





