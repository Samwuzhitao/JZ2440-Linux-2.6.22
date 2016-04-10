#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>

/* 定义主设备号 */
#define LIGHT_MAJOR   100
#define LIGHT_ON      1
#define LIGHT_OFF     0

#define DEVICE_NAME   "light"


struct light_dev
{
        //struct cdev cdev;    /* ????豸cdev???? */
        unsigned char value; /* LED ?????1???????0????????д??? */
};

struct light_dev *light_devp;
int light_major = LIGHT_MAJOR;


/* ??????????? */
int light_open(struct inode *inode, struct file *filp)
{
        struct light_dev *dev;
        /* ????豸??????? */
        //dev = container_of(inode->i_cdev, struct light_dev, cdev);
        /* ???豸????????豸???????? */
        //filp->private_data = dev;
        printk(KERN_WARNING "Open light driver!\n");

        return 0;
}

int light_release(struct inode *inode, struct file *filp)
{
        return 0;
}

/* ??д?豸:???????? */
ssize_t light_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
        struct light_dev *dev = filp->private_data; /* ?????????? */

        //if(copy_to_user(buf, &(dev->value), 1))
        //      return -EFAULT;

        return 1;
}

ssize_t light_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
        struct light_dev *dev = filp->private_data;

        //if(copy_from_user(&(dev->value), buf, 1))
        //      return -EFAULT;


        /* ???д????????????LED??*/
        if (dev->value == 1)
                //light_on();
                printk(KERN_WARNING "light_on\n");
        else
                //light_off();
                printk(KERN_WARNING "light_off\n");

        return 1;
}

int light_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
        struct light_dev *dev = filp->private_data;

        switch(cmd)
        {
                case LIGHT_ON:
                        {
              dev->value = 1;
                          //light_on();
                          printk(KERN_WARNING "light_on\n");
                        }
                        break;

                case LIGHT_OFF:
                        {
                                dev->value = 0;
                                //light_off();
                                printk(KERN_WARNING "light_off\n");
                        }
                        break;
                default:
                        return -ENOTTY;
        }
        return 0;
}

struct file_operations light_fops = {
        .read  = light_read,
        .write = light_write,
        .ioctl = light_ioctl,
        .open  = light_open,
        .release = light_release,
};

static int __init light_init(void)
{
        int ret;

        ret = register_chrdev(LIGHT_MAJOR, DEVICE_NAME, &light_fops);
        if (ret < 0) {
                printk(DEVICE_NAME "Can't register major number!\n");
                return ret;
        }
        printk(DEVICE_NAME "initialized \n");
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

