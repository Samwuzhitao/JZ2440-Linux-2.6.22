#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <asm/irq.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

#define LIGHT_MAJOR   100
#define LIGHT_ON      1
#define LIGHT_OFF     0

#define DEVICE_NAME   "light"

/* 用来指定LED所用的GPIO引脚 */
static unsigned long led_table [] = {
    S3C2410_GPF4,
    S3C2410_GPF5,
    S3C2410_GPF6,
};

/* 用来指定GPIO引脚的功能：输出 */
static unsigned int led_cfg_table [] = {
    S3C2410_GPF4_OUTP,
    S3C2410_GPF5_OUTP,
    S3C2410_GPF6_OUTP,
};

struct light_dev {
	struct cdev cdev;     /* 字符设备cdev结构体 */
	unsigned char value;  /* 1= on 0 = off */
};

struct light_dev *light_devp;
int light_major = LIGHT_MAJOR;


/* 打开或者关闭函数 */
static int light_open(struct inode *inode, struct file *filp)
{
	int i;
	struct light_dev *dev;
	/* 获得设备结构体指针 */
	dev = container_of(inode->i_cdev, struct light_dev, cdev);
	/* 让设备结构体作为设备的私有信息 */
	filp->private_data = dev;
	
	for(i=0;i<3;i++) {
		// 设置GPIO引脚的功能，设为输出
		s3c2410_gpio_cfgpin(led_table[i],led_cfg_table[i]);
	}

	printk(KERN_WARNING "Open light driver!\r\n");
	
	return 0;
}



static int light_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct light_dev *dev = filp->private_data;

	printk(KERN_WARNING "Kernel:cmd = %d arg = %d\r\n",cmd,arg);

	if ( arg > 4 ) {
		return -EINVAL;
	}
		
	switch(cmd) {
		case LIGHT_ON:
			{
			  s3c2410_gpio_setpin(led_table[arg],0);
			  printk(KERN_WARNING "Kernel:light_on\r\n");
			}
			break;

		case LIGHT_OFF:
			{
				s3c2410_gpio_setpin(led_table[arg],1);
				printk(KERN_WARNING "Kernel:light_off\r\n");
			}
			break;
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations light_fops = {
	.owner = THIS_MODULE,
	.ioctl = light_ioctl,
	.open  = light_open,
};

static void light_setup_cdev(struct light_dev *dev, int index)
{
	int err, devno = MKDEV(light_major,index);
		
	cdev_init(&dev->cdev,&light_fops);

	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &light_fops;
		
	err = cdev_add(&dev->cdev, devno, 1);

	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

static int __init light_init(void)
{
	int ret;
	/* 注册字符设备 	
	* 参数为主设备号、设备名字、file_operations结构；	  
	* 这样，主设备号就和具体的file_operations结构联系起来了， 	
	* 操作主设备为LED_MAJOR的设备文件时，就会调用s3c24xx_leds_fops中的相关成员函数	   
	* LED_MAJOR可以设为0，表示由内核自动分配主设备号 	
	*/

	dev_t dev = MKDEV(light_major, 0);

	if(light_major)
		ret = register_chrdev_region(dev, 1, "LED");
	else {
		ret = alloc_chrdev_region(&dev, 0, 1, "LED");
		light_major = MAJOR(dev);
	}

	if(ret < 0)
		return ret;
	else 
		printk(DEVICE_NAME "initialized \r\n");

	light_devp = kmalloc(sizeof(struct light_dev), GFP_KERNEL);

	if(!light_devp) {
		ret = -ENOMEM;
		goto fail_malloc;
	}

	memset(light_devp, 0, sizeof(struct light_dev));

	light_setup_cdev(light_devp,0);
	
	/*
	ret = register_chrdev(LIGHT_MAJOR, DEVICE_NAME, &light_fops);
	if (ret < 0) {
		printk(DEVICE_NAME "Can't register major number!\r\n");
		return ret;
	}
	*/
	
	fail_malloc:
		unregister_chrdev_region(dev, light_devp);
		return ret;
}

static void __exit light_exit(void)
{
	//unregister_chrdev(LIGHT_MAJOR, DEVICE_NAME);
	cdev_del(&light_devp->cdev);
	kfree(light_devp);
	unregister_chrdev_region(MKDEV(light_major, 0), 1);	
}

module_init(light_init);
module_exit(light_exit);

MODULE_AUTHOR("Samwu <1322673636@qq.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Led drive demo");





