UPDATE replace 
$d in collection("${ipvs_collection}")/*/@serviceVer
with  
attribute serviceVer {"${aftermigration_serviceVersion}"} 
 