/***
 lexical analysis 
 ***/
#include "pch.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
using namespace std;

//全局行计数器
long long int Row;
//全局列计数器
long long int Column;

fstream input;
fstream output;

//设置保留字部分
const string ReserveWord[15] =
{
	"program", "const", "var", "procedure", "begin", "if", "end", "while", "call", "read", "write", "then", "odd", "do"
};


//将搜索指示器回调一个位置，但并不需要将ch置为空白字符
void Retract() 
{
	if (!input.eof()) 
	{
		//基地址为当前字符流，偏移量为负一
		input.seekg(-1, ios::cur);
	}
}

//子程序，将ch之后的字符链接到word之后
string Concat(string word, char ch)
{
	word.push_back(ch);
	return word;
}

//判断当前读入字符是否是数字
int NumberCheck(char ch) {
	if (ch >= '0' && ch <= '9')
	{
		return true;
	}
	else 
	{
		return false;
	}
}

//判断当前读入字符是否是字母
int LetterCheck(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判断当前读入字符是否是保留字
int ReserveCheck(string word)
{
	int i = 0;
	for (i; i < 15; i++)
	{
		//分析为关键字
		if (word.compare(ReserveWord[i]) == 0)
		{
			//cout << word << "???";
			return true;
		}
		
	}
	return false;
}

//判断当前读入字符是否空格符
int Blankcheck(char ch) 
{
	//'\t'制表符 一次八个字符，比较少见
	//'\r'回车，回到当前行的行首，如果接着输出的话，本行之前的会被逐一覆
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
	{
		switch (ch)
		{
		case ' ':
			Column++;     //遇到空格列++
			break;
		case '\t':
			Column += 8;  //遇到制表符列数加8
			break;
		case '\n':
			Row++;       //遇到换行符行数加1
			Column = 1;
			break;
		case '\r':
			Column = 1;

		}//end case

		return true;
	}
	else
	{
		return false;
	}
}


//读入源文件，调用判断函数
int main() 
{

	//输入源txt文件
	input.open("3.txt", ios::in);

	//打开保存结果数据流的文件
	output.open("middle.txt", ios::out);

	//文件打开失败提醒
	if (!input.is_open()) {
		cout << "input file is Wrong!\n" << endl;
		exit(1);
	}
	if (!output.is_open()) {
		cout << "output file is Wrong!\n" << endl;
	}

	else
	{
		output << "*************************" << endl;
		output << "*** Lexical Analysis ***" << endl;
		output << "***   Reference:PL/0  ***" << endl;
		output << "*** FileSource:test.txt ***" << endl;
		output << "*************************" << endl;
	}

	//判断输入的字符
	char ch;

	//输出的单词整体
	string  word;

	while (!input.eof())
	{
		ch = input.get();

		//若当前字符是空白符
		if (Blankcheck(ch))
		{
			//不做输出,并将当前判断的word清零
			word = "";
		}

		//当前字符首元素是字母(可能为标识符或关键字)
		else if (LetterCheck(ch))
		{
			while ((NumberCheck(ch)) || LetterCheck(ch))
			{
				
				word = Concat(word, ch);
				//读取下一个字符
				Column++;
				ch = input.get();
			}


			//判断输入字符是否关键字
			if (ReserveCheck(word))
			{
				output << "{  " << word << "  }" << endl;
			}

			else {

				output << "{  " << word << ", ID  }" << endl;
			}
			word = "";

			Retract();
		}

		//若当前元素为数字
		else if (NumberCheck(ch))
		{
			while (NumberCheck(ch))
			{
				word = Concat(word, ch);
				Column++;
				ch = input.get();
			}

			//遇到非法数字或非法标识符
			if (LetterCheck(ch))
			{
				cout << "[ERROR]  Invalid ID :";
				output << "[ERROR]  Invalid ID :";

				while (LetterCheck(ch) || NumberCheck(ch))
				{
					word = Concat(word, ch);
					Column++;
					ch = input.get();
				}

				cout << word << " (Row " << Row << ", Column " << Column << ")" << endl;
				output << word << " (Row " << Row << ", Column " << Column << ")" << endl;

			}
			else {
				//                cout << strToken << ", INT" << endl;
				output << "{  " << word << ",INT  }" << endl;
			}

			Retract();
			word = "";
		}

		else
		{
			switch (ch)
			{

				//首字符为等号
			case '=':
				Column++;
				output << "{ = }" << endl;
				break;

				//首字符为>
			case'>':
				Column++;
				ch = input.get();
				if (ch == '=')
				{
					Column++;
					output << "{ >= }" << endl;
				}
				else {

					output << "{ > }" << endl;
					Retract();
				}
				break;

				//首字符为<
			case '<':
				Column++;
				ch = input.get();
				if (ch == '=')
				{
					Column++;
					output << "{ <= }" << endl;
				}
				else if (ch == '>')
				{
					Column++;
					output << "{ <> }" << endl;
				}
				else
				{
					output << "{ < }" << endl;
					Retract();
				}
				break;

				//首字符为:
			case ':':
				Column++;
				ch = input.get();
				if (ch == '=')
				{
					Column++;
					//						
					output << "{ := }" << endl;
				}
				else
				{
					cout << "[ERROR]  Missing “=”near the “:” (Row " << Row << ", Column " << Column << ")" << endl;
					output << "[ERROR] Missing “=”near the “：”  (Row " << Row << ", Column " << Column << ")" << endl;
					Retract;
				}
				break;

				//其他字符
			case '-':
			case '*':
			case '+':
			case '/':
			case ';':
			case '(':
			case ')':
			case ',':
				Column++;
				output <<"{  " <<ch << "  }" << endl;
				break;

				//遇到非法字符
			default:

				Column++;
				//判断是否为全角
				if ((ch >= 0xA1A1) && (ch <= 0xA1FE))
					cout << "[ERROR]  2 Byte :" << ch << "(Row " << Row << ", Column:" << Column << ")" << endl;
				else
				{
					cout << "[ERROR]  UNKOWN :" << ch << "(Row " << Row << ", Column " << Column << ")" << endl;
				}

			}
		}

		
	}
	input.close();
	output.close();
	cout << endl;
	cout << "Save results successfully" << endl;
	return 0;

}
		
