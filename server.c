#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<ctype.h>

#define MAXLINE 4096
#define BL 1024
pid_t pid;
int connected_des;

//creating socket address structs for server and client.
struct sockaddr_in server_addr, client_addr;
socklen_t server_len = sizeof(server_addr);
socklen_t client_len = sizeof(client_addr);

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

void clearBuffer()                             // This function is to clear the buffer from grabage data every single time after entering data.
{
    char c;
    do
    { c = getchar(); }
    while (c != '\n' && c != EOF);
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = waitpid(-1, NULL, WNOHANG);
    printf("  ------------------------------------\n");
    printf("- Child with PID: %d terminated\n", pid);
    printf("  ------------------------------------\n");
    printf("- [TCP-parent] closed connection.\n");
    printf("|_________________________________________________________________________|\n");
    printf(" _________________________________________________________________________ \n");
    printf("|                                                                         |\n");
    printf("|                                SERVER DOWN                              |\n");
    printf("|_________________________________________________________________________|\n");
    exit(0);
}

char * Encrypt(char *message)                  // Encrypting using Caesar Cipher
{
    char ch;
    int i, key = 3;

    for(i = 0; message[i] != '\0'; i++)
    {
        ch = message[i];

        if(ch >= 'a' && ch <= 'w')
        { ch = ch - 32 + key; }
        else if(ch == 'x' || ch == 'y' || ch == 'z')
        { ch = ch - 55; }

        else if(ch >= 'A' && ch <= 'W')
        { ch = ch + 32 + key; }
        else if(ch == 'X' || ch == 'Y' || ch == 'Z')
        { ch = ch + 9; }

        else if(ch >= '0' && ch <= '9')
        { ch = ch + 3; }

        else if(ch == ' ')
        { ch = ch; }

        else
        { printf("- Invalid character\n"); exit(0); }

        message[i] = ch;
    }
    return message;
}

char* Decrypt(char *message)                  // Decrypting using Caesar Cipher
{
    char ch;
    int i, key = 3;

    for(i = 0; message[i] != '\0'; ++i)
    {
        ch = message[i];

        if(ch >= 'd' && ch <= 'z')
        { ch = ch - 32 - key; }
        else if(ch == 'a' || ch == 'b' || ch == 'c')
        { ch = ch - 9; }

        else if(ch >= 'D' && ch <= 'Z')
        { ch = ch + 32 - key; }
        else if(ch == 'A' || ch == 'B' || ch == 'C')
        { ch = ch + 55; }

        else if(ch >= '0' && ch <= '<')
        { ch = ch - 3; }

        else if(ch == ' ')
        { ch = ch; }

        else
        { printf("- Invalid character\n"); exit(0); }
        
        message[i] = ch;
    }
    return message;
}

