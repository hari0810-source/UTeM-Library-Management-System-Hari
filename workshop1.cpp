#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

// MySQL Connector 
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;

// --- DATABASE  ---
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";
const string schema = "Utem_Library_Management_System";

// --- FUNCTION PROTOTYPES ---
void mainMenu();
void adminMenu();
void studentMenu(string username);
void registerStudent();

// Core 
void addBook();
void viewBooks();
void searchBook();
void updateBook();
void deleteBook();
void borrowBook(string username);
void returnBook(string username);
void viewBorrowedReport();

// COMPLEX CALCULATION & REPORT FUNCTIONS
void viewOverdueReport();
void viewMonthlyFineReport();
void viewPopularBooks();
void viewBooksYearStats();

// --- DATABASE CONNECTION ---
sql::Connection* getDBConnection() {
    try {
        sql::Driver* driver = get_driver_instance();
        sql::Connection* con = driver->connect(server, username, password);
        con->setSchema(schema);
        return con;
    }
    catch (sql::SQLException& e) {
        cout << "\nDatabase Connection Failed!" << endl;
        cout << "Error Message: " << e.what() << endl;
        system("pause");
        exit(1);
    }
}

// --- MAIN FUNCTION ---
int main() {
    cout << "Connecting to database..." << endl;
    sql::Connection* con = getDBConnection();
    delete con;
    cout << "Connection successful!\n" << endl;

    mainMenu();

    return 0;
}

// =============================================================
//  MAIN MENUS 
// =============================================================

void mainMenu() {
    while (true) {
        system("cls");
        int option;
        cout << "\n===== UTeM Library Management System =====" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register (Student)" << endl;
        cout << "3. Exit" << endl;
        cout << "Choose option: ";

        if (!(cin >> option)) {
            cout << "Invalid input.\n";
            cin.clear(); cin.ignore(1000, '\n');
            system("pause");
            continue;
        }

        if (option == 2) {
            registerStudent();
            system("pause");
            continue;
        }
        else if (option == 3) {
            cout << "Exiting...\n";
            break;
        }

        string userIn, passIn;
        cout << "\nUsername: "; cin >> userIn;
        cout << "Password: "; cin >> passIn;

        if (userIn == "admin" && passIn == "admin123") {
            cout << "\nLogin successful as Admin!" << endl;
            system("pause");
            adminMenu();
            continue;
        }

        try {
            sql::Connection* con = getDBConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement("SELECT role FROM users WHERE username = ? AND password = ?");
            pstmt->setString(1, userIn); pstmt->setString(2, passIn);
            sql::ResultSet* res = pstmt->executeQuery();

            if (res->next()) {
                if (res->getString("role") == "student") {
                    cout << "\nLogin successful as Student!" << endl;
                    system("pause");
                    studentMenu(userIn);
                }
            }
            else {
                cout << "\nInvalid username or password!" << endl;
                system("pause");
            }
            delete res; delete pstmt; delete con;
        }
        catch (sql::SQLException& e) {
            cout << "Error: " << e.what() << endl;
            system("pause");
        }
    }
}

void adminMenu() {
    int choice;
    do {
        system("cls");
        cout << "\n========================================" << endl;
        cout << "            ADMIN DASHBOARD             " << endl;
        cout << "========================================" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. View Books" << endl;
        cout << "3. Update Book" << endl;
        cout << "4. Delete Book" << endl;
        cout << "5. Search Book" << endl;                
        cout << "6. View Borrowed Books Log" << endl;    
        cout << "7. View Monthly Library Revenue($$$)" << endl;
        cout << "8. View Popular Books (***)" << endl;
        cout << "9. View Book Analysis & Summary" << endl;
        cout << "10. Logout" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            cin.clear(); cin.ignore(1000, '\n');
            system("pause");
            continue;
        }

        switch (choice) {
        case 1: addBook(); break;
        case 2: viewBooks(); break;
        case 3: updateBook(); break;
        case 4: deleteBook(); break;
        case 5: searchBook(); break;             
        case 6: viewBorrowedReport(); break;     
        case 7: viewMonthlyFineReport(); break;
        case 8: viewPopularBooks(); break;
        case 9: viewBooksYearStats(); break;
        case 10: cout << "Logging out...\n"; return;
        default: cout << "Invalid choice!\n";
        }

       
        if (choice != 9 && choice != 10) {
            cout << "\nPress Enter to return to dashboard...";
            system("pause > 0");
        }

    } while (true);
}

