#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<ctype.h>

#include "structures.h" //All structures are defined in this header file

//Semaphore variable declaration
int semIdentifier, semIdentifier1;
bool lock_critical_section(struct sembuf *semOp, int semIdentifier);
bool unlock_critical_section(struct sembuf *sem_op, int semIdentifier);

//Declaration of all functions
void attemptNormalUserLogin(int sd);
void attemptJointUserLogin(int sd);
void attemptAdminLogin(int sd);
void showMenu(int sd);
void deposit(int sd);
void withdraw(int sd);
void balanceEnquiry(int sd);
void changePassword(int sd);
void viewDetails(int sd);
void addAccount(int sd);
void deleteAccount(int sd);
void modifyAccount(int sd);
void searchAccount(int sd);
void create_admin(int sd);
//option variable will be same throughout for an instance of client
int option,currUserID;

union semun{ //Union Declaration for semaphore
	int val; // Value of the semaphore
} semSet,semSet1;
		
//Defining locking mechanism of Binary semaphore
bool lock_critical_section(struct sembuf *semOp, int semIdentifier){
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }
    return true;
}

//Defining Unlocking mechanism of Binary semaphore
bool unlock_critical_section(struct sembuf *semOp, int semIdentifier){
    semOp->sem_op = 1;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while operating on semaphore!");
        _exit(1);
    }
    return true;
}

//Welcome Page of Account Login Mode
void chooseOption(int sd){
	printf("\e[1;1H\e[2J");
	printf("\n\n\n\t\t***********************************************************************************************************\n");
	printf("\t\t****************************  WELCOME TO BANKING SYSTEM  Account Login Mode  ******************************\n");
	printf("\t\t***********************************************************************************************************\n\n\n");
	printf("1 : NORMAL-ACCOUNT USER LOGIN\n");
	printf("2 : JOINT-ACCOUNT USER LOGIN\n");
	printf("3 : ADMIN LOGIN\n");
	printf("4 : EXIT\n");
	printf("Select an Option to perform : ");
	scanf("%d",&option);
	switch(option){
		case 1 :
			attemptNormalUserLogin(sd);
			break;
		case 2 :
			attemptJointUserLogin(sd);
			break;
		case 3 :
			attemptAdminLogin(sd);
			break;
		case 4 :
			printf("\n\n\n\t\t***********************************************************************************************************\n");
			printf("\t\t*************************************** Thanks For Visiting Us ********************************************\n");
			printf("\t\t***************************************    Have a Nice Day     ********************************************\n");
			printf("\t\t***********************************************************************************************************\n\n\n");
			exit(0);
			break;
		default :
			printf("\n\t\t\t\t\t!!!INVALID OPTION!!! Please Select a Valid Option...\n");
			char c;
			while((c = getchar()) != '\n' && c != EOF) //Used to clear Keyboard Buffer
				;
			sleep(3);
			chooseOption(sd);
			break;
	}
	return;
}

//Welcome Page of User Portal after logging In (Option 1 & 2) and Admin Portal after logging In (Option 3) 
void showMenu(int sd){
	int select;
	int writeStatus;
	if(option==1 || option==2){
		printf("\e[1;1H\e[2J");
		printf("\n\n\t\t***********************************************************************************************************\n");
		printf("\t\t*******************************  WELCOME TO BANKING SYSTEM  USER-PORTAL  **********************************\n");
		printf("\t\t***********************************************************************************************************\n\n\n");
		printf("1 : DEPOSIT\n");
		printf("2 : WITHDRAW\n");
		printf("3 : CHECK BALANCE\n");
		printf("4 : VIEW TRANSACTIONS(PASSBOOK)\n");
		printf("5 : PASSWORD CHANGE\n");
		printf("6 : EXIT\n");
		printf("Select an option : ");
		scanf("%d",&select);
		switch(select){
			case 1 :
				deposit(sd);
				break;
			case 2 :
				withdraw(sd);
				break;
			case 3 :
				balanceEnquiry(sd);
				break;
			case 4 :
				viewDetails(sd);
				break;
			case 5 :
				changePassword(sd);
				break;
			case 6 :
				writeStatus=write(sd,&select,sizeof(int));
				if(writeStatus==-1){
					perror("Error while writing data to server");
					_exit(1);
				}
				printf("\n\n\n\t\t***********************************************************************************************************\n");
				printf("\t\t*************************************** Thanks For Visiting Us ********************************************\n");
				printf("\t\t***************************************    Have a Nice Day     ********************************************\n");
				printf("\t\t***********************************************************************************************************\n\n\n");
				exit(0);
			default :
				printf("\n\t\t\t\t\t!!!INVALID OPTION!!! Please Select a Valid Option...\n");
				char c;
				while((c = getchar()) != '\n' && c != EOF) //To clear Keyboard Buffer
					;
				sleep(3);
				showMenu(sd);
				break;
		}
	}
	else if(option==3){
		printf("\e[1;1H\e[2J");
		printf("\n\n\n\t\t***********************************************************************************************************\n");
		printf("\t\t*****************************   WELCOME TO BANKING SYSTEM  ADMIN-PORTAL   *********************************\n");
		printf("\t\t***********************************************************************************************************\n\n\n");
		printf("1 : ADD ACCOUNT\n");
		printf("2 : DELETE ACCOUNT\n");
		printf("3 : MODIFY ACCOUNT DATA\n");
		printf("4 : SEARCH ACCOUNT\n");
		printf("5 : CREATE NEW ADMINISTRATOR ID\n");
		printf("6 : EXIT\n");
		printf("Select an option : ");
		scanf("%d",&select);
		switch(select){
			case 1 :
				addAccount(sd);
				break;
			case 2 :
				deleteAccount(sd);
				break;
			case 3 :
				modifyAccount(sd);
				break;
			case 4 :
				searchAccount(sd);
				break;
			case 5 :
				create_admin(sd);
				break;
			case 6 :
				write(sd,&select,sizeof(int));
				printf("\n\n\n\t\t***********************************************************************************************************\n");
				printf("\t\t*************************************** Thanks For Visiting Us ********************************************\n");
				printf("\t\t***************************************    Have a Nice Day     ********************************************\n");
				printf("\t\t***********************************************************************************************************\n\n\n");
				exit(0);
			default :
				printf("Invalid option!Select valid option...\n");
				char c;
				while((c = getchar()) != '\n' && c != EOF) //To clear Keyboard Buffer
					;
				sleep(3);
				showMenu(sd);
				break;
		}
	}
}

