#!/bin/sh
# barco web interface functions
# (C) 2009-2011 Peter Korsgaard <peter.korsgaard@barco.com>

# version number
BWEB_VERSION=0.0.3

# location of html/menu templates
BWEB_TEMPLATE_DIR=/var/www/templates

# parpoolfs mount point
BWEB_PPFS_DIR=/mnt/fuse/parpool

# bweb configuration directory
BWEB_CONF_DIR=/etc/bweb

# default htpasswd file to reset to
BWEB_DEFAULT_PASSWD=/mnt/rom-etc/htpasswd

# additional message to display when password is changed
BWEB_EXTRA_PWD_CHANGE_MSG=""

# session id generator
BWEB_GENID=/usr/bin/bweb-genid

# authentication level
bweb_auth_level=0

# authenticated user if any
bweb_auth_user=''

# Parse menu description and output HTML/javascript menu.
#
# Menu description has form:
# minauth level url text (where url == '-' means no link)
#
# url can be a shell expression. Remember to quote it if it contains spaces.
# line will be skipped if url expands to the empty string.
#
_bweb_menu() {
	echo "<ul id='menu'>"
	local prevlevel=1
	local line=1
	grep -v '^#' $BWEB_TEMPLATE_DIR/menu.txt |
	while read row;
	do
	# split according to quotes
	eval set -- $row
	minauth=$1
	level=$2
	eval url=$3
	shift 3
	text="$*"

	# skip lines with empty urls or requiring higher access
	[ -n "$url" -a $bweb_auth_level -ge $minauth ] || continue
	while [ $prevlevel -gt $level ];
	do
		echo "</ul></li>"
		prevlevel=$(( $prevlevel - 1 ))
	done

	# barco style strangely enough clips at level=2
	[ $level -gt 2 ] && level=2

	if [ "$url" == "-" ];
	then
		name=$line$(echo "text"|sed 's/[^a-zA-Z0-9]/_/g');
		echo "<li class='menulevel${level}close' id='menu${name}'>"
		echo "<a href=\"javascript:toggle('menu${name}');\">$text</a>"
		echo "<ul id='menu${name}1' class='hide'>"
	else
		css=''
		# /cgi-bin/index.cgi should be treated as / for busybox httpd
		cururl=${SCRIPT_NAME%%cgi-bin/index.cgi}
		[ "$url" == "$cururl" ] && css=" style='font-weight:bold'"
		echo "<li class='menulevel${level}link'><a href='$url'$css>$text</a></li>"
	fi
	prevlevel=$level
	line=$(( $line + 1 ));
	done

	while [ $prevlevel -gt 1 ];
	do
	echo "</ul></li>"
	prevlevel=$(( $prevlevel - 1 ))
	done

	echo "</ul>"
	echo "<script type='text/javascript' src='/barcomenu.js'></script>"
}

# get auth level for user
# args:
#  username
# can be overwritten for custom passwd storage
_bweb_auth_get_level() {
	awk -F: "/^$1:/ { print \$2 }" $BWEB_CONF_DIR/htpasswd
}

# get (hashed ?) password for user
# args:
#  username
# can be overwritten for custom passwd storage
_bweb_auth_get_password() {
	awk -F: "/^$1:/ { p=\$3; for (i=4; i<=NF; i++) p=p \":\" \$i; print p }" $BWEB_CONF_DIR/htpasswd
}

# calculate password hash
# args:
#  username password
# can be overwritten for custom hash calculation; default is "plain text"
_bweb_auth_calc_hash() {
	echo -n "$2"
}

