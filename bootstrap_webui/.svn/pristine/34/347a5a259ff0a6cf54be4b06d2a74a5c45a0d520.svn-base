#!/bin/ash
# barco web interface functions
# (C) 2009-2011 Peter Korsgaard <peter.korsgaard@barco.com>
# uncomment the line below to enable debug tracing
export _DEBUG="on"
# change > to >> in the line below to log multiple cgi's
eval 'exec 2>/tmp/bweb_trace.log'
echo "================== ${SCRIPT_NAME} =================" >&2
if [ "$_DEBUG" == "on" ]; then
  env | sort >&2
  set -x
fi

# version number
BWEB_VERSION=0.0.3

# additional message to display when password is changed
BWEB_EXTRA_PWD_CHANGE_MSG=""

#signature of the system
BWEB_SIGNATURE=" "

# authentication level
bweb_auth_level=0

# authenticated user if any
bweb_auth_user=''

bweb_locate_res() {
	local path=$1
	local base=$2
	local suff=$3
	local model
	local swver
	if [ ! -f /tmp/get_version.xml ]; then
		/V2O/bin/get_version > /tmp/get_version.xml
	fi
	local model=`cat /tmp/get_version.xml | grep 'name="hardware"' | cut -f4 -d'"'`
	local swver=`cat /tmp/get_version.xml | grep 'name="swvers"' | cut -f4 -d'"'`
	model=`echo $model | tr '[:punct:]' '-' | tr '[A-Z]' '[a-z]'`
	swver=`echo $swver | tr '[:punct:]' '-' | tr '[A-Z]' '[a-z]'`
	local mParts=`echo $model | tr -s '-' '\n' | wc -l`
	local sParts=`echo $swver | tr -s '-' '\n' | wc -l`
	local i=$mParts
	local j=$sParts
	local m
	local s
	local res
	bweb_resource=''
	while [ $i -ge 0 ]; do
		m=''
		if [ $i -gt 0 ]; then
			m=`echo _$model | cut -f1-$i -d'-'`
		fi
		j=$sParts
		while [ $j -ge 0 ]; do
			s=''
			if [ $j -gt 0 ]; then
				s=`echo _$swver | cut -f1-$j -d'-'`
			fi
			j=$((--j))
			res=`echo ${path}${base}${m}${s}${suff}`
			echo "bweb_wizard res $res" >> /tmp/megm.log

			if [ -r $res ]; then
				bweb_resource=$res
				i=-1
				j=-1
			fi
		done
		i=$((--i))
	done
	
	echo "bweb_res:   $bweb_resource " >> /var/www/megm.log
	export bweb_resource
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_locate_res" >&2
}

bweb_locate_res '/usr/share/bweb/wizard/' 'bweb_private_wizard' '.sh'
if [ -n "$bweb_resource" ]; then
  source $bweb_resource
fi

_bweb_sidebar_button() {
  local minauth=$1
  local id=$2
  local label=$3
  if([ $bweb_auth_level -ge $minauth ])
  then
    echo "<p class=\"sidebar-button\">"
    echo "  <input id=\"$id\" type=\"button\" class=\"cmdbutton\" value=\"$label\">"
    echo "</p>"
  fi
  [ "$_DEBUG" == "on" ] && echo "EXIT _bweb_sidebar_button" >&2
}

# get auth level for user
# args:
#  username
# can be overwritten for custom passwd storage
_bweb_auth_get_level() {
	awk -F: "/^$1:/ { print \$2 }" $BWEB_CONF_DIR/htpasswd
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_get_level" >&2
}

# get (hashed ?) password for user
# args:
#  username
# can be overwritten for custom passwd storage
_bweb_auth_get_password() {
#	awk -F: "/^$1:/ { p=\$3; for (i=4; i<=NF; i++) p=p \":\" \$i; print p }" $BWEB_CONF_DIR/htpasswd
	_bweb_get_updateconfig "admin"
	grep password /tmp/get_admin.xml| cut -f4 -d'"'
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_get_password" >&2
}

# calculate password hash
# args:
#  username password
# can be overwritten for custom hash calculation; default is "plain text"
_bweb_auth_calc_hash() {
	echo -n "$2"
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_calc_hash" >&2
}

