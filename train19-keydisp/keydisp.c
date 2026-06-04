#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define GPIO_PHY_BASEADDR 0x3F200000
#define GPIO_AREA_SIZE    4096
#define GPIO_GPLEV0       0x0034

#define BIT_BROWN (1 << 22)
#define BIT_RED   (1 << 23)

typedef struct {
    int pin_bit;
    int last_raw_stat;
    int is_pressed;
    int lcd_row;
    char *msg;

    time_t released_time;
    int waiting_clear;

} ButtonObj;

/* 茶ボタン：上段 hello */
/* 赤ボタン：下段 world */
ButtonObj buttons[2] = {
    { BIT_BROWN, 0, 0, 0, "hello", 0, 0 },
    { BIT_RED,   0, 0, 1, "world", 0, 0 }
};

int i2c_fd;

extern void initLCD(int fd);
extern int lcd_datawrite(int fd, char *s);
extern int location(int fd, int y);

int main()
{
    /* I2Cオープン */
    i2c_fd = open("/dev/i2c-1", O_RDWR);

    if (i2c_fd < 0) {
        perror("i2c open");
        return 1;
    }

    /* LCDのI2Cアドレス設定 */
    if (ioctl(i2c_fd, I2C_SLAVE, 0x3e) < 0) {
        perror("ioctl");
        close(i2c_fd);
        return 1;
    }

    /* LCD初期化 */
    initLCD(i2c_fd);

    /* GPIOアクセス用 */
    int mem_fd = open("/dev/mem", O_RDWR);

    if (mem_fd < 0) {
        perror("mem open");
        close(i2c_fd);
        return 1;
    }

    /* GPIOメモリマップ */
    void *gpio_baseaddr = mmap(
        NULL,
        GPIO_AREA_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_PHY_BASEADDR
    );

    if (gpio_baseaddr == MAP_FAILED) {
        perror("mmap");
        close(mem_fd);
        close(i2c_fd);
        return 1;
    }

    while (1) {

        /* GPIO状態読み取り */
        unsigned int gplev0 =
            *((unsigned int *)((char *)gpio_baseaddr + GPIO_GPLEV0));

        for (int i = 0; i < 2; i++) {

            ButtonObj *b = &buttons[i];

            /*
             * GPIOは通常：
             * 押す   -> 0
             * 離す   -> 1
             * なので反転して扱う
             */
            int current_raw =
                (gplev0 & b->pin_bit) ? 0 : 1;

            /* 押された瞬間 */
            if (current_raw == 1 &&
                b->last_raw_stat == 0) {

                b->is_pressed = 1;

                /* 消去待ちキャンセル */
                b->waiting_clear = 0;

                /* LCD表示 */
                location(i2c_fd, b->lcd_row);
                lcd_datawrite(i2c_fd, b->msg);
            }

            /* 離された瞬間 */
            else if (current_raw == 0 &&
                     b->last_raw_stat == 1) {

                b->is_pressed = 0;

                /* 離した時刻保存 */
                b->released_time = time(NULL);

                /* 1秒後消去開始 */
                b->waiting_clear = 1;
            }

            /* 前回状態更新 */
            b->last_raw_stat = current_raw;

            /* 離して1秒経過したか確認 */
            if (b->waiting_clear == 1) {

                time_t now = time(NULL);

                if (now - b->released_time >= 1) {

                    b->waiting_clear = 0;

                    /* LCD消去 */
                    location(i2c_fd, b->lcd_row);
                    lcd_datawrite(i2c_fd, "        ");
                }
            }
        }

        usleep(10000);
    }

    munmap(gpio_baseaddr, GPIO_AREA_SIZE);

    close(mem_fd);
    close(i2c_fd);

    return 0;
}
