/**
 * @author ZHEN
 */

Ext.define('TabletGui.profile.Tablet',{
	extend:'Ext.app.profile',
	config:{
		name:'Tablet',
		models:['Login','Setup'],
		views:['TabletGui.view.LoginContainer','TabletGui.view.SettingsContainer'],
		controllers:['Login','Setup']	
	},
	isActive:function(){
		//Ext.os.is.Tablet
		return true;
	}	
});
