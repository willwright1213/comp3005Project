DROP TABLE IF EXISTS address;
DROP TABLE IF EXISTS order_item;
DROP TABLE IF EXISTS orders;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS user_address;
DROP TABLE IF EXISTS authors;
DROP TABLE IF EXISTS genres;
DROP TABLE IF EXISTS books;
DROP TABLE IF EXISTS phone_numbers;
DROP TABLE IF EXISTS publishers;


CREATE TABLE address (
    id INTEGER PRIMARY KEY,
    pCode TEXT NOT NULL,
    city TEXT NOT NULL,
    street_number TEXT NOT NULL,
    street_name TEXT NOT NULL
);

CREATE TABLE users (
    email TEXT PRIMARY KEY,
    password TEXT NOT NULL,
    fName TEXT NOT NULL,
    lName TEXT NOT NULL
);

CREATE TABLE user_address (
    address_id INTEGER,
    email TEXT,
    PRIMARY KEY(address_id, email),
    FOREIGN KEY(address_id) REFERENCES address(id),
    FOREIGN KEY(email) REFERENCES users(email)
);

CREATE TABLE publishers (
    email TEXT PRIMARY KEY,
    pName TEXT NOT NULL,
    account INTEGER NOT NULL,
    address_id INTEGER,
    FOREIGN KEY(address_id) REFERENCES address(id)
);

CREATE TABLE books (
    ISBN TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    pages INTEGER NOT NULL,
    cost_price REAL NOT NULL,
    sale_price REAL NOT NULL,
    return_percentage REAL NOT NULL,
    stock INTEGER DEFAULT 0 NOT NULL,
    purchased_count INTEGER DEFAULT 0 NOT NULL,
    publisher_email TEXT NOT NULL,
    FOREIGN KEY(publisher_email) REFERENCES publishers(pName)
);

CREATE TABLE orders (
    id INTEGER,
    user_email TEXT,
    oStatus TEXT DEFAULT "in-process" NOT NULL,
    creation_time INTEGER NOT NULL,
    billing_id INTEGER NOT NULL,
    shipping_id INTEGER NOT NULL,
    current_location TEXT DEFAULT "warehouse" NOT NULL,
    time_estimate INTEGER NOT NULL,
    PRIMARY KEY(id),
    FOREIGN KEY (billing_id) REFERENCES address(id),
    FOREIGN KEY (shipping_id) REFERENCES address(id),
    FOREIGN KEY(user_email) REFERENCES users(email)
);

CREATE TABLE order_item (
    ISBN TEXT,
    order_id INTEGER,
    quantity INTEGER NOT NULL,
    PRIMARY KEY(ISBN, order_id),
    FOREIGN KEY(order_id) REFERENCES checkout(id)
    FOREIGN KEY(ISBN) REFERENCES books(ISBN)
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


CREATE VIEW IF NOT EXISTS expenditure 
AS
SELECT
    checkout.id,
    checkout.cStatus AS checkout_status,
    books.ISBN,
    books.cost_price,
    books.sale_price,
    books.return_percentage,
    books.purchased_count,
    checkout_item.quantity AS quantity_sold
FROM
    checkout
INNER JOIN checkout_item on checkout_item.checkout_id=checkout.id
INNER JOIN checkout_item on books.ISBN=checkout_item.ISBN
WHERE checkout_status="ordered";

INSERT INTO address VALUES(0,"J95G6G", "Ottawa", 514, "Rideau St.");
INSERT INTO address VALUES(1,"H6H2DR", "Toronto", 438, "Somewhere St.");
INSERT INTO address VALUES(2,"X5X7F7", "Montreal", 20124, "St-Catherine");
    
--Creating a list of publishers
INSERT INTO publishers VALUES("folio@giveusmoney.com", "Folio", 1231234567, 0);
INSERT INTO publishers VALUES("publisher@pearson.com", "Pearson", 76543210, 1);
INSERT INTO publishers VALUES("mckey@gmail.com", "McKey", 01010013235, 2);

INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("0441013597", "Dune", 455, 12.50, 15.75, 0.10, "folio@giveusmoney.com", 10, 10);
INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count)VALUES("0544003411", "The Lord Of The Rings", 350, 11.75, 14.00, 0.05, "folio@giveusmoney.com", 15, 15);
INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("054792822X", "The Hobbit", 255, 11.75, 14.00, 0.05, "folio@giveusmoney.com", 25, 25);
INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("0345339738", "Return of the King", 514, 12.55, 15.35, 0.15, "folio@giveusmoney.com", 21, 21);
INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("1665929804", "The Magic of a Small Town Christmas", 130, 8.00, 10.00, 0.02, "folio@giveusmoney.com", 19, 19);

INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("9780137869763", "Organizational Behaviour", 800, 79.99, 90.00, 0.05, "publisher@pearson.com", 10, 10);
INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("9780135163825", "Stats: Data and Models, 5th edition", 1014, 200.00, 225.00, 0.02, "publisher@pearson.com", 60, 60);

INSERT INTO books(ISBN, title, pages, cost_price, sale_price, return_percentage, publisher_email, stock, purchased_count) VALUES("2894509383", "Caillou Puts Away His Toys", 15, 3.99, 7.99, 0.10, "mckey@gmail.com", 12, 12);


INSERT INTO authors VALUES("0441013597", "Frank Herbert");
INSERT INTO authors VALUES("0544003411", "J.R.R Tolkien");
INSERT INTO authors VALUES("054792822X", "J.R.R Tolkien");
INSERT INTO authors VALUES("0345339738", "J.R.R Tolkien");
INSERT INTO authors VALUES("1665929804", "Alexander, Megan");

INSERT INTO authors VALUES("9780137869763", "Nancy Langton");
INSERT INTO authors VALUES("9780137869763", "Timothy A Judge");
INSERT INTO authors VALUES("9780137869763", "Edward Perry");
INSERT INTO authors VALUES("9780135163825", "David E. Bock");
INSERT INTO authors VALUES("9780135163825", "Richard D. De Veaux");

INSERT INTO authors VALUES("2894509383", "Joceline Sanschagrin");



INSERT INTO genres VALUES("0441013597", "science-fiction");
INSERT INTO genres VALUES("0441013597", "political");
INSERT INTO genres VALUES("0544003411", "adventure");
INSERT INTO genres VALUES("0544003411", "fantasy");
INSERT INTO genres VALUES("054792822X", "fantasy");
INSERT INTO genres VALUES("054792822X", "adventure");
INSERT INTO genres VALUES("054792822X", "children");
INSERT INTO genres VALUES("0345339738", "fantasy");
INSERT INTO genres VALUES("0345339738", "adventure");
INSERT INTO genres VALUES("1665929804", "holiday");
INSERT INTO genres VALUES("1665929804", "comedy");
INSERT INTO genres VALUES("9780137869763", "education");
INSERT INTO genres VALUES("9780135163825", "education");
INSERT INTO genres VALUES("2894509383", "children");
INSERT INTO genres VALUES("2894509383", "education");

