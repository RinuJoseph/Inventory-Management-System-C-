#include<iostream>
#include <sqlite3.h>
#include<string>
#include <cstdlib>
#include <stdlib.h>
#include<map>
#include<list>
#include<algorithm>
#include<stdio.h>
#include<fstream>

using namespace::std;

//Global Scope.
bool _isAuth = false;
string _username_global = "rinu";

//Function to display title
void display_title(){
    if (system("CLS")) system("clear");
    cout<<"\n\n\n******************** INVENTORY MANAGEMENT SYSTEM ********************"<<endl<<endl;
}

//Class Definition for inventory display.
class IMS {
    private:
    public:
        auto getProdData();
        auto getOrderData();
        void viewCatalog();
        void purchaseItem();
        void reviews();
        void storeInfo(); 
        void saveOrder(int);  
        void orderHistory();   
        void updateStock(int,int,int);
};

//********************************** Member Functions ***********************************//
//Function to get data from product table
auto IMS::getProdData(){
     sqlite3* DB;
     int _dbStat = 0;
     int _statStep;
     char* error_msg;
     sqlite3_stmt* prdDB; 
     bool _stat =false;
     struct DBresult {int stat;  sqlite3_stmt* prddata;};

    _dbStat = sqlite3_open("IMS_DB.db", &DB);
    _dbStat = sqlite3_prepare_v2(DB, "SELECT * FROM PRODUCT" , -1, &prdDB, NULL);
    // cout<<"getProdData"<<_dbStat;
    if (_dbStat == SQLITE_OK)
    {
        _statStep = sqlite3_step(prdDB);
    }
    return DBresult{_statStep,prdDB};
}

//Function to display view catalog option for customer.
void IMS::viewCatalog(){
     auto prodData = getProdData();
     cout<<"\n\n\n \t\t\t\t *********CATALOG*********"<<endl<<endl;
     cout<<"Item Name \n----------\n";
     while (prodData.stat == SQLITE_ROW)
        {
            cout<<sqlite3_column_text(prodData.prddata, 1) << endl;
            prodData.stat = sqlite3_step(prodData.prddata);
        }
        sqlite3_finalize(prodData.prddata);

}

//Function to place order.
void IMS::purchaseItem(){
    auto prodData = getProdData();
    int _itCode;
    list<string>_itcodeList;

    display_title();

    cout<<"\n\n \t\t*********ITEM LIST*********"<<endl<<endl;
    cout<<"Item ID \tItem Name \tPrice \n---------------------------------------------\n";

    while (prodData.stat == SQLITE_ROW)
    {
        cout<<sqlite3_column_text(prodData.prddata, 0) << "\t\t" <<sqlite3_column_text(prodData.prddata, 1) << "\t\t" << sqlite3_column_text(prodData.prddata, 2)<<endl;
        prodData.stat = sqlite3_step(prodData.prddata);
    }
    sqlite3_finalize(prodData.prddata);

    //For placing order from item list
    cout<<"\n\nEnter Item Code to purchase : ";
    cin>>_itCode;
    saveOrder(_itCode);
       
}

