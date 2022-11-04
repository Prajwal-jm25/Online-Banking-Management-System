#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>

#include "structures.h"
#include "serverFunc.h"
#include "checkUser.h"

//All options, type of account and operation to be performed are selected in this function. It receives and send info to client. 
void serverTask(int nsd){
	int msgLength,select,type,option,accType,currUserID;
	bool result;
	while(1){
		//Reads option from client 1-Normal,2-Joint,3-admin after they login
		read(nsd,&option,sizeof(option));
		printf("Option : %d\n",option);
		//Reads the info of the user who logged in
		if(option==1){
			normalUser currUser1;
			accType=1;
			msgLength=read(nsd,&currUser1,sizeof(normalUser));
			result=checkNormalUser(currUser1);
			if(result){
				printf("Username : %d is active now\n",currUser1.userID);
				currUserID=currUser1.userID;
			}
			write(nsd,&result,sizeof(result));
		}
		else if(option==2){
			jointUser currUser2;
			accType=2;
			msgLength=read(nsd,&currUser2,sizeof(jointUser));
			result=checkJointUser(currUser2);
			if(result){
				currUserID=currUser2.userID;
				printf("Username : %d is active now\n",currUser2.userID);
			}
			write(nsd,&result,sizeof(result));
		}
		else if(option==3){
			admin currUser3;
			accType=3;
			msgLength=read(nsd,&currUser3,sizeof(admin));
			result=checkAdmin(currUser3);
			if(result){
				currUserID=currUser3.userID;
				printf("Username : %d is active now\n",currUser3.userID);
			}
			write(nsd,&result,sizeof(result));
		}
		else{
			result=false;
			write(nsd,&result,sizeof(result));
		}
		if(result)	break;
	}

	while(1){
		//Operations to be performed are interacted 
		read(nsd,&select,sizeof(int));
		if(option==1 || option==2){
			if(select==1){
				float amt;
				read(nsd,&amt,sizeof(float));
				printf("auisd : %d\n",currUserID);
				int res=depositMoney(accType,currUserID,amt);
				write(nsd,&res,sizeof(res));
			}
			else if(select==2){
				float amt;
				read(nsd,&amt,sizeof(float));
				int res=withdrawMoney(accType,currUserID,amt);
				write(nsd,&res,sizeof(res));
			}
			else if(select==3){
				float amt;
				amt=getBalance(accType,currUserID);
				write(nsd,&amt,sizeof(float));
			}
			else if(select==4){
				if(option==1){
					normalUser user1=getNormalUser(currUserID);
					write(nsd,&user1,sizeof(normalUser));
					char path[100],buf[10000];
					sprintf(path,"passbook/NormalUser/%d",user1.userID);
					int fd=open(path,O_CREAT|O_RDONLY,0744);
					FILE* fp = fopen(path, "r");
    				// checking if the file exist or not
    				if (fp == NULL) {
        				printf("File Not Found!\n");
    				} 
    				fseek(fp, 0L, SEEK_END);
    				// calculating the size of the file
    				long int res = ftell(fp);
    				// closing the file
    				fclose(fp);
 				   	res=res/sizeof(passbook);
					passbook book;
					if(res==0){
						book.amount=-1;
						book.balance=-1;
						//lseek(fd,0,SEEK_SET);
						write(nsd,&book,sizeof(book));
					}
					else if(res<10){
						off_t offset=lseek(fd,-1*sizeof(passbook),SEEK_END);
						while(res>0){
							if(offset<0){
								printf("Error Executed\n");
								book.amount=-1;
								book.balance=-1;
								write(nsd,&book,sizeof(book));
								break;
							}
							else if(offset>=0){
								int len=read(fd,&book,sizeof(book));
								if(len==-1||len==0){
									book.amount=-1;
									write(nsd,&book,sizeof(book));
									break;
								}
								else{
									write(nsd,&book,sizeof(book));
								}
							}
							res--;
							if(res>0){
								offset=lseek(fd,-2*sizeof(passbook),SEEK_CUR);
							}
						}
						book.amount=-1;
						book.balance=-1;
						write(nsd,&book,sizeof(book));
					}
					else{
						int i=0;
						off_t offset=lseek(fd,-1*sizeof(passbook),SEEK_END);
						while(i<10){
							if(offset>=0){
								int len=read(fd,&book,sizeof(book));
								if(len==-1||len==0){
									book.amount=-1;
									write(nsd,&book,sizeof(book));
									break;
								}
								else{
									write(nsd,&book,sizeof(book));
								}
							}
							else{
								book.amount=-1;
								write(nsd,&book,sizeof(book));
								break;
							}
							if(i<9){
								offset=lseek(fd,-2*sizeof(passbook),SEEK_CUR);
							}
							i++;
						}
					}
					close(fd);
				}
				else if(option==2){
					jointUser user2=getJointUser(currUserID);
					write(nsd,&user2,sizeof(jointUser));
					char path[100],buf[10000];
					sprintf(path,"passbook/JointUser/%d",user2.userID);
					int fd=open(path,O_CREAT|O_RDONLY,0744);
					FILE* fp = fopen(path, "r");
    				// checking if the file exist or not
    				if(fp == NULL) {
        				printf("File Not Found!\n");
    				} 
    				fseek(fp, 0L, SEEK_END);
    				// calculating the size of the file
    				long int res = ftell(fp);
    				// closing the file
    				fclose(fp);
 				   	res=res/sizeof(passbook);
					passbook book;
					if(res==0){
						book.amount=-1;
						book.balance=-1;
						//lseek(fd,0,SEEK_SET);
						write(nsd,&book,sizeof(book));
					}
					else if(res<10){
						off_t offset=lseek(fd,-1*sizeof(passbook),SEEK_END);
						while(res>0){
							if(offset<0){
								printf("Error Executed\n");
								book.amount=-1;
								book.balance=-1;
								write(nsd,&book,sizeof(book));
								break;
							}
							else if(offset>=0){
								int len=read(fd,&book,sizeof(book));
								if(len==-1||len==0){
									book.amount=-1;
									write(nsd,&book,sizeof(book));
									break;
								}
								else{
									write(nsd,&book,sizeof(book));
								}
							}
							res--;
							if(res>0){
								offset=lseek(fd,-2*sizeof(passbook),SEEK_CUR);
							}
						}
						book.amount=-1;
						book.balance=-1;
						write(nsd,&book,sizeof(book));
					}
					else{
						int i=0;
						off_t offset=lseek(fd,-1*sizeof(passbook),SEEK_END);
						while(i<10){
							if(offset>=0){
								int len=read(fd,&book,sizeof(book));
								if(len==-1||len==0){
									book.amount=-1;
									write(nsd,&book,sizeof(book));
									break;
								}
								else{
									write(nsd,&book,sizeof(book));
								}
							}
							else{
								book.amount=-1;
								write(nsd,&book,sizeof(book));
								break;
							}
							if(i<9){
								offset=lseek(fd,-2*sizeof(passbook),SEEK_CUR);
							}
							i++;
						}
					}
					close(fd);
				}

			}
			else if(select==5){
				char npwd[10];
				char opwd[10];
				read(nsd,opwd,sizeof(opwd));
				read(nsd,npwd,sizeof(npwd));
				int res;
				res=alterPassword(accType,currUserID,npwd,opwd);
				write(nsd,&res,sizeof(res));
			}
			else if(select==6)	break;
		}
		else if(option==3){
			read(nsd,&type,sizeof(int));
			if(select==1){
				if(type==1){
					normalUser newUser1;
					read(nsd,&newUser1,sizeof(normalUser));
					newUser1=addNormalUser(newUser1);
					write(nsd,&newUser1,sizeof(normalUser));
				}
				else if(type==2){
					jointUser newUser2;
					read(nsd,&newUser2,sizeof(jointUser));
					newUser2=addJointUser(newUser2);
					write(nsd,&newUser2,sizeof(jointUser));
				}
			}
			else if(select==2){
				if(type==1){
					int delAccID1;
					read(nsd,&delAccID1,sizeof(int));
					int res=deleteNormalAcc(delAccID1);
					write(nsd,&res,sizeof(res));
				}
				else if(type==2){
					int delAccID2;
					read(nsd,&delAccID2,sizeof(int));
					int res=deleteJointAcc(delAccID2);
					write(nsd,&res,sizeof(res));
				}
			}
			else if(select==3){
				if(type==1){
					normalUser modUser1;
					read(nsd,&modUser1,sizeof(normalUser));
					result=modifyNormalUser(modUser1);
					write(nsd,&result,sizeof(result));
				}
				else if(type==2){
					jointUser modUser2;
					read(nsd,&modUser2,sizeof(jointUser));
					result=modifyJointUser(modUser2);
					write(nsd,&result,sizeof(result));
				}
			}
			else if(select==4){
				if(type==1){
					normalUser searchUser1;
					int userID1;
					read(nsd,&userID1,sizeof(int));
					searchUser1=getNormalUser(userID1);
					write(nsd,&searchUser1,sizeof(normalUser));
				}
				else if(type==2){
					jointUser searchUser2;
					int userID2;
					read(nsd,&userID2,sizeof(int));
					searchUser2=getJointUser(userID2);
					write(nsd,&searchUser2,sizeof(jointUser));
				}
			}
			else if(select==5){
				admin admin1;
				int rs=read(nsd,&admin1,sizeof(admin));
				if(rs==-1){
					perror("read failed");
					exit(0);
				}
				int adminId=addAdmin(admin1);
				write(nsd,&adminId,sizeof(adminId));
			}
			else if(select==6){
				break;
			}
		}
	}
	close(nsd);
	write(1,"Ended client session.....\n",sizeof("Ended client session.....\n"));
	return;
}

