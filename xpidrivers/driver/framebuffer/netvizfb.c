/*
 * netvizfb.c: framebuffer driver for NetViz
 *
 * based on thinxfb by Peter Korsgaard <peter.korsgaard@barco.com>
 * adapted (hacked) for NetViz by Bernard Willaert
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include "netvizfb.h"
//#include <linux/async_tx.h>
#include <linux/delay.h>


#define REG_BASEADDR	0x00
#define REG_LUT_INDEX	0x04
#define REG_LUT_DATA	0x08
#define REG_CONTROL	0x0c
#define REG_HOR_CURSOR	0x10
#define REG_VER_CURSOR	0x14
#define REG_OSDADDR	0x18
#define REG_OSDDATA	0x1c

#define CTRL_ENABLE	0x1
#define CTRL_TRANSP	0x2
#define CTRL_BACKGRND	0x4

/* Fixed hardware settings */
#define XRES_VIRTUAL	2048
#define YRES_VIRTUAL	4096

/* Screen resolution */
#define XRES_VISIBLE	XRES_VIRTUAL
#define YRES_VISIBLE	YRES_VIRTUAL

#define BITS_PER_PIXEL	16

#define MDMA_CHANNEL 0



struct netvizfb_info {
	struct fb_info *info;
	struct _OMNITEK_INTERFACE_EXTENSION * pExt;
	PResource pDMAChannel;
	void* dummy_page;
	struct device *dev;

//	void __iomem *base;
//	u32 ctrlreg; /* write only */
};

static inline void netvizfb_setreg(struct netvizfb_info *fbi,
				  u32 reg, u32 data)
{
//	printk("t\txxx netvizfb_setreg called \n");
#if 0
	if (reg == REG_CONTROL)
		fbi->ctrlreg = data;
	out_be32(fbi->base + reg, data);
#endif
}

static inline u32 netvizfb_getreg(struct netvizfb_info *fbi, u32 reg)
{
//	printk("\t\txxx netvizfb_getreg called \n");
//	return in_be32(fbi->base + reg);
	return 0;
}

static int netvizfb_set_par(struct fb_info *info)
{
//	printk("\t\txxx netvizfb_set_par called \n");
#if 0
	struct netvizfb_info *fbi = info->par;

	netvizfb_setreg(fbi, REG_HOR_CURSOR,
			   (info->var.xres - 1) | (info->var.left_margin << 16));
	netvizfb_setreg(fbi, REG_VER_CURSOR,
			   (info->var.yres - 1) | (info->var.upper_margin << 16));
#endif
	return 0;
}


static int netvizfb_setcolreg(unsigned regno, unsigned red,
				 unsigned green, unsigned blue,
				 unsigned transp, struct fb_info *info)
{
//	printk("\t\txxx netvizfb_setcolreg called \n");
#if 0
	struct netvizfb_info *fbi = info->par;

	if (regno > 255)
		return -EINVAL;

	/* We only handle 8 bits of each color. */
	red >>= 8;
	green >>= 8;
	blue >>= 8;

	netvizfb_setreg(fbi, REG_LUT_INDEX, regno | 0x100);
	netivzfb_setreg(fbi, REG_LUT_DATA, red|(green<<8)|(blue<<16));
#endif
	return 0;
}

static int netvizfb_setcmap(struct fb_cmap *cmap, struct fb_info *info)
{
//	printk("\t\txxx netvizfb_setcmap called \n");
	return 0;
}


static int netvizfb_ioctl(struct fb_info *info, unsigned int cmd,
				unsigned long arg)
{

//	struct netvizfb_info *fbi = info->par;
//	u32 tmp = fbi->ctrlreg;

	printk("\t\txxx got IOCTL: 0x%08X \n", cmd);

	switch (cmd)
	{
		case FBIOPUT_VSCREENINFO :
		{
			u32 copy_result;
			copy_result = copy_from_user(&info->var, (void __user*)arg, sizeof(info->var));
			break;
		}

		case FBIO_TRANSPARANCY_ENABLE:
		{
			break;
		}

		case FBIO_BACKGROUND_ENABLE:
		{
			break;
		}

		default:
			return -EINVAL;
	}

//	netvizfb_setreg(fbi, REG_CONTROL, tmp);

	return 0;
}


