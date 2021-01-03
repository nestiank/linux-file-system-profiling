/********
Loadable kernel module for extracting block write log
Date: 24 Oct 2020
********/

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/time.h>
#include<asm/uaccess.h>

#define PROC_DIRNAME "splab"
#define PROC_FILENAME "splab_log"

// Global pointers for module initiation
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

// Block i/o log struct type definition
struct bio_log_node {
	char *fs_name;
	unsigned long long block_id;
	struct timeval write_time;
};

// Extern statement of block i/o log
extern struct bio_log_node bio_log[1000];
extern int bio_log_index;

// String form block i/o log in kernel space
char blog_print[1000][100];

static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO, "Project module opened.\n");

	return 0;
}
static ssize_t my_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos)
{
	// Copy block write log from kernel space to user space
	if (copy_to_user(user_buffer, blog_print, sizeof(blog_print))) {
		return -EFAULT;
	}

	return count;
}
static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
	int i, cnt = 0;
	int index = bio_log_index;
	struct tm timestamp;

	// Split to start with the oldest log
	for (i = index; i < 1000; i++) {
		// Convert timestamp format
		time_to_tm(bio_log[i].write_time.tv_sec, 0, &timestamp);

		// Save block write log into string form block i/o log
		sprintf(blog_print[cnt++], "File System: %s, Block #: %llu, Timestamp: %d:%d:%d:%ld\n",
			bio_log[i].fs_name, bio_log[i].block_id,
			timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, bio_log[i].write_time.tv_usec);
	}
	for (i = 0; i < index; i++) {
		// Convert timestamp format
		time_to_tm(bio_log[i].write_time.tv_sec, 0, &timestamp);

		// Save block write log into string form block i/o log
		sprintf(blog_print[cnt++], "File System: %s, Block #: %llu, Timestamp: %d:%d:%d:%ld\n",
			bio_log[i].fs_name, bio_log[i].block_id,
			timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, bio_log[i].write_time.tv_usec);
	}

	printk(KERN_INFO, "Project module wrote log.\n");

	return count;
}

// Global struct for module initiation
static const struct file_operations myproc_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = my_read,
	.write = my_write
};

static int __init my_init(void)
{
	printk(KERN_INFO, "Project module initiated.\n");

	proc_dir = proc_mkdir(PROC_DIRNAME, NULL);
	proc_file = proc_create(PROC_FILENAME, 0600, proc_dir, &myproc_fops);

	return 0;
}
static void __exit my_exit(void)
{
	printk(KERN_INFO, "Project module is now gone away.\n");

	return;
}

module_init(my_init);
module_exit(my_exit);

// License information to comply with GPL
MODULE_AUTHOR("System Programming");
MODULE_DESCRIPTION("Course Project LKM");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");
