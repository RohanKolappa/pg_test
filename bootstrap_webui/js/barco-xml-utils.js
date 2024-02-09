var statusRefreshInterval = 5*1000; // 5 seconds
var barco_contextMenuA = [
	{cls:"row-edit", label:"<i class='bicon-edit'></i>",
		title:"Edit item data.", func: null},
	{cls:"row-delete", label:"<i class='bicon-trash'></i>",
		title:"Mark for removal.", func: null}
];

function createForms() {
	if (document.URL.indexOf('dashboard_page') >= 0) {
		$('#collapse_expand_all').remove();
		processSignature();
		barco_ngsSetupDashboard(statusRefreshInterval);
		return;
	}
    $(".form").each (function (index) {
		formdata = $(this).children().eq(1).html();
		formdesc = $(this).children().eq(0).html();
		extras = $(this).children().eq(2).html();
		createForm(formdesc, formdata, extras);
	});
	$(".listform").each (function (index) {
		formdata = $(this).children().eq(1).html();
		formdesc = $(this).children().eq(0).html();
		createListForm(formdesc, formdata);
	});
	$(".status").each (function (index) {
		var statusdata = $(this).children().eq(0).html();
		var title = $(this).children().eq(0).attr("id");
		displayStatus(statusdata, title);
	});
	$(".liststatus").each (function (index) {
		var statusdata = $(this).children().eq(0).html();
		var title = $(this).children().eq(0).attr("id");
		displayListStatus(statusdata, title);
	});
	$('#form_ok').click (function() {
		barco_killStatusRefresh();
		processForm();
	});
	$('#form_cancel').click (function() {
		// repaints entire page; works but is not ideal.
		// barco_killStatusRefresh();
		// window.location.reload();
		barco_collapsibleFormHide();
		resetAllFormData();
	});
	$('#apply-results').each (function() {
		var applyResults = $(this).html();
		processApplyResults(applyResults);
	});
	processSignature();
	barco_ngsSetupDetail(statusRefreshInterval);
}

function processForm() {
	barco_collectUserData();
}

function populateFormForEntry(cliname, objectentry) {
	objectentry.find('attribute').each(function() {
		var attribute = $(this);
		var name = attribute.attr('name');
		var idname = cliname + '_' + name;
		var value = attribute.attr('value');
		var chkname = '#' + cliname + '_checkbox_' + name;
		var inpType = $(chkname).attr('type');
		if (inpType === 'checkbox') {
			if (value === 'Yes') {
				$(chkname).prop('checked', true);
			} else {
				$(chkname).prop('checked', false);
			}
		} else {
			$('#' + idname).val(value);
		}
	});
}

function populateForm(cliname, xmldata) {
	$('input[name="cli_' + cliname + '"]').val('');
	xmldataDoc = $.parseXML( xmldata );
	$xmldata = $( xmldataDoc );
	$xmldata.find('objectentry').each(function() {
		populateFormForEntry(cliname, $(this));
	});
}

function setSubsectionAJAXSubmit(ajax_submit) {
	var cli = ajax_submit.attr('cli');
	var label = ajax_submit.attr('label');
	$('#' + cli).append("<input id=\"" + cli +
	"_ajax_submit\" class=\"ajax_submit\" type=\"button\" value=\"" + label + "\">");  
}

function createExtras(xmlextras) {
	extrasDoc = $.parseXML( xmlextras );
	$extrasdata = $( extrasDoc );
		$extrasdata.find('ajax_submit').each(function() {
			setSubsectionAJAXSubmit($(this));
		});
}

function sectionTitle(title, cls) {
	return "<div class='table-header'>" +
			"<div class='table-header-icon'>" +
				"<a href='#' class='" + cls + "'></a>" +
			"</div>" +
			"<h2>" + title + "</h2>" +
		"</div>";
}

