// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <string.h>

#include "font_utils.h"
#include "win_utils.h"
#include "win_utils_draw.h"

//------------------------------------------------------------------------------

int gui_word_fits(const char *text, int x_start, int x_end); // in win_utils.c

//------------------------------------------------------------------------------

static int gui_r, gui_g, gui_b;

void GUI_Draw_SetDrawingColor(int r, int g, int b)
{
    gui_r = r;
    gui_g = g;
    gui_b = b;
}

void GUI_Draw_HorizontalLine(unsigned char *buffer, int w, int h,
                             int x1, int x2, int y)
{
    if (y > h)
        return;
    if (y < 0)
        return;

    if (x1 < 0)
        x1 = 0;
    if (x2 >= w)
        x2 = w - 1;

    unsigned char *dst = &(buffer[(y * w + x1) * 3]);

    while (x1 <= x2)
    {
        *dst++ = gui_r;
        *dst++ = gui_g;
        *dst++ = gui_b;

        x1++;
    }
}

void GUI_Draw_VerticalLine(unsigned char *buffer, int w, int h,
                           int x, int y1, int y2)
{
    if (x > w)
        return;
    if (x < 0)
        return;

    if (y1 < 0)
        y1 = 0;
    if (y2 >= h)
        y2 = h - 1;

    unsigned char *dst = &(buffer[(y1 * w + x) * 3]);

    while (y1 <= y2)
    {
        *dst++ = gui_r;
        *dst++ = gui_g;
        *dst++ = gui_b;

        dst += (w - 1) * 3;
        y1++;
    }
}

void GUI_Draw_Rect(unsigned char *buffer, int w, int h,
                   int x1, int x2, int y1, int y2)
{
    GUI_Draw_HorizontalLine(buffer, w, h, x1, x2, y1);
    GUI_Draw_VerticalLine(buffer, w, h, x1, y1, y2);
    GUI_Draw_HorizontalLine(buffer, w, h, x1, x2, y2);
    GUI_Draw_VerticalLine(buffer, w, h, x2, y1, y2);
}

void GUI_Draw_FillRect(unsigned char *buffer, int w, int h,
                       int x1, int x2, int y1, int y2)
{
    for (int y = y1; y <= y2; y++)
        GUI_Draw_HorizontalLine(buffer, w, h, x1, x2, y);
}

//------------------------------------------------------------------------------

static void gui_draw_menu(gui_menu *menu, unsigned char *buffer, int w, int h)
{
    if (menu == NULL)
        return;

    int x = 0;

    int selected_element_x = 0;

    int i = 0;
    while (1)
    {
        gui_menu_list *list = menu->list_entry[i];

        if (list == NULL)
            break;

        size_t l = strlen(list->title);

        FU_PrintColor(buffer, w, h, x, 0, 0xFFE0E0E0, " ");
        x += FONT_WIDTH;
        if (menu->element_opened == i)
        {
            selected_element_x = x;
            FU_PrintColor(buffer, w, h, x, 0, 0xFFFFFFFF, list->title);
        }
        else
        {
            FU_PrintColor(buffer, w, h, x, 0, 0xFFE0E0E0, list->title);
        }
        x += l * FONT_WIDTH;
        FU_PrintColor(buffer, w, h, x, 0, 0xFFE0E0E0, " ");
        x += FONT_WIDTH;

        i++;
    }

    GUI_Draw_SetDrawingColor(224, 224, 224);
    GUI_Draw_FillRect(buffer, w, h,
                      0, x, FONT_HEIGHT, FONT_HEIGHT + (FONT_HEIGHT / 2) - 1);

    GUI_Draw_SetDrawingColor(0, 0, 0);
    GUI_Draw_HorizontalLine(buffer, w, h, 0, x, FONT_HEIGHT + FONT_HEIGHT / 2);
    GUI_Draw_VerticalLine(buffer, w, h, x, 0, FONT_HEIGHT + FONT_HEIGHT / 2);

    // Draw the menu bar, but not any menu
    if (menu->element_opened < 0)
        return;

    if (menu->element_opened >= i)
        return;

    gui_menu_list *selected_list = menu->list_entry[menu->element_opened];

    if (selected_list == NULL)
        return;

    gui_menu_entry **entries = selected_list->entry;

    // Get longest string for this list
    i = 0;
    size_t longest_string = 0;
    while (1)
    {
        if (entries[i] == NULL)
            break;

        size_t l = strlen(entries[i]->text);
        if (longest_string < l)
            longest_string = l;

        i++;
    }

    GUI_Draw_SetDrawingColor(224, 224, 224);
    GUI_Draw_FillRect(buffer, w, h,
                      selected_element_x - FONT_WIDTH,
                      selected_element_x + FONT_WIDTH * (longest_string + 1),
                      FONT_HEIGHT + FONT_HEIGHT / 2,
                      FONT_HEIGHT + FONT_HEIGHT / 2 + (i + 1) * FONT_HEIGHT);

    GUI_Draw_SetDrawingColor(0, 0, 0);
    GUI_Draw_Rect(buffer, w, h,
                  selected_element_x - FONT_WIDTH - 1,
                  selected_element_x + FONT_WIDTH * (longest_string + 1) + 1,
                  FONT_HEIGHT + FONT_HEIGHT / 2 - 1,
                  FONT_HEIGHT + FONT_HEIGHT / 2 + (i + 1) * FONT_HEIGHT + 1);

    // Print list
    int y = 2 * FONT_HEIGHT;
    i = 0;
    while (1)
    {
        if (entries[i] == NULL)
            break;

        unsigned int color = entries[i]->enabled ? 0xFFE0E0E0 : 0xFFB0B0B0;
        FU_PrintColor(buffer, w, h, selected_element_x, y, color,
                      entries[i]->text);

        y += FONT_HEIGHT;
        i++;
    }
}

