#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#define chrono_minutes std::to_string(std::chrono::duration_cast<std::chrono::minutes>

void choiceMenu(int&);
void findBooks(sqlite3 *, const char *, int);
int printBooks(void*, int, char **, char **);
int printBook(void*, int, char **, char **);
void selectBook(sqlite3 *, const char *, sqlite3_callback, void *);
int printAuthors(void*, int, char **, char **);
int printGenres(void*, int, char **, char **);
int printOrders(void*, int, char **, char **);
int addToCheckout(void*, int, char **, char **);
int login(void*, int, char **, char **);
void selectForCheckout(sqlite3*, const char *, int, std::map<std::string, int> &checkout);
void selectForOrder(sqlite3*, std::map<std::string, int> &checkout, std::string user_email);
void printErrMsg(char **);


typedef struct login_user {
    std::string email;
    std::string fName;
    std::string lName;
    std::string pCode;
    std::string city;
    std::string street_number;
    std::string street_name;
} login_user;

int main() {
}