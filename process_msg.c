#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256
static int content_len =0;
static int post_flag=0;
static int echo_flag =0;
static int body_length=0;

int post_echo(){
    echo_flag=1;//printf("post echo and content_len : %d, %d, %d\n", post_flag, echo_flag, body_length);
    if((post_flag & echo_flag) && (body_length>0)){
        return 1;
    }
    return 0;
}

int second_part(int left, int right , char (*s)[], int *l){
    //printf("left and right is %d %d\n", left, right);
    //printf("string is %s\n", *s);
    
    char s_first_half[MAX];
    memset(s_first_half, 0, MAX);
    char s_second_half[MAX];
    memset(s_second_half, 0, MAX);
    
    int mid_idx=0;
    
    //printf("static content length is %d\n", content_len);
    //copy contents left to the colon into s_first_half
    for(int i=left; ((*s)[i] != ':' && i<right) ; i++){       
        s_first_half[i-left]= (*s)[i];
        //printf("  .i'm:%c. ",s_first_half[i-left]);
        mid_idx=i;
    }

    //make the last value of first buffer zero
        if((*s)[mid_idx+1] == ':'){
            s_first_half[mid_idx+1]= 0;
            //printf("s_first_half[%d] is %c\n", mid_idx+1, s_first_half[mid_idx+1]);
        } 

    //copy contents right to colon into s_second_half       
    mid_idx += 1;
    for(int j= mid_idx+2; j<right ; j++){
        //printf("hey there j and j-(mid_idx+2) is %d, %d\n", j, j-(mid_idx+2));
        s_second_half[j-(mid_idx+2)]= (*s)[j];
    }

    //printf("first half is :%s, and second half is :%s.\n", s_first_half, s_second_half);
    //printf("length of first and second half is :%lu. or .%lu.\n",strlen(s_first_half), strlen(s_second_half));

    if(strcmp(s_first_half, "Content-Length") == 0){       
        sscanf(s_second_half, "%d", &content_len);
        printf("\n Content-Length :%s.\n", s_second_half);
    }
    else if(strcmp(s_first_half, "Content-Type")==0){        
        printf("\n Content-Type :%s.\n", s_second_half);      
    }
    else if(strcmp(s_first_half, "Host")==0){
        printf("\n Host :%s.\n",s_second_half);
    }
    else{
        printf("Accepted Field names are \"Content-Length\", \"Content-Type\" or \"Host\". They are case and space sensitive and is followed by a colon. \n");
    } 
  
    return 0;
}

int first_part(int left, int right, char  (*s)[], int *l){
    int last_path_idx=0;
    int buff_size = right+1-left;
    char buff[buff_size ];
    for(int i=left; i<=right; i++){
        buff[i-left]= (*s)[i]; 
        
    }
    if( (buff[0]== 'G') && (buff[1]== 'E') && (buff[2]== 'T')==1){
        printf("\nmethod: GET\n");
        //its GET check for path
        post_flag=0;
        printf("path: ");
        for(int i=3; buff[i]!= 'H' ;i++){
            printf("%c", buff[i]);
            last_path_idx =i;
        }
        //printf("last_path_idx is %d", last_path_idx);
        //printf("strcmp %s %s\n",strstr(buff, " / "), strstr(buff, " /echo "));
        if((strstr(buff, " / ") == NULL) && (strstr(buff, " /echo ") == NULL )){
            printf("\ninvalid path \n");
            return 0;
        }
        printf("\nprotocol: ");
        for(int j=last_path_idx+1 ; j<right; j++){
            printf("%c", buff[j]);
        }
        if((strstr(buff, "HTTP/1.0") == NULL) ){
            printf("\ninvald protocol \n");
            return 0;
        }
    }
    else if ( (buff[0]== 'P') && (buff[1]== 'O') && (buff[2]== 'S') && (buff[3]== 'T')==1){
        printf("\nmethod: POST\n");
        //its GET check for path
        post_flag=1;

        printf("path: ");
        for(int i=4; buff[i]!= 'H' ;i++){
            printf("%c", buff[i]);
            last_path_idx =i;
        }
        //printf("last_path_idx is %d", last_path_idx);
        //printf("strcmp %s %s\n",strstr(buff, " / "), strstr(buff, " /echo "));
        if((strstr(buff, " / ") == NULL) && (strstr(buff, " /echo ") == NULL )){
            printf("\ninvalid path \n");
            return 0;
        }

        if(strstr(buff, " /echo ") != NULL ){
            echo_flag=1;
        }
        else{
           echo_flag=0; 
        }

        printf("\nprotocol is : ");
        for(int j=last_path_idx+1 ; j<right; j++){
            printf("%c", buff[j]);
        }
        if((strstr(buff, "HTTP/1.0") == NULL) ){
            printf("\ninvald protocol \n");
            return 0;
        }       
    }
    else{
        printf("invalid method");
        return 0;
    }
    return 0;
}
  
int process_msg(char (*input)[], int step){
    int l = strlen(*input);  
    //printf("into process_msg\n");
    if(step == 1){
      first_part(0, l-1, input, &l);  
    }
    if(step >=2){
        second_part(0, l-1, input, &l);
    }
    if(step == 0){
        int temp = content_len;
        body_length = temp;
        
        //printf("body_leng is %d\n", body_length);
        content_len=0;
        return body_length ;
    }
    
    //printf("input is %s\n", *input);
    return 0;
}



/*
printfs

//second_part
    //printf("(*s)[right] is %c  \n", (*s)[right]);
    printf("middle value is %c and right-1 is %c\n",  (*s)[mid_idx],(*s)[right-1]);
    //printf("left is %d , mid is %d, right is %d\n",left,mid_idx+2, right);
    //printf("middle value is %c and right-1 is %c\n",  (*s)[mid_idx],(*s)[right-1]);
    //printf("s_first_half[%d] is .%c.\n", 100, s_first_half[100]);
    //first_part
    //printf("buff[%d] is %c ", i-left,buff[i-left]);


//prepare second parameter for strcspn
    char second_arg[MAX];
    second_arg[0] = '\\';
    second_arg[1] = 'r';
    second_arg[2] = '\\';
    second_arg[3] = 'n';
    int till_Color = strcspn( s_second_half, second_arg);
    
    //printf( "strcspn is %lu\n", strcspn( s_second_half, second_arg) );
*/