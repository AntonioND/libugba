// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "font_utils.h"
#include "win_utils.h"
#include "win_utils_draw.h"

//------------------------------------------------------------------------------

// Values in pixels
void GUI_ConsoleReset(gui_console *con, int screen_width, int screen_height)
{
    GUI_ConsoleClear(con);
    con->chars_w = screen_width / FONT_WIDTH;
    con->chars_h = screen_height / FONT_HEIGHT;

    if (con->chars_w > GUI_CONSOLE_MAX_WIDTH)
        con->chars_w = GUI_CONSOLE_MAX_WIDTH;
    if (con->chars_h > GUI_CONSOLE_MAX_HEIGHT)
        con->chars_h = GUI_CONSOLE_MAX_HEIGHT;
}

void GUI_ConsoleClear(gui_console *con)
{
    memset(con->buffer, 0, sizeof(con->buffer));
    memset(con->buffer_color, 0xFF,
           sizeof(con->buffer_color));
}

int GUI_ConsoleModePrintf(gui_console *con, int x, int y, const char *txt, ...)
{
    char txtbuffer[1000];

    va_list args;
    va_start(args, txt);
    vsnprintf(txtbuffer, sizeof(txtbuffer), txt, args);
    va_end(args);
    txtbuffer[sizeof(txtbuffer) - 1] = '\0';

    if (x > con->chars_w)
        return 0;
    if (y > con->chars_h)
        return 0;

    int i = 0;
    while (1)
    {
        unsigned char c = txtbuffer[i++];
        if (c == '\0')
        {
            break;
        }
        else if (c == '\n')
        {
            x = 0;
            y++;
            if (y >= con->chars_h)
                break;
        }
        else
        {
            con->buffer[y * con->chars_w + x] = c;
            x++;
        }

        if (x >= con->chars_w)
        {
            x = 0;
            y++;
            if (y >= con->chars_h)
                break;
        }
    }

    return i;
}

int GUI_ConsoleColorizeLine(gui_console *con, int y, int color)
{
    if (y > con->chars_h)
        return 0;

    for (int x = 0; x < con->chars_w; x++)
    {
        con->buffer_color[y * con->chars_w + x] = color;
        if (con->buffer[y * con->chars_w + x] == 0)
            con->buffer[y * con->chars_w + x] = ' ';
    }
    return 1;
}

// buffer is 24 bit per pixel
void GUI_ConsoleDraw(gui_console *con, unsigned char *buffer,
                     int buf_w, int buf_h)
{
    for (int y = 0; y < con->chars_h; y++)
    {
        for (int x = 0; x < con->chars_w; x++)
        {
            unsigned char c;
            int color;

            c = con->buffer[y * con->chars_w + x];
            color = con->buffer_color[y * con->chars_w + x];

            if (c)
            {
                FU_PrintChar(buffer, buf_w, buf_h,
                             x * FONT_WIDTH, y * FONT_HEIGHT, c, color);
            }
        }
    }
}

void GUI_ConsoleDrawAt(gui_console *con, unsigned char *buffer,
                       int buf_w, int buf_h,
                       int scrx, int scry, UNUSED int scrw, UNUSED int scrh)
{
    for (int y = 0; y < con->chars_h; y++)
    {
        for (int x = 0; x < con->chars_w; x++)
        {
            unsigned char c;
            int color;

            c = con->buffer[y * con->chars_w + x];
            color = con->buffer_color[y * con->chars_w + x];

            if (c)
            {
                FU_PrintChar(buffer, buf_w, buf_h, scrx + x * FONT_WIDTH,
                             scry + y * FONT_HEIGHT, c, color);
            }
        }
    }
}

//------------------------------------------------------------------------------

void GUI_InputWindowOpen(gui_inputwindow *win, char *caption,
                         gui_void_arg_pchar_int_fn callback)
{
    if (win == NULL)
        return;

    win->enabled = 1;
    win->input_text[0] = '\0';
    win->callback = callback;
    snprintf(win->window_caption, sizeof(win->window_caption), "%s", caption);
}

void GUI_InputWindowClose(gui_inputwindow *win)
{
    if (win == NULL)
        return;

    win->enabled = 0;
    if (win->callback)
        win->callback(win->input_text, 0);
}

int GUI_InputWindowIsEnabled(gui_inputwindow *win)
{
    if (win == NULL)
        return 0;

    return win->enabled;
}

//------------------------------------------------------------------------------

void GUI_SetTextBox(gui_element *e, gui_console *con, int x, int y, int w,
                    int h, gui_void_arg_int_int_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_TEXTBOX;

    e->info.textbox.con = con;
    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.textbox.mouse_press_callback = callback;

    GUI_ConsoleReset(con, w, h);
}