static int netvizfb_check_var(struct fb_var_screeninfo *var,
				 struct fb_info *info)
{
//	printk("\t\txxx netvizfb_check_var called \n");
	if (var->xres > XRES_VIRTUAL || var->yres > YRES_VIRTUAL)
		return -EINVAL;

	return 0;
}



/* update 'lines' output from 'starty' */
static void netvizfb_refresh(struct netvizfb_info *fbi, int starty, int lines)
{
	unsigned long line_length = fbi->info->fix.line_length;
	struct file  filp;
	ssize_t data_transfered, size;
	loff_t offset;
	int pages = 0;
	dma_addr_t dma_addr = 0;
	ssize_t count = PAGE_SIZE;

	/* sanity checks */
	if (starty >= fbi->info->var.yres_virtual)
	{
		printk(KERN_ERR DRIVER_NAME ": Transaction start line [%d] is larger than maximum number of lines [%d]\n",starty, YRES_VIRTUAL);
		return;
	}
	offset = starty * line_length;

	size = lines*line_length;
	if (offset + size > fbi->info->fix.smem_len)
	{
		printk(KERN_ERR DRIVER_NAME ": Transaction size + offset [%u] exceeds framebuffer size [%u]\n", (unsigned int)offset+size , (unsigned int)fbi->info->fix.smem_len);

	}
//	printk("\t\txxx netvizfb_refresh called - starty=%u lines=%u offset=%u size=%u\n", starty, lines, offs, size);

	/* initate a DMA transfer */
	filp.private_data = fbi->pDMAChannel;
	/* calculate the number of pages needed */
	/* take into account transfers not starting at page borders*/
	if (offset % PAGE_SIZE)
	{
		offset = (offset >> PAGE_SHIFT) << PAGE_SHIFT;
		pages++;
	}

	while (pages--)
	{
		dma_addr = dma_map_page(fbi->dev, vmalloc_to_page(fbi->info->screen_base + offset), 0, count, DMA_TO_DEVICE);
		data_transfered = OmniTek_MDMA_dev_transfer (
														&filp,
														0,
														dma_addr,
														count,
														&offset,
														true,
														true
													);

		if (data_transfered != count)
		{
			printk(KERN_ERR DRIVER_NAME ": Transaction error: [%d] bytes transfered instead of [%d]!\n",data_transfered, size);
		}
		else
		{
			printk("\t\tnetvizfb_refresh: data_transfered=0x%08X - count=0x%08X\n", (unsigned int)data_transfered, (unsigned int)size );
		}
		offset += count;
	}



}


static void netvizfb_deferred_io(struct fb_info *info,
				struct list_head *pagelist)
{

	struct netvizfb_info *fbi = info->par;
	struct file  filp;
	struct page *page = 0, *firstpage = 0;
	ssize_t data_transfered = 0;
	ssize_t count = PAGE_SIZE;
	loff_t  offset = 0;
	dma_addr_t dma_addr =0 ;
	dma_addr_t old_dma_addr = 0, start_dma_addr = 0;
	ssize_t accu_count = 0;
	bool first = true;

	filp.private_data = fbi->pDMAChannel;

