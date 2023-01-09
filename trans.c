// 思路 在得到这个txt文件的时候我们并不知道有多少行 当我们每读取一行 我们将信息分成我们需要的部分

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>

// 配置链表 数组 comNum[2] = {Start Charater, Character Length}
typedef struct comStr 
{
    int start;
    int length;
    int count;
    struct comStr *next;
} LinkList0;

typedef struct infoStr 
{
    char* content;
    struct infoStr *next;
} LinkList1;

// Functions
static char *openTXT (char *fname);
static void split (char *source, char *seperator, char **result, int *num);
static LinkList0 *cmdLink (char *command, char *sep);
static char* substring (char *src, int start, int length);
static char* genereateRow (char* srcLine, LinkList0 *currLink);
static void showNodes (LinkList0 *currLink);
static void freeNodes (LinkList0 *currLink);
static void writeCSV (char *srcText, char* command);

//////////////////////////////////////////////////////////////////////////////////////
// 01 读取函数
/* 打开指定的txt文件并读取文档 */
static char *openTXT (char *fname) 
{    
    FILE *fp = NULL;
	fp = fopen(fname,"r");

    // 获取当前文件的大小
    fseek(fp, 0, SEEK_END);
    int fileSize;
    fileSize = ftell(fp);                                      
    printf("fileSize=====%d\n", fileSize);

    // 分配相应内存存储字符串并读取strText
    char *strText = (char*) malloc(fileSize * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(strText, fileSize, 1, fp);
    return strText;
}

// 02 分割函数
/* 根据分割符号去分割函数*/
static void split (char *source, char *seperator, char **result, int *num) 
{
    // 去除空的情况
    assert(source != NULL && strlen(source) != 0);
    assert(seperator != NULL && strlen(seperator) != 0);
    
    // 分词并运行 num返回一共分了多少个 result是所分的数组
    int count = 0;
    char* tmpStr = source;
    char* pNext;

    pNext = strtok(tmpStr, seperator);
    while (pNext != NULL) {
        result[count] = pNext;
        pNext = strtok(NULL, seperator);
        ++count;
    }

    // for (int i = 0; i < count; i++) printf(result[i]);
    // printf("%d", count);
    *num = count;
}

// 03 命令转换
/* 根据命令将转换为分割数组 输入规则为 0,4;4,6;10,2;*/
static LinkList0 *cmdLink (char *command, char *sep)
{
    // 配置链表
    LinkList0 *head, *node, *end;
    LinkList0 *cnode = (LinkList0*) malloc(sizeof(LinkList0));
    head = cnode;
    end  = head;
    head->next = end;
    
    // 根据 sep 分割输入的命令
    int counter = 0;
    int  cmdlen = strlen(command);
    char **cmds = malloc((cmdlen + 1) * sizeof(char));
    int  *cmdn  = malloc(1 * sizeof(int));

    split(command, sep, cmds, cmdn);
    // printf("\nCommand Number: %d\n", *cmdn);

    // 将分割好的命令分成数导入到数组中 并写进链表里
    for (int count = 0; count < *cmdn; count++) 
    {
        // 生成 node, 如果 count == 0, 则标记为第一个
        node = (LinkList0*) malloc(sizeof(LinkList0));
        if (count == 0) 
        {
            head = node;    // 标记链表头
            end  = head;    // 标记链表尾为链表头
            head->next = end;
            free(cnode);
        }

        // 生成数
        int currArr[2] = {0};
        char* tmpStr = cmds[count];
        char* seperator = ",";
        char* startc = strtok(tmpStr, seperator);
        char* length = strtok(NULL, seperator);

        node->start = atoi(startc), node->length = atoi(length);
        counter++;

        // 如果 count > 0, 则添加上下一个 node
        if (count > 0) 
        {
            end->next  = node;
            end = node;
        }
    }

    end->next = NULL;
    head->count = counter;
    return head;
}

// 04 生成子字符串
/* 截取并返回一段子字符串, memory set*/
static char* substring (char *src, int start, int length)
{
    char *result = malloc((length + 1) * sizeof(char));
    for (int i = 0; i < length; i++) result[i] = src[start + i];
    result[length] = '\0';
    return result;
}

// 05 生成数据行
/* 生成一行用于Excel的表达式*/
static char* genereateRow (char* srcLine, LinkList0 *currLink) 
{
    LinkList0 *currNode = currLink;
    char *result = malloc((strlen(srcLine) + currLink->count) * sizeof(char));
    strcpy(result, "");
    int  counter = 0;

    // showNodes(currLink);
    while (currNode != NULL) 
    {
        // Load in 
        char *src = substring(srcLine, currNode->start, currNode->length);
        strcat(result, src);
        strcat(result, ",");
        free(src);
        currNode = currNode->next;
    }
    strcat(result, "\0");

    // printf("\nAnswer is: %s", result);
    return result;
}

// 06 释放链表内存
/* 释放掉一个列表所占用的内存*/
static void freeNodes (LinkList0 *currLink)
{
    LinkList0 *currnode = currLink;
    while (currnode != NULL) 
    {
        LinkList0 *tempnode = currnode;
        currnode = currnode->next;
        free(tempnode);
    }
}

// 07 展示nodes
/* 展示nodes*/
static void showNodes (LinkList0 *head) 
{
    LinkList0 *currnode = head;
    while (currnode != NULL) 
    {
        printf("%d, %d\n", currnode->start, currnode->length);
        currnode = currnode->next;
    }
}

// 08 写入Csv
/* 分配内存生成数据行并写入数据行中*/
static void writeCSV (char *srcText, char* command) 
{
    FILE *fpe = NULL;
    fpe = fopen("result.csv", "w");

    // 分配内存给 lines, numline, 命令链表
    int  *numLine = malloc(1 * sizeof(int));
    char **lines  = malloc((strlen(srcText) + 1) * sizeof(char));
    LinkList0 *commandLink = cmdLink(command, ";");
    // printf()

    // 分割为每行
    split(srcText, "\n", lines, numLine);
    // printf("Lines: %d\n", *numLine);
    
    // 根据命令分割每行导入到Excel中
    for (int count = 0; count < *numLine; count++) 
    {
        char* tmpStr = lines[count];
        char* infoString = genereateRow(tmpStr, commandLink);
        printf("\nInfo is: %s", infoString);
        fprintf(fpe, "%s\n", infoString);
        free(infoString);
    }

    freeNodes(commandLink);
    free(numLine);
    free(lines);

	fclose(fpe);
}

// Main Function
//////////////////////////////////////////////////////////////////////////////////////

int main()
{   
    char fname[100];
	printf("Enter the name of the file (100 digits): ");
	scanf("%s",&fname);

    char command[100];
    printf("Enter the command (100 digits): ");
    scanf("%s", &command);
    
    char *source = openTXT(fname);
    writeCSV(source, command);
    free(source);
	return 0;
}