//Function to save order details to DB
void IMS::saveOrder(int _code){
    sqlite3* DB;
    sqlite3* DB1;
    char* error_msg;
    // char* _it;
    sqlite3_stmt* resDB;
    sqlite3_stmt* resDB1;  
    int _db;
    int _quan; 
    int _exit,_qty,price;

    int rc = sqlite3_open("IMS_DB.db", &DB);
    char *sql = "SELECT * FROM PRODUCT WHERE ItemCode = @itemcode";
    
    rc = sqlite3_prepare_v2(DB, sql, -1, &resDB, 0);
    
    if (rc == SQLITE_OK) {
        int idx = sqlite3_bind_parameter_index(resDB, "@itemcode");
        sqlite3_bind_int(resDB, 1, _code);
        
    } else {
        
        cout<<"Failed to execute statement:"<<sqlite3_errmsg(DB)<<"\n";
    }
    int step = sqlite3_step(resDB);
    if (step == SQLITE_ROW) {
        // sqlite3_close(DB);
        // cout<<sqlite3_column_int(resDB, 3)<<"\n"; 
        cout<<"Enter Quantity : "; 
        cin>>_quan;
        _qty = sqlite3_column_int(resDB, 3);
        price = sqlite3_column_int(resDB, 2);
        string _it = (char*)sqlite3_column_text(resDB, 1);
        // cout<<"it"<<_it;

        sqlite3_finalize(resDB);
        sqlite3_close(DB);

        if(_qty<_quan){
            cout<<"\nNot enough stock please try again.........."<<endl;
        }
        else{
            //update stock in prod table

            //insert into order table
            int tot = _quan*price;
            int rc1 = sqlite3_open("IMS_DB.db", &DB1);
            string qry = "INSERT INTO ORDERS (ItemCode,Orderby,ItemName,Quantity,Amount) VALUES('"+to_string(_code)+"','"+_username_global+"','"+_it+"','"+to_string(_quan)+"','"+to_string(tot)+"')";
            // string qry = "INSERT INTO ORDERS (ItemCode,Orderby,ItemName,Quantity,Amount) VALUES('206','rinu','skirt','2','20')";
            rc1 = sqlite3_prepare_v2(DB, qry.c_str(), -1, &resDB1, 0);
            
            if (rc1 == SQLITE_OK) {
            //  cout<<"Order Placed successfully\n";
            } 
            else {
                cout<<"Failed to execute statement:"<<sqlite3_errmsg(DB1)<<"\n";
            }
            int step1 = sqlite3_step(resDB1);
            // cout<<"\n"<<step1;
            if (step1 == 101) {
                cout<<"Order Placed successfully..";
                updateStock(_code,_qty,_quan);
            }
            else{
                cout<<"error in db";
            }
            sqlite3_close(DB1);
        }
    }
    else{
        purchaseItem(); 
    }
    
}

//Function to update the stock in product table after placing and order.
void IMS::updateStock(int _cde,int _sqty,int _qty){
    sqlite3* DB;
    char* error_msg;
    sqlite3_stmt* resDB; 
    int _newqty = _sqty - _qty;
    // cout<<_newqty;

    
    string qry = "UPDATE PRODUCT SET Stcok='"+to_string(_newqty)+"' WHERE ItemCode = '"+to_string(_cde)+"'";
    int rc = sqlite3_open("IMS_DB.db", &DB);
    rc = sqlite3_prepare_v2(DB, qry.c_str(), -1, &resDB, 0);
     if (rc == SQLITE_OK) {
    //  cout<<"OKKkk";
     }
    else {
        cout<<"Failed to execute statement:"<<sqlite3_errmsg(DB)<<"\n";
    }
    int step1 = sqlite3_step(resDB);
    // cout<<"\n"<<step1;
    if (step1 == 101) {
        // cout<<"Stock updated";
    }
    else{
        cout<<"error in db";
    }
}

//Function to display store informations.
void IMS::storeInfo(){
    fstream file;
    file.open('store_info.csv', ios::in);

}


//********************************** Non Member Functions ***********************************//
//Function to create a database, table and store few user information to authenticate the user.
void db_data_creation(){
    sqlite3* DB;
    string sql_comm = "CREATE TABLE USER_AUTH("
                        "USERNAME    TEXT PRIMARY KEY    NOT NULL,"
                        "PASSWORD    CHAR(50) NOT NULL);";
    int _dbStat = 0;
    _dbStat = sqlite3_open("IMS_DB.db", &DB);
    char* error_msg;
    _dbStat = sqlite3_exec(DB, sql_comm.c_str(), NULL, 0, &error_msg);

    string sql("INSERT INTO USER_AUTH VALUES('rinu', 'abc');"
               "INSERT INTO USER_AUTH VALUES('nisa', 'xyz');");
  
    _dbStat = sqlite3_exec(DB, sql.c_str(), NULL, 0, &error_msg);
}