	// Combine contiguous pages into one DMA action
	list_for_each_entry(page, pagelist, lru)
	{
		dma_addr = dma_map_page(fbi->dev, page, 0, count, DMA_TO_DEVICE);
		accu_count += PAGE_SIZE;
		if (first)
		{
			// Very first page of a possible contiguous list
			offset = page->index << PAGE_SHIFT;
			firstpage = page;
			first = false;
		}
		else
		{
			// Not the first page of a possible contiguous list -- or total size to be DMA'd > 1MB
			if ((dma_addr < old_dma_addr) || ((dma_addr-old_dma_addr ) != PAGE_SIZE) )
			{
				accu_count -= PAGE_SIZE;
			//	printk ("\t\taccu_count=%d dma_addr=0x%08X old_dma_addr=0x%08X\n",accu_count,dma_addr,old_dma_addr);
				start_dma_addr = dma_map_page(fbi->dev, firstpage, 0, accu_count, DMA_TO_DEVICE);
				data_transfered = OmniTek_MDMA_dev_transfer (
															&filp,
															0,
															start_dma_addr,
															accu_count ,
															&offset,
															true,
															true
														);
				if (data_transfered != accu_count)
				{
					printk(KERN_ERR DRIVER_NAME ": Transaction error: [%d] bytes transfered instead of [%u]!\n",data_transfered, (unsigned int)count);
				}
				dma_unmap_page(fbi->dev, start_dma_addr, accu_count, DMA_TO_DEVICE);

				accu_count = PAGE_SIZE;
				// First page of a possible contiguous list
				offset = page->index << PAGE_SHIFT;
				firstpage = page;
			}
		}
		old_dma_addr = dma_addr;
		dma_unmap_page(fbi->dev, dma_addr, count, DMA_TO_DEVICE);
	}
	// DMA the last part - if any
	if (accu_count)
	{
		start_dma_addr = dma_map_page(fbi->dev, firstpage, 0, accu_count, DMA_TO_DEVICE);
		data_transfered = OmniTek_MDMA_dev_transfer (
													&filp,
													0,
													start_dma_addr,
													accu_count ,
													&offset,
													true,
													true
												);
		if (data_transfered != accu_count)
		{
			printk(KERN_ERR DRIVER_NAME ": Transaction error: [%d] bytes transfered instead of [%u]!\n",data_transfered, (unsigned int)count);
		}
		dma_unmap_page(fbi->dev, start_dma_addr, accu_count, DMA_TO_DEVICE);

	}


}

static void netvizfb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
	struct netvizfb_info *fbi = p->par;
//	printk("\t\txxx netvizfb_fillrect called \n");

	sys_fillrect(p, rect);
	netvizfb_refresh(fbi, rect->dy, rect->height);
}

static int netvizfb_blank(int blank_mode, struct fb_info *info)
{
//	printk("\t\txxx netvizfb_blank called \n");


	return 0;
}

static void netvizfb_imageblit(struct fb_info *p, const struct fb_image *image)
{
	struct netvizfb_info *fbi = p->par;
//	printk("\t\txxx netvizfb_imageblit called \n");

	sys_imageblit(p, image);
	netvizfb_refresh(fbi, image->dy, image->height);
}

static void netvizfb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
	struct netvizfb_info *fbi = p->par;
//	printk("\t\txxx netvizfb_copyarea called \n");

	sys_copyarea(p, area);
	netvizfb_refresh(fbi, area->dy, area->height);
}

static ssize_t netvizfb_write(struct fb_info *p, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	struct netvizfb_info *fbi = p->par;
	ssize_t res;
	int y1, y2;
//	printk("\t\txxx netvizfb_write called \n");

	y1 = ((long)*ppos) / p->fix.line_length;
	res = fb_sys_write(p, buf, count, ppos);

	if (res > 0) {
		/* line of last pixel written */
		y2 = ((long)(*ppos - 1)) / p->fix.line_length;
		netvizfb_refresh(fbi, y1, y2 - y1 + 1);
	}

	return res;
}


static struct fb_ops netvizfb_fbops = {
	.owner			= THIS_MODULE,
	.fb_read		= fb_sys_read,
	.fb_write		= netvizfb_write,
	.fb_setcolreg	= netvizfb_setcolreg,
	.fb_setcmap		= netvizfb_setcmap,
	.fb_blank		= netvizfb_blank,
	.fb_ioctl		= netvizfb_ioctl,
	.fb_set_par		= netvizfb_set_par,
	.fb_fillrect	= netvizfb_fillrect,
	.fb_copyarea	= netvizfb_copyarea,
	.fb_imageblit	= netvizfb_imageblit,
	.fb_check_var	= netvizfb_check_var,
};

