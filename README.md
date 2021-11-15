# TCP-Wall-Server
TCP Wall-Server Program
Basic TCP Wall server.
Allows a client to connect to the 'Wall' run any of the following commands:

- post: add a message to the wall with a username
- clear: delete all messages on the wall
- quit: end client session
- kill: terminate the server (ends both client and main socket)

To connect to the wall you may run:
netcat localhost 5514 by default
or specify a port as the third CLI argument.
