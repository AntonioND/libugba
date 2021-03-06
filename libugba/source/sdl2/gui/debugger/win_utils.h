// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_WIN_UTILS_H__
#define SDL2_GUI_DEBUGGER_WIN_UTILS_H__

#include <SDL2/SDL.h>

#define GUI_CONSOLE_MAX_WIDTH 150
#define GUI_CONSOLE_MAX_HEIGHT 80

typedef struct
{
    int     chars_w;
    int     chars_h;
    char    buffer[GUI_CONSOLE_MAX_WIDTH * GUI_CONSOLE_MAX_HEIGHT];
    int     buffer_color[GUI_CONSOLE_MAX_WIDTH * GUI_CONSOLE_MAX_HEIGHT];
} gui_console;

// Sizes in pixels
void GUI_ConsoleReset(gui_console *con, int screen_width, int screen_height);
void GUI_ConsoleClear(gui_console *con);
int GUI_ConsoleModePrintf(gui_console *con, int x, int y, const char *txt, ...);
int GUI_ConsoleColorizeLine(gui_console *con, int y, int color);

// The buffer is 24 bit per pixel
void GUI_ConsoleDraw(gui_console *con, unsigned char *buffer,
                     int buf_w, int buf_h);
void GUI_ConsoleDrawAt(gui_console *con, unsigned char *buffer,
                       int buf_w, int buf_h,
                       int scrx, int scry, int scrw, int scrh);

//------------------------------------------------------------------------------

#define GUI_TYPE_NONE                  0
#define GUI_TYPE_TEXTBOX               1
#define GUI_TYPE_BUTTON                2
#define GUI_TYPE_RADIOBUTTON           3
#define GUI_TYPE_LABEL                 4
#define GUI_TYPE_BITMAP                5
#define GUI_TYPE_WINDOW                6
#define GUI_TYPE_MESSAGEBOX            7
#define GUI_TYPE_SCROLLABLETEXTWINDOW  8
#define GUI_TYPE_SCROLLBAR             9
#define GUI_TYPE_GROUPBOX             10
#define GUI_TYPE_CHECKBOX             11
// Helper to get callbacks when pressing keyboard or joystick buttons
#define GUI_TYPE_INPUTGET             12

typedef void (*gui_void_arg_void_fn)(void);
typedef void (*gui_void_arg_int_fn)(int);
typedef void (*gui_void_arg_int_int_fn)(int, int);
typedef int (*gui_int_arg_int_int_fn)(int, int);
typedef void (*gui_void_arg_pchar_int_fn)(char *, int);
typedef int (*gui_int_arg_sdl_event_fn)(SDL_Event *);

typedef struct
{
    int element_type;
    int x, y, w, h;
    union {
        struct
        {
            gui_console *con;
            gui_void_arg_int_int_fn mouse_press_callback; // Args -> x, y
        } textbox;
        struct
        {
            char name[60];
            gui_void_arg_void_fn callback;
            int is_pressed;
        } button;
        struct
        {
            char name[60];
            gui_void_arg_int_fn callback; // Arg -> btn_id of clicked button
            int is_pressed;
            int group_id;
            int btn_id; // Button ID inside group
            int is_enabled;
        } radiobutton;
        struct
        {
            char text[100];
        } label;
        struct
        {
            unsigned char *bitmap;
            gui_int_arg_int_int_fn callback;
        } bitmap;
        struct
        {
            int enabled;
            void *gui; // Pointer to gui
            char caption[100];
        } window;
        struct
        {
            int enabled;
            gui_console *con;
            char caption[100];
        } messagebox;
        struct
        {
            int enabled;
            const char *text;
            char caption[100];
            int currentline; // Top line of the window
            int numlines;    // Counted number of lines
            int max_drawn_lines;
        } scrollabletextwindow;
        struct
        {
            int is_vertical;
            int value;
            int value_min;
            int value_max;
            gui_void_arg_int_fn callback; // Arg -> newvalue
        } scrollbar;
        struct
        {
            char label[100];
        } groupbox;
        struct
        {
            int checked;
            char label[100];
            gui_void_arg_int_fn callback; // Arg -> 1 = is checked
        } checkbox;
        struct
        {
            // Return 1 if you want to redraw the GUI
            gui_int_arg_sdl_event_fn callback;
        } inputget;
    } info;
} gui_element;

//------------------------------------------------------------------------------

