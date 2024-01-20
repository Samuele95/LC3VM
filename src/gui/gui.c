#include "gui.h"
#include <vte/vte.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <glib.h>
void setup_terminal(void);

void send_file(GtkEntry** args) {
    char cmd[BUFSIZ];
    strcpy(cmd, "bash ./filesender.sh send ");
    strcat(cmd, gtk_entry_get_text(args[0]));
    strcat(cmd, gtk_entry_get_text(args[1]));
    execute_terminal_cmd(cmd, NULL);
    free(args);
}


void open_send_dialog(void) {

    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *grid;
    GtkWidget *hostlabel, *filelabel;
    GtkWidget *button;
    static GtkEntry *htextbox, *ftextbox;
    char cmd[BUFSIZ];
    GtkEntry** argsource = NULL;

    dialog = gtk_dialog_new_with_buttons ("Send file to another VM",
                                          window,
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          NULL);

    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    grid = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (content_area), grid);

    hostlabel = gtk_label_new("Host IP Address: ");
    gtk_grid_attach(GTK_GRID(grid), hostlabel, 0, 0, 1, 1);
    htextbox = gtk_entry_new();
    gtk_entry_set_placeholder_text(htextbox, "enter IP Address here");
    gtk_grid_attach(GTK_GRID(grid), htextbox, 1, 0, 1, 1);
    
    filelabel = gtk_label_new("File: ");
    gtk_grid_attach(GTK_GRID(grid), filelabel, 0, 1, 1, 1);
    ftextbox = gtk_entry_new();
    gtk_entry_set_placeholder_text(ftextbox, "enter filename here");
    gtk_entry_set_input_hints (ftextbox, GTK_INPUT_HINT_WORD_COMPLETION | GTK_INPUT_HINT_LOWERCASE);
    gtk_grid_attach(GTK_GRID(grid), ftextbox, 1, 1, 1, 1);

    execute_terminal_cmd("clear", NULL);
    gtk_widget_show_all (dialog);    
    
    gint result = gtk_dialog_run(GTK_DIALOG (dialog));
    switch (result)
        {
        case GTK_RESPONSE_OK:
            strcpy(cmd, "bash ./filesender.sh send ");
            strcat(cmd, VOLUME_PATH);
            strcat(cmd, gtk_entry_get_text(ftextbox));
            strcat(cmd, " ");
            strcat(cmd, gtk_entry_get_text(htextbox));
            execute_terminal_cmd(cmd, NULL);
            break;
        default:
            break;
        }

    
    //args[0] = gtk_entry_get_text(ftextbox);
    //args[1] = gtk_entry_get_text(htextbox);
    
    //strcpy(cmd, "bash ./filesender.sh send ");
    //strcat(cmd, args[0]);
    //strcat(cmd, args[1]);
    //strcat(cmd, "test.lc3 ");
    //strcat(cmd,"127.0.0.1");
}

void open_receive_dialog(void) {

    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *grid;
    GtkWidget *filelabel;
    GtkWidget *filebutton;
    static GtkEntry *ftextbox;
    char cmd[BUFSIZ];
    
    dialog = gtk_dialog_new_with_buttons ("Receive file from VM",
                                          window,
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    grid = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (content_area), grid);
    
    filelabel = gtk_label_new("File to receive: ");
    gtk_grid_attach(GTK_GRID(grid), filelabel, 0, 0, 1, 1);
    
    ftextbox = gtk_entry_new();
    gtk_entry_set_placeholder_text(ftextbox, "enter filename here");
    gtk_entry_set_input_hints (ftextbox, GTK_INPUT_HINT_WORD_COMPLETION | GTK_INPUT_HINT_LOWERCASE);
    gtk_grid_attach(GTK_GRID(grid), ftextbox, 1, 0, 1, 1);
             
    execute_terminal_cmd("clear", NULL);         
    gtk_widget_show_all (dialog);
    
    gint result = gtk_dialog_run(GTK_DIALOG (dialog));
    switch (result)
        {
        case GTK_RESPONSE_OK:
            strcpy(cmd, "bash ./filesender.sh receive ");
            strcat(cmd, VOLUME_PATH);
            strcat(cmd, gtk_entry_get_text(ftextbox));
            execute_terminal_cmd(cmd, NULL);
            break;
        default:
            break;
        }
}

