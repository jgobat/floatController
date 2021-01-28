#ifndef _BUSYBOX_H
#define _BUSYBOX_H

//config:config VI
//config:	bool "vi (23 kb)"
//config:	default y
//config:	help
//config:	'vi' is a text editor. More specifically, it is the One True
//config:	text editor <grin>. It does, however, have a rather steep
//config:	learning curve. If you are not already comfortable with 'vi'
//config:	you may wish to use something else.
//config:
#define CONFIG_FEATURE_VI_MAX_LEN 4096
#define ENABLE_FEATURE_VI_8BIT 0
#define ENABLE_FEATURE_VI_COLON 1
#define ENABLE_FEATURE_VI_YANKMARK 1
#define IF_FEATURE_VI_YANKMARK(...) __VA_ARGS__
#define ENABLE_FEATURE_VI_SEARCH 1
#define IF_FEATURE_VI_SEARCH(...) __VA_ARGS__
#define ENABLE_FEATURE_VI_REGEX_SEARCH 0
#define ENABLE_FEATURE_VI_USE_SIGNALS 0
#define ENABLE_FEATURE_VI_DOT_CMD 1
#define ENABLE_FEATURE_VI_READONLY 0
#define IF_FEATURE_VI_READONLY(...) __VA_ARGS__
#define ENABLE_FEATURE_VI_SETOPTS 1
#define ENABLE_FEATURE_VI_SET 1
#define ENABLE_FEATURE_VI_WIN_RESIZE  0
#define ENABLE_FEATURE_VI_ASK_TERMINAL 1
#define IF_FEATURE_VI_ASK_TERMINAL(...) __VA_ARGS__
#define ENABLE_FEATURE_VI_UNDO 1
#define ENABLE_FEATURE_VI_UNDO_QUEUE 1
#define CONFIG_FEATURE_VI_UNDO_QUEUE_MAX 256
#define ENABLE_FEATURE_EDITING_ASK_TERMINAL 0


// Should be after libbb.h: on some systems regex.h needs sys/types.h:
#if ENABLE_FEATURE_VI_REGEX_SEARCH
# include <regex.h>
#endif

// the CRASHME code is unmaintained, and doesn't currently build
#define ENABLE_FEATURE_VI_CRASHME 0


#if ENABLE_LOCALE_SUPPORT

#if ENABLE_FEATURE_VI_8BIT
//FIXME: this does not work properly for Unicode anyway
# define Isprint(c) (isprint)(c)
#else
# define Isprint(c) isprint_asciionly(c)
#endif

#else

// 0x9b is Meta-ESC
#if ENABLE_FEATURE_VI_8BIT
# define Isprint(c) ((unsigned char)(c) >= ' ' && (c) != 0x7f && (unsigned char)(c) != 0x9b)
#else
# define Isprint(c) ((unsigned char)(c) >= ' ' && (unsigned char)(c) < 0x7f)
#endif

#endif

//typedef unsigned long uintptr_t;
// typedef unsigned long uint64_t;
typedef unsigned long uint32_t;
// typedef long int32_t;
typedef long long int64_t;
typedef short smallint;
typedef unsigned char smalluint;
typedef unsigned char uint8_t;

#define FALSE ((int) 0)
#define TRUE ((int) 1)

#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))

/* "Keycodes" that report an escape sequence.
 * We use something which fits into signed char,
 * yet doesn't represent any valid Unicode character.
 * Also, -1 is reserved for error indication and we don't use it. */
enum {
    KEYCODE_UP        =  -2,
    KEYCODE_DOWN      =  -3,
    KEYCODE_RIGHT     =  -4,
    KEYCODE_LEFT      =  -5,
    KEYCODE_HOME      =  -6,
    KEYCODE_END       =  -7,
    KEYCODE_INSERT    =  -8,
    KEYCODE_DELETE    =  -9,
    KEYCODE_PAGEUP    = -10,
    KEYCODE_PAGEDOWN  = -11,
    KEYCODE_BACKSPACE = -12, /* Used only if Alt/Ctrl/Shifted */
    KEYCODE_D         = -13, /* Used only if Alted */
#if 0
    KEYCODE_FUN1      = ,
    KEYCODE_FUN2      = ,
    KEYCODE_FUN3      = ,
    KEYCODE_FUN4      = ,
    KEYCODE_FUN5      = ,
    KEYCODE_FUN6      = ,
    KEYCODE_FUN7      = ,
    KEYCODE_FUN8      = ,
    KEYCODE_FUN9      = ,
    KEYCODE_FUN10     = ,
    KEYCODE_FUN11     = ,
    KEYCODE_FUN12     = ,
#endif
    /* ^^^^^ Be sure that last defined value is small enough.
     * Current read_key() code allows going up to -32 (0xfff..fffe0).
     * This gives three upper bits in LSB to play with:
     * KEYCODE_foo values are 0xfff..fffXX, lowest XX bits are: scavvvvv,
     * s=0 if SHIFT, c=0 if CTRL, a=0 if ALT,
     * vvvvv bits are the same for same key regardless of "shift bits".
     */
    //KEYCODE_SHIFT_...   = KEYCODE_...   & ~0x80,
    KEYCODE_CTRL_RIGHT    = KEYCODE_RIGHT & ~0x40,
    KEYCODE_CTRL_LEFT     = KEYCODE_LEFT  & ~0x40,
    KEYCODE_ALT_RIGHT     = KEYCODE_RIGHT & ~0x20,
    KEYCODE_ALT_LEFT      = KEYCODE_LEFT  & ~0x20,
    KEYCODE_ALT_BACKSPACE = KEYCODE_BACKSPACE & ~0x20,
    KEYCODE_ALT_D         = KEYCODE_D     & ~0x20,

    KEYCODE_CURSOR_POS = -0x100, /* 0xfff..fff00 */
    /* How long is the longest ESC sequence we know?
     * We want it big enough to be able to contain
     * cursor position sequence "ESC [ 9999 ; 9999 R"
     */
    KEYCODE_BUFFER_SIZE = 16
};

char *skip_whitespace(const char *s);
char *skip_non_whitespace(const char *s);
void *xzalloc(size_t size);
char *last_char_is(const char *s, int c);
char *strchrnul(const char *s, int c);
void* memrchr(const void *s, int c, size_t n);
int64_t read_key(int fd, char *buffer, int timeout);


#endif // _BUSBOX_H
