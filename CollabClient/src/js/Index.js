/* ============================================================================
 * Main.js
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */

jQuery(function () {
    var apiAgent = new BarcoAPI();
    var serviceVer = "2.32.5";
    jQuery.ajax({
        type: "GET",
        url: "service.version",
        dataType: "xml",
        success: function (xml) {
            serviceVer = xml.documentElement.attributes[0].value;
            processMainMenu(serviceVer);
        },
        error: function () {
            //call processMainMenu anyway
            processMainMenu(serviceVer);
        }
    });

    var processMainMenu = function (serviceVer) {
        var MENU_ITEM = { SELECT_MAINMENU: "BCS Client", SELECT_PLAYER: "Media Player", SELECT_CONTROLLER: "Endpoint Controller" };
        //attache the event to navigate to main menu
        jQuery('#div-main-menu-link').on('click', function () {
            //check if it is logged then only show main menu
            if (!jQuery('#logout-submit').hasClass('hidden')) {
                jQuery.publish(MainMenu.EVENT.MSG, MainMenu.EVENT.ON_MSG_SELECT_MAINMENU);
            }
        });
        var onLoginStateChange = function (eventType, dataInfo) {
            //eventType: "onLoginStateChange"
            if (dataInfo.state == "SUCCESS") {
                jQuery('#logout-submit').removeClass('hidden').on('click', function () {
                    loginHandler.edp_login('doLogout');
                });
                loginHandler.edp_login('hide');
                mainMenuHandler.edp_mainMenu('show');
                var authInfo = loginHandler.edp_login('getAuthParams');
                mediaPlayerPanelHandler.setAuthParams({ username: authInfo.user, password: authInfo.password, resource: authInfo.clientId });
                jQuery('#uid').get(0).innerHTML = authInfo.user;
            }
            else if (dataInfo.state == "ERROR") {
                jQuery('#logout-submit').addClass('hidden');
                _showHideMenuLink(MENU_ITEM.SELECT_MAINMENU);
                mainMenuHandler.edp_mainMenu('hide');
            }
            else if (dataInfo.state == "LOGGEDOUT" || dataInfo.state == "KICKEDOUT") {
                jQuery('#logout-submit').addClass('hidden').off('click', function () {
                    loginHandler.edp_login('doLogout');
                });
                _showHideMenuLink(MENU_ITEM.SELECT_MAINMENU);
                jQuery.publish(MainMenu.EVENT.MSG, "NONE");
                loginHandler.edp_login('show');
            }

        };
        var ipAddress = location.host;
        var protocol = location.protocol;
        var userPermissions = jQuery('body').edp_userPermissions({ serviceVersion: serviceVer, barcoAPI: apiAgent, ipAddress: ipAddress, protocol: protocol, callback: onLoginStateChange }).data("edp_userPermissions");
        var loginHandler = $('#login-group').edp_login({ serviceVersion: serviceVer, barcoAPI: apiAgent, userPermissions: userPermissions, ipAddress: ipAddress, protocol: protocol, callback: onLoginStateChange });
        var mainMenuHandler = $('#divMainMenu').edp_mainMenu({});
        var mediaPlayerPanelHandler = $('#divMediaPlayerPanel').edp_mediaPlayerPanel({ barcoAPI: apiAgent, ipAddress: ipAddress, protocol: protocol }).data("edp_mediaPlayerPanel");

        //Display part
        var displayListPanel = $('#divDisplayListPanel').edp_destPanel({ barcoAPI: apiAgent, userPermissions: userPermissions, ipAddress: ipAddress, protocol: protocol }).data('edp_destPanel');
        var windowCtrlHandler = null;

        jQuery.subscribe(MainMenu.EVENT.MSG, function (event, eventData) {
            if (eventData == MainMenu.EVENT.ON_MSG_SELECT_MAINMENU) {
                _showHideMenuLink(MENU_ITEM.SELECT_MAINMENU);
                mediaPlayerPanelHandler.hide();
                displayListPanel.hide();
                mainMenuHandler.edp_mainMenu('show');
            }
            else if (eventData == MainMenu.EVENT.ON_MSG_SELECT_PLAYER) {
                _showHideMenuLink(MENU_ITEM.SELECT_PLAYER);
                mainMenuHandler.edp_mainMenu('hide');
                mediaPlayerPanelHandler.setPanel();
                mediaPlayerPanelHandler.show();
            }
            else if (eventData == MainMenu.EVENT.ON_MSG_SELECT_CONTROLLER) {
                _showHideMenuLink(MENU_ITEM.SELECT_CONTROLLER);
                mainMenuHandler.edp_mainMenu('hide');
                displayListPanel.setPanel();
                displayListPanel.show();
            }
            else if (eventData == MainMenu.EVENT.ON_MSG_SELECT_SESSION) {
                alert('Choosen Session');
            }
            else {
                _showHideMenuLink(MENU_ITEM.SELECT_MAINMENU);
                mediaPlayerPanelHandler.resetPanel();
                mediaPlayerPanelHandler.hide();
                displayListPanel.resetPanel();
                displayListPanel.hide();
                mainMenuHandler.edp_mainMenu('hide');
            }
        });

        jQuery.subscribe('NotificationError', function (event, eventData) {
            loginHandler.edp_login('doLogout');
            $errMsgDialog = jQuery('<div/>').html('Server Connection Error').dialog({
                height: 200,
                width: 320,
                autoOpen: true,
                show: { effect: "blind", duration: 800 }
            });

        });

        function _showHideMenuLink(menuItem) {
            if (menuItem == MENU_ITEM.SELECT_MAINMENU) {
                jQuery('#div-main-menu-link').addClass('hidden');
            } else {
                jQuery('#div-main-menu-link').removeClass('hidden');
            }
            jQuery('#divMenuLable').text(menuItem);
        };
    };
});

