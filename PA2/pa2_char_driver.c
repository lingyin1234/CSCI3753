#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define BUFF_SIZE 1024                 			 /* size of the buffer */
#define MAJOR_NUMBER 240               		         /* can be change */
#define DEVICE_NAME "simple_character_device" 	         /* device name */
MODULE_AUTHOR("Lingyin Lu");
MODULE_LICENSE("GPL");

static char *device_buff;
static int OpenCount = 0;
static int CloseCount = 0;





ssize_t pa2_char_driver_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	/* max num of bytes that can be read from offset to BUFF_SIZE */
	int mbytes = BUFF_SIZE - *offset;
	/*number of bytes to read */
	/*number of bytes that we passed through */
	int BYTES_TO_READ, BYTES_READ;
	/* logical statements about length of reading buffer */
	if(mbytes > length){
		BYTES_TO_READ = length;
	} else {
		BYTES_TO_READ = mbytes;
	}
	/* test for empty file */
	if(BYTES_TO_READ == 0){
		printk(KERN_ALERT "END OF DEVICE \n");
	}
	/* how many bytes did we read */
	BYTES_READ = BYTES_TO_READ - copy_to_user(buffer, device_buff + *offset, BYTES_TO_READ);	
	printk(KERN_ALERT "DEVICE READ: %d BYTES\n", BYTES_READ);
	/* move the offset hoewver many bytes we read */
	*offset += BYTES_READ;
	/* when we get to the end */
	printk(KERN_ALERT "DEVICE FINISH READING");
	/*return how many bytes read*/
	return BYTES_READ; /* 0 for empty file */
}

ssize_t pa2_char_driver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	/* max num of bytes that can be WRITE from offset to BUFF_SIZE */
	int mbytes = BUFF_SIZE - *offset;
	/*number of bytes to WRITE */
	/*number of bytes WRITTEN */
	int BYTES_TO_WRITE, BYTES_WROTE;
	/* logical statements about length of writing and buffer */	
	if(mbytes > length){
		BYTES_TO_WRITE = length;
	} else {
		BYTES_TO_WRITE = mbytes;
	}
	/* the bytes we wrote is equal with the total number of bytes to write, minus the remaining but we have yet read
		this remaaing bit comees from user space*/
	BYTES_WROTE = BYTES_TO_WRITE - copy_from_user(device_buff + *offset, buffer, BYTES_TO_WRITE);
	/* when we say howmany bytes we wrote */
	printk(KERN_ALERT "DEVICE WROTE: %d BYTES\n", BYTES_WROTE);
	/* move the offset by however many bytes we wrote*/
	*offset += BYTES_WROTE;
	/* alert kernel that we are finished */
	printk(KERN_ALERT "DEVICE FINISHED WRITING\n");

	return BYTES_WROTE; /*returning the length of the bytes.*/
}


int pa2_char_driver_open(struct inode *pinode, struct file *pfile){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	OpenCount++;
	printk("Number of devices opened: %d\n", OpenCount);
	return 0; /* indicates success */
}

int pa2_char_driver_close(struct inode *pinode, struct file *pfile){
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	CloseCount++;
	printk("Number of devices closed: %d\n", CloseCount);
	return 0; /* 0 for success */
}


loff_t pa2_char_driver_seek(struct file *pfile, loff_t offset, int whence){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	loff_t temp_offset = 0;
	switch(whence) {
		/* SEEK_SET */
		case 0: 
			temp_offset = offset;
			break;
		/* SEEK_CUR */
		case 1: 
			temp_offset = pfile->f_pos + offset;
			break;
		/* SEEK_END */
		case 2: 
			temp_offset = BUFF_SIZE - offset;
			break;
	}

	/* because the lab writeup made me worried about this*/
	if(temp_offset < 0){
		printk(KERN_ALERT "ERROR: cannot seek before buffer start");
	}

	if(temp_offset > BUFF_SIZE){
		printk(KERN_ALERT "ERROR: cannot seek past BUFF_SIZE");
	}

	/* make the file position equal to our new offset*/
	pfile->f_pos = temp_offset;

	/*return how many bytes the new offset is */
	return temp_offset;
}





struct file_operations pa2_char_driver_file_operations ={
	.owner		= THIS_MODULE,					
	.open 		= pa2_char_driver_open, 		
	.release	= pa2_char_driver_close, 		
	.read 		= pa2_char_driver_read, 		
	.write 		= pa2_char_driver_write,
	.llseek 	= pa2_char_driver_seek,  	
};



static int pa2_char_driver_init(void){

	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	device_buff = kmalloc(BUFF_SIZE, GFP_KERNEL);
	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &pa2_char_driver_file_operations);
	return 0;
}

static void pa2_char_driver_exit(void){

	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	if(device_buff){
		kfree(device_buff);
	}
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
}

module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
