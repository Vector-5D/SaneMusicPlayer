#include "app.h"

int main() {
    app_t app = {0};
    app_init(&app);
    app_run(&app);
    app_free(&app);
    return 0;
}
