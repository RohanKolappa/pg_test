jQuery(document).ready(function() {
  var self = this;
  var root1;
  var root2;
  var requestHeaders = new Object();
  var skinObj = [
    {skinObj: {
        global: {
          fontColor: "rgb(255,192,192)",
          lineColor: "#fcc",
          lineWidth: "1px"
        },
        controlBar: {
          backgroundColor: "darkred",
          scubberBar: {
            borderColor: "white",
            borderWidth: 2,
            progressBarFillColor: "red",
            backgroundColor: "black"
          },
          scubberHandle: {
            fillColor: "darkred"
          },
          bookmarkPanel: {
            backgroundColor: "red",
            inputBackgroundColor: "darkred",
            inputTextColor: "yellow"
          },
          buttonBar: {
            playStartIconImage: {height: 24, width: 44, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAYCAYAAACBbx+6AAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAMtJREFUeNpi/P//P8NQAkwMQwyMOnhIOnjR+g0LQHhIOBjq0HgQpoWjmWjkWBiguqOZaOhYmjiaicaOpbqjGSmtOIAOMQBSAUhCCUAsD2U/BGJkh26ICwy4MKAOxuKBA0DKHso9CHSgw2g5POpgGgIWGpgJymQHoOwH1DacokwHzGAN5OgDZsSGgQrheiC+CMQfiFQvAMT6QNwwkGm4AK2sRZGDFmsXoLhgsKThAzjS6wOkdA0L4QF38H4C6XzwZLrRcnjUwYMAAAQYAO0aTGVe2pWNAAAAAElFTkSuQmCC)"}
          }
        }
      }},
    {skinObj: {
        global: {
          fontColor: "rgb(128,255,128)"
        },
        controlBar: {
          backgroundColor: "#040",
          scubberBar: {
            progressBarFillColor: "green"
          },
          trimEditBar: {
            backgroundColor: "#060"
          },
          zoomPanel: {
            backgroundColor: '#040',
            borderColor: "rgb(128,255,128)",
            textColor: "rgb(128,255,128)",
            textColorHover: "yellow",
            selectedColor: "blue",
            notchOffset: "50px"
          }
        }
      }},
    {skinObj: {
        controlBar: {
          backgroundImage: {width: 1, height: 96, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAABgCAYAAADcvRh2AAAAB3RJTUUH3ggHDAYcAgamKgAAAAlwSFlzAAAewQAAHsEBw2lUUwAAAARnQU1BAACxjwv8YQUAAABsSURBVHjajZFLDoAgDESll2brNTyo21dBvoWauGloXjKdGY543aeoashD0iAIjLW8ZqqZslLMSqJUsEsVilUetEnh0fcQbZ1t9EMYzywOflHpeuYGTlQdUY37JRu1Or9Tei+fnTJlw/8edvoArQsfhtQdFSwAAAAASUVORK5CYII=)"},
          trimEditBar: {
            trimBarInProgressColor: "cyan",
            trimBarAddColor: "yellow",
            trimBarDeleteColor: "red",
            fontColor: "rgb(192,192,255)",
            backgroundColor: "blue"
          },
          buttonBar: {
            playStartIconImage: {height: 24, width: 44, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAYCAYAAACBbx+6AAAAB3RJTUUH3ggHCi41GWTsXgAAAAlwSFlzAAAewgAAHsIBbtB1PgAAAARnQU1BAACxjwv8YQUAAALnSURBVHjaxZfPaxNBFMfftjY/qk1SaiWVRBFLa1sViiJaFVEvHiSetNiigodSQYQepBeRgnhQxIMn8eZFBG/eK3jwxx+ggngopbWJTaNNo00aTcb3ku/QpFG6k2zogy+7k8zufua7s/PeWGQQ1ivVyAeRC5LzBlaelYOy+lydsnIm97fFUAWsl+VhbQZ0EwAFOsNKs34DPOs0+CaDvhpWQANQM8uN/wVymfWT9QsqDIAH65jjthyGu+Kmj9XGamcFWX5WC+6TBfAi6zu0VAJecLxW6AYDd11wOADYsDrjfaSeTpzl817WPhxFXaxO6cPaxmplbZHrefAuGFBXh11wU5zdCZhd7NZYoUPXgaR1+fY0DUSicHiB9Y0l7Xm0Sx2v2m27wOKsTIcQYLtF/MChso4nzietq3dmKNw9D9AowGdxXMCABDxdDbTdKSEhr1E+Ui/cDlT0eP3Cr64d6lNPxvsxMJkee1g9aO/AW/LhPsbTw2QOi2QJk+mhl7XKWF6y6Pn9VjUY7qfJZ338y27AylGmUwcVP1y9LNYFuBRaXHbT6nL274jPNqq7wyF1/chB+vCmB7DyEQYB3KKBTVxeF3jNzTSwhl8/Pr33qhvH9qtb545TIrodsD447Kmnw2uvM7v27cugutR5QT0cOUnFhNNkNPAagTcsTFJzaeQh+zEQiVljjyepreMjFTOipHJdMDkHLOskz2PdlJv/MXpQ7+G0NfrgC+09+plbc6wEladsZ4FLQkMK8Ar0/2gP5ayRe1E6PTQF0BkoBugUVVHN2QXWsLpszMChymj2KYqMLlqDN6fJv1Wn569UzHZzaCdoNUXX1WFxV1JqkooptjzKU3McbkbJwdRs4nAeD5GH/QBAMSqLnwT+j0GOFT8mDut6V9wVB93k8qzQxfF31pWJKQwqBWBdocUBmgJommrchdSzgE/iTThawJvs6XQBb7xFIgc3paYfXbrkPEMbsAm17TBc3vBt/l9icEPIWoIv2gAAAABJRU5ErkJggg==)"}
          }
        }
      }}
  ];
  var skinIdx = -1;
  var skinName = "Default";
  var confObj1 = {
    playerPref: 'HLS',
    showControlBar: true,
    showRecordingControls: false,
    showBookmarkCreation: true,
    showBookmarkPlayback: true,
    controllerBarFixed: false,
    customSkin: null
  };
  var confObj2 = {
    playerPref: 'HLS',
    showControlBar: true,
    showRecordingControls: false,
    showBookmarkCreation: true,
    showBookmarkPlayback: true,
    controllerBarFixed: false,
    customSkin: null
  };
  function highlightSkinButton(selIdx) {
    skinIdx = selIdx;
    var $elemSel = jQuery('#skin' + selIdx.toString());
    skinName = $elemSel.html();
    for (var i = 0; i < skinObj.length; i++) {
      if (i === selIdx) {
        $elemSel.addClass("skinButtonSelected");
      } else {
        jQuery('#skin' + i.toString()).removeClass("skinButtonSelected");
      }
    }
  }
  function setLeftHeaderTitle() {
    jQuery("div.skinTitle").html("Control Bar Skin: " + skinName);
    jQuery("div.skinButtons").hide();
  }
  var barcoplayerDiv1 = jQuery('div#p1');
  root1 = barcoplayerDiv1.player(confObj1);
  var barcoplayerDiv2 = jQuery('div#p2');
  root2 = barcoplayerDiv2.player(confObj2);
  jQuery('#skin0').on("click", function() {
    highlightSkinButton(0);
  });
  jQuery('#skin1').on("click", function() {
    highlightSkinButton(1);
  });
  jQuery('#skin2').on("click", function() {
    highlightSkinButton(2);
  });
/* =============================================================================
 * First Player
 * ========================================================================== */
  jQuery('#set1').on("click", function() {
    var inputs = jQuery('#rscform1 :input'),
      values = {};

    inputs.each(function() {
      values[this.name] = jQuery(this).val();
    });
    var lcControlURL = values['URL'];

    if (!lcControlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl1').focus();
      return;
    }
    ;
    //set player's controlURL where get from getFile or getPort response
    jQuery.extend(confObj1, {controlURL: lcControlURL});
    jQuery('#mediacontrol1').show();
    jQuery('#additionalcontrol1').show();
    jQuery('#msgboard1').text('');
    jQuery('#msgboard1').show();
    setLeftHeaderTitle();
    if (root1 != null) {
      root1.player('conf', {playerURL: lcControlURL,
        customSkin: (skinIdx >= 0 ? skinObj[skinIdx] : null)});
    }
  });

  //~~~~~~~~~~~~~~~~~ Basic functions call ~~~~~~~~~~~~~~~~
  jQuery('#authenticate1').on('click', function(evt) {
    var username = jQuery('#username1').val();
    var password = jQuery('#password1').val();
    var resource = jQuery('#resource1').val();
    var valid = username.length > 0 &&
      password.length > 0 &&
      resource.length > 0;
    evt.target.innerHTML = "Set Credentials";
    if (!valid) {
      alert('Please input all 3 fields if you want it to do video editing. Otherwise, editing will not be available.');
      return;
    }
    evt.target.innerHTML = "Credentials Entered";
    root1.player('authenticate', {
      username: username,
      password: password,
      resource: resource
    });
  });

  jQuery('#play1').bind("click", function() {
    //customize configuration with a controlURL and play
    if (!confObj1.controlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl1').focus();
      return;
    }
    if (root1.player('isPlaying')) {
      root1.player('stop');
    }
    setLeftHeaderTitle();
    root1.player('play', confObj1.controlURL);

    //init clear message 
    jQuery('#msglabel1').text('');
    jQuery('#msglabel1').show();
    jQuery('#msgboard1').text('');
    jQuery('#msgboard1').show();
    jQuery('#additionalcontrol1').show();
  });
  jQuery('#stop1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('stop');
    }
    jQuery('#msglabel1').text('');
    jQuery('#msglabel1').hide();
    jQuery('#msgboard1').text('');
    jQuery('#msgboard1').hide();
    jQuery('#additionalcontrol1').hide();
  });
  jQuery('#skipforward1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('skipforward');
    }
  });
  jQuery('#skipbackward1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('skipbackward');
    }
  });
  jQuery('#pause1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('pause');
    }
  });
  jQuery('#resume1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('resume');
    }
  });
  jQuery('#catchup1').bind("click", function() {
    if (root1.player('isPlaying')) {
      root1.player('catchup');
    }
  });
  jQuery('#fullscreen1').bind("click", function() {
    root1.player('fullscreen');
  });
  jQuery('#mute1').bind("click", function() {
    root1.player('mute');
  });
  jQuery('#unmute1').bind("click", function() {
    root1.player('unmute');
  });
  jQuery('#seekto1').bind("click", function() {
    if (root1.player('isPlaying')) {
      var toOffset = jQuery('#txtseektoOffset1').val();
      toOffset = jQuery.trim(toOffset);
      if (!toOffset) {
        alert("Enter seekto offset");
        jQuery('#txtseektoOffset1').focus();
        return;
      }
      root1.player('seekTo', toOffset);
    }
  });


  //~~~~~~~~~~~~~~~~~~ Additional functions call ~~~~~~~~~~~~~~~

  jQuery('#getPlayerVersion1').bind("click", function() {
    var version = root1.player('getVersion');
    jQuery('#msglabel1').text('Player version = ' + version);
  });

  jQuery('#getMediaURL1').bind("click", function() {
    var mediaURL = root1.player('getMediaURL');
    jQuery('#msglabel1').text('mediaURL = ' + mediaURL);
  });
  jQuery('#isPlaying1').bind("click", function() {
    var isPlaying = root1.player('isPlaying');
    jQuery('#msglabel1').text('isPlaying = ' + isPlaying);
  });
  jQuery('#isPaused1').bind("click", function() {
    var isPaused = root1.player('isPaused');
    jQuery('#msglabel1').text('isPaused = ' + isPaused);
  });
