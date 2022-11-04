#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdbool.h>
#include "clientFunc.h" //Functionalities of client is defined in this file


int main(){
	struct sockaddr_in server;
	int sd,msgLength;
	char buff[50];
	char result;
	//connection establishment
	sd=socket(AF_INET,SOCK_STREAM,0);
	server.sin_family=AF_INET;
	//server.sin_addr.s_addr=inet_addr("172.16.81.54"); //other machine
	//server.sin_addr.s_addr=INADDR_ANY; //same machine
	server.sin_addr.s_addr=inet_addr("127.0.0.1"); //same machine
	server.sin_port=htons(8080); //Port Address
	connect(sd,(struct sockaddr *)&server,sizeof(server)); //Connecting to the server
	//Welcome Page for clients when they connect to the server
	while(1){
		printf("\e[1;1H\e[2J");
		printf("\n\n\t\t***********************************************************************************************************\n");
		printf("\t\t**************************************  WELCOME TO BANKING SYSTEM  ****************************************\n");
		printf("\t\t***********************************************************************************************************\n\n\n");	
		printf("1. SIGN IN\n2. EXIT\n\nSelect an Option : ");
		int choice;
		while(scanf("%d", &choice)!=1 || getchar()!='\n'){
        scanf("%*[^\n]%*c");
        printf("\nPlease Enter proper option : ");
    	}
		printf("\n");
		//scanf("%d",&choice);
		switch(choice)
		{
			case 1:
				chooseOption(sd);
				showMenu(sd);
				break;
			case 2:
				printf("\n\n\n\t\t***********************************************************************************************************\n");
				printf("\t\t*************************************** Thanks For Visiting Us ********************************************\n");
				printf("\t\t***************************************    Have a Nice Day     ********************************************\n");
				printf("\t\t***********************************************************************************************************\n\n\n");
				exit(0);
				break;
			default:
				printf("\n\t\t\t\t\t!!!INVALID OPTION!!! Please Select a Valid Option...\n");
				char c;
				while((c = getchar()) != '\n' && c != EOF)
					;
				sleep(2);
				break;
		}
	}
	close(sd);
	return 0;
}