void GUI_SetButton(gui_element *e, int x, int y, int w, int h,
                   const char *label, gui_void_arg_void_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_BUTTON;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.button.callback = callback;
    snprintf(e->info.button.name, sizeof(e->info.button.name), "%s", label);
}

void GUI_SetRadioButton(gui_element *e, int x, int y, int w, int h,
                        const char *label, int group_id, int btn_id,
                        int start_pressed, gui_void_arg_int_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_RADIOBUTTON;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.radiobutton.callback = callback;
    e->info.radiobutton.group_id = group_id;
    e->info.radiobutton.btn_id = btn_id;
    e->info.radiobutton.is_pressed = start_pressed;
    e->info.radiobutton.is_enabled = 1;
    snprintf(e->info.radiobutton.name, sizeof(e->info.radiobutton.name),
             "%s", label);
}

void GUI_SetLabel(gui_element *e, int x, int y, int w, int h,
                  const char *label)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_LABEL;

    e->x = x;
    e->y = y;
    if (w <= 0)
        w = strlen(label) * FONT_WIDTH;
    e->w = w;
    e->h = h;
    snprintf(e->info.label.text, sizeof(e->info.label.text), "%s", label);
}

// 24-bit buffer
void GUI_SetBitmap(gui_element *e, int x, int y, int w, int h,
                   unsigned char *bitmap, gui_int_arg_int_int_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_BITMAP;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.bitmap.bitmap = bitmap;
    e->info.bitmap.callback = callback;
}

void GUI_SetWindow(gui_element *e, int x, int y, int w, int h, void *gui,
                   const char *caption)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_WINDOW;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.window.gui = gui;
    e->info.window.enabled = 0;
    snprintf(e->info.window.caption, sizeof(e->info.window.caption),
             "%s", caption);

    gui_global *relativegui = gui;

    if (relativegui == NULL)
        return;

    gui_element **gui_elements = relativegui->elements;

    if (gui_elements == NULL)
        return;

    while (*gui_elements != NULL)
    {
        (*gui_elements)->x += x;
        (*gui_elements)->y += y + FONT_HEIGHT + 2;
        gui_elements++;
    }
}

void GUI_SetMessageBox(gui_element *e, gui_console *con, int x, int y, int w,
                       int h, const char *caption)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_MESSAGEBOX;

    e->info.messagebox.enabled = 0;
    e->info.messagebox.con = con;
    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    snprintf(e->info.messagebox.caption, sizeof(e->info.messagebox.caption),
             "%s", caption);

    GUI_ConsoleReset(con, w, h - FONT_WIDTH - 1);
}

int gui_word_fits(const char *text, int x_start, int x_end)
{
    while (1)
    {
        char c = *text++;
        if (c == '\0')
            break;
        else if (c == '\n')
            break;
        else if (c == ' ')
            break;
        else if (c == '\t')
            break;
        else
            x_start++;
    }

    return (x_start <= x_end);
}

void GUI_SetScrollableTextWindow(gui_element *e, int x, int y, int w, int h,
                                 const char *text, const char *caption)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_SCROLLABLETEXTWINDOW;

    w = FONT_WIDTH * (w / FONT_WIDTH);
    h = (FONT_HEIGHT * ((h - (FONT_HEIGHT + 2)) / FONT_HEIGHT))
        + (FONT_HEIGHT + 2);

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    e->info.scrollabletextwindow.text = text;
    e->info.scrollabletextwindow.enabled = 0;
    snprintf(e->info.scrollabletextwindow.caption,
             sizeof(e->info.scrollabletextwindow.caption),
             "%s", caption);
    e->info.scrollabletextwindow.numlines = 0;
    e->info.scrollabletextwindow.currentline = 0;

    e->info.scrollabletextwindow.max_drawn_lines =
            (e->h - (FONT_HEIGHT + 1)) / FONT_HEIGHT;

    // Count number of text lines

    int textwidth = (e->w / FONT_WIDTH) - 1;

    int skipspaces = 0;

    int i = 0;
    int curx = 0;
    while (1)
    {
        char c = text[i++];
        if (skipspaces)
        {
            while (1)
            {
                if (!((c == ' ') || (c == '\0')))
                    break;
                c = text[i++];
            }
            skipspaces = 0;
        }

        if (c == '\0')
        {
            break;
        }
        else if (c == '\n')
        {
            curx = 0;
            e->info.scrollabletextwindow.numlines++;
        }
        else
        {
            if (gui_word_fits(&(text[i - 1]), curx, textwidth) == 0)
            {
                curx = 0;
                e->info.scrollabletextwindow.numlines++;
                i--;
                skipspaces = 1;
            }
            else
            {
                curx++;
                if (curx == textwidth)
                {
                    if (text[i] == '\n')
                        i++;
                    else
                        skipspaces = 1;
                    e->info.scrollabletextwindow.numlines++;
                    curx = 0;
                }
            }
        }
    }

    e->info.scrollabletextwindow.numlines++;
}

