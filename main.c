#include <gtk/gtk.h>
#include <stdbool.h>

static void gameStep(GtkWidget *button) {
    static bool first_player_turn = true;

    if (first_player_turn) {
        gtk_button_set_label(GTK_BUTTON(button), "O");
    } else {
        gtk_button_set_label(GTK_BUTTON(button), "X");
    }

    first_player_turn = !first_player_turn;
}

static void handleButtonClicked(GtkWidget *button, gpointer data) {
    int position = GPOINTER_TO_INT(data);
    g_print("Clicked on %d.button\n", position);

    gpointer btn_data = g_object_get_data(G_OBJECT(button), "clicked");

    // If this button was clicked first time
    if (!btn_data || GPOINTER_TO_INT(btn_data) != 1) {
        g_print("This button was clicked first time\n");

        gameStep(button);
    }

    // Set dirty flag
    g_object_set_data(G_OBJECT(button), "clicked", GINT_TO_POINTER(1));
}

static void createButtonGrid(GtkWidget *window) {
    GtkWidget *grid, *button;

    // Create grid layout
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create 9 buttons
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Create button and attach it to the grid
            button = gtk_button_new();
            gtk_widget_set_hexpand(button, TRUE);
            gtk_widget_set_vexpand(button, TRUE);
            gtk_grid_attach(GTK_GRID(grid), button, 0 + col, 0 + row, 1, 1);


            // Calculate button position
            int position = row * 3 + col;

            // Set button clicked handler
            g_signal_connect(button, "clicked", G_CALLBACK(handleButtonClicked),
                             GINT_TO_POINTER(position));
        }
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

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

    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Create grid filled with buttons
    createButtonGrid(window);

    // Show window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Create application object
    app = gtk_application_new("cz.vutbr.fit.stud.xmikit01.tic-tac-toe",
                              G_APPLICATION_FLAGS_NONE);

    // When g_application_run() called, function activate() will be called also
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Free memory
    g_object_unref(app);

    // Return status code
    return status;
}