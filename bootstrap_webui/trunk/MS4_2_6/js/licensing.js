function loadLicenseInfo() {
	$.ajax({
		url:"/licenseservice/get",
		type:"GET",
		contentType:"application/json",
		
		success: function(response){
			var jsonData = response;
			var licenseInfo= jsonData.licensedata.license;
			var expiresOn = licenseInfo.validity["expiry-date"];
			var dateSplitArr = expiresOn.split("-");
			var expiresStr = dateSplitArr[1] + " " + dateSplitArr[0] + "," + dateSplitArr[2];
			var expiryDate = new Date(expiresStr);

			
			// Update the values of various html input elements
			document.getElementById("projectname").value=licenseInfo.project.name
			document.getElementById("licensedto").value=licenseInfo.customer.name;
			document.getElementById("licensestatus").value=getValidity(expiryDate);
			document.getElementById("daysleft").value=licenseDaysLeft(expiryDate);
			document.getElementById("pincode").value=licenseInfo['@id'];
			document.getElementById("licensekey").value = licenseInfo['@key'];			
		},
		
		error: function(e){
			console.log("Failure: " +e.text);
		}
	});	
}

// Returns if the license id Valid or Invalid 
function getValidity(endDate)
{
	return (endDate > new Date() ? "Valid" : "Invalid");
}

// Returns number of days left in password expiration. Zero if already expired.
function licenseDaysLeft(endDate)
{
	var daysLeft = Math.floor((endDate - new Date()) / (1000*60*60*24));
	return (daysLeft > 0 ? daysLeft : 0);
}