void GUI_SetTextBox(gui_element *e, gui_console *con, int x, int y, int w,
                    int h, gui_void_arg_int_int_fn callback);

void GUI_SetButton(gui_element *e, int x, int y, int w, int h,
                   const char *label, gui_void_arg_void_fn callback);

void GUI_SetRadioButton(gui_element *e, int x, int y, int w, int h,
                        const char *label, int group_id, int btn_id,
                        int start_pressed, gui_void_arg_int_fn callback);

// if (w <= 0) w = strlen(label) * FONT_WIDTH;
void GUI_SetLabel(gui_element *e, int x, int y, int w, int h,
                  const char *label);

// Bitmap is 24 bit. return 1 from callback to redraw GUI
void GUI_SetBitmap(gui_element *e, int x, int y, int w, int h,
                   unsigned char *bitmap, gui_int_arg_int_int_fn callback);

// gui is a gui pointer
void GUI_SetWindow(gui_element *e, int x, int y, int w, int h, void *gui,
                   const char *caption);

void GUI_SetMessageBox(gui_element *e, gui_console *con, int x, int y, int w,
                       int h, const char *caption);

void GUI_SetScrollableTextWindow(gui_element *e, int x, int y, int w, int h,
                                 const char *text, const char *caption);

void GUI_SetScrollBar(gui_element *e, int x, int y, int w, int h,
                      int min_value, int max_value, int start_value,
                      gui_void_arg_int_fn callback);

void GUI_SetGroupBox(gui_element *e, int x, int y, int w, int h,
                     const char *label);

// w is ignored, left for consistency
void GUI_SetCheckBox(gui_element *e, int x, int y, int w, int h,
                     const char *label, int start_pressed,
                     gui_void_arg_int_fn callback);

// Return 1 from callback if you want to redraw the GUI
void GUI_SetInputGet(gui_element *e, gui_int_arg_sdl_event_fn callback);

//------------------------------------------------------------------------------

void GUI_SetButtonText(gui_element *e, const char *text);

void GUI_SetLabelCaption(gui_element *e, const char *label);

// The value is clamped to the configured range
void GUI_ScrollBarSetValue(gui_element *e, int value);

void GUI_RadioButtonSetEnabled(gui_element *e, int enabled);
// completegui is a gui pointer
void GUI_RadioButtonSetPressed(void *completegui, gui_element *e);

void GUI_WindowSetEnabled(gui_element *e, int enabled);
int GUI_WindowGetEnabled(gui_element *e);

void GUI_MessageBoxSetEnabled(gui_element *e, int enabled);
int GUI_MessageBoxGetEnabled(gui_element *e);

void GUI_ScrollableTextWindowSetEnabled(gui_element *e, int enabled);
int GUI_ScrollableTextWindowGetEnabled(gui_element *e);

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

#define GUI_INPUTWINDOW_MAX_LEN 30
#define GUI_INPUTWINDOW_WIDTH   ((GUI_INPUTWINDOW_MAX_LEN + 2) * FONT_WIDTH)
#define GUI_INPUTWINDOW_HEIGHT  50

typedef struct
{
    int enabled;
    char window_caption[GUI_INPUTWINDOW_MAX_LEN + 1];
    char input_text[GUI_INPUTWINDOW_MAX_LEN + 1];
    gui_void_arg_pchar_int_fn callback;
    //int outputtype
} gui_inputwindow;

//------------------------------------------------------------------------------

// callback returns (char * text, int is_valid_text)
void GUI_InputWindowOpen(gui_inputwindow *win, char *caption,
                         gui_void_arg_pchar_int_fn callback);
void GUI_InputWindowClose(gui_inputwindow *win);
int GUI_InputWindowIsEnabled(gui_inputwindow *win);
int GUI_InputWindowSendEvent(gui_inputwindow *win, SDL_Event *e);

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

typedef struct
{
    char *text;
    gui_void_arg_void_fn callback;
    int enabled; // 1 -> enabled
} gui_menu_entry;

typedef struct
{
    char *title;
    gui_menu_entry **entry;
} gui_menu_list;

typedef struct
{
    int element_opened;
    gui_menu_list **list_entry;
} gui_menu;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

typedef struct
{
    gui_element **elements;
    gui_inputwindow *inputwindow;
    gui_menu *menu;
} gui_global;

//------------------------------------------------------------------------------

#include "win_utils_draw.h"

#include "win_utils_events.h"

#endif // SDL2_GUI_DEBUGGER_WIN_UTILS_H__
