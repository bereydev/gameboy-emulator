#include "sidlib.h"
#include "lcdc.h"
#include "util.h" // for zero_init_var()
#include "error.h"
#include "gameboy.h"
#include <stdint.h>
#include <sys/time.h>

//global variables
gameboy_t gb;
struct timeval start;
struct timeval paused;

// Key press bits
#define MY_KEY_UP_BIT 0x01
#define MY_KEY_DOWN_BIT 0x02
#define MY_KEY_RIGHT_BIT 0x04
#define MY_KEY_LEFT_BIT 0x08
#define MY_KEY_A_BIT 0x10
#define MY_KEY_B_BIT 0x20
#define MY_KEY_PAGE_UP_BIT 0x40
#define MY_KEY_PAGE_DOWN_BIT 0x80

// image display scale factor
#define SCALE 2

// ======================================================================
uint64_t get_time_in_GB_cycles_since(struct timeval *from)
{
    struct timeval current_time;
    int err = gettimeofday(&current_time, NULL);
    //gettimeofday() returns 0 for success, or -1 for failure
    if (err == 0)
    {
        //timercmp returns true (nonzero) or false (0)
        int greater = timercmp(&current_time, from, >);
        if (!greater)
            return 0; //from is in the past

        struct timeval delta;
        timersub(&current_time, from, &delta);
        return (delta.tv_sec * GB_CYCLES_PER_S + (delta.tv_usec * GB_CYCLES_PER_S) / 1000000);
    }
    else
    {
        return 0;
    } //TODO what should I return otherwise?
}

// ======================================================================
static void set_grey(guchar *pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t)(3 * (row * width + col)); // 3 = RGB
    pixels[i + 2] = pixels[i + 1] = pixels[i] = grey;
}

// ======================================================================
static void generate_image(guchar *pixels, int height, int width)
{
    int err = gameboy_run_until(&gb, get_time_in_GB_cycles_since(&start));
    if (err != ERR_NONE)
        return;
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {

            uint8_t pixel = 0;
            if (image_get_pixel(&pixel, &gb.screen.display, (size_t)x / SCALE, (size_t)y / SCALE) != ERR_NONE)
                pixel = 0;
            set_grey(pixels, y, x, width, 255 - 85 * pixel);
        }
    }
}

// ======================================================================
#define do_key(X)                                  \
    do                                             \
    {                                              \
        if (!(psd->key_status & MY_KEY_##X##_BIT)) \
        {                                          \
            psd->key_status |= MY_KEY_##X##_BIT;   \
            puts(#X " key pressed");               \
        }                                          \
    } while (0)

static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t *const psd = data;
    if (psd == NULL)
        return FALSE;

    switch (keyval)
    {
    case GDK_KEY_Up:
    {
        do_key(UP);
        int err = joypad_key_pressed(&gb.pad, UP_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Down:
    {
        do_key(DOWN);
        int err = joypad_key_pressed(&gb.pad, DOWN_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Right:
    {
        do_key(RIGHT);
        int err = joypad_key_pressed(&gb.pad, RIGHT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Left:
    {
        do_key(LEFT);
        int err = joypad_key_pressed(&gb.pad, LEFT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case 'A':
    case 'a':
    {
        do_key(A);
        int err = joypad_key_pressed(&gb.pad, A_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case 'B':
    case 'b':
    {
        do_key(B);
        int err = joypad_key_pressed(&gb.pad, B_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Page_Up:
    {
        do_key(PAGE_UP);
        int err = joypad_key_pressed(&gb.pad, SELECT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Page_Down:
    {
        do_key(PAGE_DOWN);
        int err = joypad_key_pressed(&gb.pad, START_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_space:
    {
        if (psd->timeout_id > 0)
        {
            int failure = gettimeofday(&paused, NULL);
            if (failure)
                return FALSE;
        }
        else
        {
            struct timeval current_time;
            int failure = gettimeofday(&current_time, NULL);
            if (failure)
                return FALSE;
            timersub(&current_time, &paused, &paused);
            timeradd(&start, &paused, &start);
            timerclear(&paused);
        }
    }
    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key

// ======================================================================
#define do_key(X)                                                \
    do                                                           \
    {                                                            \
        if (psd->key_status & MY_KEY_##X##_BIT)                  \
        {                                                        \
            psd->key_status &= (unsigned char)~MY_KEY_##X##_BIT; \
            puts(#X " key released");                            \
        }                                                        \
    } while (0)

static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t *const psd = data;
    if (psd == NULL)
        return FALSE;

    switch (keyval)
    {
    case GDK_KEY_Up:
    {
        do_key(UP);
        int err = joypad_key_released(&gb.pad, UP_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Down:
    {
        do_key(DOWN);
        int err = joypad_key_released(&gb.pad, DOWN_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Right:
    {
        do_key(RIGHT);
        int err = joypad_key_released(&gb.pad, RIGHT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Left:
    {
        do_key(LEFT);
        int err = joypad_key_released(&gb.pad, LEFT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case 'A':
    case 'a':
    {
        do_key(A);
        int err = joypad_key_released(&gb.pad, A_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case 'B':
    case 'b':
    {
        do_key(B);
        int err = joypad_key_released(&gb.pad, B_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Page_Up:
    {
        do_key(PAGE_UP);
        int err = joypad_key_released(&gb.pad, SELECT_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }

    case GDK_KEY_Page_Down:
    {
        do_key(PAGE_DOWN);
        int err = joypad_key_released(&gb.pad, START_KEY);
        return err == ERR_NONE ? TRUE : FALSE;
    }
    }

    return FALSE;
}
#undef do_key

// ======================================================================
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        error(argv[0], "please provide input_file");
        return 1;
    }

    const char *const filename = argv[1];

    zero_init_var(gb);
    int err = gameboy_create(&gb, filename);
    if (err != ERR_NONE)
    {
        gameboy_free(&gb);
        return err;
    }

    //initialize start and paused
    int failure = gettimeofday(&start, NULL);
    if (failure)
        return failure;
    timerclear(&paused);

    sd_launch(&argc, &argv,
              sd_init("Gameboy Simulator", LCD_WIDTH * SCALE, LCD_HEIGHT * SCALE, 40,
                      generate_image, keypress_handler, keyrelease_handler));

    gameboy_free(&gb);

    return 0;
}
