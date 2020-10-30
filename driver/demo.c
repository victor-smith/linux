#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
/*
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
*/
static char *name = "Ocean";
static int count = 2;
static int para[8] = {1,2,3,4};
static int n_para = 1;
module_param(count, int, S_IRUGO);
module_param(name, charp, S_IRUGO);
module_param_array(para , int , &n_para , S_IRUGO);

static struct file_operations first_drv_fops={
    .owner = THIS_MODULE,
//    .open = first_drv_open,
//    .write = first_drv_write,
};

int first_drv_init(void)
{
    printk("init first_drv drv!\n");

    int i;
    for (i = 0; i < count; i++)
        printk(KERN_ALERT "(%d) Hello, %s ！\n", i, name);


    for (i = 0; i < 8; i++)
        printk(KERN_ALERT "para[%d] : %d \n", i, para[i]);

    for(i = 0; i < n_para; i++)
        printk(KERN_ALERT "para[%d] : %d \n", i, para[i]);

    return 0;
}

void first_drv_exit(void)
{
    printk("exit first_drv drv!\n");
}

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_AUTHOR("Ocean Byond");
MODULE_DESCRIPTION("my first char driver");
MODULE_LICENSE("GPL");