//Function to get user infor from db to authenticate user.
bool user_auth(string _uname, string _pswd){
     sqlite3* DB;
     int _dbStat = 0;
     char* error_msg;
     sqlite3_stmt* authData; 
     bool _stat =false;
    //  string user = "'"+_uname+"'";
    //  string sql_comm = "SELECT * FROM USER_AUTH WHERE USERNAME=";
    //  string qry = sql_comm+_uname;

    _dbStat = sqlite3_open("IMS_DB.db", &DB);
    _dbStat = sqlite3_prepare_v2(DB, "SELECT * FROM USER_AUTH" , -1, &authData, NULL);

    if (_dbStat == SQLITE_OK)
    {
        int _stepStat = sqlite3_step(authData);

        while (_stepStat == SQLITE_ROW)
        {
            // cout << "name: " << sqlite3_column_text(authData, 0) << endl;
            // cout << "password: " << sqlite3_column_text(authData, 1) << endl;
            string u = (char*)sqlite3_column_text(authData, 0);
            string p = (char*)sqlite3_column_text(authData, 1);
            // cout<<u;
            // cout<<p;
            if(u == _uname && p == _pswd){
                _stat = true;
            }
                _stepStat = sqlite3_step(authData);
        }
        sqlite3_finalize(authData);
    }
    else
    {
        cout << "Problem creating a prepared statement" << endl;
    }
    return _stat;
}

//Function to display user roles.
int display_user_roles(){
    int user_choice;
    // bool _isAuth = false;
    string _username;
    string _passwrd;

    // display_title();
    cout<<" 1. Customer \n 2. Administrator \n \nPlease Enter Your Choice to Continue : ";
    cin>>user_choice;
    // if (system("CLS")) system("clear");
    display_title();

    //User Authentication 
    if(!_isAuth){
        cout<<"Please Enter Username and Password to Continue....."<<endl<<endl;
        cout<<"Username : ";
        cin>>_username;
        cout<<"Password : ";
        cin>>_passwrd;

        _isAuth = user_auth(_username,_passwrd);
        // if (system("CLS")) system("clear");

        display_title();
        if(_isAuth){
            cout<<"\nUser Authentication Successfull ......."<<endl;
            // _authFlag = true;
            _username_global = _username;

        }
        else{
            cout<<"\nAuthentication Failed Please Try Again !!!!!!"<<endl<<endl;
            display_user_roles();
        }
    }
    return user_choice;
}

//Function to display customer menu.
void display_cust_menu(){
    int _custChoice;
    int _exit;
    cout<<" 1. View Catalog \n 2. Purchase Item \n 3. Order History \n 4. Leave Review \n 5. Store Information \n 6. Exit\n \nPlease Enter Your Choice to Continue : ";
    cin>>_custChoice;
    display_title();
    IMS im;
    if(_custChoice == 1){
        im.viewCatalog();
    }
    else if(_custChoice == 2){
        im.purchaseItem();
    }
    else if(_custChoice == 3){

    }
    else if(_custChoice == 4){

    }
    else if(_custChoice == 5){

    }
    else{}
    cout<<"\n\nPress 0 to exit to customer menu : ";
    do{
        cin>>_exit;
        if(_exit ==0){
            display_title();
            display_cust_menu();
        }
    }while(_exit != 0);
}
//Function to display admin menu.
void display_admin_menu(){
    int _adminChoice;
    cout<<" 1. Create Item \n 2.Modify Item \n 3. Delete Item \n 4.View Inventory \n 5. View Orders \n 6.Exit\n \nPlease Enter Your Choice to Continue : ";
    cin>>_adminChoice;
    // if(_custChoice == 1)
    // else if(_adminChoice == 2)
    // else if(_adminChoice == 3)
    // else if(_adminChoice == 4)
    // else if(_adminChoice == 5)
    // else
}

void ims_init(){
     int _usrrole;
    _usrrole = display_user_roles();
    if(_usrrole == 1){
       display_cust_menu();
    }
    else if(_usrrole ==2){
       display_admin_menu();
    }
}


int main(){

    //Variable declarations
    // int _usrrole;

    cout<<"\n\n\n******************** INVENTORY MANAGEMENT SYSTEM ********************"<<endl<<endl;

    // db_data_creation();  //Executed only one time to create db and table data.
    // _usrrole = display_user_roles();

    // if(_usrrole == 1){
    //    display_cust_menu();
    // }
    // else if(_usrrole ==2){
    //    display_admin_menu();
    // }

    // ims_init();
    IMS im;
    im.purchaseItem(); 
}