# Online-Banking-Management-System
The project aims to develop a banking system that is user-friendly and multi- functional.

This has a server client architecture where more than one client can connect to the server and can perform operations needed.

# There are 3 types of User : Administrator, Joint Account User, Normal Account User.

The functionalities of Joint Account and Normal Account Users are almost same whereas admin will have special permissions to alter the details of bank account. Read Question.pdf for more info on requirements.

The server.c is ran to up the server which initially creates a admin account(If not present) for administrator to use.
Then clients can connect to server by runnning client.c and the user can perform operations according to his requirement. Admin can add new accounts and delete accounts where normal users get user Id for account user login mode.

Fork is used in server so that it can handle multiple clients as once.
