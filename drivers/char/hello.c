#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init( void )
{
	printk(KERN_WARNING "hello world enter\n");
	return 0;
}
module_init(hello_init);

static void __exit hello_exit( void )
{
	printk(KERN_WARNING "Goodbye\n");
	printk(KERN_WARNING "12333333\n");
}
module_exit(hello_exit);

MODULE_AUTHOR("Samwu <1322673636@qq.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A Simple hello world module");
MODULE_ALIAS("A simplest module");