# set password for user
# args:
#  username unhashed_password
# can be overwritten for custom passwd storage
_bweb_auth_set_password() {
	sed -i "s/^\($1:[0-9]*:\).*/\1$(_bweb_auth_calc_hash $1 $2)/" $BWEB_CONF_DIR/htpasswd
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_set_password" >&2
}

# reset passwords to factory default
# can be overwritten for custom passwd storage
_bweb_auth_reset_passwords() {
	rm -rf /tmp/.newhtpasswd

	for line in `cat $BWEB_DEFAULT_PASSWD`;
	do
		if [ `echo $line | cut -d: -f2` -le $bweb_auth_level ];
		then
			echo $line >> /tmp/.newhtpasswd
		else
			awk -F: "/^`echo $line | cut -d: -f1`:/" $BWEB_CONF_DIR/htpasswd >> /tmp/.newhtpasswd
		fi
	done

	cp -f /tmp/.newhtpasswd $BWEB_CONF_DIR/htpasswd
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_reset_passwords" >&2
}

# check authentication and set bweb_auth_level / bweb_auth_user
# args:
#  minimum authentication level needed to view page
_bweb_auth_check() {
	local min_level=$1

	# logout?
	if [ "$QUERY_STRING" = "logout" ];
	then
		COOKIE_user=''
		COOKIE_session=''
		echo -e 'Set-Cookie: user=; path=/\r'
		echo -e 'Set-Cookie: session=; path=/\r'
	fi

	# redirect from error page?
	if [ "$QUERY_STRING" = "error" ];
	then
		bweb_auth_level=-1
	fi

	# username but no password ?
	if [ -n "$POST_user" -a -z "$POST_password" ];
	then
		bweb_auth_level=-1
	fi

	# login?
	if [ -n "$POST_user" -a -n "$POST_password" ];
	then
		set -- $($BWEB_GENID "$POST_user" "$(_bweb_auth_calc_hash $POST_user $POST_password)" "$REMOTE_ADDR")
		COOKIE_user="${POST_user:=dummy}"
		COOKIE_session="$2"
		if [ x"$COOKIE_session" = "x" ]; then
			COOKIE_session=$POST_password
		fi
	fi

	# have session?
	if [ -n "$COOKIE_user" -a -n "$COOKIE_session" ];
	then
		local raw_password=$(_bweb_auth_get_password "$COOKIE_user")
		local hashed_password=$(_bweb_auth_calc_hash "$COOKIE_user" $(_bweb_auth_get_password "$COOKIE_user"))
		# calculate ID for raw password and store in variables
		set -- $($BWEB_GENID "$COOKIE_user" "$raw_password" "$REMOTE_ADDR")
		local genid1=$1
		local genid2=$2
		# calculate ID for hashed password and store in positional variables
		set -- $($BWEB_GENID "$COOKIE_user" "$hashed_password" "$REMOTE_ADDR")

		# correct session?
		if [ "$COOKIE_session" = "$genid1" -o "$COOKIE_session" = "$genid2" -o "$COOKIE_session" = "$1" -o "$COOKIE_session" = "$2" ];
		then
			bweb_auth_user="$COOKIE_user"
			bweb_auth_level=$(_bweb_auth_get_level "$COOKIE_user")

			# password change request?
			if [ -n "$POST_newpassword" ];
			then
				_bweb_auth_set_password "$COOKIE_user" "$POST_newpassword"
				set -- $($BWEB_GENID "$COOKIE_user" "$(_bweb_auth_calc_hash "$COOKIE_user" $POST_newpassword)" "$REMOTE_ADDR")
			fi

			# need to update cookie?
			if [ -n "$POST_user" -o "$COOKIE_session" != "$2" ];
			then
				echo -e "Set-Cookie: user=$COOKIE_user; path=/\r"
				echo -e "Set-Cookie: session=$2; path=/\r"
			fi
		else
			# invalid session / wrong user/pass
			bweb_auth_level=-1
		fi
	fi

	# sufficient authentication?
	if [ $bweb_auth_level -lt $min_level ];
	then
		if [ "$SCRIPT_NAME" != "/cgi-bin/index.cgi" ];
		then
			echo -e "HTTP/1.1 302 Found\r\nLocation: http://$HTTP_HOST/?error\r\n\r"
			exit
		fi
		echo -e 'Set-Cookie: user=; path=/\r'
		echo -e 'Set-Cookie: session=; path=/\r'
	fi
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_check" >&2
}

