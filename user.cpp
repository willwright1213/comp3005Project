#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

void choiceMenu(int&);
void findBooks(sqlite3 *, const char *, int);
int printBooks(void*, int, char **, char **);
int printBook(void*, int, char **, char **);
void selectBook(sqlite3 *, const char *, sqlite3_callback, void *);
int printAuthors(void*, int, char **, char **);
int printGenres(void*, int, char **, char **);
int addToCheckout(void*, int, char **, char **);
int login(void*, int, char **, char **);
void selectForCheckout(sqlite3*, const char *, int, std::map<std::string, int> &checkout);
void selectForOrder(sqlite3*, std::map<std::string, int> &checkout, std::string user_email);
void printErrMsg(char **);

int main() {

    std::map<std::string, int> checkout;
    std::string user_email;

    sqlite3* db = nullptr;

    if(sqlite3_open("db/bookstore.sqlite", &db) != SQLITE_OK) {
        sqlite3_errmsg(db);
    }


    int choice = -1;
    int quantity;
    std::string input;
    char *errmsg = NULL;

    std::cout << "(1) login" << '\n';
    std::cout << "(2) register" << '\n';
    while(choice != 1) {
        std::cin >> choice;
        if(choice == 1) {
            std::cout << "email: \n";
            std::cin >> input;
            std::string query = "SELECT email FROM users WHERE email='" + input + "';";
            sqlite3_exec(db, query.c_str(), &login, &user_email, NULL);
        } else {
            std::string email, fName, lName, pCode, city, street_number, street_name;
            std::cout << "email:";
            std::cin >> email;
            std::cout << "first name:";
            std::cin >> fName;
            std::cout << "last name:";
            std::cin >> lName;
            std::cout << "postal code:";
            std::cin >> pCode;
            std::cout << "city:";
            std::cin >> city;
            std::cout << "street number:";
            std::cin >> street_number;
            std::cout << "street name:";
            std::cin >> street_name;
            
            std::string query = "INSERT INTO users VALUES('" + email + "', '" + fName + "', '" + lName + "', '" + pCode + "', '" + city + "', '" + street_number + "', '" \
            + street_name + "')";

            sqlite3_exec(db, query.c_str(), &login, &user_email, &errmsg);

            if(errmsg) {
                printErrMsg(&errmsg);
                return 0;
            } else {
                std::cout << "registration successful\n";
            }

        }
    }


    if(user_email == "") {
        std::cout << "this account does not exists or there was an error\n";
        return 0;
    }


    while(choice != 0) {
        printf("--------- MENU -------\n");
        choiceMenu(choice);
        switch(choice) {
            case 1:
                std::cout << "Enter title: ";
                std::cin >> input;
                findBooks(db, input.c_str(), 1);
            break;
            case 2:
                std::cout << "Enter Author's name: ";
                std::cin >> input;
                findBooks(db, input.c_str(), 2);
            break;
            case 3:
                std::cout << "Enter genre: ";
                std::cin >> input;
                findBooks(db, input.c_str(), 3);
            break;
            case 4:
            {
                std::cout << "Enter ISBN: ";
                std::cin >> input;
                printf("--------- BOOK INFO -------\n");
                selectBook(db, input.c_str(), &printBook, NULL);
                printf("--------- author(s) -------\n");
                std::string author_query = "select aName from authors where ISBN='" + input + "'";
                std::string genre_query = "select gName from genres where ISBN='" + input + "'";
                sqlite3_exec(db, author_query.c_str(), &printAuthors, NULL, NULL);
                printf("--------- genre(s) -------\n");
                sqlite3_exec(db, genre_query.c_str(), &printGenres, NULL, NULL);
            }
            break;
            case 5:
                std::cout << "Enter ISBN: ";
                std::cin >> input;
                std::cout << "Enter Quantity: ";
                std::cin >> quantity;
                selectForCheckout(db, input.c_str(), quantity, checkout);
            break;
        }
        getchar();
        getchar();
    }

    sqlite3_close(db);

    return 0;
}

