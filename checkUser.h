//Checks Whether the User exists and is active
bool checkNormalUser(normalUser currUser){
	int i=currUser.userID-1001;
	if(i<0){
		return false;
	}
	int fd=open("Normal_user",O_RDONLY,0744);
	bool result;
	normalUser temp;
	int fl1;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(normalUser); //nth record
	lock.l_len=sizeof(normalUser); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock);
	lseek(fd,-1*sizeof(normalUser),SEEK_END);
	read(fd,&temp,sizeof(normalUser));
	if((temp.userID-1001)<i){
		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
		close(fd);
		return false;
	}
	lseek(fd,(i)*sizeof(normalUser),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&temp,sizeof(normalUser));
	if(!strcmp(temp.password,currUser.password) && !strcmp(temp.status,"ACTIVE")){
		result=true;
	}
	else{
		result=false;
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	return result;
}

bool checkJointUser(jointUser currUser){
	int i=currUser.userID-1001;
	if(i<0){
		return false;
	}
	int fd=open("Joint_user",O_RDONLY,0744);
	bool result;
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
	if(!strcmp(temp.password,currUser.password) && !strcmp(temp.status,"ACTIVE")){	
		result=true;
	}
	else{
		result=false;
	}

	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);

	close(fd);
	return result;
}

bool checkAdmin(admin currUser){
	int i=currUser.userID-1001;
	if(i<0){
		return false;
	}
	int fd=open("Admin",O_RDONLY,0744);
	bool result;
	admin temp;
	int fl1;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(i)*sizeof(admin); //nth record
	lock.l_len=sizeof(admin); //sizeof(record)
	lock.l_pid=getpid();
	fl1=fcntl(fd,F_SETLKW,&lock);
	lseek(fd,(i)*sizeof(admin),SEEK_SET); //changing the file pointer to the selected record
	read(fd,&temp,sizeof(admin));
	if(!strcmp(temp.password,currUser.password)){
		result=true;
	}
	else{
		result=false;
	}
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	return result;
}