//Verifies Normal Account User login credentials
void attemptNormalUserLogin(int sd){   
	bool result;
	normalUser currUser;
	printf("\e[1;1H\e[2J");
	printf("\n\t\t\t\t\t\t******* NORMAL-ACCOUNT USER LOGIN *******\n");
	printf("\nENTER USER ID : ");
	scanf("%d",&currUser.userID); //We input User Id
	currUserID=currUser.userID;
  	char *pass=getpass("ENTER PASSWORD : "); //We input Password
  	memcpy(currUser.password,pass,sizeof(pass));
	//To the server
	int writeStatus=write(sd,&option,sizeof(int)); //We send the option i.e 1-Normal User
	if(writeStatus==-1){
		perror("Error while writing data to the server in attemptNormalUserLogin");
		_exit(1);
	}
	writeStatus=write(sd,&currUser,sizeof(normalUser)); //We send the input details for verification
	if(writeStatus==-1){
		perror("Error while writing User data to the server in attemptNormalUserLogin");
		_exit(1);
	}
	int readStatus=read(sd,&result,sizeof(result)); //We get results from the server True if success else false
    if(readStatus==-1){
    	perror("Error while reading data from the server in attemptNormalUserLogin");
    	_exit(1);
    }
	if(!result){
		printf("\n\t\t******************** LOGIN FAILED!! Enter Proper Credentials/Check if Account is Active ********************\n\n");
		char c;
		while((c = getchar()) != '\n' && c != EOF) //To clear Keyboard Buffer
			;
		sleep(3);
		chooseOption(sd);
	}
	else{
		printf("\n\t\t\t******************************  !!SUCCESSFULLY LOGGED IN!!  ******************************\n\n");
		sleep(3);
		//After Successfull Login we Initialise a semphore for each user to restrict critical operations to single user.
		//Semaphore - Deposit
		key_t semKey=ftok("Normal_user",currUser.userID); // Generate a key based on the account number hence, different customers will have different semaphores
		// union semun{
		//     int val; // Value of the semaphore
		// } semSet,semSet1;
		int semctlStatus;
		semIdentifier=semget(semKey,1,0); // Get the semaphore if it exists
		if(semIdentifier==-1){
		    semIdentifier=semget(semKey,1,IPC_CREAT|0700); // Create a new semaphore
		    if(semIdentifier==-1){
		        perror("Error while creating semaphore!");
		        _exit(1);
		    }
		    semSet.val=1; // Set a binary semaphore
		    semctlStatus=semctl(semIdentifier,0,SETVAL,semSet);
		    if(semctlStatus==-1){
		        perror("Error while initializing a binary sempahore!");
		        _exit(1);
		    }
		}
		//Semaphore - Withdraw
		int semctlStatus1;
		semIdentifier1=semget(semKey,1,0); // Get the semaphore if it exists
	}
	return;
}

