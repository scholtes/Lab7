/* Author: Garrett Scholtes
 * Date:   2015-11-30
 * 
 * Lab6 - CS4029
 * 
 * This module serves as pseudo network device driver
 * Does not handle ARP requests
 * 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/rwsem.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

int init_module(void) {
    printk("netdriver module installing\n");

    return 0;
}

void cleanup_module(void) {
    printk("netdriver module uninstalling\n");

    return;
}