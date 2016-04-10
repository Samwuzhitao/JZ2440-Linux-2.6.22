#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>
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

/* 打开或者关闭函数 */
static int light_open(struct inode *inode, struct file *filp)
{

	int i;
	/* 获得设备结构体指针 */
	//dev = container_of(inode->i_cdev, struct light_dev, cdev);
	/* 让设备结构体作为设备的私有信息 */
	//filp->private_data = dev;
	for(i=0;i<3;i++) {
		// 设置GPIO引脚的功能，设为输出
		s3c2410_gpio_cfgpin(led_table[i],led_cfg_table[i]);
	}
	printk(KERN_WARNING "Open light driver!\r\n");
	
	return 0;
}



static int light_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{

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

static int __init light_init(void)
{
	int ret;
	
	/* 注册字符设备 	
	* 参数为主设备号、设备名字、file_operations结构；	  
	* 这样，主设备号就和具体的file_operations结构联系起来了， 	
	* 操作主设备为LED_MAJOR的设备文件时，就会调用s3c24xx_leds_fops中的相关成员函数	   
	* LED_MAJOR可以设为0，表示由内核自动分配主设备号 	
	*/
	ret = register_chrdev(LIGHT_MAJOR, DEVICE_NAME, &light_fops);
	if (ret < 0) {
		printk(DEVICE_NAME "Can't register major number!\r\n");
		return ret;
	}
	printk(DEVICE_NAME "initialized \r\n");
	return 0;
}

static void __exit light_exit(void)
{
	unregister_chrdev(LIGHT_MAJOR, DEVICE_NAME);
}

module_init(light_init);
module_exit(light_exit);

MODULE_AUTHOR("Samwu <1322673636@qq.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Led drive demo");





