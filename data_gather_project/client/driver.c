#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/mach/map.h>
#include <asm/arch/map.h>
#include <asm/arch/regs-mem.h>
#include <asm/arch/regs-adc.h>
#include <asm/arch/regs-clock.h>
#include <asm/arch/regs-gpio.h>
#include <asm/io.h>
#include <linux/dma-mapping.h>
#include <asm/arch/regs-lcd.h>
#include <asm/uaccess.h>

#define ADC_MAJOR 250
#define DEBUG
MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("module example ");
MODULE_LICENSE("GPL");

#define SYS_ADDRESS	S3C24XX_VA_ADC

int adc_read(void)
{
	writel(readl(S3C2410_ADCCON + SYS_ADDRESS) | S3C2410_ADCCON_ENABLE_START,S3C2410_ADCCON + SYS_ADDRESS);
	while (!(readl(S3C2410_ADCCON + SYS_ADDRESS) & S3C2410_ADCCON_ECFLG))
		;
	return (readl(S3C2410_ADCDAT0 + SYS_ADDRESS) & 0x3ff);
}

void adc_init(void)
{
	int user_data = 0;
	unsigned int adc_con = 0;
	/* set register value*/
	/* 50,000,000/(9+1) | enable select = 0 | stat by set 1 to bit 0 | set to normal model*/
	adc_con = (S3C2410_ADCCON_PRSCVL(9) | S3C2410_ADCCON_PRSCEN | S3C2410_ADCCON_SELMUX(0));
	printk("[%x]\n", adc_con);

	/* write to register*/
	writel(readl(S3C2410_CLKCON) | S3C2410_CLKCON_ADC,S3C2410_CLKCON);
	writel(adc_con, (S3C2410_ADCCON + SYS_ADDRESS));

#ifdef DEBUG
	printk("[%x]\n", readl(S3C2410_ADCCON + SYS_ADDRESS));
#endif

//	user_data = adc_test();

	printk("%d", user_data);
}

int nick_adc_open(struct inode *inode, struct file *filp)
{

	printk("open major = %d, minjor = %d\n", MAJOR(inode->i_rdev), MINOR(inode->i_rdev));
	return 0;
}

ssize_t nick_adc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	short user_data;

	if (count < 2)
		return -1;
	
	user_data = adc_read();
	
	copy_to_user(buf, (char *)&user_data, 2);

	return 2;
}

ssize_t nick_adc_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{
	return -1;
}

int nick_adc_release(struct inode *inode, struct file *filep)
{
	return 0;
}

void adc_close(void)
{

}

int nick_adc_ioctl(struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg)
{
	return 0;
}

struct file_operations nick_adc_fops = {
	.owner = THIS_MODULE,
	.open = nick_adc_open,
	.read = nick_adc_read,
	.write = nick_adc_write,
	.release = nick_adc_release,
	.ioctl = nick_adc_ioctl,
};

int __init 
nick_adc_init (void)
{
	int rc;
	
	/* init adc*/
	adc_init();

	/* register char dev for system*/
	rc = register_chrdev(ADC_MAJOR, "nick_adc", &nick_adc_fops);
	if(rc < 0)
	{
		printk("fail to register %s\n", "nick_adc");
		unregister_chrdev(ADC_MAJOR, "nick_adc");
		return -1;
	}

	printk ("ok!\n");

	return 0;
}

void __exit
nick_adc_exit (void)
{
	adc_close();
	unregister_chrdev(ADC_MAJOR, "nick_adc");
	printk ("module exit\n");
	return ;
}

module_init(nick_adc_init);
module_exit(nick_adc_exit);
