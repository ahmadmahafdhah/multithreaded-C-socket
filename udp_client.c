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
#include<netdb.h>

#define MAXLINE 4096
char terminate[300] = "";
int socket_des;
struct sockaddr_in server_addr;
socklen_t server_len = sizeof(server_addr);

void clearBuffer()                                          // This function is to clear the buffer from grabage data every single time after entering data.
{
    char c;
    do
    { c = getchar(); }
    while (c != '\n' && c != EOF);
}

void sig_quit(int signo);

int validate_number(char *str)
{
   while(*str)
   {
      if(!isdigit(*str))
      {                                                     //if the character is not a number, return false.
         return 0;
      }
      str++;                                                //point to next character.
   }
   return 1;
}

int validate_ip(char *ip)
{
    int i, num, dots = 0;                                   //check whether the IP is valid or not.
    char *ptr;

    if(ip == NULL)
        return 0;

    ptr = strtok(ip, ".");                                  //cut the string using dor delimiter.
    if(ptr == NULL)
        return 0;

    while(ptr)
    {
        if (!validate_number(ptr))                          //check whether the sub string is holding only number or not
            return 0;

        num = atoi(ptr);                                    //convert substring to number
        if (num >= 0 && num <= 255)
        {
            ptr = strtok(NULL, ".");                        //cut the next part of the string
            if (ptr != NULL)
               dots++;                                      //increase the dot count
        }

        else
            return 0;
    }

    if (dots != 3)                                          //if the number of dots are not 3, return false
        return 0;

    return 1;
}

int main(int argc, char *argv[])
{
    signal(SIGQUIT, sig_quit);

    //checking if command line arguments are compatible not greater or less than required.
    if(argc < 3)
        {printf("Too few arguments to run client side\n"); return 0;}
    if(argc > 3)
        {printf("Too many arguments to run client side\n"); return 0;}

    //checking host name validation.
    if(strcmp(argv[1], "nes416_server") != 0)
    {
        printf("Invalid host name, try \"nes416_server\"\n");
        return 0;
    }

    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    // get the addresses of www.yahoo.com:
    he = gethostbyname(argv[1]);
    if(he == NULL)
    { // do some error checking
        herror("Error while getting hostname");
        exit(1);
    }

    // print information about this host:
    /*printf("Official name is: %s\n", he->h_name);
    printf("IP address: %s\n", inet_ntoa(*(struct in_addr*)he->h_addr));
    printf("All addresses: ");
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++)
    {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");*/

    //checking port number validation.
    if(strcmp(argv[2], "55737") != 0)
    {
        printf("Invalid port number, try \"55737\"\n");
        return 0;
    }

    //creating client-side socket.
    socket_des = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_des < 0)
        {perror("Error while creating the socket"); return 0;}

    //filling server info.
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    inet_aton(argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    printf("                         _________________________                         \n");
    printf(" _______________________| Welcome to NS encrypter |_______________________ \n");
    printf("|                                                                         |\n");
    printf("|           -You can encrypt or decrypt messages, please enter:           |\n");
    printf("|                         1) to encrypt a message                         |\n");
    printf("|                         2) to decrypt a message                         |\n");

    
    while(strcmp(terminate,"Ctrl") != 0 && strcmp(terminate,"ctrl") != 0)
    {
        int sp = 0;
        printf("- Enter your choice: ");
        scanf("%d",&sp);
        clearBuffer();
        while(sp != 1 && sp != 2)
        {
            sp = 0;
            printf("\n- Invalid operation.\n- To encrypt a message enter 1.\n- To decrypt a message enter 2.\n- please enter operation number again: ");
            scanf("%d",&sp);
            clearBuffer();
        }

        int n, len;
        char data_recv[MAXLINE];
        char data_sent[MAXLINE] = "";
        char data_entered[MAXLINE];
        printf("\n- Enter a message (small letters and numbers only): ");
        scanf("%[^\n]s",data_entered);
        clearBuffer();

        int valid = 0;
        while(valid != 1)
        {
            for(int i = 0; data_entered[i] != '\0'; ++i)
            {
                char ch = data_entered[i];
                if( (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch == ' ') )
                    valid = 1;
                else
                {
                    printf("- chat #%d \"%c\" is invalid character.\n", i+1, data_entered[i]);
                    printf("- Enter a valid message (small letters and numbers only): ");
                    scanf("%[^\n]s",data_entered);
                    clearBuffer();
                    i = 0;
                    valid = 0;
                    printf("\n");
                }
            }
        }

        if(sp == 1)
        {
            strcat(data_sent,"e");
            for(int i = 0; data_entered[i] != '\0'; ++i)
            {
                char ch = data_entered[i];

                if(ch >= 'a' && ch <= 'z')
                { ch = ch; }

                else if(ch >= '0' && ch <= '9')
                { ch = ch; }

                else if(ch == ' ')
                { ch = ch; }

                else
                { printf("- Invalid character\n"); exit(0); }

                data_entered[i] = ch;
            }
            strcat(data_sent,data_entered);
        }

        else if(sp == 2)
        {
            strcat(data_sent,"d");
            for(int i = 0; data_entered[i] != '\0'; ++i)
            {
                char ch = data_entered[i];

                if(ch >= 'a' && ch <= 'z')
                { ch = ch; }

                else if(ch >= '0' && ch <= '9')
                { ch = ch; }

                else if(ch == ' ')
                { ch = ch; }

                else
                { printf("- Invalid character\n"); exit(0); }
                
                data_entered[i] = ch;
            }
            strcat(data_sent,data_entered);
        }

        sendto(socket_des, (const char *)data_sent, strlen(data_sent), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        printf("\n- Message:\"%s\" sent to the server.\n",data_entered);
               
        n = recvfrom(socket_des, (char *)data_recv, MAXLINE, MSG_WAITALL, (struct sockaddr *) &server_addr, &server_len);
        data_recv[n] = '\0';
        printf("- Message:\"%s\" received from the server.\n",data_recv);
        memset(data_sent, 0, sizeof(data_sent));
        memset(data_recv, 0, sizeof(data_recv));

        printf("\n- If you want to stop the programm press (ctrl + \\)"".\n- Enter anything else if you want to do another operation.\n: ");
        scanf("%s",terminate);
        clearBuffer();

        sendto(socket_des, (const char *)terminate, strlen(terminate), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        if(strcmp(terminate,"Ctrl") != 0 && strcmp(terminate,"ctrl") != 0)
            {
                printf("|_________________________________________________________________________|\n\n");
                printf(" _________________________________________________________________________ \n");
                printf("|                                                                         |\n");
                printf("|                         1) to encrypt a message                         |\n");
                printf("|                         2) to decrypt a message                         |\n");
            }
    }
    return 0;
}

void sig_quit(int signo)
{
    strcpy(terminate,"Ctrl");
    sendto(socket_des, (const char *)terminate, strlen(terminate), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    int close_val = close(socket_des);                                      //closing the socket.
    if(close_val < 0)
        {perror("Error while closing the socket"); return;}

    printf("\n");
    printf("|_________________________________________________________________________|\n");
    printf(" _________________________________________________________________________ \n");
    printf("|                                                                         |\n");
    printf("|                -Thank you for using NS encrypter. BYE !!                |\n");
    printf("|_________________________________________________________________________|\n");
    exit(0);
    return;
}
