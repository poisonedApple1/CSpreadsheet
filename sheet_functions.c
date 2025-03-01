#include "sheet_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.h"
#include "parser.h"
#include <math.h>
Sheet sheet;


void initialise_sheet(int m, int n){
    sheet.rows = m;
    sheet.cols = n;
    sheet.data = malloc(m * sizeof(Cell *));
    for (int i = 0; i < m; i++)
    {
        sheet.data[i] = malloc(n * sizeof(Cell));
        for (int j = 0; j < n; j++)
        {
            sheet.data[i][j].value = 0;
            sheet.data[i][j].isError = false;
            sheet.data[i][j].op_code = 'X';
            sheet.data[i][j].cell1.col = -1;
            sheet.data[i][j].cell1.row = -1;
            sheet.data[i][j].cell2.col = -1;
            sheet.data[i][j].cell2.row = -1;
            sheet.data[i][j].dep_count = 0;
            sheet.data[i][j].dependencies = NULL;
        }
    }
}

char *column_name(int column_number)
{ // column number to col name
    static char name[4];
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    name[2] = letters[column_number % 26];
    column_number /= 26;
    if (column_number != 0)
    {
        name[1] = letters[(column_number - 1) % 26];
        column_number = (column_number - 1) / 26;
    }
    else
    {
        name[1] = ' ';
    }

    if (column_number != 0)
        name[0] = letters[(column_number - 1) % 26];
    else
        name[0] = ' ';

    name[3] = '\0';
    return name;
}

int get_col(char *col_name)
{ // column name to col number
    int col = -1;
    if (strlen(col_name) == 1)
        col = col_name[0] - 'A';
    else if (strlen(col_name) == 2)
        col = (col_name[0] - 'A' + 1) * 26 + (col_name[1] - 'A');
    else if (strlen(col_name) == 3)
        col = 26 * 26 * (col_name[0] - 'A' + 1) + (col_name[1] - 'A' + 1) * 26 + (col_name[2] - 'A');
    return col;
}

void print_table(int column_start, int row_start)
{ // printing the table including the shift
    int m = sheet.rows;
    int n = sheet.cols;
    const int display_column_number = column_start + min(n - column_start, 10), display_row_number = row_start + min(m - row_start, 10);
    const int space = 10;
    printf("%*s", space, "");
    for (int i = column_start; i < display_column_number; i++)
    {
        printf("%*s", space, column_name(i));
    }
    printf("\n");
    for (int i = row_start; i < display_row_number; i++)
    {
        printf("%*d", space, i + 1);
        for (int j = column_start; j < display_column_number; j++){
            if(sheet.data[i][j].isError)printf("       ERR");
            else printf("%*d", space, sheet.data[i][j].value);
        }
        printf("\n");
    }
}

// void free_sheet()
// {
//     for (int i = 0; i < sheet.rows; i++)
//     {
//         for (int j = 0; j < sheet.cols; j++)
//         {
//             Node *temp = sheet.data[i][j].dependencies;
//             while (temp != NULL)
//             {
//                 Node *temp2 = temp->next;
//                 free(temp);
//                 temp = temp2;
//             }
//         }
//         free(sheet.data[i]);
//     }
//     free(sheet.data);
// }

