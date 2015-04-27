#include <linux/kernel.h>

#include <linux/module.h>

#include <asm/mach/map.h>

#include <asm/arch/map.h>

#include <asm/arch/regs-mem.h>

#include <asm/arch/regs-adc.h>

#include <asm/arch/regs-gpio.h>

#include <asm/io.h>

#include <linux/dma-mapping.h>

#include <asm/arch/regs-lcd.h>

/*
MODULE_AUTHOR("Asmcos");

MODULE_DESCRIPTION("module example ");

MODULE_LICENSE("GPL");
*/
#define SYS_ADDRESS	S3C24XX_VA_ADC

float adc_test(void)
{
	writel(readl(S3C2410_ADCCON + SYS_ADDRESS) | S3C2410_ADCCON_ENABLE_START,S3C2410_ADCCON + SYS_ADDRESS);
	while (!(readl(S3C2410_ADCCON + SYS_ADDRESS) & S3C2410_ADCCON_ECFLG))
		;
	return (readl(S3C2410_ADCDAT0 + SYS_ADDRESS) & 0x3ff);
}

void adc_init(void)
{
	int user_data = 0;
	int adc_con = 0;
	/* set register value*/
	/* 50,000,000/(9+1) | enable select = 0 | stat by set 1 to bit 0 | set to normal model*/
	adc_con = (S3C2410_ADCCON_PRSCVL(9) | S3C2410_ADCCON_PRSCEN | S3C2410_ADCCON_SELMUX(0));
	
	/* write to register*/
	writel(adc_con , S3C2410_ADCCON + SYS_ADDRESS);
	user_data = adc_test();
	printk("%f", user_data);
}

void adc_close(void)
{

}
