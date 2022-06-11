# multithreaded-C-socket
TCP/UDP Multithreaded client and server

These are client-server that were build using c programming language.

The server mainly perfroms a simple process either encrypting or decrypting to a string that has been sent from the client, using caeser cipher with some constraints, where the client can only send strings with lower case letters, it is just a matter of functionallity.
**"YOU CAN MAKE THE SERVER DO WHAT EVER YOU WANT".** 

a quick revision of how socket work on server and client side works:
  
  
                                  **------**
                                 **|SERVER|**
                                  **------**
     ------
    |socket|
     ------
        |    before binding the the ip address and port number,
        |    we have to fill them in the proper fields of server socket address structure.
        |
     ------
    | bind | after binding the socket, it depend whether it is a tcp socket ot a udp socket.
     ------
     ------                                         ------
    |in  TCP|                                      |in  UDP|
     ------                                         ------
        |                                              |
        |                                              |
        |                                              |
     ------                                         ------
    |listen|                                       |rcvfrom| <--------
     ------                                         ------           |
        |                                              |             |
        |                                              |             |
        |                                              |             |
     ------                                         ------           |
    |listen|                                       |send to| -------->
     ------                                         ------
        |
        |
        |
      ------
     |accept|
      ------
        |
        |
        |
      ------
     | recv | <-------
      ------         |
        |            |
        |            |
        |            |
      ------         |
     | send |-------->
      ------
_________________________________________________________________________________________________________________________________________________________


                               **------**
                              **|CLIENT|**
                               **------**
     ------                                                   ------
    |socket|                                                 |socket|
     ------                                                   ------
        |                                                        |
        | we don't have to bind the address on the client side.  |
        |                                                        |
     ------                                                    ------
    |in  TCP|                                                 |in  UDP|
     ------                                                    ------
        |                                                        |
        |                                                        | we don't have to use "connect" in udp because we udp is connectionless.
        |                                                        |
     ------                                                    ------
    |connect|                                                 |rcvfrom| <-------
     ------                                                    ------          |
        |                                                        |             |
        |                                                        |             |
        |                                                        |             |
     ------                                                    ------          |
    | send | <--------                                        |send to| ------->
     ------          |                                         ------
        |            |
        |            |
        |            |
      ------         |
     | recv |-------->
      ------
     
     
     as we established this client-server code make encrypting or decrypting as the main functionallity, but you can make your own functinos.
     
     
     HOW TO USE:
     after compiling every file using gcc command.
     server compilation: gcc -o server server.c
     tcp client compilation: gcc -o tcp tcp_client.c
     udp client compilation: gcc -o udp udp_client.c
     
     we will get three new file: server, tcp and udp.
     we run the server side by giving the port number as a command line argument.
     ./server 55737
     
     I used 55737 as my own port number in the code, you can change it.
     
     
     we run the tcp client side by giving ip address and the port number as a command line arguments.
     ./tcp 127.0.0.1 55737
     
     we run the udp client side by giving ip address and the port number as a command line arguments.
     ./udp 127.0.0.1 55737
     
    
    
