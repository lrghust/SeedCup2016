#ifndef _HEAD_H_
#define _HEAD_H_

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<cstring>
#include<vector>
#include<queue>
#include<cctype>
#define SIZE 100
#define MAXLEN 255
#define KEYNUM 10
#define KEYLEN 30
using namespace std;

typedef struct VAR_NODE {
	char name[KEYLEN];
	int value;
	struct VAR_NODE *next;
}VAR_NODE, *VAR_TREE;

typedef struct SCOPE_NODE {
	char name[KEYLEN];
	VAR_TREE var;
	struct SCOPE_NODE *prior;
	struct SCOPE_NODE *next;
}SCOPE_NODE, *SCOPE;

extern vector<int> LINE;//行号映射
extern SCOPE HEAD_SCOPE;
extern SCOPE CUR_SCOPE;
extern int CUR_LINE;
extern int PRINTED_LINE;
extern int break_flag;
extern int firstline_flag;
extern FILE *PFILE;
extern FILE *pf_out;
extern const char KEYMAP[KEYNUM][KEYLEN];

int Calculator(char *string_in);/*计算器实现总接口，接受表达式，返回表达式的值*/
void Exp_Input(char *string_in, char(*out)[SIZE]);
int Exp_Output(char *string_out);/*返回表达式的计算结果*/
void Push_Input(char c);        /*表达式入栈操作*/
char Pop_Input(void);	        /*表达式出栈操作*/
void Push_Output(int n);        /*逆波兰式人栈操作*/
int Pop_Output(void);           /*逆波兰式出栈操作*/
void ConvertToTarStr(char string_in[SIZE], char(*tar_string)[SIZE]);/*将输入字符串转换为标准字符串*/

void Push_Scope(char *scope_name);//作用域入栈
void Pop_Scope(void);//作用域出栈
void Push_Var(char *var_name, int var_value);//保存新变量
int Get_Var(char *var_name);//获取变量值
void Set_Var(char *var_name, int var_value);//改变变量值

void ForFunction(void);//for循环
void WhileFunction(void);//while
void DoFunction(void);//do
bool IsOnce(void);

extern inline void PrintLine(void);//打印行号
void PreDeal(void);//预处理
void Order(void);//顺序结构
void Branch(void);//分支结构
void Loop(void);//循环结构

#endif // !_HEAD_H_