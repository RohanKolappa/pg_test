/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Define rules as JSON format
*/

jsx3.Class.defineClass("com.ipvs.api.RequestRules", null, null, function (RequestRules, RequestRules_prototype) {

    /** @private @jsxobf-clobber */
    RequestRules_prototype._LOG = jsx3.util.Logger.getLogger(RequestRules.jsxclass.getName());

    //login request
    RequestRules.LoginRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "xmpp", "requestname": "LoginRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "LoginRequestData": { "user": "", "password": "", "resource": "", "server": "", "port": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToLoginRequest",
            "setValueAndAttribute": null
        }
    };

    //logout request
    RequestRules.LogoutRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "xmpp", "requestname": "LogoutRequest", "loglevel": "0", "clientdata": ""
                },
                "data": { "LoginRequestData": "Logout"
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToLogoutRequest",
            "setValueAndAttribute": null
        }
    };


    //login setServiceDomainRequest
    RequestRules.SetServiceDomainRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "SetServiceDomainRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "SetServiceDomainRequestData": { "domain": "default", "AssetTemplateTitle": "", "AssetPrivateKeyData": "", "ServiceVersion": "", "SoftwareVersion": "", "IPAddress": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToSetServiceDomainRequest",
            "setValueAndAttribute": null
        }

    };

    //UpgradeRequest
    RequestRules.UpgradeRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "UpgradeRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "UpgradeRequestData": {
                        "Upgrade":{ "@deviceType": "", "FromDirPath": ""
                        }
                     }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_UpgradeRequest",
            "setValueAndAttribute": null
        }

    };

    //RestoreDBRequest
    RequestRules.RestoreDBRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "RestoreDBRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "RestoreDBRequestData": { "RestoreFrom": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_RestoreDBRequest",
            "setValueAndAttribute": null
        }

    };


    //ImportServiceDBRequest
    RequestRules.ImportServiceDBRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "ImportServiceDBRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "ImportServiceDBRequestData": { "LocalFileName": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_ImportServiceDBRequest",
            "setValueAndAttribute": null
        }

    };

    //SaveDBRequest
    RequestRules.SaveDBRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "SaveDBRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "SaveDBRequestData": ""
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_SaveDBRequest",
            "setValueAndAttribute": null
        }

    };

    //BackupDatabaseRequest
    RequestRules.BackupDatabaseRequest =
    {
      "Body": {
        "Request": {
          "@serviceVer": "", "@Ver": "1.1",
          "header": {
            "servicename": "sessionservice.service", "requestname": "BackupDatabaseRequest", "loglevel": "0", "clientdata": ""
          },
          "data": {
            "BackupDatabaseRequestData": { "BackupDatabase": { "LocalFileName": "" } }
          }
        }
      },
      "CallBack": {
        "setValue": "RequestRules.setValue_BackupDatabaseRequest",
        "setValueAndAttribute": null
      }

    };

    //ExportServiceDBRequest
    RequestRules.ExportServiceDBRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice", "requestname": "ExportServiceDBRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "ExportServiceDBRequestData": { "LocalFileName": "" }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_ExportServiceDBRequest",
            "setValueAndAttribute": null
        }

    };

    //login getServiceClientRequest
    RequestRules.GetServiceClientRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetServiceClient": { "@whereContactJID": "", "@selectFormat": "EXTENDED" }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToGetServiceClientRequest",
            "setValueAndAttribute": "RequestRules.setValueAndAttrToGetServiceClientRequest"
        }

    };

    //login getServerTimeRequest
    RequestRules.GetServerTimeRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "systemservice",
                    "requestname": "GetServerTimeRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "GetServerTimeRequestData": ""
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToGetServerTimeRequest",
            "setValueAndAttribute": null
        }
    };

    //AuditEventRequest
    RequestRules.AuditEventRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "AuditEvent": {
                         "Description": {
                             "EventUser": {
                                 "User": {
                                     "UserPrivateData": "",
                                     "Groups": "",
                                     "@NID": "", "@userName": "", "@externalAuthentication": ""
                                 }
                             },
                             "EventAsset": "",
                             "MetaData": "",
                             "@text": ""
                         },
                         "@type": "", "@date": "", "@time": "", "@level": "", "@name": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_AuditEventRequest"
         }
     };

    // !!!*** BELOW AgentMessage BLOCK ****!!!!!

    //DeviceConfigurationRequest
    RequestRules.DeviceConfigurationRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "Request": { "@serviceVer": "", "@Ver": "1.1",
                     "header": { "servicename": "DeviceAdmin", "requestname": "DeviceConfigurationRequest", "loglevel": "0", "clientdata": ""
                     },
                     "data": {
                         "DeviceConfigurationRequestData": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_DeviceConfigurationRequest",
             "setValueAndAttribute": null
         }
     };

    //FileXferSetupRequest
    RequestRules.FileXferSetupRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "Request": { "@serviceVer": "", "@Ver": "1.1",
                     "header": { "servicename": "FileTransfer", "requestname": "FileXferSetupRequest", "loglevel": "0", "clientdata": ""
                     },
                     "data": {
                         "FileXferSetupRequestData": { "FileXferID": "", "FileXfer": "" }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_FileXferSetupRequest",
             "setValueAndAttribute": null
         }
     };

    //FileXferStopRequest
    RequestRules.FileXferStopRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "Request": { "@serviceVer": "", "@Ver": "1.1",
                     "header": { "servicename": "FileTransfer", "requestname": "FileXferStopRequest", "loglevel": "0", "clientdata": ""
                     },
                     "data": {
                         "FileXferStopRequestData": { "FileXferID": "" }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_FileXferStopRequest",
             "setValueAndAttribute": null
         }
     };

    //DeviceStatusRequest
    RequestRules.GetDeviceStatusRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "Request": { "@serviceVer": "", "@Ver": "1.1",
                     "header": { "servicename": "DeviceAdmin", "requestname": "GetDeviceStatusRequest", "loglevel": "0", "clientdata": ""
                     },
                     "data": {
                         "GetDeviceStatusRequestData": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_GetDeviceStatusRequest",
             "setValueAndAttribute": null
         }
     };


    //AgentEventRequest
    RequestRules.AgentEventRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "agentmessagedata": ""
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_AgentEventRequest",
             "setValueAndAttribute": null
         }
     };

    // !!!*** ABOVE AgentMessage BLOCK ****!!!!!

    //GetMediaRoomListRequest
    RequestRules.GetMediaRoomListRequest =
  {
      "Body": {
          "Request": { "@serviceVer": "", "@Ver": "1.1",
              "header": { "servicename": "mediaroomservice", "requestname": "GetMediaRoomListRequest", "loglevel": "0", "clientdata": ""
              },
              "data": {
                  "GetMediaRoomListRequestData": { "name": "", "mediaSourceXPath": "", "streamXPath": "", "sourceXPath": ""
                  }
              }
          }
      },
      "CallBack": {
          "setValue": "RequestRules.setValue_GetMediaRoomListRequest",
          "setValueAndAttribute": null
      }

  };


    // !!!*** BELOW AssetAdminService BLOCK ****!!!!!

    //GetAssetManagementRequest
    RequestRules.GetAssetManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AssetManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AssetManagementRequestData": {
                         "GetRequest": {
                             "PathSpec": {
                                 "Select": {
                                     "ListSpec": {
                                         "SortByXPathList": {
                                             "SortByXpath": ""
                                         },
                                         "StartFrom": "",
                                         "CountToFetch": ""
                                     },
                                     "ItemSpec": {
                                         "SelectXML": "",
                                         "SelectXQuery": ""
                                     }
                                 },
                                 "Where": {
                                     "WhereXPath": ""
                                 }
                             },
                             "RequestID": "1",
                             "NID": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_GetAssetManagementRequest",
             "setValueAndAttribute": null
         }
     };


    //ReplaceAssetManagementRequest
    RequestRules.ReplaceAssetManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AssetManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AssetManagementRequestData": {
                         "ReplaceRequest": {
                             "PathSpec": {
                                 "Where": {
                                     "WhereXPath": "",
                                     "XQueryPrefix": ""
                                 },
                                 "In": {
                                     "ChildXPath": ""
                                 }
                             },
                             "RequestID": "1",
                             "NID": "",
                             "OperationData": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_ReplaceAssetManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //AddAssetManagementRequest
    RequestRules.AddAssetManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AssetManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AssetManagementRequestData": {
                         "AddRequest": {
                             "ParentPathSpec": {
                                 "Where": {
                                     "WhereAssistXML": "",
                                     "WhereXPath": ""
                                 },
                                 "Select": {
                                     "ItemSpec": {
                                         "SelectXML": ""
                                     }
                                 }
                             },
                             "RequestID": "1",
                             "OperationData": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_AddAssetManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //DeleteAssetManagementRequest
    RequestRules.DeleteAssetManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AssetManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AssetManagementRequestData": {
                         "DeleteRequest": {
                             "PathSpec": {
                                 "Where": {
                                     "WhereAssistXML": "",
                                     "WhereXPath": ""
                                 },
                                 "Select": {
                                     "ItemSpec": {
                                         "SelectXML": ""
                                     }
                                 }
                             },
                             "RequestID": "1",
                             "NID": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_DeleteAssetManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //QueueStatusExportImportFileRequest
    RequestRules.QueueStatusExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "QueueStatusExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "QueueStatusExportImportFileRequestData": {
                         "QueueStatusExportImportFile": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_QueueStatusExportImportFileRequest",
             "setValueAndAttribute": null
         }
     };

    //SuspendExportImportFileRequest
    RequestRules.SuspendExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "SuspendExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "SuspendExportImportFileRequestData": {
                         "SuspendExportImportFile": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_SuspendExportImportFileRequest",
             "setValueAndAttribute": null
         }
     };

    //ResumeExportImportFileRequest
    RequestRules.ResumeExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "ResumeExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "ResumeExportImportFileRequestData": {
                         "ResumeExportImportFile": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_ResumeExportImportFileRequest",
             "setValueAndAttribute": null
         }
     };

    //ClearQueueExportImportFileRequest
    RequestRules.ClearQueueExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "ClearQueueExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "ClearQueueExportImportFileRequestData": {
                         "ClearQueueExportImportFile": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_ClearQueueExportImportFileRequest",
             "setValueAndAttribute": null
         }
     };

    //DeleteExportImportFileRequest
    RequestRules.DeleteExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "DeleteExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "DeleteExportImportFileRequestData": {
                         "DeleteExportImportFile": { "@whereDateCreated": "", "@whereState": "", "@whereOwnerJID": "", "@whereType": "", "@wherePriority": "", "@whereNID": "", "@whereStateDescription": ""
                         }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_DeleteExportImportFileRequest"
         }
     };

    //UpdateExportImportFileRequest
    RequestRules.UpdateExportImportFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "UpdateExportImportFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "UpdateExportImportFileRequestData": {
                         "UpdateExportImportFile": { "@whereDateCreated": "", "@whereState": "", "@whereOwnerJID": "", "@whereType": "", "@wherePriority": "", "@whereNID": "", "@whereStateDescription": "",
                             "EIFLIPriority": "",
                             "EIFLIExternalFileName": "",
                             "EIFLIExternalJID": "",
                             "EIFLIExternalFolderPath": ""
                         }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_UpdateExportImportFileRequest"
         }
     };

    //DetachDirRequest
    RequestRules.DetachDirRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "DetachDirRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "DetachDirRequestData": {
                         "MediaStreamDirNID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_DetachDirRequest",
             "setValueAndAttribute": null
         }
     };

    //AttachDirRequest
    RequestRules.AttachDirRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AttachDirRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AttachDirRequestData": {
                         "MediaStreamDirNID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_AttachDirRequest",
             "setValueAndAttribute": null
         }
     };

    //EmptyDirRequest
    RequestRules.EmptyDirRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "EmptyDirRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "EmptyDirRequestData": {
                         "MediaStreamDirNID": "",
                         "ActionRequest": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_EmptyDirRequest",
             "setValueAndAttribute": null
         }
     };

    //CleanOrphanDirRequest
    RequestRules.CleanOrphanDirRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "CleanOrphanDirRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "CleanOrphanDirRequestData": {
                         "MediaStreamDirNID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_CleanOrphanDirRequest",
             "setValueAndAttribute": null
         }
     };

    //RepairMediaStreamFileRequest
    RequestRules.RepairMediaStreamFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "RepairMediaStreamFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "RepairMediaStreamFileRequestData": {
                         "MediaStreamFileResourceNID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_RepairMediaStreamFileRequest",
             "setValueAndAttribute": null
         }
     };

    //UpgradeMediaStreamDirResourceRequest
    RequestRules.UpgradeDirRequest =
    {
        "Body": {
            "iq": {
                "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "UpgradeDir": { "@byNID": "", "@byTitle": "", "@upgradeFiles": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_UpgradeDirRequest"
        }
    };

    //UpgradeMediaStreamFileResourceRequest
    RequestRules.UpgradeFileRequest =
    {
        "Body": {
            "iq": {
                "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "UpgradeFile": { "@byNID": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_UpgradeFileRequest"
        }
    };

    //UpdateMediaStreamFileResourceInfoRequest
    RequestRules.UpdateMediaStreamFileResourceInfoRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "UpdateMediaStreamFileResourceInfoRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": ""
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_UpdateMediaStreamFileResourceInfoRequest",
             "setValueAndAttribute": null
         }
     };

    //UpdateMediaStreamFileResourceStoreInfoRequest
    RequestRules.UpdateMediaStreamFileResourceStoreInfoRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "UpdateMediaStreamFileResourceStoreInfoRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": ""
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_UpdateMediaStreamFileResourceStoreInfoRequest",
             "setValueAndAttribute": null
         }
     };

    //RestoreMediaStreamFileRequest
    RequestRules.RestoreMediaStreamFileRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "RestoreMediaStreamFileRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "RestoreMediaStreamFileRequestData": {
                         "MediaStreamFileResourceNID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_RestoreMediaStreamFileRequest",
             "setValueAndAttribute": null
         }
     };

    //ReadQueryAssetManagementRequest
    RequestRules.ReadQueryAssetManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "assetadminservice", "requestname": "AssetManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "AssetManagementRequestData": {
                         "ReadQueryRequest": {
                             "RequestID": "1",
                             "Query": ""
                         }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_ReadQueryAssetManagementRequest",
             "setValueAndAttribute": null
         }
     };



    // !!!*** ABOVE AssetAdminService BLOCK ****!!!!!


    // !!!*** BELOW PolicyAdminService BLOCK ****!!!!!

    //GetPolicyManagementRequest
    RequestRules.GetPolicyManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "policyadminservice", "requestname": "PolicyManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "PolicyManagementRequestData": {
                         "GetRequest": {
                             "PathSpec": {
                                 "Select": {
                                     "ListSpec": {
                                         "SortByXPathList": {
                                             "SortByXpath": ""
                                         },
                                         "StartFrom": "",
                                         "CountToFetch": ""
                                     },
                                     "ItemSpec": {
                                         "SelectXML": "",
                                         "SelectXQuery": ""
                                     }
                                 },
                                 "Where": {
                                     "WhereXPath": ""
                                 }
                             },
                             "RequestID": "1",
                             "NID": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_GetPolicyManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //ReplacePolicyManagementRequest
    RequestRules.ReplacePolicyManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "policyadminservice", "requestname": "PolicyManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "PolicyManagementRequestData": {
                         "ReplaceRequest": {
                             "PathSpec": {
                                 "Where": {
                                     "WhereXPath": "",
                                     "XQueryPrefix": ""
                                 },
                                 "In": {
                                     "ChildXPath": ""
                                 }
                             },
                             "RequestID": "1",
                             "NID": "",
                             "OperationData": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_ReplacePolicyManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //AddPolicyManagementRequest
    RequestRules.AddPolicyManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "policyadminservice", "requestname": "PolicyManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "PolicyManagementRequestData": {
                         "AddRequest": {
                             "ParentPathSpec": {
                                 "Where": {
                                     "WhereAssistXML": "",
                                     "WhereXPath": ""
                                 },
                                 "Select": {
                                     "ItemSpec": {
                                         "SelectXML": ""
                                     }
                                 }
                             },
                             "RequestID": "1",
                             "ParentNID": "",
                             "OperationData": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_AddPolicyManagementRequest",
             "setValueAndAttribute": null
         }
     };

    //DeletePolicyManagementRequest
    RequestRules.DeletePolicyManagementRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "policyadminservice", "requestname": "PolicyManagementRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "PolicyManagementRequestData": {
                         "DeleteRequest": {
                             "ParentPathSpec": {
                                 "Where": {
                                     "WhereAssistXML": "",
                                     "WhereXPath": ""
                                 },
                                 "Select": {
                                     "ItemSpec": {
                                         "SelectXML": ""
                                     }
                                 }
                             },
                             "RequestID": "1",
                             "NID": ""
                         },
                         "ObjectType": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_DeletePolicyManagementRequest",
             "setValueAndAttribute": null
         }
     };


    // !!!*** ABOVE PolicyAdminService BLOCK ****!!!!!



    // !!!*** BELOW MediaRoomService BLOCK ****!!!!!

    //GetBookmarkListRequest
    RequestRules.GetBookmarkListRequest =
     {
         "Body": {
             "Request": { "@serviceVer": "", "@Ver": "1.1",
                 "header": { "servicename": "mediaroomservice", "requestname": "GetBookmarkListRequest", "loglevel": "0", "clientdata": ""
                 },
                 "data": {
                     "GetBookmarkListRequestData": {
                         "GetRequest": {
                             "PathSpec": {
                                 "Select": {
                                     "ListSpec": {
                                         "SortByXPathList": {
                                             "SortByXpath": ""
                                         },
                                         "StartFrom": "",
                                         "CountToFetch": ""
                                     },
                                     "ItemSpec": {
                                         "SelectXML": "",
                                         "SelectXQuery": ""
                                     }
                                 },
                                 "Where": {
                                     "WhereXPath": ""
                                 }
                             },
                             "RequestID": "1",
                             "MediaStreamFileResourceNID": ""
                         }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_GetBookmarkListRequest",
             "setValueAndAttribute": null
         }
     };

    // !!!*** ABOVE MediaRoomService BLOCK ****!!!!!





    // !!!*** BELOW IQ Request BLOCK ****!!!!!

    //SetFileMetaDataRequest
    RequestRules.SetFileMetaDataRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SetFileMetaDataRequest": {
                         "MetaData": "",
                         "@byNID": "", "@byTitle": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_SetFileMetaDataRequest"
         }
     };

    //SetFileBypassAutoStorageRulesRequest
    RequestRules.SetFileBypassAutoStorageRulesRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SetFileBypassAutoStorageRulesRequest": { "@byNID": "", "@bypassAutoStorageRules": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_SetFileBypassAutoStorageRulesRequest"
         }
     };

    //Get User List
    RequestRules.GetUserRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetUserRequest": { "@startFrom": "", "@countToFetch": "", "@sortBy": "", "@sortOrder": "", "@selectFormat": "", "@whereNID": "", "@whereUsername": "", "@whereTagExpr": "", "@whereTextSubStr": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetuserRequest"
        }
    };

    //Get Tag List
    RequestRules.GetTagsRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetTagsRequest": { "@startFrom": "", "@countToFetch": "", "@sortBy": "", "@sortOrder": "", "@selectFormat": "", "@whereNameTextSubString": "", "@whereValueTextSubString": "", "@whereObjectType": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetTagsRequest"
        }
    };

    //Get Port List
    RequestRules.GetPortRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetPortRequest": { "@sortBy": "", "@sortOrder": "", "@selectFormat": "", "@whereNID": "", "@type": "", "@wherePermissions": "", "@startFrom": "", "@countToFetch": "", "@whereTextSubStr": "", "@whereDateRange": "", "@whereTagExpr": "", "@subscriptionID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetPortRequest"
        }
    };

    //Get Dir List
    RequestRules.GetDirRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetDirRequest": { "@sortBy": "", "@sortOrder": "", "@selectFormat": "", "@whereNID": "", "@wherePermissions": "", "@startFrom": "", "@countToFetch": "", "@whereTextSubStr": "", "@whereFilePermissions": "", "@whereTagExpr": "", "@subscriptionID": "", "@whereParentDirNID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetDirRequest"
        }
    };

    //Get File List 
    RequestRules.GetFileRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
                "query": {
                    "GetFile": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "", "@searchTextSubStrIn": "", "@whereDateRange": "", "@whereTagExpr": "", "@selectFormat": "", "@whereParentDirNID": "", "@whereSessionConfigNID": "", "@sortByMetaData": "", "@sortByTagName": "", "@whereState": "", "@whereType": "", "@wherePermissions": "", "@whereMetaDataExpr": "", "@whereDownloadState": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetFileRequest"
        }
    };

    //Get File List 
    RequestRules.GetPVRFileRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
                "query": {
                    "GetPVRFile": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "", "@searchTextSubStrIn": "", "@whereDateRange": "", "@whereTagExpr": "", "@selectFormat": "", "@whereParentDirNID": "", "@whereSessionConfigNID": "", "@sortByMetaData": "", "@sortByTagName": "", "@whereState": "", "@wherePermissions": "", "@whereMetaDataExpr": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetPVRFileRequest"
        }
    };

    //Get Device List 
    RequestRules.GetDeviceRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
                "query": {
                    "GetDeviceRequest": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "", "@whereTagExpr": "", "@selectFormat": "", "@whereTitle": "", "@whereType": "", "@searchTextSubStrIn": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetDeviceRequest"
        }
    };

    RequestRules.GetDeviceConfigRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
                 "query": {
                     "GetDeviceConfig": { "@byNID": "", "@byTitle": "", "@subscriptionID": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValue_GetDeviceConfigRequest"
         }
     };


    //Add Device Request 
    RequestRules.AddDeviceRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "AddDeviceRequest": {
                        "Groups": "",
                        "DevicePrivateData": "",
                        "@title": "", "@type": "", "@password": "", "@description": "", "@unManaged": "", "@unManned": "", "@applyTagsToPorts": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_AddDeviceRequest"
        }
    };

    //Edit Device Request 
    RequestRules.SetDeviceInfoRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetDeviceInfoRequest": {
                        "Groups": "",
                        "DevicePrivateData": "",
                        "@byNID": "", "@password": "", "@description": "", "@applyTagsToPorts": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetDeviceInfoRequest"
        }
    };

    //Delete Device Request 
    RequestRules.DeleteDeviceRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "DeleteDeviceRequest": { "@byNID": "", "@byTitle": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_DeleteDeviceRequest"
        }
    };

    //Upgrade Device Request 
    RequestRules.UpgradeDeviceRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "UpgradeDevice": { "@byNID": "", "@byTitle": "",
                        "URL": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValue_UpgradeDeviceRequest"
        }
    };

    //RebootDeviceRequest
    RequestRules.RebootDeviceRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "RebootDevice": { "@byNID": "", "@byTitle": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValue_RebootDeviceRequest"
         }
     };

    //RevertToDefaultDeviceRequest
    RequestRules.RevertToDefaultDeviceRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "RevertToDefaultDevice": { "@byNID": "", "@byTitle": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValue_RevertToDefaultDeviceRequest"
         }
     };

    //SaveConfigDeviceRequest
    RequestRules.SaveConfigDeviceRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SaveConfigDevice": { "@byNID": "", "@byTitle": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValue_SaveConfigDeviceRequest"
         }
     };

    //ShutdownDeviceRequest
    RequestRules.ShutdownDeviceRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "ShutdownDevice": { "@byNID": "", "@byTitle": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValue_ShutdownDeviceRequest"
         }
     };


    RequestRules.HATakeoverDeviceRequest =
      {
      "Body": {
          "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
            "query": {
              "HATakeoverDevice": ""
            }
          }
        },
        "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValue_HATakeoverDeviceRequest"
      }
    };

    //Start DiscoveryService Request 
    RequestRules.StartDiscoveryServiceRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "Discovery", "requestname": "StartDiscoveryServiceRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "StartDiscoveryServiceRequestData": { "Restart": "" }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToStartDiscoveryServiceRequest",
            "setValueAndAttribute": null
        }
    };

    //Stop DiscoveryService Request 
    RequestRules.StopDiscoveryServiceRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "Discovery", "requestname": "StopDiscoveryServiceRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "StopDiscoveryServiceRequestData": ""
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToStopDiscoveryServiceRequest",
            "setValueAndAttribute": null
        }
    };

    //SetPortInfo Request 
    RequestRules.SetPortInfoRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetPortInfoRequest": {
                        "Groups": "",
                        "@byNID": "", "@byTitle": "", "@title": "", "@description": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetPortInfoRequest"
        }
    };

    //SetPortPVRInfo Request 
    RequestRules.SetPortPVRInfoRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetPortPVRInfoRequest": {
                        "@byNID": "", "@byTitle": "", "@enable": "", "@trimLength": "", "@mediaStreamDirResourceNID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetPortPVRInfoRequest"
        }
    };

    //SetPortStreamURL Request 
    RequestRules.SetPortStreamURLRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetPortStreamURLRequest": {
                        "@byNID": "", "@byTitle": "", "@streamURL": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetPortStreamURLRequest"
        }
    };

    //SetPortObserverInfo Request 
    RequestRules.SetPortObserverInfoRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetPortObserverInfoRequest": {
                        "Observer": {
                            "UserNID": "",
                            "ResourceNID": "",
                            "@xAuth": "", "@monitor": ""
                        },
                        "@byNID": "", "@byTitle": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetPortObserverInfoRequest"
        }
    };

    //SetPortAutoStart Request 
    RequestRules.SetPortAutoStartRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set",
                "query": {
                    "SetPortAutoStartRequest": { "@byNID": "", "@byTitle": "", "@enable": "",
                        "ProfileXML": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetPortAutoStartRequest"
        }
    };

    //SetDeviceConfig Request 
    RequestRules.SetDeviceConfigRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetDeviceConfig": { "@byNID": "", "@byTitle": "", "@doNotSaveToFlash": "true", "@path": "",
                        "DeviceConfig": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValue_SetDeviceConfigRequest"
        }
    };

    //DeviceCommandRequest
    RequestRules.DeviceCommandRequest =
     {
         "Body": {
             "AgentMessage": { "@serviceVer": "", "@Ver": "1.1", "@from": "", "@to": "", "@type": "", "@logLevel": "5",
                 "agentmessagedata": {
                     "Request": { "@serviceVer": "", "@Ver": "1.1",
                         "header": { "servicename": "DeviceAdmin", "requestname": "DeviceCommandRequest", "loglevel": "0", "clientdata": ""
                         },
                         "data": {
                             "DeviceCommandRequestData": ""
                         }
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": "RequestRules.setValue_DeviceCommandRequest",
             "setValueAndAttribute": null
         }
     };

    //SetDeviceNetworkInterface Request 
    RequestRules.SetDeviceNetworkInterfaceRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "sessionservice.resource", "requestname": "SetDeviceNetworkInterfaceRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "SetDeviceNetworkInterfaceRequestData": ""
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToSetDeviceNetworkInterfaceRequest",
            "setValueAndAttribute": null
        }
    };

    //SetDeviceXMPPAgent request 
    RequestRules.SetDeviceXMPPAgentRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "sessionservice.resource", "requestname": "SetDeviceXMPPAgentRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "SetDeviceXMPPAgentRequestData": ""
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToSetDeviceXMPPAgentRequest",
            "setValueAndAttribute": null
        }
    };


    //SetFileInfo(Title/Description)
    RequestRules.SetFileInfoRequest =
     {
         "Body": {
             "iq": {
                 "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SetFileInfo": { "@byNID": "", "@title": "", "@description": "" }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_SetFileInfoRequest"
         }
     };

    //SetFileInfo(Title/Description)
    RequestRules.SetFileGroupInfoRequest =
     {
         "Body": {
             "iq": {
                 "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SetFileInfo": {
                         "@byNID": "",
                         "Groups": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_SetFileGroupInfoRequest"
         }
     };

    //Delete File Request 
    RequestRules.DeleteFileRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "DeleteFile": { "@byNID": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_DeleteFileRequest"
        }
    };


    //ClearDownloadCache File Request 
    RequestRules.ClearDownloadCacheRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "ClearDownloadCache": { "@byNID": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_ClearDownloadCacheRequest"
        }
    };

    //Get Session Request 
    RequestRules.GetSessionRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetSession": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereId": "", "@whereTextSubStr": "", "@whereTagExpr": "", "@selectFormat": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetSessionRequest"
        }
    };

    //Stop Session Request
    RequestRules.StopRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "StopSession": { "@sessionId": "" }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_StopRequest"
        }
    };

    //SetDirStorageConfigRequest
    RequestRules.SetDirStorageConfigRequest =
     {
         "Body": {
             "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                 "query": {
                     "SetDirStorageConfigRequest": {
                         "DirSpanning": { "@diskFullThreshold": "",
                             "PortInfo": {
                                 "Port": { "@Port_byNID": "", "@priority": ""
                                 }
                             }
                         },
                         "CapacityManagement": { "@schedule": "",
                             "ThresholdRule": { "@ThresholdRule_enable": "", "@diskCapacityReached": "", "@reduceDiskCapacity": "", "@ThresholdRule_type": "", "@ThresholdRule_nasDriveURL": "", "@ThresholdRule_format": ""
                             },
                             "ILMRule": { "@ILMRule_enable": "", "@olderThan": "", "@ILMRule_type": "", "@ILMRule_nasDriveURL": "", "@ILMRule_format": ""
                             }
                         },
                         "@byNID": "", "@whereMode": ""
                     }
                 }
             }
         },
         "CallBack": {
             "setValue": null,
             "setValueAndAttribute": "RequestRules.setValueAttr_SetDirStorageConfigRequest"
         }
     };

    //AddExportFile Request 
    RequestRules.AddExportFileRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "assetadminservice", "requestname": "AddExportFileRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "AddExportFileRequestData": {
                        "AddExportFile": {
                            "File": { "@fileName": "", "@fileNID": "", "@title": "", "@dateCreated": "", "@startOffset": "", "@endOffset": "" },
                            "@JID": "", "@folderPath": "", "@priority": "", "@dateCreated": "", "@format": ""
                        }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToAddExportFileRequest",
            "setValueAndAttribute": null
        }
    };

    //AddImportFile Request 
    RequestRules.AddImportFileRequest =
    {
        "Body": {
            "Request": { "@serviceVer": "", "@Ver": "1.1",
                "header": { "servicename": "assetadminservice", "requestname": "AddImportFileRequest", "loglevel": "0", "clientdata": ""
                },
                "data": {
                    "AddImportFileRequestData": {
                        "AddImportFile": {
                            "File": { "@blobSize": "", "@fileName": "", "@blobName": "", "@groupID": "", "@title": "" },
                            "@folderPath": "", "@dirNID": "", "@dirTitle": "", "@JID": "", "@priority": "", "@dateCreated": "", "@blobFolder": "", "@isMedia2dot4": ""
                        }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValueToAddImportFileRequest",
            "setValueAndAttribute": null
        }
    };

    //GetReportRequest
    RequestRules.GetReportRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetReport": { "@startFrom": "", "@countToFetch": "", "@sortBy": "", "@sortOrder": "", "@selectFormat": "", "@whereNID": "", "@whereName": "", "@whereType": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetReportRequest"
        }
    };

    //AddReportRequest
    RequestRules.AddReportRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set",
                "query": {
                    "AddReport": { "@description": "", "@type": "", "@format": "", "@enableExport": "", "@exportSchedule": "", "@exportURL": "", "@name": "",
                        "Policy": { "@Policy_properties": "", "@permissions": "", "@whereTarget": "", "@whereTextSubStr": "", "@whereTagExpr": "", "@whereTagExprMode": "",
                            "FormatXSL": { "@Policy_mode": "", "#Policy_text": ""
                            }
                        },
                        "Audit": { "@Audit_properties": "",
                            "FormatXSL": { "@Audit_mode": "", "#Audit_text": ""
                            }
                        }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_AddReportRequest"
        }
    };


    //SetReportRequest
    RequestRules.SetReportRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetReport": { "@description": "", "@type": "", "@format": "", "@enableExport": "", "@exportSchedule": "", "@exportURL": "", "@byName": "", "@byNID": "",
                        "Policy": { "@Policy_properties": "", "@permissions": "", "@whereTarget": "", "@whereTextSubStr": "", "@whereTagExpr": "", "@whereTagExprMode": "",
                            "FormatXSL": { "@Policy_mode": "", "#Policy_text": ""
                            }
                        },
                        "Audit": { "@Audit_properties": "",
                            "FormatXSL": { "@Audit_mode": "", "#Audit_text": ""
                            }
                        }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetReportRequest"
        }
    };

    //DeleteReportRequest
    RequestRules.DeleteReportRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "DeleteReport": { "@byName": "", "@byNID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_DeleteReportRequest"
        }
    };

    //DownloadReportRequest
    RequestRules.DownloadReportRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "DownloadReport": { "@whereNID": "", "@whereName": "", "@toFileName": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_DownloadReportRequest"
        }
    };

    //GetSystemConfigRequest
    RequestRules.GetSystemConfigRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetSystemConfig": { "@whereConfig": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetSystemConfigRequest"
        }
    };


    //GetSessionConfigRequest
    RequestRules.GetSessionConfigRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetSessionConfig": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "", "@whereTagExpr": "", "@selectFormat": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetSessionConfig"
        }
    };

    //Set Auto Device Discovery Request 
    RequestRules.SetAutoDiscoveryRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetAutoDiscovery": { "@enable": "",
                        "Groups": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetAutoDiscoveryRequest"
        }
    };

    //Set Writer Options Request 
    RequestRules.SetWriterOptionsRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetWriterOptions": { "@enableHLS": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetWriterOptionsRequest"
        }
    };

    //Set Multicast IPAddress Pool Request 
    RequestRules.SetMulticastIPAddressPoolRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SetMulticastIPAddressPool": { "@enable": "", "@from": "", "@to": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SetMulticastIPAddressPoolRequest"
        }
    };


    //GetBookmarkRequest
    RequestRules.GetBookmarkRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
                "query": {
                    "GetBookmark": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "", "@whereDateRange": "", "@whereTCRange": "", "@whereFileNID": "", "@whereSessionInfo": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetBookmark"
        }
    };

    //StartRequest
    RequestRules.StartRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "StartSession": { "@sessionId": "", "#text": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_StartRequest",
            "setValueAndAttribute": null
        }
    };


    //AddBookmark
    RequestRules.AddBookmark =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_AddBookmark",
            "setValueAndAttribute": null
        }
    };

    //UpdateBookmark
    RequestRules.UpdateBookmark =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_UpdateBookmark",
            "setValueAndAttribute": null
        }
    };



    //ResumeFromBookmarkRequest
    RequestRules.ResumeFromBookmarkRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Play": { "@sessionId": "", "@timeScale": "", "@bookmarkId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_ResumeFromBookmarkRequest"
        }
    };

    //DeleteBookmark
    RequestRules.DeleteBookmark =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_DeleteBookmark",
            "setValueAndAttribute": null
        }
    };

    // DeleteConnRequest
    RequestRules.DeleteConnRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "DeleteConn": { "@sessionId": "", "@connId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_DeleteConnRequest"
        }
    };

    // PauseRequest
    RequestRules.PauseRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Pause": { "@sessionId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_PauseRequest"
        }
    };

    // PlayRequest
    RequestRules.PlayRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Play": { "@sessionId": "", "@timeScale": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_PlayRequest"
        }
    };

    // SkipBackRequest
    RequestRules.SkipBackRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SkipBack": { "@sessionId": "", "@skipOffset": "", "@timeScale": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SkipBackRequest"
        }
    };

    // SkipFwdRequest
    RequestRules.SkipFwdRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "SkipFwd": { "@sessionId": "", "@skipOffset": "", "@timeScale": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_SkipFwdRequest"
        }
    };

    // CatchupRequest
    RequestRules.CatchupRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Catchup": { "@sessionId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_CatchupRequest"
        }
    };

    // PauseRecordRequest
    RequestRules.PauseRecordRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "PauseRecord": { "@sessionId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_PauseRecordRequest"
        }
    };



    // ResumeRecordRequest
    RequestRules.ResumeRecordRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "ResumeRecord": { "@sessionId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_ResumeRecordRequest"
        }
    };

    // JoinRequest
    RequestRules.JoinRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Join": { "@sessionId": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_JoinRequest"
        }
    };

    // SetPortDisplayConfig
    RequestRules.SetPortDisplayConfig =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_SetPortDisplayConfig",
            "setValueAndAttribute": null
        }
    };

    // SetPortWindowProperties
    RequestRules.SetPortWindowProperties =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_SetPortWindowProperties",
            "setValueAndAttribute": null
        }
    };

    // SetServiceClientDisplayStatus
    RequestRules.SetServiceClientDisplayStatus =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_SetServiceClientDisplayStatus",
            "setValueAndAttribute": null
        }
    };

    // SetServiceClientDisplayConfig
    RequestRules.SetServiceClientDisplayConfig =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": ""
            }
        },
        "CallBack": {
            "setValue": "RequestRules.setValue_SetServiceClientDisplayConfig",
            "setValueAndAttribute": null
        }
    };

    // ViewRequest
    RequestRules.ViewRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "View": { "@sessionId": "", "@id": "", "@srcEntityId": "", "@sessionName": "", "@name": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": "",
                        "DstPort": { "@DstPort_id": "", "@DstPort_portNID": "", "@DstPort_portTitle": "" },
                        "SrcPort": { "@SrcPort_id": "", "@SrcPort_portNID": "", "@SrcPort_portTitle": "", "@SrcPort_pvrEnable": "", "@SrcPort_trimLength": "" }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_ViewRequest"
        }
    };


    // GetPortDisplayStatusRequest
    RequestRules.GetPortDisplayStatusRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetPortDisplayStatus": { "@whereNID": "", "@whereTextSubStr": "", "@subscriptionID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetPortDisplayStatusRequest"
        }
    };

    // GetPortTagsRequest
    RequestRules.GetPortTagsRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetPortTags": { "@countToFetch": "", "@sortBy": "", "@sortOrder": "", "@startFrom": "", "@whereNameTextSubString": "", "@whereValueTextSubString": "", "@selectFormat": "", "@wherePermissions": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetPortTagsRequest"
        }
    };


    // PlayBackSrcRequest
    RequestRules.PlayBackSrcRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Playback": { "@sessionId": "", "@sessionName": "", "@id": "", "@name": "", "@srcEntityId": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": "",
                        "DstPort": { "@DstPort_id": "", "@DstPort_portNID": "", "@DstPort_portTitle": "" },
                        "SrcFile": { "@SrcFile_id": "", "@SrcFile_fileNID": "" }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_PlayBackSrcRequest"
        }
    };

    // PlayBackGrpRequest
    RequestRules.PlayBackGrpRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "Playback": { "@sessionId": "", "@sessionName": "", "@id": "", "@name": "", "@srcEntityId": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": "",
                        "DstPort": { "@DstPort_id": "", "@DstPort_portNID": "", "@DstPort_portTitle": "" },
                        "GrpFile": { "@GrpFile_id": "", "@GrpFile_fileNID": "" }
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_PlayBackGrpRequest"
        }
    };

    // GetPortDisplayConfigRequest
    RequestRules.GetPortDisplayConfigRequest =
    {
        "Body": {
            "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
                "query": {
                    "GetPortDisplayConfig": { "@whereNID": "", "@whereTextSubStr": "", "@subscriptionID": ""
                    }
                }
            }
        },
        "CallBack": {
            "setValue": null,
            "setValueAndAttribute": "RequestRules.setValueAttr_GetPortDisplayConfigRequest"
        }
    };


    // !!!*** ABOVE IQ Request BLOCK ****!!!!!


    //set parameters value for login request
    RequestRules.setValueToLoginRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.LoginRequestData.user = RequestRules.escapeSpecialChar(objContext.User);
        objJSONRule.Body.Request.data.LoginRequestData.password = RequestRules.escapeSpecialChar(objContext.Password);
        objJSONRule.Body.Request.data.LoginRequestData.resource = RequestRules.escapeSpecialChar(objContext.Resource);
        objJSONRule.Body.Request.data.LoginRequestData.domain = objContext.Domain;
        objJSONRule.Body.Request.data.LoginRequestData.server = RequestRules.escapeSpecialChar(objContext.Server);
        objJSONRule.Body.Request.data.LoginRequestData.port = objContext.Port;
        return objJSONRule.Body
    };

    //set parameters value to logout request
    RequestRules.setValueToLogoutRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        return objJSONRule.Body
    };

    //set parameters value  setServiceDomain request
    RequestRules.setValueToSetServiceDomainRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.domain = objContext.ServiceDomain;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.AssetTemplateTitle = objContext.AssetTemplateTitle;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.AssetPrivateData = objContext.AssetPrivateData;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.AssetPrivateKeyData = objContext.AssetPrivateKeyData;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.ServiceVersion = objContext.ServiceVersion;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.SoftwareVersion = objContext.SoftwareVersion;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.IPAddress = objContext.IPAddress;
        objJSONRule.Body.Request.data.SetServiceDomainRequestData.AppName = objContext.AppName;
        return objJSONRule.Body
    };

    //setValue_UpgradeRequest
    RequestRules.setValue_UpgradeRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.UpgradeRequestData.Upgrade["@deviceType"] = objContext.DeviceType;
        objJSONRule.Body.Request.data.UpgradeRequestData.Upgrade.FromDirPath = objContext.LocalFileName;
        return objJSONRule.Body
    };

    //setValue_RestoreDBRequest
    RequestRules.setValue_RestoreDBRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.RestoreDBRequestData.RestoreFrom = objContext.RestoreFrom;
        return objJSONRule.Body
    };

    //setValue_ImportServiceDBRequest
    RequestRules.setValue_ImportServiceDBRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.ImportServiceDBRequestData.LocalFileName = objContext.LocalFileName;
        return objJSONRule.Body
    };

    //setValue_ExportServiceDBRequest
    RequestRules.setValue_ExportServiceDBRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.ExportServiceDBRequestData.LocalFileName = objContext.LocalFileName;
        return objJSONRule.Body
    }

    //setValue_BackupDatabaseRequest
    RequestRules.setValue_BackupDatabaseRequest = function (objContext, objJSONRule) {
      objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
      objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
      objJSONRule.Body.Request.data.BackupDatabaseRequestData.BackupDatabase.LocalFileName = objContext.LocalFileName;
        return objJSONRule.Body
    }

    //setValue_SaveDBRequest
    RequestRules.setValue_SaveDBRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        return objJSONRule.Body
    }

    //set parameters value to GetServiceClient request
    RequestRules.setValueToGetServiceClientRequest = function (objContext, objJSONRule) {

    };

    //set attributes and parameters value to GetServiceClient request
    RequestRules.setValueAndAttrToGetServiceClientRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value  GetServerTime request
    RequestRules.setValueToGetServerTimeRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request.data.GetServerTimeRequestData = "";
        return objJSONRule.Body
    };

    //set parameters value AuditEventRequest
    RequestRules.setValueAttr_AuditEventRequest = function (objContext) {
        var descriptionNode = objContext.RequestDataObjectArr["Description"];
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        if (jsx3.util.strEmpty(descriptionNode.selectSingleNode(".//MetaData"))) {
            objJSONStr = objJSONStr.replace('\"MetaData\":\"\",', '');
        }
        else {
            var metadataNode = descriptionNode.selectSingleNode(".//MetaData");
            var iterTag = metadataNode.selectNodeIterator(".//Tag");
            var nodeTAGList = "";
            while (iterTag.hasNext()) {
                var nodeTAG = iterTag.next();
                var strTAG = nodeTAG.toString();
                strTAG = strTAG.replace(/"/g, "'");
                nodeTAGList += strTAG;
            }
            var nodeTAGListJSON = nodeTAGList.toJSON();
            objJSONStr = objJSONStr.replace('\"MetaData\":\"\"', '\"MetaData\":\"' + nodeTAGListJSON + '\"');
        }

        var textStr = "";
        if (!jsx3.util.strEmpty(descriptionNode.getAttribute("text"))) {
            textStr = RequestRules.escapeSpecialChar(descriptionNode.getAttribute("text"));
        }

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                        .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                        .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                        .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_type + '\"')
        //                  .replace('\"@date\":\"\"', '\"@date\":\"' + objContext.date + '\"')
        //                  .replace('\"@time\":\"\"', '\"@time\":\"' + objContext.time + '\"')
                        .replace('\"@level\":\"\"', '\"@level\":\"' + objContext.ObjectList_level + '\"')
                        .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_name + '\"')
                        .replace('\"@text\":\"\"', '\"@text\":\"' + textStr + '\"')
                        .replace('\"@NID\":\"\"', '\"@NID\":\"' + descriptionNode.selectSingleNode(".//EventUser/User").getAttribute("NID") + '\"')
                        .replace('\"@userName\":\"\"', '\"@userName\":\"' + descriptionNode.selectSingleNode(".//EventUser/User").getAttribute("userName") + '\"')
                        .replace('\"@externalAuthentication\":\"\"', '\"@externalAuthentication\":\"' + descriptionNode.selectSingleNode(".//EventUser/User").getAttribute("externalAuthentication") + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_DeviceConfigurationRequest [call clsRequest.ReqSendConfiguarationAgentObject.sendRequest]
    RequestRules.setValue_DeviceConfigurationRequest = function (objContext, objJSONRule) {
        var deviceConfigNode = objContext.RequestDataObjectArr["DeviceConfigurationRequestData"].selectSingleNode(".");
        var deviceConfigStr = deviceConfigNode.toString();
        deviceConfigStr = deviceConfigStr.replace(/"/g, "'");
        var deviceConfigJSON = deviceConfigStr.toJSON();

        //caller DisplayAgent_prototype._sendSetDisplayConfigResponse
        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;

        objJSONRule.Body.AgentMessage.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage.Request.header.clientdata = objContext.TopicName;
        //move previous angentmessageheader/agent to Request/header/clientcallback
        objJSONRule.Body.AgentMessage.Request.header.clientcallback = objContext.AgentMessageHeader.fromagent;
        objJSONRule.Body.AgentMessage.Request.data.DeviceConfigurationRequestData = deviceConfigJSON;

        return objJSONRule.Body
    }

    // setValue_FileXferSetupRequest
    RequestRules.setValue_FileXferSetupRequest = function (objContext, objJSONRule) {
        var fileXferStr = "";
        var fileXferNode = objContext.RequestDataObjectArr["FileXfer"].selectSingleNode(".");
        var nodeIter = fileXferNode.selectNodeIterator("./*");
        while (nodeIter.hasNext()) {
            var nodeNode = nodeIter.next();
            var nodeStr = nodeNode.toString();
            nodeStr = nodeStr.replace(/"/g, "'");
            fileXferStr += nodeStr;
        }

        var fileXferJSON = fileXferStr.toJSON();

        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;

        objJSONRule.Body.AgentMessage.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage.Request.header.clientdata = objContext.TopicName;
        //ref. story #46274 move agentmessageheader/agent to Request/header/clientcallback
        objJSONRule.Body.AgentMessage.Request.header.clientcallback = objContext.AgentMessageHeader.agent;

        objJSONRule.Body.AgentMessage.Request.data.FileXferSetupRequestData.FileXferID = objContext.FileXferID;
        objJSONRule.Body.AgentMessage.Request.data.FileXferSetupRequestData.FileXfer = fileXferJSON;

        return objJSONRule.Body
    }

    // setValue_FileXferStopRequest
    RequestRules.setValue_FileXferStopRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;

        objJSONRule.Body.AgentMessage.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage.Request.header.clientdata = objContext.TopicName;
        //ref. story #46274 move agentmessageheader/agent to Request/header/clientcallback
        objJSONRule.Body.AgentMessage.Request.header.clientcallback = objContext.AgentMessageHeader.agent;
        objJSONRule.Body.AgentMessage.Request.data.FileXferStopRequestData.FileXferID = objContext.FileXferID;

        return objJSONRule.Body
    }

    // setValue_DeviceStatusRequest 
    RequestRules.setValue_GetDeviceStatusRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();

        //call from clsRequest.ReqGetAgentObject.sendRequest
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;

        objJSONRule.Body.AgentMessage.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage.Request.header.clientdata = objContext.TopicName;
        //move privoious 'agentmessageheader/agent' info (i.e. com.ipvs.common.service.DeviceAdminServerAgent) to Request/header/clientcallback
        objJSONRule.Body.AgentMessage.Request.header.clientcallback = objContext.AgentMessageHeader.fromagent;

        return objJSONRule.Body
    }

    // setValue_AgentEventRequest
    RequestRules.setValue_AgentEventRequest = function (objContext, objJSONRule) {
        var setStr = objContext.AgentMessageDataObjectArr["agentmessagedata"];
        setStr = setStr.replace('\"clientdata\":\"\"', '\"clientdata\":\"' + objContext.TopicName + '\"');
        var objJSONStr = JSON.stringify(objJSONRule);
        objJSONStr = objJSONStr.replace('\"agentmessagedata\":\"\"', setStr);
        objJSONRule = JSON.parse(objJSONStr);
        //call from clsRequest.ReqSendAgentEventMessageObject.sendRequest
        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;
        return objJSONRule.Body
    }

    // setValue_GetMediaRoomListRequest
    RequestRules.setValue_GetMediaRoomListRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        return objJSONRule.Body
    }

    // setValue_GetAssetManagementRequest
    RequestRules.setValue_GetAssetManagementRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXML = objContext.Op_Select_ItemSpec_SelectXML;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXQuery = objContext.Op_Select_ItemSpec_SelectXQuery;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Select.ListSpec.StartFrom = objContext.Op_Select_ListSpec_StartFrom;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Select.ListSpec.CountToFetch = objContext.Op_Select_ListSpec_CountToFetch;
        objJSONRule.Body.Request.data.AssetManagementRequestData.GetRequest.PathSpec.Select.ListSpec.SortByXPathList.SortByXpath = objContext.Op_Select_ListSpec_SortByXPathList_SortByXpath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ObjectType = objContext.ObjectType;
        return objJSONRule.Body
    }

    // setValue_ReplaceAssetManagementRequest
    RequestRules.setValue_ReplaceAssetManagementRequest = function (objContext, objJSONRule) {
        var opterationDataNode = objContext.RequestDataObjectArr["OperationData"].selectSingleNode(".");
        var operationDataStr = opterationDataNode.toString();
        operationDataStr = operationDataStr.replace(/"/g, "'");
        var operationDataJSON = operationDataStr.toJSON();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReplaceRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReplaceRequest.PathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReplaceRequest.PathSpec.Where.XQueryPrefix = objContext.Op_Where_XQueryPrefix;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReplaceRequest.PathSpec.In.ChildXPath = objContext.Op_In_ChildXPath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ObjectType = objContext.ObjectType;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReplaceRequest.OperationData = operationDataJSON;
        return objJSONRule.Body
    }

    // setValue_AddAssetManagementRequest
    RequestRules.setValue_AddAssetManagementRequest = function (objContext, objJSONRule) {
        var opterationDataNode = objContext.RequestDataObjectArr["OperationData"].selectSingleNode(".");
        var operationDataStr = opterationDataNode.toString();
        operationDataStr = operationDataStr.replace(/"/g, "'");
        var operationDataJSON = operationDataStr.toJSON();

        var assistXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Where_WhereAssistXML)) {
            var assistXML = objContext.Op_Where_WhereAssistXML;
            var assistStr = assistXML.toString();
            assistStr = assistStr.replace(/"/g, "'");
            assistXMLJSON = assistStr.toJSON();
        }

        var selectXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Select_ItemSpec_SelectXML)) {
            var selectXML = objContext.Op_Select_ItemSpec_SelectXML;
            var selectXMLStr = selectXML.toString();
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AssetManagementRequestData.AddRequest.ParentNID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.AssetManagementRequestData.AddRequest.ParentPathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.AddRequest.ParentPathSpec.Where.WhereAssistXML = assistXMLJSON;
        objJSONRule.Body.Request.data.AssetManagementRequestData.AddRequest.ParentPathSpec.Select.ItemSpec.SelectXML = selectXMLJSON;
        objJSONRule.Body.Request.data.AssetManagementRequestData.AddRequest.OperationData = operationDataJSON;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ObjectType = objContext.ObjectType;

        return objJSONRule.Body
    }

    // setValue_DeleteAssetManagementRequest
    RequestRules.setValue_DeleteAssetManagementRequest = function (objContext, objJSONRule) {
        var assistXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Where_WhereAssistXML)) {
            var assistXML = objContext.Op_Where_WhereAssistXML;
            var assistStr = assistXML.toString();
            assistStr = assistStr.replace(/"/g, "'");
            assistXMLJSON = assistStr.toJSON();
        }

        var selectXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Select_ItemSpec_SelectXML)) {
            var selectXML = objContext.Op_Select_ItemSpec_SelectXML;
            var selectXMLStr = selectXML.toString();
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AssetManagementRequestData.DeleteRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.AssetManagementRequestData.DeleteRequest.PathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.AssetManagementRequestData.DeleteRequest.PathSpec.Where.WhereAssistXML = assistXMLJSON;
        objJSONRule.Body.Request.data.AssetManagementRequestData.DeleteRequest.PathSpec.Select.ItemSpec.SelectXML = selectXMLJSON;
        objJSONRule.Body.Request.data.AssetManagementRequestData.ObjectType = objContext.ObjectType;

        return objJSONRule.Body
    }

    // setValue_QueueStatusExportImportFileRequest
    RequestRules.setValue_QueueStatusExportImportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        return objJSONRule.Body
    }

    // setValue_ResumeExportImportFileRequest
    RequestRules.setValue_ResumeExportImportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        return objJSONRule.Body
    }

    // setValue_ClearQueueExportImportFileRequest
    RequestRules.setValue_ClearQueueExportImportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        return objJSONRule.Body
    }

    // setValue_SuspendExportImportFileRequest
    RequestRules.setValue_SuspendExportImportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        return objJSONRule.Body
    }

    // setValueAttr_DeleteExportImportFileRequest
    RequestRules.setValueAttr_DeleteExportImportFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@serviceVer\":\"\"', '\"@serviceVer\":\"' + getServiceVersion() + '\"')
                          .replace('\"clientdata\":\"\"', '\"clientdata\":\"' + objContext.TopicName + '\"')
                          .replace('\"@whereDateCreated\":\"\"', '\"@whereDateCreated\":\"' + objContext.ObjectList_WhereDateCreated + '\"')
                          .replace('\"@whereState\":\"\"', '\"@whereState\":\"' + objContext.ObjectList_WhereState + '\"')
                          .replace('\"@whereOwnerJID\":\"\"', '\"@whereOwnerJID\":\"' + objContext.ObjectList_WhereOwnerJID + '\"')
                          .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"')
                          .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@whereStateDescription\":\"\"', '\"@whereStateDescription\":\"' + objContext.ObjectList_WhereStateDescription + '\"');
        return (JSON.parse(objJSONStr)).Body;
    }

    // setValueAttr_UpdateExportImportFileRequest
    RequestRules.setValueAttr_UpdateExportImportFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@serviceVer\":\"\"', '\"@serviceVer\":\"' + getServiceVersion() + '\"')
                          .replace('\"clientdata\":\"\"', '\"clientdata\":\"' + objContext.TopicName + '\"')
                          .replace('\"@whereDateCreated\":\"\"', '\"@whereDateCreated\":\"' + objContext.ObjectList_WhereDateCreated + '\"')
                          .replace('\"@whereState\":\"\"', '\"@whereState\":\"' + objContext.ObjectList_WhereState + '\"')
                          .replace('\"@whereOwnerJID\":\"\"', '\"@whereOwnerJID\":\"' + objContext.ObjectList_WhereOwnerJID + '\"')
                          .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"')
                          .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@wherePriority\":\"\"', '\"@wherePriority\":\"' + objContext.ObjectList_WherePriority + '\"')
                          .replace('\"@whereStateDescription\":\"\"', '\"@whereStateDescription\":\"' + objContext.ObjectList_WhereStateDescription + '\"')
                          .replace('\"EIFLIPriority\":\"\"', '\"EIFLIPriority\":\"' + objContext.Priority + '\"')
                          .replace('\"EIFLIExternalFileName\":\"\"', '\"EIFLIExternalFileName\":\"' + objContext.ExternalFileName + '\"')
                          .replace('\"EIFLIExternalJID\":\"\"', '\"EIFLIExternalJID\":\"' + objContext.ExternalJID + '\"')
                          .replace('\"EIFLIExternalFolderPath\":\"\"', '\"EIFLIExternalFolderPath\":\"' + objContext.ExternalFolderPath + '\"');
        return (JSON.parse(objJSONStr)).Body;
    }

    // setValue_DetachDirRequest
    RequestRules.setValue_DetachDirRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.DetachDirRequestData.MediaStreamDirNID = objContext.MediaStreamDirNID;
        return objJSONRule.Body
    }

    // setValue_AttachDirRequest
    RequestRules.setValue_AttachDirRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AttachDirRequestData.MediaStreamDirNID = objContext.MediaStreamDirNID;
        return objJSONRule.Body
    }

    // setValue_EmptyDirRequest
    RequestRules.setValue_EmptyDirRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.EmptyDirRequestData.MediaStreamDirNID = objContext.MediaStreamDirNID;
        objJSONRule.Body.Request.data.EmptyDirRequestData.ActionRequest = objContext.ActionRequest;
        return objJSONRule.Body
    }

    // setValue_CleanOrphanDirRequest
    RequestRules.setValue_CleanOrphanDirRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.CleanOrphanDirRequestData.MediaStreamDirNID = objContext.MediaStreamDirNID;
        return objJSONRule.Body
    }

    // setValue_RepairMediaStreamFileRequest
    RequestRules.setValue_RepairMediaStreamFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.RepairMediaStreamFileRequestData.MediaStreamFileResourceNID = objContext.MediaStreamFileResourceNID;
        return objJSONRule.Body
    }

    //set parameters value for UpgradeDir request 
    RequestRules.setValueAttr_UpgradeDirRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@upgradeFiles\":\"\"', '\"@upgradeFiles\":\"' + objContext.UpgradeFiles + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for UpgradeFile request 
    RequestRules.setValueAttr_UpgradeFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_UpdateMediaStreamFileResourceInfoRequest
    RequestRules.setValue_UpdateMediaStreamFileResourceInfoRequest = function (objContext, objJSONRule) {
        var dataNode = objContext.RequestDataObjectArr["UpdateMediaStreamFileResourceInfoRequestData"].selectSingleNode(".");
        var dataNodeStr = dataNode.toString();
        dataNodeStr = dataNodeStr.replace(/"/g, "'");
        var dataNodeJSON = dataNodeStr.toJSON();

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data = dataNodeJSON;

        return objJSONRule.Body
    }

    // setValue_UpdateMediaStreamFileResourceStoreInfoRequest
    RequestRules.setValue_UpdateMediaStreamFileResourceStoreInfoRequest = function (objContext, objJSONRule) {
        var dataNode = objContext.RequestDataObjectArr["UpdateMediaStreamFileResourceStoreInfoRequestData"].selectSingleNode(".");
        var dataNodeStr = dataNode.toString();
        dataNodeStr = dataNodeStr.replace(/"/g, "'");
        var dataNodeJSON = dataNodeStr.toJSON();

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data = dataNodeJSON;

        return objJSONRule.Body
    }

    //set parameters value setValueAttr_SetFileMetaDataRequest
    RequestRules.setValueAttr_SetFileMetaDataRequest = function (objContext) {
        var metadataNode = objContext.RequestDataObjectArr["MetaData"].selectSingleNode(".");
        var iterTag = metadataNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"MetaData\":\"\"', '\"MetaData\":\"' + nodeTAGListJSON + '\"');
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                            .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                            .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                            .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_type + '\"')
                            .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value setValueAttr_SetDirStorageConfigRequest
    RequestRules.setValueAttr_SetDirStorageConfigRequest = function (objContext) {
        var dirSpanningNode = objContext.RequestDataObjectArr["DirSpanning"].selectSingleNode(".");
        var portNode = dirSpanningNode.selectSingleNode(".//Port");
        var capacityManagementNode = objContext.RequestDataObjectArr["CapacityManagement"].selectSingleNode(".");
        var thresholdRuleNode = capacityManagementNode.selectSingleNode("./ThresholdRule");
        var ilmRuleNode = capacityManagementNode.selectSingleNode("./ILMRule");
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                            .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                            .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                            .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_type + '\"')
                            .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                            .replace('\"@whereMode\":\"\"', '\"@whereMode\":\"' + objContext.ObjectList_WhereType + '\"')
                            .replace('\"@diskFullThreshold\":\"\"', '\"@diskFullThreshold\":\"' + dirSpanningNode.getAttribute("diskFullThreshold") + '\"')
                            .replace('\"@Port_byNID\":\"\"', '\"@byNID\":\"' + portNode.getAttribute("byNID") + '\"')
                            .replace('\"@priority\":\"\"', '\"@priority\":\"' + portNode.getAttribute("priority") + '\"')
                            .replace('\"@schedule\":\"\"', '\"@schedule\":\"' + capacityManagementNode.getAttribute("schedule") + '\"')
                            .replace('\"@ThresholdRule_enable\":\"\"', '\"@enable\":\"' + thresholdRuleNode.getAttribute("enable") + '\"')
                            .replace('\"@diskCapacityReached\":\"\"', '\"@diskCapacityReached\":\"' + thresholdRuleNode.getAttribute("diskCapacityReached") + '\"')
                            .replace('\"@reduceDiskCapacity\":\"\"', '\"@reduceDiskCapacity\":\"' + thresholdRuleNode.getAttribute("reduceDiskCapacity") + '\"')
                            .replace('\"@ThresholdRule_type\":\"\"', '\"@type\":\"' + thresholdRuleNode.getAttribute("type") + '\"')
                            .replace('\"@ThresholdRule_format\":\"\"', '\"@format\":\"' + thresholdRuleNode.getAttribute("format") + '\"')
                            .replace('\"@ThresholdRule_nasDriveURL\":\"\"', '\"@nasDriveURL\":\"' + thresholdRuleNode.getAttribute("nasDriveURL") + '\"')
                            .replace('\"@ILMRule_enable\":\"\"', '\"@enable\":\"' + ilmRuleNode.getAttribute("enable") + '\"')
                            .replace('\"@olderThan\":\"\"', '\"@olderThan\":\"' + ilmRuleNode.getAttribute("olderThan") + '\"')
                            .replace('\"@ILMRule_nasDriveURL\":\"\"', '\"@nasDriveURL\":\"' + ilmRuleNode.getAttribute("nasDriveURL") + '\"')
                            .replace('\"@ILMRule_type\":\"\"', '\"@type\":\"' + ilmRuleNode.getAttribute("type") + '\"')
                            .replace('\"@ILMRule_format\":\"\"', '\"@format\":\"' + ilmRuleNode.getAttribute("format") + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for AddExportFile request 
    RequestRules.setValueToAddExportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile["@JID"] = "";
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile["@folderPath"] = getJsxByName("txtFolderPath").getValue();
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile["@priority"] = objContext.ExportPriority;
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile["@dateCreated"] = "";
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile["@format"] = objContext.MediaFormat;

        var arrClips = [];
        var mtxExportFiles = PNameSpace.getJSXByName("mtxExportList");
        if (!jsx3.util.strEmpty(mtxExportFiles)) {
            var files = mtxExportFiles.getRecordIds();
            for (var i = 0; i < files.length; i++) {
                var clip = mtxExportFiles.getRecord(files[i]);
                var File = new Object();
                File["@fileName"] = clip.jsxFileName;
                File["@fileNID"] = clip.jsxid;
                File["@title"] = clip.jsxtextmediaName;
                File["@dateCreated"] = "";
                File["@startOffset"] = clip.jsxstartOffset;
                File["@endOffset"] = clip.jsxendOffset;
                arrClips.push(File);
            }
        }
        else {
            var mtxPVRExportFiles = PNameSpace.getJSXByName("cmpMediamtxPVRValue");
            if (!jsx3.util.strEmpty(mtxPVRExportFiles)) {
                var selRecordNodes = mtxPVRExportFiles.getSelectedNodes();
                for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                    var selRecordNode = it.next();
                    var fileId = selRecordNode.getAttribute("jsxid");
                    var fileName = fileId + ".mkd";
                    var File = new Object();
                    File["@fileName"] = fileName;
                    File["@fileNID"] = fileId;
                    File["@title"] = selRecordNode.getAttribute("jsxtextmediaPVRName");
                    File["@dateCreated"] = "";
                    File["@startOffset"] = getJsxByName("txtHiddenStartOffset").getValue(); ;
                    File["@endOffset"] = getJsxByName("txtHiddenEndOffset").getValue(); ;
                    arrClips.push(File);
                }
            }
        }
        objJSONRule.Body.Request.data.AddExportFileRequestData.AddExportFile.File = arrClips;

        return objJSONRule.Body
    };

    //set parameters value for AddImportFile request 
    RequestRules.setValueToAddImportFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@folderPath"] = getJsxByName("txtFolderPath").getValue();
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@dirNID"] = getJsxByName("txtSelDirNID").getValue();
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@dirTitle"] = "";
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@JID"] = "";
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@priority"] = "2";
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@dateCreated"] = "";
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@blobFolder"] = getJsxByName("txtBlobFolder").getValue();
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile["@isMedia2dot4"] = getJsxByName("txtIsMedia2dot4").getValue();
        var arrClips = [];
        var mtxImportFiles = PNameSpace.getJSXByName("mtxImportList");
        if (!jsx3.util.strEmpty(mtxImportFiles)) {
            var files = mtxImportFiles.getRecordIds();
            for (var i = 0; i < files.length; i++) {
                var clip = mtxImportFiles.getRecord(files[i]);
                if (clip.jsxChkColumn == 1) {
                    var File = new Object();
                    File["@blobSize"] = clip.jsxBlobSize;
                    File["@fileName"] = clip.jsxMediaName;
                    File["@blobName"] = clip.jsxBlobName;
                    File["@title"] = clip.jsxtextmediaName;
                    File["@groupID"] = (clip.jsxGrpNID == "NONE") ? "" : clip.jsxGrpNID;
                    arrClips.push(File);
                }
            }
        }
        objJSONRule.Body.Request.data.AddImportFileRequestData.AddImportFile.File = arrClips;

        return objJSONRule.Body
    };

    //set parameters value setValueAttr_SetFileBypassAutoStorageRulesRequest
    RequestRules.setValueAttr_SetFileBypassAutoStorageRulesRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                            .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                            .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                            .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_type + '\"')
                            .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                            .replace('\"@bypassAutoStorageRules\":\"\"', '\"@bypassAutoStorageRules\":\"' + objContext.ObjectList_BypassAutoStorageRules + '\"');


        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_GetPolicyManagementRequest
    RequestRules.setValue_GetPolicyManagementRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXML = objContext.Op_Select_ItemSpec_SelectXML;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXQuery = objContext.Op_Select_ItemSpec_SelectXQuery;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Select.ListSpec.StartFrom = objContext.Op_Select_ListSpec_StartFrom;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Select.ListSpec.CountToFetch = objContext.Op_Select_ListSpec_CountToFetch;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.GetRequest.PathSpec.Select.ListSpec.SortByXPathList.SortByXpath = objContext.Op_Select_ListSpec_SortByXPathList_SortByXpath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ObjectType = objContext.ObjectType;

        return objJSONRule.Body
    }

    // setValue_ReplacePolicyManagementRequest
    RequestRules.setValue_ReplacePolicyManagementRequest = function (objContext, objJSONRule) {
        var opterationDataNode = objContext.RequestDataObjectArr["OperationData"].selectSingleNode(".");
        var operationDataStr = opterationDataNode.toString();
        operationDataStr = operationDataStr.replace(/"/g, "'");
        var operationDataJSON = operationDataStr.toJSON();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ReplaceRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ReplaceRequest.PathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ReplaceRequest.PathSpec.Where.XQueryPrefix = objContext.Op_Where_XQueryPrefix;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ReplaceRequest.PathSpec.In.ChildXPath = objContext.Op_In_ChildXPath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ObjectType = objContext.ObjectType;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ReplaceRequest.OperationData = operationDataJSON;

        return objJSONRule.Body
    }

    // setValue_AddPolicyManagementRequest
    RequestRules.setValue_AddPolicyManagementRequest = function (objContext, objJSONRule) {
        var opterationDataNode = objContext.RequestDataObjectArr["OperationData"].selectSingleNode(".");
        var operationDataStr = opterationDataNode.toString();
        operationDataStr = operationDataStr.replace(/"/g, "'");
        var operationDataJSON = operationDataStr.toJSON();

        var assistXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Where_WhereAssistXML)) {
            var assistXML = objContext.Op_Where_WhereAssistXML;
            var assistStr = assistXML.toString();
            assistStr = assistStr.replace(/"/g, "'");
            assistXMLJSON = assistStr.toJSON();
        }

        var selectXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Select_ItemSpec_SelectXML)) {
            var selectXML = objContext.Op_Select_ItemSpec_SelectXML;
            var selectXMLStr = selectXML.toString();
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.PolicyManagementRequestData.AddRequest.ParentNID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.AddRequest.ParentPathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.AddRequest.ParentPathSpec.Where.WhereAssistXML = assistXMLJSON;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.AddRequest.ParentPathSpec.Select.ItemSpec.SelectXML = selectXMLJSON;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.AddRequest.OperationData = operationDataJSON;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ObjectType = objContext.ObjectType;

        return objJSONRule.Body
    }

    // setValue_DeletePolicyManagementRequest
    RequestRules.setValue_DeletePolicyManagementRequest = function (objContext, objJSONRule) {
        var assistXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Where_WhereAssistXML)) {
            var assistXML = objContext.Op_Where_WhereAssistXML;
            var assistStr = assistXML.toString();
            assistStr = assistStr.replace(/"/g, "'");
            assistXMLJSON = assistStr.toJSON();
        }

        var selectXMLJSON = "";
        if (!jsx3.util.strEmpty(objContext.Op_Select_ItemSpec_SelectXML)) {
            var selectXML = objContext.Op_Select_ItemSpec_SelectXML;
            var selectXMLStr = selectXML.toString();
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }

        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.PolicyManagementRequestData.DeleteRequest.NID = objContext.Op_Where_NID;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.DeleteRequest.ParentPathSpec.Where.WhereXPath = objContext.Op_Where_WhereXPath;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.DeleteRequest.ParentPathSpec.Where.WhereAssistXML = assistXMLJSON;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.DeleteRequest.ParentPathSpec.Select.ItemSpec.SelectXML = selectXMLJSON;
        objJSONRule.Body.Request.data.PolicyManagementRequestData.ObjectType = objContext.ObjectType;

        return objJSONRule.Body
    }

    // setValue_RestoreMediaStreamFileRequest
    RequestRules.setValue_RestoreMediaStreamFileRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.RestoreMediaStreamFileRequestData.MediaStreamFileResourceNID = objContext.MediaStreamFileResourceNID;
        return objJSONRule.Body
    }

    // setValue_ReadQueryAssetManagementRequest
    RequestRules.setValue_ReadQueryAssetManagementRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.AssetManagementRequestData.ReadQueryRequest.Query = objContext.Op_ReadQueryRequest_Query;
        return objJSONRule.Body
    }

    //set parameters value for GetUser request
    RequestRules.setValueAttr_GetuserRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereUsername\":\"\"', '\"@whereUsername\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_WhereUsername) + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"');
        return (JSON.parse(objJSONStr)).Body;

    };

    //set parameters value for GetTag request
    RequestRules.setValueAttr_GetTagsRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@whereNameTextSubString\":\"\"', '\"@whereNameTextSubString\":\"' + objContext.ObjectList_WhereNameTextSubStr + '\"')
                              .replace('\"@whereValueTextSubString\":\"\"', '\"@whereValueTextSubString\":\"' + objContext.ObjectList_WhereValueTextSubStr + '\"')
                              .replace('\"@whereObjectType\":\"\"', '\"@whereObjectType\":\"' + objContext.ObjectList_WhereObjectType + '\"');
        return (JSON.parse(objJSONStr)).Body;

    };

    //set parameters value for GetPort request
    RequestRules.setValueAttr_GetPortRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_Type + '\"')
                              .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"');


        return (JSON.parse(objJSONStr)).Body;

    };

    //set parameters value for GetDir request
    RequestRules.setValueAttr_GetDirRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"')
                              .replace('\"@whereFilePermissions\":\"\"', '\"@whereFilePermissions\":\"' + objContext.ObjectList_WhereFilePermissions + '\"')
                              .replace('\"@whereParentDirNID\":\"\"', '\"@whereParentDirNID\":\"' + objContext.ObjectList_WhereParentDirNID + '\"');


        return (JSON.parse(objJSONStr)).Body;

    };

    //set parameters value for GetFile request 
    RequestRules.setValueAttr_GetFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                          .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                          .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                          .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                          .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                          .replace('\"@searchTextSubStrIn\":\"\"', '\"@searchTextSubStrIn\":\"' + objContext.ObjectList_SearchTextSubStrIn + '\"')
                          .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                          .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                          .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                          .replace('\"@whereParentDirNID\":\"\"', '\"@whereParentDirNID\":\"' + objContext.ObjectList_WhereParentNID + '\"')
                          .replace('\"@whereSessionConfigNID\":\"\"', '\"@whereSessionConfigNID\":\"' + objContext.ObjectList_WhereSessionConfigNID + '\"')
                          .replace('\"@sortByMetaData\":\"\"', '\"@sortByMetaData\":\"' + objContext.ObjectList_SortByMetaData + '\"')
                          .replace('\"@sortByTagName\":\"\"', '\"@sortByTagName\":\"' + objContext.ObjectList_SortByTagName + '\"')
                          .replace('\"@whereState\":\"\"', '\"@whereState\":\"' + objContext.ObjectList_WhereState + '\"')
                          .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"')
                          .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                          .replace('\"@whereMetaDataExpr\":\"\"', '\"@whereMetaDataExpr\":\"' + objContext.ObjectList_WhereMetaDataExpr + '\"')
                          .replace('\"@whereDownloadState\":\"\"', '\"@whereDownloadState\":\"' + objContext.ObjectList_WhereDownloadState + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for GetPVRFile request 
    RequestRules.setValueAttr_GetPVRFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                          .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                          .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                          .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                          .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                          .replace('\"@searchTextSubStrIn\":\"\"', '\"@searchTextSubStrIn\":\"' + objContext.ObjectList_SearchTextSubStrIn + '\"')
                          .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                          .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                          .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                          .replace('\"@whereParentDirNID\":\"\"', '\"@whereParentDirNID\":\"' + objContext.ObjectList_WhereParentNID + '\"')
                          .replace('\"@whereSessionConfigNID\":\"\"', '\"@whereSessionConfigNID\":\"' + objContext.ObjectList_WhereSessionConfigNID + '\"')
                          .replace('\"@sortByMetaData\":\"\"', '\"@sortByMetaData\":\"' + objContext.ObjectList_SortByMetaData + '\"')
                          .replace('\"@sortByTagName\":\"\"', '\"@sortByTagName\":\"' + objContext.ObjectList_SortByTagName + '\"')
                          .replace('\"@whereState\":\"\"', '\"@whereState\":\"' + objContext.ObjectList_WhereState + '\"')
                          .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                          .replace('\"@whereMetaDataExpr\":\"\"', '\"@whereMetaDataExpr\":\"' + objContext.ObjectList_WhereMetaDataExpr + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for GetPVRFile request 
    RequestRules.setValueAttr_GetDeviceRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                          .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                          .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                          .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                          .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                          .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                          .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                          .replace('\"@whereTitle\":\"\"', '\"@whereTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_WhereTitle) + '\"')
                          .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"')
                          .replace('\"@searchTextSubStrIn\":\"\"', '\"@searchTextSubStrIn\":\"' + objContext.ObjectList_SearchTextSubStrIn + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_GetDeviceConfigRequest
    RequestRules.setValue_GetDeviceConfigRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"')
                          .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    }

    //set parameters value for AddDevice request 
    RequestRules.setValueAttr_AddDeviceRequest = function (objContext) {
        var groupsNode = objContext.RequestDataObjectArr["Groups"].selectSingleNode(".");
        var iterTag = groupsNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"Groups\":\"\"', '\"Groups\":\"' + nodeTAGListJSON + '\"');

        var selectXMLJSON = "";
        var selectXML = objContext.ObjectList_DevicePrivateData;
        if (!jsx3.util.strEmpty(selectXML)) {
            //selectXML = selectXML.replace(/</g, "\&lt;").replace(/>/g, "\&gt;");
            var selectXMLStr = RequestRules.escapeSpecialChar(selectXML.toString());
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }
        var descriptionJSON = "";
        var description = objContext.ObjectList_description;
        if (!jsx3.util.strEmpty(description)) {
            //description = description.replace(/</g, "\&lt;").replace(/>/g, "\&gt;");
            var descriptionStr = RequestRules.escapeSpecialChar(description.toString());
            descriptionStr = descriptionStr.replace(/"/g, "'");
            descriptionStr = descriptionStr.replace(/(\r\n|\n|\r)/gm, " ");
            descriptionJSON = descriptionStr.toJSON();
        }
        objJSONStr = objJSONStr.replace('\"DevicePrivateData\":\"\"', '\"DevicePrivateData\":\"' + selectXMLJSON + '\"');
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@title\":\"\"', '\"@title\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Title) + '\"')
                          .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_Type + '\"')
                          .replace('\"@password\":\"\"', '\"@password\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Password) + '\"')
                          .replace('\"@description\":\"\"', '\"@description\":\"' + descriptionJSON + '\"')
                          .replace('\"@unManaged\":\"\"', '\"@unManaged\":\"' + objContext.ObjectList_UnManaged + '\"')
                          .replace('\"@unManned\":\"\"', '\"@unManned\":\"' + objContext.ObjectList_UnManned + '\"')
                          .replace('\"@applyTagsToPorts\":\"\"', '\"@applyTagsToPorts\":\"' + objContext.ObjectList_ApplyTagsToPorts + '\"');


        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for EditDevice request 
    RequestRules.setValueAttr_SetDeviceInfoRequest = function (objContext) {
        var groupsNode = objContext.RequestDataObjectArr["Groups"].selectSingleNode(".");
        var iterTag = groupsNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"Groups\":\"\"', '\"Groups\":\"' + nodeTAGListJSON + '\"');

        var selectXMLJSON = "";
        var selectXML = objContext.ObjectList_DevicePrivateData;
        if (!jsx3.util.strEmpty(selectXML)) {
            //selectXML = selectXML.replace(/</g, "\&lt;").replace(/>/g, "\&gt;");
            var selectXMLStr = RequestRules.escapeSpecialChar(selectXML.toString());
            selectXMLStr = selectXMLStr.replace(/"/g, "'");
            selectXMLJSON = selectXMLStr.toJSON();
        }
        var descriptionJSON = "";
        var description = objContext.ObjectList_description;
        if (!jsx3.util.strEmpty(description)) {
            //description = description.replace(/</g, "\&lt;").replace(/>/g, "\&gt;");
            var descriptionStr = RequestRules.escapeSpecialChar(description.toString());
            descriptionStr = descriptionStr.replace(/"/g, "'");
            descriptionStr = descriptionStr.replace(/(\r\n|\n|\r)/gm, " ");
            descriptionJSON = descriptionStr.toJSON();
        }
        objJSONStr = objJSONStr.replace('\"DevicePrivateData\":\"\"', '\"DevicePrivateData\":\"' + selectXMLJSON + '\"');
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@password\":\"\"', '\"@password\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Password) + '\"')
                          .replace('\"@description\":\"\"', '\"@description\":\"' + descriptionJSON + '\"')
                          .replace('\"@applyTagsToPorts\":\"\"', '\"@applyTagsToPorts\":\"' + objContext.ObjectList_ApplyTagsToPorts + '\"');

        return (JSON.parse(RequestRules.escapeJson(objJSONStr))).Body;
    };

    //set parameters value for DeleteDevice request 
    RequestRules.setValueAttr_DeleteDeviceRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Title) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for UpgradeDevice request 
    RequestRules.setValue_UpgradeDeviceRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for RebootDevice request 
    RequestRules.setValue_RebootDeviceRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for RevertToDefaultDevice request 
    RequestRules.setValue_RevertToDefaultDeviceRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SaveConfigDevice request 
    RequestRules.setValue_SaveConfigDeviceRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for ShutdownDevice request 
    RequestRules.setValue_ShutdownDeviceRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for HATakeoverDevice request 
    RequestRules.setValue_HATakeoverDeviceRequest = function (objContext, objJSONRule) {
      var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
      objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                             .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                             .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"');

      return (JSON.parse(objJSONStr)).Body;    
    };

    //set parameters value for StartDiscoveryService request 
    RequestRules.setValueToStartDiscoveryServiceRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        objJSONRule.Body.Request.data.StartDiscoveryServiceRequestData.Restart = "true";

        return objJSONRule.Body
    };

    //set parameters value for StopDiscoveryService request 
    RequestRules.setValueToStopDiscoveryServiceRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;

        return objJSONRule.Body
    };

    //set parameters value for SetPortInfo request 
    RequestRules.setValueAttr_SetPortInfoRequest = function (objContext) {
        var groupsNode = objContext.RequestDataObjectArr["Groups"].selectSingleNode(".");
        var iterTag = groupsNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"Groups\":\"\"', '\"Groups\":\"' + nodeTAGListJSON + '\"');

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_ByTitle) + '\"')
                          .replace('\"@title\":\"\"', '\"@title\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Title) + '\"')
                          .replace('\"@description\":\"\"', '\"@description\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_description) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetPortPVRInfo request 
    RequestRules.setValueAttr_SetPortPVRInfoRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + objContext.ObjectList_ByTitle + '\"')
                          .replace('\"@enable\":\"\"', '\"@enable\":\"' + objContext.ObjectList_Enable + '\"')
                          .replace('\"@trimLength\":\"\"', '\"@trimLength\":\"' + objContext.ObjectList_TrimLength + '\"')
                          .replace('\"@mediaStreamDirResourceNID\":\"\"', '\"@mediaStreamDirResourceNID\":\"' + objContext.ObjectList_MediaStreamDirResourceNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetPortStreamURL request 
    RequestRules.setValueAttr_SetPortStreamURLRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + objContext.ObjectList_ByTitle + '\"')
                          .replace('\"@streamURL\":\"\"', '\"@streamURL\":\"' + objContext.ObjectList_StreamURL.replace(/"/g, '&quot;') + '\"');

        return (JSON.parse(RequestRules.escapeJson(objJSONStr))).Body;
    };

    //set parameters value for SetPortObserverInfo request 
    RequestRules.setValueAttr_SetPortObserverInfoRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + objContext.ObjectList_ByTitle + '\"')
                          .replace('\"@xAuth\":\"\"', '\"@xAuth\":\"' + objContext.ObjectList_XAuth + '\"')
                          .replace('\"@monitor\":\"\"', '\"@monitor\":\"' + objContext.ObjectList_Monitor + '\"')
                          .replace('\"UserNID\":\"\"', '\"UserNID\":\"' + objContext.ObjectList_UserNID + '\"')
                          .replace('\"ResourceNID\":\"\"', '\"ResourceNID\":\"' + objContext.ObjectList_ResourceNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetPortObserverInfo request 
    RequestRules.setValueAttr_SetPortAutoStartRequest = function (objContext) {
        var profileXMLNode = objContext.RequestDataObjectArr["ProfileXML"];
        var profileXMLStr = "";
        var nodeIter = profileXMLNode.selectNodeIterator("./*");
        while (nodeIter.hasNext()) {
            var nodeNode = nodeIter.next();
            var nodeStr = nodeNode.toString();
            nodeStr = nodeStr.replace(/"/g, "'");
            nodeStr = nodeStr.replace(/<\?.*\?>/, "").replace(/[\n\r]/g, "").replace(/>\s+</g, "><");
            profileXMLStr += nodeStr;
        }
        var profileXMLJSON = profileXMLStr.toJSON();

        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"ProfileXML\":\"\"', '\"ProfileXML\":\"' + profileXMLJSON + '\"');

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + objContext.ObjectList_ByTitle + '\"')
                          .replace('\"@enable\":\"\"', '\"@enable\":\"' + objContext.ObjectList_Enable + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetDeviceConfig request 
    RequestRules.setValue_SetDeviceConfigRequest = function (objContext, objJSONRule) {
        var deviceConfigStr = "";
        var deviceConfigNode = objContext.RequestDataObjectArr["DeviceConfig"];
        var nodeIter = deviceConfigNode.selectNodeIterator("./*");
        while (nodeIter.hasNext()) {
            var nodeNode = nodeIter.next();
            var nodeStr = nodeNode.toString();
            nodeStr = nodeStr.replace(/"/g, "'");
            nodeStr = nodeStr.replace(/<\?.*\?>/, "").replace(/[\n\r]/g, "").replace(/>\s+</g, "><");
            deviceConfigStr += nodeStr;
        }

        var deviceConfigJSON = deviceConfigStr.toJSON();

        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"DeviceConfig\":\"\"', '\"DeviceConfig\":\"' + deviceConfigJSON + '\"');

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')

                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@byTitle\":\"\"', '\"@byTitle\":\"' + objContext.ObjectList_ByTitle + '\"')
                          .replace('\"@path\":\"\"', '\"@path\":\"' + objContext.ObjectList_Path + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetFileInfo request 
    RequestRules.setValueAttr_SetFileInfoRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@title\":\"\"', '\"@title\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_Title) + '\"')
                          .replace('\"@description\":\"\"', '\"@description\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_description) + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for SetFileGroupInfo request 
    RequestRules.setValueAttr_SetFileGroupInfoRequest = function (objContext) {
        var groupsNode = objContext.RequestDataObjectArr["Groups"].selectSingleNode(".");
        var iterTag = groupsNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"Groups\":\"\"', '\"Groups\":\"' + nodeTAGListJSON + '\"');

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for DeleteFile request 
    RequestRules.setValueAttr_DeleteFileRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };


    //set parameters value for ClearDownloadCacheFileRequest request 
    RequestRules.setValueAttr_ClearDownloadCacheRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_DeviceCommandRequest
    RequestRules.setValue_DeviceCommandRequest = function (objContext, objJSONRule) {
        var deviceCommandNode = objContext.RequestDataObjectArr["DeviceCommandRequestData"].selectSingleNode(".");
        var deviceCommandStr = deviceCommandNode.toString();
        deviceCommandStr = deviceCommandStr.replace(/"/g, "'");
        var deviceCommandJSON = deviceCommandStr.toJSON();

        objJSONRule.Body.AgentMessage["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage["@from"] = objContext.AgentMessageHeader.from;
        objJSONRule.Body.AgentMessage["@to"] = objContext.AgentMessageHeader.to;
        objJSONRule.Body.AgentMessage["@type"] = objContext.AgentMessageHeader.type;

        objJSONRule.Body.AgentMessage.agentmessagedata.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.AgentMessage.agentmessagedata.Request.header.clientdata = objContext.TopicName;
        //move previous agentmessageheader/agent to Reques/header/clientcallback
        objJSONRule.Body.AgentMessage.agentmessagedata.Request.header.clientcallback = objContext.AgentMessageHeader.fromagent;
        objJSONRule.Body.AgentMessage.agentmessagedata.Request.data.DeviceCommandRequestData = deviceCommandJSON;


        return objJSONRule.Body
    }

    //set parameters value for SetDeviceNetworkInterface request 
    RequestRules.setValueToSetDeviceNetworkInterfaceRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        var deviceNetworkInterfaceNode = objContext.RequestDataObjectArr["SetDeviceNetworkInterfaceRequestData"].selectSingleNode(".");
        var deviceNetworkInterfaceStr = deviceNetworkInterfaceNode.toString();
        deviceNetworkInterfaceStr = deviceNetworkInterfaceStr.replace(/"/g, "'");
        var deviceNetworkInterfaceJSON = deviceNetworkInterfaceStr.toJSON();

        objJSONRule.Body.Request.data.SetDeviceNetworkInterfaceRequestData = deviceNetworkInterfaceJSON;

        return objJSONRule.Body
    };

    //set parameters value for SetDeviceXMPPAgent request 
    RequestRules.setValueToSetDeviceXMPPAgentRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        //TBD the following
        var deviceXMPPAgentNode = objContext.RequestDataObjectArr["SetDeviceXMPPAgentRequestData"].selectSingleNode(".");
        var deviceXMPPAgentStr = deviceXMPPAgentNode.toString();
        deviceXMPPAgentStr = deviceXMPPAgentStr.replace(/"/g, "'");
        var deviceXMPPAgentJSON = deviceXMPPAgentStr.toJSON();

        objJSONRule.Body.Request.data.SetDeviceXMPPAgentRequestData = deviceXMPPAgentJSON;

        return objJSONRule.Body
    };

    //set parameters value for GetSession request 
    RequestRules.setValueAttr_GetSessionRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                          .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                          .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                          .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                          .replace('\"@whereId\":\"\"', '\"@whereId\":\"' + objContext.ObjectList_WhereID + '\"')
                          .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                          .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                          .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for Stop Session request
    RequestRules.setValueAttr_StopRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.SessionID + '\"');

        return (JSON.parse(objJSONStr)).Body;
    };



    //set parameters value for setValueAttr_GetReportRequest
    RequestRules.setValueAttr_GetReportRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereName\":\"\"', '\"@whereName\":\"' + objContext.ObjectList_name + '\"')
                              .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"');
        return (JSON.parse(objJSONStr)).Body;

    };


    //set parameters value for setValueAttr_AddReportRequest
    RequestRules.setValueAttr_AddReportRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        var policyxslJSON = "";
        var policyFormatXSL = objContext.ObjectList_policyFormatXSL;
        if (!jsx3.util.strEmpty(policyFormatXSL)) {
            var xslstr = policyFormatXSL.toString();
            xslstr = xslstr.replace(/</g, "\&lt;").replace(/>/g, "\&gt;").replace(/\\"/g, "\"");
            policyxslJSON = xslstr.toJSON();
        }

        var auditxslJSON = "";
        var auditFormatXSL = objContext.ObjectList_auditFormatXSL;
        if (!jsx3.util.strEmpty(auditFormatXSL)) {
            var xslstr = auditFormatXSL.toString();
            xslstr = xslstr.replace(/</g, "\&lt;").replace(/>/g, "\&gt;").replace(/\\"/g, "\"");
            auditxslJSON = xslstr.toJSON();
        }

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')

                              .replace('\"@description\":\"\"', '\"@description\":\"' + objContext.ObjectList_description + '\"')
                              .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_Type + '\"')
                              .replace('\"@format\":\"\"', '\"@format\":\"' + objContext.ObjectList_format + '\"')
                              .replace('\"@enableExport\":\"\"', '\"@enableExport\":\"' + objContext.ObjectList_enableExport + '\"')
                              .replace('\"@exportSchedule\":\"\"', '\"@exportSchedule\":\"' + objContext.ObjectList_exportSchedule + '\"')
                              .replace('\"@exportURL\":\"\"', '\"@exportURL\":\"' + objContext.ObjectList_exportURL + '\"')
                              .replace('\"@name\":\"\"', '\"@name\":\"' + RequestRules.escapeSpecialChar(objContext.ObjectList_name) + '\"')
                              .replace('\"@Policy_properties\":\"\"', '\"@properties\":\"' + objContext.ObjectList_policyProperties + '\"')
                              .replace('\"@permissions\":\"\"', '\"@permissions\":\"' + objContext.ObjectList_policyPermissions + '\"')
                              .replace('\"@whereTarget\":\"\"', '\"@whereTarget\":\"' + objContext.ObjectList_WhereTarget + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@whereTagExprMode\":\"\"', '\"@whereTagExprMode\":\"' + objContext.ObjectList_WhereTagExprMode + '\"')
                              .replace('\"@Policy_mode\":\"\"', '\"@mode\":\"' + objContext.ObjectList_formatXSLMode + '\"')
                              .replace('\"@Audit_mode\":\"\"', '\"@mode\":\"' + objContext.ObjectList_formatXSLMode + '\"')
                              .replace('\"@Audit_properties\":\"\"', '\"@properties\":\"' + objContext.ObjectList_auditProperties + '\"')
                              .replace('\"#Policy_text\":\"\"', '\"#text\":\"\"')
                              .replace('\"#Audit_text\":\"\"', '\"#text\":\"\"')
                              ;

        var strBody = (JSON.parse(objJSONStr)).Body;
        strBody.iq.query.AddReport.Policy.FormatXSL["#text"] = policyxslJSON;
        strBody.iq.query.AddReport.Audit.FormatXSL["#text"] = auditxslJSON;
        return strBody;

    };

    //set parameters value for setValueAttr_SetReportRequest
    RequestRules.setValueAttr_SetReportRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);

        var policyxslJSON = "";
        var policyFormatXSL = objContext.ObjectList_policyFormatXSL;
        if (!jsx3.util.strEmpty(policyFormatXSL)) {
            var xslstr = policyFormatXSL.toString();
            xslstr = xslstr.replace(/</g, "\&lt;").replace(/>/g, "\&gt;").replace(/\\"/g, "\"");
            policyxslJSON = xslstr.toJSON();
        }

        var auditxslJSON = "";
        var auditFormatXSL = objContext.ObjectList_auditFormatXSL;
        if (!jsx3.util.strEmpty(auditFormatXSL)) {
            var xslstr = auditFormatXSL.toString();
            xslstr = xslstr.replace(/</g, "\&lt;").replace(/>/g, "\&gt;").replace(/\\"/g, "\"");
            auditxslJSON = xslstr.toJSON();
        }

        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')

                              .replace('\"@description\":\"\"', '\"@description\":\"' + objContext.ObjectList_description + '\"')
                              .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_Type + '\"')
                              .replace('\"@format\":\"\"', '\"@format\":\"' + objContext.ObjectList_format + '\"')
                              .replace('\"@enableExport\":\"\"', '\"@enableExport\":\"' + objContext.ObjectList_enableExport + '\"')
                              .replace('\"@exportSchedule\":\"\"', '\"@exportSchedule\":\"' + objContext.ObjectList_exportSchedule + '\"')
                              .replace('\"@exportURL\":\"\"', '\"@exportURL\":\"' + objContext.ObjectList_exportURL + '\"')
                              .replace('\"@byName\":\"\"', '\"@byName\":\"' + objContext.ObjectList_ByName + '\"')
                              .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                              .replace('\"@Policy_properties\":\"\"', '\"@properties\":\"' + objContext.ObjectList_policyProperties + '\"')
                              .replace('\"@permissions\":\"\"', '\"@permissions\":\"' + objContext.ObjectList_policyPermissions + '\"')
                              .replace('\"@whereTarget\":\"\"', '\"@whereTarget\":\"' + objContext.ObjectList_WhereTarget + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@whereTagExprMode\":\"\"', '\"@whereTagExprMode\":\"' + objContext.ObjectList_WhereTagExprMode + '\"')
                              .replace('\"@Policy_mode\":\"\"', '\"@mode\":\"' + objContext.ObjectList_formatXSLMode + '\"')
                              .replace('\"@Audit_mode\":\"\"', '\"@mode\":\"' + objContext.ObjectList_formatXSLMode + '\"')
                              .replace('\"@Audit_properties\":\"\"', '\"@properties\":\"' + objContext.ObjectList_auditProperties + '\"')
                              .replace('\"#Policy_text\":\"\"', '\"#text\":\"\"')
                              .replace('\"#Audit_text\":\"\"', '\"#text\":\"\"')
                              ;
        var strBody = (JSON.parse(objJSONStr)).Body;
        strBody.iq.query.SetReport.Policy.FormatXSL["#text"] = policyxslJSON;
        strBody.iq.query.SetReport.Audit.FormatXSL["#text"] = auditxslJSON;
        return strBody;
    };

    //set parameters value for setValueAttr_DeleteReportRequest
    RequestRules.setValueAttr_DeleteReportRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')

                              .replace('\"@byName\":\"\"', '\"@byName\":\"' + objContext.ObjectList_ByName + '\"')
                              .replace('\"@byNID\":\"\"', '\"@byNID\":\"' + objContext.ObjectList_ByNID + '\"')
                              ;

        return (JSON.parse(objJSONStr)).Body;

    };

    //set parameters value for setValueAttr_DownloadReportRequest
    RequestRules.setValueAttr_DownloadReportRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var tofile = objContext.ObjectList_toFileName;
        var tofileStr = tofile.toString();
        var tofile = tofileStr.replace(/[\\]/g, "\\\\");
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')

                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereName\":\"\"', '\"@whereName\":\"' + objContext.ObjectList_name + '\"')
                              .replace('\"@toFileName\":\"\"', '\"@toFileName\":\"' + tofile + '\"')
                              ;

        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetSystemConfigRequest
    RequestRules.setValueAttr_GetSystemConfigRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereConfig\":\"\"', '\"@whereConfig\":\"' + objContext.ObjectList_WhereConfig + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetSessionConfig
    RequestRules.setValueAttr_GetSessionConfig = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for Set Auto Device Discovery request 
    RequestRules.setValueAttr_SetAutoDiscoveryRequest = function (objContext) {
        var groupsNode = objContext.RequestDataObjectArr["Groups"].selectSingleNode(".");
        var iterTag = groupsNode.selectNodeIterator(".//Tag");
        var nodeTAGList = "";
        while (iterTag.hasNext()) {
            var nodeTAG = iterTag.next();
            var strTAG = nodeTAG.toString();
            strTAG = strTAG.replace(/"/g, "'");
            nodeTAGList += strTAG;
        }
        var nodeTAGListJSON = nodeTAGList.toJSON();
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"Groups\":\"\"', '\"Groups\":\"' + nodeTAGListJSON + '\"');
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@enable\":\"\"', '\"@enable\":\"' + objContext.ObjectList_Enable + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for Set Writer Options request 
    RequestRules.setValueAttr_SetWriterOptionsRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                          .replace('\"@enableHLS\":\"\"', '\"@enableHLS\":\"' + objContext.ObjectList_Enable + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for Set Multicast IPAddress Pool request 
    RequestRules.setValueAttr_SetMulticastIPAddressPoolRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                          .replace('\"@whereContactJID\":\"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_ByNID + '\"')
                          .replace('\"@enable\":\"\"', '\"@enable\":\"' + objContext.ObjectList_Enable + '\"')
                          .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.ObjectList_StartIPAddress + '\"')
                          .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.ObjectList_EndIPAddress + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetBookmark
    RequestRules.setValueAttr_GetBookmark = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                              .replace('\"@whereTCRange\":\"\"', '\"@whereTCRange\":\"' + objContext.whereTCRange + '\"')
                              .replace('\"@whereFileNID\":\"\"', '\"@whereFileNID\":\"' + objContext.whereFileNID + '\"')
                              .replace('\"@whereSessionInfo\":\"\"', '\"@whereSessionInfo\":\"' + objContext.whereSessionInfo + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValue_AddBookmark
    RequestRules.setValue_AddBookmark = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var addBookmarkNode = objContext.RequestDataObjectArr["AddBookmark"].selectSingleNode(".");
        var addBookmarkStr = addBookmarkNode.toString();
        addBookmarkStr = addBookmarkStr.replace(/"/g, "'");
        var addBookmarkJSON = addBookmarkStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = addBookmarkJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValue_UpdateBookmark
    RequestRules.setValue_UpdateBookmark = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var updateBookmarkNode = objContext.RequestDataObjectArr["UpdateBookmark"].selectSingleNode(".");
        var updateBookmarkStr = updateBookmarkNode.toString();
        updateBookmarkStr = updateBookmarkStr.replace(/"/g, "'");
        var updateBookmarkJSON = updateBookmarkStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = updateBookmarkJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValue_DeleteBookmark
    RequestRules.setValue_DeleteBookmark = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var deleteBookmarkNode = objContext.RequestDataObjectArr["DeleteBookmark"].selectSingleNode(".");
        var deleteBookmarkStr = deleteBookmarkNode.toString();
        deleteBookmarkStr = deleteBookmarkStr.replace(/"/g, "'");
        var deleteBookmarkJSON = deleteBookmarkStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = deleteBookmarkJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValueAttr_ResumeFromBookmarkRequest
    RequestRules.setValueAttr_ResumeFromBookmarkRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                              .replace('\"@timeScale\":\"\"', '\"@timeScale\":\"' + objContext.timeScale + '\"')
                              .replace('\"@bookmarkId\":\"\"', '\"@bookmarkId\":\"' + objContext.bookmark_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_DeleteConnRequest
    RequestRules.setValueAttr_DeleteConnRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                              .replace('\"@connId\":\"\"', '\"@connId\":\"' + objContext.conn_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_PauseRequest
    RequestRules.setValueAttr_PauseRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_PlayRequest
    RequestRules.setValueAttr_PlayRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                              .replace('\"@timeScale\":\"\"', '\"@timeScale\":\"' + objContext.timeScale + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_SkipBackRequest
    RequestRules.setValueAttr_SkipBackRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                              .replace('\"@skipOffset\":\"\"', '\"@skipOffset\":\"' + objContext.skipOffset + '\"')
                              .replace('\"@timeScale\":\"\"', '\"@timeScale\":\"' + objContext.timeScale + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_SkipFwdRequest
    RequestRules.setValueAttr_SkipFwdRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                              .replace('\"@skipOffset\":\"\"', '\"@skipOffset\":\"' + objContext.skipOffset + '\"')
                              .replace('\"@timeScale\":\"\"', '\"@timeScale\":\"' + objContext.timeScale + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_CatchupRequest
    RequestRules.setValueAttr_CatchupRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_PauseRecordRequest
    RequestRules.setValueAttr_PauseRecordRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_ResumeRecordRequest
    RequestRules.setValueAttr_ResumeRecordRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValue_StartRequest
    RequestRules.setValue_StartRequest = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var sessionConfigNode = objContext.RequestDataObjectArr["Session"].selectSingleNode(".");
        var sessionConfigStr = sessionConfigNode.toString();
        sessionConfigStr = sessionConfigStr.replace(/"/g, "'");
        var sessionXMLJSON = sessionConfigStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query.StartSession["@sessionId"] = objContext.SessionID;
        objJSONRule.Body.iq.query.StartSession["#text"] = sessionXMLJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValueAttr_JoinRequest
    RequestRules.setValueAttr_JoinRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValue_SetPortDisplayConfig
    RequestRules.setValue_SetPortDisplayConfig = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var setPortDisplayConfigNode = objContext.RequestDataObjectArr["SetPortDisplayConfig"].selectSingleNode(".");
        var setPortDisplayConfigStr = setPortDisplayConfigNode.toString();
        setPortDisplayConfigStr = setPortDisplayConfigStr.replace(/"/g, "'");
        var setPortDisplayConfigJSON = JSON.stringify(setPortDisplayConfigStr);
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = setPortDisplayConfigJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValue_SetPortWindowProperties
    RequestRules.setValue_SetPortWindowProperties = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var setPortWindowPropertiesNode = objContext.RequestDataObjectArr["SetPortWindowProperties"].selectSingleNode(".");
        var setPortWindowPropertiesStr = setPortWindowPropertiesNode.toString();
        setPortWindowPropertiesStr = setPortWindowPropertiesStr.replace(/"/g, "'");
        var setPortWindowPropertiesJSON = setPortWindowPropertiesStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = setPortWindowPropertiesJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValue_SetServiceClientDisplayStatus
    RequestRules.setValue_SetServiceClientDisplayStatus = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var seNode = objContext.RequestDataObjectArr["SetServiceClientDisplayStatus"].selectSingleNode(".");
        var setStr = seNode.toString();
        setStr = setStr.replace(/"/g, "'");
        var setJSON = setStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = setJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValue_SetServiceClientDisplayConfig
    RequestRules.setValue_SetServiceClientDisplayConfig = function (objContext, objJSONRule) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        var seNode = objContext.RequestDataObjectArr["SetServiceClientDisplayConfig"].selectSingleNode(".");
        var setStr = seNode.toString();
        setStr = setStr.replace(/"/g, "'");
        var setJSON = setStr.toJSON();
        objJSONRule.Body.iq["@from"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        objJSONRule.Body.iq["@id"] = objContext.TopicName;
        objJSONRule.Body.iq["@to"] = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID);
        objJSONRule.Body.iq.query = setJSON;
        return objJSONRule.Body;
    };

    //set parameters value for setValueAttr_ViewRequest
    RequestRules.setValueAttr_ViewRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                              .replace('\"@id\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                              .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                              .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                              .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                              .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                              .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                              .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                              .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                              .replace('\"@DstPort_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                              .replace('\"@DstPort_portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                              .replace('\"@DstPort_portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"')
                              .replace('\"@SrcPort_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_srcEntityId + '\"')
                              .replace('\"@SrcPort_portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_srcPortNID + '\"')
                              .replace('\"@SrcPort_portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_srcPortTitle + '\"')
                              .replace('\"@SrcPort_pvrEnable\":\"\"', '\"@pvrEnable\":\"' + objContext.ObjectList_srcPVREnable + '\"')
                              .replace('\"@SrcPort_trimLength\":\"\"', '\"@trimLength\":\"' + objContext.ObjectList_srcPVRTrimLength + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetPortDisplayStatusRequest
    RequestRules.setValueAttr_GetPortDisplayStatusRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetPortTagsRequest
    RequestRules.setValueAttr_GetPortTagsRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                              .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                              .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                              .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                              .replace('\"@whereNameTextSubString\":\"\"', '\"@whereNameTextSubString\":\"' + objContext.ObjectList_WhereNameTextSubStr + '\"')
                              .replace('\"@whereValueTextSubString\":\"\"', '\"@whereValueTextSubString\":\"' + objContext.ObjectList_WhereValueTextSubStr + '\"')
                              .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                              .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_PlayBackSrcRequest
    RequestRules.setValueAttr_PlayBackSrcRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                              .replace('\"@id\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                              .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                              .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                              .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                              .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                              .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                              .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                              .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                              .replace('\"@DstPort_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                              .replace('\"@DstPort_portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                              .replace('\"@DstPort_portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"')
                              .replace('\"@SrcFile_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_fileEntityId + '\"')
                              .replace('\"@SrcFile_fileNID\":\"\"', '\"@fileNID\":\"' + objContext.ObjectList_fileNID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_PlayBackGrpRequest
    RequestRules.setValueAttr_PlayBackGrpRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                              .replace('\"@id\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                              .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                              .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                              .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                              .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                              .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                              .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                              .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                              .replace('\"@DstPort_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                              .replace('\"@DstPort_portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                              .replace('\"@DstPort_portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"')
                              .replace('\"@GrpFile_id\":\"\"', '\"@id\":\"' + objContext.ObjectList_fileEntityId + '\"')
                              .replace('\"@GrpFile_fileNID\":\"\"', '\"@fileNID\":\"' + objContext.ObjectList_fileNID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    //set parameters value for setValueAttr_GetPortDisplayConfigRequest
    RequestRules.setValueAttr_GetPortDisplayConfigRequest = function (objContext) {
        var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
        objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                              .replace('\"@from\":\"\"', '\"@from\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID) + '\"')
                              .replace('\"@to\":\"\"', '\"@to\":\"' + IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPSERVERAGENTJID) + '\"')
                              .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                              .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                              .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"');
        return (JSON.parse(objJSONStr)).Body;
    };

    // setValue_GetBookmarkListRequest
    RequestRules.setValue_GetBookmarkListRequest = function (objContext, objJSONRule) {
        objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
        objJSONRule.Body.Request["@serviceVer"] = getServiceVersion();
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.MediaStreamFileResourceNID = objContext.MediaStreamFileResourceNID;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Where.WhereXPath = objContext.WhereXPath;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXML = objContext.Op_Select_ItemSpec_SelectXML;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Select.ItemSpec.SelectXQuery = objContext.Op_Select_ItemSpec_SelectXQuery;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Select.ListSpec.StartFrom = objContext.StartFrom;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Select.ListSpec.CountToFetch = objContext.CountToFetch;
        objJSONRule.Body.Request.data.GetBookmarkListRequestData.GetRequest.PathSpec.Select.ListSpec.SortByXPathList.SortByXpath = objContext.Op_Select_ListSpec_SortByXPathList_SortByXpath;
        return objJSONRule.Body;
    }

    RequestRules.escapeJson = function (str) {
        return str.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t");
    }

    //This function added to fix bug#47464
    //JSON.parse have problem with \ and dom4j have problem with & < etc..
    RequestRules.escapeSpecialChar = function (str) {
        return str.replace(/[<>\&\\]/g, function (c) {
            return '&#' + c.charCodeAt(0) + ';';
        });
    }

    //get a specific rule with a specific caller context (which includes request name)
    RequestRules.getRule = function (objContext) {
        var requestName = objContext.RequestName;
        if (objContext.RequestName == "AssetManagementRequest" && objContext.OpType == "Get") {
            requestName = "Get" + objContext.RequestName;
        }
        else if (objContext.RequestName == "AssetManagementRequest" && objContext.OpType == "ReadQuery") {
            requestName = "ReadQuery" + objContext.RequestName;
        }
        else if (objContext.RequestName == "AssetManagementRequest" && objContext.OpType == "Replace") {
            requestName = "Replace" + objContext.RequestName;
        }
        else if (objContext.RequestName == "AssetManagementRequest" && objContext.OpType == "Add") {
            requestName = "Add" + objContext.RequestName;
        }
        else if (objContext.RequestName == "AssetManagementRequest" && objContext.OpType == "Delete") {
            requestName = "Delete" + objContext.RequestName;
        }

        if (objContext.RequestName == "PolicyManagementRequest" && objContext.OpType == "Get") {
            requestName = "Get" + objContext.RequestName;
        }
        else if (objContext.RequestName == "PolicyManagementRequest" && objContext.OpType == "Replace") {
            requestName = "Replace" + objContext.RequestName;
        }
        else if (objContext.RequestName == "PolicyManagementRequest" && objContext.OpType == "Add") {
            requestName = "Add" + objContext.RequestName;
        }
        else if (objContext.RequestName == "PolicyManagementRequest" && objContext.OpType == "Delete") {
            requestName = "Delete" + objContext.RequestName;
        }

        var objJSONRule = RequestRules[requestName];
        if (!objJSONRule.CallBack.setValueAndAttribute) {
            objJSONRule = eval(objJSONRule.CallBack.setValue)(objContext, objJSONRule);
        }
        else {
            objJSONRule = eval(objJSONRule.CallBack.setValueAndAttribute)(objContext);
        }
        return objJSONRule;
    };

});