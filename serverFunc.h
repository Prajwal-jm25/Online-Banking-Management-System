//Function to check for input whether not 0 or 0
bool check(int j){
	if(j!=0){
		return true;
	}
	else{
		return false;
	}
}

//Function to update Passbook by adding transaction details
void updatepassbook(normalUser currUser,char* s,float amount){
	char buf[100],path[100];
	sprintf(path,"passbook/NormalUser/%d",currUser.userID); //Accessing the file of user
	time_t t;
	time(&t);
	passbook transaction;
	//strcpy(transaction.type,s);
	memcpy(transaction.type,s,sizeof(s));
	//transaction.type="Deposit";
	transaction.amount=amount;
	transaction.balance=currUser.balance;
	transaction.time=t;
	//printf("%s\n",ctime(&transaction.time));
	int fd=open(path,O_CREAT|O_APPEND|O_RDWR,0744);
	//write(fd,&currUser,sizeof(normalUser));
	lseek(fd,0,SEEK_END);
	write(fd,&transaction,sizeof(passbook));
	close(fd);
}

//Function to update Passbook by adding transaction details to Joint Account User
void updatepassbookJ(jointUser currUser,char* s,float amount){
	char buf[100],path[100];
	sprintf(path,"passbook/JointUser/%d",currUser.userID); //Accessing the file of the Joint Account User
	time_t t;
	time(&t);
	passbook transaction;
	//strcpy(transaction.type,s);
	memcpy(transaction.type,s,sizeof(s));
	//transaction.type="Deposit";
	transaction.amount=amount;
	transaction.balance=currUser.balance;
	transaction.time=t;
	//printf("%s\n",ctime(&transaction.time));
	int fd=open(path,O_CREAT|O_APPEND|O_RDWR,0744);
	//write(fd,&currUser,sizeof(normalUser));
	lseek(fd,0,SEEK_END);
	write(fd,&transaction,sizeof(passbook));
	close(fd);
}

