//
// Created by hoou on 13.11.17.
//

#ifndef GUX2_CELL_H
#define GUX2_CELL_H

#include <gtk/gtktypes.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkrgba.h>

enum CellValueEnum {
    Empty,
    X,
    O
} typedef CellValue;

struct CellStruct {
    CellValue value;
    int row;
    int col;
    GtkWidget *widget;
    GdkRGBA color;
} typedef Cell;

#endif //GUX2_CELL_H
