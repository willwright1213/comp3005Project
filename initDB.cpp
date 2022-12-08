#include "lib\sqlite3.h"

int main() {
    sqlite3* db = nullptr;
    if(sqlite3_open("db\\bookstore.sqlite", &db) != SQLITE_OK) {
        sqlite3_errmsg(db);
    }
    return 0;
}