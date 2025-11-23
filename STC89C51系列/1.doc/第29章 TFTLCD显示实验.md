# 第二十九章 TFT LCD 显示实验（ILI9341，SPI 4线）

## 1. 导入

字符液晶适合显示文本，但在需要彩色图形、图片的项目中，TFT LCD 更具优势。本章选用常见的 ILI9341 控制器（240×320，2.4/2.8 寸屏），以“4线 SPI（SCK、MOSI、CS、DC）+ 复位 RST”的方式驱动，实现初始化、清屏、画点/线/矩形、简单文本（6×8 字体）等基础功能。

说明：

- 8051 的时钟与 GPIO 翻转速度有限，SPI 采用“位打（bit-bang）”实现，适合学习与中低速刷新。若需高帧率，建议改用“并口 8080 8/16 位总线”或更高主频 MCU。
- ILI9341 模块有多种：SPI/并口、分辨率、方向略有不同；本文以 240×320 SPI 型为例。

---

## 2. 硬件设计

- 电源与背光
  - VCC：一般为 3.3V；很多模块板载 LDO 可接 5V→请确认丝印（如“5V/3V3”）。
  - LED 背光：常接到 3.3V/5V，经限流或板载电阻。
- 接口信号（4 线 SPI + RST）
  - CS：片选（低有效）
  - DC（或 RS）：数据/命令选择（数据=1，命令=0）
  - SCK：SPI 时钟
  - MOSI（SDA）：主出从入
  - MISO：多数场景不用，可悬空
  - RST：硬件复位（低有效）
- 电平匹配
  - ILI9341 典型工作在 3.3V 逻辑；若 MCU 为 5V（如 89C52），需对 SCK/MOSI/CS/DC/RST 做电平转换（如 74LVC 系列、三极管/电阻分压）。部分模块自带电平转换，接 5V 亦可——务必查阅模块说明。
- 参考接线（可按需改口）
  - `P1.0 → CS`，`P1.1 → DC`，`P1.2 → RST`，`P1.3 → SCK`，`P1.4 → MOSI`
  - GND 共地，VCC/LED 按模块要求供电

---

## 3. 软件设计要点

- SPI 发送：MSB 先行；空闲 SCK 低电平。
- 区域设置：通过命令 `CASET(0x2A)`、`PASET(0x2B)` 设置写窗口，再 `RAMWR(0x2C)` 连续写入像素。
- 像素格式：设为 RGB565（命令 `COLMOD(0x3A)` 填入 `0x55`）。
- 方向：`MADCTL(0x36)` 控制横竖屏与颜色顺序（BGR/ RGB）。

---

## 4. 完整驱动代码（Keil C51，位打 SPI，ILI9341）

