#include <REGX52.H>

// 共阴极段码数据
unsigned char gsmg_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};    
// 延时函数
void delay_us(unsigned int ten_us)
{
    while(ten_us--);    
}

// 扫描函数
int key_scan()
{
    unsigned int key_value; // 定义键值

    P1 = 0xf7; // 给第一列赋值0，其余全为1（二进制1111 0111）代表第1列为低电平，P1是连接矩阵按键的

    /* 
    为什么可以实现这个功能？
    这行代码是将 P1 的值设为 0xF7，即二进制形式为 11110111。
    11110111即代表了P1.7~P1.0,意味着，P1.7~P1.4和P1.2~P1.0被赋值为1，而P1.3被赋值为0
    看我们的接线就知道-P1.3确实是接在L1上的-即代表了第一列
    从而我们就完成了给第一列赋0（即输出模式）的操作-其他位全不打开-行列扫描法-
	先给一列为低电平，其余列为高电平，然后立即检测一次各行是否为低电平，若检测到某一行为低电平，则我们便可确认当前被按下的键是哪一行哪一列的
    */

    if(P1 != 0xf7) // 判断第一列按键是否按下-如果按下（即P1口不等于1111 0111）执行下面语句
    {
        delay_us(1000);// 消抖
        switch(P1) // 保存第一列按键按下后的键值    
        {
        /*
        下面对比段码值，再返回就行了，注意要返回第一列的数据嗷
        我们举一个例子，1234
                        5678
                        9ABC
                        DF
        比如第一列的5，通过查询段码值，是0xb7
        那么我们直接写case 0xb7 让循环找这个数据，如果找到了，就返回5，然后数码管就显示5了
        */
            case 0x77: 
                key_value = 1;
                break;
            case 0xb7: 
                key_value = 5;
                break;
            case 0xd7: 
                key_value = 9;
                break;
            case 0xe7: 
                key_value = 13; // D
                break;
        }
    }while(P1 != 0xf7); // 等待按键松开    

    P1 = 0xfb;//给第二列赋值0，其余全为1
    if(P1 != 0xfb)//判断第二列按键是否按下
    {
        delay_us(1000);//消抖
        switch(P1)//保存第二列按键按下后的键值    
        {
            case 0x7b: 
                key_value = 2;
                break;
            case 0xbb: 
                key_value = 6;
                break;
            case 0xdb: 
                key_value = 10;
                break;
            case 0xeb: 
                key_value = 14;
                break;
        }
    }while(P1 != 0xfb);//等待按键松开    

    P1 = 0xfd; // 给第三列赋值0，其余全为1
    if(P1 != 0xfd) // 判断第三列按键是否按下
    {
        delay_us(1000); // 消抖
        switch(P1) // 保存第三列按键按下后的键值    
        {
            case 0x7d: 
                key_value = 3;
                break;
            case 0xbd: 
                key_value = 7;
                break;
            case 0xdd: 
                key_value = 11;
                break;
            case 0xed: 
                key_value = 15;
                break;
        }
    }while(P1 != 0xfd);//等待按键松开    

    P1 = 0xfe;//给第四列赋值0，其余全为1
    if(P1 != 0xfe)//判断第四列按键是否按下
    {
        delay_us(1000);//消抖
        switch(P1)//保存第四列按键按下后的键值    
        {
            case 0x7e: 
                key_value = 4;
                break;
            case 0xbe: 
                key_value = 8;
                break;
            case 0xde: 
                key_value = 12;
                break;
            case 0xee: 
                key_value = 16;
                break;
        }
    }while(P1 != 0xfe);//等待按键松开

    return key_value; // 返回得到的值       
}


// 主函数
void main()
{    
    int key; // 定义需要返回的值

    while(1)
    {
        key = key_scan(); // 返回值即函数中返回的值
        if(key != 0)
            P0 = ~gsmg_code[key-1]; // 得到的按键值减1换算成数组下标对应0-F段码
        // 为什么需要减1呢？（1~f）    、
        // 因为学过C语言知道，数组的下标是从0开始的，而键值是从1开始的，所以要key要减1才能对得上数组
    }        
}