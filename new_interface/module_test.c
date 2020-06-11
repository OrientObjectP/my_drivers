#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <linux/cdev.h>
/*copy_from_user*/
#include <linux/uaccess.h>



#define MYNAME "module_test"
#define MYMAJOR 200
#define MYMINOR 0
#define MYCOUNT 1

#define MIN(x,y) ((x)<(y)?(x):(y))

int ret;
char mybuf[20];


static struct cdev test_device_cdev;

static int test_chrdev_open(struct inode *inode, struct file *file)
{
	//这个函数真正应该方的是打开这个设备的硬件操作代码部分
	//但是现在暂时我们写不了这么多，所以用一个printk打印信息来表示一下
	printk(KERN_INFO"test_chrdev_open\n");
	return 0;
}

int test_chrdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"test_chrdev_release\n");
	return 0;
}



ssize_t test_chrdev_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	
	printk(KERN_INFO"test_chrdev_read\n");	
	copy_to_user(buf,mybuf,sizeof(mybuf));
}

static ssize_t test_chrdev_write(struct file *file,const char __user *user_buf,size_t count, loff_t *ppos)
{


	int buf_size=MIN(count,sizeof(mybuf)-1);
	copy_from_user(mybuf,user_buf,buf_size);	
	mybuf[buf_size]=0;
	
	printk(KERN_INFO"test_chrdev_write\n");				
}


const struct file_operations test_fops = {
	.owner		= THIS_MODULE,                //惯例，直接写即可
	.open       = test_chrdev_open,           //将来应用open打开这个设备时实际调用的函数
	.release	= test_chrdev_release,           //对应close
	.read		= test_chrdev_read,
	.write		= test_chrdev_write,
};








static	dev_t mydect = MKDEV(MYMAJOR,MYMINOR);


// 模块安装函数
static int __init chrdev_init(void)
{	
	printk(KERN_ALERT "chrdev_init helloworld init\n");
	
	
	int retval;
	


	//申请主设备号和次设备号
	retval = register_chrdev_region(mydect, MYCOUNT, MYNAME);
	if (retval) {
		printk(KERN_ERR "Unable to register minors for test_device\n");
		return 0;
	}
	else
		printk(KERN_ERR "register_chrdev_region is succeed\n");
	
	
	//初始化cdev
	cdev_init(&test_device_cdev,&test_fops);
	printk(KERN_ERR "cdev_init is succeed\n");	
	
	//利用cdev结构体和主次设备号注册驱动
	retval = cdev_add(&test_device_cdev, mydect, MYCOUNT);
	if (retval) {
		printk(KERN_ERR "Unable to get usb_device major %d\n",
		       MYMAJOR);
		return 0;
	}
	else
		printk(KERN_ERR "cdev_add is succeed\n");

	
	return 0;
}

// 模块卸载函数
static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
	
	cdev_del(&test_device_cdev);
	printk(KERN_ERR "cdev_del is succeed\n");	
	
	unregister_chrdev_region(mydect, MYCOUNT);	
	printk(KERN_ERR "unregister_chrdev_region is succeed\n");	
	
}


module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("solitude-ubuntu");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息