void GUI_SetScrollBar(gui_element *e, int x, int y, int w, int h,
                      int min_value, int max_value, int start_value,
                      gui_void_arg_int_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_SCROLLBAR;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;

    if (w > h)
        e->info.scrollbar.is_vertical = 0;
    else
        e->info.scrollbar.is_vertical = 1;

    e->info.scrollbar.value_min = min_value;
    e->info.scrollbar.value_max = max_value;
    e->info.scrollbar.value = start_value;
    e->info.scrollbar.callback = callback;
}

void GUI_SetGroupBox(gui_element *e, int x, int y, int w, int h,
                     const char *label)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_GROUPBOX;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    snprintf(e->info.groupbox.label, sizeof(e->info.groupbox.label),
             "%s", label);
}

void GUI_SetCheckBox(gui_element *e, int x, int y, int w, int h,
                     const char *label, int start_pressed,
                     gui_void_arg_int_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_CHECKBOX;

    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
    snprintf(e->info.checkbox.label, sizeof(e->info.checkbox.label),
             "%s", label);

    e->info.checkbox.checked = start_pressed;
    e->info.checkbox.callback = callback;
}

void GUI_SetInputGet(gui_element *e, gui_int_arg_sdl_event_fn callback)
{
    if (e == NULL)
        return;

    e->element_type = GUI_TYPE_INPUTGET;

    e->info.inputget.callback = callback;
}

//------------------------------------------------------------------------------

void GUI_SetButtonText(gui_element *e, const char *text)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_BUTTON)
        return;

    snprintf(e->info.button.name, sizeof(e->info.button.name), "%s", text);
}

// The value is clamped to configured range
void GUI_ScrollBarSetValue(gui_element *e, int value)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_SCROLLBAR)
        return;

    e->info.scrollbar.value = value;

    if (e->info.scrollbar.value < e->info.scrollbar.value_min)
        e->info.scrollbar.value = e->info.scrollbar.value_min;
    else if (e->info.scrollbar.value > e->info.scrollbar.value_max)
        e->info.scrollbar.value = e->info.scrollbar.value_max;
}

void GUI_SetLabelCaption(gui_element *e, const char *label)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_LABEL)
        return;

    snprintf(e->info.label.text, sizeof(e->info.label.text), "%s", label);
}

void gui_clear_radiobuttons(gui_element **element_list, int group_id)
{
    while (*element_list != NULL)
    {
        gui_element *e = *element_list;

        if (e->element_type == GUI_TYPE_RADIOBUTTON)
        {
            if (e->info.radiobutton.group_id == group_id)
                e->info.radiobutton.is_pressed = 0;
        }

        element_list++;
    }
}

void GUI_RadioButtonSetEnabled(gui_element *e, int enabled)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_RADIOBUTTON)
        return;

    e->info.radiobutton.is_enabled = enabled;
}

void GUI_RadioButtonSetPressed(void *completegui, gui_element *e)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_RADIOBUTTON)
        return;

    gui_global *g = completegui;

    gui_clear_radiobuttons(g->elements, e->info.radiobutton.group_id);

    e->info.radiobutton.is_pressed = 1;
}

void GUI_WindowSetEnabled(gui_element *e, int enabled)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_WINDOW)
        return;

    e->info.window.enabled = enabled;

    gui_global *wingui = e->info.window.gui;

    if (wingui == NULL)
        return;

    gui_element **element = wingui->elements;

    while (*element != NULL)
    {
        if ((*element)->element_type == GUI_TYPE_BUTTON)
        {
            (*element)->info.button.is_pressed = 0;
        }

        element++;
    }
}

int GUI_WindowGetEnabled(gui_element *e)
{
    if (e == NULL)
        return 0;

    if (e->element_type == GUI_TYPE_WINDOW)
        return e->info.window.enabled;

    return 0;
}

void GUI_MessageBoxSetEnabled(gui_element *e, int enabled)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_MESSAGEBOX)
        return;

    e->info.messagebox.enabled = enabled;
}

int GUI_MessageBoxGetEnabled(gui_element *e)
{
    if (e == NULL)
        return 0;

    if (e->element_type == GUI_TYPE_MESSAGEBOX)
        return e->info.messagebox.enabled;

    return 0;
}

void GUI_ScrollableTextWindowSetEnabled(gui_element *e, int enabled)
{
    if (e == NULL)
        return;

    if (e->element_type != GUI_TYPE_SCROLLABLETEXTWINDOW)
        return;

    e->info.scrollabletextwindow.enabled = enabled;
}

int GUI_ScrollableTextWindowGetEnabled(gui_element *e)
{
    if (e == NULL)
        return 0;

    if (e->element_type == GUI_TYPE_SCROLLABLETEXTWINDOW)
        return e->info.scrollabletextwindow.enabled;

    return 0;
}
