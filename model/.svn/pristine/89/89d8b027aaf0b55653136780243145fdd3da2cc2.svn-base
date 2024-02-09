let $objects := for $b in collection("${ipvs_collection}")/*${xpath}
return 
<object name='{$b/name() }'  docuri='{ document-uri(root($b)) }'>
 {$b/@NID}
</object>
return
<objects>
{$objects}
</objects>