# show authentication info in top bar (login field / currently logged in user)
_bweb_auth_show() {
	case "$bweb_auth_level" in
	0|-1)
		cat <<EOF
<form method="post">
Username <input type="text" name="user" value="admin" size="8">
Password <input name="password" type="password" size="8">
<input type="submit" value="Login"></form>
EOF
		# error?
		if [ $bweb_auth_level -lt 0 ];
		then
		cat <<EOF
<div class="ui-state-error ui-corner-all" style="padding: 0pt 0.7em;">
<p>
 <span class="ui-icon ui-icon-alert" style="float: left; margin-right: 0.3em;"></span>
<strong>Error: </strong>
EOF
		# login error?
		if [ -n "$POST_user" ];
		then
			echo 'Invalid username / password'
		else
			echo 'Session timed out'
		fi
		echo '</p></div>'
		fi
		;;

	*) echo "Logged in as <strong>$bweb_auth_user</strong> &nbsp; <a href='/?logout'>Logout</a>"

		;;

	esac
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_auth_show" >&2
}

_bweb_ajax_submit() {
  local cli=$1
  local op=`echo $2 | cut -d: -f 1`
  local label=`echo $2 | cut -d: -f 2`
  echo "      <$op cli=\"$cli\" label=\"$label\"/>"
..[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_ajax_submit" >&2
}

_bweb_extras() {
echo "  <div>"
echo "    <extras>"
local cli=$1
shift
local a
for a in "$@"
	do
		local op=${a%%:*}
		case $op in
		  ajax_submit)
			_bweb_ajax_submit $cli "$a";;
		esac
	done
	echo "    </extras>"
	echo "  </div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_extras" >&2
}

##############################################################################
# public api
##############################################################################

# output html header
# args:
#  minimum authentication level needed to view page (optional, default: -1)
bweb_upgrade_url() {
	upgrade_url=`head -1 /V2O/config/urlcache.txt | sed -e 's/^URL\s*\(.*\)$/\1/'`
	export upgrade_url
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_upgrade_url" >&2
}

bweb_header() {
	local min_level=${1:--1}

	_bweb_auth_check $min_level

	_bweb_signature
	cat $BWEB_TEMPLATE_DIR/ngs/header0.html | sed  -e s/DEVICETITLE/`hostname`/ | sed -e "s/SIGNATURE/${BWEB_SIGNATURE}/"

	_bweb_auth_show
	cat $BWEB_TEMPLATE_DIR/header1.html

	_bweb_menu
        _bweb_sidebar_button 1 "save_settings" "Save settings"

	cat $BWEB_TEMPLATE_DIR/header2.html
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_header" >&2
}

# output html footer
bweb_footer() {
	_bweb_signature
	echo "<input type='hidden' id='signature-info' value='${BWEB_SIGNATURE}'>"
	cat $BWEB_TEMPLATE_DIR/footer.html
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_footer" >&2
}
bweb_addbuttons() {
	echo '<div class="servererror hidden" ></div>';
	echo '<div class="formcontrol">';
	echo '<input type="button" id="form_cancel" class="cancel_button" value="Cancel" width="20px">'
	echo '<input type="submit" id="form_ok" class="ok_button" value="Apply" width="20px" >'
	echo '</div>';
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_addbuttons" >&2
}

