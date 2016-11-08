#include"head.h"

const char KEYMAP[KEYNUM][KEYLEN] = {
	"int",
	"if",
	"for",
	"while",
	"do",
	"printf",
	"{",
	"}",
	"break",
	";"
};
int PRINTED_LINE = 0;
FILE *pf_out = fopen("output.txt", "w+");

inline void PrintLine(void) {
	if (LINE[CUR_LINE] == PRINTED_LINE)//�����ظ���ӡͬһ�е�����к�
		return;
	if (firstline_flag) {
		firstline_flag = 0;
		fprintf(pf_out, "%d" ,LINE[CUR_LINE]);//��ӡ�к�
	}
	else {
		fprintf(pf_out, " %d", LINE[CUR_LINE]);//��ӡ�к�
	}
	PRINTED_LINE = LINE[CUR_LINE];
}

void Order(void) {
	char keyword[KEYLEN];//�ؼ���
	char line[MAXLEN + 1];//��ȡ����
	char *pline = NULL;//ָʾ���ж����ĵ���λ��
	int block_flag = 0;

	while (fgets(line, MAXLEN + 1, PFILE)) {
		CUR_LINE++;
		if (sscanf(line,"%s",keyword)==-1)continue;
		pline = line;
		int i;
		for (i = 1;i <= KEYNUM;i++) {
			if (!strcmp(keyword, KEYMAP[i - 1]))
				break;
		}
		switch (i) {
		case 1: {//int
			int j = 0;
			int var_flag = 0;
			int print_flag = 0;//��ӡ�кŵı��
			int var_value = 0;//Ĭ�ϳ�ʼ��0
			char var_name[KEYLEN];
			char word[KEYLEN];//���ڵĿո�ֿ��ĵ���
			while (1) {
				sscanf(pline, "%s", word);
				pline += strlen(word) + 1;
				j++;
				if (j == 1)continue;//����int
				if (word[0] == ';' || word[0] == ',') {//һ����������
					if (var_flag == 1) {
						Push_Var(var_name, 0);//����Ĭ�ϳ�ʼ��0
						var_flag = 0;
					}
					if (word[0] == ';')break;
					continue;
				}
				if (word[0] == '=') {
					if (!print_flag) {
						PrintLine();//��ӡ�к�
						print_flag = 1;
					}
					char operation[MAXLEN];
					int k = 0;
					while (pline[0] != ','&&pline[0] != ';') {
						operation[k] = pline[0];
						pline++;
						k++;
					}
					operation[k] = '\0';
					j++;
					var_value = Calculator(operation);//���ü�����������ʽ��ֵ
					Push_Var(var_name, var_value);
					var_flag = 0;
					continue;
				}
				strcpy(var_name, word);
				var_flag = 1;
			}
			break;
		}
		case 2: {//if
			int len = strlen(line);
			fseek(PFILE, -1 * len - 1, 1);
			CUR_LINE--;
			Push_Scope("Branch");//��֧�������ջ
			Branch();
			Pop_Scope();//��֧�������ջ
			if (break_flag) {//���ǰһ����ͨ��break�������Ƿ�֧��
				return;
			}
			break;
		}
		case 3://for
		case 4://while
		case 5: {//do
			int len = strlen(line);
			fseek(PFILE, -1 * len - 1, 1);
			CUR_LINE--;
			Push_Scope("Loop");//ѭ���������ջ
			Loop();
			Pop_Scope();//ѭ���������ջ
			break_flag = 0;
			break;
		}
		case 6: {//printf
			PrintLine();//��ӡ�к�
			char word[KEYLEN];
			char expression[MAXLEN];
			sscanf(pline, "%s", word);//printf
			pline += strlen(word) + 1;
			sscanf(pline, "%s", word);//(
			pline += strlen(word) + 1;

			int quot_flag = 0;
			int j = 0;
			char prior = '\0';
			while (quot_flag != 2) {
				if (pline[j] == '"'&&prior != '\\') {
					quot_flag++;
				}
				prior = pline[j];
				j++;
			}
			pline += j + 1;
			while (1) {
				sscanf(pline, "%s", word);
				pline += strlen(word) + 1;
				if (word[0] == ')')break;
				if (word[0] == ',') {
					int k = 0;
					while (pline[0] != ','&&pline[0] != ')') {
						expression[k] = pline[0];
						pline++;
						k++;
					}
					expression[k] = '\0';
					Calculator(expression);
				}
			}
			break;
		}
		case 7: {//�������
			block_flag = 1;
			break;
		}
		case 8: {//�Ҵ�����
			return;
		}
		case 9: {//break
			PrintLine();
			break_flag = 1;
			return;
		}
		case 10: {//�����
			break;
		}
		default: {//������
			char var_name[KEYLEN];
			char word[KEYLEN];
			int var_value;
			PrintLine();//��ӡ�к�
			while (1) {
				sscanf(pline, "%s", word);
				pline += strlen(word) + 1;
				if (word[0] == ';')break;
				if (word[0] == '=') {
					char operation[MAXLEN];
					int k = 0;
					while (pline[0] != ','&&pline[0] != ';') {
						operation[k] = pline[0];
						pline++;
						k++;
					}
					operation[k] = '\0';
					var_value = Calculator(operation);//ʹ�ü������Ա��ʽ��ֵ
					Set_Var(var_name, var_value);
				}
				else if (word[0] == '+') {
					int tmp = Get_Var(var_name);
					tmp++;
					Set_Var(var_name, tmp);
				}
				else if (word[0] == '-') {
					int tmp = Get_Var(var_name);
					tmp--;
					Set_Var(var_name, tmp);
				}
				else if (word[0] == ',')continue;
				else {
					strcpy(var_name, word);
				}
			}
			break;
		}
		}//switch
		if (!block_flag)//����ֻ��һ�����
			break;
	}//while
}
void Branch(void)
{
	typedef struct END
	{
		long end_line;
		int end_line_num;
	}END;
	char line[MAXLEN + 1];//�洢�ļ�һ������
	char word[MAXLEN + 1];//�洢����
	char exp[MAXLEN + 1];//�洢if������else if���������ڵı��ʽ
	int ret_exp;//�洢if������else if���������ڵı��ʽ�ķ���ֵ
	long start_line = ftell(PFILE);//�����ļ�ָ�뿪ʼ��λ��
	int start_line_num = CUR_LINE;//���浱ǰ��
	fgets(line, MAXLEN + 1, PFILE);
	CUR_LINE++;
	int has_brace = 0;//��������������û�д����ŵ����

	queue<END> end;

	int brace = 0;//'{'��'}'�ĸ���
	int i = 0;

	//��while(1)ѭ�������֧�ṹ�����λ��
	while (1)
	{
		while (fgets(line, MAXLEN + 1, PFILE)) //lineָʾ��������CUR_LINE�ȵ�������һ��
		{
			CUR_LINE++;
			if (sscanf(line, "%s", word) == -1)//��������
			{
				continue;
			}
			if (word[0] == '{')//�����������braceֵ��1
			{
				has_brace = 1;
				brace++;
			}
			else if (word[0] == '}') //�����Ҵ�����braceֵ��1
			{
				brace--;
				if (brace == 0)//���Ҵ�������ƥ��ģ�ֻҪbraceΪ0��˵���������Ҵ����ž���ƥ�䣬����Ϊ��ǰ�������
				{
					has_brace = 0;
					END tmp;
					tmp.end_line = ftell(PFILE);//��¼�ļ�ָ��ָ��'}'������
					tmp.end_line_num = CUR_LINE;//��¼'}'�к�
					end.push(tmp);//���������м�¼����У�Ϊ��ǰ�������λ����Ϣ
					break;
				}
			}
			if (has_brace == 0)//����û�����Ҵ����ŵ����������if�����������ֻ����һ�У���¼���������ļ�ָ��ָ����е���Ϣ
			{
				END tmp;
				tmp.end_line = ftell(PFILE);
				tmp.end_line_num = CUR_LINE;
				end.push(tmp);
				break;
			}
		}
		while (1)
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", word) != -1 || feof(PFILE))//���ǿ��м�break���ǿ��������������һ�У�����ʵ�����������У����������ļ�β����
				break;
		}
		if (strcmp(word, "else"))//����else����else if��������ѭ����˵����ǰ��֧�������
			break;
		i++;
	}//����������ع��if-else���жϴ���

	 /*END tmp = end.back();
	 int end_all_branch_line = tmp.end_line_num + 1;//ȡ��������֧�ṹ�Ľ�������һ��
	 long end_all_branch_fileline = tmp.end_line;
	 end.push(tmp);*/

	fseek(PFILE, start_line, 0);//�ض����ļ�ָ��
	CUR_LINE = start_line_num;//�ض���ǰ�У�ָ��ͷ��if���
	while (1)//�жϽ������������
	{
		fgets(line, MAXLEN + 1, PFILE);
		CUR_LINE++;

		if (sscanf(line, "%s", word) == -1)
		{
			continue;
		}

		//������ʽȡ�������ڵı��ʽ����if()��else if()�����һ����ȡ�����ʽ��else��ȡ�������ʽ��sscanf����0
		if (sscanf(line, "%*[^(](%[^)]", exp) != 0)
		{
			PrintLine();
			ret_exp = Calculator(exp);
		}
		else
		{
			ret_exp = 1;
		}
		if (ret_exp)
		{
			Order();
			if (break_flag)
			{
				END tmp = end.back();
				fseek(PFILE, tmp.end_line, 0);
				CUR_LINE = tmp.end_line_num;
				return;
			}
			END tmp = end.back();//���ض���β��Ԫ��
			fseek(PFILE, tmp.end_line, 0);
			CUR_LINE = tmp.end_line_num;
			return;
		}
		else
		{
			END tmp = end.front();//���ض���ͷ��Ԫ��
			end.pop();//��ͷ����
			fseek(PFILE, tmp.end_line, 0);
			CUR_LINE = tmp.end_line_num;
			if (end.empty()) //����Ϊ�գ����Ѱ�if-else���еı�־��������������������
			{
				return;
			}
		}
	}
}

