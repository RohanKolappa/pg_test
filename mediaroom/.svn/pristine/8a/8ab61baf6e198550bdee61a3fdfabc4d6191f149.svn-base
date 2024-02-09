1. If you are adding new Object in new collection then add new collection in MRSchema.XML in model objectXML folder.

2. Add new object in AMDbObject.xml /PMDbObject.XML based on the service you are adding : required by DB layer to create NID /parentNID         

3. To go through policy check add new Object on model/mediaroomXML/com/ipvs/policyadminservice/datamodel/PolicyRoleObjectToModelObjectMapping.XML.
       - This mapping file contains the information of model object to policy role object mapping.
       - If its a new policy role object then add the new role object in PoliciesToRoleObjectMapping.xml too.
       - update automation scripts policies to include these new object in policies
       
4. Add custom_validation for new object if it is new asset admin object : (No custome validation for Policy admin service objects) 
	model/mediaroomXML/com/ipvs/assetadminservice/datamodel/xml/OBJECT_NAME__CustomValidation.xml

5. Add new scripts to add new object in database in test_add_device.exp based on service. 

6. if device, add to config handler : /model/mediaroomXML/com/ipvs/xsd/xml/confighandler.xml (Not applicable to non ipvs devices) 

7. check you need any Enums in xsd

8. Add the devices to AddDeviceMapping.xml in mediaroom