_bweb_getupdatedconfig() {
	if [ ! -f /tmp/describe_$1.xml ]; then
		/V2O/bin/describe_$1  > /tmp/describe_$1.xml
		if [ x"$1" = "xif_list" ]; then
				grep -v LAN /tmp/describe_$1.xml \
				| grep -v enable | grep -v multicast | grep -v primary | \
				sed -e 's/List of Network Interfaces/Network Interface/' \
				> /tmp/scratch
			mv /tmp/scratch /tmp/describe_$1.xml
		elif [ x"$1" = "xtx_connection_parm" ]; then
			grep -v burst /tmp/describe_$1.xml |\
				grep -v threshold | grep -v slice > /tmp/scratch
			sed -e 's/txconnectionconf/TXCONNECTION/'  /tmp/scratch > /tmp/describe_$1.xml

		elif [ x"$1" = "xauth_table" ]; then
			grep -v burst /tmp/describe_$1.xml > /tmp/scratch
			mv /tmp/scratch /tmp/describe_$1.xml
		elif [ x"$1" = "xdisplay_parm" ]; then
			grep -v aspect /tmp/describe_$1.xml | grep -v res |\
				grep -v scheme | grep -v scale  | grep -v overlay |\
				sed -e 's/ Auto ,//' |\
				grep -v transparency | grep -v overlay> /tmp/scratch
			mv /tmp/scratch /tmp/describe_$1.xml
		elif [ x"$1" = "xrx_kbm_parm" ]; then
			grep -v local /tmp/describe_$1.xml > /tmp/scratch
			mv /tmp/scratch /tmp/describe_$1.xml
		elif [ x"$1" = "xxp_tx_video_parm" ]; then
			grep -v dvi_mode /tmp/describe_$1.xml |\
                        grep -v Charge > /tmp/scratch
			mv /tmp/scratch /tmp/describe_$1.xml
		fi
	fi
	if [ ! -f /tmp/get_$1.xml ]; then
		/V2O/bin/get_$1  > /tmp/get_$1.xml
	fi
	datafile=`grep ConfigFile /tmp/describe_$1.xml | cut -f2 -d'>' | cut -f1 -d'<'`
	if [ -f "${datafile}.txt" ]; then
		if [ "${datafile}.txt" -nt  /tmp/get_$1.xml ]; then
			/V2O/bin/get_$1  > /tmp/get_$1.xml
		fi
	elif [ -f "${datafile}_0.txt" ]; then
		if [ "${datafile}_0.txt" -nt  /tmp/get_$1.xml ]; then
			/V2O/bin/get_$1  > /tmp/get_$1.xml
		fi
	else
			/V2O/bin/get_$1  > /tmp/get_$1.xml
	fi
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_getupdatedconfig" >&2
}
bweb_getlistconfig() {
	echo "<div id='rightpanel'>"
	echo "</div>"
	echo "<div class='listform' style='display:none'>"
	echo "<div>"
	_bweb_getupdatedconfig $1
	if [ x"$1" = "xedid_list" ]; then
# Special code to handle lut list variables
		lutlist=""
		for i in `grep alutreg /tmp/get_lut_list.xml| cut -f4 -d'"'`
		do
			lutlist="${lutlist} $i, "
		done
        cat /tmp/describe_$1.xml |grep -v hpol |grep -v vpol > /tmp/scratch
        mv /tmp/scratch  /tmp/describe_$1.xml 
		sed -e "s/lut_list_alutreg ,/${lutlist}/" /tmp/describe_$1.xml > /tmp/describe_$1.xml.enum
		cat /tmp/describe_$1.xml.enum
	else
		cat /tmp/describe_$1.xml
	fi
	echo "</div>"
	echo "<div>"
	cat /tmp/get_$1.xml
	echo "</div>"
	echo "</div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_getlistconfig" >&2
}

