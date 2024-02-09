#ifndef __LEGACY_V2D_INCLUDED__
#define __LEGACY_V2D_INCLUDED__


#define __NO_FPGA__


#ifdef __KERNEL__

/********************************************
 * Function prototpyes for fpga_v2          *
 ********************************************/
int v2_fpga_hardware_init(struct v2d_board *board);
int v2_fpga_hardware_exit(struct v2d_board *board);


/********************************************
 * Function prototpyes for fpga_control.c *
 ********************************************/
int video_available(struct v2d_board *board);
int fpga_control_init(struct v2d_board *board);
void fpga_control_exit(struct v2d_board *board);

/********************************************
 * Function prototpyes for kbm_control.c *
 ********************************************/
int kbm_control_init(struct v2d_board *board);
int kbm_control_init2(struct v2d_board *board);
void kbm_control_exit(struct v2d_board *board);


/********************************************
 * Function prototpyes for flow_control.c *
 ********************************************/
int flow_control_init(struct v2d_board *board);
int flow_control_init2(struct v2d_board *board);
void flow_control_exit2(struct v2d_board *board);
void flow_control_exit(struct v2d_board *board);


#endif /* __KERNEL__ */



#endif /* __LEGACY_V2D_INCLUDED__ */
