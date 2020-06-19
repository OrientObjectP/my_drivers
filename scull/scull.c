#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include<linux/device.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/delay.h>

#define SCULL_INIT 0
#define SCULL_QSET 2000
#define SCULL_QUANTUM 512

 
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev_st {
	char mych[100];
	int quantum;
	int qset;
	unsigned long size;
	unsigned int access_key;
	struct semaphore sem;
	struct cdev cdev;
};


#define MYNAME "scull_test"
#define MYCOUNT 3

static struct class *test_dev_class;
dev_t scull_devnum;
struct scull_dev_st scull_dev;


/*
static struct scull_qset *scull_follow(struct scull_dev_st *dev,unsigned int item)
{
	struct scull_qset *dptr = dev->data;

	if (!dptr) {
		dptr = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (!dptr) {
			printk(KERN_DEBUG "kmalloc failed\n");
			return NULL;
		}
		memset(dptr, 0, sizeof(struct scull_qset));
	}
	while(item--) {
		if (!dptr->next) {
			dptr->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if (!dptr->next) {
				printk(KERN_DEBUG "kmalloc failed\n");
				return NULL;
			}
			memset(dptr->data, 0, sizeof(struct scull_qset));
		}
		dptr = dptr->next;
		continue;
	}
	return dptr;
}

*/

static int scull_open(struct inode *inode, struct file *filp)
{
	
	struct scull_dev_st *dev;

	dev = container_of(inode->i_cdev, struct scull_dev_st, cdev);
	filp->private_data = dev;

	
	return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_ops)
{
	
	struct scull_dev_st *dev;


	ssize_t retval = 0;

	dev = filp->private_data;


	
	if (*f_ops > dev->size) {
		printk(KERN_DEBUG "can not read beyond data length\n");
		goto out;
	}
	if (*f_ops + count > dev->size) {
		count = dev->size - *f_ops;
		printk(KERN_DEBUG "*f_ops + count > dev->size. count=%d\n",count);	
	}
	



	if(copy_to_user(buffer, dev->mych + *f_ops, count)) {
		printk(KERN_DEBUG "copy_to_user failed\n");
		goto out;
	}

	*f_ops += count;
	retval = count;
	printk(KERN_DEBUG "copy_to_user succeeded\n");	
	printk(KERN_DEBUG "*f_ops=%d\n",*f_ops);		


out:

	return retval;
	
	
}

static ssize_t scull_write(struct file *filp, const char __user *buffer, size_t count, loff_t *f_ops)
{

	






	int retval=0;
	struct scull_dev_st *dev;
	dev = filp->private_data;




    //down(&(dev->sem));
	if(dev->mych[0]=='Q')
		printk(KERN_DEBUG "ev->mych[0]=='Q',my pid = %d\n",current->pid);	
	else
		printk(KERN_DEBUG "ev->mych[0]!='Q',my pid = %d\n",current->pid);	

	printk("down-my_pid=%d\n",current->pid);
	printk(KERN_INFO "The cuurent process commond ： \"%s\"  the pid ：%x\n", current->comm, current->pid);	
	if (*f_ops>100)
		retval = 200;
	if( *f_ops + count > 100)
		count = 100 - *f_ops;
	if (copy_from_user(dev->mych + *f_ops, buffer, count)) {
		goto out;
	}
	*f_ops += count;
	retval = count;
	
	if (dev->size < *f_ops)
		dev->size = *f_ops;	
		
	printk(KERN_DEBUG "writeeeee*f_ops=%d\n",*f_ops);	
	printk(KERN_INFO "The cuurent process commond ： %s  the pid ：%x\n", current->comm, current->pid);		
	printk("  up-my_pid=%d\n",current->pid);


	udelay(5000);
	dev->mych[0] = 'Q';
    //up(&(dev->sem));	
		



out:
	return retval;
	
}






struct file_operations scull_ops = {
	.owner		    = THIS_MODULE,
	.open		    = scull_open,
	.read		    = scull_read,
	.write		    = scull_write,
};


