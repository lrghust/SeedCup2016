#include"head.h"

//预处理加空格，删注释，存行号，改为一行只有一条语句
void PreDeal(void) {
	FILE *pf_in = fopen("input.txt", "r+");
	if (!pf_in) {
		cout << "文件打开失败！" << endl;
		exit(-1);
	}
	FILE *pf_tmp = fopen(".tmp", "w+");
	char line[MAXLEN + 1];//读入的一行
	int space_flag = 0;//0表示行首的空字符,1表示已读到非空字符
	int string_flag = 0;//是否进入printf中的字符串
	int comment_flag1 = 0;//是否进入行注释
	int comment_flag2 = 0;//是否进入块注释
	int bracket_flag = 0;//是否进入括号即for里的多条语句不分行
	int block_flag = 0;
	int line_num = 0;//表示源文件里的行号
	LINE.push_back(0);//跳过向量首元素，从索引为1开始使用
	putc('{', pf_tmp);//文件开头额外添加一个左大括号便于进行块处理
	putc('\n', pf_tmp);
	LINE.push_back(0);//跳过文件开头额外添加的左大括号

	while (fgets(line, MAXLEN + 1, pf_in)) {
		line_num++;//当前读入行的行号
		for (int i = 0;line[i] != '\0';i++) {

			if (!space_flag && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n'))//跳过句首空字符
				continue;
			if (!string_flag && !comment_flag2&&line[i] == '/'&&line[i + 1] == '/') {//整行注释
				break;
			}
			//添加大括号
			if (line[i] == 'f'&&line[i+1] == 'o'&&line[i+2] == 'r' && !comment_flag2)
				block_flag = 1;
			else if (line[i] == 'w'&&line[i+1] == 'h'&&line[i+2] == 'i' &&line[i+3] == 'l'&&line[i+4] == 'e' && !comment_flag2)
				block_flag = 1;
			else if (line[i] == 'i'&&line[i+1] == 'f' && !comment_flag2)
				block_flag = 1;
			else if (line[i] == 'e'&&line[i+1] == 'l'&&line[i+2] == 's'&&line[i+3] == 'e' && !comment_flag2) {
				if (line[i + 5] == 'i'&&line[i + 6] == 'f')block_flag = 1;
				else {
					i = i + 4;
					fprintf(pf_tmp, "else\n");
					LINE.push_back(line_num);
					while(line[i]==' ')i++;
				}
			}
			space_flag = 1;//读到非空字符

			switch (line[i]) {
			case '"'://遇到printf中的字符串，不做下面的处理
				if (comment_flag2) {//在块注释内部
					break;
				}
				/*if (line[i - 1] != '\\') {//读到的双引号不是字符串中转义的双引号
					string_flag = !string_flag;//取反,这样第二次读到"可以退出字符串
				}*/
				putc(line[i], pf_tmp);
				while (1) {
					i++;
					putc(line[i], pf_tmp);
					if (line[i] == '"')
						break;
					if (line[i] == '\\') {
						i++;
						putc(line[i], pf_tmp);
					}
				}
				break;
			case '/':
				if (comment_flag2 && line[i - 1] == '*') {//退出块注释
					comment_flag2 = 0;
					space_flag = 0;
					break;
				}
				if (comment_flag2)
					break;
				if (string_flag) {
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i + 1] == '/') {//进入行注释
					comment_flag1 = 1;
				}
				else if (line[i + 1] == '*') {//进入块注释
					comment_flag2 = 1;
				}
				else {//普通除号
					if (line[i - 1] != ' ')//在符号前加空格
						putc(' ', pf_tmp);
					putc(line[i], pf_tmp);//输出符号
					if (line[i + 1] != ' ')//在符号后加空格
						putc(' ', pf_tmp);
				}
				break;
			case '=':
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' '&&line[i - 1] != '>'&&line[i - 1] != '<'&&line[i - 1] != '!'&&line[i - 1] != '=')//在等号前加空格
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//输出等号
				if (line[i + 1] != ' '&&line[i + 1] != '=')//在等号后加空格
					putc(' ', pf_tmp);
				break;
			case '(':
			case ')':
				if (comment_flag2) {//块注释内部括号不处理
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				bracket_flag = !bracket_flag;//标记位取反，进入或退出括号
				if (line[i - 1] != ' ')//在符号前加空格
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//输出符号
				if (line[i + 1] != ' ')//在符号后加空格
					putc(' ', pf_tmp);

				if (block_flag&&line[i]==')') {
					putc('\n', pf_tmp);
					LINE.push_back(line_num);
					block_flag = 0;
					i++;
					while(line[i]==' '||line[i]=='\n')i++;
					i--;
				}

				break;
			case '{':
			case '}':
				if (comment_flag2) {//块注释内部括号不处理
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				putc(line[i], pf_tmp);
				putc('\n', pf_tmp);
				LINE.push_back(line_num);
				break;
			case ',':
			case '*':
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' ')//在符号前加空格
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//输出符号
				if (line[i + 1] != ' ')//在符号后加空格
					putc(' ', pf_tmp);
				break;
			case ';':
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				if (bracket_flag) {//在括号内部
					if (line[i - 1] != ' ')//在符号前加空格
						putc(' ', pf_tmp);
					putc(line[i], pf_tmp);//输出符号
					if (line[i + 1] != ' ')//在符号后加空格
						putc(' ', pf_tmp);
					break;
				}
				LINE.push_back(line_num);
				if (line[i - 1] != ' ')//在符号前加空格
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//输出符号
				putc('\n', pf_tmp);//输出换行符
				space_flag = 0;
				break;
			case '>':case '<': {
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' ')
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);
				if (line[i + 1] == '=') {
					putc(line[i + 1], pf_tmp);
					i++;
					if(line[i+1]!=' ')
						putc(' ', pf_tmp);
				}
				else if(line[i+1]!=' ')
					putc(' ', pf_tmp);
				break;
			}
			case '!': {
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' ')
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);
				i++;
				putc(line[i], pf_tmp);
				if (line[i + 1] != ' ')
					putc(' ', pf_tmp);
				break;
			}
			case '+':case '-': {
				if (comment_flag2) {//在块注释内部
					break;
				}
				if (string_flag) {//在字符串内部
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' ')
					putc(' ', pf_tmp);
				vector<char> op;
				while (line[i] == '+' || line[i] == '-' || line[i] == ' ') {
					if (line[i] == ' ') {
						i++;
						continue;
					}
					op.push_back(line[i]);
					i++;
				}
				i--;
				switch (op.size()) {
				case 1:
					putc(op[0], pf_tmp);
					break;
				case 2:
					if (op[0] == op[1]) {//++ --
						putc(op[0], pf_tmp);
						putc(op[1], pf_tmp);
					}
					else {//+- -+
						putc('-', pf_tmp);
					}
					break;
				case 3:
					putc(op[0], pf_tmp);
					putc(op[1], pf_tmp);
					putc(' ', pf_tmp);
					putc(op[2], pf_tmp);
					break;
				case 4:
					putc(op[0], pf_tmp);
					putc(op[1], pf_tmp);
					putc(' ', pf_tmp);
					putc('-', pf_tmp);
					break;
				}
				putc(' ', pf_tmp);
				break;
			}
			default:
				if (comment_flag2&&line[i]!='\n') {//在块注释内部
					break;
				}
				if (line[i] == '\n') {
					LINE.push_back(line_num);//无分号的行结束时换行要存行号
				}
				putc(line[i], pf_tmp);
				break;
			}//switch
			if (comment_flag1) {
				LINE.push_back(line_num);
				putc('\n', pf_tmp);
				break;
			}
		}//for
		space_flag = 0;
		comment_flag1 = 0;
	}//while
	fclose(pf_in);
	fclose(pf_tmp);
}//PreDeal