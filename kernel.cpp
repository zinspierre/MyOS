#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"


void printf(char* str) {
    static uint16_t*  VideoMemory = (uint16_t*) 0xB8000;

    static uint8_t x = 0, y = 0;

    // *((char*)0xb8000) = 0x4c;
    // *((char*)0xb8001) = 0x07;

    // *((int*)0xb8000)=0x0759074c;
    // *((int*)0xb8004)=0x074e074f;
    // *((short*)0xb8008)=0x0720;
    // *((int*)0xb800a)=0x074c074f;

    for(int i = 0; str[i] != '\0'; ++i) {

        switch(str[i]) {
            case '\n':
                y++;
                x = 0;
                break;
            default:
                uint16_t s = (0x0700 | str[i]);
                VideoMemory[80*y+x] = s;
                ++x;
                break;
        }
        if(x >= 80) {
            ++y;
            x = 0;
        }
        if(y >= 25) {
            for(y = 0; y < 25; ++y)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printHex(uint8_t n) {
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(n >> 4) & 0xF];
    foo[1] = hex[n & 0xF];
    printf(foo);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void onKeyDown(char c) {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
private:
    int8_t x, y;
public:

    MouseToConsole() {
        // offset = 0;
        // buttons = 0;

        static uint16_t*  VideoMemory = (uint16_t*) 0xB8000;


        VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4) |
                              ((VideoMemory[80*12+40] & 0x0F00) << 4) |
                              (VideoMemory[80*12+40] & 0x00FF);


    }
    void onMouseMove(int xoffset, int yoffset) {
        static uint16_t*  VideoMemory = (uint16_t*) 0xB8000;

        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) |
                          ((VideoMemory[80*y+x] & 0x0F00) << 4) |
                          (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x < 0) x = 0;
        if(x >= 80) x = 79;

        y -= yoffset;
        if(y < 0) y = 0;
        if(y >= 25) y = 24;


        VideoMemory[80*y+x] = ((0xFFFF & 0xF000) >> 4) |
                              ((VideoMemory[80*y+x] & 0x0F00) << 4) |
                              (VideoMemory[80*y+x] & 0x00FF);
    }
    void onMouseDown(uint8_t button) {

    }
    void onMouseUp(uint8_t button) {

    }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstrutors() {
     for(constructor* i = &start_ctors; i != &end_ctors; ++i)
        (*i)();

}

extern "C" void kernelMain(void* mutliboot_structure, uint32_t magicnumber)
{
    printf("Lacazette Tolisso!!!\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    printf("Initializing Hardware, Stage 1\n");

    DriverManager drvManager;

    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
    drvManager.AddDriver(&mouse);
    printf("Initializing Hardware, Stage 2\n");

    drvManager.ActivateAll();
    interrupts.Activate();
    printf("Initializing Hardware, Stage 3\n");


    while (1);
}
