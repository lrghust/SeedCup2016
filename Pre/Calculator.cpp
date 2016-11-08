#include"head.h"

int stack_output[SIZE];         /*��������Ϊֵջʹ��*/
char stack_input[SIZE];         /*�����ջ*/
int TOP = -1;                   /*�±���Ϊջ�����ָ��*/
int LENGTH = 0;

/** \brief �����������Լ�����ʽ��ֵ������
*
* \param ���ʽ�ַ���
* \param
* \return ���ʽֵ
*
*/

int Calculator(char *string_in)
{
	TOP = -1;
	LENGTH = 0;
	char string_out[SIZE];
	Exp_Input(string_in, &string_out);
	return Exp_Output(string_out);

}

/** \brief �����沨��ʽ
*
* \param Դ�����еı��ʽ
* \param
* \return �沨��ʽ
*
*/

void Exp_Input(char *string_in, char(*out)[SIZE])
{
	char convert[SIZE];
	//char out[SIZE];                 /*����沨�����ʽ������*/
	char c;
	char *p = string_in;
	char *q = string_in;
	int flag = -1;//�����ж������Լ�
	char variable[SIZE];//���ʽ��ı�����

						/*ConvertToTarStr������ԭ���ʽת����ֻ
						�����ֺ�'+'��'-'��'*'��'/'�ı��ʽ����ʹ�ñ���ֵ�滻������*/
	ConvertToTarStr(string_in, &convert);

	/*46-85�е�forѭ����Ҫʵ�ֵĹ����ǽ���������
	���ʽ������б������������Լ�����ı����ǵ�ֵ*/
	for (;*p != '\0';p++)
	{
		if (isalpha(*p) || *p == '_')
		{
			q = p;//�洢������
			for (;isalpha(*p) || *p == '_' || *p == ' ';p++)
			{
				;
			}//��ʱpָ�뵽����var����Ŀո񣬿ո�֮����++����--
			if (*p == '\0') break;
			if (*p == '-' && *(p + 1) == '-')
			{
				flag = 0;
			}
			else if (*p == '+' && *(p + 1) == '+')
			{
				flag = 1;
			}

			if (1 == flag)
			{
				sscanf(q, "%s", variable);
				Set_Var(variable, Get_Var(variable) + 1);
				flag = -1;
			}
			else if (0 == flag)
			{
				sscanf(q, "%s", variable);
				Set_Var(variable, Get_Var(variable) - 1);
				flag = -1;
			}
		}
	}
	p = convert;//pָ��Ŀ����ʽ
	memset(*out, ' ', SIZE);/*ȫ����ʼ��Ϊ�ո�*/
	for (;*p != '\0';p++)
	{
		if (isdigit(*p))
		{
			while (isdigit(*p))
			{
				(*out)[LENGTH++] = *p++; /*�����ַ�ֱ�ӽ����沨������*/
			}
			p--;
			LENGTH++;
		}
		else if (*p == '+' || *p == '-')
		{
			while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
				|| stack_input[TOP] == '/'))
			{
				(*out)[LENGTH++] = Pop_Input();        /*��ջ*/
				LENGTH++;
			}
			Push_Input(*p); /*��ջ*/
		}
		else if (*p == '*' || *p == '/')
		{
			while (TOP >= 0 && (stack_input[TOP] == '*' || stack_input[TOP] == '/'))
			{
				(*out)[LENGTH++] = Pop_Input();
				LENGTH++;
			}
			Push_Input(*p);
		}
		else if (*p == '<')
		{
			if (*(p + 1) == '=')
			{
				p++;
				while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
					|| stack_input[TOP] == '/' || stack_input[TOP] == '<' || stack_input[TOP] == '>'
					|| stack_input[TOP] == '@' || stack_input[TOP] == '#'))
				{
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input('@');//��@���Ŵ���<=
			}
			else
			{
				while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
					|| stack_input[TOP] == '/' || stack_input[TOP] == '<' || stack_input[TOP] == '>'
					|| stack_input[TOP] == '@' || stack_input[TOP] == '#')) {
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input(*p);
			}
		}
		else if (*p == '>')
		{
			if (*(p + 1) == '=')
			{
				p++;
				while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
					|| stack_input[TOP] == '/' || stack_input[TOP] == '<' || stack_input[TOP] == '>'
					|| stack_input[TOP] == '@' || stack_input[TOP] == '#'))
				{
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input('#');//��#���Ŵ���>=
			}
			else
			{
				while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
					|| stack_input[TOP] == '/' || stack_input[TOP] == '<' || stack_input[TOP] == '>'
					|| stack_input[TOP] == '@' || stack_input[TOP] == '#')) {
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input(*p);
			}
		}
		else if (*p == '!')
		{
			if (*(p + 1) == '=')
			{
				p++;
				while (TOP >= 0)
				{
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input('$');//��$���Ŵ���!=
			}
			else
			{
				while (TOP >= 0)
				{
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input(*p);
			}
		}
		else if (*p == '=')
		{
			if (*(p + 1) == '=')
			{
				p++;
				while (TOP >= 0)
				{
					(*out)[LENGTH++] = Pop_Input();
					LENGTH++;
				}
				Push_Input('%');//��%���Ŵ���==
			}
			/*else
			{
			while(TOP>=0)
			{
			out[LENGTH++] = Pop_Input();
			LENGTH++;
			}
			Push_Input(*p);
			}*/
		}
	}
	while ((c = Pop_Input()) != '\0') {
		(*out)[LENGTH++] = c;        /*��ջ*/
		LENGTH++;
	}
	(*out)[LENGTH] = '\0';    /*�γ��沨���ַ���*/
							  //puts(out);
							  //return out;
}

