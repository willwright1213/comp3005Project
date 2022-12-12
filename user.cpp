#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#define chrono_minutes std::to_string(std::chrono::duration_cast<std::chrono::seconds>

typedef struct login_user {
    std::string email;
    std::string fName;
    std::string lName;
} login_user;

typedef struct address {
    std::string pCode;
    std::string city;
    std::string street_number;
    std::string street_name;
} address_t;

void choiceMenu(int&);
void findBooks(sqlite3 *, const char *, int);
int printBooks(void*, int, char **, char **);
int printBook(void*, int, char **, char **);
void selectBook(sqlite3 *, const char *, sqlite3_callback, void *);
int printAuthors(void*, int, char **, char **);
int printGenres(void*, int, char **, char **);
int printOrders(void*, int, char **, char **);
int addToCheckout(void*, int, char **, char **);
int getAddress(void*, int, char **, char **);
int login(void*, int, char **, char **);
void selectForCheckout(sqlite3*, const char *, int, std::map<std::string, int> &checkout);
void selectForOrder(sqlite3*, std::map<std::string, int> &checkout, std::string user_email);
void printErrMsg(char **);
void createAddress(address_t *);
void addAddress(sqlite3 *, address_t &, login_user &);
void updateAddressList(sqlite3 *, std::vector<std::vector<std::string>> &, login_user &); 



