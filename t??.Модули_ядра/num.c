#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
 
 
int main(void)
{
    int fd = open("/dev/console",O_NOCTTY);
    if (fd == -1){perror("open");return -1;}
 
    int state = 0;
    int i = 0;
    // получаем состояние из переменной сотояний
    
    while(i!=50){
        if (-1 == ioctl(fd, KDGKBLED, &state)){perror("ioctl");close(fd);return -1;}
        
        state ^= K_NUMLOCK;
        //if (-1 == ioctl(fd, KDSKBLED, state)){perror("ioctl set");close(fd);return -1;}
        state ^= K_CAPSLOCK;
        //state ^= K_POWER;    
 
        // устанавливаем нвоое состояние
        if (-1 == ioctl(fd, KDSKBLED, state)){perror("ioctl set");close(fd);return -1;}

        sleep(1);
        i++;
    }
    
    
    close(fd);
    return 0;
}