//Verifies Joint Account User login credentials
void attemptJointUserLogin(int sd){
	bool result;
	jointUser currUser;    
	printf("\e[1;1H\e[2J");
	printf("\n\t\t\t\t\t\t******* JOINT-ACCOUNT USER LOGIN *******\n");
	printf("\nENTER USER ID : ");
	scanf("%d",&currUser.userID); //Input User Id
	currUserID=currUser.userID;
  	char *pass=getpass("ENTER PASSWORD : "); //Input Password
  	memcpy(currUser.password,pass,sizeof(pass));
	//To the server
	int writeStatus=write(sd,&option,sizeof(int)); //Write option to server indicating Joint Account user i.e Option-2
	if(writeStatus==-1){
		perror("Error while writing data to the server in attemptJointUserLogin");
		_exit(1);
	}
	writeStatus=write(sd,&currUser,sizeof(jointUser)); //Write input info to server for verification
	if(writeStatus==-1){
		perror("Error while writing user data to the server in attemptJointUserLogin");
		_exit(1);
	}
	int readStatus=read(sd,&result,sizeof(result)); //Read result from server, True if success else false
	if(readStatus==-1){
		perror("Error while reading data from the server in attemptJointUserLogin");
	}
	if(!result){
		printf("\n\t\t******************** LOGIN FAILED!! Enter Proper Credentials/Check if Account is Active ********************\n\n");
		char c;
		while((c = getchar()) != '\n' && c != EOF) //To clear Keyboard Buffer
			;
		sleep(3);
		chooseOption(sd);
	}
	else{
		printf("\n\t\t\t******************************  !!SUCCESSFULLY LOGGED IN!!  ******************************\n\n");
		sleep(3);
		//Creating Binary semaphore for each Joint account user while logging in to restrict critical operations
		//Semaphore - Deposit 
		key_t semKey=ftok("Joint_user",currUser.userID); // Generate a key based on the account number hence, different customers will have different semaphores
		// union semun{
		//     int val; // Value of the semaphore
		// } semSet,semSet1;
		int semctlStatus;
		semIdentifier=semget(semKey,1,0); // Get the semaphore if it exists
		if(semIdentifier==-1){
		    semIdentifier=semget(semKey,1,IPC_CREAT|0700); // Create a new semaphore
		    if(semIdentifier==-1){
		        perror("Error while creating semaphore!");
		        _exit(1);
		    }
		    semSet.val=1; // Set a binary semaphore
		    semctlStatus=semctl(semIdentifier,0,SETVAL,semSet);
		    if(semctlStatus==-1){
		        perror("Error while initializing a binary sempahore!");
		        _exit(1);
		    }
		}
		//Semaphore - Withdraw
		int semctlStatus1;
		semIdentifier1=semget(semKey,1,0); // Get the semaphore if it exists
	}
	return;
}

//Verifies Admin login credentials
void attemptAdminLogin(int sd){
	bool result;
	admin currUser;
	printf("\e[1;1H\e[2J");
	printf("\n\t\t\t\t\t\t******* ADMINISTRATOR ACCOUNT LOGIN *******\n");
	printf("\nENTER USER ID : ");
	scanf("%d",&currUser.userID); //Input Admin Userid
	currUserID=currUser.userID;
  	char *pass=getpass("ENTER PASSWORD : "); //Input Admin Password
  	memcpy(currUser.password,pass,sizeof(pass));
	//to the server
	int writeStatus=write(sd,&option,sizeof(int)); //Send Option as 3 to the server
	if(writeStatus==-1){
		perror("Error while writing data to the server in attemptAdminLogin");
		_exit(1);
	}
	writeStatus=write(sd,&currUser,sizeof(admin)); //Send input info to the server for verification
	if(writeStatus==-1){
		perror("Error while writing user data to the server in attemptAdminLogin");
		_exit(1);
	}
	int readStatus=read(sd,&result,sizeof(result)); //Read result from server, True for success else false
	if(readStatus==-1){
		perror("Error while reading data from the server in attemptAdminLogin");
		_exit(1);
	}
	if(!result){
		printf("\n\t\t\t\t***************** LOGIN FAILED!! Enter Proper Credentials *****************\n\n");
		char c;
		while((c = getchar()) != '\n' && c != EOF) //To clear Keyboard Buffer
			;
		sleep(3);
		chooseOption(sd);
	}
	else{
		printf("\n\t\t\t******************************  !!SUCCESSFULLY LOGGED IN!!  ******************************\n\n");
		sleep(3);
	}
	return;
}