void studentMenu(string username) {
    int choice;
    do {
        system("cls");
        cout << "\n========================================" << endl;
        cout << "       STUDENT MENU (" << username << ")     " << endl;
        cout << "========================================" << endl;
        cout << "1. View Books" << endl;
        cout << "2. Borrow Book" << endl;
        cout << "3. Return Book" << endl;
        cout << "4. Logout" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: viewBooks(); break;
        case 2: borrowBook(username); break;
        case 3: returnBook(username); break;
        case 4: cout << "Logging out...\n"; return;
        default: cout << "Invalid choice!\n";
        }

        if (choice != 4) {
            cout << "\nPress Enter to return to menu...";
            system("pause > 0");
        }

    } while (true);
}

// --- STUDENT REGISTRATION ---
void registerStudent() {
    string username, password;
    cout << "\n=== Student Registration ===" << endl;
    cout << "Enter new username: "; cin >> username;

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* check = con->prepareStatement("SELECT * FROM users WHERE username = ?");
        check->setString(1, username);
        sql::ResultSet* res = check->executeQuery();

        if (res->next()) {
            cout << "Username already exists!" << endl;
        }
        else {
            cout << "Enter password: "; cin >> password;
            sql::PreparedStatement* insert = con->prepareStatement("INSERT INTO users(username, password, role) VALUES(?, ?, 'student')");
            insert->setString(1, username); insert->setString(2, password);
            insert->execute();
            cout << "Registration successful! You can now log in." << endl;
            delete insert;
        }
        delete res; delete check; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// --- BOOK MANAGEMENT ---
void addBook() {
    string isbn, title, author;
    int year, quantity;
    cout << "\nEnter ISBN: "; cin.ignore(); getline(cin, isbn);
    cout << "Enter title: "; getline(cin, title);
    cout << "Enter author: "; getline(cin, author);
    cout << "Enter year: "; cin >> year;
    cout << "Enter quantity: "; cin >> quantity;

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("INSERT INTO books(isbn, title, author, year, quantity) VALUES(?, ?, ?, ?, ?)");
        pstmt->setString(1, isbn); pstmt->setString(2, title); pstmt->setString(3, author);
        pstmt->setInt(4, year); pstmt->setInt(5, quantity);
        pstmt->execute();
        cout << "Book added successfully!\n";
        delete pstmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void viewBooks() {
    try {
        sql::Connection* con = getDBConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM books");

        cout << "\n--- Book List ---\n";
        cout << left << setw(15) << "ISBN" << setw(25) << "Title" << setw(20) << "Author" << setw(10) << "Year" << setw(15) << "Availability" << endl;

        while (res->next()) {
            int qty = res->getInt("quantity");
            string status = (qty > 0) ? "Available (" + to_string(qty) + ")" : "Out of Stock";
            cout << left << setw(15) << res->getString("isbn") << setw(25) << res->getString("title")
                << setw(20) << res->getString("author") << setw(10) << res->getInt("year") << setw(15) << status << endl;
        }
        delete res; delete stmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// --- SEARCH FUNCTION ---
void searchBook() {
    string keyword;
    cout << "\nEnter Book Title to Search: ";
    cin.ignore(); getline(cin, keyword);

    try {
        sql::Connection* con = getDBConnection();
        
        string query = "SELECT * FROM books WHERE title LIKE ?";
        sql::PreparedStatement* pstmt = con->prepareStatement(query);
        pstmt->setString(1, "%" + keyword + "%");
        sql::ResultSet* res = pstmt->executeQuery();

        cout << "\n--- Search Results ---\n";
        if (res->next()) {
            
            cout << left << setw(15) << "ISBN" << setw(25) << "Title" << setw(20) << "Author" << setw(10) << "Year" << endl;

            
            do {
                cout << left << setw(15) << res->getString("isbn")
                    << setw(25) << res->getString("title")
                    << setw(20) << res->getString("author")
                    << setw(10) << res->getInt("year") << endl;
            } while (res->next());
        }
        else {
            cout << "No matching books found.\n";
        }
        delete res; delete pstmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void updateBook() {
    string isbn, title, author;
    int year, quantity;
    cout << "\nEnter ISBN to update: "; cin.ignore(); getline(cin, isbn);

    cout << "Enter NEW title: "; getline(cin, title);
    cout << "Enter NEW author: "; getline(cin, author);
    cout << "Enter NEW year: "; cin >> year;
    cout << "Enter NEW quantity: "; cin >> quantity;

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* update = con->prepareStatement("UPDATE books SET title=?, author=?, year=?, quantity=? WHERE isbn=?");
        update->setString(1, title); update->setString(2, author);
        update->setInt(3, year); update->setInt(4, quantity); update->setString(5, isbn);
        int rows = update->executeUpdate();
        if (rows > 0) cout << "Book updated.\n"; else cout << "Book not found.\n";
        delete update; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void deleteBook() {
    string isbn;
    cout << "\nEnter ISBN to delete: "; cin.ignore(); getline(cin, isbn);
    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* del = con->prepareStatement("DELETE FROM books WHERE isbn=?");
        del->setString(1, isbn);
        int rows = del->executeUpdate();
        if (rows > 0) cout << "Book deleted.\n"; else cout << "Book not found.\n";
        delete del; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// --- BORROW & RETURN ---
void borrowBook(string username) {
    string isbn;
    cout << "\nEnter ISBN to borrow: "; cin.ignore(); getline(cin, isbn);

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* check = con->prepareStatement("SELECT title, quantity FROM books WHERE isbn=?");
        check->setString(1, isbn);
        sql::ResultSet* res = check->executeQuery();

        if (res->next() && res->getInt("quantity") > 0) {
            sql::PreparedStatement* borrow = con->prepareStatement("INSERT INTO borrow_records(username, isbn, borrow_date, status) VALUES(?, ?, NOW(), 'Borrowed')");
            borrow->setString(1, username); borrow->setString(2, isbn);
            borrow->execute();

            sql::PreparedStatement* stock = con->prepareStatement("UPDATE books SET quantity = quantity - 1 WHERE isbn=?");
            stock->setString(1, isbn);
            stock->execute();

            cout << "Success! Borrowed '" << res->getString("title") << "'. Return in 7 days.\n";
            delete borrow; delete stock;
        }
        else {
            cout << "Book unavailable or not found.\n";
        }
        delete res; delete check; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void returnBook(string username) {
    string isbn;
    cout << "\nEnter ISBN to return: "; cin.ignore(); getline(cin, isbn);

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* check = con->prepareStatement(
            "SELECT id, borrow_date, DATEDIFF(NOW(), borrow_date) as days_held FROM borrow_records WHERE username=? AND isbn=? AND status='Borrowed'"
        );
        check->setString(1, username); check->setString(2, isbn);
        sql::ResultSet* res = check->executeQuery();

        if (res->next()) {
            int id = res->getInt("id");
            int days = res->getInt("days_held");
            double fine = 0.0;

            if (days > 7) {
                fine = (days - 7) * 1.0;
                cout << "\n LATE RETURN: " << (days - 7) << " days late.\n";
                cout << " FINE AMOUNT: RM " << fine << endl;
            }
            else {
                cout << "\nReturned on time. No fine." << endl;
            }

            sql::PreparedStatement* update = con->prepareStatement("UPDATE borrow_records SET status='Returned', return_date=NOW(), fine=? WHERE id=?");
            update->setDouble(1, fine);
            update->setInt(2, id);
            update->execute();

            sql::PreparedStatement* stock = con->prepareStatement("UPDATE books SET quantity = quantity + 1 WHERE isbn=?");
            stock->setString(1, isbn);
            stock->execute();

            cout << "Book returned successfully.\n";
            delete update; delete stock;
        }
        else {
            cout << "No active borrow record found for this book.\n";
        }
        delete res; delete check; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// --- BORROW REPORT ---
void viewBorrowedReport() {
    int reportType;
    int targetMonth, targetYear;

    cout << "\n--- View Borrow Log ---" << endl;
    cout << "1. View Entire History" << endl;
    cout << "2. Filter by Borrow Month & Year" << endl;
    cout << "Enter choice: ";

    if (!(cin >> reportType)) {
        cin.clear(); cin.ignore(1000, '\n');
        return;
    }

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* pstmt;

        if (reportType == 2) {
            cout << "\nEnter Month : "; cin >> targetMonth;
            cout << "\nEnter Year : "; cin >> targetYear;


            string query = "SELECT * FROM borrow_records WHERE MONTH(borrow_date) = ? AND YEAR(borrow_date) = ?";
            pstmt = con->prepareStatement(query);
            pstmt->setInt(1, targetMonth);
            pstmt->setInt(2, targetYear);
        }
        else {
            pstmt = con->prepareStatement("SELECT * FROM borrow_records");
        }

        sql::ResultSet* res = pstmt->executeQuery();

        cout << "\n--- Borrow Records Log ---\n";
        cout << left << setw(15) << "User" << setw(20) << "ISBN" << setw(15) << "Status" << setw(10) << "Fine" << "Date" << endl;
        cout << "----------------------------------------------------------------------\n";

        bool found = false;
        while (res->next()) {
            found = true;
            cout << left << setw(15) << res->getString("username")
                << setw(20) << res->getString("isbn")
                << setw(15) << res->getString("status")
                << setw(10) << res->getDouble("fine")
                << res->getString("borrow_date") << endl;
        }

        if (!found && reportType == 2) {
            cout << "\nNo books were borrowed in that specific month.\n";
        }

        delete res; delete pstmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// ======================================
//  COMPLEX CALCULATION & REPORTING
// ======================================

void viewOverdueReport() {
    try {
        sql::Connection* con = getDBConnection();
        string query = "SELECT username, isbn, borrow_date, DATEDIFF(NOW(), borrow_date) as days_held "
            "FROM borrow_records "
            "WHERE status = 'Borrowed' AND DATEDIFF(NOW(), borrow_date) > 7";

        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);

        cout << "\n==============================================" << endl;
        cout << "        OVERDUE BOOKS / BORROWERS LIST        " << endl;
        cout << "==============================================" << endl;

        if (!res->isBeforeFirst()) {
            cout << "Good news! No books are currently overdue." << endl;
        }
        else {
            cout << left << setw(15) << "User" << setw(20) << "ISBN" << setw(15) << "Days Held" << "Est. Fine" << endl;
            cout << "------------------------------------------------------------" << endl;
            while (res->next()) {
                int days = res->getInt("days_held");
                double estFine = (days - 7) * 1.0;

                cout << left << setw(15) << res->getString("username")
                    << setw(20) << res->getString("isbn")
                    << setw(15) << (to_string(days) + " days")
                    << "RM " << fixed << setprecision(2) << estFine << endl;
            }
            cout << "------------------------------------------------------------" << endl;
            cout << "NOTE: These items need immediate return." << endl;
        }
        delete res; delete stmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

// --- MONTHLY FINE REPORT (REVENUE) ---
void viewMonthlyFineReport() {
    int reportType;
    int targetMonth, targetYear;

    cout << "\n--- Revenue Report ---" << endl;
    cout << "1. View All History " << endl;
    cout << "2. Search Specific Month & Year" << endl;
    cout << "Enter choice: ";

    if (!(cin >> reportType)) {
        cin.clear(); cin.ignore(1000, '\n');
        return;
    }

    try {
        sql::Connection* con = getDBConnection();
        sql::PreparedStatement* pstmt;

        if (reportType == 2) {
            cout << "\nEnter Month : "; cin >> targetMonth;
            cout << "\nEnter Year : "; cin >> targetYear;

            // RETURN date 
            string query = "SELECT YEAR(return_date) as y, MONTHNAME(return_date) as m, SUM(fine) as total "
                "FROM borrow_records "
                "WHERE status='Returned' AND MONTH(return_date) = ? AND YEAR(return_date) = ? "
                "GROUP BY YEAR(return_date), MONTH(return_date)";

            pstmt = con->prepareStatement(query);
            pstmt->setInt(1, targetMonth);
            pstmt->setInt(2, targetYear);
        }
        else {
            string query = "SELECT YEAR(return_date) as y, MONTHNAME(return_date) as m, SUM(fine) as total "
                "FROM borrow_records "
                "WHERE status='Returned' "
                "GROUP BY YEAR(return_date), MONTH(return_date)";
            pstmt = con->prepareStatement(query);
        }

        sql::ResultSet* res = pstmt->executeQuery();

        cout << "\n--- Revenue Report Result ---\n";
        cout << left << setw(10) << "Year" << setw(15) << "Month" << setw(10) << "Revenue" << "Graph (1$ = RM1)" << endl;
        cout << "------------------------------------------------------------\n";

        double grandTotal = 0;
        bool found = false;

        while (res->next()) {
            found = true;
            double total = res->getDouble("total");
            grandTotal += total;

            cout << left << setw(10) << res->getInt("y")
                << setw(15) << res->getString("m")
                << setw(10) << fixed << setprecision(2) << total << "| ";

            for (int i = 0; i < (int)total; i++) cout << "$";
            if (total > 0 && total < 1) cout << ".";
            cout << endl;
        }

        if (!found && reportType == 2) {
            cout << "\nNo records found for that specific month/year.\n";
        }

        cout << "------------------------------------------------------------\n";
        cout << "TOTAL COLLECTED: RM " << grandTotal << endl;

        delete res; delete pstmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void viewPopularBooks() {
    try {
        sql::Connection* con = getDBConnection();
        string query = "SELECT b.title, COUNT(br.isbn) as cnt FROM borrow_records br "
            "JOIN books b ON br.isbn = b.isbn GROUP BY br.isbn ORDER BY cnt DESC LIMIT 5";

        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);

        cout << "\n--- Top 5 Popular Books ---\n";
        cout << left << setw(30) << "Title" << setw(10) << "Borrows" << "Popularity" << endl;
        cout << "------------------------------------------------------------\n";

        while (res->next()) {
            string title = res->getString("title");
            int count = res->getInt("cnt");

            cout << left << setw(30) << title << setw(10) << count << "| ";
            for (int i = 0; i < count; i++) cout << "*";
            cout << endl;
        }
        delete res; delete stmt; delete con;
    }
    catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
}

void viewBooksYearStats() {
    int subChoice;
    do {
        system("cls");
        cout << "\n========================================" << endl;
        cout << "      LIBRARY ANALYSIS REPORT MENU      " << endl;
        cout << "========================================" << endl;
        cout << "1. View Library General Summary" << endl;
        cout << "2. View Overdue / Borrowers List" << endl;
        cout << "3. View New Books " << endl;
        cout << "4. Search book for specific year" << endl;
        cout << "5. Back to Admin Menu" << endl;
        cout << "Enter choice: ";

        if (!(cin >> subChoice)) {
            cout << "Invalid input.\n";
            cin.clear(); cin.ignore(1000, '\n');
            system("pause");
            continue;
        }

        try {
            sql::Connection* con = getDBConnection();

            if (subChoice == 1) {
                sql::Statement* stmt = con->createStatement();

                sql::ResultSet* res1 = stmt->executeQuery("SELECT SUM(quantity) AS total_qty FROM books");
                int totalBooks = 0;
                if (res1->next()) totalBooks = res1->getInt("total_qty");
                delete res1;

                sql::ResultSet* res2 = stmt->executeQuery("SELECT COUNT(*) AS active FROM borrow_records WHERE status='Borrowed'");
                int activeBorrows = 0;
                if (res2->next()) activeBorrows = res2->getInt("active");
                delete res2;

                cout << "\n----------------------------------------" << endl;
                cout << "        LIBRARY BOOKS SUMMARY        " << endl;
                cout << "----------------------------------------" << endl;
                cout << left << setw(30) << "Books Available on Shelf:" << totalBooks << endl;
                cout << left << setw(30) << "Books Currently Borrowed:" << activeBorrows << endl;
                cout << left << setw(30) << "Total Library Assets:" << (totalBooks + activeBorrows) << endl;
                cout << "----------------------------------------" << endl;

                delete stmt;
            }
            else if (subChoice == 2) {
                viewOverdueReport();
            }
            else if (subChoice == 3) {
                sql::Statement* stmt = con->createStatement();
                string query = "SELECT title, author, year FROM books "
                    "WHERE year >= (SELECT AVG(year) FROM books) "
                    "ORDER BY year DESC";
                sql::ResultSet* res = stmt->executeQuery(query);

                cout << "\n--- New Books ---\n";
                cout << left << setw(30) << "Title" << setw(20) << "Author" << setw(10) << "Year" << endl;
                cout << "------------------------------------------------------------\n";
                while (res->next()) {
                    cout << left << setw(30) << res->getString("title")
                        << setw(20) << res->getString("author")
                        << setw(10) << res->getInt("year") << endl;
                }
                delete res; delete stmt;
            }
            else if (subChoice == 4) {
                int targetYear;
                cout << "\nEnter Year to Search: ";
                cin >> targetYear;

                sql::PreparedStatement* pstmt = con->prepareStatement("SELECT title, author, year FROM books WHERE year = ?");
                pstmt->setInt(1, targetYear);
                sql::ResultSet* res = pstmt->executeQuery();

                cout << "\n--- Books Published in " << targetYear << " ---\n";
                cout << left << setw(30) << "Title" << setw(20) << "Author" << setw(10) << "Year" << endl;
                cout << "------------------------------------------------------------\n";
                bool found = false;
                while (res->next()) {
                    found = true;
                    cout << left << setw(30) << res->getString("title")
                        << setw(20) << res->getString("author")
                        << setw(10) << res->getInt("year") << endl;
                }
                if (!found) cout << "(No books found for this year)\n";
                delete res; delete pstmt;
            }
            else if (subChoice == 5) {
                delete con;
                break;
            }

            delete con;
        }
        catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }

        if (subChoice != 5) {
            cout << "\nPress Enter to continue...";
            system("pause > 0");
        }

    } while (subChoice != 5);
}