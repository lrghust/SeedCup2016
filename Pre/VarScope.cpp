#include"head.h"
SCOPE HEAD_SCOPE = NULL;//作用域链头结点指针
SCOPE CUR_SCOPE = NULL;//当前作用域/栈顶指针
void Push_Scope(char *scope_name) {//作用域入栈
	CUR_SCOPE->next = (SCOPE)malloc(sizeof(SCOPE_NODE));
	CUR_SCOPE->next->prior = CUR_SCOPE;
	CUR_SCOPE = CUR_SCOPE->next;
	strcpy(CUR_SCOPE->name , scope_name);
	CUR_SCOPE->var = (VAR_TREE)malloc(sizeof(VAR_NODE));
	CUR_SCOPE->var->next = NULL;
	CUR_SCOPE->next = NULL;
}
void Pop_Scope(void) {//作用域出栈
	VAR_TREE p= CUR_SCOPE->var;
	VAR_TREE q = p;
	CUR_SCOPE = CUR_SCOPE->prior;
	free(CUR_SCOPE->next);
	CUR_SCOPE->next = NULL;
	q = q->next;
	while (q) {
		free(p);
		p = q;
		q = q->next;
	}
	free(p);
	p = q;
}
void Push_Var(char *var_name, int var_value) {//保存新变量
	VAR_TREE p = CUR_SCOPE->var;
	while (p->next)
		p = p->next;
	p->next = (VAR_TREE)malloc(sizeof(VAR_NODE));
	p = p->next;
	strcpy(p->name , var_name);
	p->value = var_value;
	p->next = NULL;
}
int Get_Var(char *var_name) {//获取变量值
	SCOPE pscope = CUR_SCOPE;
	VAR_TREE pvar = CUR_SCOPE->var;
	while (1) {
		while (pvar->next) {
			pvar = pvar->next;
			if (!strcmp(pvar->name , var_name))
				return pvar->value;
		}
		pscope = pscope->prior;
		if (pscope->prior == NULL)return 0;
		pvar = pscope->var;
	}
}
void Set_Var(char *var_name, int var_value) {//改变变量值
	SCOPE pscope = CUR_SCOPE;
	VAR_TREE pvar = CUR_SCOPE->var;
	while (1) {
		while (pvar->next) {
			pvar = pvar->next;
			if (!strcmp(pvar->name, var_name)) {
				pvar->value = var_value;
				return;
			}
		}
		pscope = pscope->prior;
		if (pscope->prior == NULL)break;
		pvar = pscope->var;
	}
}