/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var MessagePanel = Class.extend({
  init: function(options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
  },
  _defaults: {
  },
  _build: function () {
    // override with concrete method
  },
  destroy: function(pluginName) {
    // override with concrete method
    // destroy self
    this.$elem.removeData(pluginName);
  },
  display: function(message) {
    // override with concrete method
  },
  clear: function() {
    // override with concrete method
  }
});

var SessionMessagePanelDefault = MessagePanel.extend({
  init: function(options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$sessionCtl = this.$elem.closest('.SessionController').parent();
    this._build();
  },
  _defaults: {
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    //Message Panel
    var child = jQuery('<div id="' + this.options.divId + '_sessionMessagePanel' +
      '" class="sessionMessagePanelPopup hidden">' + '<span class="sessionMessagePanelTitle"></span>' +
      '<a class="sessionMessagePanelDismiss"></a>' +
      '<span class="sessionMessagePanelSummary"></span>' +
      '</div>').addClass('SessionMessagePanelDefault');
    this.$elem.append(child);
    this.ctrls = {};
    this.ctrls.$title = child.find('span.sessionMessagePanelTitle').first();
    this.ctrls.$summary = child.find('span.sessionMessagePanelSummary').first();
    child.find('a.sessionMessagePanelDismiss').first().click(jQuery.proxy(this._doDismiss, this));
    this.log(bINFO, "_build", "return", "Done");
  },
  _doDismiss: function (event) {
    event.preventDefault();
    this.log(bDEBUG, "_doDismiss", "call", "");
    this.clear();
    },
  destroy: function(pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.SessionMessagePanelDefault').first().remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
   },
  display: function(severity, title, summary) {
    this.log(bDEBUG, "display", "call", title + ", " + summary);
    var $messagePanel = this.$elem.find('.SessionMessagePanelDefault').first();
    $messagePanel.removeClass('messagePanel-error');
    $messagePanel.removeClass('messagePanel-warning');
    $messagePanel.removeClass('messagePanel-info');
    $messagePanel.removeClass('messagePanel-success');
    switch (severity) {
      case "ERROR": $messagePanel.addClass('messagePanel-error'); break;
      case "WARNING": $messagePanel.addClass('messagePanel-warning'); break;
      case "SUCCESS": $messagePanel.addClass('messagePanel-success'); break;
      default: $messagePanel.addClass('messagePanel-info');
    }
    if (!$messagePanel.is(':visible')) {
      $messagePanel.removeClass('hidden');
    }
   this.ctrls.$title.html(title);
   this.ctrls.$summary.html(summary);
  },
  clear: function () {
    this.log(bDEBUG, "clear", "call", "");
    var $messagePanel = this.$elem.find('.SessionMessagePanelDefault').first();
    this.ctrls.$summary.html("");
    this.ctrls.$title.html("");
    if ($messagePanel.is(':visible')) {
      $messagePanel.addClass('hidden');
    }
  }
});

jQuery.plugin('sessionMessagePanelDefault', SessionMessagePanelDefault);