function createForm(xml, xmldata, extras) {
	var tabletext="";
	var cliname="";
	xmlDoc = $.parseXML( xml ),
	$xml = $( xmlDoc ),
	$header = $xml.find( "description" );
	$section = $xml.find( "title" );
	$cli = $xml.find( "clicommandname" );
	if ($cli) {
		cliname = $cli.text();
	}
	$('#rightpanel').append(sectionTitle($header.text(), 'collapse-open'));
	tabletext = "<div id='" + cliname + "' class='sectiondiv' style='display:none;'>" +
				"<input type='hidden' name='cli_" + cliname + "'>"+
				"<table class='formTable' width='100%'>";
	var ctr=0;
	var field="";
	$xml.find('attribute').each(function() {
		var label = $(this).attr('label');
		var type = $(this).attr('type');
		var validation = $(this).attr('validation');
		var fieldname = $(this).attr('name');
		var input = "<input ";
		var format_sans_id = "";
		var format = "";
		if (type == "boolean") {
			fieldname = "checkbox_" + fieldname;
		}
		var format_name = "name=" + cliname + "_" + fieldname + " ";
		format = "id=" + cliname + "_" + fieldname + " " + format_name;
		format = format + "class=\"" + barco_validateFromXMLschema($(this)) + "\" ";
		format = format + "type=";
		var format_hidden = format_name + "type='hidden'";
		if (type == "boolean") {
			var hidden = input + format_hidden + " value='off'>";
			format = hidden + input + format + "checkbox" + ">";
		} else if (type == "password") {
			format = input + format + "password " + "size=" + $(this).attr('size') + ">";
		} else {
			if (validation=="range") {
				format = input + format + "text " + "size=" + 
					$(this).attr('size') + " fieldtype=" + type ;
				format = format + 
					" validation=" + $(this).attr('validation') +
					" min=" + $(this).attr('min') +
					" max=" + $(this).attr('max') + ">";
			} else if (validation == "enumeration") {
				var options = $(this).attr('values').split(",");
				var optionval = "";
				format = "<select "  + format + ">";
				for (i=0; i < options.length; i++) {
					if (options[i].length == 0) continue;
					optionval = options[i].trim();
					optionval = optionval.replace(/{/g, '');
					optionval = optionval.replace(/}/g, '');
					format = format + 
						"<option value='" +  optionval +
						"'>" + optionval +
						"</option>";
				}
				format = format + "</select>";
			} else {
				if ($(this).attr('size') < 33)
				{
					format = input + format + "text " + "size=" + 
						$(this).attr('size') + " fieldtype=" + type ;
				}
				else {
					format = input + format + "text " + "size=32" + " fieldtype=" + type ;
				}
			}
		}
		field = field + '<td class="table-label" width="25%">' + label +
			'</td>' + '<td width="25%" class="table-text">' + format + '</td>';
		ctr++;
		if (ctr == 2)  {
			ctr=0;
			tabletext = tabletext + '<tr>' + field + '</tr>';
			field="";
		}
	});
	if (ctr == 1) {
		field = field + '<td class="table-label" width="25%">' + '</td>' +
			'<td width="25%" class="table-text">' + '</td>';
		tabletext = tabletext + '<tr>' + field + '</tr>';
	}
	tabletext = tabletext + '</table>';
	
	if (cliname=="ms_server_options" || cliname=="dms_server_options")
		tabletext = tabletext + '<br><p>*After updating this section, please save settings and reboot the system.</p>';
	if (cliname=="demos_mode")
		tabletext = tabletext + '<br><p>*Example apps provided as is without warranty. Not for production deployment.</p>';
	
	tabletext = tabletext + '</div>';
	
			
	$('#rightpanel').append(tabletext);
	// != used to catch both null and undefined
	if (extras != null) {
		createExtras(extras)
	}
	populateForm(cliname, xmldata);
}