int main() {


    login_user user;
    std::string shipping_address;
    std::string billing_address;

    std::map<std::string, int> checkout;
    std::vector<std::vector<std::string>> address;

    sqlite3* db = nullptr;

    if(sqlite3_open("db/bookstore.sqlite", &db) != SQLITE_OK) {
        sqlite3_errmsg(db);
    }


    int choice = -1;
    int quantity;
    std::string input;
    std::string password;
    std::string query;
    char *errmsg = NULL;

    while(choice != 1) {
        std::cout << "(1) login" << '\n';
        std::cout << "(2) register" << '\n';
        std::cin >> choice;
        if(choice == 1) {
            std::cout << "email: \n";
            std::cin >> input;
            std::cout << "password: ";
            std::cin >> password;
            std::string query = "SELECT * FROM users WHERE email='" + input + "' AND password='"+password+"';";
            sqlite3_exec(db, query.c_str(), &login, &user, NULL);
        } else if(choice == 2) {
            std::string email, fName, lName, pCode, city, street_number, street_name;
            std::cout << "email:";
            std::cin >> email;
            std::cout << "first name:";
            std::cin >> fName;
            std::cout << "last name:";
            std::cin >> lName;
            std::cout << "password: ";
            std::cin >> password;
            
            query = "INSERT INTO users VALUES('" + email + "', '" + password + "', '" + fName + "', '" + lName + "');";

            sqlite3_exec(db, query.c_str(), &login, &user, &errmsg);
            
            // gather address of user
            query = "SELECT address.id, pCode, city, street_number, street_name FROM user_address JOIN address on address_id=id WHERE user_address.email='"+user.email+"';";
            sqlite3_exec(db, query.c_str(), &getAddress, &address, &errmsg);
            if(errmsg) {
                printErrMsg(&errmsg);
                return 0;
            } else {
                std::cout << "registration successful\n";
            }

        }
    }


    if(user.email == "") {
        std::cout << "this account does not exists or there was an error\n";
        return 0;
    }

    query = "SELECT address.id, pCode, city, street_number, street_name FROM user_address JOIN address on address_id=id WHERE user_address.email='"+user.email+"';";
    sqlite3_exec(db, query.c_str(), &getAddress, &address, &errmsg);
    if(errmsg) {
        printErrMsg(&errmsg);
        return 0;
    } 


    while(choice != 0) {
        printf("--------- MENU -------\n");
        std::cout << "User: " << user.fName << " " << user.lName << "!\n\n";
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
            case 6:
                printf("%-20s %-20s\n", "Book", "Quantity");
                for(auto const& item: checkout) {
                    printf("%-20s %-20s\n", item.first.c_str(), std::to_string(item.second).c_str());
                }
                break;
            case 7:
            {
                if(checkout.size() < 1) {
                    std::cout << "you have nothing in your checkout basket" << '\n';
                    break;
                }
                if(address.size() < 1) {
                    address_t tmp_address;
                    std::cout << "Please create an address first\n";
                    createAddress(&tmp_address);
                    addAddress(db, tmp_address, user);
                    updateAddressList(db, address, user);
                }
                while(billing_address == "") {
                    int prompt = 0;
                    std::cout << "please choose a billing address type number or (n) to create a new one" << '\n';
                    for(auto const &a: address) {
                        printf("hey\n");
                        printf("(%s) %-15s%-20s%-20s%-20s\n", std::to_string(prompt).c_str(), a[1].c_str(), a[2].c_str(), a[3].c_str(), a[4].c_str());
                        prompt++;
                    }
                    std::cin >> input;
                    if(input == "n") {
                        address_t tmp_address;
                        createAddress(&tmp_address);
                        addAddress(db, tmp_address, user);
                        updateAddressList(db, address, user);
                    } else {
                        int chosen = atoi(input.c_str());
                        billing_address = address[chosen][0];
                    }
                }
                while(shipping_address == "") {
                    int prompt = 0;
                    std::cout << "please choose a shipping address type number or (n) to create a new one" << '\n';
                    for(auto const &a: address) {
                        printf("(%s) %-15s%-20s%-20s%-20s\n", std::to_string(prompt).c_str(), a[1].c_str(), a[2].c_str(), a[3].c_str(), a[4].c_str());
                    }
                    std::cin >> input;
                    if(input == "n") {
                        address_t tmp_address;
                        createAddress(&tmp_address);
                        addAddress(db, tmp_address, user);
                        updateAddressList(db, address, user);
                    } else {
                        int chosen = atoi(input.c_str());
                        shipping_address = address[chosen][0];
                    }
                }
                const auto p1 = std::chrono::system_clock::now();
                std::string creation_time = chrono_minutes(p1.time_since_epoch()).count());
                std::string estimate_time = chrono_minutes(p1.time_since_epoch()).count() + 259200);
                query = "INSERT INTO ORDERS (user_email, creation_time, billing_id, shipping_id, time_estimate) " \
                    "VALUES('"+user.email+"', "+creation_time+", "+billing_address+", "+shipping_address+", "+estimate_time+");";
                sqlite3_exec(db, query.c_str(), NULL, NULL, &errmsg);
                if(errmsg) {
                    printErrMsg(&errmsg);
                    break;
                }
                std::string order_id = std::to_string(sqlite3_last_insert_rowid(db));
                for(auto const& item: checkout) {
                    std::string item_query = "INSERT INTO order_item VALUES ('"+item.first+"', "+order_id+", "+std::to_string(item.second)+");";
                    sqlite3_exec(db, item_query.c_str(), NULL, NULL, &errmsg);
                    if(errmsg) {
                        printErrMsg(&errmsg);
                        break;
                    }
                }
                checkout.clear();
            }
            break;
            case 8: // view orders status
                printf("%-20s%-20s%-20s%-20s\n", "Order Number", "Status", "current location", "estimate date");
                query = "SELECT id, oStatus, current_location, time_estimate FROM " \
                "orders;";
                sqlite3_exec(db, query.c_str(), &printOrders, NULL, &errmsg);
                if(errmsg) {
                    printErrMsg(&errmsg);
                }
            break;
        }
        std::cout << "\npress any key to continue";
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
    std::cout << "(8) view your orders" << '\n';
    std::cout << "(0) exit" << '\n';
    std::cin >> c;
}

