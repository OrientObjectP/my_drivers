#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>

/*copy_from_user*/
#include <linux/uaccess.h>



#define MYNAME "module_test"

#define MIN(x,y) ((x)<(y)?(x):(y))

int ret;
char mybuf[20];
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











// 模块安装函数
static int __init chrdev_init(void)
{	
	printk(KERN_ALERT "chrdev_init helloworld init\n");
	

	ret = register_chrdev(0,MYNAME,&test_fops);    //驱动注册函数 需提供头文件 <include/fs.h>
	if (!ret){
		printk(KERN_INFO"register chrdev failed\n");	
	}
	printk(KERN_INFO "register chrdev succeed MAJOR=%d\n",ret);		
	
	return 0;
}

// 模块卸载函数
static void __exit chrdev_exit(void)
{
	unregister_chrdev(ret, MYNAME);

	printk(KERN_INFO "chrdev_exit helloworld exit\n");
	
}


module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("solitude-ubuntu");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息