/* =============================================================================
 * Second Player
 * ========================================================================== */
  jQuery('#set2').on("click", function() {
    var inputs = jQuery('#rscform2 :input'),
      values = {};

    inputs.each(function() {
      values[this.name] = jQuery(this).val();
    });
    var lcControlURL = values['URL'];

    if (!lcControlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl2').focus();
      return;
    }
    ;
    //set player's controlURL where get from getFile or getPort response
    jQuery.extend(confObj2, {controlURL: lcControlURL});
    jQuery('#mediacontrol2').show();
    jQuery('#additionalcontrol2').show();
    jQuery('#msgboard2').text('');
    jQuery('#msgboard2').show();
    setLeftHeaderTitle();
    if (root2 != null) {
      root2.player('conf', {playerURL: lcControlURL,
        customSkin: (skinIdx >= 0 ? skinObj[skinIdx] : null)});
    }
  });

  //~~~~~~~~~~~~~~~~~ Basic functions call ~~~~~~~~~~~~~~~~
  jQuery('#authenticate2').on('click', function(evt) {
    var username = jQuery('#username2').val();
    var password = jQuery('#password2').val();
    var resource = jQuery('#resource2').val();
    var valid = username.length > 0 &&
      password.length > 0 &&
      resource.length > 0;
    evt.target.innerHTML = "Set Credentials";
    if (!valid) {
      alert('Please input all 3 fields if you want it to do video editing. Otherwise, editing will not be available.');
      return;
    }
    evt.target.innerHTML = "Credentials Entered";
    root2.player('authenticate', {
      username: username,
      password: password,
      resource: resource
    });
  });

  jQuery('#play2').bind("click", function() {
    //customize configuration with a controlURL and play
    if (!confObj2.controlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl2').focus();
      return;
    }
    if (root2.player('isPlaying')) {
      root2.player('stop');
    }
    setLeftHeaderTitle();
    root2.player('play', confObj2.controlURL);

    //init clear message 
    jQuery('#msglabel2').text('');
    jQuery('#msglabel2').show();
    jQuery('#msgboard2').text('');
    jQuery('#msgboard2').show();
    jQuery('#additionalcontrol2').show();
  });
  jQuery('#stop2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('stop');
    }
    jQuery('#msglabel2').text('');
    jQuery('#msglabel2').hide();
    jQuery('#msgboard2').text('');
    jQuery('#msgboard2').hide();
    jQuery('#additionalcontrol2').hide();
  });
  jQuery('#skipforward2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('skipforward');
    }
  });
  jQuery('#skipbackward2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('skipbackward');
    }
  });
  jQuery('#pause2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('pause');
    }
  });
  jQuery('#resume2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('resume');
    }
  });
  jQuery('#catchup2').bind("click", function() {
    if (root2.player('isPlaying')) {
      root2.player('catchup');
    }
  });
  jQuery('#fullscreen2').bind("click", function() {
    root2.player('fullscreen');
  });
  jQuery('#mute2').bind("click", function() {
    root2.player('mute');
  });
  jQuery('#unmute2').bind("click", function() {
    root2.player('unmute');
  });
  jQuery('#seekto2').bind("click", function() {
    if (root2.player('isPlaying')) {
      var toOffset = jQuery('#txtseektoOffset2').val();
      toOffset = jQuery.trim(toOffset);
      if (!toOffset) {
        alert("Enter seekto offset");
        jQuery('#txtseektoOffset2').focus();
        return;
      }
      root2.player('seekTo', toOffset);
    }
  });

  //~~~~~~~~~~~~~~~~~~ Additional functions call ~~~~~~~~~~~~~~~

  jQuery('#getPlayerVersion2').bind("click", function() {
    var version = root2.player('getVersion');
    jQuery('#msglabel2').text('Player version = ' + version);
  });

  jQuery('#getMediaURL2').bind("click", function() {
    var mediaURL = root2.player('getMediaURL');
    jQuery('#msglabel2').text('mediaURL = ' + mediaURL);
  });
  jQuery('#isPlaying2').bind("click", function() {
    var isPlaying = root2.player('isPlaying');
    jQuery('#msglabel2').text('isPlaying = ' + isPlaying);
  });
  jQuery('#isPaused2').bind("click", function() {
    var isPaused = root2.player('isPaused');
    jQuery('#msglabel2').text('isPaused = ' + isPaused);
  });


  //~~~~~~~~~~~~~~~~ Events subscription ~~~~~~~~~~~~~~~~~~
  jQuery.subscribe("onStart", displayEventMsg);
  jQuery.subscribe("onStop", displayEventMsg);
  jQuery.subscribe("onMute", displayEventMsg);
  jQuery.subscribe("onUnmute", displayEventMsg);
  jQuery.subscribe("onPause", displayEventMsg);
  jQuery.subscribe("onResume", displayEventMsg);
  jQuery.subscribe("onCatchup", displayEventMsg);
  jQuery.subscribe("onSeek", displayEventMsg);
  jQuery.subscribe("onFullscreen", displayEventMsg);
  jQuery.subscribe("onError", displayEventMsg);

  function displayEventMsg(obj1, obj2) {
    setTimeout(function() {
      if (obj2.id === "p1") {
      jQuery('#msgboard1').html('type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
    } else {
      jQuery('#msgboard2').html('type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
    }
    }, 0);
  }
  ;
});

