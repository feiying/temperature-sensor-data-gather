/*
 * linux/drivers/net/cs8900.c
 *
 * Author: Abraham van der Merwe <abraham at 2d3d.co.za>
 *
 * A Cirrus Logic CS8900A driver for Linux
 * based on the cs89x0 driver written by Russell Nelson,
 * Donald Becker, and others.
 */

#define VERSION_STRING "Cirrus Logic CS8900A driver for Linux (Modified for akae)"

#include <linux/config.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

// Added BSt
#include <asm/mach-types.h>

#include "asm/arch/smdk2410.h"

#include "cs8900.h"
int cs8900_probe (struct net_device *dev);

static struct net_device cs8900_dev = {
  init:cs8900_probe
};

/*
 * I/O routines
 */

static inline u16
cs8900_read (struct net_device *dev, u16 reg)
{
  outw (reg, dev->base_addr + PP_Address);
  return (inw (dev->base_addr + PP_Data));
}

static inline void
cs8900_write (struct net_device *dev, u16 reg, u16 value)
{
  outw (reg, dev->base_addr + PP_Address);
  outw (value, dev->base_addr + PP_Data);
}

static inline void
cs8900_set (struct net_device *dev, u16 reg, u16 value)
{
  cs8900_write (dev, reg, cs8900_read (dev, reg) | value);
}

static inline void
cs8900_clear (struct net_device *dev, u16 reg, u16 value)
{
  cs8900_write (dev, reg, cs8900_read (dev, reg) & ~value);
}

static inline void
cs8900_frame_read (struct net_device *dev, struct sk_buff *skb, u16 length)
{
  insw (dev->base_addr, skb_put (skb, length), (length + 1) / 2);
}

static inline void
cs8900_frame_write (struct net_device *dev, struct sk_buff *skb)
{
  outsw (dev->base_addr, skb->data, (skb->len + 1) / 2);
}

/*
 * Driver functions
 */

static void
cs8900_receive (struct net_device *dev)
{
  struct sk_buff *skb;
  u16 status, length;

  status = cs8900_read (dev, PP_RxStatus);
  length = cs8900_read (dev, PP_RxLength);

  if (!(status & RxOK))
    {
      return;
    }

  if ((skb = dev_alloc_skb (length + 2)) == NULL)
    {
      return;
    }

  skb->dev = dev;
  skb_reserve (skb, 2);

  cs8900_frame_read (dev, skb, length);

  skb->protocol = eth_type_trans (skb, dev);

  netif_rx (skb);/* 送到协议栈 */
}

static int
cs8900_send_start (struct sk_buff *skb, struct net_device *dev)
{
  u16 status;

  disable_irq(dev->irq);

  netif_stop_queue (dev);

 
  cs8900_write (dev, PP_TxCMD, TxStart (After5));
  cs8900_write (dev, PP_TxLength, skb->len);

  status = cs8900_read (dev, PP_BusST);

  if ((status & TxBidErr))
    {
      printk (KERN_WARNING "%s: Invalid frame size %d!\n", dev->name,
	      skb->len);
      enable_irq(dev->irq);
      return (1);
    }

  if (!(status & Rdy4TxNOW))
    {
      printk (KERN_WARNING "%s: Transmit buffer not free!\n", dev->name);
      enable_irq(dev->irq);
      return (1);
    }

  cs8900_frame_write (dev, skb);

  dev_kfree_skb (skb);
  enable_irq(dev->irq);

  return (0);
}

static irqreturn_t
cs8900_interrupt (int irq, void *id, struct pt_regs *regs)
{
  struct net_device *dev = (struct net_device *) id;
  volatile u16 status;
  irqreturn_t handled = 0;
 
  while ((status = cs8900_read (dev, PP_ISQ)))
    {
      handled = 1;
      switch (RegNum (status))
	{
	case RxEvent:
	  cs8900_receive (dev);
	  break;

	case TxEvent:
	  netif_wake_queue (dev);
	  break;

	}
    }
  return IRQ_RETVAL (handled);
}