void execute_terminal_cmd(char* argcmd, char* argument) {
    char cmd[BUFSIZ];
    if (argcmd == NULL) {
        perror("Executing empty command!");
        exit(EXIT_FAILURE);
    }
    strcpy(cmd, argcmd);
    if (argument != NULL)
        strcat(cmd, argument);
  
    gchar **envp = g_get_environ();
    gchar **command = (gchar *[]){ g_strdup(g_environ_getenv(envp, "SHELL")), "-c", cmd, NULL};
    g_strfreev(envp);
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT, NULL, command, NULL,
        0, NULL, NULL, NULL,
        250, NULL, NULL, NULL);
}



void print_usage(void) {
    fprintf(stderr, "%s: usage: %s [file]\n",
            PROGNAME,
            PROGNAME);
}


void gtk_notepad_set_title(const char* filename) {
    char *fn = malloc(strlen(filename) + 1);
    strcpy(fn, filename);

    int i = 0;
    int index = 0;
    char *slash = strrchr(filename, '/');
    if (slash) {
        index = (int)(slash - filename);

        for (i=0; i <= index; i++, fn++);
    }

    char *_title = malloc(strlen(" - LC3 VIRTUAL MACHINE") + strlen(fn) + 1);
    strcpy(_title, fn);
    if (slash)
        for (i=0; i <= index; i++, fn--);

    strcat(_title, " - LC3 VIRTUAL MACHINE");

    gtk_window_set_title(gwindow, _title);

    free(_title);
    free(fn);
}


void gtk_notepad_show_about_box(void) {
    GtkWidget *dialog = gtk_about_dialog_new();
    GtkAboutDialog *dlg = GTK_ABOUT_DIALOG(dialog);
    //GFile *logo_file = g_file_new_for_path ("../resources/logo/lc3-high-resolution-logo-color-on-transparent-background.png");
    char* authors[] = {"Samuele Stronati", NULL};
    gtk_about_dialog_set_authors(dlg, authors);
    gtk_about_dialog_set_program_name(dlg, "LC3 Virtual Machine");
    gtk_about_dialog_set_version(dlg, "0.1");
    gtk_about_dialog_set_logo(dlg, gdk_pixbuf_new_from_file("../resources/logo/lc3-logo.png", NULL));
    gtk_about_dialog_set_license(dlg, MIT_LICENSE);
    gtk_about_dialog_set_copyright(dlg, "\302\251 2023 Samuele Stronati");
    gtk_about_dialog_set_comments(dlg, "LC3 Virtual Machine is an LC3 emulator "
                            "program written in C and GTK.");
    gtk_about_dialog_set_website(dlg,
        "https://github.com/Samuele95/LC3VM");
    gtk_about_dialog_set_website_label(dlg,"Github Source Code Repository");
    //gtk_about_dialog_add_credit_section(dlg, "Credits", authors);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); 
}


int gtk_notepad_ask_save_cancel(void) {
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(gwindow,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO,
                "File has been modified. Do you want to save it?");
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          "Cancel", GTK_RESPONSE_CANCEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "File has been modified");
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    return response;
}


/* Callbacks */


/* Create window icon */
GdkPixbuf *create_pixbuf(const char *filename) {
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if (!pixbuf) {
        fprintf(stderr, "%s: %s\n",
                PROGNAME,
                error->message);
        g_error_free(error);
    }

    return pixbuf;
}

void gtk_notepad_text_changed(void) {
    modified = TRUE;
    gtk_statusbar_update_lncol();
}


void gtk_notepad_cut(void) {
    gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
}


void gtk_notepad_copy(void) {
    gtk_text_buffer_copy_clipboard(buffer, clipboard);
}


