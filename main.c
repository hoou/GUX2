#include <gtk/gtk.h>
#include "Game.h"

// Global variables
Game *game;
GtkApplication *app;

void markWinningCells(struct CellStruct **cells) {
    for (int i = 0; i < 3; i++) {
        cells[i]->color.red = 0;
        cells[i]->color.green = 0;
        cells[i]->color.blue = 1;
        gtk_widget_queue_draw(cells[i]->widget);
    }
}

void drawX(GtkWidget *widget, cairo_t *cr, GdkRGBA *color) {
    GdkWindow *window;
    GtkStyleContext *context;
    gint size;
    int window_width, window_height;

    window = gtk_widget_get_window(widget);
    window_width = gdk_window_get_width(window);
    window_height = gdk_window_get_height(window);

    size = (gint) (window_width / 5);

    context = gtk_widget_get_style_context(widget);

    gtk_render_background(context, cr, 0, 0, window_width, window_height);

    cairo_save(cr);

    cairo_set_source_rgb(cr, 0.25, 0.4, 1.0);
    cairo_set_line_width(cr, 2.5);
    cairo_translate(cr, window_width / 2, window_height / 2);
    cairo_move_to(cr, -size, -size);
    cairo_line_to(cr, +size, +size);
    cairo_move_to(cr, +size, -size);
    cairo_line_to(cr, -size, +size);

    gdk_cairo_set_source_rgba(cr, color);

    cairo_stroke(cr);
    cairo_restore(cr);
}

void drawO(GtkWidget *widget, cairo_t *cr, GdkRGBA *color) {
    GdkWindow *window;
    GtkStyleContext *context;
    int window_width, window_height;

    window = gtk_widget_get_window(widget);
    window_width = gdk_window_get_width(window);
    window_height = gdk_window_get_height(window);

    context = gtk_widget_get_style_context(widget);

    gtk_render_background(context, cr, 0, 0, window_width, window_height);

    cairo_arc(cr,
              window_width / 2.0, window_height / 2.0,
              MIN (window_width, window_height) / 3.0,
              0, 2 * G_PI);


    gdk_cairo_set_source_rgba(cr, color);

    cairo_stroke(cr);
}

gboolean onDraw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    Cell *cell;

    cell = data;

    if (cell->value == O) {
        drawO(widget, cr, &cell->color);
    } else if (cell->value == X) {
        drawX(widget, cr, &cell->color);
    }

    return FALSE;
}

gboolean onClick(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    Cell *cell;
    Cell *winning_cells[3];

    // Don't do anything if game is finished
    if (game->state == Finished) {
        return FALSE;
    }

    cell = data;

    // If cell is not empty, it is not valid move
    if (!cell->value == Empty) {
        return FALSE;
    }

    makeGameMove(game, cell);

    // Redraw cell
    gtk_widget_queue_draw(widget);

    if (doWeHaveWinner(game, cell, winning_cells)) {
        g_print("Winner winner chicken dinner!\n");
        if (game->currentPlayer == SecondPlayer) {
            g_print("Congratulations Player 1!\n");
        } else {
            g_print("Congratulations Player 2!\n");
        }
        markWinningCells(winning_cells);
        endGame(game);
    } else if (game->moveCount == 9) {
        g_print("Tie!\n");
        endGame(game);
    }

    return FALSE;
}

void newGameDialogRepsonseCallback(GtkDialog *dialog, gint response_id,
                                   gpointer user_data) {
    if (response_id == GTK_RESPONSE_YES) {
        restartGame(game);
        g_print("Game restarted\n");
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void exitDialogRepsonseCallback(GtkDialog *dialog, gint response_id,
                                gpointer user_data) {
    if (response_id == GTK_RESPONSE_YES) {
        g_application_quit(G_APPLICATION(app));
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void newGameCallback(GtkWidget *widget) {
    GtkWidget *dialog = NULL;
    GtkWidget *window = NULL;

    window = gtk_widget_get_toplevel(widget);

    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "Do you really want to start the new game?");

    g_signal_connect(dialog, "response",
                     G_CALLBACK(newGameDialogRepsonseCallback),
                     NULL);
    gtk_widget_show_all(dialog);
}

static void exitCallback(GtkWidget *widget) {
    GtkWidget *dialog = NULL;
    GtkWidget *window = NULL;

    window = gtk_widget_get_toplevel(widget);

    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "Do you really want to exit the game?");

    g_signal_connect(dialog, "response",
                     G_CALLBACK(exitDialogRepsonseCallback),
                     NULL);
    gtk_widget_show_all(dialog);
}

GtkWidget *createPlayGrid() {
    GtkWidget *grid, *cell;

    // Create grid layout
    grid = gtk_grid_new();

    // Create 9 cells
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Create cell and attach it to the grid
            cell = gtk_drawing_area_new();
            gtk_widget_set_hexpand(cell, TRUE);
            gtk_widget_set_vexpand(cell, TRUE);
            gtk_grid_attach(GTK_GRID(grid), cell, 0 + col, 0 + row, 1, 1);

            game->grid[row][col]->widget = cell;

            // Setup handlers
            g_signal_connect(G_OBJECT(cell), "draw", G_CALLBACK(onDraw),
                             game->grid[row][col]);

            g_signal_connect(G_OBJECT(cell), "button-release-event",
                             G_CALLBACK(onClick), game->grid[row][col]);

            gtk_widget_add_events(cell, GDK_BUTTON_RELEASE_MASK |
                                        GDK_BUTTON_PRESS_MASK);
        }
    }

    return grid;
}

GtkWidget *createMenuBar() {
    GtkWidget *box, *menu_bar, *file_menu, *file_menu_item;
    GtkWidget *new_game_item_menu, *exit_item_menu;

    menu_bar = gtk_menu_bar_new();

    file_menu = gtk_menu_new();
    file_menu_item = gtk_menu_item_new_with_label("File");
    new_game_item_menu = gtk_menu_item_new_with_label("New game");
    exit_item_menu = gtk_menu_item_new_with_label("Exit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_game_item_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);

    // Add handlers
    g_signal_connect(G_OBJECT(new_game_item_menu), "activate",
                     G_CALLBACK(newGameCallback), NULL);
    g_signal_connect(G_OBJECT(exit_item_menu), "activate",
                     G_CALLBACK(exitCallback), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), menu_bar, FALSE, FALSE, 0);

    return box;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *layout_grid, *menu_bar, *play_grid;

    // Create window
    window = gtk_application_window_new(app);

    // Set window title and default size
    gtk_window_set_title(GTK_WINDOW(window), "Tic-tac-toe");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    GdkGeometry hints;
    hints.min_width = 200;
    hints.min_height = 200;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &hints,
                                  GDK_HINT_MIN_SIZE);

    // Create layout grid
    layout_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), layout_grid);

    // Create menu bar
    menu_bar = createMenuBar();

    // Create grid filled with buttons
    play_grid = createPlayGrid();

    gtk_grid_attach(GTK_GRID(layout_grid), menu_bar, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout_grid), play_grid, 0, 1, 1, 1);

    // Show window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    int status;

    // Create game
    game = createGame();

    // Create application object
    app = gtk_application_new("cz.vutbr.fit.stud.xmikit01.tic-tac-toe",
                              G_APPLICATION_FLAGS_NONE);

    // When g_application_run() called, function activate() will be called also
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Free memory
    g_object_unref(app);

    destroyGame(game);

    // Return status code
    return status;
}