function createInputFields(xml) {
	var inputform="";
	var cliname="";
	xmlDoc = $.parseXML( xml ),
	$xml = $( xmlDoc ),
	$header = $xml.find( "description" );
	$section = $xml.find( "title" );
	$cli = $xml.find( "clicommandname" );
	if ($cli) {
		cliname = $cli.text();
	}
	inputform = "<input type='hidden' id='cli_" + cliname + "' name='cli_" + cliname + "' value=''>"+
		"<input type='hidden' id='" + cliname + "_deletes' name='" + cliname + "_deletes' value=''>"+
		"<input type='hidden' id='" + cliname + "_index' name='" + cliname + "_index' value=''>"+
		"<table class='formTable' width='100%'>";
	var ctr=0;
	var field="";
	$xml.find('attribute').each(function() {
		var label = $(this).attr('label');
		var type = $(this).attr('type');
		var validation = $(this).attr('validation');
		var fieldname = $(this).attr('name');
		var input = "<input ";
		var format = "";
		if (type == "boolean") {
			fieldname = "checkbox_" + fieldname;
		}
		var format_name = "name=" + cliname + "_" + fieldname + " ";
		format = "id=" + cliname + "_" + fieldname + " " + format_name;
		format = format + "class=\"" + barco_validateFromXMLschema($(this)) + "\" ";
		format = format + "type=";
		var format_hidden = format_name + "type='hidden'";
		if (type == "boolean") {
			var hidden = input + format_hidden + " value='off'>";
			format = hidden + input + format + "checkbox" + ">";
		} else if (type == "password") {
			format = input + format + "password " + "size=" + $(this).attr('size') + ">";
		} else {
			if (validation=="range") {
				format = input + format + "text " + "size=" + 
					$(this).attr('size') + " fieldtype=" + type ;
				format = format + 
					" validation=" + $(this).attr('validation') +
					" min=" + $(this).attr('min') +
					" max=" + $(this).attr('max') + ">";
			} else if (validation == "enumeration") {
				var options = $(this).attr('values').split(",");
				var optionval = "";
				format = "<select "  + format + ">";
				for (i=0; i < options.length; i++) {
					if (options[i].length == 0) continue;
					optionval = options[i].trim();
					optionval = optionval.replace(/{/g, '');
					optionval = optionval.replace(/}/g, '');
					format = format + 
						"<option value='" +  optionval +
						"'>" + optionval +
						"</option>";
				}
				format = format + "</select>";
			} else {
				format = input + format + "text " + "size=" + 
					$(this).attr('size') + " fieldtype=" + type ;
			}
		}
		field = field + '<td class="table-label" width="25%">' + label +
			'</td>' + '<td width="25%" class="table-text">' + format + '</td>';
		ctr++;
		if (ctr == 2)  {
			ctr=0;
			inputform = inputform + '<tr>' + field + '</tr>';
			field="";
		}
	});
	if (ctr == 1) {
		field = field + '<td class="table-label" width="25%">' +
			'</td>' + '<td width="25%" class="table-text">' + '</td>';
		inputform = inputform + '<tr>' + field + '</tr>';
	}
	inputform = inputform + '</table>';
	return inputform;
}

function createRowContextMenu(idx, id, contextMenu) {
	var htmlText = '<ul class="row-context-menu" style="display: none;">';
	for (var i in contextMenu) {
		var item = contextMenu[i];
		htmlText += '<li>' + '<a class="' + item.cls + '" href="#' +
			id + '" data-index="' + idx + '" title="' + item.title +
			'">' + item.label + "</a></li>"; 
	}
	return htmlText;
}

function editRowMenu(idx, id, contextMenu) {
	var htmlText = '<td class="table-row-edit"><div><a class="bicon-settings" href="#"></a>' +
		createRowContextMenu(idx, id, contextMenu) + '</div></td>';
	return htmlText;
}

function resetAllFormData() {
	$(".form").each (function (index) {
		formdesc = $(this).children().eq(0).html();
		formdata = $(this).children().eq(1).html();
		var cliname="",
		xmlDoc = $.parseXML( formdesc ),
		$xml = $( xmlDoc ),
		$cli = $xml.find( "clicommandname" );
		if ($cli) {
			cliname = $cli.text();
			populateForm(cliname, formdata);
		}
	});
	$(".listform").each (function (index) {
		formdesc = $(this).children().eq(0).html();
		// formdata = $(this).children().eq(1).html();
		var cliname="",
		xmlDoc = $.parseXML( formdesc ),
		$xml = $( xmlDoc ),
		$cli = $xml.find( "clicommandname" );
		if ($cli) {
			cliname = $cli.text();
			$('input[name="cli_' + cliname + '"]').val('');
		}
	});
}

function addBarButtons(cliCmdName) {
	var str = '';
	if (cliCmdName == 'conn_table') {
		str += "<input type='button' class='button' id='break_conn' value='Disconnect'>&nbsp;"; 
	}
	return str + "<input type='button' class='addbutton' id='add_" +
		cliCmdName + "' value='Add'>";
}

function listFormTableRowLabel(cliCmdName, column, label) {
	var str = '<th class="table-label">' + label + '</th>';
	if (cliCmdName === 'conn_table') {
		if (column === 1) {
			str = '<th class="table-label">Connect to ' +  label + '</th>';
		}
	}
	return str;
}

function listFormTableRowValue(cliCmdName, attrIdx, column, value) {
	var str = '<td class="table-text">' + value + '</td>';
	if (cliCmdName === 'conn_table') {
		if (column === 1) {
			str = '<td class="table-text"><input type="button" class="button connectbut" value="' + value + '"></td>'
		}
	}
	return str;
}

