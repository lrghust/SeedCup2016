#include"head.h"

vector<int> LINE;//行号映射
int CUR_LINE = 0;//当前行数
int break_flag = 0;//是否遇到break
int firstline_flag = 1;//是否是要打印的第一个行号
FILE *PFILE = NULL;

int main(void) {
	PreDeal();//执行预处理，进行词法分析，设置行号映射
	PFILE = fopen(".tmp", "r");//预处理得到中间临时文件
	if (!PFILE) {
		cout << "文件打开失败！" << endl;
		exit(-1);
	}
	HEAD_SCOPE = (SCOPE)malloc(sizeof(SCOPE_NODE));//作用域头结点
	CUR_SCOPE = HEAD_SCOPE;
	CUR_SCOPE->next = NULL;
	CUR_SCOPE->prior = NULL;
	CUR_SCOPE->var = NULL;

	Push_Scope("Order");//当前作用域顺序结构入栈
	Order();//执行顺序结构函数

	fclose(PFILE);
	fclose(pf_out);
	return 0;
}