/** \brief ��ԭ���ʽת����Ŀ����ʽ��������������
*
* \param ԭ���ʽ
* \param
* \return Ŀ����ʽ
*
*/

void ConvertToTarStr(char string_in[SIZE], char(*tar_string)[SIZE])
{
	char var_value_instring[SIZE];//�洢����ֵ���ַ����飨��Ҫ��int��ת����char���ͣ�
	char var[SIZE];//�洢������
				   //char tar_string[SIZE];//Ŀ�ı��ʽ
	char *p = string_in;//ԭʼ���ʽ
	char *q = *tar_string;
	char *r = var_value_instring;
	char *t;
	int mul_div_flag = 0;

	while (*p == ' ')
		p++;
	if (*p == '-')
	{
		*q++ = '0';
	}
	else {
		*q++ = '0';
		*q++ = '+';
	}
	for (;*p != '\0';)
	{
		if (isalpha(*p) || *p == '_')//������Ǳ���
		{
			sscanf(p, "%s", var);//��ȡ������
			itoa(Get_Var(var), r, 10);
			/* ָ��rָ�����ֵ���ַ�����ָ��qָ������ɵ�Ŀ���ַ�����t��¼q��λ�� */
			for (;*r != '\0';)
			{
				if (*r == '-')//������ʽ�еı����Ǹ����Ĵ�����
				{
					r++;
					while (*r == ' ')//ȥ���ո�
					{
						r++;
					}
					t = q;//*t��¼Ŀ����ʽ�ĵ�ǰ���һ���ַ�����һλ
					while (*q != '+' && *q != '-')
					{
						/* ��ѭ������������ǰ��' + '����' - '�������' * '����' / '��
						��ô' * '����' / 'ǰ��' + '����' - '������' + '��' - '����ѭ����
						��˵��һ���Ǿ�����Ŀ����ʽ��ǰ���Ĵ�����ϡ� 0 ���� 0 + ���󣬱�����ǰһ������' + '����' - ' */
						if (*q == '*' || *q == '/')
						{
							mul_div_flag = 1;
						}
						q--;
					}
					if (*q == '+')
					{
						if (mul_div_flag)//mul_div_flag�����ж��Ƿ��г˳�
						{
							*q = '-';//��������㣺�����ø�
							q = t;//�ָ�q
							*q++ = *r++;//�ٶ������ֵ���ַ������˸��ź��������
							mul_div_flag = 0;
							continue;
						}
						*q++ = '-';//��������㣺�����ø�
						*q++ = *r++;//û�г˳�����Ҫ�ָ�q���ٶ������ֵ���ַ������˸��ź��������
					}
					else if (*q == '-')
					{
						if (mul_div_flag)
						{
							*q = '+';//��������㣺��������
							q = t;//�ָ�q
							*q++ = *r++;//�ٶ������ֵ���ַ��������ź��������
							mul_div_flag = 0;
							continue;
						}
						*q++ = '+';//��������㣺�����ø�
						*q++ = *r++;//û�г˳�����Ҫ�ָ�q���ٶ������ֵ���ַ������˸��ź��������
					}
				}
				else
				{
					*q++ = *r++;//����ֵ���ַ����ͽ�����
				}

			}
			for (;(isalpha(*p) || *p == '_') && *p != '\0';p++)//p�����˱���������Ŀո�
			{
				;//�����������pָ�����������Ŀո񣬵�û�й�ϵ���ո����Ŀ�ı��ʽ��Ӱ���沨��ʽ
			}
		}
		if ((*p == '+' || *p == '-'))
		{
			if (*(p + 1) == '+' || *(p + 1) == '-')
			{
				p += 2;
				while (*p == ' ')
				{
					p++;
				}
				//p += 2;//��������/�� �������������һ���ַ�
				continue;
			}
			else
			{
				*q++ = *p++;//+��-������
			}
		}
		else if ((*p == '*' || *p == '/'))
		{
			*q++ = *p++;
			if (*p == ' ')
			{
				*q++ = *p++;
			}
			if (isalpha(*p) || *p == '_')
			{
				continue;
			}
			else if (isdigit(*p)) continue;
			t = q;
			if (*p == '-')
			{
				while (*q != '+' && *q != '-')
				{
					q--;
				}
				if (*q == '-')
				{
					*q = '+';
				}
				else if (*q = '+')
				{
					*q = '-';
				}
			}
			else if (*p == '+')
			{
				while (*q != '+' && *q != '-')
				{
					q--;
				}
				if (*q == '-')
				{
					*q = '-';
				}
				else if (*q = '+')
				{
					*q = '+';
				}
			}
			q = t;
			p++;
			*q++ = *p++;
		}
		else if (*p == '>' || *p == '<' || *p == '!' || *p == '=')
		{
			*q++ = *p;
			if (*(p + 1) == '=')//'>='��'<=','!=','=='�����
			{
				*q++ = *(p + 1);
				p += 2;//'>'��'='������Ŀ����ʽ
				while (*p == ' ') p++;
				if (*p == '-')//����ȽϷ��ź��Ǹ��ŵ����
				{
					*q++ = '0';//Ϊ�������'0' �����ɡ�0 - remained_expression��������������Ϊ���ܷ���������ȷ���沨�����ʽ
				}
				else
				{
					*q++ = '0';
					*q++ = '+';//���Ǹ������'0'��'+' �����ɡ�0 + remained_expression��������������Ϊ���ܷ���������ȷ���沨�����ʽ
				}
			}
			else
			{
				p++;
				while (*p == ' ') p++;
				if (*p == '-')
				{
					*q++ = '0';
				}
				else
				{
					*q++ = '0';
					*q++ = '+';
				}
			}
		}
		else
		{
			*q++ = *p++;//��ͨ�ַ�����0~9��'*'��'/'�� ' '��ֱ�ӽ�����
		}

	}

	*q = '\0';
	//return tar_string;
}

