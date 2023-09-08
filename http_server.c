// i'm a web server
//i receive http GET and POST messages


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#define MAX 256

static int keepRunning = 1;

int post_echo();
int process_msg(char (*message)[], int step);

void intHandler(int a){
    keepRunning = 0;
}

int server(char *port){

//for getting ctrl+c
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

//addrinfo
    int rv;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    rv = getaddrinfo(NULL, port, &hints, &servinfo);
    if(rv == -1){
        perror("addrinfo");
        return 0;
    }

//socket and bind
    int sock_res;
    int bind_res;
    //printf("into socket and connect \n");
    for(p=servinfo; p!=NULL; p=p->ai_next){
       //printf("into socket \n"); 
        sock_res = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        //printf("sock_res is %d \n", sock_res);
        if(sock_res == -1){
            perror("socket");
            continue;
        }
       
        // to avoid "address already in use" error message
        int yes =1;
        setsockopt(sock_res, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        //printf("into bind \n"); 
        bind_res = bind(sock_res, p->ai_addr, p->ai_addrlen);
        if(bind_res == -1){
            perror("bind");
            continue;
        }
        break;
    }

//listen
    int listen_res;
    listen_res = listen(sock_res,10);
    if(listen_res ==-1){
        perror("listen");
        return 0;
    }
    //printf("listening \n");

    int p_count=0;   //keep track of current no of events
    int p_size=5;   
    struct epoll_event event, events[p_size];
    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1){
        printf("failed to create epoll file descriptor\n");
        return 1;
    }
    event.events =EPOLLIN;
    event.data.fd = sock_res;
    
    p_count=1;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_res, &event)){
        printf("failed to add file descriptor to epoll");
        return 1;
    }

    struct sockaddr_storage their_addr;
    socklen_t len = sizeof their_addr;

    char message[MAX];
    int len_message = strlen(message);

//accept and recv
    while(keepRunning){
       // printf("into big for \n");
        int event_count = epoll_wait(epoll_fd, events, p_count, 3000);

        //printf("read events is %d \n", event_count);
        if(event_count < 0){
            perror("event_count");
            return 1;
        }

        for(int i=0; i<event_count; i++){

            //printf(" for i = %d  \n", i);               
            if( events[i].data.fd == sock_res){
                int accept_res;
                accept_res = accept(sock_res,(struct sockaddr *)&their_addr, &len);
                printf("accepted a new fd \n");
                if(accept_res == -1){
                    perror("accept");
                    continue;
                }
                event.events = EPOLLIN;
                event.data.fd = accept_res;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_res, &event)){
                    printf("failed to insert socket to epoll\n");
                }
                p_count++;
            }//its the listener
                else{
                    static int step=1;
                    int phase=1;
                    int recv_res;
                    //printf("waiting for a message \n");
                    while(phase){
                        
                        recv_res = recv(events[i].data.fd , message, MAX, 0);

                        if(recv_res ==0){
                            perror("recv"); 
                            break;                       
                        }
                        if(recv_res <0){
                            perror("recv"); 
                            continue;                       
                        }
                        
                        if(!strncmp(message, "\n",1)){
                            //keepRunning=0;
                            phase=0;
                            step =0;
                            int body_len = process_msg(&message, step );
                            //printf("body_les is %d\n", body_len);
                            recv_res = recv(events[i].data.fd , message, body_len, 0); 
                            
                            printf("\ns:"); //printing the input   
                            for(int i=0; i< recv_res -1 ; i++){
                                printf("%c", message[i]);
                            }
                            printf("\n");
                            int check_post_echo = post_echo(); 
                            //printf("check_post_Echo %d\n",check_post_echo);
                            if(check_post_echo){
                                int send_res = send(events[i].data.fd, message, body_len, 0 );
                            }
                            
                            close(events[i].data.fd );
                            p_count--;
                            step++;
                            memset(message, 0, recv_res);//clean up                           
                            continue;
                        } 
                        else if(recv_res > 0){                           
                            //printf("step is %d\n", step);
                            //printf("gonna process \n");
                            process_msg(&message, step );
                            //printf("done process \n");
                            step++;
                        }
                        
                        printf("\ns:");   //printing the input   
                        for(int i=0; i< recv_res -1 ; i++){
                            printf("%c", message[i]);
                        }
                        printf("\n"); 
                        memset(message, 0, recv_res);//clean up                       
                    }                  
                }//not a listener
        }//end counting on i.
    }//end empty for loop
    close(sock_res);
    return 0;
}

int main(int argv, char *argc[]){
    if(argv !=2){
        printf("usage: <file_name> <PORT>\n");
        return 0;
    }
    server(argc[1]);
    return 0;
}

//problem when listener stops listening in unterminating while loop is solved
//by <=0 as result for recv.
                    //printf("strlen(message) is %lu\n", strlen(message));
                    //printf("message received %d bytes successfully \n", recv_res);
                    
                    //printf("before strncmp\n");
                    //printf("trncmp is %d\n", strncmp(message, "\n",10));
                    //message[recv_res]='\0';//clean up
                    //message[recv_res]='\0';