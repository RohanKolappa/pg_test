/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.MainPage', {
    extend: 'Ext.Container',
    alias:'widget.mainpage',

    config: {
	    id: 'mainscreen',	     	
        layout: {
            type: 'fit'
        },
        items: [
	            {
	                xtype: 'toolbar',
	                docked: 'top',
	                ui: 'red',
	                items: [
	                    {
	                        xtype: 'selectfield',
	                        docked: 'right',
	                        label: '',
	                        //autoSelect:true,
	                        options: [
	                            {
	                                text: 'Logout',
	                                value: 'Logout'
	                            },
	                            {
	                                text: 'Settings',
	                                value: 'Settings'
	                            }
	                        ]
	                    }
	                ]
	            },
	            {
	                xtype: 'container',
	                id: 'mainpanel',
	                layout: {
	                    type: 'hbox'
	                },
	                items: [
	                   	{
							xtype: 'tabpanel',
	                        ui: 'gray',
	                        id:'tabpanel',
							cls: 'source-panel-container',
	                        width: 250,
	                        layout: {
	                            animation: 'flip',
	                            type: 'card'
	                        },
	                        items: [
	                            {
	 								xtype: 'container',
	                                id: 'livepanel',
	                                layout: {
	                                    type: 'fit'
	                                },
	                                title: 'Live',
									items: [
	                                    {
	                                        //itemTpl: '<div>{title}  {presence} </div> ',                                    	
	                                        xtype: 'srcportlist'
	                                    }
	                                ]                                        
	                            },
	                            {
	                                xtype: 'container',
	                                title: 'Recordings',
	                                id: 'recordingpanel',
	                                layout: {
	                                    type: 'fit'
	                                },
	                                title: 'Recording',
									items: [
	                                    {
	                                        //itemTpl: '<div>{title}  {presence} </div> ',                                    	
	                                        xtype: 'recordinglist'
	                                    }
	                                ]    
	                            }
	                        ]
	                    },                           
	                    {
	                        xtype: 'container',
	                        flex: 1,
	                        id: 'destpanel',
							margin: '0 0 0 10',
	                        cls: 'display-panel-container',
	                        layout: {
	                            type: 'fit'
	                        },
	                       
	                        items: [
	                            {
	                                //itemTpl:'</a><h2>{title}</h2></p>' ,                                     	
	                                xtype: 'dstportlist'
	                            }
	                        ]
	                    }
                ]
            }
        ]						
    }

});