function createListForm(xml, xmldata) {
	var tabletext="";
	var listtoolbar="";
	var rows=0;
	xmlDoc = $.parseXML( xml ),
	$xml = $( xmlDoc ),
	$header = $xml.find( "description" );
	$section = $xml.find( "title" );
	var cliCmdName = $xml.find("clicommandname").text();
	listtoolbar ="<div class='form-toolbar'>" + addBarButtons(cliCmdName) + "</div>";
	$('#rightpanel').append(sectionTitle($header.text(), 'collapse-open')); 
	tabletext = "<div id='" + cliCmdName + "' class='sectiondiv' style='display:none;'>" +
		listtoolbar +
		"<div class='listcontainer'>" +
		"<table class='formTable' width='100%'>";
	var ctr=0;
	var field="";
	$xml.find('attribute').each(function() {
		var label = $(this).attr('label');
		ctr++;
		if (ctr < 8 ) {
			field += listFormTableRowLabel(cliCmdName, ctr, label);
		}
	});
	tabletext = tabletext + '<thead>' + field + '</thead><tbody>';
	xmldataDoc = $.parseXML( xmldata );
	$xmldata = $( xmldataDoc );
	$xmldata.find('objectentry').each(function() {
		ctr=0;
		field="";
		var attrIdx = $(this).attr('index');
		$(this).find('attribute').each(function() {
			var value = $(this).attr('value');
			value = maybeReplaceWithIcon(value);
			ctr++;
			if (ctr < 8 ) {
				field += listFormTableRowValue(cliCmdName, attrIdx, ctr, value);
			}
		});
		var rowId = cliCmdName + '-row-' + attrIdx;
		tabletext = tabletext + '<tr id="' + rowId + '" data-index="' +
			attrIdx + '">' + field +
			editRowMenu(attrIdx, rowId, barco_contextMenuA) + '</tr>';
		rows++;
	});
	tabletext = tabletext + '</tbody></table></div>' +
		'<div class="collapsible-form" style="display:none;">' +
		createInputFields(xml) + '</div></div>';
	$('#rightpanel').append(tabletext);
	var listContainerHeight = 30*(rows+3);
	if (listContainerHeight > 300) listContainerHeight = 300;
	$('#' + cliCmdName + ' .listcontainer').height(listContainerHeight);
}

function maybeReplaceWithIcon(value, source) {
	var ciVal = value.toLowerCase();
	if (source === "truth") {
		if (ciVal === "true") return 'Yes';
		if (ciVal === "false") return 'No';
	}
	if (source === "configuration") {
		if (ciVal === "true") return 'Enabled';
		if (ciVal === "false") return 'Disabled';
	}
	if (source === "health") {
		if (ciVal === "true") return 'Up';
		if (ciVal === "false") return 'Down';
	}
	return value;
}
        
function createStatusHTMLFromParamter(self) {
	var source = self.attr('source');
	var fmt = {before:'', after:''};
	if (source === 'dynamic') return '';
	if (source === 'preformatted') {
		fmt.before = '<pre>'; fmt.after = '</pre>';
	}
	var name = self.attr('name');
	var value = self.text();
	value = maybeReplaceWithIcon(value, source);
	if (source === 'preformatted') {
		return '<td class="table-label" width="0%">' + name +
		'</td>' + '<td width="100%" class="table-text">' +
		fmt.before + value + fmt.after + '</td>';
	}
	return '<td class="table-label" width="25%">' + name +
		'</td>' + '<td width="25%" class="table-text">' +
		fmt.before + value + fmt.after + '</td>';
}

function createStatusInnerHTML($xml, tablecli) {
	var innerHTML = "";
	var ctr = 0;
	var field = "";
	$xml.find('Parameter').each(function() {
		fieldName=($(this)).attr('name');

		field += createStatusHTMLFromParamter($(this));
		// In Disk Status, field next to Percentage is Disk. In order to bring Disk in next line, increasing the counter additionally by 1
		if (tablecli=="get_disk_status" && fieldName == "Percentage")
			ctr++;
		ctr++;

		if (ctr == 2) {
			ctr = 0;
			innerHTML = innerHTML + '<tr>' + field + '</tr>';
			field = "";
		}
	});
	$xml.find('parameter').each(function() {
		fieldName=($(this)).attr('name');
		field += createStatusHTMLFromParamter($(this));
		// In Disk Status, field next to Percentage is Disk. In order to bring Disk in next line, increasing the counter additionally by 1
		if (tablecli=="get_disk_status" && fieldName == "Percentage")
			ctr++;
		ctr++;

		if (ctr == 2) {
			ctr = 0;
			innerHTML = innerHTML + '<tr>' + field + '</tr>';
			field = "";
		}
	});
	if (ctr == 1) {
		field = field + '<td class="table-label" width="25%">' +
			'</td>' + '<td width="25%" class="table-text">' + '</td>';
		innerHTML = innerHTML + '<tr>' + field + '</tr>';
	}
	return innerHTML;
}

