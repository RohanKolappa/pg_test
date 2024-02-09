function cmpBlkMultiPlayerUI_Create(lytHeaderBlkJSX, eventMultiPly)
{
  // get vars from object instance
  var winMultiPly = this;
  var winMultiPlyBlkJSX = this.BlkJSX;
  var tMultiPly = this.tWin;

  //index number for child blocks
  winMultiPly.cntAdoptedBlks = 0;
  // array of video blocks - the subcmparr of this container is filled dynamically in response to "Add" actions
  var arrVideoBlockList = new Object;
  //set current palyer instance, for SINGLE mode
  this.playerInstance = null; 
  //define update view type  
  winMultiPly.updateViewType = "NONE";

  //init layout maxviewtoggle selection
  var playerLayoutMode = "GRID";    //SINGLE,GRID;HSTRIP;VSTRIP;CANVAS
  topic_Set(tMultiPly, "MediaPlayerLayoutMode", playerLayoutMode); 
  var mediaPlayerLayoutMode = topic_Get(tMultiPly, "MediaPlayerLayoutMode");

  topic_Set(tMultiPly, "UpdateFps", null);
  topic_Set(tMultiPly, "FullScreenOff", "NONE");

  //init some global variable,it can move to somewhere else
  //the real time fps will be divided by a number of players refer to adjustFPS(fps,true)
  var fpsMin = 0;

  /* ------------------------ blkLayoutMenuContainer including btns and a FPS Slider ------- */
  var cmpBtnRadioGridLayout = new CmpRadioButton([winMultiPlyBlkJSX, "radioGridLayout"], null,
                                                     { action: "FNCCALL",
                                                       context: null,
                                                       fnc: function()
                                                       {
                                                         var ttt = "tt";
                                                         topic_Publish(tMultiPly, "MediaPlayerLayoutMode", "GRID"); //GRID;HSTRIP;VSTRIP
                                                       }
                                                   });
                                                  

  var cmpBtnRadioHorzStripLayout = new CmpRadioButton([winMultiPlyBlkJSX, "radioHorzStripLayout"], null,
                                                      { action: "FNCCALL",
                                                       context: null,
                                                       fnc: function()
                                                       {
                                                         var ttt = "tt";
                                                         topic_Publish(tMultiPly, "MediaPlayerLayoutMode", "HSTRIP");
                                                       }
                                                      });

                                                   

  var cmpBtnRadioVertStripLayout = new CmpRadioButton([winMultiPlyBlkJSX, "radioVertStripLayout"], null,
                                                       { action: "FNCCALL",
                                                         context: null,
                                                         fnc: function()
                                                         {
                                                           var ttt = "tt";
                                                           topic_Publish(tMultiPly, "MediaPlayerLayoutMode", "VSTRIP");
                                                         }
                                                       });


  var cmpBlkLayoutMenu = new CmpContainer([winMultiPlyBlkJSX, "blkLayoutMenu"],
                                                 [{ cmp: cmpBtnRadioGridLayout },
                                                  { cmp: cmpBtnRadioHorzStripLayout },
                                                  { cmp: cmpBtnRadioVertStripLayout }],
                                                 null,
                                                 null
                                              );

  var cmpBlkLayoutMenuContainer = new CmpContainer([winMultiPlyBlkJSX, "blkLayoutMenuContainer"],
                                                 [{ cmp: cmpBlkLayoutMenu }],
                                                 null,
                                                 null);

  // container blocks for video blocks

  /* --------------- simple grid ---------------*/
  var cmpLytVideoBlocksGrid = new CmpBlock([winMultiPlyBlkJSX, "lytVideoBlocksContainerGrid"]);

  var addressTable = new Object();

  //----------------------------------------------------------------------------------------
  // listen to drop fps for all
  eventMultiPly.subscribe("DropFPS", null, this,
                            function(EventVar, EventVal) {
                              log.info(" cmpBlkMultiPlayerUI:: Dropping fps to " + fpsMin);
                              cmpCtrVideoBlockList.adjustFPS(fpsMin, false);
                            });
 
  /* ------------------------ paint GRID layout ---------------------*/
  cmpLytVideoBlocksGrid.repaint = function() {
    // go thru the array of blocks
    var cntBlks = 0;
    var adoptFlag = false;
    // drop the fps for all players, but avoid to repaint the slider pointer - bug 26585
    cmpCtrVideoBlockList.adjustFPS(fpsMin,false);
    var deleteInstace = null;

    for (var instanceId in arrVideoBlockList) {
      //CASE_1:Add_Player and repaint Grid view
      if (winMultiPly.updateViewType == "Add_Player") {
        cntBlks++;
        // get current rows and columns
        var nRows = this.BlkJSX.getRows().split(",").length;
        var nCols = this.BlkJSX.getCols().split(",").length;
        //create and keep a windowing player header bind to arrVideoBlockList[instanceId], if it isn't exist
        (!arrVideoBlockList[instanceId].windowPlayerHeader) && (arrVideoBlockList[instanceId].windowPlayerHeader = new com.ipvs.gui.WindowingPlayerHeader(instanceId, tMultiPly));

        // if all spots in grid are taken
        if (cntBlks > (nRows * nCols)) {
          if (nRows < nCols) {
            // add to rows
            this.BlkJSX.setRows(this.BlkJSX.getRows() + ',*', true);
            //set rows for windowing playe header
            arrVideoBlockList[instanceId].windowPlayerHeader.setRows(this.BlkJSX.getRows(), true);
          }
          else {
            // add to cols
            this.BlkJSX.setCols(this.BlkJSX.getCols() + ',*', true);
            //set cols for windowing playe header 
            arrVideoBlockList[instanceId].windowPlayerHeader.setCols(this.BlkJSX.getCols(), true);
          }
        }

        var objPosition = arrVideoBlockList[instanceId].position;
       
        // [2] add a new player to gird
        // if not in the grid - add a spot for it in the grid
        if (objPosition.ctr != "GRID") {
          // add a new player into the grid
          var cntBlkJSX = this.BlkJSX.getChild(winMultiPly.cntAdoptedBlks);
          cmpLytVideoBlocksGrid.removeVideoBlockByJSX(cntBlkJSX);
          // adopt the block; 'true'- repaint and resize
          cntBlkJSX.adoptChild(arrVideoBlockList[instanceId].BlkJSX, true, true);
          winMultiPly.cntAdoptedBlks++;
          //keep order of  current player in position
          arrVideoBlockList[instanceId].position.order = winMultiPly.cntAdoptedBlks;
          //reisze for the last player of a group,if only one no need resize ifself
//          jsx3.sleep(function()
//          {
//            this.resize();
//          }, null, arrVideoBlockList[instanceId].cmpPlayer);
          
          // mark it as part of the grid
          arrVideoBlockList[instanceId].position.ctr = "GRID";

          //add a windowing header block in layout windowing selector
          arrVideoBlockList[instanceId].windowPlayerHeader.addBlkWinToLytWinSeletor(winMultiPly.cntAdoptedBlks, instanceId, arrVideoBlockList); 
          
        }

        //calculate each position row and col based on its order and row/col number
        if (arrVideoBlockList[instanceId].cmpPlayer.displaymode.maxviewtoggle == 1) {
          var rowStrArr = new Array();
          var rowStrArr = this.BlkJSX.getRows().split(",");
          var colStrArr = new Array();
          var colStrArr = this.BlkJSX.getCols().split(",");
          // maxview=1, fullscreen the  grid
          // for this row/col change the '*' to '100%' in the grid contiainer Blkjsx
          var rowIndex = arrVideoBlockList[instanceId].position.row;
          var colIndex = arrVideoBlockList[instanceId].position.col;
          if (arrVideoBlockList[instanceId].cmpPlayer.displaymode.maxview == 1) {
            rowStrArr[rowIndex - 1] = '100%';
            colStrArr[colIndex - 1] = '100%';
            arrVideoBlockList[instanceId].cmpPlayer.setAudioFps = "DOWN";
            //bug 30284 RTP,MPEG overlay over V2D when switch from 1:1 to Fullscreen, setZIndex don't work
            //arrVideoBlockList[instanceId].initZIndex = arrVideoBlockList[instanceId].BlkJSX.getZIndex();
            //arrVideoBlockList[instanceId].BlkJSX.setZIndex(com.ipvs.agent.DisplayAgent.MAX_ZINDEX, true);
            for (var lcID in arrVideoBlockList) {
              //hide all none full screen blocks while go to full screen;call only once a fullscreen go
              if (lcID != instanceId) {
                arrVideoBlockList[lcID].BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
              }
            }
          } else {
            //  maxviewtoggle=1,toggle button click again,restore the grid to reglar size
            // for this row/col change the '*' to '100%' in the grid contiainer Blkjsx
            rowStrArr[rowIndex - 1] = '*';
            colStrArr[colIndex - 1] = '*';
            arrVideoBlockList[instanceId].cmpPlayer.setAudioFps = "RESTORE";
            //bug 30284 RTP,MPEG overlay over V2D when switch from 1:1 to Fullscreen
            //if (arrVideoBlockList[instanceId].BlkJSX.getZIndex() == com.ipvs.agent.DisplayAgent.MAX_ZINDEX) {
              //arrVideoBlockList[instanceId].BlkJSX.setZIndex(arrVideoBlockList[instanceId].cmpPlayer.initZIndex, true);
            //}
            for (var lcID in arrVideoBlockList) {
              //display all none full screen blocks while back from full screen;call only once a fullscreen back
              if (lcID != instanceId) {
                arrVideoBlockList[lcID].BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
              }
            }
          }
          arrVideoBlockList[instanceId].cmpPlayer.displaymode.maxviewtoggle = 0;
          //adjust the layout grid for all players
          this.BlkJSX.setRows(rowStrArr.toString(), true);
          this.BlkJSX.setCols(colStrArr.toString(), true);

          //update layout windowing selector when expand/unexpand a window
          arrVideoBlockList[instanceId].windowPlayerHeader.updateLytWinSeletor(rowStrArr.toString(), colStrArr.toString()); 
        }
         
      }//CASE_1
   
      //CASE_2:Remove_Player repaint Grid view after deleting a player either from API or UI
      if (winMultiPly.updateViewType == "Remove_Player" 
            && arrVideoBlockList[instanceId].position.flag=="DELETE") {
        //re-arrange multiplayer view after deleting players
        cntBlks = getAssociativeArrayLength(arrVideoBlockList);
        // get current rows and columns
        var rowStr = this.BlkJSX.getRows();
        var colStr = this.BlkJSX.getCols();
        var nRows = rowStr.split(",").length;
        var nCols = colStr.split(",").length;


        //[1].rows=clos::one step moving before shrink a row;
        if (nRows == nCols){
         if( ((cntBlks - 1) && (cntBlks - 1) <= nCols * (nCols - 1))) {
            //Deleted is not the last,swap video blocks
            deleteInstace = cmpLytVideoBlocksGrid.swapVideo(arrVideoBlockList, instanceId);
            if ((cntBlks-1) == nCols * (nCols - 1)) {
              //row=row -1
              var removeOneRowStr = rowStr.substring(0, rowStr.lastIndexOf(','));
              this.BlkJSX.setRows(removeOneRowStr, true);

              //update player windowing header selector layout
              arrVideoBlockList[instanceId].windowPlayerHeader.updateLytWinSeletor(removeOneRowStr, null);
            }

            //Deleted is the last, just remove it
            var lastElement = cmpCtrVideoBlockList.SubCmpArr[cntBlks-1];
            cmpCtrVideoBlockList.SubCmpArr.remove(lastElement);
          }
          else {
            //No row shrink but need adjust positions
            //If delete the last do nothing otherwise moving the last to deleted position
            if (arrVideoBlockList[instanceId].position.order < nRows * nRows) {
              //swap video blocks
              deleteInstace = cmpLytVideoBlocksGrid.swapVideo(arrVideoBlockList, instanceId);
            }
            else {
              deleteInstace = instanceId;
            }
            //Deleted is the last, just remove it
            var lastElement = cmpCtrVideoBlockList.SubCmpArr[cntBlks - 1];
            //bug #38499 no row reduce after deleting, then set the last player un-clickable
            //lastElement.cmp.BlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN, true);
            lastElement.cmp.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
            cmpCtrVideoBlockList.SubCmpArr.remove(lastElement); 
          }
        }
        //[2].rows<clos::one step moving after shrink a col;
        //In fact,algo. of shrink col based on 'oder' instead of 'col' & auto. adjust positions
        if (nRows < nCols){
          if((cntBlks-1) == nRows * nRows) {
            var orderDelete = arrVideoBlockList[instanceId].position.order;
            var maxNewOrder = nRows * nRows;

            if (orderDelete <= maxNewOrder) {
              //delete player in display area, swap video blocks
              deleteInstace = cmpLytVideoBlocksGrid.swapVideo(arrVideoBlockList, instanceId);
              var swappedElement = cmpCtrVideoBlockList.SubCmpArr[arrVideoBlockList[deleteInstace].position.order - 1];
              cmpCtrVideoBlockList.SubCmpArr.remove(swappedElement);
            }
            else {
              //if it is the last
              deleteInstace = instanceId;
            }
            //remove one col: nCols = nCols - 1;
            var removeOneColStr = colStr.substring(0, colStr.lastIndexOf(','));
            this.BlkJSX.setCols(removeOneColStr, true);

            //update player windowing header selector layout
            arrVideoBlockList[instanceId].windowPlayerHeader.updateLytWinSeletor(null, removeOneColStr);
          }
          else {
            //No col shrink but need adjust position if deleted is not the last
            if (arrVideoBlockList[instanceId].position.order < nRows * nCols) { //> nRows * nRows
              //swap video blocks
              deleteInstace = cmpLytVideoBlocksGrid.swapVideo(arrVideoBlockList, instanceId);
            }
            else {
              deleteInstace = instanceId;
            }
            //Deleted is the last, just remove it
            var lastElement = cmpCtrVideoBlockList.SubCmpArr[cntBlks - 1];
            cmpCtrVideoBlockList.SubCmpArr.remove(lastElement);
          }
          cntBlkJSX = this.BlkJSX.getChild(cntBlks - 1);
          cmpLytVideoBlocksGrid.removeVideoBlockByJSX(cntBlkJSX);
          break;
        }
      } //end of CASE_2: Remove_Player
    } //end of for

    // reset updateViewType to Add_Player. This is being done so that after deleting
    // a connection expandview button can toggle and enter CASE 1
    winMultiPly.updateViewType = "Add_Player";

    //CASE_2 only
    if (deleteInstace != null) {
      arrVideoBlockList[deleteInstace] = null;
      delete arrVideoBlockList[deleteInstace];
      winMultiPly.cntAdoptedBlks--;
    }

    //CASE_1 or CASE_2
    if (winMultiPly.updateViewType == "Remove_Player" || winMultiPly.updateViewType == "Add_Player") {
      cmpLytVideoBlocksGrid.relocatePosition(arrVideoBlockList);
    }
    // restore the Fps for all players
    jsx3.sleep(function() {
      cmpLytVideoBlocksGrid.resetPlayerAudioFps();
    }, null, cmpCtrVideoBlockList);
  }
  //delete video block
  cmpLytVideoBlocksGrid.removeVideoBlockByJSX = function(cntBlkJSX) {
    var videoBlock = cntBlkJSX.getFirstChild();
    if (videoBlock != null) {
      cntBlkJSX.removeChild(videoBlock);
    }
  }
  //swap the last video block with the block to be deleted
  cmpLytVideoBlocksGrid.swapVideo = function (lcArrVideoBlockList, lcInstanceId) {
    //exchange lastInstance to lcInstanceId
    var lastInstance = getAssociativeArrLastElement(lcArrVideoBlockList);
    arrVideoBlockList[lastInstance].cmpPlayer.doAction("Stop");

    if (arrVideoBlockList[lastInstance].cmpPlayer.NativeMode) {
      //restore player with previous native state
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("NativesizeOn");
      //set 'NativesizeOff' when it's ON,so as to remove the scrollbar left after deleting
      arrVideoBlockList[lastInstance].cmpPlayer.doAction("NativesizeOff");
    }
    else {
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("NativesizeOff");
    }
    arrVideoBlockList[lcInstanceId].position.flag = "NONE";
    arrVideoBlockList[lcInstanceId].connId = arrVideoBlockList[lastInstance].connId;
    arrVideoBlockList[lcInstanceId].sessionId = arrVideoBlockList[lastInstance].sessionId;
    arrVideoBlockList[lcInstanceId].streamURL = arrVideoBlockList[lastInstance].streamURL;
    //exchange src entity name
    arrVideoBlockList[lcInstanceId].connSrcEntityName = arrVideoBlockList[lastInstance].connSrcEntityName;
    arrVideoBlockList[lcInstanceId].cmpLblVideoSrcEntity.BlkJSX.setText(arrVideoBlockList[lcInstanceId].connSrcEntityName, true);
    //get mute button states
    //var lastMuteState = arrVideoBlockList[lastInstance].cmpPlayer.cmpBtnMute.BlkJSX.getState();
    var lastMuteState = arrVideoBlockList[lastInstance].cmpPlayer.muteState;
    // get expandview button state
    var lastExpandViewState = arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnVideoExpand.BlkJSX.getState();
    //set lastInstance to '1' for mutedFlag at "OneMuteEvent" listener
    arrVideoBlockList[lastInstance].cmpPlayer.muteState = 1;
    //make sure current player visible #42886
    if (arrVideoBlockList[lcInstanceId].BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYNONE) arrVideoBlockList[lcInstanceId].BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    //streamURL of lastInstance re-strat in new position
    arrVideoBlockList[lcInstanceId].cmpPlayer.ffTrackState = arrVideoBlockList[lastInstance].cmpPlayer.ffTrackState;
    arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("Start", arrVideoBlockList[lcInstanceId].streamURL);
    //retian mute state after restart streamURL of lastInstance
    if (lastMuteState == jsx3.gui.ImageButton.STATE_ON) {
      //mute the swapped player
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("MuteOn");
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnMute.BlkJSX.setState(lastMuteState);
      arrVideoBlockList[lcInstanceId].cmpPlayer.muteState = 1;
    }
    else {
      //unmute the swapped player
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnMute.BlkJSX.setState(lastMuteState);
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("MuteOff");
      arrVideoBlockList[lcInstanceId].cmpPlayer.muteState = 0;
    }
    //set the instance player expandView button state
    if (lastExpandViewState == jsx3.gui.ImageButton.STATE_ON) {
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnVideoExpand.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
    }
    else {
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnVideoExpand.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
    }

    /***
    * Scenario here is: check whether the two swapping videos has the same remoteKBM status; 
    * if different,then have to display/hide the button based on the last player status
    * otherwise, exchange only ON/OFF status
    ***/
    if (arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBMFlag && arrVideoBlockList[lastInstance].cmpPlayer.cmpBtnRemoteKBMFlag) {
      //both remoteKBMBtn available, exchange deleted one with the last remote KBM button state
      cmpLytVideoBlocksGrid.exchangeRemoteKBMState(lcInstanceId, lastInstance);
    }
    else {
      if (arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBMFlag) {
        //deleted one cmpBtnRemoteKBM available, the last cmpBtnRemoteKBM not; just hide it
        //set to 'null' to hide the KBM button
        topic_Set(tMultiPly, "EnableInstanceKbm_" + lcInstanceId, "NONE");
        arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBMFlag = false;
      }
      if (arrVideoBlockList[lastInstance].cmpPlayer.cmpBtnRemoteKBMFlag) {
        //the last cmpBtnRemoteKBM available, deleted one not; show button and set it KBM status
        topic_Publish(tMultiPly, "EnableInstanceKbm_" + lcInstanceId, lcInstanceId);
        arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBMFlag = true;
        //exchange the KBM state
        cmpLytVideoBlocksGrid.exchangeRemoteKBMState(lcInstanceId, lastInstance);
      }
    }

    topic_Publish(tMultiPly, "OneMuteEvent", "Update");

    // If a pass to b, b pass to c, then a should pass to c
    var passValue = false;
    for (var myIstance in addressTable) {
      if (addressTable[myIstance] == lastInstance) {
        addressTable[myIstance] = lcInstanceId;
        passValue = true;
      }
    }
    if (!passValue) {
      addressTable[lastInstance] = lcInstanceId;
    }

    return lastInstance;
  }

  //help function to exchange remote KBM button state
  cmpLytVideoBlocksGrid.exchangeRemoteKBMState = function(lcInstanceId, lastInstance) {
    //get the last cmpBtnRemoteKBM status
    var lastRemoteKBMState = arrVideoBlockList[lastInstance].cmpPlayer.cmpBtnRemoteKBM.BlkJSX.getState();
    //swap deleted instance's remoteKBM state with the last player remoteKBM button state
    if (lastRemoteKBMState == jsx3.gui.ImageButton.STATE_ON) {
      //if it is ON set KBM to 'RemoteOn'
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBM.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      arrVideoBlockList[lcInstanceId].cmpPlayer.remoteEnable = 1;
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("RemoteOn");
    }
    else {
      //if it is OFF set KBM to 'RemoteOff'
      arrVideoBlockList[lcInstanceId].cmpPlayer.cmpBtnRemoteKBM.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      arrVideoBlockList[lcInstanceId].cmpPlayer.remoteEnable = 0;
      arrVideoBlockList[lcInstanceId].cmpPlayer.doAction("RemoteOff");
    }
  }
  //relocate position of each video block after "Add" or "Delete"
  cmpLytVideoBlocksGrid.relocatePosition = function(lcArrVideoBlockList) {
    //CASE_1 & CASE_2: relocat position for each video grid
    for (var instanceId in lcArrVideoBlockList) {
      //calculate each position row and col based on its order and row/col number
      var rowStrArr = new Array();
      var rowStrArr = this.BlkJSX.getRows().split(",");
      var colStrArr = new Array();
      var colStrArr = this.BlkJSX.getCols().split(",");
      var plyIndex = lcArrVideoBlockList[instanceId].position.order;
      var rowIndex = Math.ceil(plyIndex / colStrArr.length); //row & col start from '1'
      var colIndex = (plyIndex % colStrArr.length) == 0 ? colStrArr.length : (plyIndex % colStrArr.length);
      // set row and clo by the way
      lcArrVideoBlockList[instanceId].position.row = rowIndex;
      lcArrVideoBlockList[instanceId].position.col = colIndex;
    }
  }
  //drop media player Fps and mute audio
  cmpLytVideoBlocksGrid.resetPlayerAudioFps = function() {
    var sliderSettingFps = topic_Get(tMultiPly, "UpdateFps");
    sliderSettingFps = jsx3.util.strEmpty(sliderSettingFps) ? null : sliderSettingFps;
    for (var instanceId in arrVideoBlockList) {
      if (arrVideoBlockList[instanceId].cmpPlayer.setAudioFps == "DOWN") {
          //comment the following code as bug 18766 discussion
//        //get current player original mute state
//        var originalMuteState = arrVideoBlockList[instanceId].cmpPlayer.cmpBtnMute.BlkJSX.getState(); 
//        //mute all
//        cmpCtrVideoBlockList.adjustAudio("MuteOn");

//        if (originalMuteState == jsx3.gui.ImageButton.STATE_ON){
//          //mute off the current player if original is ummute 
//          cmpCtrVideoBlockList.adjustAudio("MuteOff", arrVideoBlockList[instanceId].cmpPlayer);
//        }
        //set all fps to '0' but not repaint slider pointer
        cmpCtrVideoBlockList.adjustFPS(fpsMin,false);
        //reset this fps
        cmpCtrVideoBlockList.adjustFPS(sliderSettingFps, true, arrVideoBlockList[instanceId].cmpPlayer);
        arrVideoBlockList[instanceId].cmpPlayer.setAudioFps = "NONE";
        break;
      }
      else {
        //set all fps to normal
        cmpCtrVideoBlockList.adjustFPS(sliderSettingFps, true, arrVideoBlockList[instanceId].cmpPlayer);
      }
    }
  }
  /* ------------------------ main + horiz strip --------------------*/
  var cmpBlkVideoBlockHorzMain = new Component([winMultiPlyBlkJSX,"blkVideoBlockHorzMain"]);
  var cmpLytVideoBlockHorzStrip = new Component([winMultiPlyBlkJSX,"lytVideoBlockHorzStrip"]);

  var cmpLytVideoBlocksHorzStrip = new CmpContainer([winMultiPlyBlkJSX,"lytVideoBlocksContainerHorzStrip"],
                                                 [{ cmp: cmpBlkVideoBlockHorzMain },
                                                  { cmp: cmpLytVideoBlockHorzStrip }]);

  cmpLytVideoBlocksHorzStrip.repaint = function() {
    cmpLytVideoBlocksStrip_repaint.call(this, "HSTRIP", cmpBlkVideoBlockHorzMain.BlkJSX, 
                                                            cmpLytVideoBlockHorzStrip.BlkJSX);
  }
     
  /*------------------------ END of horiz ---------------------------*/
  /* ----------------------- main + vert strip ----------------------*/
  var cmpBlkVideoBlockVertMain = new Component([winMultiPlyBlkJSX,"blkVideoBlockVertMain"]);
  var cmpLytVideoBlockVertStrip = new Component([winMultiPlyBlkJSX,"lytVideoBlockVertStrip"]);
 

  var cmpLytVideoBlocksVertStrip = new CmpContainer([winMultiPlyBlkJSX,"lytVideoBlocksContainerVertStrip"],
                                                 [{ cmp: cmpBlkVideoBlockVertMain },
                                                  { cmp: cmpLytVideoBlockVertStrip }]);

  cmpLytVideoBlocksVertStrip.repaint = function() {
    cmpLytVideoBlocksStrip_repaint.call(this, "VSTRIP", cmpBlkVideoBlockVertMain.BlkJSX,
                                                        cmpLytVideoBlockVertStrip.BlkJSX);
  }
  /*------------------------ END of Vert ---------------------------*/
  function cmpLytVideoBlocksStrip_repaint(posStrip, mainBlkJSX, stripBlkJSX)
  {
    var cntBlks = 0;
    for( var instanceId in arrVideoBlockList ) {
      // check its current position
      var objPosition = arrVideoBlockList[instanceId].position;
      // if not in the grid - add it in
      if( objPosition.ctr != posStrip ) {
        // get current rows and columns
        var nRows = stripBlkJSX.getRows().split(",").length;
        var nCols = stripBlkJSX.getCols().split(",").length;
        // if all spots in grid are taken - including main block
        if( cntBlks >= 1 + (nRows * nCols) ) {
          if( nRows == 1 ) {
            // add to rows
            stripBlkJSX.setCols(stripBlkJSX.getCols()+',*',true);
          }
          else {
            // add to cols
            stripBlkJSX.setRows(stripBlkJSX.getRows()+',*',true);
          }
        }
        // adopt the block
        if( cntBlks == 0 ) {
          // first block goes in the main area; 'true'- repaint and resize
          mainBlkJSX.adoptChild(arrVideoBlockList[instanceId].BlkJSX, true);
        }
        else {
          var cntBlkJSX = stripBlkJSX.getChild(winMultiPly.cntAdoptedBlks);
          // all others go in the strip
          cntBlkJSX.adoptChild(arrVideoBlockList[instanceId].BlkJSX, true);
          winMultiPly.cntAdoptedBlks++;
        }
        // mark it as part of the grid
        arrVideoBlockList[instanceId].position.ctr = posStrip;  
      }
        cntBlks++;
      }
  }
  /* ------------------------ END OF GRID,HSTRIP,VSTRIP --------------------------------------- */
  var cmpCtrVideoBlockList = new CmpContainer([winMultiPlyBlkJSX, "blkVideoBlocksContainer"],
                                                [],
                                                [{ action: "TOPICEVENT", topic: tMultiPly, eventvar: "UpdateFps", eventval: null },
                                                 { action: "TOPICEVENT", topic: tMultiPly, eventvar: "UpdateMute", eventval: null}],
                                                null
                                              );

  cmpCtrVideoBlockList.repaint = function (EventVar, EventVal){
    var fps = EventVal;
    switch (EventVar) {
      case "UpdateFps":
          //Adi:fps minimum is '1'(not 0);keep '' to set null/default fps for multiple players
        var lcFps = (jsx3.util.strEmpty(EventVal)) ? null : EventVal;
          cmpCtrVideoBlockList.adjustFPS(lcFps,true);
          break;
      case "UpdateMute":
        for (var instanceId in arrVideoBlockList) {
          //controle all media player's audio state
          if( topic_Get(tMultiPly, "UpdateMute")!="NONE"){
            var muteStateAll = topic_Get(tMultiPly, "MuteStateAll");
            if (muteStateAll == 0 && arrVideoBlockList[instanceId].cmpPlayer.muteState == 0) {
              //mute 
              arrVideoBlockList[instanceId].cmpPlayer.doAction("MuteOn");
              arrVideoBlockList[instanceId].cmpPlayer.muteState = 1;
              arrVideoBlockList[instanceId].cmpPlayer.cmpBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF); 
            }
            else if (muteStateAll == 1 && arrVideoBlockList[instanceId].cmpPlayer.muteState == 1) {
              //unmute 
              arrVideoBlockList[instanceId].cmpPlayer.doAction("MuteOff");
              arrVideoBlockList[instanceId].cmpPlayer.muteState = 0;
              arrVideoBlockList[instanceId].cmpPlayer.cmpBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON); 
            }
          }
        }
        //reset MuteStateAll to initial value to avoid repeat check
        if(topic_Get(tMultiPly, "UpdateMute") !="NONE") topic_Set(tMultiPly, "UpdateMute", "NONE");
        break;
      default:
        break;    
    }
  }
  // overall video blocks container                              
  var cmpCtrVideoBlocks = new CmpContainer([winMultiPlyBlkJSX, "blkVideoBlocksContainer"],
                                           [{ cmp: cmpLytVideoBlocksGrid, 
                                              sel: "topic_Get('" + tMultiPly + "', 'MediaPlayerLayoutMode') == 'GRID'"
                                            },
                                            { cmp: cmpCtrVideoBlockList }
                                           ],
                                           [{ action: "TOPICEVENT", topic: tMultiPly, eventvar: "MultiPlayerViewUpdate", eventval: null }/*,
                                           { action: "TOPICEVENT", topic: tMultiPly, eventvar: "PlayerLayoutModeChanging", eventval: null}*/],
                                           null, null);

  // event listner for select media src for single player
  eventMultiPly.subscribe("MediaSrcPortSelected", null, this,
                              function(EventVar, EventVal) {
                                var srcEntityID = EventVal;
                                if (srcEntityID == "NONE") {
                                    return;      
                                }
                                else {
                                  if (getAssociativeArrayLength(arrVideoBlockList) == 0) {
                                    windowAlert(winMultiPlyBlkJSX,"Wait for source connection ready.");
                                  }
                                  for (var lcInstanceid in arrVideoBlockList) {
                                    //if not selected, back any one instance
                                    //if selected, find the instance which has srcEntityID
                                    if (srcEntityID == arrVideoBlockList[lcInstanceid].connSrcEntityID) {
                                      if (!this.playerInstance){
                                        cmpLytVideoBlocksGrid.SubCmpArr.splice(0, 1, { cmp: arrVideoBlockList[lcInstanceid].cmpBlkVideoBlock });
                                        var cntBlkJSX = cmpLytVideoBlocksGrid.BlkJSX.getChild(0);
                                        // adopt the block and repaint
                                        cntBlkJSX.adoptChild(arrVideoBlockList[lcInstanceid].BlkJSX, true, true);
                                        this.playerInstance = lcInstanceid;
                                      }
                                      else {
                                        cmpLytVideoBlocksGrid.swapURL(this.playerInstance,lcInstanceid);
                                      }
                                      break;
                                    }
                                  }
                                  topic_Publish(tMultiPly, "MultiPlayerViewUpdate", (new Date));
                                }
                              }
                         );
  //swap the last video block with the block to be deleted
  cmpLytVideoBlocksGrid.swapURL = function (lastInstance, lcInstanceId) {
    var cmpPlayer = arrVideoBlockList[lastInstance].cmpPlayer;
    //set lastInstance to '1' for mutedFlag at "OneMuteEvent" listener
    cmpPlayer.muteState = 1;
    cmpPlayer.doAction("Stop");
    //set new src entity name
    arrVideoBlockList[lastInstance].cmpLblVideoSrcEntity.BlkJSX.setText(arrVideoBlockList[lcInstanceId].connSrcEntityName, true);
    //streamURL of lastInstance re-strat in new position
    cmpPlayer.doAction("Start", arrVideoBlockList[lcInstanceId].streamURL);
    var mutestate = cmpPlayer.muteState;

  }
  /* ------------------------ END OF blkMainVideoPane(blkVideoPanelHeader,blkVideoBlocksContainer,blkLayoutMenu) -- */

  cmpCtrVideoBlocks.destroy = function () {
    winMultiPly.cntAdoptedBlks = 0;
    if (getAssociativeArrayLength(arrVideoBlockList) > 0) {
      //delete 'instanceId' properties from arrVideoBlockList 
      for (var instanceId in arrVideoBlockList)
      {
        if (arrVideoBlockList[instanceId].cmpBlkVideoBlock) {
          //destroy windowPlayerHeader
          arrVideoBlockList[instanceId].windowPlayerHeader.destroy(instanceId, tMultiPly);    
          //destroy video block
          arrVideoBlockList[instanceId].cmpBlkVideoBlock.destroy();
        }
        delete arrVideoBlockList[instanceId];
      }
      arrVideoBlockList = null;
    }
  }

  // adjust FPS by slider;fpsValue:value of fps; booleanRepaint:repaint slider point boolean varible; cmpPlayer:player instance; 
  cmpCtrVideoBlockList.adjustFPS = function(fpsValue, booleanRepaint, cmpPlayer) {
    if (!cmpPlayer) {
      var arrLength = getAssociativeArrayLength(arrVideoBlockList);
      if (arrLength==0) return;
      // adjust fps for all players, slider value can be 'null'-- restore to default
      for (var instanceId in arrVideoBlockList) {
        var currentPlayer = arrVideoBlockList[instanceId].cmpPlayer;
        if (currentPlayer.IPVideoPlayer != null) {
          currentPlayer.setFPS(fpsValue);
        }
      }
    }
    else {
      //be able to adjust a specified cmpPlayer's FPS
      if (cmpPlayer.IPVideoPlayer != null) {
        cmpPlayer.setFPS(fpsValue);
      }
    }
    //set slider handler to a proper value
    if (jsx3.util.strEmpty(fpsValue)) {
      //if no fps slider value get defautl fps
      fpsValue = topic_Get(tApp, "IPVSPlayerDefaultFps");
    }
    //repaint slider point only when booleanRepaint 'true'
    if (winMultiPly.cmpFPSSlider && booleanRepaint) {
      winMultiPly.cmpFPSSlider.setOffset(fpsValue);
      winMultiPly.cmpFPSSlider.setTxtOffset(fpsValue);
    }
  }
  // adjust player audio MUTE/UNMUTE
  cmpCtrVideoBlockList.adjustAudio = function(audioCmd, cmpPlayer) {
    if (!cmpPlayer) {
      var arrLength = getAssociativeArrayLength(arrVideoBlockList);
      if (arrLength == 0) return;
      var btnMuteAll = getJsxByName([winMultiPlyBlkJSX, "btnMute"]);
      //mute on all players
      if (audioCmd == "MuteOn") {
        if (btnMuteAll.getState() == jsx3.gui.ImageButton.STATE_ON) {
          topic_Publish(tMultiPly, "ControllerEventRequest", "MUTE");
          btnMuteAll.setState(jsx3.gui.ImageButton.STATE_OFF);
        }
      }
      else {//UNMUTE
        if (btnMuteAll.getState() == jsx3.gui.ImageButton.STATE_OFF) {
          topic_Publish(tMultiPly, "ControllerEventRequest", "MUTE");
          btnMuteAll.setState(jsx3.gui.ImageButton.STATE_ON);
        }
      }
    }
    else {
      //be able to adjust a specified cmpPlayer's audio
      if (cmpPlayer != null) {
        var lcBtnMute = cmpPlayer.cmpBtnMute;
        //mute on all players
        if (audioCmd == "MuteOn") {
          if (lcBtnMute.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_ON) {
            lcBtnMute.onSelect.call(lcBtnMute);
            lcBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
          }
        }
        else {
          if (lcBtnMute.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_OFF) {
            lcBtnMute.onSelect.call(lcBtnMute);
            lcBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
          }
        }
      }
    }
  }

  /* ------------------------ END OF swap maxviewtoggle code --------------------------------- */
  // init vars
  topic_Set(tMultiPly, "CurrSessionID", "NONE");

  // event listner for one player mute action
  eventMultiPly.subscribe("OneMuteEvent", null, this,
                              function(EventVar, EventVal) {
                                var mutedFlag = true;
                                for (var instanceId in arrVideoBlockList) {
                                  mutedFlag = mutedFlag && arrVideoBlockList[instanceId].cmpPlayer.muteState;
                                }
                               //set general MUTE button(only for display):
                               var btnGeneralMute = getJsxByName([winMultiPlyBlkJSX, "btnMute"]);
                               if (mutedFlag) {
                                 //[1]muted if all players are muted;
                                 btnGeneralMute.setState(jsx3.gui.ImageButton.STATE_OFF);
                                 topic_Set(tMultiPly, "MuteStateAll", 0); 
                               }
                               else {
                                 //[2]unmuted if one of the players is unmuted
                                 btnGeneralMute.setState(jsx3.gui.ImageButton.STATE_ON);
                                 topic_Set(tMultiPly, "MuteStateAll", 1); 
                               }
                             }

                         );
  // event listner for collaboration objects
  eventMultiPly.subscribe("SessionID", null, this,
                              function(EventVar, EventVal)
                              {
                                // get mediaroom node from collab/ ctl state
                                var SessionID = EventVal;
                                var currSessionID = topic_Get(tMultiPly, "CurrSessionID");
                                if (SessionID != currSessionID) {
                                  if (currSessionID != "NONE") {
                                    // if changing media room, leave collaboration
                                    topic_Publish(tMultiPly, "CollaborationEventMsg", "\nLeave Session: SessionID: " + currSessionID);
                                    agtMediaStreamDst.leaveSession(tMultiPly);
                                  }
                                  // stash new mediaroom nid
                                  topic_Set(tMultiPly, "CurrSessionID", SessionID);
                                  if (SessionID != "NONE") {
                                    // join with the dest agent
                                    topic_Publish(tMultiPly, "CollaborationEventMsg", "\nJoin Session: SessionID: " + SessionID);
                                    agtMediaStreamDst.joinSession(tMultiPly, SessionID);
                                  }
                                }
                              }
                            ); 
  
  // event listner for delete a session connection
  eventMultiPly.subscribe("DeleteSessionConnection", null, this,
                              function(EventVar, EventVal) {
                                var instanceId = EventVal;
                                if (topic_Get(tMultiPly, "SinglePlayerMode") == "true") {
                                  instanceId = arrVideoBlockList[instanceId] == null ? getAssociativeArrayFirstElement(arrVideoBlockList) : instanceId;
                                }
                                else{
                                  instanceId = arrVideoBlockList[instanceId] == null ? addressTable[instanceId] : instanceId;
                                }
                                var deletingSessionID = arrVideoBlockList[instanceId].sessionId;
                                var deletingConnID = arrVideoBlockList[instanceId].connId;
                                arrVideoBlockList[instanceId].isProcessed = true;
                                //send request to delete a conneciton,
                                //response "MediaStreamDstState" event with streamAction:"Stop" if success
                                agtCollaborationCtrl.DeleteConn(tMultiPly, deletingSessionID, deletingConnID);
                              }
                            );

  // event listner for destination events ||eventMediaPlayer
  eventMultiPly.subscribe("MediaStreamDstState", null, this,
                              function(EventVar, EventVal) {
                                // get SS cmd object
                                var objSSCmd = EventVal;
                                var streamAction = objSSCmd.streamAction;
                                var streamURL = objSSCmd.streamURL;
                                var instanceId = objSSCmd.instanceId;
                                var streamNID = objSSCmd.streamNID;
                                var srcEntityType = objSSCmd.srcEntityType;
                                var connectionId = objSSCmd.connId;
                                var switchsrcFlag = false;
                                log.info("cmpBlkMultiPlayerUI: MediaStreamDstState: instanceId: " + instanceId + ": streamAction: " + streamAction + ": streamURL: " + streamURL );
                                topic_Publish(tMultiPly, "MultiPlayerUIEventMsg", "\nSSCmd: instanceId: " + instanceId + ": streamAction: " + streamAction + ": streamURL: " + streamURL);

                                /**
                                 * new condition check: 'topic_Get(tMultiPly, "SinglePlayerMode") == "true"';
                                 * For singlePlayerMode,in srcPanel there are 3 cases PLUS intial start with multi-connections, 
                                 * 1. one connection: SWITCHSRC, same instanceId,connectionId,sessionId but not streamNID;
                                 * 2. no connection: VIEW, new instanceId,streamNID,connectionId but same sessionId;
                                 * 3. initial start with many connections: add a new one and delete previous one
                                 * (2) similar to (3) but no deleteConnection, VIEW including STOP previous and START current
                                 *
                                 * Following part conside both singlePlayerMode and non-singlePlayerMode logic.
                                 **/
                                if ((arrVideoBlockList[instanceId] == null ||topic_Get(tMultiPly, "SinglePlayerMode") == "true" ) && streamAction == "Add") {
                                  //define a cmpBlkVideoBlock
                                  var cmpBlkVideoBlock = null;
                                  var positionJSONObj = null; 
                                  var initialArrLength = getAssociativeArrayLength(arrVideoBlockList);
                                  if (topic_Get(tMultiPly, "SinglePlayerMode") == "true" && getAssociativeArrayLength(arrVideoBlockList) > 0) {
                                    // only add one player for single player mode
                                    var singlePlayerInstance = getAssociativeArrayFirstElement(arrVideoBlockList);
                                    //reuse video player in single player mode
                                    cmpBlkVideoBlock = arrVideoBlockList[singlePlayerInstance].cmpBlkVideoBlock;
                                    positionJSONObj = arrVideoBlockList[singlePlayerInstance].position; 
                                    if (singlePlayerInstance != instanceId) {
                                      //2-VIEW
                                      var existingConnID = arrVideoBlockList[singlePlayerInstance].connId;
                                      if (connectionId != existingConnID) {
                                        //3-initial start with many connections
                                        var sessionID = arrVideoBlockList[singlePlayerInstance].sessionId;
                                        // send delete delete connection(connId)
                                        agtCollaborationCtrl.DeleteConn(tMultiPly, sessionID, existingConnID);
                                      }
                                      // stash player instance to connection instance for futrue header buttons click mapping
                                      var playerInstanceId = arrVideoBlockList[singlePlayerInstance].cmpPlayer.InstanceID;
                                      addressTable[playerInstanceId] = instanceId;
                                      delete arrVideoBlockList[singlePlayerInstance];
                                    }
                                    else {
                                      //1-SWITCHSRC
                                      switchsrcFlag = true;
                                    }

                                    // reset updateViewType for cmpLytVideoBlocksGrid.repaint
                                    jsx3.sleep(function() { winMultiPly.updateViewType = "NONE"; }, null, this);
                                  }
                                  else {
                                    // add player(s) for connections of MultiPlayer mode OR add a player for the first connection of SinglePlayer mode
                                    winMultiPly.updateViewType = "Add_Player";
                                    // create a new instance of a video block
                                    var plyVideoWidgetInstance = new com.ipvs.gui.PlayerVideoWidget(tMultiPly, eventMultiPly, this.BlkJSX, instanceId);
                                    // assign player video block to cmpBlkVideoBlock
                                    cmpBlkVideoBlock = plyVideoWidgetInstance.getVideoWidgetBlock();
                                  }

                                  // stash a reference to the new block in the array by instanceid
                                  if (!switchsrcFlag) {
                                    arrVideoBlockList[instanceId] = new Object;
                                    //stash player video block to the arrVideoBlockList 'instance' property
                                    arrVideoBlockList[instanceId].cmpBlkVideoBlock = cmpBlkVideoBlock;
                                    arrVideoBlockList[instanceId].cmpPlayer = cmpBlkVideoBlock.cmpPlayer;
                                    arrVideoBlockList[instanceId].cmpVideoHeader = cmpBlkVideoBlock.cmpVideoHeader;
                                    arrVideoBlockList[instanceId].BlkJSX = cmpBlkVideoBlock.BlkJSX;
                                    arrVideoBlockList[instanceId].cmpLblVideoSrcEntity = cmpBlkVideoBlock.cmpLblVideoSrcEntity;
                                  }
                                  //stash SSCmd to the arrVideoBlockList 'instance' property
                                  arrVideoBlockList[instanceId].connId = objSSCmd.connId;
                                  arrVideoBlockList[instanceId].sessionId = objSSCmd.sessionId;
                                  arrVideoBlockList[instanceId].streamNID = objSSCmd.streamNID;
                                  arrVideoBlockList[instanceId].connSrcEntityName = objSSCmd.connSrcEntityName;
                                  arrVideoBlockList[instanceId].connSrcEntityID = objSSCmd.connSrcEntityID;
                                  arrVideoBlockList[instanceId].srcEntityType = srcEntityType;
                                  arrVideoBlockList[instanceId].streamURL = objSSCmd.streamURL;
                                  arrVideoBlockList[instanceId].streamAction = objSSCmd.streamAction;
                                  if (objSSCmd.streamAction == "Add") arrVideoBlockList[instanceId].cmpPlayer.ffTrackState = objSSCmd.ffTrackState;
                                  //avoid reset for singlePlayerMode
                                  if (topic_Get(tMultiPly, "SinglePlayerMode") != "true") {
                                    //'ctr' control adoptChild video block during 'cmpLytVideoBlocksGrid' repaint
                                    arrVideoBlockList[instanceId].position = { ctr: "NONE", row: 0, col: 0, order: 0, flag: "NONE" };
                                  }
                                  else {
                                    arrVideoBlockList[instanceId].position = positionJSONObj ? positionJSONObj : { ctr: "NONE", row: 0, col: 0, order: 0, flag: "NONE" };
                                  }
                                  
                                  //add the player video block to GIRD and repaint it 
                                  if (topic_Get(tMultiPly, "SinglePlayerMode") != "true" || initialArrLength == 0) {
                                      // attach this to the sub cmp arr for the blocks containers
                                      var cmpBlkVideoBlockObj = { cmp: cmpBlkVideoBlock };
                                      cmpCtrVideoBlockList.SubCmpArr.push(cmpBlkVideoBlockObj);
                                      // publish event to update view (this will cause adopt and then show)
                                      jsx3.sleep((function() {
                                                  var data = new Date();
                                                  //(1)cmpCtrVideoBlockList.SubCmpArr.push finished 
                                                  //(2)clone is complete - publish event to update view (this will cause adopt and then show)
                                                  topic_Publish(tMultiPly, "MultiPlayerViewUpdate", data);
                                                  }
                                                ), null, this);
                                  }
                                  else if (topic_Get(tMultiPly, "SinglePlayerMode") == "true" && initialArrLength > 0) {
                                    //using publish 'DeleteConnection' to synchronize windowing player header update
                                    arrVideoBlockList[instanceId].windowPlayerHeader = new com.ipvs.gui.WindowingPlayerHeader(instanceId, tMultiPly);
                                    arrVideoBlockList[instanceId].windowPlayerHeader.updateLytWinSeletor(null, null, arrVideoBlockList);
                                    arrVideoBlockList[instanceId].windowPlayerHeader.focusDefaultBlkWindow();
                                  }
                                  
                                }
                                else {
                                  if (topic_Get(tMultiPly, "SinglePlayerMode")!= "true") {
                                    // in case of switch video block    
                                    if(arrVideoBlockList[instanceId] == null) {instanceId = addressTable[instanceId];}
                                  }

                                  if (!jsx3.util.strEmpty(arrVideoBlockList[instanceId])) {
                                    // for a valid arrVideoBlockList[instanceId]
                                    // set connId for srcPanel 
                                    if (!jsx3.util.strEmpty(arrVideoBlockList[instanceId].connId)) {
                                        topic_Set(tMultiPly, "SingleMediaPlayerConnId", arrVideoBlockList[instanceId].connId);
                                    }

                                    // send the cmd to the player
                                    if (streamAction == "Start"){
                                      arrVideoBlockList[instanceId].streamURL = streamURL;
                                      arrVideoBlockList[instanceId].cmpLblVideoSrcEntity.BlkJSX.setText(objSSCmd.connSrcEntityName, true);
                                      //check remote KBM enable for each player instance
                                      var EnableKbm = streamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");
                                      if (EnableKbm == "true" && srcEntityType == "SrcPort") {
                                        topic_Publish(tMultiPly, "EnableInstanceKbm_" + instanceId, instanceId);
                                        arrVideoBlockList[instanceId].cmpPlayer.cmpBtnRemoteKBMFlag = true;
                                      }
                                      else {
                                        arrVideoBlockList[instanceId].cmpPlayer.cmpBtnRemoteKBMFlag = false;
                                      }
                                      //check mute all state and if true(0) then set muteState = 1
                                      if(topic_Get(tMultiPly, "MuteStateAll") == 0) {
                                        arrVideoBlockList[instanceId].cmpPlayer.muteState = 1;
                                        arrVideoBlockList[instanceId].cmpPlayer.cmpBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                      }
                                      //update remoteKBM button while streamURL is available
                                      if (arrVideoBlockList[instanceId].windowPlayerHeader) arrVideoBlockList[instanceId].windowPlayerHeader.focusDefaultBlkWindow();
                                    }

                                    //if action is resume and timescale is not 1.0 then set trickplay for player
                                    if (streamAction == "Resume" || streamAction == "Start") {
                                      var currentPlaySpeed;
                                      if (streamAction == "Resume") {
                                          currentPlaySpeed = parseFloat(topic_Get(tMultiPly, "TimeScale"));
                                      }
                                      else {
                                        //check if user joined/switched when the session is paused and time scale is 0.0
                                        var playBySpeed = objSSCmd.sessionTimescale;
                                        if (streamAction == "Start" && parseFloat(objSSCmd.sessionTimescale) == 0.0) {
                                          playBySpeed = topic_Get(tMultiPly, "PlayBySpeed");
                                          //convert '0.5x' to '0.5'
                                          playBySpeed = parseFloat(playBySpeed.replace(/x$/, ''));
                                        }
                                        topic_Publish(tMultiPly, "PlayBySpeed", playBySpeed + "x");
                                        topic_Set(tMultiPly, "TimeScale", objSSCmd.sessionTimescale);
                                        currentPlaySpeed = objSSCmd.sessionTimescale;
                                      }
                                      var bolTrickPlay = (currentPlaySpeed == 1.0) ? 0 : 1;
                                      arrVideoBlockList[instanceId].cmpPlayer.setTrickMode(bolTrickPlay);
                                      if(currentPlaySpeed > 1.0 && !arrVideoBlockList[instanceId].cmpPlayer.ffTrackState) {
                                        arrVideoBlockList[instanceId].cmpBlkVideoBlock.handleMask(jsx3.gui.Block.DISPLAYBLOCK);
                                      }
                                      else {
                                        arrVideoBlockList[instanceId].cmpBlkVideoBlock.handleMask(jsx3.gui.Block.DISPLAYNONE);
                                      }
                                    }
                                    arrVideoBlockList[instanceId].cmpPlayer.doAction(streamAction, streamURL);

                                    //CASE:remove the player video block after stop it;some special cases for single player mode
                                    if (streamAction == "Stop" &&
                                          ((getAssociativeArrayLength(arrVideoBlockList) > 0 && topic_Get(tMultiPly, "SinglePlayerMode") != "true"))) {
                                        arrVideoBlockList[instanceId].cmpBlkVideoBlock.handleMask(jsx3.gui.Block.DISPLAYNONE);
                                        //consider swap multi to single player mode, need leave one player and stop the rest 
                                        //for single player mode reuse the player 
                                        if(!arrVideoBlockList[instanceId].isProcessed) {
                                          topic_Publish(tMultiPly, "DeleteConnection", connectionId);
                                        }
                                        arrVideoBlockList[instanceId].isProcessed = false;
                                        winMultiPly.updateViewType = "Remove_Player";
                                        arrVideoBlockList[instanceId].position.flag = "DELETE";
                                        var maxview = arrVideoBlockList[instanceId].cmpPlayer.displaymode.maxviewtoggle;
                                        topic_Publish(tMultiPly, "MultiPlayerViewUpdate", (new Date));
                                    }
                                  }
                                }
                              }
                            );

    // event listner for toggle display
    eventMultiPly.subscribe("ToggleDisplay", null, this,
                              function(EventVar, EventVal) {
                                if(!jsx3.util.strEmpty(EventVal)) {
                                  var instanceId = EventVal.substring(0, EventVal.indexOf(':'));
                                  var callBackMsg = EventVal.substring(EventVal.indexOf(':') + 1);
                                  log.info("cmpBlkMultiPlayerUI:fullscreen BKMKPANEL/SRCPANEL:ToggleDisplay listener: instanceId: " + instanceId + ": callBackMsg=" + callBackMsg);
                                  if (topic_Get(tMultiPly, "SinglePlayerMode") == "true") {
                                    // mapping player instance (trigger by player header's button) to connection instance
                                    instanceId = arrVideoBlockList[instanceId] == null ? addressTable[instanceId] : instanceId;
                                  }
                                  if (callBackMsg == 'true') {
                                    //go to fullscreen
                                    lytHeaderBlkJSX.setRows("0,*,0", false);
                                    winMultiPly.setLytSrcDstBkmk("BKMKPANEL", "HIDE");
                                    winMultiPly.setLytSrcDstBkmk("SRCPANEL", "HIDE");
                                    topic_Publish(tMultiPly, "FullScreenOff", false);
                                    log.info("cmpBlkMultiPlayerUI:fullscreen BKMKPANEL/SRCPANEL:ToggleDisplay listener: instanceId: " + instanceId + ":go to fullscreen");
                                  }
                                  else {
                                    //lytHeaderBlkJSX.setRows("26,*,76", false);
                                    if(arrVideoBlockList[instanceId] && arrVideoBlockList[instanceId].windowPlayerHeader && arrVideoBlockList[instanceId].windowPlayerHeader.blkWindow) {
                                      //highlight when back from fullscreen
                                      arrVideoBlockList[instanceId].windowPlayerHeader.resetCSSpvrWindowForOtherWindows(instanceId);
                                      arrVideoBlockList[instanceId].windowPlayerHeader.blkWindow.BlkJSX.setClassName(CSSpvrWindowSelected).repaint();
                                    } 
                                    topic_Publish(tMultiPly, "FullScreenOff", true);
                                    log.info("cmpBlkMultiPlayerUI:fullscreen BKMKPANEL/SRCPANEL:ToggleDisplay listener: instanceId: " + instanceId + ":back from fullscreen");
                                  }
                                }
                               }
                            );
                            
    return cmpCtrVideoBlocks;
}
