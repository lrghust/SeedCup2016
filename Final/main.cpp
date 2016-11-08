#include"cppjieba/Jieba.hpp"
#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<vector>
#define MAXLEN 255

using namespace std;

const char* const DICT_PATH = "dict//jieba.dict.utf8";
const char* const HMM_PATH = "dict//hmm_model.utf8";
const char* const USER_DICT_PATH = "dict//user.dict.utf8";
const char* const IDF_PATH = "dict//idf.utf8";
const char* const STOP_WORD_PATH = "dict//stop_words.utf8";

const int NUM = 50;
const int RET = 50;

vector<cppjieba::KeywordExtractor::Word> keywordres1[4];
vector<cppjieba::KeywordExtractor::Word> keywordres2[4];

FILE *in = NULL;
FILE *out = NULL;

void CutWord(void);
void Weight(void);
void Calculator(void);
double Max(vector<double> tmp);
double Levenshtein(string str1, string str2);

int main(int argc, char** argv) {
	in = fopen(argv[1], "r");
	out = fopen(argv[2], "w+");
	CutWord();
	Weight();
	Calculator();
	fclose(in);
	fclose(out);
	return 0;
}

void CutWord(void) {
	cppjieba::Jieba jieba(
		DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH);
	vector<string> words;
	vector<cppjieba::Word> jiebawords;
	string s;
	string result;

	char keyword[2][5][MAXLEN];
	for (int i = 0;i <= 1;i++) {
		char str[MAXLEN];
		fgets(str, MAXLEN, in);
		int pos = 0;
		int key_pos = 0;
		int j = 0;
		while (str[pos] != '\n')
		{
			if (str[pos] == ',') {
				keyword[i][key_pos][j] = '\0';
				pos++;
				key_pos++;
				j = 0;
				continue;
			}
			else {
				keyword[i][key_pos][j] = str[pos];
				pos++;
				j++;
			}
		}
		keyword[i][key_pos][j] = '\0';
		/*for(int k=0;k<=4;k++){
		puts(keyword[k]);
		putchar('\n');
		}*/
	}
	for (int i = 1;i <= 4;i++) {
		//jieba.Cut(keyword[0][i], words, true);
		//cout << limonp::Join(words.begin(), words.end(), "/") << endl;
		const size_t topk = 50;
		jieba.extractor.Extract(keyword[0][i], keywordres1[i - 1], topk);
		jieba.extractor.Extract(keyword[1][i], keywordres2[i - 1], topk);
		//cout << keywordres << endl;

	}

}

double Levenshtein(string str1, string str2)
{
	int len1 = str1.length();
	int len2 = str2.length();

	//动态分配二维数组
	int **dif = (int**)malloc(sizeof(int*)*(len1 + 1)); //开辟行
	for (int i = 0; i < len1 + 1; i++)
	{
		*(dif + i) = (int*)malloc(sizeof(int)*(len2 + 1));//开辟列
	}

	//初始化
	for (int a = 0; a <= len1; a++)
	{
		dif[a][0] = a;
	}
	for (int a = 0; a <= len2; a++)
	{
		dif[0][a] = a;
	}

	int temp;
	for (int i = 1; i <= len1; i++)
	{
		for (int j = 1; j <= len2; j++)
		{
			if (str1[i - 1] == str2[j - 1])
			{
				temp = 0;
			}
			else
			{
				temp = 1;
			}
			// 取三个值中最小的
			dif[i][j] = min(min(dif[i - 1][j - 1] + temp, dif[i][j - 1] + 1), dif[i - 1][j] + 1);
		}
	}

	// 计算相似度
	double similarity = 1 - (float)dif[len1][len2] / max(str1.length(), str2.length());

	for (int i = 0; i < len1 + 1; i++)
		free(*(dif + i));

	return similarity;
}

void Weight(void)
{

	for (int k = 0; k < 4; k++)
	{
		//cout << endl;
		//cout << k << ":" << endl;
		int str1_size = keywordres1[k].size();
		int str2_size = keywordres2[k].size();
		for (int i = 0; i < str1_size; i++)
		{
			vector<double> tmp;
			string str1 = (keywordres1[k].begin() + i)->word;
			for (int j = 0; j < str2_size; j++)
			{
				string str2 = (keywordres2[k].begin() + j)->word;
				tmp.insert(tmp.begin() + j, Levenshtein(str1, str2));
			}
			(keywordres1[k].begin() + i)->weight *= Max(tmp);
			//cout << endl;
			//cout << i << ":" << "Max(tmp)" << Max(tmp) << endl;
			tmp.~vector <double>();
			//cout << (keywordres1[k].begin() + i)->weight << endl;
		}
	}

}


//计算tmp中的最大值
double Max(vector<double> tmp)
{
	int length = tmp.size();
	double eps = 0.00001;
	double maxmum = 0;


	for (int i = 0; i < length - 1; i++)
	{
		if (tmp[i] - tmp[i + 1] > eps)
		{
			maxmum = tmp[i];
		}
	}

	return maxmum;
}


void Calculator(void)
{
	int i, j;
	double ret_value[4];//计算分段总值
	double ret = 0;

	for (j = 0; j < 4; j++)
	{
		int size = keywordres1[j].size();
		ret_value[j] = 0;
		for (i = 0; i < size; i++)
		{
			ret_value[j] += (keywordres1[j].begin() + i)->weight;
		}
	}

	for (i = 0; i < 4; i++)
	{
		ret += ret_value[i];
	}
	/*fprintf(out, "%lf", ret);
	cout << endl;
	cout << endl;
	cout << "总的相似度：" << ret << endl;*/
	if (ret > RET) {
		putc('1', out);
	}
	else
		putc('0', out);
}
