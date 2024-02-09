#!/bin/sh

# Source common library 
. /etc/scripts/common.rc

_PARAM_REPORT_TYPE=$1
_PARAM_OUTPUT_FILE=$2	 #-- The output file refers to a path in the workarea 
_PARAM_LIVE_AREA=$3
_PARAM_WORK_AREA=$4
_PARAM_PREVIOUS_TIME_STAMP=$5
_PARAM_CURRENT_TIME_STAMP=$6

_DIR_LIVE="${_PARAM_LIVE_AREA}"
_DIR_WORK_AREA="${_DIR_LIVE}/${_PARAM_WORK_AREA}"

_FILE_LIVE_REPORT_INPUT="${_DIR_LIVE}/${_PARAM_REPORT_TYPE}.log"
_FILE_LIVE_REPORT_XSL="${_DIR_LIVE}/${_PARAM_REPORT_TYPE}.xsl"
_FILE_WORK_AREA_REPORT_INPUT="${_DIR_WORK_AREA}/${_PARAM_REPORT_TYPE}.log"
_FILE_WORK_AREA_REPORT_XSL="${_DIR_WORK_AREA}/${_PARAM_REPORT_TYPE}.xsl"
_FILE_WORK_AREA_REPORT_OUTPUT="${_DIR_WORK_AREA}/${_PARAM_OUTPUT_FILE}"
_FILE_AUDIT_EVENT_LOG="/data/reports/auditevent/audit.*"
_FILE_TMP_AUDIT_REPORT="${_DIR_WORK_AREA}/audit.tmp"
_FILE_TRIMMED_AUDIT_REPORT="${_DIR_WORK_AREA}/trimmed_audit.tmp"

AUDIT_REPORT_TAG="AuditReport"
POLICY_REPORT_TAG="PolicyReport"

setupWorkarea() 
{	
    # Create work area directory along with its parent directories if they don't exist
    /bin/mkdir -p ${_DIR_WORK_AREA} 

    # Cleanup the workarea directory if there are any temporary files
    /bin/rm -f ${_FILE_WORK_AREA_REPORT_INPUT}
    /bin/rm -f ${_FILE_WORK_AREA_REPORT_XSL}
    /bin/rm -f ${_FILE_WORK_AREA_REPORT_OUTPUT}

    # Move the necessary files to the work area
    if [ ${_PARAM_REPORT_TYPE} = "audit" ]; then
	/bin/cp -f ${_FILE_AUDIT_EVENT_LOG} ${_DIR_LIVE}
        /bin/ls ${_FILE_LIVE_REPORT_INPUT}.1
        if [ "x$?" = "x0" ]; then
            /bin/cat ${_FILE_LIVE_REPORT_INPUT}.1 >> ${_FILE_WORK_AREA_REPORT_INPUT}
        fi
        /bin/cat ${_FILE_LIVE_REPORT_INPUT} >> ${_FILE_WORK_AREA_REPORT_INPUT}
    else
        /bin/mv -f ${_FILE_LIVE_REPORT_INPUT} ${_FILE_WORK_AREA_REPORT_INPUT}
    fi

    if [ "x$?" != "x0" ]; then
    	/bin/touch ${_FILE_WORK_AREA_REPORT_INPUT}
        if [ ${_PARAM_REPORT_TYPE} = "policy" ]; then
	    echo "<${POLICY_REPORT_TAG}/>" >> ${_FILE_WORK_AREA_REPORT_INPUT}
        fi
	#-- The reason for removing this file is when a blank log file is generated and we apply proper xslt on it then the output file is not being generated 
	#-- This could be because of improper xsl format
        /bin/rm -f ${_FILE_LIVE_REPORT_XSL}
    fi

    /bin/ls ${_FILE_LIVE_REPORT_XSL}
    if [ "x$?" = "x0" ]; then
        /bin/mv -f ${_FILE_LIVE_REPORT_XSL} ${_FILE_WORK_AREA_REPORT_XSL}    
    else	
        /bin/touch ${_FILE_WORK_AREA_REPORT_XSL}
        echo "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">" >> ${_FILE_WORK_AREA_REPORT_XSL}
        echo "</xsl:stylesheet>" >> ${_FILE_WORK_AREA_REPORT_XSL}
    fi
}

generateReportAndExport()
{
    if [ ${_PARAM_REPORT_TYPE} = "audit" ]; then    
        if [ ${_PARAM_WORK_AREA} = "exportToFTPServer" ]; then
            while read line; do
                currDate=`echo $line | sed -e "s/.* date='\(.*\)' time=.*/\1/"`
		currTime=`echo $line | sed -e "s/.* time='\(.*\)' level=.*/\1/"`
		transformedDate=`echo $currDate | sed -e "s/^\(.*\)\/\(.*\)\/\(.*\)/20\3.\1.\2/"`                
		timestamp=`date -d "$transformedDate-$currTime" +%s`
                if [ $timestamp -ge ${_PARAM_PREVIOUS_TIME_STAMP} ] &&  [ $timestamp -le ${_PARAM_CURRENT_TIME_STAMP} ]; then
                    echo $line >> ${_FILE_TRIMMED_AUDIT_REPORT}
                fi
            done < ${_FILE_WORK_AREA_REPORT_INPUT}

            #- Move the extracted text file back to audit.log present in the workarea
            /bin/ls ${_FILE_TRIMMED_AUDIT_REPORT}
            if [ "x$?" != "x0" ]; then
                /bin/touch ${_FILE_TRIMMED_AUDIT_REPORT}  
            fi
	    /bin/mv -f ${_FILE_TRIMMED_AUDIT_REPORT} ${_FILE_WORK_AREA_REPORT_INPUT}
        fi

	# Move the contents of audit.log into an empty log file and append start and end tags to it
        echo "<${AUDIT_REPORT_TAG}>" > ${_FILE_TMP_AUDIT_REPORT}
	/bin/cat ${_FILE_WORK_AREA_REPORT_INPUT} >> ${_FILE_TMP_AUDIT_REPORT}
        echo "</${AUDIT_REPORT_TAG}>" >> ${_FILE_TMP_AUDIT_REPORT}

	# Rename audit.tmp to audit.log
        /bin/rm -f ${_FILE_WORK_AREA_REPORT_INPUT}
        /bin/mv -f ${_FILE_TMP_AUDIT_REPORT} ${_FILE_WORK_AREA_REPORT_INPUT}
    fi

    # Run xsltproc to generate the user-defined format report
    xsltproc --output ${_FILE_WORK_AREA_REPORT_OUTPUT} ${_FILE_WORK_AREA_REPORT_XSL} ${_FILE_WORK_AREA_REPORT_INPUT}

    if [ "x$?" != "x0" ]; then
	logprint "xsltproc command failed. Report generation process terminated."
	return 1
    fi
   
    return 0
}

setupWorkarea
generateReportAndExport
