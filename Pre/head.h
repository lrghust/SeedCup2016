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

extern vector<int> LINE;//�к�ӳ��
extern SCOPE HEAD_SCOPE;
extern SCOPE CUR_SCOPE;
extern int CUR_LINE;
extern int PRINTED_LINE;
extern int break_flag;
extern int firstline_flag;
extern FILE *PFILE;
extern FILE *pf_out;
extern const char KEYMAP[KEYNUM][KEYLEN];

int Calculator(char *string_in);/*������ʵ���ܽӿڣ����ܱ��ʽ�����ر��ʽ��ֵ*/
void Exp_Input(char *string_in, char(*out)[SIZE]);
int Exp_Output(char *string_out);/*���ر��ʽ�ļ�����*/
void Push_Input(char c);        /*���ʽ��ջ����*/
char Pop_Input(void);	        /*���ʽ��ջ����*/
void Push_Output(int n);        /*�沨��ʽ��ջ����*/
int Pop_Output(void);           /*�沨��ʽ��ջ����*/
void ConvertToTarStr(char string_in[SIZE], char(*tar_string)[SIZE]);/*�������ַ���ת��Ϊ��׼�ַ���*/

void Push_Scope(char *scope_name);//��������ջ
void Pop_Scope(void);//�������ջ
void Push_Var(char *var_name, int var_value);//�����±���
int Get_Var(char *var_name);//��ȡ����ֵ
void Set_Var(char *var_name, int var_value);//�ı����ֵ

void ForFunction(void);//forѭ��
void WhileFunction(void);//while
void DoFunction(void);//do
bool IsOnce(void);

extern inline void PrintLine(void);//��ӡ�к�
void PreDeal(void);//Ԥ����
void Order(void);//˳��ṹ
void Branch(void);//��֧�ṹ
void Loop(void);//ѭ���ṹ

#endif // !_HEAD_H_