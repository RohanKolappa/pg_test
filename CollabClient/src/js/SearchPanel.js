/* ============================================================================
 * SearchPanel Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var SearchPanel = Class.extend({
  /**
  * _defaults options 
  */
  _defaults: {
  },

  /**
  * instance variables to manage session data, windowing control status
  */
  viewState: false,


  /**
  * init function 
  */
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.log(bINFO, "init", "call", "");
    this._timeId = new Date().getTime();
    this._build();
    this._resetVariables();
    this.timezone = this._getTimezone();
  },

  /**
  * _resetVariables function 
  */
  _resetVariables: function () {
    this.recBasicTextSearch = '';
    this.liveBasicTextSearch = '';
    this.enableAdvanceSearch = false;
    this.tabSelection = 'Live';
    this.tagGroupArray = [];
    this.resetBasicSearch();
    this._resetAdvSearch();
    this._clearAllTagSelection();
    this.$panelTagValue.empty();
    this.selectedTags = [];
    this.excutedSelectedTags = [];
    this.$btnAdvSearchExpand.removeClass('filter-on');
    this.excutedSearchCriteria = { keyword: true, bookmark: true, startDate: null, endDate: null };
  },

  /**
  * _build function 
  */
  _build: function () {
    var html =
          '<div id="search-panel">  ' +
          '  <div id="search-actions" style="height:45px;">  ' +
          '    <div class="pull-left">  ' +
          '      <input id="searchInputField" type="text" class="search-form-input" value="" placeholder="Search" id="">  ' +
          '      <i id="searchClearBtn" class="fa fa-times-circle search-panel-btn search-cancel-btn vis-hidden"></i>  ' +
          '    </div>  ' +
          '    <div class="pull-left" style="margin-left:-17px;padding-top:3px;">  ' +
          '      <i id="searchBtn" class="fa fa-search search-panel-btn"></i>  ' +
          '      <i id="btnAdvSearchExpand" class="fa fa-filter search-panel-btn "></i>  ' +
          '    </div>  ' +
          '    <div class="pull-right" style="padding-top:3px;">  ' +
          '      <i id="btnCancelSearchFilter" class="fa fa-times search-panel-btn hidden"></i>  ' +
          '    </div>  ' +
          '  </div>  ' +
          ' <div id="panelSearchCriteria" class="filter-panel clear hidden"> ' +
          '   <div id="sectionAdvanceSearch" class="filter-panel-card" > ' +
          '     <div class="title">Advanced </div> ' +
          '      <span id="btnClearAllSearch" class="link" style="position:absolute;right:10px;top:8px;font-size:12px;margin-bottom:15px"> Clear All </span>' +
          '     <div id="searchCheckbox" class="form-group-small"> ' +
          '       <label style="padding-bottom:7px">Keywords</label> ' +
          '       <input id="keywordCheckbox" type="checkbox" name="keywords" value="keywords" checked style="margin-left:15px"><br> ' +
          '       <label style="padding-bottom:7px">Bookmarks</label> ' +
          '       <input id="bookmarkCheckbox" type="checkbox" name="bookmarks" value="bookmarks" checked style="margin-left:15px"> ' +
          '     </div> ' +
          '     <br> ' +
          '     <div id="searchDate" class="form-group-small"> ' +
          '       <label>Start Date</label> ' +
          '       <input id="searchStartDate_' + this._timeId + '" class="form-group-controls" type="text" placeholder="select" size="10">&nbsp;<i id="startDate" ></i><br> ' +
          '       <label>End Date</label> ' +
          '       <input id="searchEndDate_' + this._timeId + '" class="form-group-controls" type="text" placeholder="select" size="10">&nbsp;<i id="endDate" ></i> ' +
          '     </div> ' +
          '   </div> ' +
          '   <div id="tagGroupSection" class="noselect tag-panel-card" > ' +
          '      <div class="tag-name-title">Tag Filter</div> ' +
          '      <div class="tag-name-select"> ' +
          '        <select id="selectTagGroup"> ' +
          '        </select> ' +
          '      </div> ' +
          '      <span id="btnClearAllTag" class="link" style="position:absolute;right:10px;top:8px;font-size:12px;margin-bottom:15px"> Clear All </span>' +
          '      <div id="panelTagValue" > ' +
          '      </div> ' +
          '      <div id="panelTagSelectionMsg" class="selectable" > ' +
          '      </div>  ' +
          '   </div> ' +
          '</div>  ';
    jQuery(html).appendTo(this.$elem);

    //two sections of panel Advance Search
    this.$panelSearchCriteria = this.$elem.find('#panelSearchCriteria');
    this.$sectionTagGroup = this.$elem.find('#tagGroupSection');
    this.$sectionAdvanceSearch = this.$elem.find('#sectionAdvanceSearch');
    this.$btnClearAllSearch = this.$elem.find('#btnClearAllSearch');
    this.$btnCancelSearchFilter = this.$elem.find('#btnCancelSearchFilter');

    //search action buttons and fields
    this.$fieldSeachInput = this.$elem.find('#searchInputField');
    this.$btnSeachClearInput = this.$elem.find('#searchClearBtn');
    this.$btnSearch = this.$elem.find('#searchBtn');
    this.$btnAdvSearchExpand = this.$elem.find('#btnAdvSearchExpand');
    //search option buttons and fields
    this.$checkKeywords = this.$elem.find('#keywordCheckbox');
    this.$checkBookmark = this.$elem.find('#bookmarkCheckbox');
    this.$searchStartDate = this.$elem.find('#startDate');
    this.$searchEndDate = this.$elem.find('#endDate');
    this.$fieldSearchStartDate = this.$elem.find('#searchStartDate_' + this._timeId);
    this.$fieldSearchEndDate = this.$elem.find('#searchEndDate_' + this._timeId);

    //tag group section 
    this.$selectTagGroup = this.$elem.find('#selectTagGroup');
    this.$panelTagValue = this.$elem.find('#panelTagValue');
    this.$btnClearAllTag = this.$elem.find('#btnClearAllTag');
    this.$panelTagSelectionMsg = this.$elem.find('#panelTagSelectionMsg');

    jQuery.datepicker.setDefaults({
      showOn: "both",
      buttonImage: "./resource/images/icon-16-calendar.png",
      buttonImageOnly: true,
      changeDay: true,
      changeMonth: true,
      changeYear: true,
      dateFormat: 'mm/dd/yy',
      yearRange: '-10:+0',
      defaultDate: null,
      onClose: function (e) {
        function isDonePressed() {
          return ($('#ui-datepicker-div').html().indexOf('ui-datepicker-close ui-state-default ui-priority-primary ui-corner-all ui-state-hover') > -1);
        }
        if (isDonePressed()) {
          this.value = "";
        }
      },
      showButtonPanel: true,
      closeText: "Clear",
      buttonText: ''
    });
    this.startDatePicker = this.$fieldSearchStartDate.datepicker({
      onSelect: jQuery.proxy(function (selected) {
        this.$fieldSearchEndDate.datepicker("option", "minDate", selected)
      }, this)
    });
    this.endDatePicker = this.$fieldSearchEndDate.datepicker({
      onSelect: jQuery.proxy(function (selected) {
        this.$fieldSearchStartDate.datepicker("option", "maxDate", selected)
      }, this)
    });
  },

  /**
  * setPanel function 
  */
  setPanel: function () {
    this.log(bINFO, "setPanel", "call", "");
    if (!this.viewState) {
      this._attachEvents();
      this.viewState = true;
      this._getPortTags();
    }
  },

  /**
  * resetPanel function 
  */
  resetPanel: function () {
    if (this.viewState) {
      this._detachEvents();
      this.viewState = false;
      this._resetVariables();
    }
  },

  /**
  * _attachEvents function, attach events 
  */
  _attachEvents: function () {
    this.$fieldSeachInput.on('keypress', jQuery.proxy(this._pressEnterToSearch, this));
    this.$fieldSeachInput.on('keydown', jQuery.proxy(this._showClearBtn, this));
    this.$btnSeachClearInput.on('click', jQuery.proxy(this.clearBasicSearchField, this));
    this.$btnSearch.on('click', jQuery.proxy(this._doSearch, this));
    this.$btnAdvSearchExpand.on('click', jQuery.proxy(this._showHideAdvSearch, this));
    this.$fieldSearchStartDate.on('keypress', jQuery.proxy(this._inputDateSearch, this));
    this.$fieldSearchEndDate.on('keypress', jQuery.proxy(this._inputDateSearch, this));
    this.$selectTagGroup.on('change', jQuery.proxy(this._changeTagGroup, this));
    this.$btnClearAllTag.on('click', jQuery.proxy(this._clearAllTagSelection, this));
    this.$btnClearAllSearch.on('click', jQuery.proxy(this._resetAdvSearch, this));
    this.$btnCancelSearchFilter.on('click', jQuery.proxy(this._cancel, this));
  },

  /**
  * _detachEvents function, detach events
  */
  _detachEvents: function () {
    this.$fieldSeachInput.off('keypress', jQuery.proxy(this._pressEnterToSearch, this));
    this.$fieldSeachInput.on('keydown', jQuery.proxy(this._showClearBtn, this));
    this.$btnSeachClearInput.off('click', jQuery.proxy(this.clearBasicSearchField, this));
    this.$btnSearch.off('click', jQuery.proxy(this._doSearch, this));
    this.$btnAdvSearchExpand.off('click', jQuery.proxy(this._showHideAdvSearch, this));
    this.$fieldSearchStartDate.off('keypress', jQuery.proxy(this._inputDateSearch, this));
    this.$fieldSearchEndDate.off('keypress', jQuery.proxy(this._inputDateSearch, this));
    this.$selectTagGroup.off('change', jQuery.proxy(this._changeTagGroup, this));
    this.$btnClearAllTag.off('click', jQuery.proxy(this._clearAllTagSelection, this));
    this.$btnClearAllSearch.off('click', jQuery.proxy(this._resetAdvSearch, this));
    this.$btnCancelSearchFilter.off('click', jQuery.proxy(this._cancel, this));
    if (this.$panelTagValue.find('input[name="tagkeyword"]').length > 0) this.$panelTagValue.find('input[name="tagkeyword"]').off('click', jQuery.proxy(this._clickTagVal, this));
  },

  /**
  * _pressEnterToSearch function 
  * @param event {Object}, 
  * @param data {JSON}
  */
  _pressEnterToSearch: function (e, data) {
    if (e.keyCode == 13) {
      this._doSearch();
    }
  },

  /**
  * clearBasicSearchField function 
  */
  clearBasicSearchField: function () {
    if (this.tabSelection == 'Live') {
      this.liveBasicTextSearch = '';
    } else {
      this.recBasicTextSearch = '';
      //this._resetAdvSearch();
    }
    this.$fieldSeachInput.val('');
    this._hideClearBtn();
  },

  /**
  * _showClearBtn function 
  */
  _showClearBtn: function () {
    this.$btnSeachClearInput.removeClass('vis-hidden');
  },

  /**
  * _hideClearBtn function 
  */
  _hideClearBtn: function () {
    this.$btnSeachClearInput.addClass('vis-hidden');
  },

  /**
  * _showHideAdvSearch function 
  */
  _showHideAdvSearch: function () {
    var displayRefreshBtn = true;
    if (this.$panelSearchCriteria.hasClass('hidden')) {
      this.$btnAdvSearchExpand.addClass('hidden');
      this.$elem.addClass('open');
      this.show();
      displayRefreshBtn = false;
    } else {
      this.$elem.removeClass('open');
      this.$btnAdvSearchExpand.removeClass('hidden');
      this.hide();
    }
    //{mode:"DISPLAY",data:true|flase}
    if (this.options.callBack) {
      this.options.callBack({ mode: "DISPLAY", data: displayRefreshBtn });
    }
  },

  /**
  * resetBasicSearch function 
  **/
  resetBasicSearch: function () {
    this.$fieldSeachInput.val('');
    this._hideClearBtn();
  },

  /**
  * _resetAdvSearch function 
  **/
  _resetAdvSearch: function () {
    this.$checkKeywords.prop("checked", true);
    this.$checkBookmark.prop("checked", true);
    this.startDatePicker.val('');
    this.endDatePicker.val('');
    this.$fieldSearchStartDate.datepicker("option", "maxDate", new Date());
    this.$fieldSearchEndDate.datepicker("option", "minDate", new Date());
  },

  /**
  * _clearAllTagSelection function 
  **/
  _clearAllTagSelection: function () {
    var selectedTags = this.$elem.find('input[name="tagkeyword"]:checked');
    jQuery.each(selectedTags, jQuery.proxy(function (index, tag) {
      jQuery(tag).prop('checked', false);
    }, this));
    this.selectedTags = [];
    this.$panelTagSelectionMsg.html(this._displayTagValueString(this.selectedTags));
  },

  /**
  * _cancel function 
  */
  _cancel: function () {
    if (this.tabSelection == 'Live') {
      this._clearAllTagSelection();
    }
    this._showHideAdvSearch();
    this.$btnAdvSearchExpand.removeClass('hidden');
  },

  /**
  * _inputDateSearch function 
  */
  _inputDateSearch: function () {
    if (e.keyCode == 13) {
      this._doSearch();
    }
  },

  /**
  * _getTimezone function 
  */
  _getTimezone: function () {
    var timeZone = '+0000';
    var newDate = new Date();
    //-270:+0430;270:-0430;-345:+0545
    var timezoneOffset = newDate.getTimezoneOffset();
    hset = Math.abs(timezoneOffset) / 60;
    hset = Math.floor(hset);
    hset = (hset >= 10) ? hset : ('0' + hset);
    mset = Math.abs(timezoneOffset) % 60;
    mset = (mset >= 10) ? mset : ('0' + mset);
    if (timezoneOffset <= 0) {
      timeZone = '+' + hset + mset;
    } else {
      timeZone = '-' + hset + mset;
    }
    return timeZone;
  },

  /**
  * _doSearch function 
  */
  _doSearch: function () {
    var basicSearchText = this.$fieldSeachInput.val();
    var whereClause = '&whereTextSubStr=' + basicSearchText;
    if (this.tabSelection == 'Live') {
      this.liveBasicTextSearch = basicSearchText;
    } else {
      this.recBasicTextSearch = basicSearchText;
    }

    if (this.enableAdvanceSearch) {
      //Acvance Search
      var isKeywordSearch = this.$checkKeywords.is(':checked');
      var isBookmarkSearch = this.$checkBookmark.is(':checked');
      if (isKeywordSearch && isBookmarkSearch) {
        whereClause = '&whereTextSubStr=' + basicSearchText + '&searchTextSubStrIn=TITLE,DESCRIPTION,METADATA,BOOKMARK';
      }
      else if (isBookmarkSearch) {
        whereClause = '&whereTextSubStr=' + basicSearchText + '&searchTextSubStrIn=BOOKMARK';
      }
      else if (isKeywordSearch) {
        whereClause = '&whereTextSubStr=' + basicSearchText + '&searchTextSubStrIn=TITLE,DESCRIPTION,METADATA';
      }

      var startDate = this.$fieldSearchStartDate.datepicker('getDate'),
          endDate = this.$fieldSearchEndDate.datepicker('getDate');

      if (startDate || endDate) {
        var dateRange = '', startDateStr = '', endDateStr = '';
        if (!startDate) {
          //only have 'End Date', cook up 'Start Date' as beginning of the epoch time
          startDate = new Date('January 1, 1970 00:00:00');
        }
        else if (!endDate) {
          //only have 'Start Date', cook up 'End Date' as today
          endDate = new Date();
        }

        //have both 'Start Date' & 'End Date'
        if (endDate >= startDate) {
          startDateStr = new Date(startDate.toString()).format('Y-m-d  G:i:s.u ') + this.timezone; 
          endDateStr = new Date(endDate.toString()).format('Y-m-d  h:i:s.u ') + this.timezone; 
          endDateStr = endDateStr.replace(/\d\d:\d\d:\d\d.\d\d\d/, "23:59:59.999");
          dateRange = 'From:' + startDateStr + ',To:' + endDateStr;
        } else {
          alert('Please input a correct data range');
          this.$searchStartDate.focus();
          return;
        }
        dateRange = escape(dateRange);
        if (dateRange.match(/\+\d\d\d\d/)) {
          dateRange = dateRange.replace(/\+/g, '%2B');
        }
        //date range search format : From:2014-03-01 00:00:00.000 -0700,To:2014-03-11 23:59:59.999 -0700
        whereClause += '&whereDateRange=' + dateRange;
      }
      this.excutedSearchCriteria = { keyword: isKeywordSearch, bookmark: isBookmarkSearch, startDate: startDate, endDate: endDate };
    } else {
      //Name='Location' and Value='Hyderabad'
      //var selectedTags = this.$elem.find('input[name="tagkeyword"]:checked');
      if (this.selectedTags.length > 0) {
        whereClause += "&whereTagExpr=";
        jQuery.each(this.selectedTags, jQuery.proxy(function (index, tagValPair) {
          var tagValPairArr = tagValPair.trim().split('_');
          whereClause += "Name%3D'" + tagValPairArr[0] + "'%20and%20Value%3D'" + tagValPairArr[1] + "'%2C";
        }, this));
        whereClause = whereClause.substring(0, whereClause.length - 3);
      }
      this.excutedSelectedTags = this.selectedTags.concat();
    }
    //{mode:"QUERY",data:whereClause}
    if (this.options.callBack) {
      this.options.callBack({ mode: "QUERY", data: whereClause });
    }
    this.hide();
  },

  /**
  * getPortTags: send getPortTags request 
  */
  _getPortTags: function () {
    var urlStr = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?selectFormat=NameValueTree';
    this.log(bINFO, "_getPortTags", "call", "url: " + urlStr);
    this.options.barcoAPI.getPortTags(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, '_getPortTags', 'success', '_getPortTags success ');
        var tagArr = data.Resource ? data.Resource.Tag : null;
        if (tagArr) {
          if (!(tagArr instanceof Array)) tagArr = new Array(tagArr);
          this._displayTagGroupByResponse(tagArr);
        } else {
          this.log(bERROR, '_getPortTags', 'fail', ' getPortTags response empty');
        }

      } else {
        this.log(bERROR, '_getPortTags', 'fail', ' getPortTags request fail');
      }
    }, this));
  },

  /**
  * _displayTagGroupByResponse: render getPortTags in panel 
  * @param tagArr {Array|object}
  */
  _displayTagGroupByResponse: function (tagArr) {
    this.tagGroupArray = tagArr;
    this.log(bINFO, "_displayTagGroupByResponse", "call", "  ");
    var tagGroupNames = this._getTagGroupNames();

    //display tags 
    this.$selectTagGroup.empty();
    var htmlStr = ' <option>All</option> ';
    jQuery.each(tagGroupNames, function (index, tagGroup) {
      htmlStr += '<option id="options_' + tagGroup + '">' + tagGroup + '</option>';
    });
    jQuery(htmlStr).appendTo(this.$selectTagGroup);

    //display tag with its values
    this.$panelTagValue.empty();
    var html = '';
    jQuery.each(this.tagGroupArray, jQuery.proxy(function (index, value) {
      var tagName = value['@Name'];
      html += '<div id="' + tagName + '" style="margin-top:15px" class="hidden"> ' +
               '  <p> ' + tagName + ' </p>';
      var tagValueArr = this._getTagValByName(tagName);
      var indexId = new Date().getTime();
      jQuery.each(tagValueArr, jQuery.proxy(function (index) {
        html += ' <div> ';
        //should use this.excutedSelectedTags 
        // <input id='xx'> <label for='xx'> where id has to identical with DOM  
        var tagValuePair = tagName + '_' + tagValueArr[index] + '_' + indexId;
        var checkedProp = (this.excutedSelectedTags.indexOf(tagValuePair) != -1) ? ' checked' : '';
        html += '   <input id="' + tagValuePair + '" type="checkbox" name="tagkeyword" value="' + tagValueArr[index] + '" ' + checkedProp + '> ';
        html += '   <label for="' + tagValuePair + '">' + tagValueArr[index] + '</label> </div> ';
      }, this));
      html += '</div>';
    }, this));
    jQuery(html).appendTo(this.$panelTagValue);
    //bind tag click
    this.$panelTagValue.find('input[name="tagkeyword"]').on('click', jQuery.proxy(this._clickTagVal, this));
  },

  /**
  * _clickTagVal
  */
  _clickTagVal: function (event) {
    this.log(bINFO, "_clickTagVal", "call", "  ");
    var isChecked = event.currentTarget.checked;
    var tagValuePairStr = event.currentTarget.id;
    var tag = tagValuePairStr.split('_')[0];

    if (isChecked) {
      if (this.selectedTags.indexOf(tagValuePairStr) == -1) {
        this.selectedTags.push(tagValuePairStr);
      }
    } else {
      if (this.selectedTags.indexOf(tagValuePairStr) >= 0) {
        this.selectedTags.splice(this.selectedTags.indexOf(tagValuePairStr), 1);
      }
    }
    //update tag/value message while switch group
    this.$panelTagSelectionMsg.html(this._displayTagValueString(this.selectedTags));
  },

  /**
  * _getTagGroupNames
  * @return {Array|String}
  */
  _getTagGroupNames: function () {
    this.log(bINFO, "_getTagGroupNames", "call", " this.tagGroupArray: " + this.tagGroupArray);
    var tagGroupNames = [];
    jQuery.each(this.tagGroupArray, function (index, value) {
      tagGroupNames.push(value['@Name']);
    });
    return tagGroupNames;
  },

  /**
  * _getTagValByName
  * @param tagGroupName {String}
  * @return {Array|String}
  */
  _getTagValByName: function (tagGroupName) {
    this.log(bINFO, "_getTagValByName", "call", "  tagGroupName: " + tagGroupName);
    var tagValues = [];
    jQuery.each(this.tagGroupArray, function (index, value) {
      if (value['@Name'] === tagGroupName) {
        if (!(value.Value instanceof Array)) tagValues = new Array(value.Value);
        else tagValues = value.Value;
        return false;
      }
    });
    return tagValues;
  },

  /**
  * _changeTagGroup
  */
  _changeTagGroup: function (event) {
    this.log(bINFO, "_changeTagGroup", "call", "  _changeTagGroup: ");
    var index = event.target ? event.target.selectedIndex : 0;
    var tagGroupName = (event.target && event.target.children[index]) ? event.target.children[index].value : null;
    if (this.selectedTags.length == 0 && this.excutedSelectedTags.length > 0) {
      //update tag/value message with this.excutedSelectedTags
      this.$panelTagSelectionMsg.html(this._displayTagValueString(this.excutedSelectedTags));
      //shallow copy
      this.selectedTags = this.excutedSelectedTags.concat();
      //mark selected by excuted tag/value
      jQuery.each(this.excutedSelectedTags, jQuery.proxy(function (index, tagValPair) {
        var tagValPairArr = tagValPair.split('_');
        var tagName = tagValPairArr[0];
        var tagValue = tagValPairArr[1];
        var $tagDiv = this.$panelTagValue.find('#' + tagName);
        $tagDiv.find('input[value="' + tagValue + '"]').prop('checked', true);
      }, this));
    }

    if (tagGroupName) {
      this._showTagGroupHTMLByName(tagGroupName);
      if (tagGroupName == 'All') {
        this.$panelTagValue.find('p').show();
      } else {
        this.$panelTagValue.find('p').hide();
      }
    } else {
      this.log(bERROR, '_changeTagGroup', 'error', 'tagName is empty');
    }
  },

  /**
  * _displayTagValueString
  */
  _displayTagValueString: function (jsonObjArr) {
    var tagValStr = '';
    jQuery.each(jsonObjArr, jQuery.proxy(function (index, tagValPair) {
      var tagValPairArr = tagValPair.split('_');
      tagValStr += tagValPairArr[0] + ':' + tagValPairArr[1] + ', ';
    }, this));
    tagValStr = '<p style="color:lightblue;">' + tagValStr.substring(0, tagValStr.lastIndexOf(',')) + '</p>';
    return tagValStr;
  },

  /**
  * _showTagGroupHTMLByName by tag name
  * @param tagName {String}
  * @param display {Bool}
  * @return {String}
  */
  _showTagGroupHTMLByName: function (tagName) {
    jQuery.each(this.tagGroupArray, jQuery.proxy(function (index, value) {
      if (tagName != value['@Name'].trim() && tagName != "All") {
        this.$panelTagValue.find('#' + value["@Name"]).css("display", "none");
      } else {
        this.$panelTagValue.find('#' + value["@Name"]).css("display", "block");
      }
    }, this));
  },

  /**
  * _setTagFilterPanelToPreviousVal function 
  */
  _setTagFilterPanelToPreviousVal: function () {
    this.$selectTagGroup.val('All').trigger('change');
  },

  /**
  * _setAdvSearchPanelToPreviousVal function 
  */
  _setAdvSearchPanelToPreviousVal: function () {
    this.$checkKeywords.prop("checked", this.excutedSearchCriteria.keyword);
    this.$checkBookmark.prop("checked", this.excutedSearchCriteria.bookmark);
    this.$fieldSearchStartDate.datepicker('setDate', this.excutedSearchCriteria.startDate);
    this.$fieldSearchEndDate.datepicker('setDate', this.excutedSearchCriteria.endDate);
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.resetPanel();
    this.$elem.removeData('edp_searchPanel');
  },

  /**
  * show function 
  */
  show: function () {
    this.$elem.addClass('open');
    this.$panelSearchCriteria.removeClass('hidden');
    if (this.tabSelection == 'Live') {
      this.$sectionTagGroup.removeClass('hidden');
      this.$sectionAdvanceSearch.addClass('hidden');
      this._setTagFilterPanelToPreviousVal();
    } else {
      this.$sectionTagGroup.addClass('hidden');
      this.$sectionAdvanceSearch.removeClass('hidden');
      this._setAdvSearchPanelToPreviousVal();
    }
    this.$btnCancelSearchFilter.removeClass('hidden');
    this.$btnAdvSearchExpand.addClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.removeClass('open');
    this.$panelSearchCriteria.addClass('hidden');
    this.$btnAdvSearchExpand.removeClass('hidden');
    if (this.tabSelection == 'Live') {
      //Live tab
      if (this.excutedSelectedTags.length > 0) {
        this.$btnAdvSearchExpand.addClass('filter-on');
      } else {
        this.$btnAdvSearchExpand.removeClass('filter-on');
      }
    } else {
      //Recordings tab
      if (this._hasAdvSearch(this.excutedSearchCriteria)) {
        this.$btnAdvSearchExpand.addClass('filter-on');
      } else {
        this.$btnAdvSearchExpand.removeClass('filter-on');
      }
    }
    this.$btnCancelSearchFilter.addClass('hidden');
  },

  /**
  * isAdvanceHidden function 
  */
  isAdvanceHidden: function () {
    return this.$panelSearchCriteria.hasClass('hidden');
  },

  /**
  * _hasAdvSearch function 
  * @param obj {JSON}, i.e { keyword: true, bookmark: true, startDate: null, endDate: null }
  * @return advSearch {Bool}
  */
  _hasAdvSearch: function (obj) {
    return (!obj["keyword"] || !obj["bookmark"] || (obj["startDate"] != null) || (obj["endDate"] != null));
  },

  /**
  * configSearchFilterPanel function, triggered by swith Live/Recordings tabs 
  * @param {boolean|true|false}, true:Live tab; false:Recordings tab;
  */
  configSearchFilterPanel: function (isLiveTab) {
    if (isLiveTab) {
      this.tabSelection = 'Live';
      this.enableAdvanceSearch = false;
      if (this.liveBasicTextSearch != '') {
        this._showClearBtn();
      } else {
        this._hideClearBtn();
      }
      this.$fieldSeachInput.val(this.liveBasicTextSearch);
      if (this.excutedSelectedTags.length > 0) {
        this.$btnAdvSearchExpand.addClass('filter-on');
      } else {
        this.$btnAdvSearchExpand.removeClass('filter-on');
      }
    } else {
      this.tabSelection = 'Recordings';
      this.enableAdvanceSearch = true;
      if (this.recBasicTextSearch != '') {
        this._showClearBtn();
      } else {
        this._hideClearBtn();
      }
      this.$fieldSeachInput.val(this.recBasicTextSearch);
      if (this._hasAdvSearch(this.excutedSearchCriteria)) {
        this.$btnAdvSearchExpand.addClass('filter-on');
      } else {
        this.$btnAdvSearchExpand.removeClass('filter-on');
      }

    }

  }

});
// expose Objects as jQuery plugins
jQuery.plugin('edp_searchPanel', SearchPanel);