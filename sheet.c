#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <unistd.h> 



int min(int a,int b){
    return (a<b)?a:b;
}


char* column_name(int column_number) {
    static char name[4];
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    name[2]=letters[column_number% 26];
    column_number/= 26;
    
    if (column_number!=0){
        name[1]=letters[(column_number-1)% 26];
        column_number=(column_number-1)/ 26;
    } else name[1]=' ';
    
    if (column_number!=0) name[0]=letters[(column_number-1)%26];
     else name[0]=' ';
    
    name[3]='\0';
    return name;
}
void print_table(int m,int n,int column_start,int row_start){
    const int display_column_number=column_start+min(n,10),display_row_number=row_start+min(m,10);
    const int space=10;
    

    printf("%*s",space,"");
    for(int i=column_start;i<display_column_number;i++){
        printf("%*s",space,column_name(i));
    }

    printf("\n");

    for(int i=row_start;i<display_row_number;i++){
        printf("%*d",space,i+1);

        
        for(int j=column_start;j<display_column_number;j++){
            printf("%*s",space,"0");

        }
        printf("\n");
    }

}


int main(int argc, char *argv[]){

    if (argc != 3) {
        printf("Enter m and n values");
        return 0;
    }
    
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    int column_start=0,row_start=0;
    float execution_time=0.0;
    char status[20]="ok";
    char command[10];
    while(1){
        print_table(m,n,column_start,row_start);
        printf("[%.1f] (%s) >",execution_time,status);
        scanf("%s",command);
        
        if(strcmp(command,"w")==0){
            if(row_start>0) row_start-=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"s")==0){
            if(row_start<m-10)row_start+=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"a")==0){
            if(column_start>0)column_start-=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"d")==0){
            if(column_start<n-10) column_start+=10;
            strcpy(status,"ok");
        }

        else if(strcmp(command,"q")==0){
            break;
        }
        else{
            strcpy(status,"unrecognized cmd");
        }
    }
    


    return 0;

}