void Loop(void)
{
	char line[MAXLEN + 1];
	long filePosition = ftell(PFILE);
	char keyword[KEYLEN];

	fgets(line, MAXLEN + 1, PFILE);
	sscanf(line, "%s", keyword);
	fseek(PFILE, filePosition, 0);

	if (!strcmp(keyword, "for"))
	{
		ForFunction();
	}
	else if (!strcmp(keyword, "do"))
	{
		DoFunction();
	}
	else if (!strcmp(keyword, "while"))
	{
		WhileFunction();
	}
	else
	{
		printf("ִ���ж�ѭ���ؼ��ʳ���\n");
		exit(-1);
	}

}


void ForFunction(void)
{
	CUR_LINE++;

	char expression[MAXLEN]; //���ʽ
	char valueName[KEYLEN]; //��������
	char keyword[KEYLEN];//�ؼ���
	char line[MAXLEN + 1];//��ȡ����
	char *pline = NULL;//ָʾ���ж����ĵ���λ��
	int startLine = CUR_LINE;  //��for������������һ��
	int endLine;
	int i = 0;   //��ʱ����
	bool isTheFirstFor = true;
	bool isOnce = IsOnce();	   //�Ƿ�ִֻ��һ��
	long firstPosition = ftell(PFILE);
	long endPosition;

	//�ж��Ƿ�������ֻ��һ�Σ��Ӷ�ȷ���������к�
	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //����for������������һ��
		fgets(line, MAXLEN + 1, PFILE);  //����for��������������һ��
		CUR_LINE++;
		while (sscanf(line, "%s", keyword) == -1)	 //��������
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
		}
		endPosition = ftell(PFILE);
		endLine = CUR_LINE;
	}
	else
	{
		int brace = 0;//����������
		fgets(line, MAXLEN + 1, PFILE);  //�� for(...)��һ��
		CUR_LINE++;
		while (1) {
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", keyword) == -1)	  //�����հ���
				continue;
			if (keyword[0] == '{') {
				brace++;
			}
			if (keyword[0] == '}') {
				brace--;
				if (brace == 0) {
					endPosition = ftell(PFILE);
					endLine = CUR_LINE - 1;
					break;
				}
			} //if
		}//while
	}



	//forѭ��
	while (1)
	{
		fseek(PFILE, firstPosition, 0);   //�ҵ���for������������һ�� 
		fgets(line, MAXLEN + 1, PFILE);	 //����for������������һ�� 
		CUR_LINE = startLine;

		PrintLine();	    //���forѭ�������к�

							//�����һ��������֮ǰ�Ķ���
		if (isTheFirstFor == true)
		{
			isTheFirstFor = false; //��ֹ��һ��ִ�е�һ��������֮ǰ�Ĳ���

			pline = line;
			while (*pline++ != '(');   //������plineָ�򡮣���֮��ĵ�һ���ַ�
			while (1)	//�����һ��������֮ǰ�ı��ʽ
			{
				sscanf(pline, "%s", keyword);
				if (!strcmp(keyword, KEYMAP[0])) //��һ���ǡ�int��
				{

					pline = pline + strlen(keyword) + 1;   //������int��
					sscanf(pline, "%s", valueName);       //������
					pline = pline + strlen(valueName) + 1;//����������
					sscanf(pline, "%s", keyword);         //��=�� ���� ������ ���ߡ�����
					pline = pline + strlen(keyword) + 1;
					if (!strcmp(keyword, "=")) //��ֵ
					{
						pline++;

						i = 0;
						while (*pline != ';' && *pline != ',')
						{
							expression[i++] = *pline++;
						}
						expression[i] = '\0';
						Push_Var(valueName, Calculator(expression));
						if (*pline == ';')	 //������
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //������һ�����
						{
							pline++;
							continue;
						}
					}
					else if (!strcmp(keyword, ";"))
					{
						pline++;
						break;
					}
				}
				else    //��һ���Ǳ�����
				{
					strcpy(valueName, keyword);  //��ȡ������
					pline = pline + strlen(valueName) + 1;//����������
					sscanf(pline, "%s", keyword);  //��ȡ��=�����߲��������
					if (!strcmp(keyword, "="))  //��������һ����ֵ���
					{
						pline = pline + strlen(keyword) + 1;
						i = 0;
						while (*pline != ';' && *pline != ',')
						{
							expression[i++] = *pline++;
						}
						expression[i] = '\0';
						Set_Var(valueName, Calculator(expression));
						if (*pline == ';')	 //������
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //������һ�����
						{
							pline++;
							continue;
						}

					}
					else  //��仰��һ������ʽ
					{
						pline = pline - strlen(valueName) - 1;
						i = 0;
						while (*pline != ';' && *pline != ',')
						{
							expression[i++] = *pline++;
						}
						expression[i] = '\0';
						Calculator(expression);
					}
					if (*pline == ';')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}
				}
			}//while����
		}
		else   //���ǵ�һ�ν���forѭ��
		{
			pline = line;
			while (*pline++ != ';');   //������plineָ��;��֮��ĵ�һ���ַ�
		} //if����

		  //�����һ�����������ڶ���������֮��Ķ���
		i = 0;
		while (*pline != ';')  //ȡ���м�ı��ʽ
		{
			expression[i++] = *pline++;
		}
		expression[i] = '\0';
		for (i = 0; expression[i] != '\0'; i++)	   //�жϡ�for(;;)�������(�м��ж�����ǿյ�)
		{
			if (expression[i] == ' ') continue;
			else break;
		}
		if (expression[i] == '\0' && expression[i - 1] == ' ')
		{
			strcpy(expression, "1");
		}
		if (Calculator(expression) == 1)	//����ѭ����
		{
			fseek(PFILE, firstPosition, 0);   //�ҵ���for������������һ��
			fgets(line, MAXLEN + 1, PFILE);  //����for������������һ��
			if (isOnce == false)
			{
				while (line[0] != '{') //�ҵ�ѭ�������ڵ�������
				{
					CUR_LINE = CUR_LINE + 1;
					fgets(line, MAXLEN + 1, PFILE);
				}
				int len = strlen(line);
				fseek(PFILE, -1 * len - 1, 1);
				CUR_LINE--;
			}

			Push_Scope("Order");
			//˳����
			Order();
			Pop_Scope();//ѭ���������ջ
			if (break_flag) {
				CUR_LINE = endLine;
				fseek(PFILE, firstPosition, 0);   //�ҵ���for������������һ��
				for (i = 0; i < endLine - startLine + 1; i++)	  //����forѭ������󡣡�}��֮��
				{
					fgets(line, MAXLEN + 1, PFILE);
				}
				return;
			}
			//�ص���for�������������
			CUR_LINE = startLine;
			fseek(PFILE, firstPosition, 0);  //�ҵ���for������������һ��
			fgets(line, MAXLEN + 1, PFILE);	 //����for������������һ��
			pline = line;
			while (*pline++ != ';');   //������plineָ���һ����;��֮��ĵ�һ���ַ�
			while (*pline++ != ';');   //������plineָ��ڶ�����;��֮��ĵ�һ���ַ�
		}
		else
		{
			CUR_LINE = endLine;
			fseek(PFILE, endPosition, 0);   //�ҵ���for������������һ��
			return;
		}

		//����ڶ�����������������������֮��Ķ���
		while (1)
		{
			sscanf(pline, "%s", keyword);
			if (!strcmp(keyword, KEYMAP[0])) //��һ���ǡ�int��
			{
				pline = pline + strlen(keyword) + 1;   //������int��
				sscanf(pline, "%s", valueName);       //������
				pline = pline + strlen(valueName) + 1;//����������
				sscanf(pline, "%s", keyword);         //��=�� ���� ������ ���ߡ�����
				pline = pline + strlen(keyword) + 1;
				if (!strcmp(keyword, "=")) //��ֵ
				{
					pline++;
					i = 0;
					while (*pline != ';' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Push_Var(valueName, Calculator(expression));
					if (*pline == ';')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}
				}
			}
			else    //��һ���Ǳ�����
			{
				strcpy(valueName, keyword);  //��ȡ������
				pline = pline + strlen(valueName) + 1;//����������
				sscanf(pline, "%s", keyword);  //��ȡ��=�����߲��������
				if (!strcmp(keyword, "="))  //��������һ����ֵ���
				{
					pline = pline + strlen(keyword) + 1;
					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Set_Var(valueName, Calculator(expression));
					if (*pline == ')')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}

				}
				else  //��仰��һ������ʽ
				{
					pline = pline - strlen(valueName) - 1;
					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Calculator(expression);
				}
				if (*pline == ')')	 //������
				{
					pline++;
					break;
				}
				else if (*pline == ',')	  //������һ�����
				{
					pline++;
					continue;
				}
			}
		}//while����
	}//while����
}


