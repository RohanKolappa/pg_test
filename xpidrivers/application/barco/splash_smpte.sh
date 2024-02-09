# This script enables a 2560x1600@60Hz SMPTE pattern on an NGS-D200 in bringup mode
PLINK="/home/rkale/bin/plink -ssh " 
PSCP="/home/rkale/bin/pscp -scp -v " 
USER="root"
PASSWD="-pw 0at3n" 
h_res=2560
v_res=1600

#hreg_59=0.59*v_res
hreg_59=944
#hreg_8=0.08*v_res
hreg_8=128
#hreg_25=0.25*v_res
hreg_25=400

#wreg_12p5=0.125*h_res
wreg_12p5=328
#wreg_10p7=0.107*h_res
wreg_10p7=272
#wreg_64p2=0.642*h_res
wreg_64p2=1632
#wreg_15p75=0.1575*h_res 403.2
wreg_15p75=404
#wreg_21=0.21*h_res
wreg_21=538
#wreg_10p5=0.105*h_res
wreg_10p5=266
#wreg_3p4=0.034*h_res
wreg_3p4=86

#ramp_step=wreg_64p2/255
ramp_step=7

check_computations() {
	fail=0
	sum_hreg=`expr ${hreg_59} + 2 \* ${hreg_8} + ${hreg_25}`
	sum_vreg1=`expr 2 \* ${wreg_12p5} + 7 \* ${wreg_10p7}`
	sum_vreg2=`expr 2 \* ${wreg_12p5} + ${wreg_10p7} + ${wreg_64p2}`
	sum_vreg3=`expr 2 \* ${wreg_12p5} + ${wreg_15p75} + ${wreg_21} + 2 \* ${wreg_10p5} + 5 \* ${wreg_3p4}`

	if [ ${sum_hreg} != ${v_res} ] ; then
		echo "sum_hreg = ${sum_hreg} while v_res = ${v_res}"
		fail=`expr ${fail} + 1`
	fi
	
	if [ ${sum_vreg1} != ${h_res} ] ; then
		echo "sum_vreg1 = ${sum_vreg1} while h_res = ${h_res}"
		fail=`expr ${fail} + 1`
	fi
	
	if [ ${sum_vreg2} != ${h_res} ] ; then
		echo "sum_vreg2 = ${sum_vreg2} while h_res = ${h_res}"
		fail=`expr ${fail} + 1`
	fi
	
	if [ ${sum_vreg3} != ${h_res} ] ; then
		echo "sum_vreg3 = ${sum_vreg3} while h_res = ${h_res}"
		fail=`expr ${fail} + 1`
	fi

	if [ ${fail} != 0 ] ; then
		echo "Exiting.."
		exit 1
	fi
}	

program_registers() {
	echo "Now programming Fanout FPGA registers.."
/V2O/bin/maint dreg -6 -r 0x21c -W ${hreg_59}
/V2O/bin/maint dreg -6 -r 0x220 -W ${hreg_8}
/V2O/bin/maint dreg -6 -r 0x224 -W ${hreg_25}
/V2O/bin/maint dreg -6 -r 0x228 -W ${ramp_step}
/V2O/bin/maint dreg -6 -r 0x22c -W ${wreg_12p5}
/V2O/bin/maint dreg -6 -r 0x230 -W ${wreg_10p7}
/V2O/bin/maint dreg -6 -r 0x234 -W ${wreg_64p2}
/V2O/bin/maint dreg -6 -r 0x238 -W ${wreg_15p75}
/V2O/bin/maint dreg -6 -r 0x23c -W ${wreg_21}
/V2O/bin/maint dreg -6 -r 0x240 -W ${wreg_10p5}
/V2O/bin/maint dreg -6 -r 0x244 -W ${wreg_3p4}
}

check_computations
program_registers