```c
#include <reg52.h>
#include <intrins.h>

/* ========== 引脚定义（按接线修改） ========== */
sbit TFT_CS   = P1^0;   // 片选（低有效）
sbit TFT_DC   = P1^1;   // 数据/命令：1=数据 0=命令
sbit TFT_RST  = P1^2;   // 复位（低有效）
sbit TFT_SCK  = P1^3;   // SPI时钟
sbit TFT_MOSI = P1^4;   // SPI主出从入（DIN）

/* ========== 公用延时 ========== */
static void tiny_delay(void) { _nop_(); _nop_(); _nop_(); _nop_(); }
void delay_ms(unsigned int ms){
    unsigned int i,j;
    for(i=0;i<ms;i++) for(j=0;j<125;j++);
}

/* ========== SPI位打发送 ========== */
static void spi_write8(unsigned char v){
    unsigned char i;
    for(i=0;i<8;i++){
        TFT_SCK = 0;
        TFT_MOSI = (v & 0x80) ? 1 : 0;  // 发送高位
        tiny_delay();
        TFT_SCK = 1;                    // 上升沿采样
        tiny_delay();
        v <<= 1;
    }
    TFT_SCK = 0;
}

/* ========== ILI9341 基本写接口 ========== */
static void tft_write_cmd(unsigned char cmd){
    TFT_CS = 0;
    TFT_DC = 0;           // 命令
    spi_write8(cmd);
    TFT_CS = 1;
}

static void tft_write_data8(unsigned char data){
    TFT_CS = 0;
    TFT_DC = 1;           // 数据
    spi_write8(data);
    TFT_CS = 1;
}

static void tft_write_data16(unsigned int data){
    TFT_CS = 0;
    TFT_DC = 1;
    spi_write8((unsigned char)(data >> 8));  // 高字节
    spi_write8((unsigned char)(data & 0xFF));// 低字节
    TFT_CS = 1;
}

/* 连续写数据流（提升效率） */
static void tft_begin_stream(void){
    TFT_CS = 0;
    TFT_DC = 1;
}
static void tft_stream_data16(unsigned int data){
    spi_write8((unsigned char)(data >> 8));
    spi_write8((unsigned char)(data & 0xFF));
}
static void tft_end_stream(void){
    TFT_CS = 1;
}

/* ========== 参数与颜色 ========== */
#define TFT_W 240
#define TFT_H 320

/* RGB888→RGB565 */
#define RGB565(r,g,b) ( ((r&0xF8)<<8) | ((g&0xFC)<<3) | ((b)>>3) )
/* 常用色 */
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_GRAY    0x8410

/* ========== 设置窗口 & 基础绘图 ========== */
static void tft_set_window(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1){
    // Column Address Set
    tft_write_cmd(0x2A);
    tft_write_data8(x0 >> 8); tft_write_data8(x0 & 0xFF);
    tft_write_data8(x1 >> 8); tft_write_data8(x1 & 0xFF);
    // Page Address Set
    tft_write_cmd(0x2B);
    tft_write_data8(y0 >> 8); tft_write_data8(y0 & 0xFF);
    tft_write_data8(y1 >> 8); tft_write_data8(y1 & 0xFF);
    // Memory Write
    tft_write_cmd(0x2C);
}

void tft_draw_pixel(unsigned int x, unsigned int y, unsigned int color){
    if(x>=TFT_W || y>=TFT_H) return;
    tft_set_window(x, y, x, y);
    tft_write_data16(color);
}

void tft_fill_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color){
    unsigned long count;
    unsigned int i;
    if(x>=TFT_W || y>=TFT_H) return;
    if(x+w-1 >= TFT_W) w = TFT_W - x;
    if(y+h-1 >= TFT_H) h = TFT_H - y;

    tft_set_window(x, y, x+w-1, y+h-1);
    count = (unsigned long)w * h;
    tft_begin_stream();
    while(count--) tft_stream_data16(color);
    tft_end_stream();
}

void tft_fill_screen(unsigned int color){
    tft_fill_rect(0, 0, TFT_W, TFT_H, color);
}

/* 画线（Bresenham） */
void tft_draw_line(int x0,int y0,int x1,int y1,unsigned int color){
    int dx = (x1>x0) ? (x1-x0) : (x0-x1);
    int sx = (x0<x1) ? 1 : -1;
    int dy = (y1>y0) ? (y0-y1) : (y1-y0);
    int sy = (y0<y1) ? 1 : -1;
    int err = dx + dy, e2;

    for(;;){
        tft_draw_pixel(x0,y0,color);
        if(x0==x1 && y0==y1) break;
        e2 = 2*err;
        if(e2 >= dy){ err += dy; x0 += sx; }
        if(e2 <= dx){ err += dx; y0 += sy; }
    }
}

/* 画矩形（边框） */
void tft_draw_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color){
    if(w==0 || h==0) return;
    tft_draw_line(x, y, x+w-1, y, color);
    tft_draw_line(x, y+h-1, x+w-1, y+h-1, color);
    tft_draw_line(x, y, x, y+h-1, color);
    tft_draw_line(x+w-1, y, x+w-1, y+h-1, color);
}

/* ========== ILI9341 初始化（常用序列） ========== */
void tft_reset(void){
    TFT_RST = 1; TFT_CS = 1; TFT_SCK = 0; TFT_MOSI = 0;
    delay_ms(5);
    TFT_RST = 0; delay_ms(20);
    TFT_RST = 1; delay_ms(120);
}

void tft_init(void){
    tft_reset();

    // ILI9341 init (通用稳定序列)
    tft_write_cmd(0xEF); tft_write_data8(0x03); tft_write_data8(0x80); tft_write_data8(0x02);
    tft_write_cmd(0xCF); tft_write_data8(0x00); tft_write_data8(0xC1); tft_write_data8(0x30);
    tft_write_cmd(0xED); tft_write_data8(0x64); tft_write_data8(0x03); tft_write_data8(0x12); tft_write_data8(0x81);
    tft_write_cmd(0xE8); tft_write_data8(0x85); tft_write_data8(0x00); tft_write_data8(0x78);
    tft_write_cmd(0xCB); tft_write_data8(0x39); tft_write_data8(0x2C); tft_write_data8(0x00); tft_write_data8(0x34); tft_write_data8(0x02);
    tft_write_cmd(0xF7); tft_write_data8(0x20);
    tft_write_cmd(0xEA); tft_write_data8(0x00); tft_write_data8(0x00);

    tft_write_cmd(0xC0); tft_write_data8(0x23);      // Power control VRH[5:0]
    tft_write_cmd(0xC1); tft_write_data8(0x10);      // Power control SAP[2:0];BT[3:0]
    tft_write_cmd(0xC5); tft_write_data8(0x3E); tft_write_data8(0x28); // VCM
    tft_write_cmd(0xC7); tft_write_data8(0x86);      // VCM control2

    // 内存访问控制：0x48 竖屏，BGR=1；如需横屏可改 0x28/0xE8 等
    tft_write_cmd(0x36); tft_write_data8(0x48);

    // 像素格式：16bpp RGB565
    tft_write_cmd(0x3A); tft_write_data8(0x55);

    tft_write_cmd(0xB1); tft_write_data8(0x00); tft_write_data8(0x18);  // Frame rate
    tft_write_cmd(0xB6); tft_write_data8(0x08); tft_write_data8(0x82); tft_write_data8(0x27); // Display Function
    tft_write_cmd(0xF2); tft_write_data8(0x00);  // 3Gamma off
    tft_write_cmd(0x26); tft_write_data8(0x01);  // Gamma curve

    // Gamma设置（可按需调整）
    tft_write_cmd(0xE0);
    tft_write_data8(0x0F); tft_write_data8(0x31); tft_write_data8(0x2B); tft_write_data8(0x0C);
    tft_write_data8(0x0E); tft_write_data8(0x08); tft_write_data8(0x4E); tft_write_data8(0xF1);
    tft_write_data8(0x37); tft_write_data8(0x07); tft_write_data8(0x10); tft_write_data8(0x03);
    tft_write_data8(0x0E); tft_write_data8(0x09); tft_write_data8(0x00);

    tft_write_cmd(0xE1);
    tft_write_data8(0x00); tft_write_data8(0x0E); tft_write_data8(0x14); tft_write_data8(0x03);
    tft_write_data8(0x11); tft_write_data8(0x07); tft_write_data8(0x31); tft_write_data8(0xC1);
    tft_write_data8(0x48); tft_write_data8(0x08); tft_write_data8(0x0F); tft_write_data8(0x0C);
    tft_write_data8(0x31); tft_write_data8(0x36); tft_write_data8(0x0F);

    // 退出睡眠、开显示
    tft_write_cmd(0x11); delay_ms(120);
    tft_write_cmd(0x29); delay_ms(20);

    tft_fill_screen(COLOR_BLACK);
}

/* ========== 6×8 点阵字体（精简版） ========== */
/* 说明：为控制篇幅，此处提供数字'0'~'9'、大写'A'~'Z'、空格及符号".:-/"等
   未覆盖字符显示为空白。你可按需扩展完整ASCII表。 */
typedef struct { char ch; unsigned char cols[6]; } glyph_t;
unsigned char code GLYPH_SPACE[6] = {0,0,0,0,0,0};
const glyph_t code font6x8[] = {
    {' ',{0,0,0,0,0,0}},
    {'0',{0x3E,0x51,0x49,0x45,0x3E,0x00}},
    {'1',{0x00,0x42,0x7F,0x40,0x00,0x00}},
    {'2',{0x62,0x51,0x49,0x49,0x46,0x00}},
    {'3',{0x22,0x49,0x49,0x49,0x36,0x00}},
    {'4',{0x18,0x14,0x12,0x7F,0x10,0x00}},
    {'5',{0x2F,0x49,0x49,0x49,0x31,0x00}},
    {'6',{0x3E,0x49,0x49,0x49,0x32,0x00}},
    {'7',{0x01,0x71,0x09,0x05,0x03,0x00}},
    {'8',{0x36,0x49,0x49,0x49,0x36,0x00}},
    {'9',{0x26,0x49,0x49,0x49,0x3E,0x00}},
    {'A',{0x7E,0x09,0x09,0x09,0x7E,0x00}},
    {'B',{0x7F,0x49,0x49,0x49,0x36,0x00}},
    {'C',{0x3E,0x41,0x41,0x41,0x22,0x00}},
    {'D',{0x7F,0x41,0x41,0x22,0x1C,0x00}},
    {'E',{0x7F,0x49,0x49,0x49,0x41,0x00}},
    {'F',{0x7F,0x09,0x09,0x09,0x01,0x00}},
    {'G',{0x3E,0x41,0x49,0x49,0x7A,0x00}},
    {'H',{0x7F,0x08,0x08,0x08,0x7F,0x00}},
    {'I',{0x41,0x41,0x7F,0x41,0x41,0x00}},
    {'J',{0x20,0x40,0x41,0x3F,0x01,0x00}},
    {'K',{0x7F,0x08,0x14,0x22,0x41,0x00}},
    {'L',{0x7F,0x40,0x40,0x40,0x40,0x00}},
    {'M',{0x7F,0x02,0x0C,0x02,0x7F,0x00}},
    {'N',{0x7F,0x04,0x08,0x10,0x7F,0x00}},
    {'O',{0x3E,0x41,0x41,0x41,0x3E,0x00}},
    {'P',{0x7F,0x09,0x09,0x09,0x06,0x00}},
    {'Q',{0x3E,0x41,0x51,0x21,0x5E,0x00}},
    {'R',{0x7F,0x09,0x19,0x29,0x46,0x00}},
    {'S',{0x26,0x49,0x49,0x49,0x32,0x00}},
    {'T',{0x01,0x01,0x7F,0x01,0x01,0x00}},
    {'U',{0x3F,0x40,0x40,0x40,0x3F,0x00}},
    {'V',{0x1F,0x20,0x40,0x20,0x1F,0x00}},
    {'W',{0x7F,0x20,0x18,0x20,0x7F,0x00}},
    {'X',{0x63,0x14,0x08,0x14,0x63,0x00}},
    {'Y',{0x07,0x08,0x70,0x08,0x07,0x00}},
    {'Z',{0x61,0x51,0x49,0x45,0x43,0x00}},
    {'.',{0x00,0x60,0x60,0x00,0x00,0x00}},
    {':',{0x00,0x36,0x36,0x00,0x00,0x00}},
    {'-',{0x08,0x08,0x08,0x08,0x08,0x00}},
    {'/',{0x40,0x30,0x0C,0x03,0x00,0x00}}
};
#define FONT_COUNT (sizeof(font6x8)/sizeof(font6x8[0]))

/* 查找字模；未找到返回空白 */
static const unsigned char* font_lookup(char ch){
    unsigned char i;
    for(i=0;i<FONT_COUNT;i++){
        if(font6x8[i].ch == ch) return font6x8[i].cols;
    }
    return GLYPH_SPACE;
}

/* 在(x,y)绘制6×8字符（前景/背景色），左上为起点 */
void tft_draw_char6x8(unsigned int x, unsigned int y, char ch, unsigned int fg, unsigned int bg){
    unsigned char col, row;
    const unsigned char* p = font_lookup(ch);
    if(x+6 > TFT_W || y+8 > TFT_H) return;

    tft_set_window(x, y, x+5, y+7);
    tft_write_cmd(0x2C); // RAMWR
    tft_begin_stream();
    for(row=0; row<8; row++){
        for(col=0; col<6; col++){
            if( (p[col] >> row) & 0x01 ) tft_stream_data16(fg);
            else                         tft_stream_data16(bg);
        }
    }
    tft_end_stream();
}

/* 打印字符串，等宽间隔=6像素 */
void tft_print6x8(unsigned int x, unsigned int y, const char* s, unsigned int fg, unsigned int bg){
    unsigned int cx = x;
    while(*s){
        if(*s=='\n'){ y += 8; cx = x; s++; continue; }
        tft_draw_char6x8(cx, y, *s, fg, bg);
        cx += 6;
        s++;
        if(cx+6 > TFT_W){ y+=8; cx = x; }
        if(y+8 > TFT_H) break;
    }
}

/* ========== 示例：主程序 ========== */
void main(void){
    unsigned int i;
    tft_init();

    // 彩条测试
    tft_fill_rect(0,   0, TFT_W, 40, COLOR_RED);
    tft_fill_rect(0,  40, TFT_W, 40, COLOR_GREEN);
    tft_fill_rect(0,  80, TFT_W, 40, COLOR_BLUE);
    tft_fill_rect(0, 120, TFT_W, 40, COLOR_CYAN);
    tft_fill_rect(0, 160, TFT_W, 40, COLOR_MAGENTA);
    tft_fill_rect(0, 200, TFT_W, 40, COLOR_YELLOW);
    tft_fill_rect(0, 240, TFT_W, 40, COLOR_GRAY);
    tft_fill_rect(0, 280, TFT_W, 40, COLOR_WHITE);

    // 文本与图形
    tft_print6x8(10, 10,  "TFT LCD ILI9341", COLOR_WHITE, COLOR_RED);
    tft_print6x8(10, 26,  "SPI 4-WIRE DEMO", COLOR_WHITE, COLOR_GREEN);
    tft_print6x8(10, 42,  "RES 240x320",     COLOR_WHITE, COLOR_BLUE);

    tft_draw_rect(20, 100, 200, 120, COLOR_WHITE);
    for(i=0;i<50;i++){
        tft_draw_line(20,100+i, 219,100+119-i, COLOR_WHITE);
    }

    // 简单计数刷新（避开全屏，以免过慢）
    {
        unsigned int cnt = 0;
        char buf[20];
        while(1){
            // 覆盖区域清背景
            tft_fill_rect(20, 230, 200, 16, COLOR_BLACK);

            // 构造字符串（只用到数字/大写）
            buf[0]='C';buf[1]='N';buf[2]='T';buf[3]=':';buf[4]=' '; // "CNT: "
            buf[5] = (cnt/10000)%10 + '0';
            buf[6] = (cnt/1000)%10  + '0';
            buf[7] = (cnt/100)%10   + '0';
            buf[8] = (cnt/10)%10    + '0';
            buf[9] = (cnt%10)       + '0';
            buf[10]='\0';
            tft_print6x8(30, 234, buf, COLOR_YELLOW, COLOR_BLACK);

            // 粗略延时
            delay_ms(200);
            cnt++;
        }
    }
}
```