void gtk_notepad_paste(void) {
    gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}


void gtk_notepad_delete(void) {
    gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);
}


void gtk_notepad_select_all(void) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gtk_text_buffer_select_range(buffer, &start, &end);
}

void gtk_notepad_insert_time_date(void) {
    time_t now = time(0);
    char* datetime = asctime(localtime(&now));

    gtk_text_buffer_insert_at_cursor(buffer,
        datetime,
        strlen(datetime) - 1);
}


void gtk_text_view_toggle_wrapping(void) {
    GtkWrapMode mode;
    if (wrapping)
        mode = GTK_WRAP_NONE;
    else
        mode = GTK_WRAP_CHAR;

    wrapping = !wrapping;

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textarea),
                                mode);
}


void gtk_notepad_select_font(void) {
    GtkResponseType result;

    GtkWidget *dialog = gtk_font_selection_dialog_new("Select a font");

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY) {
        PangoFontDescription *font_desc;
        char *fontname = gtk_font_selection_dialog_get_font_name(
            GTK_FONT_SELECTION_DIALOG(dialog));

        font_desc = pango_font_description_from_string(fontname);

        gtk_widget_modify_font(textarea, font_desc);

        g_free(fontname);
    }

    gtk_widget_destroy(dialog);
}


void gtk_statusbar_update_lncol(void) {
    char *msg;
    int row, col;
    GtkTextIter iter;

    GtkStatusbar *gstatusbar = GTK_STATUSBAR(statusbar);

    gtk_statusbar_pop(gstatusbar, 0);

    gtk_text_buffer_get_iter_at_mark(buffer, &iter,
                            gtk_text_buffer_get_insert(buffer));

    row = gtk_text_iter_get_line(&iter);
    col = gtk_text_iter_get_line_offset(&iter);

    msg = g_strdup_printf("Ln: %d Col: %d", row + 1, col + 1);

    gtk_statusbar_push(gstatusbar, 0, msg);

    g_free(msg);
}


/* Create a new file */
void gtk_notepad_new(void) {
    free(loaded_fn);
    loaded_fn = malloc(1);
    loaded_fn[0] = '\0';

    gtk_text_buffer_set_text(buffer, "", -1);

    gtk_window_set_title(gwindow, "Untitled - LC3 Virtual Machine");
}


char gtk_notepad_open_file(const char* filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "%s: fopen(null): %s\n",
                PROGNAME,
                strerror(errno));
        fclose(fp);

        return 0;
    }
    else {
        char* buf;
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        buf = malloc(fsize + 1);
        fread(buf, fsize, 1, fp);
        buf[fsize] = '\0';

        gtk_text_buffer_set_text(buffer, buf, -1);

        free(buf);
        fclose(fp);

        gtk_notepad_set_title(filename);

        modified = FALSE;
    }

    return 1;
}

/* Open an existing file */
void gtk_notepad_open(void) {
    if (modified == TRUE) {
        int response = gtk_notepad_ask_save_cancel();
        switch (response) {
            case GTK_RESPONSE_YES:
                gtk_notepad_save();
                break;
            case GTK_RESPONSE_NO:
                break;
            case GTK_RESPONSE_DELETE_EVENT: case GTK_RESPONSE_CANCEL:
                return;
                break;

            default:
                return;
                break;
        }
    }

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    int res;

    dialog = gtk_file_chooser_dialog_new("Open a file",
                                         gwindow,
                                         action,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    char* filename;
    if (res == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(
                        GTK_FILE_CHOOSER(dialog));
        
        if (gtk_notepad_open_file(filename)) {
            free(loaded_fn);
            loaded_fn = malloc(strlen(filename) + 1);
            strcpy(loaded_fn, filename);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}


/* Save the file (actually) */
char gtk_notepad_save_file(const char* filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "%s: fopen(null): %s\n",
                PROGNAME,
                strerror(errno));
        fclose(fp);
        return 0;
    }
    else {
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);

        char* buf = gtk_text_buffer_get_text(buffer,
                                             &start,
                                             &end,
                                             TRUE);

        fwrite(buf, strlen(buf), 1, fp);
        free(buf);
        fclose(fp);

        gtk_notepad_set_title(filename);

        modified = FALSE;
    }

    return 1;
}