static struct fb_deferred_io netvizfb_defio = {
	.delay		=  HZ/60,
	.deferred_io	= netvizfb_deferred_io,
};

static int __devinit  netvizfb_probe(struct platform_device *op)
{
	struct fb_info *info;
	struct netvizfb_info *fbi;
//	struct resource res, res2;
//	const u32 *prop;
	u32 bits_per_pixel;
	int ret=0;
//	int size;
	unsigned long framebuffer_size;
//	void __iomem *ctrl_base;

	PResource pChannel;
	bool found = false;

	/* use defaults */
	bits_per_pixel = BITS_PER_PIXEL;

	/* allocate fb information buffer */
	info = framebuffer_alloc(sizeof(struct netvizfb_info), &op->dev);
	if (!info)
	{
		printk(KERN_ERR DRIVER_NAME ": NetViz framebuffer info buffer allocation failed\n");
		return -ENOMEM;
	}

	/* NetViz specific info */
	fbi = info->par;
	fbi->info = info;


	/*==============================================================================*/

	/*Get the OmniTek Extension */
	if (!(fbi->pExt = OmniTekGetExtension(0)))
	{
		printk(KERN_ERR DRIVER_NAME ": Couldn't get OmniTek Extension\n");
		goto failed_omnitek;
	}
	printk(KERN_INFO DRIVER_NAME ": OmniTek Extension OK\n");

	//Get the relevant channel pointer
	list_for_each_entry(pChannel,&(fbi->pExt->pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
	{
	  if (pChannel->u.DmaChannel.Index == MDMA_CHANNEL)
	  {
		  found = true;
		break;
	  }
	}
	if (found)
	{
		printk(KERN_INFO DRIVER_NAME ": MDMA channel %d OK\n",MDMA_CHANNEL);
	}
	else
	{
		printk(KERN_ERR DRIVER_NAME ": MDMA channel %d not found !\n",MDMA_CHANNEL);
	}

	// here we have a valid pChannel to MDMAchannel --> create transaction
	fbi->pDMAChannel = pChannel;

	/*==============================================================================*/


	strlcpy(info->fix.id, DRIVER_NAME, sizeof(info->fix.id));
	//	info->fix.smem_start  = res2.start;		/* start of framebuffer memory = physical address */
	info->fix.type        = FB_TYPE_PACKED_PIXELS; /* see FB_TYPE_*                */
	info->fix.visual      = bits_per_pixel==8 ? FB_VISUAL_PSEUDOCOLOR : FB_VISUAL_TRUECOLOR; /* see FB_VISUAL_*              */
	info->fix.xpanstep    = 0; /* zero if no hardware panning  */
	info->fix.ypanstep    = 0; /* zero if no hardware panning  */
	info->fix.ywrapstep   = 0; /* zero if no hardware ywrap    */
	info->fix.line_length = XRES_VIRTUAL*bits_per_pixel/8; /* length of a line in bytes    */
	info->fix.accel       = FB_ACCEL_NONE; /* Indicate to driver which specific chip/card we have  */
//	info->fix.mmio_start  = res.start; /* Start of Memory Mapped I/O   */
//	info->fix.mmio_len    = res.end - res.start + 1; /* Length of Memory Mapped I/O  */


	/* allocate a virtual framebuffer in CPU memory for the OS to write on */
	framebuffer_size = (XRES_VIRTUAL*bits_per_pixel/8 ) * YRES_VIRTUAL;
	info->screen_base = vmalloc(framebuffer_size); /* Virtual address */
	if (!info->screen_base)
	{
		printk(KERN_ERR DRIVER_NAME ": failed to map framebuffer - size=%lu\n", framebuffer_size);
		ret = -ENOMEM;
		goto failed_vmalloc;
	}
	info->screen_size = framebuffer_size;

	printk(KERN_INFO DRIVER_NAME ": framebuffer mapped - size=%lu\n", framebuffer_size);

	/* dma physical address */
#if 0
	struct page *vmalloc_to_page(void *vmaddr);
	dma_addr_t dma_map_page(struct device *dev, struct page *page,
							unsigned long offset, size_t size,
							enum dma_data_direction direction);
#endif
	fbi->dev = &pChannel->pExt->Device->dev;

	/* set fix screeninfo	*/
	info->fix.smem_len = framebuffer_size;		/* Length of frame buffer mem */
//	printk(KERN_INFO DRIVER_NAME ": screen_base=0x%08X info->fix.smem_len=0x%08X\n", (unsigned int)info->screen_base,  (unsigned int)info->fix.smem_len);

#if 0
	/* registers */
	if (!request_mem_region(info->fix.mmio_start, info->fix.mmio_len,
				NAME)) {
		ret = -EBUSY;
		goto failed_req_regs;
	}

	/* map registers */
	fbi->base = ioremap(info->fix.mmio_start, info->fix.mmio_len);
	if (!fbi->base) {
		dev_err(&op->dev, "Unable to map registers\n");
		ret = -EIO;
		goto failed_map_regs;
	}
#endif

	/* fill in the info structures  */
	info->var.xres             = XRES_VISIBLE;	/* visible resolution           */
	info->var.yres             = YRES_VISIBLE;
	info->var.left_margin      = 0;
	info->var.upper_margin     = 0;
	info->var.xres_virtual     = XRES_VIRTUAL;	/* virtual resolution           */
	info->var.yres_virtual     = YRES_VIRTUAL;
	info->var.bits_per_pixel   = bits_per_pixel; /* guess what                   */
	info->var.height           = -1;	/* height of picture in mm    */
	info->var.width            = -1;
	info->var.xoffset		   = 0;
	info->var.yoffset		   = 0;

	if (bits_per_pixel == 32) {
		info->var.red.offset       = 24;
		info->var.red.length       = 8;
		info->var.red.msb_right    = 0;
		info->var.green.offset     = 16;
		info->var.green.length     = 8;
		info->var.green.msb_right  = 0;
		info->var.blue.offset      = 8;
		info->var.blue.length      = 8;
		info->var.blue.msb_right   = 0;
		info->var.transp.offset    = 0;
		info->var.transp.length    = 8;
		info->var.transp.msb_right = 0;
	} else if (bits_per_pixel == 16) {
		info->var.red.length       = 5;
		info->var.green.length     = 6;
		info->var.blue.length      = 5;
		info->var.transp.length    = 0;

		info->var.red.offset       = 11;
//		info->var.red.offset       = 0;
		info->var.green.offset     = 5;
		info->var.blue.offset      = 0;
//		info->var.blue.offset      = 11;
		info->var.transp.offset    = 0;

		info->var.red.msb_right    = 0;
		info->var.green.msb_right  = 0;
		info->var.blue.msb_right   = 0;
		info->var.transp.msb_right = 0;
	} else {
		/* default to 8 bit paletted */
		info->var.red.length       = 8;
		info->var.green.length     = 8;
		info->var.blue.length      = 8;
		info->var.transp.length    = 0;

		info->var.red.offset       = 0;
		info->var.green.offset     = 0;
		info->var.blue.offset      = 0;
		info->var.transp.offset    = 0;

		info->var.red.msb_right    = 0;
		info->var.green.msb_right  = 0;
		info->var.blue.msb_right   = 0;
		info->var.transp.msb_right = 0;
	}

	info->var.vmode	= FB_VMODE_NONINTERLACED;

	info->fbops              = &netvizfb_fbops;
	info->flags              = FBINFO_MODULE | FBINFO_DEFAULT ; //| FBINFO_READS_FAST;

	info->fbdefio            = &netvizfb_defio;
	fb_deferred_io_init(info);

#if 0
	ret = fb_alloc_cmap(&info->cmap, 256, 0);
	if (ret < 0)
		goto failed_cmap;

	/* tell the hw where the framebuffer is */
	netvizfb_setreg(fbi, REG_BASEADDR, res2.start & 0xfffffff);

	/* set cursors */
	netvizfb_set_par(info);


	/* turn the display on */
	netvizfb_setreg(fbi, REG_CONTROL, CTRL_ENABLE | CTRL_BACKGRND);
#endif
	/* clear screen */
	//netvizfb_refresh(fbi, 0, YRES_VISIBLE);


	ret = register_framebuffer(info);
	if (ret < 0)
	{
		printk(KERN_ERR DRIVER_NAME ": failed to register framebuffer\n");
		goto failed_register;
	}

	dev_set_drvdata(&op->dev, info);

	printk(KERN_INFO DRIVER_NAME ": frame buffer device registered:[ %s ]\n", info->fix.id);

	return 0;

 failed_register:
	fb_dealloc_cmap(&info->cmap);
#if 0
 failed_cmap:
	fb_deferred_io_cleanup(info);
	iounmap(fbi->base);
 failed_map_regs:
	release_mem_region(info->fix.mmio_start, info->fix.mmio_len);
 failed_req_regs:
//	vfree(info->screen_base);
	kfree(info->screen_base);
#endif
 failed_omnitek:
 failed_vmalloc:
	framebuffer_release(info);
#if 0
 failed_ctrl:
	iounmap(ctrl_base);
#endif
	return ret;
}

static int __devexit netvizfb_remove(struct platform_device *op)
{
	struct fb_info *info = dev_get_drvdata(&op->dev);
//	struct netvizfb_info *fbi = info->par;
#if 0
	/* disable hardware */
	netvizfb_setreg(fbi, REG_CONTROL, 0);
#endif
	unregister_framebuffer(info);

	fb_deferred_io_cleanup(info);

#if 0
	fb_dealloc_cmap(&info->cmap);
#endif
//	iounmap(fbi->base);
//	release_mem_region(info->fix.mmio_start, info->fix.mmio_len);

	vfree(info->screen_base);

	framebuffer_release(info);

	dev_set_drvdata(&op->dev, NULL);

	return 0;
}

#if 0
static struct of_device_id netvizfb_of_match[] __devinitdata = {
	{ .compatible = "barco,netviz-fb-1.00", },
	{}
};
MODULE_DEVICE_TABLE(of, netvizfb_of_match);
#endif

static void netvizfb_device_release(struct device *dev)
{
}


/* platform device description -- name has to match the platform_driver.name*/
static struct platform_device netvizfb_device = {
		.name = DRIVER_NAME,
		.id = 0,
		.dev = {
		.release = netvizfb_device_release,
	},
};

static struct platform_driver netvizfb_driver = {
	.probe = netvizfb_probe,
	.remove = __devexit_p(netvizfb_remove),
	.driver = {
	.owner = THIS_MODULE,
	.name = DRIVER_NAME,
#if 0
	.of_match_table = netvizfb_of_match,
#endif
	},
};



static int __init netvizfb_init(void)
{
	int returnvalue = 0;
	printk(KERN_INFO DRIVER_NAME ": NetViz framebuffer driver loaded\n");
	returnvalue = platform_device_register(&netvizfb_device);
	if (returnvalue < 0)
	{
		printk(KERN_INFO DRIVER_NAME ": Failed to register platform device\n");
		return(returnvalue);
	}
	returnvalue = platform_driver_register(&netvizfb_driver);
	if (returnvalue < 0)
	{
		printk(KERN_INFO DRIVER_NAME ": Failed to register platform driver\n");
		return(returnvalue);
	}

	return returnvalue;
}



static void __exit netvizfb_exit(void)
{
	printk(KERN_INFO DRIVER_NAME ": NetViz framebuffer driver exit\n");
	platform_driver_unregister(&netvizfb_driver);
	platform_device_unregister(&netvizfb_device);
}

module_init(netvizfb_init);
module_exit(netvizfb_exit);

MODULE_AUTHOR("Peter Korsgaard <peter.korsgaard@barco.com> & Jeroen De Wachter <jeroen.dewachter@barco.com>");
MODULE_DESCRIPTION("Barco NetViz OSD frame buffer driver");
MODULE_LICENSE("GPL");
