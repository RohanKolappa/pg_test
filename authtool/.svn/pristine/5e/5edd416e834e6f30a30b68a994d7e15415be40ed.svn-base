var oIE = new ActiveXObject("InternetExplorer.Application");

var fso = new ActiveXObject("Scripting.FileSystemObject");

var fullPathName = fso.GetAbsolutePathName("launchie.js");

var parentDir = fullPathName.substring(0, fullPathName.indexOf("launchie.js"));

var indexUrl = parentDir + "index.html";

oIE.Navigate2(indexUrl);

oIE.Visible = true;
