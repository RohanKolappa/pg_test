{
    "xdsVersion": "2.2.0",
    "frameworkVersion": "touch21",
    "internals": {
        "type": "Ext.Container",
        "reference": {
            "name": "items",
            "type": "array"
        },
        "codeClass": null,
        "userConfig": {
            "cls": [
                "bg"
            ],
            "id": "appcardctn",
            "designer|userClassName": "AppContainer",
            "designer|initialView": true,
            "layout": "card"
        },
        "cn": [
            {
                "type": "Ext.Container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "cls": [
                        "bg"
                    ],
                    "id": "settingsscreen",
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "settingscontainer",
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "Ext.Img",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "height": 50,
                            "id": "imgLogo",
                            "margin": "10 10 10 10",
                            "width": 245,
                            "designer|userClassName": "MyImg",
                            "src": "app/resource/image/barcoTextLogoRed.png"
                        }
                    },
                    {
                        "type": "Ext.Container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "settingsdlgpanel",
                            "width": "",
                            "designer|userClassName": "MyContainer",
                            "designer|displayName": "settingsPanel"
                        },
                        "cn": [
                            {
                                "type": "Ext.Toolbar",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "docked": "top",
                                    "html": null,
                                    "ui": "red",
                                    "designer|userClassName": "MyToolbar1",
                                    "title": "Settings"
                                }
                            },
                            {
                                "type": "Ext.form.Panel",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "height": 150,
                                    "id": "settingsFormPanle",
                                    "margin": null,
                                    "padding": "20 20 20 20",
                                    "top": null,
                                    "ui": "",
                                    "width": 400,
                                    "designer|userClassName": "MyFormPanel1",
                                    "designer|displayName": "settingsFormPanel"
                                },
                                "cn": [
                                    {
                                        "type": "Ext.form.FieldSet",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "height": null,
                                            "left": null,
                                            "style": "background:none",
                                            "top": null,
                                            "width": null,
                                            "designer|userClassName": "MyFieldSet1",
                                            "designer|displayName": "settingsfieldset",
                                            "title": null
                                        },
                                        "cn": [
                                            {
                                                "type": "Ext.field.Text",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField3",
                                                    "designer|displayName": "serverip",
                                                    "label": "Server IP",
                                                    "labelWidth": "40%",
                                                    "name": "serverip"
                                                }
                                            },
                                            {
                                                "type": "Ext.field.Text",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField4",
                                                    "designer|displayName": "clientid",
                                                    "label": "Client ID",
                                                    "labelWidth": "40%",
                                                    "name": "clientid"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "Ext.Button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "left": 40,
                                    "style": null,
                                    "top": 200,
                                    "width": 320,
                                    "designer|userClassName": "MyButton",
                                    "designer|displayName": "btnNext",
                                    "text": "Next"
                                }
                            }
                        ]
                    }
                ]
            },
            {
                "type": "Ext.Container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "cls": [
                        "bg"
                    ],
                    "id": "loginscreen",
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "logincontainer",
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "Ext.Img",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "height": 50,
                            "id": "imgLogo1",
                            "margin": "10 10 10 10",
                            "width": 245,
                            "designer|userClassName": "MyImg",
                            "src": "app/resource/image/barcoTextLogoRed.png"
                        }
                    },
                    {
                        "type": "Ext.Container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "logindlgpanel",
                            "style": null,
                            "ui": "",
                            "width": "",
                            "designer|userClassName": "MyContainer",
                            "designer|displayName": "loginpanel"
                        },
                        "cn": [
                            {
                                "type": "Ext.Toolbar",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "docked": "top",
                                    "html": null,
                                    "ui": "red",
                                    "designer|userClassName": "MyToolbar1",
                                    "title": "User Login"
                                }
                            },
                            {
                                "type": "Ext.form.Panel",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "height": 178,
                                    "id": "loginFormPanle",
                                    "padding": "20 20 20 20",
                                    "width": 402,
                                    "designer|userClassName": "MyFormPanel1",
                                    "designer|displayName": "loginFormPanel"
                                },
                                "cn": [
                                    {
                                        "type": "Ext.form.FieldSet",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "height": null,
                                            "left": null,
                                            "style": "background:none",
                                            "top": null,
                                            "width": null,
                                            "designer|userClassName": "MyFieldSet1",
                                            "designer|displayName": "loginfieldset",
                                            "title": null
                                        },
                                        "cn": [
                                            {
                                                "type": "Ext.field.Text",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "id": "userfield",
                                                    "margin": null,
                                                    "width": null,
                                                    "designer|userClassName": "MyTextField3",
                                                    "designer|displayName": "userfield",
                                                    "label": "Username",
                                                    "labelWidth": "40%",
                                                    "name": "userfield"
                                                }
                                            },
                                            {
                                                "type": "Ext.field.Text",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "id": "passwordfield",
                                                    "width": null,
                                                    "designer|userClassName": "MyTextField4",
                                                    "designer|displayName": "passwordfield",
                                                    "label": "Password",
                                                    "labelWidth": "40%",
                                                    "name": "passwordfield"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "Ext.Button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "left": 40,
                                    "style": null,
                                    "top": 200,
                                    "width": 320,
                                    "designer|userClassName": "MyButton",
                                    "designer|displayName": "btnLogin",
                                    "text": "Sign In"
                                }
                            },
                            {
                                "type": "Ext.Button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "id": "btnSettings",
                                    "left": 40,
                                    "top": 280,
                                    "designer|userClassName": "MyButton3",
                                    "iconCls": "settings9",
                                    "iconMask": true,
                                    "text": ""
                                }
                            }
                        ]
                    }
                ]
            },
            {
                "type": "Ext.Container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "cls": [
                        "bg"
                    ],
                    "id": "mainscreen",
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "mainscreen",
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "Ext.Toolbar",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "docked": "top",
                            "padding": null,
                            "ui": "red",
                            "designer|userClassName": "MyToolbar",
                            "designer|displayName": "headertoolbar"
                        },
                        "cn": [
                            {
                                "type": "Ext.Img",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "height": 25,
                                    "width": 150,
                                    "designer|userClassName": "MyImage",
                                    "designer|displayName": "imglogo",
                                    "src": "app/resource/image/BarcoTextLogoWhite.png"
                                }
                            },
                            {
                                "type": "Ext.Spacer",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MySpacer",
                                    "designer|displayName": "spacer"
                                }
                            },
                            {
                                "type": "Ext.Button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "margin": "0 20 0 0",
                                    "padding": null,
                                    "designer|userClassName": "MyButton",
                                    "designer|displayName": "btnSettings",
                                    "iconAlign": "right",
                                    "iconCls": "arrow_down",
                                    "iconMask": true,
                                    "text": "User Name"
                                }
                            }
                        ]
                    },
                    {
                        "type": "Ext.Container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "id": "mainpanel",
                            "padding": "8 8 8 8",
                            "designer|userClassName": "MyContainer6",
                            "designer|displayName": "mainbody",
                            "layout": "hbox"
                        },
                        "cn": [
                            {
                                "type": "Ext.Container",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "layout|flex": 1,
                                    "id": "sourcepanel",
                                    "margin": "0 10 0 0",
                                    "padding": "10 10 10 10",
                                    "designer|userClassName": "MyContainer",
                                    "designer|displayName": "sourcepanel",
                                    "layout": "fit"
                                },
                                "cn": [
                                    {
                                        "type": "Ext.dataview.List",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "designer|userClassName": "MyList",
                                            "designer|displayName": "sourcelist"
                                        },
                                        "cn": [
                                            {
                                                "type": "Ext.XTemplate",
                                                "reference": {
                                                    "name": "itemTpl",
                                                    "type": "object"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "itemTpl",
                                                    "implHandler": [
                                                        "<div>List Item {string}</div>"
                                                    ],
                                                    "designer|displayName": "itemTpl"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "Ext.Container",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "layout|flex": 1,
                                    "id": "destpanel",
                                    "padding": "10 10 10 10",
                                    "designer|userClassName": "MyContainer2",
                                    "designer|displayName": "destpanel",
                                    "layout": "fit"
                                },
                                "cn": [
                                    {
                                        "type": "Ext.dataview.List",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "margin": "0 0 0 10",
                                            "designer|userClassName": "MyList1"
                                        },
                                        "cn": [
                                            {
                                                "type": "Ext.XTemplate",
                                                "reference": {
                                                    "name": "itemTpl",
                                                    "type": "object"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "itemTpl",
                                                    "implHandler": [
                                                        "<div>List Item {string}</div>"
                                                    ],
                                                    "designer|displayName": "itemTpl"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        "type": "Ext.Container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "container|pack": "center",
                            "docked": "bottom",
                            "height": 60,
                            "designer|userClassName": "MyContainer9",
                            "designer|displayName": "controllerbar",
                            "layout": "hbox"
                        },
                        "cn": [
                            {
                                "type": "Ext.Button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "disabled": true,
                                    "height": 44,
                                    "id": "btnconnect",
                                    "ui": "action-round",
                                    "width": 300,
                                    "designer|userClassName": "MyButton3",
                                    "designer|displayName": "btnConnect",
                                    "text": "Connect"
                                }
                            }
                        ]
                    }
                ]
            },
            {
                "type": "Ext.Panel",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "height": 140,
                    "hidden": true,
                    "right": 20,
                    "top": 45,
                    "width": 200,
                    "designer|userClassName": "MyPanel",
                    "designer|displayName": "menudroplist",
                    "layout": "vbox",
                    "modal": true
                },
                "cn": [
                    {
                        "type": "Ext.Button",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "id": "btnmenusettings",
                            "margin": "20 8 8 8",
                            "designer|userClassName": "MyButton5",
                            "designer|displayName": "btnMenuSettings",
                            "text": "Settings"
                        }
                    },
                    {
                        "type": "Ext.Button",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "id": "btnmenulogout",
                            "margin": "8 8 8 8",
                            "designer|userClassName": "MyButton5",
                            "designer|displayName": "btnMenuLogout",
                            "text": "Sign Out"
                        }
                    }
                ]
            },
            {
                "type": "Ext.form.Panel",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "height": 500,
                    "hidden": true,
                    "right": 20,
                    "top": 45,
                    "width": 700,
                    "designer|userClassName": "MyFormPanel2",
                    "designer|displayName": "mainSettingsPanel",
                    "hideOnMaskTap": true,
                    "modal": true
                },
                "cn": [
                    {
                        "type": "Ext.form.FieldSet",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "height": null,
                            "left": null,
                            "style": "background:none",
                            "top": null,
                            "width": null,
                            "designer|userClassName": "MyFieldSet1",
                            "designer|displayName": "settingsfieldset",
                            "title": "Default Settings"
                        },
                        "cn": [
                            {
                                "type": "Ext.field.Text",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyTextField3",
                                    "designer|displayName": "serverip",
                                    "label": "Server IP",
                                    "labelWidth": "40%",
                                    "name": "serverip"
                                }
                            },
                            {
                                "type": "Ext.field.Text",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyTextField4",
                                    "designer|displayName": "clientid",
                                    "label": "Client ID",
                                    "labelWidth": "40%",
                                    "name": "clientid"
                                }
                            }
                        ]
                    }
                ]
            }
        ]
    },
    "linkedNodes": {},
    "boundStores": {},
    "boundModels": {}
}