void choiceMenu(int& c) {
    std::cout << "(1) search book by title" << '\n';
    std::cout << "(2) view books by author" << '\n';
    std::cout << "(3) view book by genre" << '\n';
    std::cout << "(4) view book info" << '\n';
    std::cout << "(5) Add book to checkout" << '\n';
    std::cout << "(6) view checkout" << '\n';
    std::cout << "(7) process order" << '\n';
    std::cout << "(0) exit" << '\n';
    std::cin >> c;
}

void findBooks(sqlite3 *db, const char *value, int mode) {
    printf("--------- RESULTS -------\n");
    printf("%-15s %-20s\n", "ISBN", "Title");
    char *errmsg = nullptr;
    std::string sqlquery = "select ISBN, title from books where books.title LIKE'%" + std::string(value) + "%';";
    std::string authorquery = "select distinct books.ISBN, title from books INNER JOIN authors on books.ISBN=authors.ISBN WHERE aName LIKE '%" + std::string(value) + "%';";
    std::string genrequery = "select distinct books.ISBN, title from books INNER JOIN genres on books.ISBN=genres.ISBN WHERE gName LIKE '%" + std::string(value) + "%';";
    switch(mode) {
        case 1:
            sqlite3_exec(db, sqlquery.c_str(), &printBooks, NULL, &errmsg);
            break;
        case 2:
            sqlite3_exec(db, authorquery.c_str(), &printBooks, NULL, &errmsg);
            break;
        case 3:
            sqlite3_exec(db, genrequery.c_str(), &printBooks, NULL, &errmsg);
            break;

    }
    sqlite3_exec(db, sqlquery.c_str(), &printBooks, NULL, &errmsg);
    if(errmsg) {
        std::cout << errmsg << '\n';
    }
}

void selectBook(sqlite3 *db, const char * ISBN, sqlite3_callback callback, void *first_argument) {
    char *errmsg = NULL;
    std::string sqlquery = "select ISBN, pages, publishers.pName, sale_price, stock from books INNER JOIN publishers on books.publisher_email = publishers.email " \
        "WHERE books.ISBN='" + std::string(ISBN) + "'";
    sqlite3_exec(db, sqlquery.c_str(), callback, first_argument, &errmsg);
    if(errmsg) {
        std::cout << errmsg << '\n';
    }
}

int printBooks(void * connection, int size, char ** values, char ** column_name){
    printf("%-15s %-20s\n", values[0], values[1]);
    return SQLITE_OK;
}

int printBook(void * connection, int size, char ** values, char ** column_name) {
    printf("%-20s %s\n", "ISBN:", values[0]);
    printf("%-20s %s\n", "Title:", values[1]);
    printf("%-20s %s\n", "Pages:", values[2]);
    printf("%-20s %s\n", "Publisher:", values[3]);
    printf("%-20s %s\n", "Price:", values[4]);
    return SQLITE_OK;
}

int printAuthors(void * connection, int size, char ** values, char ** column_name) {
    std::cout << values[0] << '\n';
    return SQLITE_OK;
}

int printGenres(void * connection, int size, char ** values, char ** column_name) {
    std::cout << values[0] << '\n';
    return SQLITE_OK;
}

void selectForCheckout(sqlite3 *db, const char * isbn, int quantity, std::map<std::string, int> & checkout) {
    selectBook(db, isbn, &addToCheckout, &checkout);
    if(checkout.count(std::string(isbn)) > 0) {
        if(checkout[std::string(isbn)] < quantity) {
            std::cout << "not enough copies to buy\n";
            checkout.erase(std::string(isbn));
        } else {
            checkout[std::string(isbn)] = quantity;
            std::cout << "Book added succesfully to checkout\n";
        }
    } else {
        std::cout << "This book does not exists\n";
    }
}

int addToCheckout(void * checkout, int size, char ** values, char ** column_name) {
    std::map<std::string, int> *c = (std::map<std::string, int> *) checkout;
    (*c)[std::string(values[0])] = atoi(values[5]);
    return 0;
}

int login(void * user_email, int size, char ** values, char ** column_name) {
    std::string *u = (std::string *) user_email;
    (*u) = std::string(values[0]);
    return 0;
}

void printErrMsg(char ** errmsg) {
    printf("%s\n", *errmsg);
    sqlite3_free(*errmsg);
    *errmsg = NULL;
}