bweb_getconfig() {
	echo "<div id='rightpanel'>"
	echo "</div>"
	echo "<div class='form' style='display:none'>"
	echo "<div>"
	_bweb_getupdatedconfig $1
	cat /tmp/describe_$1.xml
	echo "</div>"
	echo "<div>"
	cat /tmp/get_$1.xml
	echo "</div>"
	if [ $# -gt 1 ]; then
		_bweb_extras "$@"
	fi
	echo "</div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_getconfig" >&2
}

bweb_data() {
	echo "<div id='status-panel'>"
	echo "</div>"
	echo "<div class='status' style='display:none'>"
	echo "<div id='" $2 "'>"
	/V2O/bin/$1
	echo "</div>"
	echo "</div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_data" >&2
}

bweb_listdata() {
	echo "<div id='status-panel'>"
	echo "</div>"
	echo "<div class='status' style='display:none'>"
	echo "<div id='"  $2 "'>"
	/V2O/bin/$1 | sed -e 's/Command/cmd/g' | grep -v '<?xml'
	echo "</div>"
	echo "</div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_listdata" >&2
}

bweb_liststatus() {
	echo "<div id='status-panel'>"
	echo "</div>"
	echo "<div class='liststatus' style='display:none'>"
	echo "<div id='"  $2 "'>"
	/V2O/bin/$1 | grep -v Command | grep -v '<?xml'
	echo "</div>"
	echo "</div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_liststatus" >&2
}

bweb_cli() {
	/bin/rm -f /tmp/cmd.log
	/bin/rm -f /tmp/cmd.results
	for iv in `env | grep FORM_cli | sed -e 's/FORM_cli_//'`
	do
		i=`echo ${iv} | sed -e 's/=.*$//'`
		v=`echo ${iv} | sed -e 's/^.*=//'`
		if [ -z $v ]; then
			continue;
		fi
		idx=`env | grep FORM_${i}_index | sed -e 's/^.*=\(.*\)$/\1/'`
		del=`env | grep FORM_${i}_deletes | sed -e 's/^.*=\(.*\)$/\1/'`
		if (test "$idx" = "0")
			then
			cmdcli="/V2O/bin/add_$i"
		elif (test -n "$idx")
			then
			cmdcli="/V2O/bin/mod_$i $idx"
		else
			cmdcli="/V2O/bin/set_$i"
		fi
#		cmdcli=${cmdcli/_table/_entry}
#		cmdcli=${cmdcli/set_if_list/mod_if_list 1}
		cmdcli=`echo $cmdcli | sed -e s'/_table/_entry/'`
		cmdcli=`echo $cmdcli | sed -e s'/set_if_list/mod_if_list 1/'`
		unset FORM_${i}_index
		unset FORM_${i}_deletes
		cmd=""
		for j in `env |grep $i\_ |grep FORM |grep -v _cli_  | sed -e 's/ /_spaces_/g'`
		do
#			Skip over empty FORM_ env's
			if [ -n "${j##*=}" ]; then
				case "$j" in
					*_checkbox_*)
						cmdarg=${j#FORM_$i\_checkbox\_}
						if [ ${cmdarg##*=} == "on" ]; then
#							cmd="$cmd -${cmdarg//=on/  Yes} "
							cmdarg=`echo $cmdarg | sed -e 's/=on/  Yes/g'`
							cmd="$cmd -${cmdarg}"
						else
#							cmd="$cmd -${cmdarg//=off/  No} "
							cmdarg=`echo $cmdarg | sed -e 's/=off/  No/g'`
							cmd="$cmd -${cmdarg}"
						fi
						;;
					*)
						cmdarg=${j#FORM_$i\_}
#						tmparg=${cmdarg//_spaces_/\\ }
						tmparg=`echo $cmdarg | sed -e 's/_spaces_/\\ /g'`
						tmparg=`echo $tmparg | sed -e 's/=/ /g'`
#						cmd="$cmd -${tmparg//=/ } "
						cmd="$cmd -${tmparg} "
						;;
			esac
			fi
		done
		if [ $i == "ntp_client" ]; then 
		   i="ntp"
		fi
		
	        if [ $cmdcli == "/V2O/bin/set_ntp_client" ] ;then
	           cmdcli="/V2O/bin/set_ntp"
	        fi	
		
		echo "<clicmd name=\"$i\">" >> /tmp/cmd.results
		echo $cmdcli $cmd >> /tmp/cmd.log
		echo $cmdcli $cmd | /bin/ash | tee -a /tmp/cmd.results >> /tmp/cmd.log
		echo "</clicmd>" >> /tmp/cmd.results
#		Deletes must be done after in reverse order to maintain proper
#		indexes (JavaScript takes care of the reverse order part)
		oldIFS=$IFS
		IFS=','
		for delIdx in $del
		do
#			cmdcli="/V2O/bin/del_${i/_table/_entry} $delIdx"
			ic=`echo $i | sed -e 's/_table/_entry/'`
			cmdcli="/V2O/bin/del_${ic} $delIdx"
			echo $cmdcli >> /tmp/cmd.log
			echo $cmdcli | /bin/ash >> /tmp/cmd.log
		done
		IFS=$oldIFS
		echo /V2O/bin/get_${i} | /bin/ash > /tmp/get_${i}.xml
	done
#   Gather up all the command execution results and return to client for error display
	echo "<div id=\"apply-results\" style=\"display: none;\"><results>"
	cat /tmp/cmd.results
	echo "</results></div>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_cli" >&2
}

# HTML form handling. If authentication level is below minlevel, form elements
# will be disabled (grayed out) and saves ignored.

# get value of ppvar
# args:
#  ppvar
bweb_form_get() {
	cat $BWEB_PPFS_DIR/$1/value
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_get" >&2
}

# save (update) form element value to parpool (with a given value)
# args:
#  minlevel ppvar val
bweb_form_set() {
	local minlevel=$1
	local ppvar=$2
	local val=$3
	[ $bweb_auth_level -ge $minlevel ] && \
	echo -n $val > $BWEB_PPFS_DIR/$ppvar/value
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_set" >&2
}

# draw html select (combo box) form element
# args:
#  minlevel ppvar option1-name ... optionN-name
bweb_form_select() {
	local minlevel=$1
	local name=$(echo $2|sed 's/[^a-zA-Z0-9]\+/_/g')
	local val=$(bweb_form_get $2)
	local i=0
	local v=0

	shift 2

	echo -n "<select name=\"$name\""
	[ $bweb_auth_level -lt $minlevel ] && echo -n " disabled"
	echo ">"

	for i in "$@";
	do
	echo -n "  <option value=\"$v\""
	[ $v -eq $val ] && echo -n " selected"
	echo ">$i</option>"
	v=$(( $v + 1 ))
	done

	echo "</select>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_select" >&2
}

# draw html select (combo box) form element with name value pairs
# args:
#  minlevel ppvar option1-name option1-value ... optionN-name optionN-value
bweb_form_select_with_value() {
	local minlevel=$1
	local name=$(echo $2|sed 's/[^a-zA-Z0-9]\+/_/g')
	local val=$(bweb_form_get $2)

	shift 2

	echo -n "<select name=\"$name\""
	[ $bweb_auth_level -lt $minlevel ] && echo -n " disabled"
	echo ">"

	while [ -n "$*" ]
	do
		echo -n "  <option value=\"$2\""
		[ $2 -eq $val ] && echo -n " selected"
		echo ">$1</option>"
		shift 2
	done

	echo "</select>"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_select_with_value" >&2
}

# draw html checkbox form element
# args:
#  minlevel ppvar disableval enableval
bweb_form_checkbox() {
	local minlevel=$1
	local name=$(echo $2|sed 's/[^a-zA-Z0-9]\+/_/g')
	local val=$(bweb_form_get $2)
	local disableval=${3:-0}
	local enableval=${4:-1}

	echo "<input type=\"hidden\" name=\"$name\" value=\"$disableval\">"
	echo -n "<input type=\"checkbox\" name=\"$name\" value=\"$enableval\""
	[ $bweb_auth_level -lt $minlevel ] && echo -n " disabled"
	[ $val -eq $enableval ] && echo -n " checked"
	echo ">"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_checkbox" >&2
}

# draw html text form element
# args:
#  minlevel ppvar
bweb_form_text() {
	local minlevel=$1
	local name=$(echo $2|sed 's/[^a-zA-Z0-9]\+/_/g')
	local val=$(bweb_form_get $2)

	echo -n "<input type=\"text\" name=\"$name\" value=\"$val\""
	[ $bweb_auth_level -lt $minlevel ] && echo -n " disabled"
	echo ">"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_text" >&2
}

# draw html password form element
# args:
#  minlevel ppvar
bweb_form_password() {
	local minlevel=$1
	local name=$(echo $2|sed 's/[^a-zA-Z0-9]\+/_/g')
	local val=$(bweb_form_get $2)

	echo -n "<input type=\"password\" name=\"$name\" value=\"$val\""
	[ $bweb_auth_level -lt $minlevel ] && echo -n " disabled"
	echo ">"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_password" >&2
}

# save (update) form element value to parpool if set
# args:
#  minlevel ppvar
bweb_form_save() {
	local minlevel=$1
	local ppvar=$2
	local name=$(echo $ppvar|sed 's/[^a-zA-Z0-9]\+/_/g')
	eval local val=$"FORM_$name"

	# only try to save if new value is provided
	env | egrep -q "^FORM_$name=" && bweb_form_set $minlevel $ppvar "$val"
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_save" >&2
}

# execute form element ppapply to parpool
# args:
#  minlevel ppvar
bweb_form_execute() {
	local minlevel=$1
	local ppvar=$2

	[ $bweb_auth_level -ge $minlevel ] && \
	echo -n 1 > $BWEB_PPFS_DIR/$ppvar/execute
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_form_execute" >&2
}

# allow platform to override defaults if needed
[ -r $BWEB_CONF_DIR/bweb.sh ] && . $BWEB_CONF_DIR/bweb.sh

bweb_locate_res '/usr/share/bweb/' 'bweb' '.sh'
if [ -n "$bweb_resource" ]; then
  source $bweb_resource
fi

