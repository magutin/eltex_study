#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/timer.h>            // Для set_timer 
#include <linux/init.h>
#include <linux/tty.h>              // Для port, MAX_NR_CONSOLES 
#include <linux/kd.h>               // Для KDSETLED 
#include <linux/vt_kern.h>          // Для fg_console 

MODULE_DESCRIPTION("Blink leds on keyboard");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergey Magutin");

static struct timer_list my_timer;
struct tty_driver *my_driver;         // для управления устройствами
static int *pstatus;                  // для чтения состояния ламп
int kbledstatus = 0;                  // состояние ламп

#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

// вызов функции по таймеру
void my_timer_callback(struct timer_list *timer)
{
    pstatus = &kbledstatus;

    if (*pstatus == ALL_LEDS_ON)
      *pstatus = RESTORE_LEDS;
    else
      *pstatus = ALL_LEDS_ON;
    
    // управление драйвером устрйоства через ioctl()
    (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,*pstatus);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(100));
}

static int __init kbleds_init(void)
{
        int i;
        printk(KERN_INFO "kbleds: loading\n");
        printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
        
        // ищем dev/console
        for (i = 0; i < MAX_NR_CONSOLES; i++) {
                if (!vc_cons[i].d)
                        break;
        }
        
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        
        // устанавливаем таймер в первый раз
        timer_setup(&my_timer, my_timer_callback, 0);
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(100));
        
        return 0;
}

static void __exit kbleds_cleanup(void){
        printk(KERN_INFO "kbleds: unloading...\n");
        
        del_timer(&my_timer); // удаляем таймер

        // управление драйвером устрйоства через ioctl()
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);