static int __init scull_init(void)
{
	
	printk(KERN_ALERT "chrdev_init helloworld init\n");
	int retval = 0;
	
	//自动生成主设备号和次设备号，
	retval = alloc_chrdev_region(&scull_devnum,0, MYCOUNT, MYNAME);
	if (retval<0) {
		printk(KERN_ERR "Unable to alloc minors for test_device\n");
		return 0;
	}
	else {
		printk(KERN_ERR "alloc_chrdev_region is succeed\n");
		printk(KERN_ERR "MAJOR=%d,MINOR=%d\n",MAJOR(scull_devnum),MINOR(scull_devnum));		
	}
	

	scull_dev.size = 0;
	
	cdev_init(&(scull_dev.cdev), &scull_ops);
	scull_dev.cdev.owner = THIS_MODULE;
	scull_dev.cdev.ops = &scull_ops;		
	printk(KERN_ERR "scull_dev & cdev_init is succeed\n");		
	
	retval = cdev_add(&(scull_dev.cdev), scull_devnum, MYCOUNT);
	if (retval) {
		printk(KERN_ERR "Unable to get usb_device major %d\n",
		       MAJOR(scull_devnum));
		return 0;
	}
	else
		printk(KERN_ERR "cdev_add is succeed\n");
	
	
	
	
	test_dev_class = class_create(THIS_MODULE, "solitude_scull");
	if (IS_ERR(test_dev_class))
		return;
	
	//在/sys/class/solitude_test/下会创建多个文件，同时在/dev/下也会创建多个设备文件
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 0), NULL, "scull0");    //最后一个参数就是在/dev/xx下的文件名
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 1), NULL, "scull1");
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 2), NULL, "scull2");
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 3), NULL, "scull3");	
	
	
	
	sema_init(&(scull_dev.sem),1);
	
	
	
	
	
	
		
		/*
	//自动生成主设备号和次设备号，
	retval = alloc_chrdev_region(&scull_devnum,0, MYCOUNT, MYNAME);
	if (retval<0) {
		printk(KERN_ERR "Unable to alloc minors for test_device\n");
		return 0;
	}
	else {
		printk(KERN_ERR "alloc_chrdev_region is succeed\n");
		printk(KERN_ERR "MAJOR=%d,MINOR=%d\n",MAJOR(scull_devnum),MINOR(scull_devnum));		
	}
	
	//初始化cdev
	cdev_init(&(scull_dev.cdev),&scull_ops);
	scull_dev.cdev.owner = THIS_MODULE;
	scull_dev.cdev.ops = &scull_ops;	
	printk(KERN_ERR "cdev_init is succeed\n");	
	
	//利用cdev结构体和主次设备号注册驱动，在/proc/device下就可以看到MYNAME的设备
	retval = cdev_add(&(scull_dev.cdev), scull_devnum, MYCOUNT);
	if (retval) {
		printk(KERN_ERR "Unable to get usb_device major %d\n",
		       MYMAJOR);
		return 0;
	}
	else
		printk(KERN_ERR "cdev_add is succeed\n");	
		
	scull_dev.data = NULL;
	scull_dev.qset = SCULL_QSET;
	scull_dev.quantum = SCULL_QUANTUM;
	scull_dev.size = 0;
	
	//注册完成设备驱动后，添加设备类的操作，以让内核帮我们发信心给udev，让udev自动创建和删除设备文件
	//在/sys/class中会创建一个solitude_test文件夹
	test_dev_class = class_create(THIS_MODULE, "solitude_test");
	if (IS_ERR(test_dev_class))
		return;
	
	//在/sys/class/solitude_test/下会创建多个文件，同时在/dev/下也会创建多个设备文件
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 0), NULL, "test0");    //最后一个参数就是在/dev/xx下的文件名
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 1), NULL, "test1");
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 2), NULL, "test2");
	device_create(test_dev_class, NULL, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 3), NULL, "test3");	
		
	*/
	return 0;
}

static void __exit scull_exit(void)
{
	/*
	scull_trim(&scull_dev);
	
	cdev_del(&scull_dev.cdev);	
	printk(KERN_ERR "cdev_del is succeed\n");	
	
	unregister_chrdev_region(scull_devnum, MYCOUNT);
	printk(KERN_ERR "unregister_chrdev_region is succeed\n");	
	
	//自动删除设备文件

	*/
	printk(KERN_DEBUG "scull driver exit\n");
	
	
	cdev_del(&(scull_dev.cdev));
	printk(KERN_ERR "cdev_del is succeed\n");	
	
	unregister_chrdev_region(scull_devnum, MYCOUNT);	
	printk(KERN_ERR "unregister_chrdev_region is succeed\n");		
	
	
	device_destroy(test_dev_class, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 0));
	device_destroy(test_dev_class, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 1));	
	device_destroy(test_dev_class, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 2));	
	device_destroy(test_dev_class, MKDEV(MAJOR(scull_devnum),MINOR(scull_devnum) + 3));	
	
	class_destroy(test_dev_class);		
	
	
	
	
	
}



module_init(scull_init);
module_exit(scull_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("solitude-ubuntu");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