//------------------------------------------------------------------------------

static void gui_draw_inputwindow(gui_inputwindow *win, unsigned char *buffer,
                                  int w, int h)
{
    if (win == NULL)
        return;

    if (win->enabled == 0)
        return;

    int x = (w - GUI_INPUTWINDOW_WIDTH) / 2;
    int y = (h - GUI_INPUTWINDOW_HEIGHT) / 2;

    GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY, GUI_BACKGROUND_GREY,
                             GUI_BACKGROUND_GREY);
    GUI_Draw_FillRect(buffer, w, h,
                      x, x + GUI_INPUTWINDOW_WIDTH - 1,
                      y, y + FONT_HEIGHT + 1);

    GUI_Draw_SetDrawingColor(0, 0, 0);
    GUI_Draw_Rect(buffer, w, h,
                  x - 1, x + GUI_INPUTWINDOW_WIDTH - 1 + 1,
                  y - 1, y + GUI_INPUTWINDOW_HEIGHT - 1 + 1);
    GUI_Draw_HorizontalLine(buffer, w, h,
                            x - 1, x + GUI_INPUTWINDOW_WIDTH - 1 + 1,
                            y + FONT_HEIGHT + 1);

    GUI_Draw_SetDrawingColor(255, 255, 255);
    GUI_Draw_FillRect(buffer, w, h,
                      x, x + GUI_INPUTWINDOW_WIDTH - 1,
                      y + FONT_HEIGHT + 2, y + GUI_INPUTWINDOW_HEIGHT - 1);

    size_t text_width = strlen(win->window_caption) * FONT_WIDTH;
    int x_off = (GUI_INPUTWINDOW_WIDTH - text_width) / 2;

    FU_PrintColor(buffer, w, h, x + x_off, y, 0xFFE0E0E0, win->window_caption);

    FU_Print(buffer, w, h, x + FONT_WIDTH, y + FONT_HEIGHT + 4,
             win->input_text);
}

//------------------------------------------------------------------------------

static void gui_draw_init_buffer(unsigned char *buffer, int w, int h)
{
    GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY, GUI_BACKGROUND_GREY,
                             GUI_BACKGROUND_GREY);
    GUI_Draw_FillRect(buffer, w, h, 0, w - 1, 0, h - 1);
}

