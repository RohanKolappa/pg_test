let $assetResourceList := for $resource in collection('ipvs.default.assetresourcelist')[(contains(.//Info/Type,"MNA") or contains(.//Info/Type,"V2D-XP")) and not(.//Info/Type='V2D-XPi-Device')] 
                              return $resource
let $out := for $resource in (collection('ipvs.default.mediastreamsrcportresourcelist')
                                              | collection('ipvs.default.mediastreamioportresourcelist'))
							   return 
							       if($resource[ (.//StreamType[.="V2D" or .="RTP-RAW"])]//AssetResourceNID=$assetResourceList//@NID) then
							         update replace $resource//ThumbnailConfig/Enable with <Enable>true</Enable> 
                                   else
                                     update replace $resource//ThumbnailConfig/Enable with <Enable>false</Enable>
return $out