function createStatusHTML($xml, tablecli) {
	var tabletext="";
	tabletext = "<table class='formTable' width='100%'" +
		"id='" + tablecli + 
		"' cli='" + tablecli + "'>";

	tabletext = tabletext + createStatusInnerHTML($xml, tablecli);
	tabletext = tabletext + '</table>';
	return tabletext;
}
        
function displayStatus(xml, title) {
	var tablecli="";
	xmlDoc = $.parseXML( xml ),
	$xml = $( xmlDoc ),
	$('#status-panel').append(sectionTitle(title, 'collapse-close')); 
	$xml.find('clicmd').each(function(){
		tablecli=$(this).text();
	});
	barco_addCmdToRefreshList(tablecli);
	var tabletext = "<div class='sectiondiv' style='display:block;'>" +
		createStatusHTML($xml, tablecli) + "</div>";

	if (tablecli=="get_disk_status")
		var tabletext = tabletext+"<p>* Please rebuild the disk in case of error.</p>";
	$('#status-panel').append(tabletext);
}

function displayListStatus(xml, title) {
	var tabletext="";
	xmlDoc = $.parseXML( xml ),
	$xml = $( xmlDoc ),
	$('#status-panel').append(sectionTitle(title, 'collapse-open'));
	tabletext = "<div class='sectiondiv' style='display:none;'>" +
		"<table class='formTable' width='100%'>";
	var ctr=0;
	var headerrow="";
	var datarow="";
	$xml.find('parameters').each(function() {
		datarow="";
		ctr++;
		$(this).find ('parameter').each (function() {
			var name = $(this).attr('name');
			var value = $(this).text();
			value = maybeReplaceWithIcon(value);
			if (ctr == 1) {
				headerrow = headerrow + '<td class="table-label" >' + name + '</td>' ;
			}
			datarow = datarow + '<td class="table-text" >' + value + '</td>' ;
		});
		if (ctr == 1) {
			tabletext = tabletext + '<tr>' + headerrow + '</tr>';
		}
		tabletext = tabletext + '<tr>' + datarow + '</tr>';
	});
	tabletext = tabletext + '</table></div>';
	$('#status-panel').append(tabletext);
}

function processApplyResults(applyResultsXML) {
	var resultsText="";
	xmlDoc = $.parseXML(applyResultsXML);
	$xml = $( xmlDoc );
	$xml.find('clicmd').each(function() {
		var clicmd = $(this).attr('name');
		$(this).find('group').each(function() {
			var title = $(this).attr('name');
			var gResponse = $(this).children().eq(0);
			var statusAttr = gResponse.children().eq(0);
			var reasonAttr = gResponse.children().eq(1);
			var status = statusAttr.attr('value');
			var reason = reasonAttr.attr('value');
			if (status !== "Success") {
				resultsText += title + ' ' + status + ": " + reason + '. ';
			}
		});
	});
	if (resultsText.length > 0) {
		$('div.servererror').removeClass('hidden').html(resultsText);
		  barco_errorPriority = 1; // defined in barco_ngs-setup-detail.js
	}
}

function getListFormDescription(cliName) {
	var cliXml = null;
	$(".listform").each (function (index) {
		var formdesc = $(this).children().eq(0).html(),
		xmlDoc = $.parseXML( formdesc ),
		$xml = $( xmlDoc ),
		cliCmdName = $xml.find("clicommandname").text();
		if (cliCmdName === cliName) cliXml = $xml;
	});
	return cliXml;
}

function prefillListForm(cliName) {
	var cliXml = getListFormDescription(cliName);
	cliXml.find('attribute').each(function(){
		var name = $(this).attr('name');
		var attrType = $(this).attr('type');
		var value = $(this).attr('defaultvalue');
		if (value != null && value.length > 0) {
			if (attrType === 'boolean') {
				var chkname = '#' + cliName + '_checkbox_' + name;
				if (value === 'Yes') {
					$(chkname).prop('checked', true);
				} else {
					$(chkname).prop('checked', false);
				}
			} else {
				var idname = cliName + '_' + name;
				$('#' + idname).val(value);
			}
		}
	});
}

function processSignature() {
	var signature = $('#signature-info').val();
	var oldSignature = $('#signature').html();
	if (oldSignature !== signature)
	  $('#signature').html(signature);
}