/* Save currently open file */
void gtk_notepad_save(void) {
    if (NO_FILE_LOADED)
        gtk_notepad_saveas();
    else
        gtk_notepad_save_file(loaded_fn);
}


/* Save file as */
void gtk_notepad_saveas(void) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    int res;

    dialog = gtk_file_chooser_dialog_new("Save the file",
                                         gwindow,
                                         action,
                                         "_Save",
                                         GTK_RESPONSE_ACCEPT,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);

    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_folder(chooser, g_get_current_dir());
    if (loaded_fn)
        gtk_file_chooser_set_filename(chooser, loaded_fn);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    char* filename;
    if (res == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(
                        GTK_FILE_CHOOSER(dialog));

        if (gtk_notepad_save_file(filename)) {
            free(loaded_fn);
            loaded_fn = malloc(strlen(filename) + 1);
            strcpy(loaded_fn, filename);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* Compile program */
void gtk_compile(void) {
    execute_terminal_cmd("clear", NULL);
    gtk_notepad_save();
    execute_terminal_cmd("./lc3compiler ", loaded_fn); 
} 

void gtk_run(void) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    int res;
    char* filename;

    execute_terminal_cmd("clear", NULL);
    dialog = gtk_file_chooser_dialog_new("Run a program",
                                        gwindow,
                                        action,
                                        "_Run",
                                        GTK_RESPONSE_ACCEPT,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        NULL);
                                        

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(
                        GTK_FILE_CHOOSER(dialog));
    }
    
    gtk_widget_destroy(dialog);

    execute_terminal_cmd(SH_COMMAND, filename);   
}


/* Other functions */


