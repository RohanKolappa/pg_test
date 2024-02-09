jQuery(document).ready(function () {
  var self = this;
  var root;
  var requestHeaders = new Object();
  var skinObj = [
    { skinObj: {
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
          playStartIconImage: { height: 24, width: 44, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAYCAYAAACBbx+6AAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAMtJREFUeNpi/P//P8NQAkwMQwyMOnhIOnjR+g0LQHhIOBjq0HgQpoWjmWjkWBiguqOZaOhYmjiaicaOpbqjGSmtOIAOMQBSAUhCCUAsD2U/BGJkh26ICwy4MKAOxuKBA0DKHso9CHSgw2g5POpgGgIWGpgJymQHoOwH1DacokwHzGAN5OgDZsSGgQrheiC+CMQfiFQvAMT6QNwwkGm4AK2sRZGDFmsXoLhgsKThAzjS6wOkdA0L4QF38H4C6XzwZLrRcnjUwYMAAAQYAO0aTGVe2pWNAAAAAElFTkSuQmCC)" }
        }
      }
    }
    },
    { skinObj: {
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
    }
    },
    { skinObj: {
      controlBar: {
        backgroundImage: { width: 1, height: 96, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAABgCAYAAADcvRh2AAAAB3RJTUUH3ggHDAYcAgamKgAAAAlwSFlzAAAewQAAHsEBw2lUUwAAAARnQU1BAACxjwv8YQUAAABsSURBVHjajZFLDoAgDESll2brNTyo21dBvoWauGloXjKdGY543aeoashD0iAIjLW8ZqqZslLMSqJUsEsVilUetEnh0fcQbZ1t9EMYzywOflHpeuYGTlQdUY37JRu1Or9Tei+fnTJlw/8edvoArQsfhtQdFSwAAAAASUVORK5CYII=)" },
        trimEditBar: {
          trimBarInProgressColor: "cyan",
          trimBarAddColor: "yellow",
          trimBarDeleteColor: "red",
          fontColor: "rgb(192,192,255)",
          backgroundColor: "blue"
        },
        buttonBar: {
          playStartIconImage: { height: 24, width: 44, image: "url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAYCAYAAACBbx+6AAAAB3RJTUUH3ggHCi41GWTsXgAAAAlwSFlzAAAewgAAHsIBbtB1PgAAAARnQU1BAACxjwv8YQUAAALnSURBVHjaxZfPaxNBFMfftjY/qk1SaiWVRBFLa1sViiJaFVEvHiSetNiigodSQYQepBeRgnhQxIMn8eZFBG/eK3jwxx+ggngopbWJTaNNo00aTcb3ku/QpFG6k2zogy+7k8zufua7s/PeWGQQ1ivVyAeRC5LzBlaelYOy+lydsnIm97fFUAWsl+VhbQZ0EwAFOsNKs34DPOs0+CaDvhpWQANQM8uN/wVymfWT9QsqDIAH65jjthyGu+Kmj9XGamcFWX5WC+6TBfAi6zu0VAJecLxW6AYDd11wOADYsDrjfaSeTpzl817WPhxFXaxO6cPaxmplbZHrefAuGFBXh11wU5zdCZhd7NZYoUPXgaR1+fY0DUSicHiB9Y0l7Xm0Sx2v2m27wOKsTIcQYLtF/MChso4nzietq3dmKNw9D9AowGdxXMCABDxdDbTdKSEhr1E+Ui/cDlT0eP3Cr64d6lNPxvsxMJkee1g9aO/AW/LhPsbTw2QOi2QJk+mhl7XKWF6y6Pn9VjUY7qfJZ338y27AylGmUwcVP1y9LNYFuBRaXHbT6nL274jPNqq7wyF1/chB+vCmB7DyEQYB3KKBTVxeF3jNzTSwhl8/Pr33qhvH9qtb545TIrodsD447Kmnw2uvM7v27cugutR5QT0cOUnFhNNkNPAagTcsTFJzaeQh+zEQiVljjyepreMjFTOipHJdMDkHLOskz2PdlJv/MXpQ7+G0NfrgC+09+plbc6wEladsZ4FLQkMK8Ar0/2gP5ayRe1E6PTQF0BkoBugUVVHN2QXWsLpszMChymj2KYqMLlqDN6fJv1Wn569UzHZzaCdoNUXX1WFxV1JqkooptjzKU3McbkbJwdRs4nAeD5GH/QBAMSqLnwT+j0GOFT8mDut6V9wVB93k8qzQxfF31pWJKQwqBWBdocUBmgJommrchdSzgE/iTThawJvs6XQBb7xFIgc3paYfXbrkPEMbsAm17TBc3vBt/l9icEPIWoIv2gAAAABJRU5ErkJggg==)" }
        }
      }
    }
    }
  ];
  var skinIdx = -1;
  var skinName = "Default";
  var confObj = {
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
  var barcoplayerDiv = jQuery('div.barcoplayer');
  root = barcoplayerDiv.player(confObj);
  jQuery('#skin0').on("click", function () {
    highlightSkinButton(0);
  });
  jQuery('#skin1').on("click", function () {
    highlightSkinButton(1);
  });
  jQuery('#skin2').on("click", function () {
    highlightSkinButton(2);
  });
  jQuery('#set').on("click", function () {
    var inputs = jQuery('#rscform :input'),
      values = {};

    inputs.each(function () {
      values[this.name] = jQuery(this).val();
    });
    var lcControlURL = values['URL'];

    if (!lcControlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl').focus();
      return;
    }
    ;
    //set player's controlURL where get from getFile or getPort response
    jQuery.extend(confObj, { controlURL: lcControlURL });
    jQuery('#mediacontrol').show();
    jQuery('#additionalcontrol').show();
    jQuery('#msgboard').text('');
    jQuery('#msgboard').show();
    setLeftHeaderTitle();
    if (root != null) {
      root.player('conf', { playerURL: lcControlURL,
        customSkin: (skinIdx >= 0 ? skinObj[skinIdx] : null)
      });
    }
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
  jQuery.subscribe("onAuthorizing", displayEventMsg);
  jQuery.subscribe("onError", displayEventMsg);

  function displayEventMsg(obj1, obj2) {
    setTimeout(function () {
      jQuery('#msgboard').html('type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
    }, 0);
  }
  ;

  //~~~~~~~~~~~~~~~~~ Basic functions call ~~~~~~~~~~~~~~~~
  jQuery('#authenticate').on('click', function (evt) {
    var username = jQuery('#username').val();
    var password = jQuery('#password').val();
    var resource = jQuery('#resource').val();
    var valid = username.length > 0 &&
      password.length > 0 &&
      resource.length > 0;
    evt.target.innerHTML = "Set Credentials";
    if (!valid) {
      alert('Please input all 3 fields if you want it to do video editing. Otherwise, editing will not be available.');
      return;
    }
    evt.target.innerHTML = "Credentials Entered";
    root.player('authenticate', {
      username: username,
      password: password,
      resource: resource
    });
  });

  jQuery('#play').bind("click", function () {
    //customize configuration with a controlURL and play
    if (!confObj.controlURL) {
      alert('Please input a media URL.');
      jQuery('#controlurl').focus();
      return;
    }
    if (root.player('isPlaying')) {
      root.player('stop');
    }
    setLeftHeaderTitle();
    root.player('play', confObj.controlURL);

    //init clear message 
    jQuery('#msglabel').text('');
    jQuery('#msglabel').show();
    jQuery('#msgboard').text('');
    jQuery('#msgboard').show();
    jQuery('#additionalcontrol').show();
  });
  jQuery('#stop').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('stop');
    }
    jQuery('#msglabel').text('');
    jQuery('#msglabel').hide();
    jQuery('#msgboard').text('');
    jQuery('#msgboard').hide();
    jQuery('#additionalcontrol').hide();
  });
  jQuery('#skipforward').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('skipforward');
    }
  });
  jQuery('#skipbackward').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('skipbackward');
    }
  });
  jQuery('#pause').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('pause');
    }
  });
  jQuery('#resume').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('resume');
    }
  });
  jQuery('#catchup').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('catchup');
    }
  });
  //  jQuery('#fullscreen').bind("click", function() {
  //    root.player('fullscreen');
  //  });
  jQuery('#mute').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('mute');
    }
  });
  jQuery('#unmute').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('unmute');
    }
  });
  jQuery('#zoom25').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('zoom', '25%');
    }
  });
  jQuery('#zoom50').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('zoom', '50%');
    }
  });
  jQuery('#zoom100').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('zoom', '100%');
    }
  });
  jQuery('#zoom200').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('zoom', '200%');
    }
  });
  jQuery('#zoomTo').bind("click", function () {
    if (root.player('isPlaying')) {
      var toZoom = jQuery('#txtZoomTo').val();
      toZoom = jQuery.trim(toZoom);
      if (!toZoom) {
        alert("Enter zoomto value");
        jQuery('#txtZoomTo').focus();
        return;
      }
      root.player('zoom', toZoom);
    }
  });
  jQuery('#fitToWin').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('zoom', 'Fit');
    }
  });
  jQuery('#enableKBM').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('setLocalKBMState', true);
    }
  });
  jQuery('#disableKBM').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('setLocalKBMState', false);
    }
  });
  jQuery('#toggleScreen').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('toggleScreen');
    }
  });
  jQuery('#snapShot').bind("click", function () {
    if (root.player('isPlaying')) {
      root.player('snapShot');
    }
  });
  jQuery('#seekto').bind("click", function () {
    if (root.player('isPlaying')) {
      var toOffset = jQuery('#txtseektoOffset').val();
      toOffset = jQuery.trim(toOffset);
      if (!toOffset) {
        alert("Enter seekto offset");
        jQuery('#txtseektoOffset').focus();
        return;
      }
      root.player('seekTo', toOffset);
    }
  });


  //~~~~~~~~~~~~~~~~~~ Additional functions call ~~~~~~~~~~~~~~~

  jQuery('#getPlayerVersion').bind("click", function () {
    var version = root.player('getVersion');
    jQuery('#msglabel').text('Player version = ' + version);
  });

  jQuery('#getMediaURL').bind("click", function () {
    var mediaURL = root.player('getMediaURL');
    jQuery('#msglabel').text('mediaURL = ' + mediaURL);
  });
  jQuery('#isPlaying').bind("click", function () {
    var isPlaying = root.player('isPlaying');
    jQuery('#msglabel').text('isPlaying = ' + isPlaying);
  });
  jQuery('#isPaused').bind("click", function () {
    var isPaused = root.player('isPaused');
    jQuery('#msglabel').text('isPaused = ' + isPaused);
  });

});
