for $dir in    index-scan('IPVSis_colnids_default','default.mediastreamdirresourcelist', 'EQ')/..[//Info/Title='<RecordingDir>']/@NID       
let $dirNID := string($dir)
let $sub :=
for $file in    index-scan('IPVSis_colnids_default','default.mediastreamfileresourcelist', 'EQ')/..
where $file//Properties/ParentMediaDirNID = $dirNID
return  <exec>  {string ( $file/@NID )  }   </exec> 
return <exec>  { $sub   } </exec>

