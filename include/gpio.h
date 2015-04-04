#ifndef GPIO_H
#define GPIO_H

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define SYSFS_AI_DIR "/sys/devices/ocp.3/helper.18" 

int readGPIO(int pin);
float readAI(int pin);	

#endif
