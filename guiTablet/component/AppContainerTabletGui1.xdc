{
    "xdsVersion": "2.1.0",
    "frameworkVersion": "touch21",
    "internals": {
        "type": "container",
        "reference": {
            "name": "items",
            "type": "array"
        },
        "codeClass": null,
        "userConfig": {
            "designer|userClassName": "AppContainer",
            "cls": [
                "bg"
            ],
            "layout": "card"
        },
        "cn": [
            {
                "type": "container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "settingscontainer",
                    "cls": [
                        "bg"
                    ],
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyContainer",
                            "designer|displayName": "settingsPanel",
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "setingsscreen",
                            "width": ""
                        },
                        "cn": [
                            {
                                "type": "toolbar",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyToolbar1",
                                    "docked": "top",
                                    "html": null,
                                    "ui": "red",
                                    "title": "Settings"
                                }
                            },
                            {
                                "type": "formpanel",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyFormPanel1",
                                    "height": 140,
                                    "id": "settingsFormPanle",
                                    "margin": null,
                                    "padding": "20 20 20 20",
                                    "top": null,
                                    "ui": "",
                                    "width": 400,
                                    "scrollable": [
                                        "false"
                                    ]
                                },
                                "cn": [
                                    {
                                        "type": "fieldset",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "designer|userClassName": "MyFieldSet1",
                                            "designer|displayName": "settingsfieldset",
                                            "height": null,
                                            "left": null,
                                            "style": "background:none",
                                            "top": null,
                                            "width": null,
                                            "layout": "default",
                                            "title": null
                                        },
                                        "cn": [
                                            {
                                                "type": "textfield",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField3",
                                                    "designer|displayName": "serverip",
                                                    "label": "Server IP",
                                                    "labelWidth": "40%"
                                                }
                                            },
                                            {
                                                "type": "textfield",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField4",
                                                    "designer|displayName": "clientid",
                                                    "label": "Client ID",
                                                    "labelWidth": "40%"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "label",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyLabel",
                                    "cls": [
                                        "errMsg"
                                    ],
                                    "docked": null,
                                    "height": 40,
                                    "id": "settingErrorMessageBar",
                                    "top": null,
                                    "width": 400
                                }
                            },
                            {
                                "type": "button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyButton",
                                    "designer|displayName": "btnNext",
                                    "left": 40,
                                    "style": null,
                                    "top": 200,
                                    "width": 320,
                                    "text": "Next"
                                }
                            }
                        ]
                    }
                ]
            },
            {
                "type": "container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "logincontainer",
                    "cls": [
                        "bg"
                    ],
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyContainer",
                            "designer|displayName": "loginpanel",
                            "centered": true,
                            "cls": [
                                "dialogPanel"
                            ],
                            "height": 400,
                            "id": "loginscreen",
                            "style": null,
                            "ui": "",
                            "width": ""
                        },
                        "cn": [
                            {
                                "type": "toolbar",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyToolbar1",
                                    "docked": "top",
                                    "html": null,
                                    "ui": "red",
                                    "title": "User Login"
                                }
                            },
                            {
                                "type": "formpanel",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyFormPanel1",
                                    "height": 178,
                                    "id": "loginFormPanle",
                                    "padding": "20 20 20 20",
                                    "width": 402
                                },
                                "cn": [
                                    {
                                        "type": "fieldset",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "designer|userClassName": "MyFieldSet1",
                                            "designer|displayName": "loginfieldset",
                                            "height": null,
                                            "left": null,
                                            "style": "background:none",
                                            "top": null,
                                            "width": null,
                                            "title": null
                                        },
                                        "cn": [
                                            {
                                                "type": "textfield",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField3",
                                                    "designer|displayName": "userfield",
                                                    "margin": null,
                                                    "width": null,
                                                    "label": "Username",
                                                    "labelWidth": "40%"
                                                }
                                            },
                                            {
                                                "type": "textfield",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyTextField4",
                                                    "designer|displayName": "passwordfield",
                                                    "width": null,
                                                    "label": "Password",
                                                    "labelWidth": "40%"
                                                }
                                            }
                                        ]
                                    }
                                ]
                            },
                            {
                                "type": "label",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyLabel",
                                    "cls": [
                                        "errMsg"
                                    ],
                                    "docked": null,
                                    "height": 30,
                                    "id": "loginErrorMessageBar",
                                    "top": null,
                                    "width": 400
                                }
                            },
                            {
                                "type": "button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyButton",
                                    "designer|displayName": "btnLogin",
                                    "left": 40,
                                    "style": null,
                                    "top": 200,
                                    "width": 320,
                                    "text": "Sign In"
                                }
                            },
                            {
                                "type": "button",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MyButton3",
                                    "id": "btnSettings",
                                    "left": 40,
                                    "top": 300,
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
                "type": "container",
                "reference": {
                    "name": "items",
                    "type": "array"
                },
                "codeClass": null,
                "userConfig": {
                    "designer|userClassName": "MyContainer9",
                    "designer|displayName": "mainscreen",
                    "cls": [
                        "bg"
                    ],
                    "layout": "fit"
                },
                "cn": [
                    {
                        "type": "toolbar",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyToolbar",
                            "designer|displayName": "headertoolbar",
                            "docked": "top",
                            "style": null,
                            "ui": "red"
                        },
                        "cn": [
                            {
                                "type": "selectfield",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "designer|userClassName": "MySelectField",
                                    "designer|displayName": "settings",
                                    "docked": "right",
                                    "label": "",
                                    "options": "[{text: 'Logout',  value: 'Logout'},\r\n{text: 'Settings', value: 'Settings'},\r\n{text: 'Third',  value: 'third'}]"
                                }
                            }
                        ]
                    },
                    {
                        "type": "container",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyContainer6",
                            "designer|displayName": "mainbody",
                            "padding": "8 8 8 8",
                            "layout": "hbox"
                        },
                        "cn": [
                            {
                                "type": "container",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "layout|flex": 1,
                                    "designer|userClassName": "MyContainer",
                                    "designer|displayName": "sourcepanel",
                                    "margin": "0 10 0 0",
                                    "padding": "10 10 10 10",
                                    "layout": "fit"
                                },
                                "cn": [
                                    {
                                        "type": "list",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "designer|userClassName": "MyList",
                                            "designer|displayName": "sourcelist",
                                            "layout": "fit",
                                            "scrollable": [
                                                "vertical"
                                            ]
                                        },
                                        "cn": [
                                            {
                                                "type": "template",
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
                                "type": "container",
                                "reference": {
                                    "name": "items",
                                    "type": "array"
                                },
                                "codeClass": null,
                                "userConfig": {
                                    "layout|flex": 1,
                                    "designer|userClassName": "MyContainer2",
                                    "designer|displayName": "destpanel",
                                    "padding": "10 10 10 10",
                                    "layout": "fit"
                                },
                                "cn": [
                                    {
                                        "type": "list",
                                        "reference": {
                                            "name": "items",
                                            "type": "array"
                                        },
                                        "codeClass": null,
                                        "userConfig": {
                                            "designer|userClassName": "MyList1",
                                            "margin": "0 0 0 10"
                                        },
                                        "cn": [
                                            {
                                                "type": "template",
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