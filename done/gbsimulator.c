#include "sidlib.h"
#include "lcdc.h"
#include "util.h"  // for zero_init_var()
#include <stdint.h>
#include <sys/time.h>


//global variables 
gameboy_t gb;
struct timeval start;
struct timeval paused;

// Key press bits
#define MY_KEY_UP_BIT    0x01
#define MY_KEY_DOWN_BIT  0x02
#define MY_KEY_RIGHT_BIT 0x04
#define MY_KEY_LEFT_BIT  0x08
#define MY_KEY_A_BIT     0x10
// ======================================================================
uint64_t get_time_in_GB_cycles_since(struct timeval* from)
{
    struct timeval* current_time = {0};
    int err = gettimeofday(current_time, NULL);
    if(err == 0) {
        int greater = timercmp(current_time, from, >);
        if(!greater) return 0; //from is in the past

        struct timeval* res = 0;
        timersub(current_time,from, res);
        return delta.tv_sec * GB_CYCLES_PER_S + (delta.tv_usec * GB_CYCLES_PER_S)/1000000;
    }
}

// ======================================================================
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

// ======================================================================
static void generate_image(guchar* pixels, int height, int width)
{
    int err = gameboy_run_until(&gb, get_time_in_GB_cycles_since(&start));
    if (err == ERR_NONE) {

        for(int i=0; i<height, ++i) {
            for (int j=0; j<width, ++j){

                uint8_t output = 0;
                err=image_get_pixel(&output, gb.screen.display, (size_t) i, (size_t) j);

                if(err == ERR_NONE) {
                    guchar color = 255 - 85*output;
                    set_grey(pixels, i, j, width, color);
                }
            }
        }
    }
}

// ======================================================================
#define do_key(X) \
    do { \
        if (! (psd->key_status & MY_KEY_ ## X ##_BIT)) { \
            psd->key_status |= MY_KEY_ ## X ##_BIT; \
            puts(#X " key pressed"); \
        } \
    } while(0)

static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;
    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key

// ======================================================================
#define do_key(X) \
    do { \
        if (psd->key_status & MY_KEY_ ## X ##_BIT) { \
          psd->key_status &= (unsigned char) ~MY_KEY_ ## X ##_BIT; \
            puts(#X " key released"); \
        } \
    } while(0)

static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;
    }

    return FALSE;
}
#undef do_key

// ======================================================================
int main(int argc, char *argv[])
{
    //initialize start and paused
    int failure = gettimeofday(&start, NULL);
    if (failure) return failure;
    timerclear(paused);


    sd_launch(&argc, &argv,
                  sd_init("demo", LCD_WIDTH, LCD_HEIGHT, 40,
                          generate_image, keypress_handler, keyrelease_handler));

    if (argc < 2) {
        error(argv[0], "please provide input_file");
        return 1;
    }

    const char* const filename = argv[1];

    zero_init_var(gb);
    int err = gameboy_create(&gb, filename);
    if (err != ERR_NONE) {
        gameboy_free(&gb);
        return err;
    }

     gameboy_free(&gb);

     return err;

}
