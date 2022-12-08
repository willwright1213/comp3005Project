DROP TABLE checkout_item;
DROP TABLE checkout;
DROP TABLE trackers;
DROP TABLE orders;
DROP TABLE users;
DROP TABLE authors;
DROP TABLE genres;
DROP TABLE books;
DROP TABLE phone_numbers;
DROP TABLE publishers;



CREATE TABLE users (
    email TEXT PRIMARY KEY,
    fName TEXT NOT NULL,
    lName TEXT NOT NULL,
    pCode TEXT NOT NULL,
    city  TEXT NOT NULL,
    street_number INTEGER NOT NULL,
    street_name TEXT NOT NULL
);

CREATE TABLE publishers (
    email TEXT PRIMARY KEY,
    pName TEXT NOT NULL,
    account TEXT NOT NULL,
    pCode TEXT NOT NULL,
    city  TEXT NOT NULL,
    street_number INTEGER NOT NULL,
    street_name TEXT NOT NULL
);

CREATE TABLE books (
    ISBN TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    pages INTEGER NOT NULL,
    cost_price REAL NOT NULL,
    sale_price REAL NOT NULL,
    return_percentage REAL NOT NULL,
    stock INTEGER NOT NULL,
    purchased_count INTEGER NOT NULL,
    publisher_name TEXT NOT NULL
    FOREIGN KEY(publisher_name) REFERENCES publishers(pName)
);

CREATE TABLE checkout (
    id INTEGER PRIMARY KEY,
    cStatus TEXT DEFAULT "active" NOT NULL,
    user_email TEXT NOT NULL,
    FOREIGN KEY(user_email) REFERENCES users(email)
);

CREATE TABLE orders (
    id INTEGER,
    user_email TEXT,
    oStatus TEXT DEFAULT "in-process" NOT NULL,
    creation_time INTEGER NOT NULL,
    pCode TEXT NOT NULL,
    city  TEXT NOT NULL,
    street_number INTEGER NOT NULL,
    street_name TEXT NOT NULL,
    PRIMARY KEY(id),
    FOREIGN KEY(id, user_email) REFERENCES checkout(id, user_email)
) WITHOUT ROWID;

CREATE TABLE checkout_item (
    ISBN TEXT,
    checkout_id INTEGER,
    quantity INTEGER NOT NULL,
    PRIMARY KEY(ISBN, checkout_id),
    FOREIGN KEY(checkout_id) REFERENCES checkout(id)
    FOREIGN KEY(ISBN) REFERENCES books(ISBN)
) WITHOUT ROWID;

CREATE TABLE trackers (
    order_id INTEGER,
    current_location TEXT DEFAULT "Left the warehouse" NOT NULL,
    time_estimate INTEGER NOT NULL
    PRIMARY KEY(order_id),
    FOREIGN KEY(order_id) REFERENCES orders(id)
) WITHOUT ROWID;

CREATE TABLE authors (
    ISBN TEXT,
    aName TEXT,
    PRIMARY KEY(ISBN, aName),
    FOREIGN KEY(ISBN) REFERENCES books(ISBN)
) WITHOUT ROWID;

CREATE TABLE genres (
    ISBN TEXT,
    gName TEXT,
    PRIMARY KEY(ISBN, gName),
    FOREIGN KEY(ISBN) REFERENCES books(ISBN)
) WITHOUT ROWID;

CREATE TABLE phone_numbers (
    publisher_email  TEXT,
    phone_number TEXT,
    PRIMARY KEY(publisher_email, phone_number),
    FOREIGN KEY(publisher_email) REFERENCES publishers(email)
) WITHOUT ROWID;