//Creates a Admin Account if none exists
admin initAdmin(){
	admin newAdmin;
	int fd=open("Admin",O_RDWR|O_CREAT,mode);
	int size=lseek(fd,0,SEEK_END);
	if(size==0){
		newAdmin.userID=1001;
		strcpy(newAdmin.username,"admin");
		strcpy(newAdmin.password,"admin");
	}
	else{
		newAdmin.userID=-1;
		close(fd);
		return newAdmin;
	}
	int writeStatus=write(fd,&newAdmin,sizeof(admin));
	if(writeStatus==-1){
		perror("Error while adding new admin");
		_exit(1);
	}
	close(fd);
	return newAdmin;
}

int main(){
	admin adm=initAdmin(); //Creates admin account
	if(adm.userID==-1){
		printf("You already know the credentials\n");
	}
	else{ //Admin credentials created are displayed in server for access
		printf("Init Admin credentials\n");
		printf("Admin Id : %d\n",adm.userID);
		printf("Admin username : %s\n",adm.username);
		printf("Admin Password : %s\n",adm.password);
	}
	struct sockaddr_in server,client;
	int sd,nsd,clientLen;
	bool result;
	sd=socket(AF_INET,SOCK_STREAM,0);

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(8080);

	bind(sd,(struct sockaddr *)&server,sizeof(server));
	listen(sd,10);	//queue length :- 7 clients per connection

	printf("Waiting for the client.....\n");
	while(1){
		clientLen=sizeof(client);
		nsd=accept(sd,(struct sockaddr *)&client,&clientLen);

		printf("Connected to the client.....\n");
		if(!fork()){
			close(sd);	//child doesn't require sd
			serverTask(nsd);
			exit(0);
		}
		else{
			close(nsd);	//child has nsd and parent doesn't require it
		}
	}
	return 0;
}
