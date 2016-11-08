#include"head.h"

int stack_output[SIZE];         /*将数组作为值栈使用*/
char stack_input[SIZE];         /*运算符栈*/
int TOP = -1;                   /*下标作为栈顶标记指针*/
int LENGTH = 0;

/** \brief 计算器，可以计算表达式的值并返回
*
* \param 表达式字符串
* \param
* \return 表达式值
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

/** \brief 生成逆波兰式
*
* \param 源代码中的表达式
* \param
* \return 逆波兰式
*
*/

void Exp_Input(char *string_in, char(*out)[SIZE])
{
	char convert[SIZE];
	//char out[SIZE];                 /*存放逆波兰表达式的数组*/
	char c;
	char *p = string_in;
	char *q = string_in;
	int flag = -1;//用于判断自增自减
	char variable[SIZE];//表达式里的变量名

						/*ConvertToTarStr函数将原表达式转换成只
						含数字和'+'、'-'、'*'、'/'的表达式，即使用变量值替换变量名*/
	ConvertToTarStr(string_in, &convert);

	/*46-85行的for循环所要实现的功能是将含变量的
	表达式中如果有变量自增或者自减，则改变它们的值*/
	for (;*p != '\0';p++)
	{
		if (isalpha(*p) || *p == '_')
		{
			q = p;//存储变量名
			for (;isalpha(*p) || *p == '_' || *p == ' ';p++)
			{
				;
			}//这时p指针到达了var后面的空格，空格之后是++或者--
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
	p = convert;//p指向目标表达式
	memset(*out, ' ', SIZE);/*全部初始化为空格*/
	for (;*p != '\0';p++)
	{
		if (isdigit(*p))
		{
			while (isdigit(*p))
			{
				(*out)[LENGTH++] = *p++; /*数字字符直接进入逆波兰序列*/
			}
			p--;
			LENGTH++;
		}
		else if (*p == '+' || *p == '-')
		{
			while (TOP >= 0 && (stack_input[TOP] == '+' || stack_input[TOP] == '-' || stack_input[TOP] == '*'
				|| stack_input[TOP] == '/'))
			{
				(*out)[LENGTH++] = Pop_Input();        /*出栈*/
				LENGTH++;
			}
			Push_Input(*p); /*入栈*/
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
				Push_Input('@');//以@符号代替<=
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
				Push_Input('#');//以#符号代替>=
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
				Push_Input('$');//以$符号代替!=
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
				Push_Input('%');//以%符号代替==
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
		(*out)[LENGTH++] = c;        /*出栈*/
		LENGTH++;
	}
	(*out)[LENGTH] = '\0';    /*形成逆波兰字符串*/
							  //puts(out);
							  //return out;
}

/** \brief 将原表达式转换成目标表达式，即不含变量名
*
* \param 原表达式
* \param
* \return 目标表达式
*
*/

void ConvertToTarStr(char string_in[SIZE], char(*tar_string)[SIZE])
{
	char var_value_instring[SIZE];//存储变量值的字符数组（需要将int型转换成char类型）
	char var[SIZE];//存储变量名
				   //char tar_string[SIZE];//目的表达式
	char *p = string_in;//原始表达式
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
		if (isalpha(*p) || *p == '_')//处理的是变量
		{
			sscanf(p, "%s", var);//获取变量名
			itoa(Get_Var(var), r, 10);
			/* 指针r指向变量值的字符串，指针q指向待生成的目标字符串，t记录q的位置 */
			for (;*r != '\0';)
			{
				if (*r == '-')//如果表达式中的变量是负数的处理方案
				{
					r++;
					while (*r == ' ')//去掉空格
					{
						r++;
					}
					t = q;//*t记录目标表达式的当前最后一个字符的下一位
					while (*q != '+' && *q != '-')
					{
						/* 此循环搜索变量名前是' + '还是' - '，如果是' * '或者' / '，
						那么' * '或者' / '前是' + '还是' - '。遇到' + '或' - '跳出循环。
						需说明一点是经过在目标表达式在前述的处理加上“ 0 ”或“ 0 + ”后，变量名前一定会有' + '或者' - ' */
						if (*q == '*' || *q == '/')
						{
							mul_div_flag = 1;
						}
						q--;
					}
					if (*q == '+')
					{
						if (mul_div_flag)//mul_div_flag用于判断是否含有乘除
						{
							*q = '-';//运算符计算：正负得负
							q = t;//恢复q
							*q++ = *r++;//再读入变量值的字符串除了负号后面的内容
							mul_div_flag = 0;
							continue;
						}
						*q++ = '-';//运算符计算：正负得负
						*q++ = *r++;//没有乘除则不需要恢复q，再读入变量值的字符串除了负号后面的内容
					}
					else if (*q == '-')
					{
						if (mul_div_flag)
						{
							*q = '+';//运算符计算：负负得正
							q = t;//恢复q
							*q++ = *r++;//再读入变量值的字符串除负号后面的内容
							mul_div_flag = 0;
							continue;
						}
						*q++ = '+';//运算符计算：正负得负
						*q++ = *r++;//没有乘除则不需要恢复q，再读入变量值的字符串除了负号后面的内容
					}
				}
				else
				{
					*q++ = *r++;//变量值以字符串型进数组
				}

			}
			for (;(isalpha(*p) || *p == '_') && *p != '\0';p++)//p到达了变量名后面的空格
			{
				;//从这里出来的p指向变量名后面的空格，但没有关系，空格进了目的表达式不影响逆波兰式
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
				//p += 2;//跳过自增/减 运算符，到达下一个字符
				continue;
			}
			else
			{
				*q++ = *p++;//+或-进数组
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
			if (*(p + 1) == '=')//'>='，'<=','!=','=='的情况
			{
				*q++ = *(p + 1);
				p += 2;//'>'和'='均进入目标表达式
				while (*p == ' ') p++;
				if (*p == '-')//处理比较符号后是负号的情况
				{
					*q++ = '0';//为负号则加'0' ，构成“0 - remained_expression”，这样处理是为了能方便生成正确的逆波兰表达式
				}
				else
				{
					*q++ = '0';
					*q++ = '+';//不是负号则加'0'和'+' ，构成“0 + remained_expression”，这样处理是为了能方便生成正确的逆波兰表达式
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
			*q++ = *p++;//普通字符包括0~9，'*'、'/'、 ' '，直接进数组
		}

	}

	*q = '\0';
	//return tar_string;
}

/** \brief 运算符入栈
*
* \param 运算符
* \param
* \return void
*
*/

void Push_Input(char c) /*运算符c入栈操作*/
{
	TOP++;
	stack_input[TOP] = c;
}

/** \brief 运算符出栈
*
* \param void
* \param
* \return 运算符
*
*/

char Pop_Input(void)
{
	char c = stack_input[TOP];;
	TOP--;
	return c;
}

/** \brief 求值
*
* \param 逆波兰式
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
	int ret;//函数返回值

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
			case '@':// <=的情况
				left_num = left_num <= right_num ? 1 : 0;
				break;
			case '#':// >=的情况
				left_num = left_num >= right_num ? 1 : 0;
				break;
			case '$':// !=的情况
				left_num = left_num != right_num ? 1 : 0;
				break;
			case '%':// ==的情况
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

/** \brief 数据入栈
*
* \param 数据
* \param
* \return void
*
*/

void Push_Output(int n)
{
	TOP++;    /*栈指针指向待压单元*/
	stack_output[TOP] = n;/*数据入栈*/
}

/** \brief 栈顶元素出栈
*
* \param void
* \param
* \return 栈顶元素
*
*/

int Pop_Output(void)
{
	int x;
	x = stack_output[TOP];/*栈顶元素值赋给x*/
	TOP--;/*栈指针减1，指向下一单元*/

	return x;/*返回栈顶元素值*/
}