//Function to get Normal User Account Details
normalUser getNormalUser(int ID){
	int i=ID-1001; //Subtracting so it will be easier to locate the record in the file
	normalUser currUser2;
	if(i<0){ //Indicated Invalid User Id
		printf("Invalid UserID\n");
		currUser2.userID=-1; //Invalid Condition
		return currUser2;	
	}
	int fd=open("Normal_user",O_RDONLY,0744); //Opening the file which stores user info
	if(fd==-1){
		printf("Invalid userID\n");
		currUser2.userID=-1;
	}
	else{
		int fl1;
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(normalUser); //Locking only that users record
		lock.l_len=sizeof(normalUser); //Length of each record is sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //Executing lock, locks the selected record
		int offset=lseek(fd,-sizeof(normalUser),SEEK_END); //Getting the offset of last user record
		if(errno==EINVAL){
			printf("Invalid userID\n");
			currUser2.userID=-1;
		}
		else{
			read(fd,&currUser2,sizeof(normalUser)); //Reading info of last user in the record
			if(currUser2.userID<(i+1001)){ //If user Id is greater than the last user then it doesn't exist as we store the info in sorting order.
				printf("Invalid userID\n");
				currUser2.userID=-1; //Setting Invalid condition
			}
			else{
				i=currUser2.userID-ID+1;
				lseek(fd,-(i)*sizeof(normalUser),SEEK_END); //Getting offser of the User Ids record 
				read(fd,&currUser2,sizeof(normalUser)); //reading info of the user Id provided
			}
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
	}
	close(fd);
	return currUser2;
}

//Function to get info of Joint User 
jointUser getJointUser(int ID){
	int i=ID-1001; //Subtracting so it will be easier to locate the record in the file
	jointUser currUser;
	if(i<0){ //Indicated Invalid User Id
		printf("Invalid UserID\n");
		currUser.userID=-1;
		return currUser;	
	}
	int fd=open("Joint_user",O_RDONLY,0744); //Opening Joint Account User file
	if(fd==-1){
		printf("Invalid userID\n");
		currUser.userID=-1;
	}
	else{
		int fl1;
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(jointUser); //Locking only that users record
		lock.l_len=sizeof(jointUser); //Length of each record is sizeof(record)
		lock.l_pid=getpid();

		fl1=fcntl(fd,F_SETLKW,&lock); //Executing lock, locks the selected record

		int offset=lseek(fd,(i)*sizeof(jointUser),SEEK_SET);  //changing the file pointer to the selected record
		if(errno==EINVAL){
			printf("Invalid UserID\n");
			currUser.userID=-1; //Setting Invalid Condition
		}
		else{
			read(fd,&currUser,sizeof(jointUser));
			if(currUser.userID<(i+1001)){
				printf("Invalid UserID\n");
				currUser.userID=-1; //Setting Invalid Condition
			}
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
	}
	close(fd);
	return currUser;
}

//Function to get Info of a Admin 
admin getAdmin(int ID){
	int i=ID-1001; //Subtracting so it will be easier to locate the record in the file
	admin currUser;
	int fd=open("Admin",O_RDONLY,0744);
	int fl1;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(admin); //Locking only that users record
	lock.l_len=sizeof(admin); //Length of each record is sizeof(record)
	lock.l_pid=getpid();

	fl1=fcntl(fd,F_SETLKW,&lock);	//lock the selected record

	lseek(fd,(i)*sizeof(admin),SEEK_SET);  //changing the file pointer to the selected record
	read(fd,&currUser,sizeof(admin));

	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);

	close(fd);
	return currUser;
}

//Function to deposit money to a account
int depositMoney(int accType,int ID,float amt){
	int amount=amt;
	if(amount<=0){
		return -1;
	}
	int i=ID-1001;
	int balance=-1;
	if(accType==1){
		int fd=open("Normal_user",O_RDWR,0744);
		bool result;
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(normalUser); //nth record
		lock.l_len=sizeof(normalUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		normalUser currUser;
		lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(normalUser));
		if(!strcmp(currUser.status,"ACTIVE")){
			currUser.balance+=amt;
			lseek(fd,sizeof(normalUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(normalUser));
			balance=currUser.balance;
			normalUser newU=currUser;
			char* s="Deposit";
			updatepassbook(newU,s,amt);
		}
		else{	
			balance=-1;
		}	
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		printf("Balance: %d\n",balance);
		return balance;
	}
	else if(accType==2){
		int fd=open("Joint_user",O_RDWR,0744);
		bool result;
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(jointUser); //nth record
		lock.l_len=sizeof(jointUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		jointUser currUser;
		lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(jointUser));
		if(!strcmp(currUser.status,"ACTIVE")){
			currUser.balance+=amt;
			lseek(fd,sizeof(jointUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(jointUser));
			result=true;
			balance=currUser.balance;
			char* s="Deposit";
			updatepassbookJ(currUser,s,amt);
		}
		else{
			balance=-1;
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return balance;
	}
	return -1;
}

//Function to Withdraw money from a account
int withdrawMoney(int accType,int ID,float amt){
	int amount=amt;
	if(amount<=0){
		return -1;
	}
	int i=ID-1001;
	int balance=-1;
	if(accType==1){
		int fd=open("Normal_user",O_RDWR,0744);
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(normalUser); //nth record
		lock.l_len=sizeof(normalUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		normalUser currUser;
		lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(normalUser));
		if(!strcmp(currUser.status,"ACTIVE") && currUser.balance>=amt){ //Condition to check whether there is sufficient balance
			currUser.balance-=amt;
			lseek(fd,sizeof(normalUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(normalUser));
			balance=currUser.balance;
			char* s="Withdraw";
			updatepassbook(currUser,s,amt);
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return balance;
	}
	else if(accType==2){
		int fd=open("Joint_user",O_RDWR,0744);
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(jointUser); //nth record
		lock.l_len=sizeof(jointUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		jointUser currUser;
		lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(jointUser));
		if(!strcmp(currUser.status,"ACTIVE") && currUser.balance>=amt){ //Condition to check for sufficient balance
			currUser.balance-=amt;
			lseek(fd,sizeof(jointUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(jointUser));
			balance=currUser.balance;
			char* s="Withdraw";
			updatepassbookJ(currUser,s,amt);
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return balance;
	}
	return -1;
}

//Function to getBalance details of an account 
float getBalance(int accType,int ID){
	int i=ID-1001;
	float result;
	if(accType==1){
		int i=ID-1001;
		int fd=open("Normal_user",O_RDONLY,0744);
		normalUser temp;
		int fl1;
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(normalUser); //nth record
		lock.l_len=sizeof(normalUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&temp,sizeof(normalUser));
		if(!strcmp(temp.status,"ACTIVE")){	
			result=temp.balance;
		}
		else{
			result=0;
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	else if(accType==2){
		int i=ID-1001;
		int fd=open("Joint_user",O_RDONLY,0744);
		jointUser temp;
		int fl1;
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(jointUser); //nth record
		lock.l_len=sizeof(jointUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&temp,sizeof(jointUser));
		if(!strcmp(temp.status,"ACTIVE")){
			result=temp.balance;
		}
		else{					
			result=0;
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	return 0;
}

//Function to change Password
int alterPassword(int accType,int ID,char newPwd[10],char oldPwd[10]){
	int i=ID-1001;
	int result;
	if(accType==1){
		int fd=open("Normal_user",O_RDWR,0744);
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(normalUser); //nth record
		lock.l_len=sizeof(normalUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		normalUser currUser;
		lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(normalUser)); //Reading User Info
		if(strcmp(currUser.password,oldPwd)){
			result=-1;
		}
		else if(!strcmp(currUser.status,"ACTIVE")){ //Checks whether the user is active
			strcpy(currUser.password,newPwd); //Changing Password of that user
			lseek(fd,sizeof(normalUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(normalUser));
			result=1;
		}
		else{	
			result=0;
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	else if(accType==2){
		int fd=open("Joint_user",O_RDWR,0744);
		int result;
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_SET;
		lock.l_start=(i)*sizeof(jointUser); //nth record
		lock.l_len=sizeof(jointUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		jointUser currUser;
		lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
		read(fd,&currUser,sizeof(jointUser)); //Reading User Info
		if(strcmp(currUser.password,oldPwd)){
			result=-1;
		}
		else if(!strcmp(currUser.status,"ACTIVE")){ //Checks whether the user is active
			strcpy(currUser.password,newPwd); //Changing the password of that user
			lseek(fd,sizeof(jointUser)*(-1),SEEK_CUR);
			write(fd,&currUser,sizeof(jointUser));
			result=1;
		}
		else{
			result=0;
		}
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	return false;
}

//Function to add normal user 
normalUser addNormalUser(normalUser record){
	int fd=open("Normal_user",O_RDWR|O_CREAT,0744);
	//bool result;
	int size=lseek(fd,0,SEEK_END);
	if(size==0){
		record.userID=1001;
		record.account_no=100001;
		strcpy(record.status,"ACTIVE");
		write(fd,&record,sizeof(normalUser));
	}
	else{
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_END;
		lock.l_start=(-1)*sizeof(normalUser); //nth record
		lock.l_len=sizeof(normalUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		normalUser endUser;
		lseek(fd,(-1)*sizeof(normalUser),SEEK_END); //changing the file pointer to the selected record
		read(fd,&endUser,sizeof(normalUser));
		record.userID=endUser.userID+1;
		record.account_no=endUser.account_no+1;
		strcpy(record.status,"ACTIVE");
		int j=write(fd,&record,sizeof(normalUser));
		//result=check(j);
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
	}	
	close(fd);
	return record;
}

//Function to add Joint User record
jointUser addJointUser(jointUser record){
	int fd=open("Joint_user",O_RDWR|O_CREAT,0744);
	int size=lseek(fd,0,SEEK_END);
	if(size==0){
		record.userID=1001;
		record.account_no=100001;
		strcpy(record.status,"ACTIVE");
		write(fd,&record,sizeof(jointUser));
	}
	else{
		int fl1;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence=SEEK_END;
		lock.l_start=(-1)*sizeof(jointUser); //nth record
		lock.l_len=sizeof(jointUser); //sizeof(record)
		lock.l_pid=getpid();
		fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
		jointUser endUser;
		lseek(fd,(-1)*sizeof(jointUser),SEEK_END); //changing the file pointer to the selected record
		read(fd,&endUser,sizeof(jointUser));
		record.userID=endUser.userID+1;
		record.account_no=endUser.account_no+1;
		strcpy(record.status,"ACTIVE");
		int j=write(fd,&record,sizeof(jointUser));
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
	}
	close(fd);
	return record;
}

//Function to delete Normal Account
int deleteNormalAcc(int ID){
	int i=ID-100001;
	int result;
	if(i<0){
		return 0;
	}
	int fd=open("Normal_user",O_RDWR,0744);
	int fl1;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(normalUser); //nth record
	lock.l_len=sizeof(normalUser); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
	normalUser currUser;
	lseek(fd,-sizeof(normalUser),SEEK_END); //changing the file pointer to the last record
	read(fd,&currUser,sizeof(normalUser));
	if(currUser.account_no<ID){
		result=0;
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		fflush(stdout);
		fflush(stdin);
		close(fd);
		return result;
	}
	lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&currUser,sizeof(normalUser));
	if(currUser.balance>0){
		result=-1;
	}
	else if(!strcmp(currUser.status,"ACTIVE")){
		strcpy(currUser.status,"CLOSED");
		currUser.balance=0;
		lseek(fd,(-1)*sizeof(normalUser),SEEK_CUR);
		int j=write(fd,&currUser,sizeof(normalUser));
		result=check(j);
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	fflush(stdout);
	fflush(stdin);
	close(fd);
	return result;
}

//Function to delete Joint Account 
int deleteJointAcc(int ID){
	int i=ID-100001;
	int result;
	if(i<0){
		return 0;
	}
	int fd=open("Joint_user",O_RDWR,0744);
	int fl1;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(jointUser); //nth record
	lock.l_len=sizeof(jointUser); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
	jointUser currUser;
	lseek(fd,-sizeof(jointUser),SEEK_END); //changing the file pointer to the last record
	read(fd,&currUser,sizeof(jointUser));
	if(currUser.account_no<ID){
		result=0;
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&currUser,sizeof(jointUser));
	if(currUser.balance>0){
		result=-1;
	}
	else if(!strcmp(currUser.status,"ACTIVE")){
		strcpy(currUser.status,"CLOSED");
		currUser.balance=0;
		lseek(fd,(-1)*sizeof(jointUser),SEEK_CUR);
		int j=write(fd,&currUser,sizeof(jointUser));
		result=check(j);
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	return result;
}

//Function to modify the details of user Account
bool modifyNormalUser(normalUser modUser){
	int i=modUser.userID-1001;
	if(i<0){
		return false;
	}
	int fd=open("Normal_user",O_RDWR,0744);
	bool result=false;
	int fl1;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(normalUser); //nth record
	lock.l_len=sizeof(normalUser); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
	normalUser currUser;
	lseek(fd,-sizeof(normalUser),SEEK_END);
	read(fd,&currUser,sizeof(normalUser));
	if(currUser.userID<i){
		result=false;
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&currUser,sizeof(normalUser));
	if(!strcmp(currUser.status,"ACTIVE") && (modUser.account_no==currUser.account_no)){
		strcpy(modUser.status,"ACTIVE");
		lseek(fd,(-1)*sizeof(normalUser),SEEK_CUR);
		int j=write(fd,&modUser,sizeof(normalUser));
		result=check(j);
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	return result;
}

//Function to modify the details of Joint Account User
bool modifyJointUser(jointUser modUser){
	int i=modUser.userID-1001;
	if(i<0){
		return false;
	}
	int fd=open("Joint_user",O_RDWR,0744);
	bool result=false;
	int fl1;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(jointUser); //nth record
	lock.l_len=sizeof(jointUser); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock); //lock the selected record
	jointUser currUser;
	lseek(fd,-sizeof(jointUser),SEEK_END);
	read(fd,&currUser,sizeof(jointUser));
	if(currUser.userID<i){
		result=false;
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return result;
	}
	lseek(fd,(i)*sizeof(jointUser),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&currUser,sizeof(jointUser));
	if(!strcmp(currUser.status,"ACTIVE")  && (modUser.account_no==currUser.account_no)){
		strcpy(modUser.status,"ACTIVE");
		lseek(fd,(-1)*sizeof(jointUser),SEEK_CUR);
		int j=write(fd,&modUser,sizeof(jointUser));
		result=check(j);
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	return result;
}

#define mode 0744
#define flags O_RDWR | O_CREAT

//Function to get New User ID
int getNewUserID(char* user){
	int fd=open(user,flags,mode);
	int readStatus;
	if(!strcmp(user,"Admin")){
		admin record;
		lseek(fd,-sizeof(admin),SEEK_END);
		readStatus=read(fd,&record,sizeof(admin));
		if(readStatus==-1){
			perror("Error while reading from the record");
			_exit(1);
		}
		close(fd);
		return record.userID+1;
	}
}

//Function to add new Admin Account
int addAdmin(admin newAdmin){
	int fd=open("Admin",O_RDWR,mode);
	if(fd==-1){
		perror("Error while opening file");
		exit(0);
	}
	int size=lseek(fd,0,SEEK_END);
	if(size==0){
		newAdmin.userID=1001;
	}
	else{
		newAdmin.userID=getNewUserID("Admin");
	}
	int writeStatus=write(fd,&newAdmin,sizeof(admin));
	if(writeStatus==-1){
		perror("Error while adding new admin");
		_exit(1);
	}
	close(fd);
	return newAdmin.userID;
}