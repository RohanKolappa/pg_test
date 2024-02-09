/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.LoginPage', {
    extend: 'Ext.Container',
    alias:'widget.loginpage',

    config: {
	    id: 'loginscreen',	     	
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
                        src: 'app/resource/image/user.png'
                    },
                    {
                        xtype: 'label',
                        centered: false,
                        height: 64,
                        html: 'Login',
                        padding: '6 0 0 20',
                        style: 'font-size: 2em; color:#777;',
                        width: 300
                    }
                ]
            },
            {
                xtype: 'formpanel',
                height: 199,
                id: 'loginFormPanle',
                scrollable: false,
                items: [
                    {
                        xtype: 'fieldset',
                        centered: false,
                        instructions: '',
                        items: [
                            {
                                xtype: 'textfield',
                                id: 'userfield',
                                style: 'font-size: 1.4em',
                                label: 'Username',
                                name: 'username',			                                
                                labelWrap: true
                            },
                            {
                                xtype: 'passwordfield',
                                id: 'passwordfield',
                                style: 'font-size: 1.4em',
                                label: 'Password',
                                name: 'password',			                                
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
                        text: 'Sign In'
                    },
                    {
                        xtype: 'button',
                        id: 'btnSettings',
                        left: 40,
                        padding: '6 0 0 0 ',
                        width: 40,
                        iconCls: 'settings',
                        text: ''
                    }
                ]
            }           
        ]						
    }

});