void WhileFunction(void)
{
	CUR_LINE++;

	char expression[MAXLEN]; //���ʽ
	char valueName[KEYLEN]; //��������
	char keyword[KEYLEN];//�ؼ���
	char line[MAXLEN + 1];//��ȡ����
	char *pline = NULL;//ָʾ���ж����ĵ���λ��
	int startLine = CUR_LINE;  //��while��,,..������һ��
	long startPosition = ftell(PFILE);
	long endPosition;
	int endLine;
	int i = 0;   //��ʱ����
	int whileValue;  //��¼while����ֵ
	bool isOnce = IsOnce();


	//�ж��Ƿ�������ֻ��һ�Σ��Ӷ�ȷ���������к�
	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //����for������������һ��
		fgets(line, MAXLEN + 1, PFILE);  //����for��������������һ��
		CUR_LINE++;
		while (sscanf(line, "%s", keyword) == -1)	 //��������
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
		}
		endPosition = ftell(PFILE);
		endLine = CUR_LINE;
	}
	else
	{
		int brace = 0;//����������
		fgets(line, MAXLEN + 1, PFILE);  //�� while(...)��һ��
		CUR_LINE++;
		while (1) {
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", keyword) == -1)	  //�����հ���
				continue;
			if (keyword[0] == '{') {
				brace++;
			}
			if (keyword[0] == '}') {
				brace--;
				if (brace == 0) {
					endPosition = ftell(PFILE);
					endLine = CUR_LINE - 1;
					break;
				}
			} //if
		}//while
	}//else


	 //����whileѭ��
	while (1)
	{
		fseek(PFILE, startPosition, 0);   //�ҵ���while��...������һ�� 
		fgets(line, MAXLEN + 1, PFILE);	 //����while��...������һ�� 
		CUR_LINE = startLine;

		PrintLine();  //���whileѭ�������к�

		pline = line;
		while (*pline++ != '(');   //������plineָ�򡮣���֮��ĵ�һ���ַ�
		while (1)	//��������֮ǰ�ı��ʽ
		{
			sscanf(pline, "%s", keyword);
			if (!strcmp(keyword, KEYMAP[0])) //��һ���ǡ�int��
			{

				pline = pline + strlen(keyword) + 1;   //������int��
				sscanf(pline, "%s", valueName);       //������
				pline = pline + strlen(valueName) + 1;//����������
				sscanf(pline, "%s", keyword);         //��=�� ���� ������ ���ߡ�����
				pline = pline + strlen(keyword) + 1;
				if (!strcmp(keyword, "=")) //��ֵ
				{
					pline++;

					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Push_Var(valueName, Calculator(expression));
					if (*pline == ')')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}
				}
			}
			else    //��һ���Ǳ�����
			{
				strcpy(valueName, keyword);  //��ȡ������
				pline = pline + strlen(valueName) + 1;//����������
				sscanf(pline, "%s", keyword);  //��ȡ��=�����߲��������
				if (!strcmp(keyword, "="))  //��������һ����ֵ���
				{
					pline = pline + strlen(keyword) + 1;
					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					whileValue = Calculator(expression);
					Set_Var(valueName, whileValue);
					if (*pline == ')')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}

				}
				else  //��仰��һ������ʽ
				{
					pline = pline - strlen(valueName) - 1;
					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					whileValue = Calculator(expression);
				}
				if (*pline == ')')	 //������
				{
					pline++;
					break;
				}
				else if (*pline == ',')	  //������һ�����
				{
					pline++;
					continue;
				}
			}
		}//while����

		 //����ѭ����
		if (whileValue != 0)
		{
			fseek(PFILE, startPosition, 0);   //�ҵ���while��...������һ��
			fgets(line, MAXLEN + 1, PFILE);  //����{����һ��

			if (isOnce == false)
			{
				while (line[0] != '{') //�ҵ�ѭ�������ڵ�������
				{
					CUR_LINE = CUR_LINE + 1;
					fgets(line, MAXLEN + 1, PFILE);
				}
				int len = strlen(line);
				fseek(PFILE, -1 * len - 1, 1);
				CUR_LINE--;
			}

			Push_Scope("Order");
			//˳����
			Order();
			Pop_Scope();//ѭ���������ջ
			if (break_flag) {
				CUR_LINE = endLine;
				fseek(PFILE, startPosition, 0);   //�ҵ���while��...������һ��
				for (i = 0; i < endLine - startLine + 1; i++)	  //����whileѭ������󡣡�}��֮��
				{
					fgets(line, MAXLEN + 1, PFILE);
				}
				return;
			}
		}
		else  //����ѭ��
		{
			CUR_LINE = endLine;
			fseek(PFILE, endPosition, 0);   //�ҵ�������
			return;
		}
	}
}