/* Create and setup a menubar */
void setup_menubar(void) {
    menubar = gtk_menu_bar_new();

    filemenu = gtk_menu_new();
    file = gtk_menu_item_new_with_mnemonic("_File");
    new  = gtk_menu_item_new_with_mnemonic("_New");
    openf = gtk_menu_item_new_with_mnemonic("_Open");
    save = gtk_menu_item_new_with_mnemonic("_Save");
    saveas = gtk_menu_item_new_with_mnemonic("Save _as...");
    compile = gtk_menu_item_new_with_mnemonic("_Compile");
    run = gtk_menu_item_new_with_mnemonic("_Run");
    quit = gtk_menu_item_new_with_mnemonic("_Quit");

    editmenu = gtk_menu_new();
    edit = gtk_menu_item_new_with_mnemonic("_Edit");
    cut  = gtk_menu_item_new_with_mnemonic("Cu_t");
    copy = gtk_menu_item_new_with_mnemonic("_Copy");
    paste = gtk_menu_item_new_with_mnemonic("_Paste");
    delete = gtk_menu_item_new_with_mnemonic("_Delete");
    selectall = gtk_menu_item_new_with_mnemonic("_Select All");
    time_date = gtk_menu_item_new_with_mnemonic("_Time/Date");
    wll  = gtk_check_menu_item_new_with_mnemonic("_Wrap long "
                                                 "lines");
    select_font = gtk_menu_item_new_with_mnemonic("_Font...");
    
    connectmenu = gtk_menu_new();
    connect = gtk_menu_item_new_with_mnemonic("_Connect");
    sendto = gtk_menu_item_new_with_mnemonic("_Send file to");
    receivefrm = gtk_menu_item_new_with_mnemonic("_Receive file from");

    helpmenu = gtk_menu_new();
    help = gtk_menu_item_new_with_mnemonic("_Help");
    about = gtk_menu_item_new_with_mnemonic("_About");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), openf);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), saveas);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), compile);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), run);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),
                            gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), editmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), cut);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), copy);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), paste);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), delete);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),
                            gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), selectall);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), time_date);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),
                            gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), wll);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), select_font);
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect), connectmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(connectmenu), sendto);
    gtk_menu_shell_append(GTK_MENU_SHELL(connectmenu), receivefrm);
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), connect);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);

    // Accelerators

    // File menu
    gtk_widget_add_accelerator(new,  "activate", accel, GDK_KEY_n,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(openf, "activate", accel, GDK_KEY_o,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(save, "activate", accel, GDK_KEY_s,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(saveas, "activate", accel, GDK_KEY_s,
                               GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(compile, "activate", accel, GDK_KEY_d,
                               GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(run, "activate", accel, GDK_KEY_r,
                               GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(quit, "activate", accel, GDK_KEY_q,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);

    // Edit menu
    gtk_widget_add_accelerator(cut, "activate", accel, GDK_KEY_x,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(copy, "activate", accel, GDK_KEY_c,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(paste, "activate", accel, GDK_KEY_v,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(delete, "activate", accel, GDK_KEY_Delete,
                               0,
                               GTK_ACCEL_VISIBLE);

    gtk_widget_add_accelerator(selectall, "activate", accel, GDK_KEY_a,
                               GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(time_date, "activate", accel, GDK_KEY_F5,
                               0,
                               GTK_ACCEL_VISIBLE);

    // Signals

    // File menu
    g_signal_connect(new, "activate",
                             (GCallback) gtk_notepad_new, NULL);
    g_signal_connect(openf, "activate",
                             (GCallback) gtk_notepad_open, NULL);
    g_signal_connect(save, "activate",
                             (GCallback) gtk_notepad_save, NULL);
    g_signal_connect(saveas, "activate",
                             (GCallback) gtk_notepad_saveas, NULL);
    g_signal_connect(compile,"activate",
                             (GCallback) gtk_compile, NULL);
    g_signal_connect(run,"activate",
                             (GCallback) gtk_run, NULL);
    g_signal_connect(quit, "activate",
                             (GCallback) gtk_main_quit, NULL);

    // Edit menu
    g_signal_connect(cut, "activate",
                             (GCallback)
                             gtk_notepad_cut, NULL);
    g_signal_connect(copy, "activate",
                             (GCallback)
                             gtk_notepad_copy, NULL);
    g_signal_connect(paste, "activate",
                             (GCallback)
                             gtk_notepad_paste, NULL);
    g_signal_connect(delete, "activate",
                             (GCallback)
                             gtk_notepad_delete, NULL);

    g_signal_connect(selectall, "activate",
                             (GCallback)
                             gtk_notepad_select_all, NULL);
    g_signal_connect(time_date, "activate",
                             (GCallback)
                             gtk_notepad_insert_time_date, NULL);
    g_signal_connect(wll, "activate",
                             (GCallback)
                             gtk_text_view_toggle_wrapping, NULL);
    g_signal_connect(select_font, "activate",
                             (GCallback)
                             gtk_notepad_select_font, NULL);

    // Connect menu
    g_signal_connect(sendto, "activate",
                             (GCallback)
                             open_send_dialog, NULL);
    g_signal_connect(receivefrm, "activate",
                             (GCallback)
                             open_receive_dialog, NULL);

    // Help menu
    g_signal_connect(about, "activate",
                             (GCallback)
                             gtk_notepad_show_about_box, NULL);
}


void setup_textarea(void) {
    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(
                                   scrolledwindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    textarea = gtk_text_view_new();

    gtk_container_add(GTK_CONTAINER(scrolledwindow), textarea);

    gtk_paned_pack1(GTK_PANED(paned), scrolledwindow, TRUE, FALSE);
    //gtk_box_pack_start(GTK_BOX(vbox),
    //                   scrolledwindow, TRUE, TRUE, 0);

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textarea));

    g_signal_connect(buffer, "changed",
                     (GCallback) gtk_notepad_text_changed, NULL);
    g_signal_connect(buffer, "mark_set",
                     (GCallback) gtk_statusbar_update_lncol, NULL);

    gtk_text_view_toggle_wrapping();
}

void setup_terminal(void) {
    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(
                                   scrolledwindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);


    terminal = vte_terminal_new();
    //gtk_window_set_child ((GtkWindow    *)scrollview1, terminal);

    gtk_container_add(GTK_CONTAINER(scrolledwindow), terminal);
    g_object_set_data(G_OBJECT(scrolledwindow), "terminal", terminal);
	//gtk_widget_set_visual(scrolledwindow, gdk_screen_get_rgba_visual(gtk_widget_get_screen(scrolledwindow)));
	g_object_set(gtk_settings_get_default(), "gtk-xft-rgba", "none", NULL);
	
	/* Configure terminal */
	vte_terminal_set_word_char_exceptions(VTE_TERMINAL(terminal),
	    TERM_WORD_CHARS);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal),
	    -1);
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal),
	    TRUE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal),
	    TRUE);
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal),
	    TRUE);

	vte_terminal_set_colors(VTE_TERMINAL(terminal),
	&CLR_GDK(0x000000),
	&CLR_GDKA(0xF5F5F5, TERM_OPACITY),
	NULL,
	0);

	vte_terminal_set_bold_is_bright(VTE_TERMINAL(terminal),
	    TRUE);
	vte_terminal_set_color_cursor(VTE_TERMINAL(terminal),
	    &CLR_GDK(0xF5F5F5));
	vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal),
	    VTE_CURSOR_BLINK_OFF);
	
	vte_terminal_set_audible_bell(VTE_TERMINAL(terminal),
	    FALSE);

    gchar **envp = g_get_environ();
    char shcommand[BUFSIZ];
    strcpy(shcommand, "PS1="" ; ");
    strcat(shcommand, g_environ_getenv(envp, "SHELL"));
    gchar **command = (gchar *[]){ shcommand, NULL };
    
    
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT, NULL, command, envp,
        0, NULL, NULL, NULL,
        250, NULL, NULL, NULL);
    gtk_paned_pack2(GTK_PANED(paned), scrolledwindow, TRUE, FALSE);
    g_strfreev(envp);
}


