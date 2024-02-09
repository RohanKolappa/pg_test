#!/bin/sh
SCRIPTS_DIR=/etc/migration
ver=2.34.8
DEVICEXML=/etc/config/Device.xml
OLDFILE='/tmp/apply_old'
NEWFILE='/tmp/apply_new'
SETTMPSH='/tmp/setclicmd.xml'
TMPSH='/tmp/getclicmd.sh'
TMPOUT='/tmp/getcliout.xml'
STAGE1='/tmp/stagefirst.xsl'
STAGE2='/tmp/stagesecond.xsl'
DELTACONFIG='/tmp/testconfig.xml'

xsltproc -o ${DEVICEXML}.tmp ${SCRIPTS_DIR}/$ver/xsl/Device.xsl ${DEVICEXML}

xsltproc -o ${OLDFILE}.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl ${DEVICEXML}` /etc/scripts/xsl/deviceconfig.xsl  /home/onair/objectXml/confighandler.xml
xsltproc ${OLDFILE}.xsl ${DEVICEXML} | sed -e 's/false/No/gI' | sed -e 's/true/Yes/gI' > ${OLDFILE}.sh

xsltproc ${OLDFILE}.xsl ${DEVICEXML}.tmp | sed -e 's/false/No/gI' | sed -e 's/true/Yes/gI' > ${NEWFILE}.sh

#xsl that gives me the clis for delta
diff /tmp/apply_old.sh /tmp/apply_new.sh | sed -n '/^+[^+]/p'| cut -d "+" -f 2 |cut -d ' ' -f 1 | sed -n '/^set/p' > ${SETTMPSH}
sed  -e 's/^/<CLI>/' -i  ${SETTMPSH}
sed -e 's/$/<\/CLI>/'  -i ${SETTMPSH}
sed -i '1i <CLIS>' $SETTMPSH
echo '</CLIS>' >> $SETTMPSH
diff /tmp/apply_old.sh /tmp/apply_new.sh | sed -n '/^+[^+]/p'| cut -d "+" -f 2 |cut -d ' ' -f 1 |sed -e 's/set/get/g' | sed -n '/^get/p' > ${TMPSH}
/bin/rm -f ${TMPOUT}
sh ${TMPSH} >> ${TMPOUT}
sed -i '1i <Groups>' $TMPOUT
echo '</Groups>' >> $TMPOUT

mv ${DEVICEXML}.tmp $DEVICEXML
xsltproc -o ${DELTACONFIG} --stringparam getrequestfile /tmp/getcliout.xml --stringparam setrequestfile ${SETTMPSH} /${SCRIPTS_DIR}/$ver/xsl/convertFormat.xsl /tmp/Device.xml

xsltproc -o ${STAGE1} --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/devicesaveconfig.xsl  /home/onair/objectXml/confighandler.xml


sed -ie 's/mod_/add_/' ${DELTACONFIG}
xsltproc ${STAGE1} ${DELTACONFIG} | sed -e 's/No_BOOL_/false/g' | sed -e 's/Yes_BOOL_/true/g' | sed -e 's/Both_BOOL_/Both/g'  > ${STAGE2}
xsltproc -o ${DEVICEXML}.tmp ${STAGE2} ${DEVICEXML}
mv ${DEVICEXML}.tmp $DEVICEXML

/bin/rm ${TMPOUT} ${TMPSH} ${STAGE2} ${STAGE1} ${OLDFILE}.xsl ${OLDFILE}.sh ${NEWFILE}.sh  ${SETTMPSH} ${DELTACONFIG}