void DoFunction(void)
{
	char *pline;
	char valueName[KEYLEN];	//��������
	int i = 0;	//��ʱ����
	char line[MAXLEN + 1];
	char exp[MAXLEN + 1];
	int ret_exp = 1;
	char word[KEYLEN];	//�ؼ���
	long start_line = ftell(PFILE);
	int start_line_num = CUR_LINE;
	long end_line;
	int end_line_num;
	int brace = 0;//����������
	bool isOnce = IsOnce();


	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //����do����һ��
		fgets(line, MAXLEN + 1, PFILE);  //������������������һ��
		CUR_LINE++;
		while (sscanf(line, "%s", word) == -1)	 //��������
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
		}
		end_line = ftell(PFILE);
		end_line_num = CUR_LINE;
	}
	else
	{
		fgets(line, MAXLEN + 1, PFILE);
		CUR_LINE++;
		while (1) {
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", word) == -1)
				continue;
			if (word[0] == '{') {
				brace++;
			}
			if (word[0] == '}') {
				brace--;
				if (brace == 0) {
					end_line = ftell(PFILE);
					end_line_num = CUR_LINE;
					break;
				}
			}
		}
	}

	fseek(PFILE, start_line, 0);
	CUR_LINE = start_line_num;
	while (1) {
		fgets(line, MAXLEN + 1, PFILE);
		CUR_LINE++;
		if (ret_exp) {
			Push_Scope("Order");
			//˳����
			Order();
			Pop_Scope();//ѭ���������ջ
			if (break_flag) {
				fseek(PFILE, end_line, 0);
				CUR_LINE = end_line_num;
				while (1) {
					fgets(line, MAXLEN + 1, PFILE);
					CUR_LINE++;
					if (sscanf(line, "%s", word) != -1)
						break;
				}
				return;
			}
			while (1) {
				fgets(line, MAXLEN + 1, PFILE);
				CUR_LINE++;
				if (sscanf(line, "%s", word) != -1)
					break;
			}
			PrintLine(); //��ӡ�к�

			pline = line;
			while (*pline++ != '(');   //������plineָ�򡮣���֮��ĵ�һ���ַ�
			while (1)	//��������֮ǰ�ı��ʽ
			{
				sscanf(pline, "%s", word);
				if (!strcmp(word, KEYMAP[0])) //��һ���ǡ�int��
				{

					pline = pline + strlen(word) + 1;   //������int��
					sscanf(pline, "%s", valueName);       //������
					pline = pline + strlen(valueName) + 1;//����������
					sscanf(pline, "%s", word);         //��=�� ���� ������ ���ߡ�����
					pline = pline + strlen(word) + 1;
					if (!strcmp(word, "=")) //��ֵ
					{
						pline++;

						i = 0;
						while (*pline != ')' && *pline != ',')
						{
							exp[i++] = *pline++;
						}
						exp[i] = '\0';
						Push_Var(valueName, Calculator(exp));
						if (*pline == ')')	 //������
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //������һ�����
						{
							pline++;
							continue;
						}
					}
				}
				else    //��һ���Ǳ�����
				{
					strcpy(valueName, word);  //��ȡ������
					pline = pline + strlen(valueName) + 1;//����������
					sscanf(pline, "%s", word);  //��ȡ��=�����߲��������
					if (!strcmp(word, "="))  //��������һ����ֵ���
					{
						pline = pline + strlen(word) + 1;
						i = 0;
						while (*pline != ')' && *pline != ',')
						{
							exp[i++] = *pline++;
						}
						exp[i] = '\0';
						ret_exp = Calculator(exp);
						Set_Var(valueName, ret_exp);
						if (*pline == ')')	 //������
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //������һ�����
						{
							pline++;
							continue;
						}

					}
					else  //��仰��һ������ʽ
					{
						pline = pline - strlen(valueName) - 1;
						i = 0;
						while (*pline != ')' && *pline != ',')
						{
							exp[i++] = *pline++;
						}
						exp[i] = '\0';
						ret_exp = Calculator(exp);
					}
					if (*pline == ')')	 //������
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //������һ�����
					{
						pline++;
						continue;
					}
				}
			}//while����

			fseek(PFILE, start_line, 0);
			CUR_LINE = start_line_num;
		}
		else {
			fseek(PFILE, end_line, 0);
			CUR_LINE = end_line_num;
			while (1) {
				fgets(line, MAXLEN + 1, PFILE);
				CUR_LINE++;
				if (sscanf(line, "%s", word) != -1)
					break;
			}
			return;

		}
	}
}


bool IsOnce(void)
{
	char line[MAXLEN + 1];
	char keyword[KEYLEN];
	int  startLine = CUR_LINE;
	long startPosition = ftell(PFILE);
	bool isOnce;

	fgets(line, MAXLEN + 1, PFILE);	  //�� ��for����do����while����һ��
	fgets(line, MAXLEN + 1, PFILE);	  //�� ��for����do����while������һ��
	while (sscanf(line, "%s", keyword) == -1)	 //��������
	{
		fgets(line, MAXLEN + 1, PFILE);	  //�� ��for����do����while������һ��
	}
	if (!strcmp(keyword, "{"))	 //��һ���ַ��ǡ�{��
	{
		isOnce = false;
	}
	else
	{
		isOnce = true;
	}

	//������ָ��ص���ʼλ��
	CUR_LINE = startLine;
	fseek(PFILE, startPosition, 0);

	return isOnce;
}