//User Functions
//Function to deposit amount into User Account
void deposit(int sd){
	float amt;
	int select=1;
	bool result;
	int balance;
	struct sembuf semOp;
	if(!semctl(semIdentifier,0,GETVAL,semSet)){
		printf("\nLoading ...\n");
	}
	//Locking the section using semaphore as only one instance of an account can deposit at a time.
    lock_critical_section(&semOp,semIdentifier); 
    printf("\e[1;1H\e[2J");
	printf("\nENTER AMOUNT TO BE DEPOSITED : Rs. ");
	scanf("%f",&amt);
	while(amt<=0){
		printf("\nPlease Enter a Valid Amount!!\n");
		printf("ENTER AMOUNT TO BE DEPOSITED : Rs. ");
		scanf("%f",&amt);
	}
	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select info to server which represents deposit operation
	if(writeStatus==-1){
		perror("Error while writing data to the server in deposit");
		_exit(1);
	}
	writeStatus=write(sd,&amt,sizeof(float)); //Sending the amount to be deposited
	if(writeStatus==-1){
		perror("Error while writing data to the server in deposit");
		_exit(1);
	}
	//From the server
	int readStatus=read(sd,&balance,sizeof(balance)); //Receiving Balance amount after depositing
	if(readStatus==-1){
		perror("Error while reading data from the server in deposit");
		_exit(1);
	}
	unlock_critical_section(&semOp,semIdentifier); //Unlocking semaphore 
	if(balance==-1){ //Condition for failing to deposit money
		printf("\n\t\t******************** FAILED!! Error While Depositing Money!!! Check if Account is Active ********************\n\n");
	}
	else{
		printf("\n\t\t\t******************************  !!SUCCESSFULLY DESPOSITED MONEY!!  ******************************\n\n");
		printf("\t\t\tCURRENT ACCOUNT BALANCE : Rs. %d\n",balance);

	}
	printf("\n\nPress ENTER to go back to Menu...\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF) //Clearing keyboard buffer
		;
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to withdraw amount from user account
void withdraw(int sd){
	float amt;
	int select=2;
	bool result;
	int balance;
	struct sembuf semOp1;
	if(!semctl(semIdentifier,0,GETVAL,semSet1)){
		printf("\nLoading ...\n");
	}
	//Locking the section using semaphore as only one instance of an account can withdraw at a time.
    lock_critical_section(&semOp1, semIdentifier1);
    printf("\e[1;1H\e[2J");
	printf("\nENTER AMOUNT TO BE WITHDRAWN : Rs. ");
	scanf("%f",&amt);
	while(amt<=0){
		printf("\nPlease Enter a Valid Amount!!\n");
		printf("\nENTER AMOUNT TO BE WITHDRAWN : Rs. ");
		scanf("%f",&amt);
	}
	//To the Server
	int writeStatus=write(sd,&select,sizeof(int));//Sending select info to server indicating that withdraw operation to be performed
	if(writeStatus==-1){
		perror("Error while writing data to the server in withdraw");
		_exit(1);
	}
	writeStatus=write(sd,&amt,sizeof(float)); //Sending amount to be withdrawn
	if(writeStatus==-1){
		perror("Error while writing data to the server in withdraw");
		_exit(1);
	}
	//From the Server
	int readStatus=read(sd,&balance,sizeof(balance)); //Reading the balance after performing withdraw operation 
	if(readStatus==-1){
		perror("Error while reading data from the server in withdraw");
		_exit(1);
	}
	unlock_critical_section(&semOp1, semIdentifier1); //Unlocking Critical section
	if(balance==-1){ //condition for insufficient balance
		printf("\n\t\t\t******************** FAILED!! Insufficient Balance in your Account ********************\n\n");
	}
	else{
		printf("\n\t\t\t******************************  !!SUCCESSFULLY WITHDRAWN MONEY!!  ******************************\n\n");
		printf("\t\t\tCURRENT ACCOUNT BALANCE : Rs. %d\n",balance);
	}
	printf("\n\nPress ENTER to go back to Menu...\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF) //Clearing keyboard buffer
		;
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to retrieve available balance in user account
void balanceEnquiry(int sd){
	float amt;
	int select=3;
	int len;
	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select info to perform balance enquiry
	if(writeStatus==-1){
		perror("Error while writing data to the server in balanceEnquiry");
		_exit(1);
	}
	//From the server
	len=read(sd,&amt,sizeof(float)); //Reading balance data from the server
	if(len==-1){
		perror("Error while reading data from the server in balanceEnquiry");
		_exit(1);
	}
	printf("\nCURRENT AVAILABLE BALANCE :: Rs. ");
	printf("%0.2f\n\n",amt);
	printf("\n\nPress ENTER to go back to Menu...\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF) //Clearing keyboard buffer
		;
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to change password of user account
void changePassword(int sd){
	int select=5;
	char nPassword[10];
	char oldPassword[10];
	int result;
	char *oldpass=getpass("\nPlease Enter the Old Password(max 10 characters) : ");
	memcpy(oldPassword,oldpass,sizeof(oldpass));
  	char *pass=getpass("\nPlease Enter the New Password(max 10 characters) : ");
  	memcpy(nPassword,pass,sizeof(pass));
	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select info to server to perform Password Change operation
	if(writeStatus==-1){
		perror("Error while sending data to server in changePassword");
		_exit(1);
	}
	writeStatus=write(sd,oldPassword,sizeof(oldPassword));
	if(writeStatus==-1){
		perror("Error while writing data to server");
		_exit(1);
	}
	writeStatus=write(sd,nPassword,sizeof(nPassword)); //Sending the new password
	if(writeStatus==-1){
		perror("Error while sending password data to server in changePassword");
		_exit(1);
	}
	//From the server
	int readStatus=read(sd,&result,sizeof(result)); //Reading result from the server, True if success else False
	if(readStatus==-1){
		perror("Error while reading data from the server in changePassword");
		_exit(1);
	}
	if(result==-1){
		printf("\n\n\t\t\t******************** FAILED!! Incorrect Old Password ********************\n\n");
	}
	else if(result==0){
		printf("\n\t\t******************** FAILED!! Error in Changing the Password/Account Closed ********************\n\n");
	}
	else{
		printf("\n\n\t\t\t**************************  !!PASSWORD CHANGED SUCCESSFULLY!!  **************************\n\n");
	}
	sleep(3);
	showMenu(sd);
	return;
}

//Function to View Transaction Details
void viewDetails(int sd){
	int select=4;
	//To the server
	write(sd,&select,sizeof(int)); //Sending select info to perform view Details operation
	if(option==1){
		normalUser currUser1;
		printf("\e[1;1H\e[2J");
		read(sd,&currUser1,sizeof(normalUser)); //Read the User details of the account
		printf("\t\t\t\t\t\t\t*** PASSBOOK ***\n");
		printf("\t\t\t********************************* USER DETAILS ****************************************\n");
		printf("\n\t\t\t\t\t\tUSER ID \t\t: %d\n",currUser1.userID);
		printf("\t\t\t\t\t\tNAME \t\t\t: %s\n",currUser1.name);
		printf("\t\t\t\t\t\tACCOUNT NUMBER \t\t: %d\n",currUser1.account_no);
		printf("\t\t\t\t\t\tAVAILABLE BALANCE \t: Rs. %0.2f\n",currUser1.balance);
		printf("\t\t\t\t\t\tACCOUNT STATUS \t\t: %s\n\n",currUser1.status);
		char buff[10000];
		int len=read(sd,buff,10000); //Reading transaction details of the account
		if(strcmp(buff,"empty")==0)
		{
		printf("\n\n\t\t\t*************************** YOUR TRASACTIONS ARE EMPTY!!! *****************************\n\n");
		}
		else
		{
		printf("\n\n\t\t\t****************************** TRANSACTION DETIALS ************************************");
		printf("\n\t\t\t***************************************************************************************");
		printf("\n%s",buff);
		printf("\n\t\t\t***************************************************************************************\n\n");
		}
		
	}
	else if(option==2){
		jointUser currUser2;
		printf("\e[1;1H\e[2J");
		read(sd,&currUser2,sizeof(jointUser)); // Reading user details of Joint Account
		printf("\t\t\t\t\t\t\t*** PASSBOOK ***\n");
		printf("\t\t\t********************************* USER DETAILS ****************************************\n");
		printf("\n\t\t\t\t\t\tUSER ID \t\t\t: %d\n",currUser2.userID);
		printf("\t\t\t\t\t\tPRIMARY ACCOUNT HOLDER'S NAME \t: %s\n",currUser2.name1);
		printf("\t\t\t\t\t\tSECONDARY ACCOUNT HOLDER'S NAME : %s\n",currUser2.name2);
		printf("\t\t\t\t\t\tACCOUNT NUMBER \t\t\t: %d\n",currUser2.account_no);
		printf("\t\t\t\t\t\tAVAILABLE BALANCE \t\t: Rs.%0.2f\n",currUser2.balance);
		printf("\t\t\t\t\t\tACCOUNT STATUS \t\t\t: %s\n\n",currUser2.status);
		char buff[10000];
		int len=read(sd,buff,10000); //Reading transaction details of Joint Account
		if(strcmp(buff,"empty")==0)
		{
		printf("\n\n\t\t\t*************************** YOUR TRASACTIONS ARE EMPTY!!! *****************************\n\n");
		}
		else
		{
		printf("\n\n\t\t\t****************************** TRANSACTION DETIALS ************************************");
		printf("\n\t\t\t***************************************************************************************");
		printf("\n%s",buff);
		printf("\n\t\t\t***************************************************************************************\n\n");
		}
	}
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); //Used to clear keyboard buffer
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Administrator Functions
//Function to add new account
void addAccount(int sd){
	int select=1;
	int type;
	int writeStatus;
	//To the Server
	writeStatus=write(sd,&select,sizeof(int)); //Sending select info to the server indicating addAccount operation
	if(writeStatus==-1){
		perror("Error while writing data to server in addAccount");
		_exit(1);
	}
	printf("\e[1;1H\e[2J");
	printf("\n\t\t\t\t\t\t ******* ACCOUNT CREATION MODE *******\n");
	printf("\nENTER TYPE OF ACCOUNT(1: NORMAL-ACCOUNT  2: JOINT-ACCOUNT) : ");
	scanf("%d",&type);
	writeStatus=write(sd,&type,sizeof(int)); //Sending type of the account info to be created
	if(writeStatus==-1){
		perror("Error while writing type data to server in addAccount");
		_exit(1);
	}
	if(type==1){
		normalUser addUser1;
		//Inputing all the required info for creation of Normal Account
		printf("\n\t\t\t\t\t****** ENTER DETAILS FOR NORMAL-ACCOUNT CREATION ******\n");
		printf("\nACCOUNT HOLDER NAME : ");
		scanf(" %[^\n]",addUser1.name);
    	char *pass=getpass("ENTER PASSWORD (max 10 characters) : ");
    	memcpy(addUser1.password,pass,sizeof(pass));
		printf("INITIAL DEPOSIT AMOUNT : Rs. ");
		scanf("%f",&addUser1.balance);
		while(addUser1.balance<0){
			printf("\nENTER VALID AMOUNT!! (can start from 0) Rs. ");
			scanf("%f",&addUser1.balance);
		}
		writeStatus=write(sd,&addUser1,sizeof(normalUser)); //Sending account details to server which needs to be created
		if(writeStatus==-1){
			perror("Error while sending normal user details to server in addAccount");
			_exit(1);
		}
		//From the server
		int readStatus=read(sd,&addUser1,sizeof(normalUser)); //We receive data from server and check whether it is created or not
		if(readStatus==-1){
			perror("Error while reading data from the server in addAccount");
			_exit(1);
		}
		if(strcmp(addUser1.status,"ACTIVE")){ //Condtion to check whether there was error
			printf("\n\n\t\t\t**************************  !!ERROR WHILE CREATING ACCOUNT!!  **************************\n\n");
		}
		else{
			printf("\n\tUSER ID OF NEWLY CREATED ACCOUNT : %d\n",addUser1.userID); //Printing User Id of new account
			printf("\tACCOUNT NO OF NEWLY CREATED ACCOUNT : %d\n",addUser1.account_no); //Printing Account No of new Account
			printf("\n\n\t\t\t**************************  !!SUCCESSFULLY CREATED ACCOUNT!!  **************************\n\n");
		}
	}

	if(type==2){
		jointUser addUser2;
		//Inputing account details of Joint Account
		printf("\n\t\t\t\t\t****** ENTER DETAILS FOR JOINT-ACCOUNT CREATION ******\n");
		printf("\nPRIMARY ACCOUNT HOLDER NAME : ");
		scanf(" %[^\n]",addUser2.name1);
		printf("SECONDARY ACCOUNT HOLDER NAME : ");
		scanf(" %[^\n]",addUser2.name2);
    	char *pass=getpass("ENTER PASSWORD (max 10 characters) : ");
    	memcpy(addUser2.password,pass,sizeof(pass));
		printf("INITIAL DEPOSIT AMOUNT : Rs. ");
		scanf("%f",&addUser2.balance);
		while(addUser2.balance<0){
			printf("\nENTER VALID AMOUNT!! (can start from 0) Rs. ");
			scanf("%f",&addUser2.balance);
		}
		writeStatus=write(sd,&addUser2,sizeof(jointUser)); //Sending account details to the  server
		if(writeStatus==-1){
			perror("Error while sending joint user details to server in addAccount");
			_exit(1);
		}
		//From the server
		int readStatus=read(sd,&addUser2,sizeof(jointUser)); //Reading result from the server
		if(readStatus==-1){
			perror("Error while reading data from the server in addAccount");
			_exit(1);
		}
		if(strcmp(addUser2.status,"ACTIVE")){
			printf("\n\n\t\t\t**************************  !!ERROR WHILE CREATING ACCOUNT!!  **************************\n\n");
		}
		else{
			printf("\n\tUSER ID OF NEWLY CREATED ACCOUNT : %d\n",addUser2.userID); //Printing UserId of new Account
			printf("\tACCOUNT NO OF NEWLY CREATED ACCOUNT : %d\n",addUser2.account_no); //Printing Account No of new Account
			printf("\n\n\t\t\t**************************  !!SUCCESSFULLY CREATED ACCOUNT!!  **************************\n\n");
		}
	}
	// char c;
	// while((c = getchar()) != '\n' && c != EOF); //Used to clear keyboard buffer
	// sleep(3);
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); //Used to clear keyboard buffer
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to Delete Account
void deleteAccount(int sd){
	int select=2;
	int type,accNo;
	int result;
	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending the select info indicating delete operation
	if(writeStatus==-1){
		perror("Error while writing data to the server in deleteAccount");
		_exit(1);
	}

	printf("\nENTER TYPE OF ACCOUNT(1: NORMAL-ACCOUNT  2: JOINT-ACCOUNT) : ");
	scanf("%d",&type);

	writeStatus=write(sd,&type,sizeof(int)); //Sending Type of Account Info
	if(writeStatus==-1){
		perror("Error while writing type data to server in deleteAccount");
		_exit(1);
	}

	printf("ENTER THE ACCOUNT NUMBER TO BE DEACTIVATED : "); //Sending Account Number to be deleted
	scanf("%d",&accNo);
	writeStatus=write(sd,&accNo,sizeof(int));
	if(writeStatus==-1){
		perror("Error while sending userID to server in deleteAccount");
		_exit(1);
	}

	//From the server
	int readStatus=read(sd,&result,sizeof(result)); //Reading result from the server, True if Success else False
	if(readStatus==-1){
		perror("Error while reading data from the server in deleteAccount");
	}
	if(result==-1){
		printf("\n\n\t\t********************* !!ERROR WHILE DELETING ACCOUNT!! Balance in the Account is not Zero *********************\n\n");
	}
	else if(result==0){
		printf("\n\n\t\t********************* !!ERROR WHILE DELETING ACCOUNT!! Check Account Number/Check if already Deactivated *********************\n\n");
	}
	else{
		printf("\n\n\t\t\t**************************  !!SUCCESSFULLY DEACTIVATED ACCOUNT!!  **************************\n\n");
	}
	sleep(1);
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); //Used to clear keyboard buffer
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to modify the account details
void modifyAccount(int sd){
	int select=3;
	int type;
	bool result;

	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select info indicating modify operation to server
	if(writeStatus==-1){
		perror("Error while writing data to the server in modifyAccount");
		_exit(1);
	}

	printf("\nENTER TYPE OF ACCOUNT(1: NORMAL-ACCOUNT  2: JOINT-ACCOUNT) : "); 
	scanf("%d",&type); //Reading Type of account

	writeStatus=write(sd,&type,sizeof(int)); //Sending type of account info to server
	if(writeStatus==-1){
		perror("Error while sending type data to server in modifyAccount");
		_exit(1);
	}

	if(type==1){
		normalUser modUser1;
		//Inputing new details for the account
		printf("\n\t\t\t\t\t*** ENTER DETAILS OF NORMAL-ACCOUNT TO BE MODIFIED ***\n");
		printf("\nUSER ID : ");
		scanf("%d",&modUser1.userID);
		printf("ACCOUNT NUMBER : ");
		scanf("%d",&modUser1.account_no);
		printf("NEW NAME OF ACCOUNT HOLDER : ");
		scanf(" %[^\n]",modUser1.name);
    	char *pass=getpass("ENTER NEW PASSWORD (max 10 characters) : ");
    	memcpy(modUser1.password,pass,sizeof(pass));
		printf("NEW BALANCE : ");
		scanf("%f",&modUser1.balance);
		writeStatus=write(sd,&modUser1,sizeof(normalUser)); //Sending the modified details to server
		if(writeStatus==-1){
			perror("Error while sending user info to server in modifyAccount");
			_exit(1);
		}
	}

	if(type==2){
		jointUser modUser2;
		//Inputing new details for Joint Account
		printf("\n\t\t\t\t\t *** ENTER DETAILS OF JOINT-ACCOUNT TO BE MODIFIED ***\n");
		printf("\nUSER ID : ");
		scanf("%d",&modUser2.userID);
		printf("ACCOUNT NUMBER : ");
		scanf("%d",&modUser2.account_no);
		printf("NEW NAME OF PRIMARY ACCOUNT HOLDER : ");
		scanf(" %[^\n]",modUser2.name1);
		printf("NEW NAME OF SECONDARY ACCOUNT HOLDER : ");
		scanf(" %[^\n]",modUser2.name2);
		
    	char *pass=getpass("ENTER NEW PASSWORD (max 10 characters) : ");
    	memcpy(modUser2.password,pass,sizeof(pass));
		printf("NEW BALANCE : ");
		scanf("%f",&modUser2.balance);
		writeStatus=write(sd,&modUser2,sizeof(jointUser)); //Sending the modified details to server
		if(writeStatus==-1){
			perror("Error while sending joint user info to server in modifyAccount");
			_exit(1);
		}
	}

	//From the server
	int readStatus=read(sd,&result,sizeof(result)); //Reading info from server, True if success else Flase
	if(readStatus==-1){
		perror("Error while reading data from server in modifyAccount");
		_exit(1);
	}

	if(!result){
			printf("\n\n\t\t********************* !!ERROR WHILE MODIFYING ACCOUNT!! Check Account Number and User ID *********************\n\n");
	}
	else{
			printf("\n\n\t\t**************************  !!SUCCESSFULLY MODIFIED ACCOUNT DETAILS!!  **************************\n\n");
	}
	sleep(1);
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); //Used to clear keyboard buffer
	scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to search for an account using User Id
void searchAccount(int sd){
	int select=4;
	int type,len;
	bool result;

	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select info indicating search Account operation
	if(writeStatus==-1){
		perror("Error while sending data to server in searchAccount");
		_exit(1);
	}
	printf("\e[1;1H\e[2J");
	printf("\nENTER TYPE OF ACCOUNT(1: NORMAL-ACCOUNT  2: JOINT-ACCOUNT) : ");
	scanf("%d",&type);

	writeStatus=write(sd,&type,sizeof(int)); //Sending the type of account information
	if(writeStatus==-1){
		perror("Error while sending type data to server in searchAccount");
		_exit(1);
	}

	if(type==1){
		normalUser searchUser1;
		int userID;
		printf("\nENTER USER ID OF ACCOUNT TO BE SEARCHED : ");
		scanf("%d%*c",&userID);
		writeStatus=write(sd,&userID,sizeof(userID)); //Sending User Id to be searched
		if(writeStatus==-1){
			perror("Error while sending userInfo to server in searchAccount");
			_exit(1);
		}

		//From the server
		len=read(sd,&searchUser1,sizeof(normalUser));
		if(len==-1){
			perror("Error while reading userInfo from server in searchAccount");
			_exit(1);
		}
		else if(len==0 || searchUser1.userID==-1){
			printf("\n\t\t\t\t\tUSER ID NOT FOUNT!!!\tPLEASE VERIFY THE USER ID!!\n\n");
		}
		else{
			printf("\n\n\t\t\t**************************  !!NORMAL-ACCOUNT DETAILS!!  **************************\n\n");
			printf("\nUSER ID \t\t: %d\n",searchUser1.userID);
			printf("ACCOUNT HOLDER NAME \t: %s\n",searchUser1.name);
			printf("ACCOUNT NUMBER \t\t: %d\n",searchUser1.account_no);
			printf("AVAILABLE BALANCE \t: Rs. %0.2f\n",searchUser1.balance);
			printf("ACCOUNT STATUS \t\t: %s\n\n",searchUser1.status);
		}
	}
	else if(type==2){
		jointUser searchUser2;
		int userID1;
		printf("\nENTER USER ID OF ACCOUNT TO BE SEARCHED : ");
		scanf("%d%*c",&userID1);
		writeStatus=write(sd,&userID1,sizeof(userID1)); //Sending User Id to be searched
		if(writeStatus==-1){
			perror("Error while sending userInfo to server in searchAccount");
			_exit(1);
		}

		//From the server
		len=read(sd,&searchUser2,sizeof(jointUser)); //reading the data from the server
		if(len==-1){
			perror("Error while reading userInfo from server in searchAccount");
			_exit(1);
		}
		else if(len==0 || searchUser2.userID==-1){
			printf("\n\t\t\t\t\tUSER ID NOT FOUNT!!!\tPLEASE VERIFY THE USER ID!!\n\n");
		}
		else{
			printf("\n\n\t\t\t**************************  !!JOINT-ACCOUNT DETAILS!!  **************************\n\n");
			printf("\nUSER ID \t\t\t\t: %d\n",searchUser2.userID);
			printf("PRIMARY ACCOUNT HOLDER'S NAME \t\t: %s\n",searchUser2.name1);
			printf("SECONDARY ACCOUNT HOLDER'S NAME \t: %s\n",searchUser2.name2);
			printf("ACCOUNT NUMBER \t\t\t\t: %d\n",searchUser2.account_no);
			printf("AVAILABLE BALANCE \t\t\t: Rs. %0.2f\n",searchUser2.balance);
			printf("ACCOUNT STATUS \t\t\t\t: %s\n\n",searchUser2.status);
		}
	}
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); //Clearing keyboard buffer
	//scanf("%c",&c);
	showMenu(sd);
	return;
}

//Function to create new Admin Account
void create_admin(int sd){
	admin newAdmin;
	printf("\e[1;1H\e[2J");
	printf("\n\t\t\t\t\t\t\t ******* ADMINISTRATOR ACCOUNT CREATION MODE *******\n");
	printf("\nENTER THE NAME OF ADMIN : ");
	scanf(" %[^\n]",newAdmin.username);
	char *pass=getpass("ENTER THE PASSWORD(max 10 characters) : ");
	memcpy(newAdmin.password,pass,sizeof(pass));
	int select=5;

	//To the server
	int writeStatus=write(sd,&select,sizeof(int)); //Sending select Info indicating to create admin account
	int type=0;
	writeStatus=write(sd,&type,sizeof(int));
	writeStatus=write(sd,&newAdmin,sizeof(newAdmin)); //Sending new admin details to server
	if(writeStatus==-1){
		perror("Error while writing data to the record");
		exit(0);
	}
	int res;

	//From the server
	int r=read(sd,&res,sizeof(res)); //Reading data from the server
	if(r==-1){
		perror("Error");
		exit(0);
	}
	printf("\n\nUSER ID OF NEWLY CREATED ACCOUNT : %d\n",res); //Priniting User Id of new admin
	printf("\n\nPress ENTER to go Back to Menu\n");
	char c;
	while((c = getchar()) != '\n' && c != EOF); // Used to clear keyboard buffer
	scanf("%c",&c);
	showMenu(sd);
}