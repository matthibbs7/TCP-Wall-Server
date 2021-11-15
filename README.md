# TCP-Wall-Server
TCP Wall-Server Program
Basic TCP Wall server.
Allows a client to connect to the 'Wall' and run any of the following commands:

- post: add a message to the wall with a username
- clear: delete all messages on the wall
- quit: end client session
- kill: terminate the server (ends both client and main socket)

To connect to the wall you may run:
netcat localhost 5514 by default
or specify a port as the third CLI argument.

<img src="https://user-images.githubusercontent.com/89611778/141712954-1e2b4445-c141-4c3c-b91f-830a9b9c3a53.png" alt="alt text" width="300" height="300">
Note: the wall server will remain online until the 'kill' command is ran or the CLI that ran the executable cancels the process.