要点说明：

- `MADCTL(0x36)` 写入 `0x48` 对应竖屏方向（X 镜像 + BGR）。若显示方向/颜色不对，请尝试 `0x28/0x88/E8` 等组合调整。
- 以上字库为“精简版”，覆盖数字与大写字母，足以展示计数、标题等。需要完整 ASCII/中文字体时，可外扩字库或字模芯片，或从上位机/外部存储加载。

---

## 5. 性能与优化建议

- 批量像素写入时使用“数据流”接口（`tft_begin_stream/tft_stream_data16/tft_end_stream`）减少 CS/DC 翻转开销。
- 降低刷新区域：避免频繁全屏 `fill_screen`，仅更新变化区域。
- SPI 加速：
  - 使用片上 SPI 外设替代位打。
  - 提高 MCU 主频，减少 `_nop_()` 次数。
- 并口 8080 替代：ILI9341 支持 8080 8/16 位并口，配合外部锁存与时序，可大幅提升速率（需要更多 IO）。

---

## 6. 常见问题与排查

- 屏幕无显示/全白：
  - 复位时序不足；电源/背光未供电；CS/DC 接错；模块并非 ILI9341（型号不匹配）。
- 颜色颠倒（蓝变红）：
  - `MADCTL` 的 BGR/RGB 位未设置，尝试改 `0x36` 的参数（添加/去除 0x08）。
- 显示方向不合预期：
  - 调整 `MADCTL` 的 MX/MY/MV 位（0x40/0x80/0x20）。
- 出现杂点/花屏：
  - SPI 时序过快、线过长；3.3V/5V 电平不匹配；接地不良；供电纹波大（为屏加 10µF+0.1µF 去耦）。
- 刷新过慢：
  - 8051 位打 SPI 限制；尽量减少像素写入量，或换并口/更快 MCU。

---

## 7. 扩展方向

- 完整 6×8/8×16 ASCII 字库、中文点阵（GB2312 匹配）与字库芯片（如 HY-字库）。
- 图片显示：将 RGB565 位图存于外部 SPI Flash/SD 卡，按窗口流式写入。
- 触摸屏（XPT2046）联动，做菜单界面。
- 换用 ST7735（128×160）、ST7789（240×240/240×320）控制器，仅需调整初始化与分辨率。

---

## 8. 小结

本章完成了 ILI9341 彩屏的 SPI 基础驱动：初始化、窗口设定、像素/线条/矩形与文本显示。通过合理的时序与区域刷新，8051 同样能在 TFT 上实现直观的人机界面。