/** \brief �������ջ
*
* \param �����
* \param
* \return void
*
*/

void Push_Input(char c) /*�����c��ջ����*/
{
	TOP++;
	stack_input[TOP] = c;
}

/** \brief �������ջ
*
* \param void
* \param
* \return �����
*
*/

char Pop_Input(void)
{
	char c = stack_input[TOP];;
	TOP--;
	return c;
}

/** \brief ��ֵ
*
* \param �沨��ʽ
* \param
* \return
*
*/

int Exp_Output(char *string_out)
{
	TOP = -1;
	char *p = string_out;
	int n;
	int left_num, right_num;
	int ret;//��������ֵ

	while (*p == ' ')
	{
		p++;
	}
	while (*p != '\0')
	{
		if (isdigit(*p))
		{
			n = 0;
			for (;isdigit(*p);p++)
			{
				n = n * 10 + *p - '0';
			}
			Push_Output(n);
		}
		else
		{
			right_num = Pop_Output();
			left_num = Pop_Output();
			switch (*p)
			{
			case '+':
				left_num += right_num;
				break;
			case '-':
				left_num -= right_num;
				break;
			case '*':
				left_num *= right_num;
				break;
			case '/':
				left_num /= right_num;
				break;
			case '<':
				left_num = left_num<right_num ? 1 : 0;
				break;
			case '>':
				left_num = left_num>right_num ? 1 : 0;
				break;
			case '@':// <=�����
				left_num = left_num <= right_num ? 1 : 0;
				break;
			case '#':// >=�����
				left_num = left_num >= right_num ? 1 : 0;
				break;
			case '$':// !=�����
				left_num = left_num != right_num ? 1 : 0;
				break;
			case '%':// ==�����
				left_num = left_num == right_num ? 1 : 0;
				break;
			default:
				break;
			}
			Push_Output(left_num);
			p++;
		}
		while (*p == ' ')
			p++;
	}
	ret = Pop_Output();
	return ret;
}

/** \brief ������ջ
*
* \param ����
* \param
* \return void
*
*/

void Push_Output(int n)
{
	TOP++;    /*ջָ��ָ���ѹ��Ԫ*/
	stack_output[TOP] = n;/*������ջ*/
}

/** \brief ջ��Ԫ�س�ջ
*
* \param void
* \param
* \return ջ��Ԫ��
*
*/

int Pop_Output(void)
{
	int x;
	x = stack_output[TOP];/*ջ��Ԫ��ֵ����x*/
	TOP--;/*ջָ���1��ָ����һ��Ԫ*/

	return x;/*����ջ��Ԫ��ֵ*/
}
