//All Structures needed are defined here

typedef struct normalUser{
	int userID;
	char name[30];
	char password[10];
	int account_no;
	float balance;
	char status[20];
}normalUser;

typedef struct jointUser{
	int userID;
	char name1[30];
	char name2[30];
	char password[10];
	int account_no;
	float balance;
	char status[20];
}jointUser;


typedef struct admin{
	int userID;
	char username[30];
	char password[10];
}admin;

typedef struct passbook
{
	char type[20];
	int amount;
	int balance;
	time_t time;
}passbook;