# Multiclient Chatroom
It's a multiclient chatroom in c using sockets and multithreading. It works with a client and a server module. Server keeps the message if the target client is offline; if the client is online, it will forward the message to the client. When a client connect to the server again it will receive the messages that server kept while the client is offline.

## Instructions
* BEGIN <name>: Begin the chat session.
* SEND <name> <message>: Send a message to a client.
* END: End the session and disconnect from server.
  
A sample output when you begin the session

[![Output](https://www.linkpicture.com/q/Screenshot_1_11.jpg)](https://www.linkpicture.com/view.php?img=LPic63d1604eb33d4582303472)
  
# Installation
  
* Paste the following code to your terminal of choice in whichever directory you want: `git clone https://github.com/hennastone/multiclient-chatroom-in-c`.
* Compile the modules with -lpthread option.
  
That's it, run the server.c first and then run the client.c to connect to the server.