static int
cs8900_start (struct net_device *dev)
{
  int result;

  set_irq_type (dev->irq, IRQT_RISING);

  /* enable the ethernet controller */
  cs8900_set (dev, PP_RxCFG0.,
	      RxOKiE | BufferCRC | CRCerroriE | RuntiE | ExtradataiE);/* SET IRQ */
  cs8900_set (dev, PP_RxCTL, RxOKA | IndividualA | BroadcastA);
  cs8900_set (dev, PP_TxCFG, TxOKiE | Out_of_windowiE | JabberiE);
  cs8900_set (dev, PP_BufCFG,
	      Rdy4TxiE | RxMissiE | TxUnderruniE | TxColOvfiE | MissOvfloiE);
  cs8900_set (dev, PP_LineCTL, SerRxON | SerTxON);
  cs8900_set (dev, PP_BusCTL, EnableRQ);

  udelay (200);
  /* install interrupt handler */
  if ((result = request_irq (dev->irq, &cs8900_interrupt, SA_INTERRUPT, dev->name, dev)) < 0)
    {
      printk (KERN_ERR "%s: could not register interrupt %d\n", dev->name,
	      dev->irq);
      return (result);
    }

  /* start the queue */
  netif_start_queue (dev);/* 启动一个网卡的队列 */

  return (0);
}

static int
cs8900_stop (struct net_device *dev)
{
  /* disable ethernet controller */
  cs8900_write (dev, PP_BusCTL, 0);
  cs8900_write (dev, PP_TestCTL, 0);
  cs8900_write (dev, PP_SelfCTL, 0);
  cs8900_write (dev, PP_LineCTL, 0);
  cs8900_write (dev, PP_BufCFG, 0);
  cs8900_write (dev, PP_TxCFG, 0);
  cs8900_write (dev, PP_RxCTL, 0);
  cs8900_write (dev, PP_RxCFG, 0);

  /* uninstall interrupt handler */
  free_irq (dev->irq, dev);

  /* stop the queue */
  netif_stop_queue (dev);

  return (0);
}
/*
 * Driver initialization routines
 */

int
cs8900_probe (struct net_device *dev)
{
  int i;
  u16 value;

  printk (VERSION_STRING "\n");
  ether_setup (dev);							/* */

  dev->open = cs8900_start;
  dev->stop = cs8900_stop;
  dev->hard_start_xmit = cs8900_send_start;
 
  dev->dev_addr[0] = 0x00;
  dev->dev_addr[1] = 0x00;
  dev->dev_addr[2] = 0x3e;
  dev->dev_addr[3] = 0x26;
  dev->dev_addr[4] = 0x0a;
  dev->dev_addr[5] = 0x5b;

  dev->if_port = IF_PORT_10BASET;
  dev->base_addr = vSMDK2410_ETH_IO + 0x300;
  dev->irq = SMDK2410_ETH_IRQ;

  /* verify EISA registration number for Cirrus Logic */
  if ((value = cs8900_read (dev, PP_ProductID)) != EISA_REG_CODE)
    {
      printk (KERN_ERR "%s: incorrect signature 0x%.4x\n", dev->name, value);
      return (-ENXIO);
    }

  /* setup interrupt number */
  cs8900_write (dev, PP_IntNum, 0);

  for (i = 0; i < ETH_ALEN; i += 2)/* MAC */
    cs8900_write (dev, PP_IA + i,
		  dev->dev_addr[i] | (dev->dev_addr[i + 1] << 8));

  return (0);
}

static int __init
cs8900_init (void)
{
  strcpy (cs8900_dev.name, "eth%d");
  return (register_netdev (&cs8900_dev));
}

static void __exit
cs8900_cleanup (void)
{
  unregister_netdev (&cs8900_dev);
}

MODULE_AUTHOR ("Abraham van der Merwe <abraham at 2d3d.co.za>");
MODULE_DESCRIPTION (VERSION_STRING);
MODULE_LICENSE ("GPL");

module_init (cs8900_init);
module_exit (cs8900_cleanup);
