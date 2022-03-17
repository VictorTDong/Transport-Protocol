Client-server network application to demonstrate transport protocols

To compile the program, you will have to run:
gcc -o client client.c
gcc -o server server.c
gcc -o voting voting.c
gcc -o converter converter.c
gcc -o translator translator.c

Now after you have compiled the program, you will have to run it typing ./<filename> on
seperate terminal windows or you could also run the voting, converter and translator on a seperate
server and have client and server on Linux terminal
Once the program is running on separate terminal windows, client will have instructions in the
terminal window on what options they can choose.
The program will successfully establish connections from TCP client -> TCP server -> UDP
Client -> UDP server and also in the reverse order. Program has been tested for the
functionality that was required and everything works.
  
Achieved a grade of 40/40

