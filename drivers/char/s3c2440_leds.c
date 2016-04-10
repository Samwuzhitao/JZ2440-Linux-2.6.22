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

/* ����ָ��LED���õ�GPIO���� */
static unsigned long led_table [] = {
    S3C2410_GPF4,
    S3C2410_GPF5,
    S3C2410_GPF6,
};

/* ����ָ��GPIO���ŵĹ��ܣ���� */
static unsigned int led_cfg_table [] = {
    S3C2410_GPF4_OUTP,
    S3C2410_GPF5_OUTP,
    S3C2410_GPF6_OUTP,
};

/* �򿪻��߹رպ��� */
static int light_open(struct inode *inode, struct file *filp)
{

	int i;
	/* ����豸�ṹ��ָ�� */
	//dev = container_of(inode->i_cdev, struct light_dev, cdev);
	/* ���豸�ṹ����Ϊ�豸��˽����Ϣ */
	//filp->private_data = dev;
	for(i=0;i<3;i++) {
		// ����GPIO���ŵĹ��ܣ���Ϊ���
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
	
	/* ע���ַ��豸 	
	* ����Ϊ���豸�š��豸���֡�file_operations�ṹ��	  
	* ���������豸�žͺ;����file_operations�ṹ��ϵ�����ˣ� 	
	* �������豸ΪLED_MAJOR���豸�ļ�ʱ���ͻ����s3c24xx_leds_fops�е���س�Ա����	   
	* LED_MAJOR������Ϊ0����ʾ���ں��Զ��������豸�� 	
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