void findBooks(sqlite3 *db, const char *value, int mode) {
    printf("--------- RESULTS -------\n");
    printf("%-15s %-20s%-20s\n", "ISBN", "Title", "price");
    char *errmsg = nullptr;
    std::string sqlquery = "select distinct ISBN, title, sale_price from books where books.title LIKE'%" + std::string(value) + "%';";
    std::string authorquery = "select distinct books.ISBN, title, sale_price from books INNER JOIN authors on books.ISBN=authors.ISBN WHERE aName LIKE '%" + std::string(value) + "%';";
    std::string genrequery = "select distinct books.ISBN, title,  sale_price from books INNER JOIN genres on books.ISBN=genres.ISBN WHERE gName LIKE '%" + std::string(value) + "%';";
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
    if(errmsg) {
        std::cout << errmsg << '\n';
    }
}

void selectBook(sqlite3 *db, const char * ISBN, sqlite3_callback callback, void *first_argument) {
    std::string sqlquery = "select ISBN, title, pages, publishers.pName, sale_price, stock from books INNER JOIN publishers on books.publisher_email = publishers.email " \
        "WHERE books.ISBN='" + std::string(ISBN) + "'";
    sqlite3_exec(db, sqlquery.c_str(), callback, first_argument, NULL);
}

int printBooks(void * connection, int size, char ** values, char ** column_name){
    printf("%-15s %-20s%-20s\n", values[0], values[1], values[2]);
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

int printOrders(void * connection, int size, char ** values, char ** column_name) {
    const auto p1 = std::chrono::system_clock::now();
    std::string chrono = chrono_minutes(p1.time_since_epoch()).count());
    printf("%-20s%-20s%-20s%-20s\n", values[0], values[1], values[2], values[3]);
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
    (*c)[std::string(values[0])] = atoi(values[4]);
    return 0;
}

int login(void * user_email, int size, char ** values, char ** column_name) {
    login_user *u = (login_user *) user_email;
    u->email = std::string(values[0]);
    u->fName = std::string(values[1]);
    u->lName = std::string(values[2]);
    return 0;
}

void printErrMsg(char ** errmsg) {
    printf("%s\n", *errmsg);
    sqlite3_free(*errmsg);
    *errmsg = NULL;
}

int getAddress(void* a, int size, char ** values, char ** column_name) {
    std::vector<std::vector<std::string>> *address = (std::vector<std::vector<std::string>> *) a;
    std::vector<std::string> tmp;
    for(int i=0; i<size; i++){
        tmp.push_back(std::string(values[i]));
    }
    address->push_back(tmp);
    return 0;
}

void createAddress(address_t * address) {
    std::string pCode, city, street_number, street_name;
    std::cout << "postal code:";
    std::cin >> pCode;
    std::cout << "city:";
    std::cin >> city;
    std::cout << "street number:";
    std::cin >> street_number;
    std::cout << "street name:";
    std::cin >> street_name;
    address->pCode = pCode;
    address->city = city;
    address->street_number = street_number;
    address->street_name = street_name;
}

void addAddress(sqlite3 *db, address_t &a, login_user &u) {
    char *errmsg;
    std::string query = "INSERT INTO ADDRESS(pCode, city, street_number, street_name) VALUES('"+a.pCode+"', '"+a.city+"', "+a.street_number+", '"+a.street_name+"')";
    sqlite3_exec(db, query.c_str(), NULL, NULL, &errmsg);
    if(errmsg) {
        printErrMsg(&errmsg);
    }
    std::string address_id = std::to_string(sqlite3_last_insert_rowid(db));
    query = "INSERT INTO user_address VALUES("+address_id+", '"+u.email+"');";
    sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
}

void updateAddressList(sqlite3 *db, std::vector<std::vector<std::string>> &a, login_user &u) {
    a.clear();
    std::string query = "SELECT id, pCode, city, street_number, street_name FROM address JOIN user_address on address_id=id WHERE user_address.email='"+u.email+"';";
    sqlite3_exec(db, query.c_str(), &getAddress, &a, NULL);
}