# set password for user
# args:
#  username unhashed_password
# can be overwritten for custom passwd storage
_bweb_auth_set_password() {
	sed -i "s/^\($1:[0-9]*:\).*/\1$(_bweb_auth_calc_hash $1 $2)/" $BWEB_CONF_DIR/htpasswd
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
		echo "Got Here 1.1 " ${COOKIE_session} >> /tmp/log
		if [ x"$COOKIE_session" = "x" ]; then
			COOKIE_session=$POST_password
		fi
	fi
	echo "Got Here 2.0  "  ${POST_password}>> /tmp/log

	# have session?
	if [ -n "$COOKIE_user" -a -n "$COOKIE_session" ];
	then
		local raw_password=$(_bweb_auth_get_password "$COOKIE_user")
		local hashed_password=$(_bweb_auth_calc_hash "$COOKIE_user" $(_bweb_auth_get_password "$COOKIE_user"))
	echo "Got Here 2.1"  $raw_password>> /tmp/log
		# calculate ID for raw password and store in variables
		set -- $($BWEB_GENID "$COOKIE_user" "$raw_password" "$REMOTE_ADDR")
		local genid1=$1
		local genid2=$2
	echo "Got Here 2.2 genid1 " ${genid1} >> /tmp/log
	echo "Got Here 2.3 genid2 " ${genid2} >> /tmp/log
		# calculate ID for hashed password and store in positional variables
		set -- $($BWEB_GENID "$COOKIE_user" "$hashed_password" "$REMOTE_ADDR")

	echo "Got Here 2.3.1 cookie session " ${COOKIE_session} >> /tmp/log
	echo "Got Here 2.3.2 -1 " ${1} >> /tmp/log
	echo "Got Here 2.3.3 -2 " ${2} >> /tmp/log
		# correct session?
		if [ "$COOKIE_session" = "$genid1" -o "$COOKIE_session" = "$genid2" -o "$COOKIE_session" = "$1" -o "$COOKIE_session" = "$2" ];
		then
	echo "Got Here 2.4 " >> /tmp/log
			bweb_auth_user="$COOKIE_user"
			bweb_auth_level=$(_bweb_auth_get_level "$COOKIE_user")

			# password change request?
			if [ -n "$POST_newpassword" ];
			then
	echo "Got Here 2.5 " >> /tmp/log
				_bweb_auth_set_password "$COOKIE_user" "$POST_newpassword"
				set -- $($BWEB_GENID "$COOKIE_user" "$(_bweb_auth_calc_hash "$COOKIE_user" $POST_newpassword)" "$REMOTE_ADDR")
			fi

			# need to update cookie?
			if [ -n "$POST_user" -o "$COOKIE_session" != "$2" ];
			then
	echo "Got Here 2.6 " >> /tmp/log
				echo -e "Set-Cookie: user=$COOKIE_user; path=/\r"
				echo -e "Set-Cookie: session=$2; path=/\r"
			fi
		else
			# invalid session / wrong user/pass
			bweb_auth_level=-1
		fi
	fi
	echo "Got Here 3.0 "  $bweb_auth_level>> /tmp/log
	bweb_auth_level=1

	# sufficient authentication?
	if [ $bweb_auth_level -lt $min_level ];
	then
	echo "Got Here 3.1 " >> /tmp/log
		if [ "$SCRIPT_NAME" != "/cgi-bin/index.cgi" ];
		then
			echo -e "HTTP/1.1 302 Found\r\nLocation: http://$HTTP_HOST/?error\r\n\r"
			exit
		fi
		echo -e 'Set-Cookie: user=; path=/\r'
		echo -e 'Set-Cookie: session=; path=/\r'
	fi
}

# show authentication info in top bar (login field / currently logged in user)
_bweb_auth_show() {
	echo "Got Here 4.0 "  ${bweb_auth_level} >> /tmp/log
	case "$bweb_auth_level" in
	0|-1)
		cat <<EOF
<form method="post">
Username <input type="text" name="user" size="8">
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
}

##############################################################################
# public api
##############################################################################

# output html header
# args:
#  minimum authentication level needed to view page (optional, default: -1)
bweb_header() {
	local min_level=${1:--1}

	_bweb_auth_check $min_level

	cat $BWEB_TEMPLATE_DIR/header0.html

	_bweb_auth_show

	cat $BWEB_TEMPLATE_DIR/header1.html

	_bweb_menu

	cat $BWEB_TEMPLATE_DIR/header2.html
}

# output html footer
bweb_footer() {
	cat $BWEB_TEMPLATE_DIR/footer.html
}
bweb_getconfig() {
	/V2O/bin/get_$1 > /tmp/x.xml
	/V2O/bin/describe_$1 > /tmp/y.xml
	/usr/bin/xsltproc /etc/scripts/xsl/get_status.xsl /tmp/x
}

bweb_data() {
	/V2O/bin/$1 > /tmp/x
	/usr/bin/xsltproc /etc/scripts/xsl/get_status.xsl /tmp/x
}

bweb_listdata() {
	/V2O/bin/$1 > /tmp/x
	/usr/bin/xsltproc /etc/scripts/xsl/get_liststatus.xsl /tmp/x
}

# HTML form handling. If authentication level is below minlevel, form elements
# will be disabled (grayed out) and saves ignored.

# get value of ppvar
# args:
#  ppvar
bweb_form_get() {
	cat $BWEB_PPFS_DIR/$1/value
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
}

# execute form element ppapply to parpool
# args:
#  minlevel ppvar
bweb_form_execute() {
	local minlevel=$1
	local ppvar=$2

	[ $bweb_auth_level -ge $minlevel ] && \
	echo -n 1 > $BWEB_PPFS_DIR/$ppvar/execute
}

# allow platform to override defaults if needed
[ -r $BWEB_CONF_DIR/bweb.sh ] && . $BWEB_CONF_DIR/bweb.sh

