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
            "designer|userClassName": "AppContainer",
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
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "settingscontainer",
                    "layout": "fit"
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
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "setingsscreen",
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
                                    "height": 140,
                                    "id": "settingsFormPanle",
                                    "margin": null,
                                    "padding": "20 20 20 20",
                                    "top": null,
                                    "ui": "",
                                    "width": 400,
                                    "designer|userClassName": "MyFormPanel1"
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
                                "type": "Ext.Label",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "cls": [
                                        "errMsg"
                                    ],
                                    "docked": null,
                                    "height": 40,
                                    "id": "settingErrorMessageBar",
                                    "top": null,
                                    "width": 400,
                                    "designer|userClassName": "MyLabel"
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
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "logincontainer",
                    "layout": "fit"
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
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "loginscreen",
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
                                    "designer|userClassName": "MyFormPanel1"
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
                                                    "margin": null,
                                                    "width": null,
                                                    "designer|userClassName": "MyTextField3",
                                                    "designer|displayName": "userfield",
                                                    "label": "Username",
                                                    "labelWidth": "40%",
                                                    "name": "username"
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
                                                    "width": null,
                                                    "designer|userClassName": "MyTextField4",
                                                    "designer|displayName": "passwordfield",
                                                    "label": "Password",
                                                    "labelWidth": "40%",
                                                    "name": "password"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "Ext.Label",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "cls": [
                                        "errMsg"
                                    ],
                                    "docked": null,
                                    "height": 30,
                                    "id": "loginErrorMessageBar",
                                    "top": null,
                                    "width": 400,
                                    "designer|userClassName": "MyLabel"
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
                                    "top": 300,
                                    "designer|userClassName": "MyButton3",
                                    "iconCls": "settings",
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
                            "style": null,
                            "ui": "red",
                            "designer|userClassName": "MyToolbar",
                            "designer|displayName": "headertoolbar"
                        },
                        "cn": [
                            {
                                "type": "Ext.field.Select",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "docked": "right",
                                    "designer|userClassName": "MySelectField",
                                    "designer|displayName": "settings",
                                    "label": "",
                                    "options": "[{text: 'Logout',  value: 'Logout'},\r\n{text: 'Settings', value: 'Settings'},\r\n{text: 'Third',  value: 'third'}]"
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
                    }
                ]
            }
        ]
    },
    "linkedNodes": {},
    "boundStores": {},
    "boundModels": {}
}