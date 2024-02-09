Follow these steps on linux to change service version

	1. Update Service version in ObjectXML/service.version file
	2. run createRevision.sh with old_service_version and new_service_version
	3. It will create old_service_version folder in migration folder.
	4. run svn add on migration/old_service_ version folder 
	5. commit all the changes

Follow these steps to add xsl for db objects.

	1. If your update related to Device XMl then update migration/old_service_version/xsl/Device.xsl.
				- Should include any device XML change (including status and config)
	2. If your update related to any db object other than any device xml then add those xsl file in migration/old_service_version/database/xsl
				- To update any existing object document create XSL file as ObjectName.xsl 
				- To add new object from existing object then use add_ObjectName.xsl 
				  (This one allows you to add one object at a time.
				   If your requirement is from one object you have to create multiple then you have to change service version multiple times)
	3. Add xsl files mapping in migration/old_service_version/database/DBMigrationFileSyste.xml
	           - check the file for syntax how to add object mapping
	           - to add object check 1.0 DBMigrationFileSystem.xml and DBMIgration.xml files migration/1.0/database/DBMigrationFileSystem and migration/1.0/DBMigration.xml
	           
Testing:

1. Modify migration scripts and make a build with your changes
2. restore/run admin setup with older service version 
3. upload new build
4. check the db files and /etc/config/Device.xml on devices that your changes are effecting these files or not.


	           
               
			

