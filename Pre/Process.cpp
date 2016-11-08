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
	if (LINE[CUR_LINE] == PRINTED_LINE)//避免重复打印同一行的语句行号
		return;
	if (firstline_flag) {
		firstline_flag = 0;
		fprintf(pf_out, "%d" ,LINE[CUR_LINE]);//打印行号
	}
	else {
		fprintf(pf_out, " %d", LINE[CUR_LINE]);//打印行号
	}
	PRINTED_LINE = LINE[CUR_LINE];
}

void Order(void) {
	char keyword[KEYLEN];//关键字
	char line[MAXLEN + 1];//读取的行
	char *pline = NULL;//指示行中读到的单词位置
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
			int print_flag = 0;//打印行号的标记
			int var_value = 0;//默认初始化0
			char var_name[KEYLEN];
			char word[KEYLEN];//行内的空格分开的单词
			while (1) {
				sscanf(pline, "%s", word);
				pline += strlen(word) + 1;
				j++;
				if (j == 1)continue;//跳过int
				if (word[0] == ';' || word[0] == ',') {//一个声明结束
					if (var_flag == 1) {
						Push_Var(var_name, 0);//变量默认初始化0
						var_flag = 0;
					}
					if (word[0] == ';')break;
					continue;
				}
				if (word[0] == '=') {
					if (!print_flag) {
						PrintLine();//打印行号
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
					var_value = Calculator(operation);//调用计算器计算表达式的值
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
			Push_Scope("Branch");//分支作用域进栈
			Branch();
			Pop_Scope();//分支作用域出栈
			if (break_flag) {//如果前一个块通过break结束且是分支块
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
			Push_Scope("Loop");//循环作用域进栈
			Loop();
			Pop_Scope();//循环作用域出栈
			break_flag = 0;
			break;
		}
		case 6: {//printf
			PrintLine();//打印行号
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
		case 7: {//左大括号
			block_flag = 1;
			break;
		}
		case 8: {//右大括号
			return;
		}
		case 9: {//break
			PrintLine();
			break_flag = 1;
			return;
		}
		case 10: {//空语句
			break;
		}
		default: {//变量名
			char var_name[KEYLEN];
			char word[KEYLEN];
			int var_value;
			PrintLine();//打印行号
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
					var_value = Calculator(operation);//使用计算器对表达式求值
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
		if (!block_flag)//块内只有一条语句
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
	char line[MAXLEN + 1];//存储文件一行内容
	char word[MAXLEN + 1];//存储变量
	char exp[MAXLEN + 1];//存储if语句或者else if语句后括号内的表达式
	int ret_exp;//存储if语句或者else if语句后括号内的表达式的返回值
	long start_line = ftell(PFILE);//保存文件指针开始的位置
	int start_line_num = CUR_LINE;//保存当前行
	fgets(line, MAXLEN + 1, PFILE);
	CUR_LINE++;
	int has_brace = 0;//处理条件语句后面没有大括号的情况

	queue<END> end;

	int brace = 0;//'{'或'}'的个数
	int i = 0;

	//下while(1)循环处理分支结构块结束位置
	while (1)
	{
		while (fgets(line, MAXLEN + 1, PFILE)) //line指示的行数比CUR_LINE先到，即多一行
		{
			CUR_LINE++;
			if (sscanf(line, "%s", word) == -1)//跳过空行
			{
				continue;
			}
			if (word[0] == '{')//遇到左大括号brace值加1
			{
				has_brace = 1;
				brace++;
			}
			else if (word[0] == '}') //遇到右大括号brace值减1
			{
				brace--;
				if (brace == 0)//左右大括号是匹配的，只要brace为0则说明所有左右大括号均已匹配，可认为当前语句块结束
				{
					has_brace = 0;
					END tmp;
					tmp.end_line = ftell(PFILE);//记录文件指针指向'}'所在行
					tmp.end_line_num = CUR_LINE;//记录'}'行号
					end.push(tmp);//将上面两行记录入队列，为当前块结束的位置信息
					break;
				}
			}
			if (has_brace == 0)//处理没有左右大括号的情况，这样if语句后面的行数只能有一行，记录此行数和文件指针指向此行的信息
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
			if (sscanf(line, "%s", word) != -1 || feof(PFILE))//不是空行即break，是空行则继续处理下一行，（其实就是跳过空行）或者遇到文件尾跳出
				break;
		}
		if (strcmp(word, "else"))//不是else或者else if，则跳出循环，说明当前分支块结束了
			break;
		i++;
	}//处理结束，回归对if-else的判断处理

	 /*END tmp = end.back();
	 int end_all_branch_line = tmp.end_line_num + 1;//取到整个分支结构的结束的下一行
	 long end_all_branch_fileline = tmp.end_line;
	 end.push(tmp);*/

	fseek(PFILE, start_line, 0);//重定向文件指针
	CUR_LINE = start_line_num;//重定向当前行，指向开头的if语句
	while (1)//判断进入的条件语句块
	{
		fgets(line, MAXLEN + 1, PFILE);
		CUR_LINE++;

		if (sscanf(line, "%s", word) == -1)
		{
			continue;
		}

		//正则表达式取出括号内的表达式，在if()和else if()情况下一定能取到表达式，else会取不到表达式，sscanf返回0
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
			END tmp = end.back();//返回队列尾部元素
			fseek(PFILE, tmp.end_line, 0);
			CUR_LINE = tmp.end_line_num;
			return;
		}
		else
		{
			END tmp = end.front();//返回队列头部元素
			end.pop();//队头出队
			fseek(PFILE, tmp.end_line, 0);
			CUR_LINE = tmp.end_line_num;
			if (end.empty()) //队列为空，即已把if-else所有的标志块结束的行数都调用完毕
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
		printf("执行判断循环关键词出错！\n");
		exit(-1);
	}

}


void ForFunction(void)
{
	CUR_LINE++;

	char expression[MAXLEN]; //表达式
	char valueName[KEYLEN]; //变量名字
	char keyword[KEYLEN];//关键字
	char line[MAXLEN + 1];//读取的行
	char *pline = NULL;//指示行中读到的单词位置
	int startLine = CUR_LINE;  //“for（；；）”这一行
	int endLine;
	int i = 0;   //临时变量
	bool isTheFirstFor = true;
	bool isOnce = IsOnce();	   //是否只执行一次
	long firstPosition = ftell(PFILE);
	long endPosition;

	//判断是否这个语句只有一次，从而确定结束的行号
	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //读“for（；；）”这一行
		fgets(line, MAXLEN + 1, PFILE);  //读“for（；；）”后面一行
		CUR_LINE++;
		while (sscanf(line, "%s", keyword) == -1)	 //跳过空行
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
		}
		endPosition = ftell(PFILE);
		endLine = CUR_LINE;
	}
	else
	{
		int brace = 0;//大括号数量
		fgets(line, MAXLEN + 1, PFILE);  //读 for(...)这一行
		CUR_LINE++;
		while (1) {
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", keyword) == -1)	  //跳过空白行
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



	//for循环
	while (1)
	{
		fseek(PFILE, firstPosition, 0);   //找到“for（；；）”这一行 
		fgets(line, MAXLEN + 1, PFILE);	 //读“for（；；）”这一行 
		CUR_LINE = startLine;

		PrintLine();	    //输出for循环所在行号

							//处理第一个“；”之前的东西
		if (isTheFirstFor == true)
		{
			isTheFirstFor = false; //防止下一次执行第一个“；”之前的操作

			pline = line;
			while (*pline++ != '(');   //结束后pline指向‘（’之后的第一个字符
			while (1)	//处理第一个“；”之前的表达式
			{
				sscanf(pline, "%s", keyword);
				if (!strcmp(keyword, KEYMAP[0])) //第一个是“int”
				{

					pline = pline + strlen(keyword) + 1;   //跳过“int”
					sscanf(pline, "%s", valueName);       //变量名
					pline = pline + strlen(valueName) + 1;//跳过变量名
					sscanf(pline, "%s", keyword);         //“=” 或者 “；” 或者“，”
					pline = pline + strlen(keyword) + 1;
					if (!strcmp(keyword, "=")) //赋值
					{
						pline++;

						i = 0;
						while (*pline != ';' && *pline != ',')
						{
							expression[i++] = *pline++;
						}
						expression[i] = '\0';
						Push_Var(valueName, Calculator(expression));
						if (*pline == ';')	 //语句结束
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //进入下一个语句
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
				else    //第一个是变量名
				{
					strcpy(valueName, keyword);  //读取变量名
					pline = pline + strlen(valueName) + 1;//跳过变量名
					sscanf(pline, "%s", keyword);  //读取“=”或者布尔运算符
					if (!strcmp(keyword, "="))  //这个语句是一个赋值语句
					{
						pline = pline + strlen(keyword) + 1;
						i = 0;
						while (*pline != ';' && *pline != ',')
						{
							expression[i++] = *pline++;
						}
						expression[i] = '\0';
						Set_Var(valueName, Calculator(expression));
						if (*pline == ';')	 //语句结束
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //进入下一个语句
						{
							pline++;
							continue;
						}

					}
					else  //这句话是一个计算式
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
					if (*pline == ';')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}
				}
			}//while结束
		}
		else   //不是第一次进入for循环
		{
			pline = line;
			while (*pline++ != ';');   //结束后pline指向‘;’之后的第一个字符
		} //if结束

		  //处理第一个“；”到第二个“；”之间的东西
		i = 0;
		while (*pline != ';')  //取出中间的表达式
		{
			expression[i++] = *pline++;
		}
		expression[i] = '\0';
		for (i = 0; expression[i] != '\0'; i++)	   //判断“for(;;)的情况”(中间判断语句是空的)
		{
			if (expression[i] == ' ') continue;
			else break;
		}
		if (expression[i] == '\0' && expression[i - 1] == ' ')
		{
			strcpy(expression, "1");
		}
		if (Calculator(expression) == 1)	//进入循环体
		{
			fseek(PFILE, firstPosition, 0);   //找到“for（；；）”这一行
			fgets(line, MAXLEN + 1, PFILE);  //读“for（；；）”这一行
			if (isOnce == false)
			{
				while (line[0] != '{') //找到循环体所在的行数。
				{
					CUR_LINE = CUR_LINE + 1;
					fgets(line, MAXLEN + 1, PFILE);
				}
				int len = strlen(line);
				fseek(PFILE, -1 * len - 1, 1);
				CUR_LINE--;
			}

			Push_Scope("Order");
			//顺序函数
			Order();
			Pop_Scope();//循环作用域出栈
			if (break_flag) {
				CUR_LINE = endLine;
				fseek(PFILE, firstPosition, 0);   //找到“for（；；）”这一行
				for (i = 0; i < endLine - startLine + 1; i++)	  //跳到for循环的最后。“}”之后
				{
					fgets(line, MAXLEN + 1, PFILE);
				}
				return;
			}
			//回到“for（；；）”语句
			CUR_LINE = startLine;
			fseek(PFILE, firstPosition, 0);  //找到“for（；；）”这一行
			fgets(line, MAXLEN + 1, PFILE);	 //读“for（；；）”这一行
			pline = line;
			while (*pline++ != ';');   //结束后pline指向第一个‘;’之后的第一个字符
			while (*pline++ != ';');   //结束后pline指向第二个‘;’之后的第一个字符
		}
		else
		{
			CUR_LINE = endLine;
			fseek(PFILE, endPosition, 0);   //找到“for（；；）”这一行
			return;
		}

		//处理第二个“；”到第三个“；”之间的东西
		while (1)
		{
			sscanf(pline, "%s", keyword);
			if (!strcmp(keyword, KEYMAP[0])) //第一个是“int”
			{
				pline = pline + strlen(keyword) + 1;   //跳过“int”
				sscanf(pline, "%s", valueName);       //变量名
				pline = pline + strlen(valueName) + 1;//跳过变量名
				sscanf(pline, "%s", keyword);         //“=” 或者 “；” 或者“，”
				pline = pline + strlen(keyword) + 1;
				if (!strcmp(keyword, "=")) //赋值
				{
					pline++;
					i = 0;
					while (*pline != ';' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Push_Var(valueName, Calculator(expression));
					if (*pline == ';')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}
				}
			}
			else    //第一个是变量名
			{
				strcpy(valueName, keyword);  //读取变量名
				pline = pline + strlen(valueName) + 1;//跳过变量名
				sscanf(pline, "%s", keyword);  //读取“=”或者布尔运算符
				if (!strcmp(keyword, "="))  //这个语句是一个赋值语句
				{
					pline = pline + strlen(keyword) + 1;
					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Set_Var(valueName, Calculator(expression));
					if (*pline == ')')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}

				}
				else  //这句话是一个计算式
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
				if (*pline == ')')	 //语句结束
				{
					pline++;
					break;
				}
				else if (*pline == ',')	  //进入下一个语句
				{
					pline++;
					continue;
				}
			}
		}//while结束
	}//while结束
}


void WhileFunction(void)
{
	CUR_LINE++;

	char expression[MAXLEN]; //表达式
	char valueName[KEYLEN]; //变量名字
	char keyword[KEYLEN];//关键字
	char line[MAXLEN + 1];//读取的行
	char *pline = NULL;//指示行中读到的单词位置
	int startLine = CUR_LINE;  //“while（,,..）”这一行
	long startPosition = ftell(PFILE);
	long endPosition;
	int endLine;
	int i = 0;   //临时变量
	int whileValue;  //记录while语句的值
	bool isOnce = IsOnce();


	//判断是否这个语句只有一次，从而确定结束的行号
	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //读“for（；；）”这一行
		fgets(line, MAXLEN + 1, PFILE);  //读“for（；；）”后面一行
		CUR_LINE++;
		while (sscanf(line, "%s", keyword) == -1)	 //跳过空行
		{
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
		}
		endPosition = ftell(PFILE);
		endLine = CUR_LINE;
	}
	else
	{
		int brace = 0;//大括号数量
		fgets(line, MAXLEN + 1, PFILE);  //读 while(...)这一行
		CUR_LINE++;
		while (1) {
			fgets(line, MAXLEN + 1, PFILE);
			CUR_LINE++;
			if (sscanf(line, "%s", keyword) == -1)	  //跳过空白行
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


	 //进入while循环
	while (1)
	{
		fseek(PFILE, startPosition, 0);   //找到“while（...）”这一行 
		fgets(line, MAXLEN + 1, PFILE);	 //读“while（...）”这一行 
		CUR_LINE = startLine;

		PrintLine();  //输出while循环所在行号

		pline = line;
		while (*pline++ != '(');   //结束后pline指向‘（’之后的第一个字符
		while (1)	//处理“）”之前的表达式
		{
			sscanf(pline, "%s", keyword);
			if (!strcmp(keyword, KEYMAP[0])) //第一个是“int”
			{

				pline = pline + strlen(keyword) + 1;   //跳过“int”
				sscanf(pline, "%s", valueName);       //变量名
				pline = pline + strlen(valueName) + 1;//跳过变量名
				sscanf(pline, "%s", keyword);         //“=” 或者 “；” 或者“，”
				pline = pline + strlen(keyword) + 1;
				if (!strcmp(keyword, "=")) //赋值
				{
					pline++;

					i = 0;
					while (*pline != ')' && *pline != ',')
					{
						expression[i++] = *pline++;
					}
					expression[i] = '\0';
					Push_Var(valueName, Calculator(expression));
					if (*pline == ')')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}
				}
			}
			else    //第一个是变量名
			{
				strcpy(valueName, keyword);  //读取变量名
				pline = pline + strlen(valueName) + 1;//跳过变量名
				sscanf(pline, "%s", keyword);  //读取“=”或者布尔运算符
				if (!strcmp(keyword, "="))  //这个语句是一个赋值语句
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
					if (*pline == ')')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}

				}
				else  //这句话是一个计算式
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
				if (*pline == ')')	 //语句结束
				{
					pline++;
					break;
				}
				else if (*pline == ',')	  //进入下一个语句
				{
					pline++;
					continue;
				}
			}
		}//while结束

		 //进入循环体
		if (whileValue != 0)
		{
			fseek(PFILE, startPosition, 0);   //找到“while（...）”这一行
			fgets(line, MAXLEN + 1, PFILE);  //读“{”这一行

			if (isOnce == false)
			{
				while (line[0] != '{') //找到循环体所在的行数。
				{
					CUR_LINE = CUR_LINE + 1;
					fgets(line, MAXLEN + 1, PFILE);
				}
				int len = strlen(line);
				fseek(PFILE, -1 * len - 1, 1);
				CUR_LINE--;
			}

			Push_Scope("Order");
			//顺序函数
			Order();
			Pop_Scope();//循环作用域出栈
			if (break_flag) {
				CUR_LINE = endLine;
				fseek(PFILE, startPosition, 0);   //找到“while（...）”这一行
				for (i = 0; i < endLine - startLine + 1; i++)	  //跳到while循环的最后。“}”之后
				{
					fgets(line, MAXLEN + 1, PFILE);
				}
				return;
			}
		}
		else  //跳出循环
		{
			CUR_LINE = endLine;
			fseek(PFILE, endPosition, 0);   //找到结束行
			return;
		}
	}
}


void DoFunction(void)
{
	char *pline;
	char valueName[KEYLEN];	//变量名称
	int i = 0;	//临时变量
	char line[MAXLEN + 1];
	char exp[MAXLEN + 1];
	int ret_exp = 1;
	char word[KEYLEN];	//关键字
	long start_line = ftell(PFILE);
	int start_line_num = CUR_LINE;
	long end_line;
	int end_line_num;
	int brace = 0;//大括号数量
	bool isOnce = IsOnce();


	if (isOnce == true)
	{
		fgets(line, MAXLEN + 1, PFILE);	 //读“do”这一行
		fgets(line, MAXLEN + 1, PFILE);  //读“（；；）”后面一行
		CUR_LINE++;
		while (sscanf(line, "%s", word) == -1)	 //跳过空行
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
			//顺序函数
			Order();
			Pop_Scope();//循环作用域出栈
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
			PrintLine(); //打印行号

			pline = line;
			while (*pline++ != '(');   //结束后pline指向‘（’之后的第一个字符
			while (1)	//处理“）”之前的表达式
			{
				sscanf(pline, "%s", word);
				if (!strcmp(word, KEYMAP[0])) //第一个是“int”
				{

					pline = pline + strlen(word) + 1;   //跳过“int”
					sscanf(pline, "%s", valueName);       //变量名
					pline = pline + strlen(valueName) + 1;//跳过变量名
					sscanf(pline, "%s", word);         //“=” 或者 “；” 或者“，”
					pline = pline + strlen(word) + 1;
					if (!strcmp(word, "=")) //赋值
					{
						pline++;

						i = 0;
						while (*pline != ')' && *pline != ',')
						{
							exp[i++] = *pline++;
						}
						exp[i] = '\0';
						Push_Var(valueName, Calculator(exp));
						if (*pline == ')')	 //语句结束
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //进入下一个语句
						{
							pline++;
							continue;
						}
					}
				}
				else    //第一个是变量名
				{
					strcpy(valueName, word);  //读取变量名
					pline = pline + strlen(valueName) + 1;//跳过变量名
					sscanf(pline, "%s", word);  //读取“=”或者布尔运算符
					if (!strcmp(word, "="))  //这个语句是一个赋值语句
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
						if (*pline == ')')	 //语句结束
						{
							pline++;
							break;
						}
						else if (*pline == ',')	  //进入下一个语句
						{
							pline++;
							continue;
						}

					}
					else  //这句话是一个计算式
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
					if (*pline == ')')	 //语句结束
					{
						pline++;
						break;
					}
					else if (*pline == ',')	  //进入下一个语句
					{
						pline++;
						continue;
					}
				}
			}//while结束

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

	fgets(line, MAXLEN + 1, PFILE);	  //读 “for”或“do”或“while”这一行
	fgets(line, MAXLEN + 1, PFILE);	  //读 “for”或“do”或“while”的下一行
	while (sscanf(line, "%s", keyword) == -1)	 //跳过空行
	{
		fgets(line, MAXLEN + 1, PFILE);	  //读 “for”或“do”或“while”的下一行
	}
	if (!strcmp(keyword, "{"))	 //第一个字符是“{”
	{
		isOnce = false;
	}
	else
	{
		isOnce = true;
	}

	//行数和指针回到初始位置
	CUR_LINE = startLine;
	fseek(PFILE, startPosition, 0);

	return isOnce;
}