static void gui_draw_element(gui_element *e, unsigned char *buffer,
                               int w, int h)
{
    if (e->element_type == GUI_TYPE_TEXTBOX)
    {
        GUI_Draw_SetDrawingColor(255, 255, 255);

        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + e->h - 1);

        GUI_Draw_SetDrawingColor(256 / 4, 256 / 4, 256 / 4);
        GUI_Draw_Rect(buffer, w, h,
                      e->x - 1, e->x + e->w,
                      e->y - 1, e->y + e->h);

        GUI_ConsoleDrawAt(e->info.textbox.con,
                          buffer, w, h,
                          e->x, e->y,
                          e->w, e->h);
    }
    else if (e->element_type == GUI_TYPE_BUTTON)
    {
        if (e->info.button.is_pressed)
        {
            GUI_Draw_SetDrawingColor(255, 255, 255);
        }
        else
        {
            GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY, GUI_BACKGROUND_GREY,
                                     GUI_BACKGROUND_GREY);
        }

        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + e->h - 1);

        for (int i = 0; i < 4; i++)
        {
            GUI_Draw_SetDrawingColor(i * (GUI_BACKGROUND_GREY / 4),
                                     i * (GUI_BACKGROUND_GREY / 4),
                                     i * (GUI_BACKGROUND_GREY / 4));
            GUI_Draw_Rect(buffer, w, h,
                          e->x + i, e->x + e->w - i - 1,
                          e->y + i, e->y + e->h - i - 1);
        }

        int namewidth = FONT_WIDTH * strlen(e->info.button.name);
        int xoff = (e->w - namewidth) / 2;
        int yoff = (e->h - FONT_HEIGHT) / 2;

        if (e->info.button.is_pressed)
        {
            FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff, 0xFFFFFFFF,
                          e->info.button.name);
        }
        else
        {
            FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff,
                          GUI_BACKGROUND_GREY_RGBA, e->info.button.name);
        }

        e->info.button.is_pressed = 0;
    }
    else if (e->element_type == GUI_TYPE_RADIOBUTTON)
    {
        if (e->info.radiobutton.is_enabled)
        {
            if (e->info.radiobutton.is_pressed)
                GUI_Draw_SetDrawingColor(255, 255, 255);
            else
                GUI_Draw_SetDrawingColor(0xC0, 0xC0, 0xC0);
        }
        else
            GUI_Draw_SetDrawingColor(0x80, 0x80, 0x80);

        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + e->h - 1);

        size_t namewidth = FONT_WIDTH * strlen(e->info.radiobutton.name);
        int xoff = (e->w - namewidth) / 2;
        int yoff = (e->h - FONT_HEIGHT) / 2;

        if (e->info.radiobutton.is_enabled)
        {
            if (e->info.radiobutton.is_pressed)
            {
                FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff,
                              0xFFFFFFFF, e->info.radiobutton.name);
            }
            else
            {
                FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff,
                              0xFFC0C0C0, e->info.radiobutton.name);
            }
        }
        else
        {
            FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff, 0xFF808080,
                          e->info.radiobutton.name);
        }
    }
    else if (e->element_type == GUI_TYPE_LABEL)
    {
        int namewidth = FONT_WIDTH * strlen(e->info.label.text);
        int xoff = (e->w - namewidth) / 2;
        int yoff = (e->h - FONT_HEIGHT) / 2;

        FU_PrintColor(buffer, w, h, e->x + xoff, e->y + yoff,
                      GUI_BACKGROUND_GREY_RGBA, e->info.label.text);
    }
    else if (e->element_type == GUI_TYPE_BITMAP)
    {
        for (int y = 0; y < e->h; y++)
        {
            for (int x = 0; x < e->w; x++)
            {
                unsigned char *src = &(e->info.bitmap.bitmap[(y * e->w + x) * 3]);
                unsigned char *dst = &(buffer[((e->y + y) * w + (e->x + x)) * 3]);
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
            }
        }
    }
    else if (e->element_type == GUI_TYPE_WINDOW)
    {
        if (e->info.window.enabled == 0)
            return;

        GUI_Draw_SetDrawingColor(GUI_WINDOWBAR_GREY, GUI_WINDOWBAR_GREY,
                                 GUI_WINDOWBAR_GREY);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(0, 0, 0);
        GUI_Draw_Rect(buffer, w, h,
                      e->x - 1, e->x + e->w - 1 + 1,
                      e->y - 1, e->y + e->h - 1 + 1);
        GUI_Draw_HorizontalLine(buffer, w, h, e->x - 1, e->x + e->w - 1 + 1,
                                e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY, GUI_BACKGROUND_GREY,
                                 GUI_BACKGROUND_GREY);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y + FONT_HEIGHT + 2, e->y + e->h - 1);

        int text_width = strlen(e->info.window.caption) * FONT_WIDTH;
        int x_off = (e->w - text_width) / 2;

        FU_PrintColor(buffer, w, h, e->x + x_off, e->y, GUI_WINDOWBAR_GREY_RGBA,
                      e->info.window.caption);

        if (e->info.window.gui)
            GUI_Draw(e->info.window.gui, buffer, w, h, 0);
    }
    else if (e->element_type == GUI_TYPE_MESSAGEBOX)
    {
        if (e->info.messagebox.enabled == 0)
            return;

        GUI_Draw_SetDrawingColor(GUI_WINDOWBAR_GREY, GUI_WINDOWBAR_GREY,
                                 GUI_WINDOWBAR_GREY);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(0, 0, 0);
        GUI_Draw_Rect(buffer, w, h,
                      e->x - 1, e->x + e->w - 1 + 1,
                      e->y - 1, e->y + e->h - 1 + 1);
        GUI_Draw_HorizontalLine(buffer, w, h,
                                e->x - 1, e->x + e->w - 1 + 1,
                                e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(255, 255, 255);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y + FONT_HEIGHT + 2, e->y + e->h - 1);

        size_t text_width = strlen(e->info.messagebox.caption) * FONT_WIDTH;
        int x_off = (e->w - text_width) / 2;

        FU_PrintColor(buffer, w, h, e->x + x_off, e->y, GUI_WINDOWBAR_GREY_RGBA,
                      e->info.messagebox.caption);

        GUI_ConsoleDrawAt(e->info.messagebox.con, buffer, w, h,
                          e->x, e->y + FONT_HEIGHT + 2,
                          e->x + e->w - 1, e->y + e->h - 1);
    }
    else if (e->element_type == GUI_TYPE_SCROLLABLETEXTWINDOW)
    {
        if (e->info.scrollabletextwindow.enabled == 0)
            return;

        GUI_Draw_SetDrawingColor(GUI_WINDOWBAR_GREY, GUI_WINDOWBAR_GREY,
                                 GUI_WINDOWBAR_GREY);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(0, 0, 0);
        GUI_Draw_Rect(buffer, w, h,
                      e->x - 1, e->x + e->w - 1 + 1,
                      e->y - 1, e->y + e->h - 1 + 1);
        GUI_Draw_HorizontalLine(buffer, w, h,
                                e->x - 1, e->x + e->w - 1 + 1,
                                e->y + FONT_HEIGHT + 1);

        GUI_Draw_SetDrawingColor(255, 255, 255);
        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y + FONT_HEIGHT + 2, e->y + e->h - 1);

        uint32_t caption_width =
                strlen(e->info.scrollabletextwindow.caption) * FONT_WIDTH;

        uint32_t x_off = (e->w - caption_width) / 2;

        int skipspaces = 0;

        FU_PrintColor(buffer, w, h, e->x + x_off, e->y, GUI_WINDOWBAR_GREY_RGBA,
                      e->info.scrollabletextwindow.caption);

        if (e->info.scrollabletextwindow.text)
        {
            int countlines = 0;

            int textwidth = (e->w / FONT_WIDTH) - 1;

            int i = 0;
            int curx = 0;

            while (1) // Search start of printed text
            {
                if (countlines == e->info.scrollabletextwindow.currentline)
                    break;

                char c = e->info.scrollabletextwindow.text[i++];
                if (skipspaces)
                {
                    while (1)
                    {
                        if (!((c == ' ') || (c == '\0')))
                            break;
                        c = e->info.scrollabletextwindow.text[i++];
                    }
                    skipspaces = 0;
                }

                if (c == '\0')
                {
                    return;
                }
                else if (c == '\n')
                {
                    curx = 0;
                    countlines++;
                }
                else
                {
                    if (gui_word_fits(
                                &(e->info.scrollabletextwindow.text[i - 1]),
                                curx, textwidth) == 0)
                    {
                        curx = 0;
                        countlines++;
                        i--;
                        skipspaces = 1;
                    }
                    else
                    {
                        curx++;
                        if (curx == textwidth)
                        {
                            if (e->info.scrollabletextwindow.text[i] == '\n')
                                i++;
                            countlines++;
                            curx = 0;
                            skipspaces = 1;
                        }
                    }
                }
            }

            int cury = 0;

            int basexcoord = e->x;
            int baseycoord = e->y + FONT_HEIGHT + 2;

            countlines = 0;

            skipspaces = 0;

            while (1) // Print text
            {
                char c = e->info.scrollabletextwindow.text[i++];
                if (skipspaces)
                {
                    while (1)
                    {
                        if (!((c == ' ') || (c == '\0')))
                            break;
                        c = e->info.scrollabletextwindow.text[i++];
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
                    cury++;
                    countlines++;
                }
                else
                {
                    if (gui_word_fits(
                                &(e->info.scrollabletextwindow.text[i - 1]),
                                curx, textwidth) == 0)
                    {
                        curx = 0;
                        cury++;
                        countlines++;
                        i--;
                        skipspaces = 1;
                    }
                    else
                    {
                        FU_PrintChar(buffer, w, h,
                                     basexcoord + curx * FONT_WIDTH,
                                     baseycoord + cury * FONT_HEIGHT,
                                     c, 0xFFFFFFFF);
                        curx++;
                        if (curx == textwidth)
                        {
                            if (e->info.scrollabletextwindow.text[i] == '\n')
                                i++;
                            else
                                skipspaces = 1;
                            cury++;
                            countlines++;
                            curx = 0;
                        }
                    }
                }

                if (e->info.scrollabletextwindow.max_drawn_lines == countlines)
                    break;
            }

            for (i = 0; i < e->info.scrollabletextwindow.max_drawn_lines; i++)
            {
                FU_PrintChar(buffer, w, h,
                             basexcoord + textwidth * FONT_WIDTH,
                             baseycoord + i * FONT_HEIGHT,
                             ' ', 0xFFE0E0E0);
            }

            int percent =
                (e->info.scrollabletextwindow.currentline * 100) /
                    (e->info.scrollabletextwindow.numlines
                     - e->info.scrollabletextwindow.max_drawn_lines);

            int bar_position = 1
                + (percent * (e->info.scrollabletextwindow.max_drawn_lines - 5)
                    / 100);

            FU_PrintChar(buffer, w, h,
                         basexcoord + textwidth * FONT_WIDTH,
                         baseycoord + bar_position * FONT_HEIGHT,
                         CHR_SHADED_DARK, 0xFFE0E0E0);
            FU_PrintChar(buffer, w, h,
                         basexcoord + textwidth * FONT_WIDTH,
                         baseycoord + (bar_position + 1) * FONT_HEIGHT,
                         CHR_SHADED_DARK, 0xFFE0E0E0);
            FU_PrintChar(buffer, w, h,
                         basexcoord + textwidth * FONT_WIDTH,
                         baseycoord + (bar_position + 2) * FONT_HEIGHT,
                         CHR_SHADED_DARK, 0xFFE0E0E0);

            //if (e->info.scrollabletextwindow.currentline > 0)
                FU_PrintChar(buffer, w, h,
                             basexcoord + textwidth * FONT_WIDTH, baseycoord,
                             CHR_ARROW_UP, 0xFFE0E0E0);
            //if (e->info.scrollabletextwindow.currentline <
            //        (e->info.scrollabletextwindow.numlines
            //         - e->info.scrollabletextwindow.max_drawn_lines))
                FU_PrintChar(buffer, w, h, basexcoord + textwidth * FONT_WIDTH,
                    baseycoord
                    + (e->info.scrollabletextwindow.max_drawn_lines - 1)
                        * FONT_HEIGHT,
                    CHR_ARROW_DOWN, 0xFFE0E0E0);
        }
    }
    else if (e->element_type == GUI_TYPE_SCROLLBAR)
    {
        GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY, GUI_BACKGROUND_GREY,
                                 GUI_BACKGROUND_GREY);

        GUI_Draw_FillRect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + e->h - 1);

        GUI_Draw_SetDrawingColor(0, 0, 0);
        GUI_Draw_Rect(buffer, w, h,
                      e->x, e->x + e->w - 1,
                      e->y, e->y + e->h - 1);

        GUI_Draw_SetDrawingColor(128, 128, 128);
        GUI_Draw_Rect(buffer, w, h,
                      e->x + 1, e->x + e->w - 2,
                      e->y + 1, e->y + e->h - 2);

        int barsize = 0;
        if (e->info.scrollbar.is_vertical)
        {
            // Bar size = total length - side buttons - scrollable button
            barsize = e->h - (2 * (e->w + 1));
            barsize -= 2 * e->w;
        }
        else
        {
            // Bar size = total length - side buttons - scrollable button
            barsize = e->w - (2 * (e->h + 1));
            barsize -= 2 * e->h;
        }
        if (barsize < 0)
            return; // bad size...

        int range = e->info.scrollbar.value_max - e->info.scrollbar.value_min;
        if (range <= 0)
            range = 1;

        int position = (e->info.scrollbar.value - e->info.scrollbar.value_min)
                       * barsize / range;

        if (e->info.scrollbar.is_vertical)
        {
            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y, e->y + e->w - 1); // Up
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + 1, e->x + e->w - 2,
                          e->y + 1, e->y + e->w - 2);

            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y + e->h - e->w, e->y + e->h - 1); // Down
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + 1, e->x + e->w - 2,
                          e->y + e->h - e->w + 1, e->y + e->h - 2); // Down

            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x, e->x + e->w - 1,
                          e->y + e->w + position + 1,
                          e->y + e->w + position + 2 * e->w - 1); // Scroll
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + 1, e->x + e->w - 2,
                          e->y + e->w + position + 2,
                          e->y + e->w + position + 2 * e->w - 2); // Scroll
        }
        else
        {
            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x, e->x + e->h - 1,
                          e->y, e->y + e->h - 1); // Up
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + 1, e->x + e->h - 2,
                          e->y + 1, e->y + e->h - 2);

            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + e->w - e->h, e->x + e->h - 1,
                          e->y, e->y + e->h - 1); // Down
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + e->w - e->h + 1, e->x + e->h - 2,
                          e->y + 1, e->y + e->h - 2); // Down

            GUI_Draw_SetDrawingColor(0, 0, 0);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + e->h + position + 1,
                          e->x + e->h + position + 2 * e->h - 1,
                          e->y, e->y + e->h - 1); // Scroll
            GUI_Draw_SetDrawingColor(128, 128, 128);
            GUI_Draw_Rect(buffer, w, h,
                          e->x + e->h + position + 2,
                          e->x + e->h + position + 2 * e->h - 2,
                          e->y + 1, e->y + e->h - 2); // Scroll
        }
    }
    else if (e->element_type == GUI_TYPE_GROUPBOX)
    {
        GUI_Draw_SetDrawingColor(GUI_BACKGROUND_GREY / 2,
                                 GUI_BACKGROUND_GREY / 2,
                                 GUI_BACKGROUND_GREY / 2);

        GUI_Draw_Rect(buffer, w, h,
                      e->x, e->x + e->w - 1,
                      e->y + FONT_HEIGHT / 2 - 1, e->y + e->h - 1);

        FU_PrintColor(buffer, w, h, e->x + 6, e->y, GUI_BACKGROUND_GREY_RGBA,
                      e->info.groupbox.label);
    }
    else if (e->element_type == GUI_TYPE_CHECKBOX)
    {
        GUI_Draw_SetDrawingColor(255, 255, 255);

        GUI_Draw_FillRect(buffer, w, h,
                          e->x + 1, e->x + e->h - 1 - 1,
                          e->y + 1, e->y + e->h - 1 - 1);

        GUI_Draw_SetDrawingColor(0, 0, 0);

        GUI_Draw_Rect(buffer, w, h,
                      e->x, e->x + e->h - 1,
                      e->y, e->y + e->h - 1);

        if (e->info.checkbox.checked)
        {
            GUI_Draw_SetDrawingColor(0, 0, 0);

            GUI_Draw_FillRect(buffer, w, h,
                              e->x + 3, e->x + e->h - 1 - 3,
                              e->y + 3, e->y + e->h - 1 - 3);
        }

        FU_PrintColor(buffer, w, h, e->x + e->h + FONT_WIDTH / 2, e->y,
                      GUI_BACKGROUND_GREY_RGBA, e->info.checkbox.label);
    }
    else if (e->element_type == GUI_TYPE_INPUTGET)
    {
        // Don't draw
    }
}

void GUI_Draw(gui_global *gui, unsigned char *buffer, int w, int h, int clean)
{
    if (gui == NULL)
        return;

    if (clean)
        gui_draw_init_buffer(buffer, w, h);

    gui_element **e = gui->elements;

    if (e == NULL)
        return;

    while (*e != NULL)
        gui_draw_element(*e++, buffer, w, h);

    gui_draw_inputwindow(gui->inputwindow, buffer, w, h);
    gui_draw_menu(gui->menu, buffer, w, h);
}