int main(int argc, char *argv[])
{
    //checking if command line arguments are compatible not greater or less than required.
    if(argc < 2)
        {printf("Too few arguments to run server side\n"); return 0;}
    if(argc > 2)
        {printf("Too many arguments to run server side\n"); return 0;}

    //checking port number validation.
    if(strcmp(argv[1], "55737") != 0)
    {
        printf("Invalid port number, try \"55737\"\n");
        return 0;
    }

    char tcp_data_recv[MAXLINE];
    char udp_data_recv[MAXLINE];

    fd_set rset;
    ssize_t n;
    const int on = 1;

    signal(SIGCHLD, sig_chld);

    //creating socket address structs for server and client.
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_len = sizeof(server_addr);
    socklen_t client_len = sizeof(client_addr);

    //creating server tcp-socket.
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_socket < 0)
        {perror("Error while creating socket"); return 0;}

    int enable = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {perror("Error while setting socket option"); return 0;}

    //filling server info.
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    //binding server tcp-socket with local socket address.
    int tcp_bind_value = bind(tcp_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(tcp_bind_value < 0)
        {perror("Error while binding socket"); return 0;}

    //making server listen at tcp-socket.
    int listen_val = listen(tcp_socket, BL);
    if(listen_val < 0)
        {perror("Error while listening to socket"); return 0;}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    //creating server udp-socket.
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket < 0)
        {perror("Error while creating socket"); return 0;}

    //filling server info.
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    //binding server udp-socket with local socket address.
    int udp_bind_value = bind(udp_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(udp_bind_value < 0)
        {perror("Error while binding socket"); return 0;}
 
    //clear the descriptor set
    FD_ZERO(&rset);

    printf("                         _________________________                         \n");
    printf(" _______________________|        SERVER UP        |_______________________ \n");
    printf("|                                                                         |\n");

    printf("- Waiting for messages from clients ");
    for(int i = 0; i < 3; i++)
    {
        printf(". ");
        fflush(stdout);
        usleep(500000);
    }
    printf("\n\n");

    //get maxfd
    int maxfdp1 = max(tcp_socket, udp_socket) + 1;
    for (;;) 
    {
        //set tcp-socket and udp-socket in readset.
        FD_SET(tcp_socket, &rset);
        FD_SET(udp_socket, &rset);
 
        //select the ready descriptor
        int nready = select(maxfdp1, &rset, NULL, NULL, NULL);
 
        //accepting tcp connections if tcp-socket readable.
        if(FD_ISSET(tcp_socket, &rset)) 
        {
            //parent server accepting connections on server tcp-socket.
            connected_des = accept(tcp_socket, (struct sockaddr*) &client_addr, &client_len);
            if(connected_des < 0)
                {perror("Error while conneting to client"); return 0;}

            //getting client-side info to print server information after connecting to it.
            getpeername(connected_des, (struct sockaddr *) &client_addr, &client_len);

            if( (pid = fork()) == 0 )
            {
                int close_lis = close(tcp_socket);
                if(close_lis < 0)
                    {perror("Error while closing the socket"); return 0;}

                char terminate[300] = "";
                while(strcmp(terminate,"Ctrl") != 0 && strcmp(terminate,"ctrl") != 0)
                {
                    read(connected_des, tcp_data_recv, MAXLINE);
                    if(strcmp(tcp_data_recv,"Ctrl") == 0 || strcmp(tcp_data_recv,"ctrl") == 0 || strcmp(tcp_data_recv,"") == 0)
                    {
                        printf("\n- client (%s:%d) [TCP-child] closed connection.\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                        int close_con = close(connected_des);
                        if(close_con < 0)
                            {perror("Error while closing the socket"); return 0;}
                        exit(0);
                    }
                    printf("- Message:\"%s\" received from the client (%s:%d) [TCP].\n", tcp_data_recv,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

                    char data_recv[MAXLINE];                               // 012345678901234
                    for(int i = 0; i < strlen(tcp_data_recv); i++)         // ehello students
                        data_recv[i] = tcp_data_recv[i+1];                 // 01234567890123
                                                                           // hello students
                    if(tcp_data_recv[0] == 'e')
                    {
                        char *data_sent = Encrypt(data_recv);
                        write(connected_des, data_sent, strlen(data_sent));
                        printf("- Message:\"%s\" sent to client.\n\n",data_sent);
                        memset(tcp_data_recv, 0, sizeof(tcp_data_recv));
                        memset(data_recv, 0, sizeof(data_recv));
                        memset(data_sent, 0, sizeof(data_sent));
                    }

                    else if(tcp_data_recv[0] == 'd')
                    {
                        char *data_sent = Decrypt(data_recv);
                        write(connected_des, data_sent, strlen(data_sent));
                        printf("- Message:\"%s\" sent to client.\n\n",data_sent);
                        memset(tcp_data_recv, 0, sizeof(tcp_data_recv));
                        memset(data_recv, 0, sizeof(data_recv));
                        memset(data_sent, 0, sizeof(data_sent));
                    }

                    read(connected_des, terminate, sizeof(terminate));
                    if(strcmp(terminate,"Ctrl") == 0 || strcmp(terminate,"ctrl") == 0 || strcmp(terminate,"") == 0)
                    {
                        printf("\n- client (%s:%d) [TCP-child] closed connection.\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                        int close_con = close(connected_des);
                        if(close_con < 0)
                            {perror("Error while closing the socket"); return 0;}
                        exit(0);
                    }
                }
                int close_con = close(connected_des);
                if(close_con < 0)
                    {perror("Error while closing the socket"); return 0;}

                exit(0);
            }
            int close_con = close(connected_des);
            if(close_con < 0)
                {perror("Error while closing the socket"); return 0;}
        }

        // if udp socket is readable receive the message.
        if (FD_ISSET(udp_socket, &rset)) 
        {
            //getting client-side info to print server information after connecting to it.
            getpeername(udp_socket, (struct sockaddr *) &client_addr, &client_len);

            char terminate[MAXLINE] = "";
            while(strcmp(terminate,"Ctrl") != 0 && strcmp(terminate,"ctrl") != 0)
            {
                int n1,n2;

                n1 = recvfrom(udp_socket, (char *)udp_data_recv, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &client_len);
                udp_data_recv[n1] = '\0';
                if(strcmp(udp_data_recv,"Ctrl") == 0 || strcmp(udp_data_recv,"ctrl") == 0 || strcmp(udp_data_recv,"") == 0)
                {
                    printf("\n- client (%s:%d) [UDP] closed connection.\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                    printf("|_________________________________________________________________________|\n");
                    printf(" _________________________________________________________________________ \n");
                    printf("|                                                                         |\n");
                    printf("|                                SERVER DOWN                              |\n");
                    printf("|_________________________________________________________________________|\n");
                    exit(0);
                    int close_val = close(udp_socket);
                    if(close_val < 0)
                        {perror("Error while closing the socket"); return 0;}
                    return 0;
                }
                printf("- Message:\"%s\" received from the client (%s:%d) [UDP].\n", udp_data_recv,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

                char data_recv[MAXLINE];                               // 012345678901234
                for(int i = 0; i < strlen(udp_data_recv); i++)         // ehello students
                    data_recv[i] = udp_data_recv[i+1];                 // 01234567890123
                                                                       // hello students

                if(udp_data_recv[0] == 'e')
                {
                    char *data_sent = Encrypt(data_recv);
                    sendto(udp_socket, (const char *)data_sent, strlen(data_sent), MSG_CONFIRM, (const struct sockaddr *) &client_addr, client_len);
                    printf("- Message:\"%s\" sent to client.\n\n",data_sent);
                    memset(udp_data_recv, 0, sizeof(udp_data_recv));
                    memset(data_recv, 0, sizeof(data_recv));
                    memset(data_sent, 0, sizeof(data_sent));
                }

                else if(udp_data_recv[0] == 'd')
                {
                    char *data_sent = Decrypt(data_recv);
                    sendto(udp_socket, (const char *)data_sent, strlen(data_sent), MSG_CONFIRM, (const struct sockaddr *) &client_addr, client_len);
                    printf("- Message:\"%s\" sent to client.\n\n",data_sent);
                    memset(udp_data_recv, 0, sizeof(udp_data_recv));
                    memset(data_recv, 0, sizeof(data_recv));
                    memset(data_sent, 0, sizeof(data_sent));
                }

                n2 = recvfrom(udp_socket, (char *)terminate, MAXLINE, MSG_WAITALL, (struct sockaddr *) &server_addr, &server_len);
                terminate[n2] = '\0';
            }
            int close_val = close(udp_socket);
            if(close_val < 0)
                {perror("Error while closing the socket"); return 0;}

            printf("\n- client (%s:%d) [UDP] closed connection.\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            printf("|_________________________________________________________________________|\n");
            printf(" _________________________________________________________________________ \n");
            printf("|                                                                         |\n");
            printf("|                                SERVER DOWN                              |\n");
            printf("|_________________________________________________________________________|\n");
            return 0;
        }
    }
}
