#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <string>

int execSQL(const char *, sqlite3 *);

int main() {
    sqlite3* db = nullptr;
    if(sqlite3_open("db/bookstore.sqlite", &db) != SQLITE_OK) {
        sqlite3_errmsg(db);
    }
    execSQL("sql/init_db.sql",db);

    return 0;
}

int execSQL(const char *path, sqlite3 *db) {
    std::fstream file;
    std::string sqlstr;
    char ch;
    char *emsg = nullptr;
    file.open(path, std::ios::in);
    if(!file) {
        return -1;
    }
    while(file.get(ch)) {
        sqlstr.push_back(ch);
    }
    int code = sqlite3_exec(db,sqlstr.c_str(),NULL,NULL,&emsg);
    if(emsg) {
        std::cout << emsg << '\n';
    }
    return 0;
}