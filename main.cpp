#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <windows.h>
#include <sstream>
using namespace std;

const char* HOST = "localhost";
const char* USER = "root";
const char* PW = "Tanu@123";
const char* DB = "mydb";

class Student{
private:
 string Id;
 string Name;
 int Age;
public:
Student() : Id(""), Name(""), Age(0) {}

void setId(string id) {
  Id = id;
 }
 
 string getId() {
  return Id;
 }
void setName(string name){
	Name=name;
} 
string getName(){
	return Name;
}
void setAge( int age){
	Age=age;
} 
int getAge(){
	return Age;
}
 
};

class Library{
private:
 string Name;
 int Quantity;
public:
Library() : Name(""), Quantity(0) {}

void setName(string name) {
  Name = name;
}

void setQuantity(int quantity) {
  Quantity = quantity;
}

int getQuantity() {
  return Quantity;
}
string getName() {
  return Name;
}

	
};

void admin(MYSQL* conn, Library l, Student s){
	bool closed = false;
	while(!closed){
	int choice;
cout << "1. Add Book." << endl;
 cout << "2. Add Student." << endl;
 cout << "0. Exit." << endl;
 cout << "Enter Choice: ";
 cin >> choice;
 
 if(choice==1){
 	system("cls");
string name;
int quantity;

cout<<"Book Name: ";
cin>>name;
l.setName(name);

cout<<"Enter Quantity: ";
cin>>quantity;
l.setQuantity(quantity);

int Iq = l.getQuantity();
stringstream ss;
ss<<Iq;
string Sq = ss.str();

string book = "INSERT INTO lib (Name,Quantity) VALUES('"+l.getName()+"', '"+Sq+"') ";
if(mysql_query(conn,book.c_str())){
	cout<<"Error: "<<mysql_error(conn)<<endl;
}
else{
	cout<<"Book Inserted Successfuly"<<endl;
}
 }// if1
 
 else if(choice==2){
 	cout<<"RULE OF LIBRARY "<<endl;
 	
 	cout<<"** If you have 2 borrow book you have to return 1 book to borrow another book **"<<endl;
 	cout<<"Type Okay if you are agree else type No"<<endl;
 	string type;
 	cin>>type;
 	if(type=="Okay"){
 	system("cls");
string id;
 cout << "Enter Student ID: ";
 cin >> id;
 s.setId(id);
 
  string st = "INSERT INTO student (Id) VALUES('" + s.getId() + "')";
  string name;
  cout<<"Enter student Name: ";
  cin>>name;
  s.setName(name);
 // string na="INSERT INTO student (Name) VALUES('"+s.getName()+"')";

   int age;
  cout<<"Enter Age: ";
  cin>>age;
  s.setAge(age);

  int ag = s.getAge();
  stringstream aa;
  aa<<ag;
  string a = aa.str();
  string student = "INSERT INTO student (Name,Age) VALUES('"+l.getName()+"', '"+a+"') ";
  
 if (mysql_query(conn, st.c_str())) {
 cout << "Error: " << mysql_error(conn) << endl;
 }
else {
  cout << "Student Inserted Successfully" << endl; 
}}
else{
	cout<<"You need to follow rule"<<endl;
}
}//if2
else if(choice ==0){
	closed = true;
	cout<<"System is closing"<<endl;
}
	}//while
Sleep(3000);
}//function

//display function
void display(MYSQL* conn){
	system("cls");
cout<<"Available Books"<<endl;
cout<<"***************"<<endl;

string disp= "SELECT * FROM lib";
 if (mysql_query(conn, disp.c_str())) {
  cout << "Error: " << mysql_error(conn) << endl;
 }
 else{
 	MYSQL_RES* res;
 	res= mysql_store_result(conn);
 	if(res){
 		int num= mysql_num_fields(res);
 	MYSQL_ROW row;
 	while(row=mysql_fetch_row(res)){
 		for(int i=0; i< num; i++){
 			cout<<" "<<row[i];
		 }
		cout<<endl;
	 }
	 mysql_free_result(res);
	 }
 }
}

//book function

int book(MYSQL* conn, string Bname){
string exist = "SELECT Name, Quantity FROM lib WHERE Name = '" + Bname + "'";
if (mysql_query(conn, exist.c_str())) {
 cout << "Error: " << mysql_error(conn) << endl;
 }
else{
MYSQL_RES* res;
res = mysql_store_result(conn);
if(res){
	int num = mysql_num_fields(res);
MYSQL_ROW row;
while(row=mysql_fetch_row(res)){
	for(int i=0; i< num; i++){
	if(Bname == row[i]){
	int quantity = atoi(row[i+1]);
	return quantity;	
	}
	else{
		cout<<"Book Not Found."<<endl;
	}
	}
} 
mysql_free_result(res);
}
	
}//else if exist
return 0;
Sleep(5000);
}//function

// user function