void remove_from_list(Cell *target, int delete_cell)
{
    dependency_node *curr = (target->dependencies);
    dependency_node *prev = NULL;
    while (curr != NULL){
        if (curr->data == delete_cell){
            if(prev == NULL){
                target->dependencies = curr->next;
            }
            else{
                prev->next = curr->next;
            }
            free(curr);
            target->dep_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    target->dep_count--;
    return;
}

void insert_into_list(Cell *target, int insert_cell){
    dependency_node *new_node = (dependency_node *)malloc(sizeof(dependency_node));
    new_node->data = insert_cell;
    new_node->next=target->dependencies;
    target->dependencies=new_node;
    target->dep_count++;
    return;
}

//dfs to find indegree 0 node
avl_node* dfs(avl_node *root){
    if(root==NULL) return NULL;
    if(root->indegree==0){
        return root;
    }
    else{
        avl_node *left = dfs(root->left);
        avl_node *right = dfs(root->right);
        if(right!=NULL) return right;
        else if(left!=NULL) return left;
        else return NULL;
    }
}


queue* topological_sort(avl_tree *tree){
    queue *q = NULL;
    avl_node *indegree_0_node = dfs(tree->root);
    q=push(q,indegree_0_node);
    queue *final_sorted=NULL;
    while(!is_empty(q)){
        avl_node *node = front(q);
        q=pop(q);
        final_sorted=push(final_sorted,node);
        dependency_node *curr = sheet.data[node->data%1000][node->data/1000].dependencies;
        while(curr!=NULL){
            avl_node *temp = avl_find(tree,curr->data);
            temp->indegree--;
            if(temp->indegree==0){
                q=push(q,temp);
            }
            curr=curr->next;
        }
    }   
    return final_sorted;
}

/*
    = ->cell
    + -> cell+cell
    - ->cell-cell
    * -> cell*cell
    / -> cell/cell
    S -> SUM
    m -> MIN
    M -> MAX
    A -> AVG
    D -> STDEV
    Z -> sleep(cell)
    X -> const op const,const , sleep(const)
    p -> const+cell or cell+const
    s -> const-cell or cell-const
    u -> const*cell or cell*const
    d -> const/cell or cell/const
*/
   
   
void recalculate(Cell *cell){
    if(cell == NULL) return ;
    int ans;

    switch(cell->op_code){
        case '=':
            ans=sheet.data[cell->cell1.row][cell->cell1.col].value;
            break;
        case '+':  
        case '-':
        case '*':
        case '/':
            if(sheet.data[cell->cell1.row][cell->cell1.col].isError || sheet.data[cell->cell2.row][cell->cell2.col].isError){
                cell->isError=true;
                return;
            }
            int val1=sheet.data[cell->cell1.row][cell->cell1.col].value;
            int val2=sheet.data[cell->cell2.row][cell->cell2.col].value;
            ans=compute_cell(cell->op_code,val1,val2);
            if(calc_error) cell->isError=true;
            else cell->isError=false;
            break;
        case 'p':
        case 's':
        case 'u':
        case 'd':
            if(sheet.data[cell->cell1.row][cell->cell1.col].isError){
                cell->isError=true;
                return;
            }
            int val=cell->cell2.col + cell->cell2.row* pow(2,16);
            char op_code=get_op_code_rev(cell->op_code);
            ans=compute_cell(op_code,sheet.data[cell->cell1.row][cell->cell1.col].value,val);
            if(calc_error) cell->isError=true;
            else cell->isError=false;
            cell->value=ans;
            break;
        case 'Z':
            ans=sheet.data[cell->cell1.row][cell->cell1.col].value;
            sleep(ans);
            break;
        case 'S':
        case 'm':
        case 'M':
        case 'A':
        case 'D':
            for(int i=cell->cell1.row;i<=cell->cell2.row;i++){
                for(int j=cell->cell1.col;j<=cell->cell2.col;j++){
                    if(sheet.data[i][j].isError){
                        cell->isError=true;
                        return;
                    }
                }
            }
            ans=compute_range_func(cell->op_code,cell->cell1.row,cell->cell1.col,cell->cell2.row,cell->cell2.col);
            if(calc_error) cell->isError=true;
            else cell->isError=false;
            break;
        default:
            return ;
    }
    cell->value=ans;    
    return ;
}

void remove_dependency(cell_info cell){
    Cell * curr_cell = &sheet.data[cell.row][cell.col];
    switch(curr_cell->op_code){
        case 'X':
            break;
        case '=':
        case 'p':
        case 's':
        case 'u':
        case 'd':
        case 'Z':
            remove_from_list(&sheet.data[curr_cell->cell1.row][curr_cell->cell1.col],cell.col*1000+cell.row);
            break;
        case 'S':
        case 'm':
        case 'M':
        case 'A':
        case 'D':
            for(int i=curr_cell->cell1.row;i<=curr_cell->cell2.row;i++){
                for(int j=curr_cell->cell1.col;j<=curr_cell->cell2.col;j++){
                    remove_from_list(&sheet.data[i][j],cell.col*1000+cell.row);
                }
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            remove_from_list(&sheet.data[curr_cell->cell1.row][curr_cell->cell1.col],cell.col*1000+cell.row);
            remove_from_list(&sheet.data[curr_cell->cell2.row][curr_cell->cell2.col],cell.col*1000+cell.row);
            break;
    }
    curr_cell->cell1.col = -1;
    curr_cell->cell1.row = -1;
    curr_cell->cell2.col = -1;
    curr_cell->cell2.row = -1;
}

void add_to_tree(avl_tree *head,cell_info cell){
    printf("adding to tree %d %d\n",cell.row,cell.col);
    if(sheet.data[cell.row][cell.col].dependencies==NULL){
        return;
    }
    dependency_node *curr = sheet.data[cell.row][cell.col].dependencies;
    while(curr!=NULL){
        cell_info temp = {curr->data%1000,curr->data/1000};
        avl_insert(head,curr->data);
        add_to_tree(head,temp);
        curr=curr->next;
    }
}

// void topological_sort(stack *head){
    
// }
    
void add_constraints(cell_info curr_cell,cell_info cell1,cell_info cell2,int value,char op_code){
    Cell *cell = &sheet.data[curr_cell.row][curr_cell.col];
    remove_dependency(curr_cell);
    int curr_cell_row_col = curr_cell.col*1000+curr_cell.row;

    avl_tree *tree = avl_create();
    avl_insert(tree,curr_cell_row_col);
    tree->root->indegree=0;
    add_to_tree(tree,curr_cell);
    queue *sorted = topological_sort(tree);
    
    cell->cell1 = cell1;
    cell->cell2 = cell2;
    cell->op_code = op_code;
    switch(op_code){
        case 'X':
            break;
        case '=':
        case 'p':
        case 's':
        case 'u':
        case 'd':
        case 'Z':
            insert_into_list(&sheet.data[cell1.row][cell1.col],curr_cell_row_col);
            break;
        case 'S':
        case 'm':
        case 'M':
        case 'A':
        case 'D':
            for(int i=cell1.row;i<=cell2.row;i++){
                for(int j=cell1.col;j<=cell2.col;j++){
                    insert_into_list(&sheet.data[i][j],curr_cell_row_col);
                }
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            insert_into_list(&sheet.data[cell1.row][cell1.col],curr_cell_row_col);
            insert_into_list(&sheet.data[cell2.row][cell2.col],curr_cell_row_col);
            break;
    }

    cell->cell1.col = cell1.col;
    cell->cell1.row = cell1.row;
    cell->cell2.col = cell2.col;
    cell->cell2.row = cell2.row;
    cell->value = value;


    // pretty_print(tree);
    // queue *sorted = topological_sort(tree);
    // while(!is_empty(sorted)){
    //     avl_node *node = front(sorted);
    //     sorted=pop(sorted);
    //     // printf("\n----------------------------------\n");
    //     // printf("calculating %d %d %d\n",node->data%1000,node->data/1000,sheet.data[node->data%1000][node->data/1000].value);
    //     recalculate(&sheet.data[node->data%1000][node->data/1000]);
    //     // printf("calculated %d %d %d\n",node->data%1000,node->data/1000,sheet.data[node->data%1000][node->data/1000].value);
    //     // printf("----------------------------------\n");
    // }
}
