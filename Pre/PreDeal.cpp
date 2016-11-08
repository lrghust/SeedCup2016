#include"head.h"

//Ԥ����ӿո�ɾע�ͣ����кţ���Ϊһ��ֻ��һ�����
void PreDeal(void) {
	FILE *pf_in = fopen("input.txt", "r+");
	if (!pf_in) {
		cout << "�ļ���ʧ�ܣ�" << endl;
		exit(-1);
	}
	FILE *pf_tmp = fopen(".tmp", "w+");
	char line[MAXLEN + 1];//�����һ��
	int space_flag = 0;//0��ʾ���׵Ŀ��ַ�,1��ʾ�Ѷ����ǿ��ַ�
	int string_flag = 0;//�Ƿ����printf�е��ַ���
	int comment_flag1 = 0;//�Ƿ������ע��
	int comment_flag2 = 0;//�Ƿ�����ע��
	int bracket_flag = 0;//�Ƿ�������ż�for��Ķ�����䲻����
	int block_flag = 0;
	int line_num = 0;//��ʾԴ�ļ�����к�
	LINE.push_back(0);//����������Ԫ�أ�������Ϊ1��ʼʹ��
	putc('{', pf_tmp);//�ļ���ͷ�������һ��������ű��ڽ��п鴦��
	putc('\n', pf_tmp);
	LINE.push_back(0);//�����ļ���ͷ������ӵ��������

	while (fgets(line, MAXLEN + 1, pf_in)) {
		line_num++;//��ǰ�����е��к�
		for (int i = 0;line[i] != '\0';i++) {

			if (!space_flag && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n'))//�������׿��ַ�
				continue;
			if (!string_flag && !comment_flag2&&line[i] == '/'&&line[i + 1] == '/') {//����ע��
				break;
			}
			//��Ӵ�����
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
			space_flag = 1;//�����ǿ��ַ�

			switch (line[i]) {
			case '"'://����printf�е��ַ�������������Ĵ���
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				/*if (line[i - 1] != '\\') {//������˫���Ų����ַ�����ת���˫����
					string_flag = !string_flag;//ȡ��,�����ڶ��ζ���"�����˳��ַ���
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
				if (comment_flag2 && line[i - 1] == '*') {//�˳���ע��
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
				if (line[i + 1] == '/') {//������ע��
					comment_flag1 = 1;
				}
				else if (line[i + 1] == '*') {//�����ע��
					comment_flag2 = 1;
				}
				else {//��ͨ����
					if (line[i - 1] != ' ')//�ڷ���ǰ�ӿո�
						putc(' ', pf_tmp);
					putc(line[i], pf_tmp);//�������
					if (line[i + 1] != ' ')//�ڷ��ź�ӿո�
						putc(' ', pf_tmp);
				}
				break;
			case '=':
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' '&&line[i - 1] != '>'&&line[i - 1] != '<'&&line[i - 1] != '!'&&line[i - 1] != '=')//�ڵȺ�ǰ�ӿո�
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//����Ⱥ�
				if (line[i + 1] != ' '&&line[i + 1] != '=')//�ڵȺź�ӿո�
					putc(' ', pf_tmp);
				break;
			case '(':
			case ')':
				if (comment_flag2) {//��ע���ڲ����Ų�����
					break;
				}
				if (string_flag) {//���ַ����ڲ�
					putc(line[i], pf_tmp);
					break;
				}
				bracket_flag = !bracket_flag;//���λȡ����������˳�����
				if (line[i - 1] != ' ')//�ڷ���ǰ�ӿո�
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//�������
				if (line[i + 1] != ' ')//�ڷ��ź�ӿո�
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
				if (comment_flag2) {//��ע���ڲ����Ų�����
					break;
				}
				if (string_flag) {//���ַ����ڲ�
					putc(line[i], pf_tmp);
					break;
				}
				putc(line[i], pf_tmp);
				putc('\n', pf_tmp);
				LINE.push_back(line_num);
				break;
			case ',':
			case '*':
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {//���ַ����ڲ�
					putc(line[i], pf_tmp);
					break;
				}
				if (line[i - 1] != ' ')//�ڷ���ǰ�ӿո�
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//�������
				if (line[i + 1] != ' ')//�ڷ��ź�ӿո�
					putc(' ', pf_tmp);
				break;
			case ';':
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {//���ַ����ڲ�
					putc(line[i], pf_tmp);
					break;
				}
				if (bracket_flag) {//�������ڲ�
					if (line[i - 1] != ' ')//�ڷ���ǰ�ӿո�
						putc(' ', pf_tmp);
					putc(line[i], pf_tmp);//�������
					if (line[i + 1] != ' ')//�ڷ��ź�ӿո�
						putc(' ', pf_tmp);
					break;
				}
				LINE.push_back(line_num);
				if (line[i - 1] != ' ')//�ڷ���ǰ�ӿո�
					putc(' ', pf_tmp);
				putc(line[i], pf_tmp);//�������
				putc('\n', pf_tmp);//������з�
				space_flag = 0;
				break;
			case '>':case '<': {
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {//���ַ����ڲ�
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
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {//���ַ����ڲ�
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
				if (comment_flag2) {//�ڿ�ע���ڲ�
					break;
				}
				if (string_flag) {//���ַ����ڲ�
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
				if (comment_flag2&&line[i]!='\n') {//�ڿ�ע���ڲ�
					break;
				}
				if (line[i] == '\n') {
					LINE.push_back(line_num);//�޷ֺŵ��н���ʱ����Ҫ���к�
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