void setup_statusbar(void) {
    statusbar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    gtk_statusbar_update_lncol();
}


int main(int argc, char* argv[]) {

    PROGNAME = argv[0];

    // We don't want errors on the first `free()'
    loaded_fn = malloc(1);
    loaded_fn[0] = '\0';
 

    if (argc == 2) {
        FILE *temp = fopen(argv[1], "rb");
        if (!temp) {
            // Error
            fprintf(stderr, "%s: failed to open file: %s\n",
                    PROGNAME,
                    strerror(errno));
            return EXIT_FAILURE;
        }

        fclose(temp);

        free(loaded_fn);
        loaded_fn = malloc(strlen(argv[1]));
        strcpy(loaded_fn, argv[1]);
    }
    else if (argc > 2) {
        print_usage();
        return EXIT_FAILURE;
    }

    gtk_init(&argc, &argv);

    atom = gdk_atom_intern("CLIPBOARD", TRUE);
    clipboard = gtk_clipboard_get(atom); // get primary clipboard

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gwindow = GTK_WINDOW(window);

    gtk_window_set_title(gwindow, title);
    gtk_window_set_default_size(gwindow, 640, 480);
    gtk_window_set_position(gwindow, GTK_WIN_POS_CENTER);
    gtk_window_set_icon(gwindow, gdk_pixbuf_new_from_file("../resources/logo/lc3-high-resolution-logo-black-on-white-background.png", NULL));

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    accel = gtk_accel_group_new();
    gtk_window_add_accel_group(gwindow, accel);


    setup_menubar();
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);
    setup_terminal();
    setup_textarea();
    setup_statusbar();

    if (argc == 2)
        gtk_notepad_open_file(loaded_fn);

    g_signal_connect(window, "destroy",
                     (GCallback) gtk_main_quit, NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return EXIT_SUCCESS;
}
