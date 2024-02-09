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
            "designer|userClassName": "LoginContainer",
            "designer|initialView": true,
            "cls": [
                "bg"
            ]
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
                    "designer|displayName": "loginformpanel",
                    "centered": true,
                    "height": 390,
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
                            "designer|displayName": "loginfieldset",
                            "left": 40,
                            "top": 40,
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
                                    "label": "Username",
                                    "labelWidth": "40%",
                                    "name": "user"
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
                                    "label": "Password",
                                    "labelWidth": "40%",
                                    "name": "password"
                                }
                            }
                        ]
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
                            "height": 36,
                            "left": 220,
                            "style": null,
                            "top": 220,
                            "ui": "confirm",
                            "width": 153,
                            "text": "Login"
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