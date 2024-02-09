/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.SettingsPage', {
    extend: 'Ext.Container',
	alias: 'widget.settingpage',
	
    config: {
    	id: 'settingsscreen',
        layout: {
            type: 'vbox'
        },
		padding: '20 20 20 20',        
        items: [
            {
                xtype: 'image',
                height: 50,
                margin: '0 0 20 0',
                width: 245,
                src: 'app/resource/image/BarcoTextLogoRed.png'
            },
            {
                xtype: 'container',
                height: 100,
                padding: '20 0 0 10',
                layout: {
                    type: 'hbox'
                },
                items: [
                    {
                        xtype: 'image',
                        height: 50,
                        width: 50,
                        src: 'app/resource/image/cog.png'
                    },
                    {
                        xtype: 'label',
                        centered: false,
                        height: 64,
                        html: 'Setup',
                        padding: '6 0 0 20',
                        style: 'font-size: 2em; color:#777;',
                        width: 300
                    }
                ]
            },
            {
                xtype: 'formpanel',
                id: 'settingsFormPanle',		                
                height: 199,
                scrollable: false,
                items: [
                    {
                        xtype: 'fieldset',
                        centered: false,
                        instructions: '',
                        items: [
                            {
                                xtype: 'textfield',
                                style: 'font-size: 1.4em',
                                label: 'Server IP',
								name: 'serverip',		                                
                                labelWrap: true
                            },
                            {
                                xtype: 'textfield',
                                style: 'font-size: 1.4em',
                                label: 'Client ID',
                                name: 'clientid',		                                
                                labelWrap: true
                            }
                        ]
                    }
                ]
            },
			{
                xtype: 'container',
                items: [
                    {
                        xtype: 'button',
                        centered: true,
                        height: 44,
                        style: 'font-size: 1.4em;',
                        width: '50%',
                        text: 'Next'
                    }
                ]
            }            
        ]                
    }

});