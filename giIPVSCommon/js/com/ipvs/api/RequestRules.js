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
        "Request": {
          "@serviceVer": "",
          "@Ver": "1.1",
          "header": {
            "servicename": "xmpp",
            "requestname": "LoginRequest",
            "loglevel": "0",
            "clientdata": ""
          },
          "data": {
            "LoginRequestData": {
              "user": "",
              "password": "",
              "resource": "",
              "server": "",
              "port": ""
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
        "Request": {
          "@serviceVer": "",
          "@Ver": "1.1",
          "header": {
            "servicename": "xmpp",
            "requestname": "LogoutRequest",
            "loglevel": "0",
            "clientdata": ""
          },
          "data": {
            "LoginRequestData": "Logout"
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
        "Request": {
          "@serviceVer": "",
          "@Ver": "1.1",
          "header": {
            "servicename": "systemservice",
            "requestname": "SetServiceDomainRequest",
            "loglevel": "0",
            "clientdata": ""
          },
          "data": {
            "SetServiceDomainRequestData": {
              "domain": "default",
              "AssetTemplateTitle": "",
              "AssetPrivateKeyData": "",
              "ServiceVersion": "",
              "SoftwareVersion": "",
              "IPAddress": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": "RequestRules.setValueToSetServiceDomainRequest",
        "setValueAndAttribute": null
      }

    };

  //login getServiceClientRequest
  RequestRules.GetServiceClientRequest =
    {
      "Body": {
        "iq": {
          "@from": "",
          "@id": "",
          "@to": "",
          "@type": "set ",
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
        "Request": {
          "@serviceVer": "",
          "@Ver": "1.1",
          "header": {
            "servicename": "systemservice",
            "requestname": "GetServerTimeRequest",
            "loglevel": "0",
            "clientdata": ""
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

  //Get File List
  RequestRules.GetFileRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "GetFile": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "",
              "@searchTextSubStrIn": "", "@whereDateRange": "", "@whereTagExpr": "", "@selectFormat": "", "@whereParentDirNID": "",
              "@whereSessionConfigNID": "", "@sortByMetaData": "", "@whereState": "", "@whereType": "", "@wherePermissions": "",
              "@whereMetaDataExpr": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_GetFileRequest"
      }
    };

  //Get Port List
  RequestRules.GetPortRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "GetPort": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortBy": "", "@whereNID": "", "@whereTextSubStr": "",
              "@whereDateRange": "", "@whereTagExpr": "", "@subscriptionID": "", "@selectFormat": "", "@type": "", "@wherePermissions": "", "@whereStreamType": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_GetPortRequest"
      }
    };

  //Pause Request
  RequestRules.PauseRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "Pause": { "@sessionId": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_PauseRequest"
      }
    };

  //Resume Request
  RequestRules.ResumeRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "Resume": { "@sessionId": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_ResumeRequest"
      }
    };

  //SkipBack Request
  RequestRules.SkipBackRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "SkipBack": { "@sessionId": "", "@skipOffset": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_SkipBackRequest"
      }
    };

  //SkipFwd Request
  RequestRules.SkipFwdRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "SkipFwd": { "@sessionId": "", "@skipOffset": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_SkipFwdRequest"
      }
    };

  //Catchup Request
  RequestRules.CatchupRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "Catchup": { "@sessionId": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_CatchupRequest"
      }
    };

  //Stop Request
  RequestRules.StopRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
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

  //View Request
  RequestRules.ViewRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "View": {
              "DstPort": { "@DstPortId": "", "@DstPortNID": "", "@DstPortTitle": "" },
              "SrcPort": { "@SrcPortId": "", "@SrcPortNID": "", "@SrcPortTitle": "", "@SrcPortPVREnable": "" },
              "@sessionId": "", "@ViewId": "", "@srcEntityId": "", "@sessionName": "", "@name": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_ViewRequest"
      }
    };

  //Record Request
  RequestRules.RecordRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "Record": {
              "Profile": { "@ProfileId": "", "@profileNID": "", "@profileTitle": "" },
              "SrcPort": { "@SrcPortId": "", "@portNID": "", "@portTitle": "" },
              "GrpFile": { "@GrpFileId": "", "@fileNID": "" },
              "DstFile": {
                "MetaData": "",
                "@DstFileId": "", "@fileTitle": "", "@dirNID": "", "@DstFileGrpFileId": "", "@dirId": "", "@key": "", "@action": "", "@keyMetaDataKeyName": ""
              },
              "@sessionId": "", "@sessionName": "", "@RecordId": "", "@name": "", "@srcEntityId": "", "@resetTimeline": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_RecordRequest"
      }
    };

  //Delete Connection Request
  RequestRules.DeleteConnRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "DeleteConn": { "@sessionId": "", "@connId": "" }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_DeleteConnRequest"
      }
    };

  //Playback Src Request
  RequestRules.PlayBackSrcRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "Playback": {
              "SrcFile": { "@SrcFileId": "", "@fileNID": "" },
              "DstPort": { "@DstPortId": "", "@portNID": "", "@portTitle": "" },
              "@sessionId": "", "@sessionName": "", "@PlaybackId": "", "@name": "", "@srcEntityId": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_PlayBackSrcRequest"
      }
    };

  //Playback Grp Request
  RequestRules.PlayBackGrpRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "Playback": {
              "GrpFile": { "@GrpFileId": "", "@fileNID": "" },
              "DstPort": { "@DstPortId": "", "@portNID": "", "@portTitle": "" },
              "@sessionId": "", "@sessionName": "", "@PlaybackId": "", "@name": "", "@srcEntityId": "", "@dstEntityId": "", "@windowId": "", "@canvasId": "", "@restartSession": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_PlayBackGrpRequest"
      }
    };

  //Get Bookmark Request
  RequestRules.GetBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "GetBookmark": { "@startFrom": "", "@countToFetch": "", "@sortOrder": "", "@sortby": "", "@whereNID": "", "@whereTextSubStr": "",
              "@whereDateRange": "", "@whereTCRange": "", "@whereFileNID": "", "@whereSessionInfo": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_GetBookmarkRequest"
      }
    };

  //New Bookmark Request
  RequestRules.NewBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "NewBookmark": { "@whereFileNID": "", "@whereSessionInfo": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_NewBookmarkRequest"
      }
    };

  //Add Bookmark Request
  RequestRules.AddBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "AddBookmark": {
              "Bookmark": {
                "Description": "",
                "MetaData": "",
                "@title": "", "@TC": "", "@offset": ""
              },
              "@whereFileNID": "", "@whereSessionInfo": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_AddBookmarkRequest"
      }
    };

  //Update Bookmark Request
  RequestRules.UpdateBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "UpdateBookmark": {
              "Bookmark": {
                "Description": "",
                "MetaData": "",
                "@TC": "", "@offset": "", "@title": ""
              },
              "@whereNID": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_UpdateBookmarkRequest"
      }
    };

  //Resume From Bookmark Request
  RequestRules.ResumeFromBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "Resume": { "@sessionId": "", "@bookmarkId": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_ResumeFromBookmarkRequest"
      }
    };

  //Delete Bookmark Request
  RequestRules.DeleteBookmarkRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "get ",
          "query": {
            "DeleteBookmark": { "@whereNID": "", "@whereTextSubStr": "", "@whereDateRange": "", "@whereTcRange": "", "@whereFileNID": "", "@whereSessionInfo": ""
            }
          }
        }
      },
      "CallBack": {
        "setValue": null,
        "setValueAndAttribute": "RequestRules.setValueAttr_DeleteBookmarkRequest"
      }
    };

  //AuditEvent Request
  RequestRules.AuditEventRequest =
    {
      "Body": {
        "iq": { "@from": "", "@id": "", "@to": "", "@type": "set ",
          "query": {
            "AuditEvent": { "@type": "", "@date": "", "@time": "", "@level": "", "@name": "",
              "Description": { "@text": "", "EventUser": "", "EventAsset": "" }
            }
          }
        }
      },
      "CallBack": {
        "setValue": "RequestRules.setValue_AuditEventRequest",
        "setValueAndAttribute": null
      }
    };

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
    objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
    return objJSONRule.Body
  };

  //set parameters value  setServiceDomain request
  RequestRules.setValueToSetServiceDomainRequest = function (objContext, objJSONRule) {
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

  //set parameters value to GetServiceClient request
  RequestRules.setValueToGetServiceClientRequest = function (objContext, objJSONRule) {

  };

  //set attributes and parameters value to GetServiceClient request
  RequestRules.setValueAndAttrToGetServiceClientRequest = function (objContext) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
            .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
            .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
            .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"');
    return (JSON.parse(objJSONStr)).Body;
  };

  //set parameters value  GetServerTime request
  RequestRules.setValueToGetServerTimeRequest = function (objContext, objJSONRule) {
    objJSONRule.Body.Request.header.clientdata = objContext.TopicName;
    objJSONRule.Body.Request.data.GetServerTimeRequestData = "";
    return objJSONRule.Body
  };

  //set parameters value for GetFile request
  RequestRules.setValueAttr_GetFileRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
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
                                .replace('\"@whereParentDirNID\":\"\"', '\"@whereParentDirNID\":\"' + objContext.ObjectList_WhereParentDirNID + '\"')
                                .replace('\"@whereSessionConfigNID\":\"\"', '\"@whereSessionConfigNID\":\"' + objContext.ObjectList_WhereSessionConfigNID + '\"')
                                .replace('\"@sortByMetaData\":\"\"', '\"@sortByMetaData\":\"' + objContext.ObjectList_SortByMetaData + '\"')
                                .replace('\"@whereState\":\"\"', '\"@whereState\":\"' + objContext.ObjectList_WhereState + '\"')
                                .replace('\"@whereType\":\"\"', '\"@whereType\":\"' + objContext.ObjectList_WhereType + '\"')
                                .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                                .replace('\"@whereMetaDataExpr\":\"\"', '\"@whereMetaDataExpr\":\"' + objContext.ObjectList_WhereMetaDataExpr + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for GetPort request
  RequestRules.setValueAttr_GetPortRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                                .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                                .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                                .replace('\"@sortBy\":\"\"', '\"@sortBy\":\"' + objContext.ObjectList_SortBy + '\"')
                                .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                                .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                                .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                                .replace('\"@whereTagExpr\":\"\"', '\"@whereTagExpr\":\"' + objContext.ObjectList_WhereTagExpr + '\"')
                                .replace('\"@subscriptionID\":\"\"', '\"@subscriptionID\":\"' + objContext.ObjectList_SubscriptionID + '\"')
                                .replace('\"@selectFormat\":\"\"', '\"@selectFormat\":\"' + objContext.ObjectList_SelectFormat + '\"')
                                .replace('\"@type\":\"\"', '\"@type\":\"' + objContext.ObjectList_Type + '\"')
                                .replace('\"@wherePermissions\":\"\"', '\"@wherePermissions\":\"' + objContext.ObjectList_WherePermissions + '\"')
                                .replace('\"@whereStreamType\":\"\"', '\"@whereStreamType\":\"' + objContext.ObjectList_WhereStreamType + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Pause request
  RequestRules.setValueAttr_PauseRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Resume request
  RequestRules.setValueAttr_ResumeRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for SkipBack request
  RequestRules.setValueAttr_SkipBackRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                                .replace('\"@skipOffset\":\"\"', '\"@skipOffset\":\"' + objContext.skipOffset + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for SkipFwd request
  RequestRules.setValueAttr_SkipFwdRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                                .replace('\"@skipOffset\":\"\"', '\"@skipOffset\":\"' + objContext.skipOffset + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Catchup request
  RequestRules.setValueAttr_CatchupRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for StopSession request
  RequestRules.setValueAttr_StopRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for View request
  RequestRules.setValueAttr_ViewRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                                .replace('\"@ViewId\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                                .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                                .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                                .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                                .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                                .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                                .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                                .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                                .replace('\"@DstPortId\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                                .replace('\"@DstPortNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                                .replace('\"@DstPortTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"')
                                .replace('\"@SrcPortId\":\"\"', '\"@id\":\"' + objContext.ObjectList_srcEntityId + '\"')
                                .replace('\"@SrcPortNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_srcPortNID + '\"')
                                .replace('\"@SrcPortTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_srcPortTitle + '\"')
                                .replace('\"@SrcPortPVREnable\":\"\"', '\"@pvrEnable\":\"' + objContext.ObjectList_pvrEnable + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Record request
  RequestRules.setValueAttr_RecordRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                                .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                                .replace('\"@RecordId\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                                .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                                .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_srcEntityId + '\"')
                                .replace('\"@resetTimeline\":\"\"', '\"@resetTimeline\":\"' + objContext.ObjectList_resetTimeline + '\"')
                                .replace('\"@ProfileId\":\"\"', '\"@id\":\"' + objContext.ObjectList_profileId + '\"')
                                .replace('\"@profileNID\":\"\"', '\"@profileNID\":\"' + objContext.ObjectList_profileNID + '\"')
                                .replace('\"@profileTitle\":\"\"', '\"@profileTitle\":\"' + objContext.ObjectList_profileTitle + '\"')
                                .replace('\"@SrcPortId\":\"\"', '\"@id\":\"' + objContext.ObjectList_srcEntityId + '\"')
                                .replace('\"@portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_srcPortNID + '\"')
                                .replace('\"@portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_srcPortTitle + '\"')
                                .replace('\"@GrpFileId\":\"\"', '\"@id\":\"' + objContext.ObjectList_grpFileId + '\"')
                                .replace('\"@fileNID\":\"\"', '\"@fileNID\":\"' + objContext.ObjectList_grpFileNID + '\"')
                                .replace('\"@DstFileId\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                                .replace('\"@fileTitle\":\"\"', '\"@fileTitle\":\"' + objContext.ObjectList_dstFileTitle + '\"')
                                .replace('\"@dirNID\":\"\"', '\"@dirNID\":\"' + objContext.ObjectList_dstDirNID + '\"')
                                .replace('\"@DstFileGrpFileId\":\"\"', '\"@grpFileId\":\"' + objContext.ObjectList_grpFileId + '\"')
                                .replace('\"@dirId\":\"\"', '\"@dirId\":\"' + objContext.ObjectList_dirId + '\"')
                                .replace('\"@key\":\"\"', '\"@key\":\"' + objContext.ObjectList_Key + '\"')
                                .replace('\"@action\":\"\"', '\"@action\":\"' + objContext.ObjectList_Action + '\"')
                                .replace('\"@keyMetaDataKeyName\":\"\"', '\"@keyMetaDataKeyName\":\"' + objContext.ObjectList_keyMetaDataKeyName + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Delete Connection request
  RequestRules.setValueAttr_DeleteConnRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                                .replace('\"@connId\":\"\"', '\"@connId\":\"' + objContext.ObjectList_connId + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Playback Src request
  RequestRules.setValueAttr_PlayBackSrcRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                                .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                                .replace('\"@PlaybackId\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                                .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                                .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                                .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                                .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                                .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                                .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                                .replace('\"@SrcFileId\":\"\"', '\"@id\":\"' + objContext.ObjectList_fileEntityId + '\"')
                                .replace('\"@fileNID\":\"\"', '\"@fileNID\":\"' + objContext.ObjectList_fileNID + '\"')
                                .replace('\"@DstPortId\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                                .replace('\"@portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                                .replace('\"@portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Playback Grp request
  RequestRules.setValueAttr_PlayBackGrpRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.ObjectList_sessionId + '\"')
                                .replace('\"@sessionName\":\"\"', '\"@sessionName\":\"' + objContext.ObjectList_sessionName + '\"')
                                .replace('\"@PlaybackId\":\"\"', '\"@id\":\"' + objContext.ObjectList_connId + '\"')
                                .replace('\"@name\":\"\"', '\"@name\":\"' + objContext.ObjectList_connName + '\"')
                                .replace('\"@srcEntityId\":\"\"', '\"@srcEntityId\":\"' + objContext.ObjectList_SrcEntityId + '\"')
                                .replace('\"@dstEntityId\":\"\"', '\"@dstEntityId\":\"' + objContext.ObjectList_DstEntityId + '\"')
                                .replace('\"@windowId\":\"\"', '\"@windowId\":\"' + objContext.ObjectList_windowId + '\"')
                                .replace('\"@canvasId\":\"\"', '\"@canvasId\":\"' + objContext.ObjectList_canvasId + '\"')
                                .replace('\"@restartSession\":\"\"', '\"@restartSession\":\"' + objContext.ObjectList_restartSession + '\"')
                                .replace('\"@GrpFileId\":\"\"', '\"@id\":\"' + objContext.ObjectList_fileEntityId + '\"')
                                .replace('\"@fileNID\":\"\"', '\"@fileNID\":\"' + objContext.ObjectList_fileNID + '\"')
                                .replace('\"@DstPortId\":\"\"', '\"@id\":\"' + objContext.ObjectList_dstEntityId + '\"')
                                .replace('\"@portNID\":\"\"', '\"@portNID\":\"' + objContext.ObjectList_dstPortNID + '\"')
                                .replace('\"@portTitle\":\"\"', '\"@portTitle\":\"' + objContext.ObjectList_dstPortTitle + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for GetBookmark request
  RequestRules.setValueAttr_GetBookmarkRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@startFrom\":\"\"', '\"@startFrom\":\"' + objContext.ObjectList_StartFrom + '\"')
                                .replace('\"@countToFetch\":\"\"', '\"@countToFetch\":\"' + objContext.ObjectList_CountToFetch + '\"')
                                .replace('\"@sortOrder\":\"\"', '\"@sortOrder\":\"' + objContext.ObjectList_SortOrder + '\"')
                                .replace('\"@sortby\":\"\"', '\"@sortby\":\"' + objContext.ObjectList_SortBy + '\"')
                                .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + objContext.ObjectList_WhereNID + '\"')
                                .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + objContext.ObjectList_WhereTextSubStr + '\"')
                                .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + objContext.ObjectList_WhereDateRange + '\"')
                                .replace('\"@whereTCRange\":\"\"', '\"@whereTCRange\":\"' + objContext.ObjectList_WhereTCRange + '\"')
                                .replace('\"@whereFileNID\":\"\"', '\"@whereFileNID\":\"' + objContext.ObjectList_WhereFileNID + '\"')
                                .replace('\"@whereSessionInfo\":\"\"', '\"@whereSessionInfo\":\"' + objContext.ObjectList_WhereSessionInfo + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for New Bookmark request
  RequestRules.setValueAttr_NewBookmarkRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@whereFileNID\":\"\"', '\"@whereFileNID\":\"' + objContext.ObjectList_WhereFileNID + '\"')
                                .replace('\"@whereSessionInfo\":\"\"', '\"@whereSessionInfo\":\"' + objContext.ObjectList_WhereSessionInfo + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Add Bookmark request
  RequestRules.setValueAttr_AddBookmarkRequest = function (objContext, objJSONRule) {
    var addBookmarkNode = objContext.RequestDataObjectArr["AddBookmark"];
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    if (jsx3.util.strEmpty(addBookmarkNode.selectSingleNode(".//MetaData"))) {
      objJSONStr = objJSONStr.replace('\"MetaData\":\"\",', '');
    }
    else {
      var metadataNode = addBookmarkNode.selectSingleNode(".//MetaData");
      var nodeTAGListJSON = RequestRules.replace_TagNode(metadataNode);
      objJSONStr = objJSONStr.replace('\"MetaData\":\"\"', '\"MetaData\":\"' + nodeTAGListJSON + '\"');
    }
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@whereFileNID\":\"\"', '\"@whereFileNID\":\"' + addBookmarkNode.getAttribute("whereFileNID") + '\"')
                                .replace('\"@whereSessionInfo\":\"\"', '\"@whereSessionInfo\":\"' + addBookmarkNode.getAttribute("whereSessionInfo") + '\"')
                                .replace('\"@title\":\"\"', '\"@title\":\"' + addBookmarkNode.selectSingleNode("//Bookmark").getAttribute("title") + '\"')
                                .replace('\"@TC\":\"\"', '\"@TC\":\"' + addBookmarkNode.selectSingleNode("//Bookmark").getAttribute("TC") + '\"')
                                .replace('\"@offset\":\"\"', '\"@offset\":\"' + addBookmarkNode.selectSingleNode("//Bookmark").getAttribute("offset") + '\"')
                                .replace('\"Description\":\"\"', '\"Description\":\"' + RequestRules.escapeSpecialChar(addBookmarkNode.selectSingleNode(".//Bookmark/Description").getValue()) + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Update Bookmark request
  RequestRules.setValueAttr_UpdateBookmarkRequest = function (objContext, objJSONRule) {
    var updateBookmarkNode = objContext.RequestDataObjectArr["UpdateBookmark"];
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    if (jsx3.util.strEmpty(updateBookmarkNode.selectSingleNode(".//MetaData"))) {
      objJSONStr = objJSONStr.replace('\"MetaData\":\"\",', '');
    }
    else {
      var metadataNode = updateBookmarkNode.selectSingleNode(".//MetaData");
      var nodeTAGListJSON = RequestRules.replace_TagNode(metadataNode);
      objJSONStr = objJSONStr.replace('\"MetaData\":\"\"', '\"MetaData\":\"' + nodeTAGListJSON + '\"');
    }
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + updateBookmarkNode.getAttribute("whereNID") + '\"')
                                .replace('\"@TC\":\"\"', '\"@TC\":\"' + updateBookmarkNode.selectSingleNode("//Bookmark").getAttribute("TC") + '\"')
                                .replace('\"@offset\":\"\"', '\"@offset\":\"' + updateBookmarkNode.selectSingleNode("//Bookmark").getAttribute("offset") + '\"')
                                .replace('\"Description\":\"\"', '\"Description\":\"' + RequestRules.escapeSpecialChar(updateBookmarkNode.selectSingleNode(".//Bookmark/Description").getValue()) + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for ResumeFrom Bookmark request
  RequestRules.setValueAttr_ResumeFromBookmarkRequest = function (objContext, objJSONRule) {
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@sessionId\":\"\"', '\"@sessionId\":\"' + objContext.session_id + '\"')
                                .replace('\"@bookmarkId\":\"\"', '\"@bookmarkId\":\"' + objContext.bookmark_id + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  //set parameters value for Delete Bookmark request
  RequestRules.setValueAttr_DeleteBookmarkRequest = function (objContext, objJSONRule) {
    var deleteBookmarkNode = objContext.RequestDataObjectArr["DeleteBookmark"];
    var objJSONStr = JSON.stringify(RequestRules[objContext.RequestName]);
    objJSONStr = objJSONStr.replace('\"@id\":\"\"', '\"@id\":\"' + objContext.TopicName + '\"')
                                .replace('\"@from\":\"\"', '\"@from\":\"' + objContext.iq_from + '\"')
                                .replace('\"@to\":\"\"', '\"@to\":\"' + objContext.iq_to + '\"')
                                .replace('\"@whereContactJID\": \"\"', '\"@whereContactJID\":\"' + objContext.ObjectList_whereContactJID + '\"')
                                .replace('\"@whereNID\":\"\"', '\"@whereNID\":\"' + deleteBookmarkNode.getAttribute("whereNID") + '\"')
                                .replace('\"@whereTextSubStr\":\"\"', '\"@whereTextSubStr\":\"' + deleteBookmarkNode.getAttribute("whereTextSubStr") + '\"')
                                .replace('\"@whereDateRange\":\"\"', '\"@whereDateRange\":\"' + deleteBookmarkNode.getAttribute("whereDateRange") + '\"')
                                .replace('\"@whereTcRange\":\"\"', '\"@whereTcRange\":\"' + deleteBookmarkNode.getAttribute("whereTcRange") + '\"')
                                .replace('\"@whereFileNID\":\"\"', '\"@whereFileNID\":\"' + deleteBookmarkNode.getAttribute("whereFileNID") + '\"')
                                .replace('\"@whereSessionInfo\":\"\"', '\"@whereSessionInfo\":\"' + deleteBookmarkNode.getAttribute("whereSessionInfo") + '\"');

    return (JSON.parse(objJSONStr)).Body;

  };

  RequestRules.replace_TagNode = function (replaceNode) {
    var iterTag = replaceNode.selectNodeIterator(".//Tag");
    var nodeTAGList = "";
    while (iterTag.hasNext()) {
      var nodeTAG = iterTag.next();
      var strTAG = nodeTAG.toString();
      strTAG = strTAG.replace(/"/g, "'");
      nodeTAGList += strTAG;
    }
    return nodeTAGList.toJSON();
  }

  //set parameters value for AuditEvent request
  RequestRules.setValue_AuditEventRequest = function (objContext, objJSONRule) {
    var descriptionNode = objContext.RequestDataObjectArr["Description"];

    var eventuserNode = descriptionNode.selectSingleNode(".//EventUser/User");
    var eventuserJSON = "";
    if (!jsx3.util.strEmpty(eventuserNode)) {
      var eventuserStr = eventuserNode.toString();
      eventuserStr = eventuserStr.replace(/"/g, "'");
      eventuserJSON = eventuserStr.toJSON();
    }

    var eventassetJSON = "";
    var eventssetNode = descriptionNode.selectSingleNode(".//EventAsset/Asset");
    if (!jsx3.util.strEmpty(eventssetNode)) {
      var eventassetStr = eventssetNode.toString();
      eventassetStr = eventassetStr.replace(/"/g, "'");
      eventassetJSON = eventassetStr.toJSON();
    }

    var testStr = "";
    if (!jsx3.util.strEmpty(descriptionNode.getAttribute("text"))) {
      testStr = descriptionNode.getAttribute("text")
    }

    objJSONRule.Body.iq["@from"] = objContext.iq_from;
    objJSONRule.Body.iq["@id"] = objContext.TopicName;
    objJSONRule.Body.iq["@to"] = objContext.iq_to;

    objJSONRule.Body.iq.query.AuditEvent["@type"] = objContext.type;
    objJSONRule.Body.iq.query.AuditEvent["@date"] = objContext.date;
    objJSONRule.Body.iq.query.AuditEvent["@time"] = objContext.time;
    objJSONRule.Body.iq.query.AuditEvent["@level"] = objContext.level;
    objJSONRule.Body.iq.query.AuditEvent["@name"] = objContext.name;

    objJSONRule.Body.iq.query.AuditEvent.Description["@text"] = testStr;
    objJSONRule.Body.iq.query.AuditEvent.Description.EventUser = eventuserJSON;
    objJSONRule.Body.iq.query.AuditEvent.Description.EventAsset = eventassetJSON;

    return objJSONRule.Body;

  };

  //get a specific rule with a specific caller context (which includes request name)
  RequestRules.getRule = function (objContext) {
    var objJSONRule = RequestRules[objContext.RequestName];
    if (!objJSONRule.CallBack.setValueAndAttribute) {
      objJSONRule = eval(objJSONRule.CallBack.setValue)(objContext, objJSONRule);
    }
    else {
      objJSONRule = eval(objJSONRule.CallBack.setValueAndAttribute)(objContext);
    }
    return objJSONRule;
  };

  RequestRules.escapeSpecialChar = function (str) {
    return str.replace(/[<>\&\\]/g, function (c) {
      return '&#' + c.charCodeAt(0) + ';';
    });
  }
});