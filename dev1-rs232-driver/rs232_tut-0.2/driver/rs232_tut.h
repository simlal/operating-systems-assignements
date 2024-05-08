#ifndef __RS232_TUT_H
# define __RS232_TUT_H

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/kdev_t.h>
#include "cbuffer.h"

#define RS232_TUT_VERSION "0.2"
#define RS232_TUT_AUTHOR "Kristian Benoit et Nicolas Dufresne"
#define RS232_TUT_DESC "Pilote tutoriel seriel rs232"

#define INFO(...) printk(KERN_INFO MODULE_NAME ": " __VA_ARGS__)
#define ERR(...)  printk(KERN_ERR MODULE_NAME ": " __VA_ARGS__)
#ifdef _DEBUG
# define DEBUG(...) printk(KERN_DEBUG MODULE_NAME ": " __VA_ARGS__)
#endif //_DEBUG


struct rs232_tut_dev {
    struct cdev cdev;
    dev_t num;
    struct resource *resource;
    struct cbuffer *cbuf_in;
    struct cbuffer *cbuf_out;
};

extern struct rs232_tut_dev rs232_tut_dev;

int             rs232_tut_open      (struct inode *i,
                                     struct file *file);

int             rs232_tut_release   (struct inode *i,
                                     struct file *file);

ssize_t         rs232_tut_read      (struct file *file,
                                     char __user *userbuffer,
                                     size_t size,
                                     loff_t *o);

ssize_t         rs232_tut_write     (struct file *file,
                                     const char __user *userbuffer,
                                     size_t size, loff_t *o);

irqreturn_t     rs232_tut_isr       (int irq,
                                     void *dev_id,
                                     struct pt_regs *state);

int             init_rs232          (void);
void            clean_rs232         (void);

#endif /* RS232_TUT_VERSION */
