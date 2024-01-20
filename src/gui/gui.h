#include <gtk/gtk.h>
#include <gdk/gdk.h>
#define NO_FILE_LOADED (loaded_fn[0] == '\0')
#define SH_COMMAND "./run_program "
#define VOLUME_PATH "/utils/"

/* Non alphanumeric characters we consider part of a word. */
#define TERM_WORD_CHARS "-./?%&_=+@~:"

char *PROGNAME;
char *title = "Untitled - LC3 VIRTUAL MACHINE";
char *icon  = "gtk-notepad.png";
char *loaded_fn = NULL;
int errno;

// Main window
GtkWidget *window, *vbox;
GtkWindow *gwindow;
GtkAccelGroup *accel = NULL;
GtkWidget *statusbar;

GtkWidget *paned;

GdkAtom atom;
GtkClipboard *clipboard;

// Menubar
GtkWidget *menubar, *sep;

// File menu
GtkWidget *filemenu, *file;
GtkWidget *new, *openf, *save, *saveas, *compile, *run, *quit;

// Edit menu
GtkWidget *editmenu, *edit;
GtkWidget *cut, *copy, *paste, *delete;
GtkWidget *selectall, *time_date;
GtkWidget *wll, *select_font;

// Connect menu
GtkWidget *connectmenu, *connect;
GtkWidget *sendto, *receivefrm;


// Help menu
GtkWidget *helpmenu, *help;
GtkWidget *about;

// GtkTextView
GtkWidget *textarea;
GtkTextBuffer *buffer;
GtkWidget *scrolledwindow;
gboolean modified = FALSE;
char wrapping = 1; // Wrapping will be set to 0 by default

GtkWidget *terminal;
/* Non alphanumeric characters we consider part of a word. */
#define TERM_WORD_CHARS "-./?%&_=+@~:"
/* Minimum prefix to try completing a word. */
#define TERM_DABBREV_MIN_PREFIX 2
/* Terminal opacity */
#define TERM_OPACITY 1
/* Terminal font */
#define TERM_FONT "Iosevka Term SS18 10"


GtkWidget *output_window;
GtkWidget *output_text;
GtkTextBuffer *output_buffer; 
GtkWidget *output_frame;
GtkTextIter end;

void gtk_statusbar_update_lncol(void);
char gtk_notepad_open_file(const char*);
void gtk_notepad_save();
void gtk_notepad_saveas(void);

#define WIDTH         600
#define HEIGHT        400
#define R_FACTOR        5                 // window resize factor

#define TITLE         "Simple Terminal"
#define FONT          "Fantasque Sans Mono"
#define FONT_SIZE     10
#define FONT_SCALE    1                  // Leave this like this
#define SCALE_FACTOR  0.1                // Leave this like this
#define CURSOR_AH     TRUE               // auto hide the cursor while typing
#define REWRAP        TRUE               // rewrap terminal based on content size
#define BELL          FALSE              // no bell sound
#define MAX_TERM      10

// colors
#define CLR_0         0x1C1C1C
#define CLR_1         0xD81860
#define CLR_2         0x60FF60
#define CLR_3         0xF9FD75
#define CLR_4         0x4695C8
#define CLR_5         0xA78EDB
#define CLR_6         0x43AFCE
#define CLR_7         0xF3EBE2
#define CLR_8         0x1C1C1C
#define CLR_9         0xD81860
#define CLR_10        0x60FF60
#define CLR_11        0xF9FD75
#define CLR_12        0x4695C8
#define CLR_13        0xA78EDB
#define CLR_14        0x43AFCE
#define CLR_15        0xF3EBE2
#define PALETTE_SIZE  16

#define MODIFIER GDK_CONTROL_MASK|GDK_MOD1_MASK 

#define COPY                GDK_KEY_c
#define PASTE               GDK_KEY_v
#define INCREASE_FONT       GDK_KEY_p
#define DECREASE_FONT       GDK_KEY_o
#define RESET_FONT          GDK_KEY_i
#define NEW_TERMINAL        GDK_KEY_k
#define NEXT_TERMINAL       GDK_KEY_m
#define PREVIOUS_TERMINAL   GDK_KEY_n

// used to parse colors
#define CLR_R(x)       (((x) & 0xff0000) >> 16)
#define CLR_G(x)       (((x) & 0x00ff00) >>  8)
#define CLR_B(x)       (((x) & 0x0000ff) >>  0)
#define CLR_16(x)      ((double)(x) / 0xff)
#define CLR_GDKA(x, a) (const GdkRGBA){ .red = CLR_16(CLR_R(x)), .green = CLR_16(CLR_G(x)), .blue = CLR_16(CLR_B(x)), .alpha = a }
#define CLR_GDK(x)     CLR_GDKA(x, 0)
//#define CLR_GDK(x) (const GdkRGBA){ .red = CLR_16(CLR_R(x)), \
//                                    .green = CLR_16(CLR_G(x)), \
//                                    .blue = CLR_16(CLR_B(x)) }


const GdkRGBA PALETTE[] = {                         // palette color converted from config file
    CLR_GDK(CLR_0), CLR_GDK(CLR_1), CLR_GDK(CLR_2),
    CLR_GDK(CLR_3), CLR_GDK(CLR_4), CLR_GDK(CLR_5),
    CLR_GDK(CLR_6), CLR_GDK(CLR_7), CLR_GDK(CLR_8),
    CLR_GDK(CLR_9), CLR_GDK(CLR_10), CLR_GDK(CLR_11),
    CLR_GDK(CLR_12), CLR_GDK(CLR_13), CLR_GDK(CLR_14), CLR_GDK(CLR_15) };

GtkWidget * term[MAX_TERM];

gboolean sticked = FALSE;                           // used to indicate if window should persist on all workspace
int current = 0;                                    // indicates the current terminal
int used = 0;                                       // indicates how many terminals are open
int W = WIDTH;                                      // WIDTH for the window
int H = HEIGHT;                                     // HEIGHT for the window
int R = R_FACTOR;                                   // resize factor for the window

#define MIT_LICENSE "MIT License\n\Copyright (c) 2023 Samuele Stronati\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all\n\
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
SOFTWARE."
