/**
 * @author ZHEN
 */

Ext.define('TabletGui.view.AppContainer', {
  extend: 'Ext.Container',
	alias:'widget.main',
	 
  config: {
    cls: 'bg',
    layout: {
      type: 'card'
    },
    id: 'appcardctn',
    activeItem:0,
    items: [
            {
             xtype: 'settingpage'
            },
            
            {
             xtype: 'loginpage'
            },
            {
             xtype: 'mainpage'
            }
      ]
  }
});