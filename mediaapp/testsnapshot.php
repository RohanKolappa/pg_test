<html>

<head>
<script type="text/javascript"> 
function addParamToURL()
{	
	//adds following params to the url
	/*
	*ProcedureType
	*Patient
	*Phycisian
	*Hospital
	*/
    var form=document.getElementById('form_submit');
    var action=new String();
    var title=document.getElementById('title');	
    var patientId=document.getElementById('pId');
    var patientName=document.getElementById('patientName');
    var patientBirthDate=document.getElementById('patientBirthDate');
    var patientGender=document.getElementById('patientGender');
    var accessionNumber=document.getElementById('accessionNumber');
    var doctorName=document.getElementById('doctorName');
	//action = 'http://10.51.51.9/mediaapp/snapshot.php';
	if(document.getElementById('urlName').value){
		action = urlName.value;
		action += '&title='+title.value+'&patientId='+patientId.value+'&patientName='+patientName.value+'&patientBirthDate='+patientBirthDate.value+'&patientGender='+patientGender.value+'&accessionNumber='+accessionNumber.value+'&doctorName='+doctorName.value;	
		form.action=action;
	}
	else{
		alert("Please enter base URL");
	}
}
</script>
</head>
<body>
<label for="urlName" name="urlLabel" id=="urlLabel">*Enter Base URL </label>
<input type="text" name="urlName" id="urlName" /></p>
<form id="form_submit" action="" method="post" enctype="multipart/form-data" onsubmit= "addParamToURL()">
<fieldset>
<legend>Barco Patient Form </legend>

<p><label  name="titleLabel" id=="titleLabel">Title </label>
<input type="text" name="title" id="title" /></p>
<p><label  name="pIdLabel" id=="pIdLabel">Patient ID </label>
<input type="text" name="pId" id="pId" /></p>
<p><label  name="patientNameLabel" id="patientNameLabel">Patient Name </label>
<input type="text" name="patientName" id="patientName" /></p>
<p><label  name="patientBirthDateLable" id="patientBirthDateLable">Patient Birth Date </label>
<input type="text" name="patientBirthDate" id="patientBirthDate" /></p>
<p><label  name="patientGenderLabel" id=="patientGenderLabel">Patient Gender </label>
<input type="text" name="patientGender" id="patientGender" /></p>
<p><label  name="accessionNumberLabel" id=="accessionNumberLabel">Accession Number </label>
<input type="text" name="accessionNumber" id="accessionNumber" /></p>
<p><label  name="doctorNameLabel" id=="doctorNameLabel">Doctor Name</label>
<input type="text" name="doctorName" id="doctorName" /></p>
<label for="file" name="fileLabel">Filename:</label>
<input type="file" name="file" id="file" />
<br />
<input type="submit" name="submit" value="Submit" />
</fieldset>
</form>

</body> 

</html>
