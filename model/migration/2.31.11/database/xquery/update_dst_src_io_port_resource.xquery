let $assetResourceList := for $resource in collection('ipvs.default.assetresourcelist') where $resource//TemplateAssetResourceNID='TEMPLATE'
                              return $resource

let $webclientlist := for $resource in collection('ipvs.default.mediastreamdstportresourcelist') where $resource//AssetResourceNID = $assetResourceList//AssetResource/@NID
                   					return update replace $resource//StateFlag with <StateFlag>Template</StateFlag>

let $out := for $resource in (collection('ipvs.default.mediastreamsrcportresourcelist')
                                              | collection('ipvs.default.mediastreamdstportresourcelist') | collection('ipvs.default.mediastreamioportresourcelist')) where $resource[not(.//RemotelyOperatedMode)]
							   return 									
									if($resource//AssetResourceNID = $assetResourceList//AssetResource/@NID) then
										update insert <RemotelyOperatedMode>false</RemotelyOperatedMode> into $resource//Info
									else
										update insert <RemotelyOperatedMode>true</RemotelyOperatedMode> into $resource//Info

return $out