void user(MYSQL* conn, Library l, Student s) {
    system("cls");
    display(conn);
    string Sid;
    cout << "Enter Your ID: ";
    cin >> Sid;

    // Check if Student ID exists
    string com = "SELECT * FROM student WHERE Id = '" + Sid + "'";
    if (mysql_query(conn, com.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cout << "Error fetching student data: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_num_rows(res) != 1) {
        cout << "This Student is Not Registered." << endl;
        mysql_free_result(res);
        return;
    }

    cout << "Student ID Found." << endl;
    mysql_free_result(res); // Free result set

    // FIX: Use correct column names 'Student_Id' and 'book_name' in borrowed_books table
    string countQuery = "SELECT COUNT(*) FROM borrowed_books WHERE Student_Id = '" + Sid + "'"; 
    if (mysql_query(conn, countQuery.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* countRes = mysql_store_result(conn);
    if (!countRes) {
        cout << "Error fetching borrowed book count: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW countRow = mysql_fetch_row(countRes);
    int borrowedCount = (countRow && countRow[0]) ? atoi(countRow[0]) : 0; // Check for NULL
    mysql_free_result(countRes); // Free result set

    if (borrowedCount >= 2) {
        cout << "You have already borrowed 2 books. Please return at least 1 book before borrowing another." << endl;
        return;
    }

    // Proceed with borrowing
    string Bname;
    cout << "Enter Book Name: ";
    cin >> Bname;

    int availableBooks = book(conn, Bname); // Call book() only once
    if (availableBooks <= 0) {
        cout << "Book is not available." << endl;
        return;
    }

    // Update book quantity
    stringstream ss;
    ss << (availableBooks - 1);
    string Sq = ss.str();
    string upd = "UPDATE lib SET Quantity = '" + Sq + "' WHERE Name = '" + Bname + "'";

    if (mysql_query(conn, upd.c_str())) {
        cout << "Error updating book quantity: " << mysql_error(conn) << endl;
        return;
    }

    cout << "Book is available. Borrowing Book..." << endl;

    // Insert into borrowed_books table with correct column name 'book_name'
    string insertBorrow = "INSERT INTO borrowed_books (Student_Id, book_name) VALUES ('" + Sid + "', '" + Bname + "')";
    if (mysql_query(conn, insertBorrow.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Book borrowed successfully!" << endl;
    }
}


void returnBook(MYSQL* conn, string studentId) {
    string Bname;
    cout << "Enter Book Name to Return: ";
    cin >> Bname;

    // Check if the student actually borrowed this book
    string checkQuery = "SELECT * FROM borrowed_books WHERE Student_Id = '" + studentId + "' AND book_name = '" + Bname + "'";
    if (mysql_query(conn, checkQuery.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }
    
    MYSQL_RES* res = mysql_store_result(conn);
    int num = mysql_num_rows(res);
    mysql_free_result(res);

    if (num == 0) {
        cout << "You have not borrowed this book." << endl;
        return;
    }

    // Remove the record from borrowed_books
    string deleteQuery = "DELETE FROM borrowed_books WHERE Student_Id = '" + studentId + "' AND book_name = '" + Bname + "' LIMIT 1";
    if (mysql_query(conn, deleteQuery.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else {
        // Increase book quantity in lib table
        string updateQuery = "UPDATE lib SET Quantity = Quantity + 1 WHERE Name = '" + Bname + "'";
        if (mysql_query(conn, updateQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        } else {
            cout << "Book returned successfully!" << endl;
        }
    }
}

int main() {
    Student s;
    Library l;

    MYSQL* conn;
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, HOST, USER, PW, DB, 3306, NULL, 0)) {
        cout << "Error: " << mysql_error(conn) << endl;
        return 1;
    } else {
        cout << "Logged In!" << endl;
    }
    Sleep(3000);

    // ? Declare exit before using it
    bool exit = false;

    // ? Check if while loop is inside main()
    while (!exit) {
        system("cls");
        int val;
        cout << "Welcome To Library Management System" << endl;
        cout << "************************************" << endl;
        cout << "1. Administration." << endl;
        cout << "2. User." << endl;
        cout << "3. Return Book." << endl;
        cout << "0. Exit." << endl;
        cout << "Enter Choice: ";
        cin >> val;

        if (val == 1) {
            system("cls");
            admin(conn, l, s);
        } else if (val == 2) {
            user(conn, l, s);
            Sleep(5000);
        } else if (val == 3) {
    string studentId; // Declare studentId
    cout << "Enter Your Student ID: ";
    cin >> studentId;  // Take user input
    returnBook(conn, studentId);  
    Sleep(5000);
}
 else if (val == 0) {
            exit = true;
            cout << "Good Luck!" << endl;
            Sleep(3000);
        }
    }

    // ? Close MySQL connection properly
    mysql_close(conn);

    // ? Ensure main() has a return statement
    return 0;
}

