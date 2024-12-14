#include <iostream>
#include <pqxx/pqxx>
#include <conio.h> // For getch()
#include <cstdlib>
#include <chrono>
#include <thread>
#include <cctype>



using namespace std;
using namespace pqxx;

void clearScreen() {
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#else
    system("clear"); // Clear screen for Linux/macOS
#endif
}

void insertion(connection& conn) {
    try {
        string description, due_date;
        bool status = false;
        char status_input, insert_confirm;

        // Input for description
        cout << "Enter task description: ";
        getline(cin, description);

        // Input for due date
        cout << "Enter due date (YYYY-MM-DD): ";
        getline(cin, due_date);

        // Input for status
        while (true) {
            cout << "Is the task completed? (Y/N): ";
            _getch() >> status_input;
            status_input = tolower(status_input);
            if (status_input == 'y') {
                status = true;
                break;
            }
            else if (status_input == 'n') {
                status = false;
                break;
            }
            else {
                cout << "Invalid input. Please enter Y or N.\n";
            }
        }

        // Confirm insertion
        do {
            cout << "\nAre you sure you want to commit changes?\n\nY to accept\nN to decline\n";
            cin >> insert_confirm;
            insert_confirm = tolower(insert_confirm);

            if (insert_confirm == 'y') {
                try {
                    // Declare work transaction object in the correct scope
                    work txn(conn);

                    // Construct the SQL INSERT query
                    string query = "insert into tasks (description, due_date, status) values (" +
                        txn.quote(description) + "," +
                        txn.quote(due_date) + "," +
                        (status ? "TRUE" : "FALSE") + ");";

                    txn.exec(query); // Execute the query
                    cout << "\nInserting changes...";
                    txn.commit(); // Commit the transaction
                    this_thread::sleep_for(chrono::milliseconds(3000));
                    clearScreen();
                    insert_confirm = 'n';
                    break;
                }
                catch (const exception& e) {
                    cerr << "\nError during transaction: " << e.what() << endl;
                    return;
                }
            }
            else if (insert_confirm == 'n') {
                cout << "Changes were not saved.\n";
                break;
            }
            else {
                cout << "\nInvalid input. Please press Y to accept or N to decline.\n";
            }
        } while (insert_confirm != 'n');
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}


void showtable(connection& conn) {
    clearScreen();
    try {
        work txn(conn);
        string query = "select * from tasks";
        result res = txn.exec(query);
        cout << left << setw(5) << "ID" << setw(30) << "Description" << setw(15) << "Due Date" << setw(10) << "Status" << endl;
        cout << "-------------------------------------------------------------" << endl;
        txn.commit();
        for (auto row : res) {
            cout << left << setw(5) << row["id"].as<int>()
                << setw(30) << row["description"].as<string>()
                << setw(15) << row["due_date"].as<string>()
                << setw(10) << (row["status"].as<bool>() ? "Colmpleted" : "Undone") << endl;

        }
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

int main() {
    try {
        string bdssrting = "dbname=todolist user=postgres password=postgres host=localhost";
        // Create a connection object to the database
        connection conn(bdssrting);

        // Check if connection was successful
        if (conn.is_open()) {
            cout << "Connected to " << conn.dbname() << endl;
        }
        else {
            cout << "Failed to connect to the database!" << endl;
            return 1;
        }
        this_thread::sleep_for(chrono::milliseconds(2000));
        clearScreen();      // Create a transactional object to execute SQL commands
        char option;  // Declare option here so it is accessible in the do-while loop
        do {
            // Display the menu options to the user
            cout << "\nMenu:\n";
            cout << "i: Input task\n";
            cout << "d: Delete task\n";
            cout << "s: Show tasks\n";
            cout << "q: Quit\n";
            cout << "Enter your choice: ";

            // Get user input using getch (no need to press Enter)
            option = _getch();
            bool validchoice = false;
            // Handle the input based on the option chosen
            switch (option) {
            case 'i':
                // Input task function
                cout << "\nInput task placeholder\n";
                insertion(conn);
                validchoice = true;
                break;
            case 'd': {
                // Delete task function
                cout << "\nDelete task placeholder\n";
                validchoice = true;
                break;
            }
            case 's': {
                // Show task function
                cout << "\nShow tasks placeholder\n";
                showtable(conn);
                validchoice = true;
                break;
            }
            case 'q':
                cout << "\nExiting...\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
                break;
            }       
            cout << "\nPress Enter to continue...";
            while (_kbhit()) _getch();  // Clear any remaining characters in the buffer
            _getch();  // Wait for Enter (or any key press)
            clearScreen();  // Clear the terminal screen
        } while (option != 'q'); // Keep looping until 'q' is pressed

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
