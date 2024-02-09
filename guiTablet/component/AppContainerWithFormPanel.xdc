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
            "container|animation": "fade",
            "designer|userClassName": "AppContainer",
            "designer|initialView": true,
            "cls": [
                "bg"
            ],
            "ui": "",
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
                    "designer|userClassName": "mainapp",
                    "designer|displayName": "mainscreen",
                    "id": "mainscreen",
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
                            "designer|userClassName": "MyContainer2",
                            "designer|displayName": "mainpanel",
                            "id": "mainpanel",
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
                                    "ui": "light"
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
                                            "padding": "10 10 10 10"
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
                                                    "designer|displayName": "sourcelist"
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
                                            "padding": "10 10 10 10"
                                        },
                                        "cn": [
                                            {
                                                "type": "carousel",
                                                "reference": {
                                                    "name": "items",
                                                    "type": "array"
                                                },
                                                "codeClass": null,
                                                "userConfig": {
                                                    "designer|userClassName": "MyCarousel",
                                                    "designer|displayName": "destcarousel"
                                                }
                                            }
                                        ]
                                    }
                                ]
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
                    "designer|userClassName": "MyContainer",
                    "designer|displayName": "logincontainer",
                    "centered": true,
                    "cls": [
                        "loginPanel2"
                    ],
                    "height": "",
                    "hidden": true,
                    "id": "loginscreen",
                    "width": ""
                },
                "cn": [
                    {
                        "type": "formpanel",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyFormPanel1",
                            "height": 210,
                            "id": "loginFormPanle",
                            "width": 398
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
                                    "height": 138,
                                    "left": 40,
                                    "style": "background:none",
                                    "top": 46,
                                    "width": 320,
                                    "title": "Sign In"
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
                                            "width": 320,
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
                                            "width": 320,
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
                            "docked": "bottom",
                            "height": 42,
                            "id": "loginErrorMessageBar",
                            "top": 220,
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
                            "left": 240,
                            "style": null,
                            "top": 300,
                            "ui": "confirm",
                            "width": 120,
                            "text": "Login"
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
                            "text": "Settings"
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
                    "designer|userClassName": "MyContainer",
                    "designer|displayName": "settingscontainer",
                    "centered": true,
                    "cls": [
                        "loginPanel2"
                    ],
                    "height": "",
                    "hidden": true,
                    "id": "setingsscreen",
                    "width": ""
                },
                "cn": [
                    {
                        "type": "formpanel",
                        "reference": {
                            "name": "items",
                            "type": "array"
                        },
                        "codeClass": null,
                        "userConfig": {
                            "designer|userClassName": "MyFormPanel1",
                            "height": 279,
                            "id": "settingsFormPanle",
                            "width": 400
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
                                    "height": 140,
                                    "left": 40,
                                    "style": "background:none",
                                    "top": 40,
                                    "width": 320,
                                    "title": "Settings"
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
                            "docked": "bottom",
                            "height": 42,
                            "id": "settingErrorMessageBar",
                            "top": 220,
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
                            "left": 240,
                            "style": null,
                            "top": 300,
                            "width": 120,
                            "text": "Next"
                        }
                    }
                ]
            }
        ]
    },
    "linkedNodes": {},
    "boundStores": {},
    "boundModels": {}
}