#include"head.h"

vector<int> LINE;//�к�ӳ��
int CUR_LINE = 0;//��ǰ����
int break_flag = 0;//�Ƿ�����break
int firstline_flag = 1;//�Ƿ���Ҫ��ӡ�ĵ�һ���к�
FILE *PFILE = NULL;

int main(void) {
	PreDeal();//ִ��Ԥ�������дʷ������������к�ӳ��
	PFILE = fopen(".tmp", "r");//Ԥ����õ��м���ʱ�ļ�
	if (!PFILE) {
		cout << "�ļ���ʧ�ܣ�" << endl;
		exit(-1);
	}
	HEAD_SCOPE = (SCOPE)malloc(sizeof(SCOPE_NODE));//������ͷ���
	CUR_SCOPE = HEAD_SCOPE;
	CUR_SCOPE->next = NULL;
	CUR_SCOPE->prior = NULL;
	CUR_SCOPE->var = NULL;

	Push_Scope("Order");//��ǰ������˳��ṹ��ջ
	Order();//ִ��˳��ṹ����

	fclose(PFILE);
	fclose(pf_out);
	return 0;
}



