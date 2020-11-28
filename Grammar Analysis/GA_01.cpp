// GA_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>

using namespace std;

//全局行计数器
long long int Row=1;
//全局列计数器
long long int Column;

fstream input;
fstream output;

string line;
struct group
{

	std::string value; //单词的值
	std::string cate;    //单词的类别
	int Row;
	int Column;

	//void print();
}test,group[500];
int nn=0;       //词法分析计数
int ann = 0;    //语法分析计数
int mia = 1; //层数计数器

//设置保留字部分
const string ReserveWord[15] =
{
	"program", "const", "var", "procedure", "begin", "if", "end", "while", "call", "read", "write", "then", "odd", "do"
};

//函数声明
void Retract();
string Concat(string word, char ch);
int NumberCheck(char ch);
int LetterCheck(char ch);
int ReserveCheck(string word);
int Blankcheck(char ch);
int main();
void prog();
void block();
void condecl();
void const_();
void vardecl();
void proc();
void body();
void statement();
void exp();
void term();
void factor();
void lexp();

void lexp()
{
	cout << "开始分析lexp模块" << endl;
	test = group[ann];
	    if (test.value == "odd")
		{
			exp();
	    }
		else
		{
			exp();
			test = group[ann];
			if ((test.value != "=") && (test.value != "<>") && (test.value != "<") && (test.value != "<=") && (test.value != ">") && (test.value != "<="))
				cout << "Row" << test.Row << ",Column" << test.Column << "lexp比较运算符缺失" << endl;
			else
				ann++;
			exp();
		}
		cout << "lexp模块分析结束" << endl;
}
void factor()
{
	cout << "开始分析 factor" << endl;
	test = group[ann];
	if (test.cate == "ID")
		ann++;
	else if (test.cate == "INT")
		ann++;
	else if (test.value == "(")
	{
		ann++;
		exp();
		test = group[ann];
		if(test.value!=")")
			cout << "Row" << test.Row << ",Column" << test.Column << "factor的右括号缺失" << endl;
	}
	cout << " factor 分析结束" << endl;

}
void term()
{
	cout << "开始分析 term" << endl;
	test = group[ann];
	factor();
	test = group[ann];
	while (test.value == "*" || test.value == "/")
	{
		ann++;
		factor();
		test = group[ann];
	}
	cout << "term 分析结束" << endl;

}
void exp()
{
	cout << "开始分析 exp  模块" << endl;
	test = group[ann];
	if (test.value == "+" || test.value == "-")
	{
		ann++;
		term();
	}
	else
		term();
	test = group[ann];
	while (test.value == "+" || test.value == "-")
	{
		ann++;
		term();
		test = group[ann];
	}
	cout << "exp模块分析结束" << endl;

}

/*
<statement> → <id> := <exp>
			   |if <lexp> then <statement>[else <statement>]
			   |while <lexp> do <statement>
			   |call <id>（[<exp>{,<exp>}]）
			   |<body>
			   |read (<id>{，<id>})
			   |write (<exp>{,<exp>})
*/
void statement()
{
	cout << "*****进入 statement 模块*****" << endl;
	
	test = group[ann];

	// <id> := <exp>
	if (test.cate == "ID")
	{
		ann++;
		test = group[ann];
		if (test.value != ":=")
		{
			cout << ":=赋值符缺失" << endl;
		}
		//即使:=缺失也可以继续往下分析
		else ann++;
		exp();
	}
	else if (test.value == "if")
	{
		cout << "开始分析 if " << endl;
		ann++;
		lexp();
		test = group[ann];
		if (test.value != "then")
		{
			cout << "Row" << test.Row << ",Column" << test.Column << "if判断的then缺失" << endl;
		}
		else ann++;
		statement();
		test = group[ann];
		if (test.value == "else")
		{
			ann++;
			test = group[ann];
			statement();
		}
		cout << " if 分析结束 " << endl;

	}
	else if (test.value == "while")
	{
		cout << "开始分析 while " << endl;
		ann++;
		test = group[ann];
		lexp();
		test = group[ann];
		if (test.value != "do")
		{
			cout << "Row" << test.Row << ",Column" << test.Column << "while模块的do缺失" << endl;
		}
		else
		{
			ann++;
			statement();
		}
		cout << "while分析结束" << endl;
	}
	else if(test.value=="call")
	{
		cout << "开始分析 call" << endl;
		ann++;
		test = group[ann];
		if(test.cate!="ID")
			cout << "Row" << test.Row << ",Column" << test.Column << "call 模块 ID 缺失" << endl;
		else
		{
			ann++;
			test = group[ann];
		}
		if(test.value!="(")
			cout << "Row" << test.Row << ",Column" << test.Column << "while模块的(缺失" << endl;
		else
		{
			ann++;
			test = group[ann];
		}
		//有传递参数
		if (test.value != ")")
		{
			if ((test.value != "end") && (test.value != ";"))
			{
				ann++;
				test = group[ann];
				exp();
				test = group[ann];
				while (test.value == ",")
				{
					ann++;
					test = group[ann];
					exp();
					test = group[ann];
				}
			}
			else
			{
				cout << "Row" << test.Row << ",Column" << test.Column << "call 模块的 ) 缺失" << endl;
			}
		}
		if(test.value!=")")
			cout << "Row" << test.Row << ",Column" << test.Column << "call 模块的 )缺失" << endl;
		else
		{
			ann++;
			test = group[ann];
		}

		cout << "call 分析结束" << endl;
	}
	else if (test.value == "begin")
	{
		cout << "再次分析 body 模块" << endl;
		body();
    }
	else if(test.value=="read")
	{
	   cout << "开始分析read" << endl;
	   ann++;
	   test = group[ann];
	   if (test.value != "(")
	   {
		   cout << "Row" << test.Row << ",Column" << test.Column << "read模块 ( 缺失" << endl;
	   }
	   else
	   {
		   ann++;
		   test = group[ann];
	   }
	   if(test.cate!="ID")
		   cout << "Row" << test.Row << ",Column" << test.Column << "read模块 ID 缺失" << endl;
	   else 
	   {
		   ann++;
		   test = group[ann];
	   }
	   while (test.value == ",")
	   {
		   ann++;
		   test = group[ann];
		   if (test.cate == "ID")
		   {
			   ann++;
			   test = group[ann];
		   }
		   else
		   {
			   cout << "Row" << test.Row << ",Column" << test.Column << "read模块 ,后变量缺失" << endl;
		   }
	   }
	   if(test.value!=")")
		   cout << "Row" << test.Row << ",Column" << test.Column << "read模块 )缺失" << endl;
	   else
	   {
		   ann++;
		   test = group[ann];
	   }
	   cout << "read 分析结束" << endl;

	}
	else if (test.value == "write")
	{
	   cout << "开始分析 write" << endl;
	   ann++;
	   test = group[ann];
	   if (test.value != "(")
	   {
		   cout << "Row" << test.Row << ",Column" << test.Column << "write 模块 ( 缺失" << endl;
	   }
	   else
	   {
		   ann++;
		   test = group[ann];
	   }
	   exp();
	   test = group[ann];
	   while (test.value == ",")
	   {
		   ann++;
		   exp();
		   test = group[ann];
	   }
	   if (test.value != ")")
		   cout << "Row" << test.Row << ",Column" << test.Column << "write模块 )缺失" << endl;
	   else
	   {
		   ann++;
		   test = group[ann];
	   }
	   cout << "write 分析结束" << endl; 
    }
	
    
	cout << "*****statement 分析结束*****" << endl;
    
	
}

//会进入end之后的
void body()
{

	cout << "---------开始分析body---------" << endl;
	
	ann++;
	test = group[ann];
	statement();
	test = group[ann];

	//begin end可能嵌在body里
	while (test.value == ";"||group[ann-1].value=="end")
	{
		if (group[ann - 1].value != "end")
		{
			ann++;
			test = group[ann];
		}
		
		//嵌套begin end的判断
		statement();
	
		test = group[ann];
		
	}

	if (test.value == "end")
	{
		std::cout << "--------body分析结束---------" << endl;
		ann++;
		test = group[ann];
	}
	else
	{
		if( (test.cate == "ID"||ReserveCheck(test.value))&&(test.Row!=0))
		{
			//std::cout << test.value << "hhhhhhhh" << endl;
			std::cout << "Row" << test.Row << ",Column" << test.Column << "： statement缺少分号" << endl;
			/*ann++;
			test = group[ann];*/
			statement();
			test = group[ann];
			while (test.value == ";")
			{
				ann++;
				test = group[ann];
				//嵌套begin end的判断
				statement();
				test = group[ann];
			}
			if (test.value == "end")
			{
				std::cout << "--------body分析结束---------" << endl;
				ann++;
				test = group[ann];
				
			}

		}
		else
		{
			//cout << test.value << "this???" << endl;
			std::cout << "Row" << test.Row << ",Column" << test.Column << "： body模块 end缺失" << endl;
		}
		
	}
	
	
}

//proc在进入和出去时都会多分析一个词
void proc()
{
	cout << "------------开始分析proc----------" << endl;
	ann++;
	test = group[ann];
	if (test.cate == "ID")
	{
		ann++;
		test = group[ann];
		if (test.value == "(")
		{
			ann++;
			test = group[ann];
			if (test.cate == "ID")
			{
				ann++;
				test = group[ann];
			}
			while (test.value == ",")
			{
				ann++;
				test = group[ann];
				if (test.cate == "ID")
				{
					ann++;
					test = group[ann];
				}
			}
			if (test.value == ")")
			{
				ann++;
			}
			else
			{
				cout << "[GRAMMAR ERROR] " << "  procedure 参数右括号缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
				//cout << "Row" << test.Row << ",Column" << test.Column << ": procedure 参数右括号缺失" << endl;
			}

		}
		
	}
	
	
	test = group[ann];
	if (test.value != ";")
	{
		//cout << test.value << "^^^^^^^^^^^" << endl;
		cout <<"[GRAMMAR ERROR] "<<": proc分号缺失" << "  ( Row" << test.Row << ",Column" << test.Column <<")"<< endl;
	}
	else
	{
		ann++;
	}

	block();
	//cout << test.value << "help--------" << endl;
	/*ann++;
	test = group[ann];*/
	//cout << test.value << "what--------" << endl;
	if (test.value == "procedure")
	{
		//说明出现嵌套的procedure
		cout << "!!!!!!!!!!!进入嵌套定义的procedure!!!!!!!!!!!" << endl;
		proc();
		
		
	
	}
	cout << "------------proc分析结束------------" << endl;
}

void vardecl()
{
	cout << "开始分析vardecl" << endl;
	ann++;
	test = group[ann];
	if (test.cate == "ID")
	{
		ann++;
		test = group[ann];
	}
	else
	{
		cout << "[GRAMMAR ERROR] " << "  var定义缺少标识符" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
		//cout << "Row" << test.Row << ",Column" << test.Column << ": var定义缺少标识符" << endl;
	}
	while (test.value == ",")
	{
		ann++;
		test = group[ann];
		
		if (test.cate == "ID")
		{
			//cout << test.value << "kk" << endl;
			ann++;
			test = group[ann];
		}
		else
		{
			//cout << test.value << "why" << endl;
			cout << "[GRAMMAR ERROR] " << "  var定义缺少标识符" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
			//cout << "Row" << test.Row << ",Column" << test.Column << ": ??var定义缺少标识符" << endl;
		}	
	}
	if (test.value == ";")
	{
		ann++;
		test = group[ann];
	}
	else
	{
		test = group[ann - 1];
		cout << "Row" << test.Row << ",Column" << test.Column << "var定义缺少分号" << endl;

	}
	cout << "vardecl分析结束" << endl;

}


void const_()
{
	//cout << "开始分析const" << endl;
	test = group[ann];
	if (test.cate == "ID")
	{
		ann++;
		test = group[ann];
		//if(test.)
		if (test.value == ":=")
		{
			ann++;
			test = group[ann];
			/*cout << test.cate << "kkk"<<endl;
			cout << test.value << "yyy"<<endl;*/
			if (test.cate == "INT")
			{
				ann++;

			}
			else
			{
				if (test.cate == "ID")
				{
					ann++;
					test = group[ann];
					cout << "[GRAMMAR ERROR] " << "  const 赋值后不是数字" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
					//cout << "Row" << test.Row << ",Column" << test.Column << ": const 赋值后不是数字" << endl;
				}
				else if (test.value == "," || test.value == ";")
				{
					cout << "[GRAMMAR ERROR] " << "  const 赋值后缺少数字" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
					//cout << "Row" << test.Row << ",Column" << test.Column << ": const 赋值后缺少数字" << endl;
				}
			}
		}
		

	}
	else
	{
		cout << "[GRAMMAR ERROR] " << "  const 缺少标识符" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
		//cout << "Row" << test.Row << ",Column" << test.Column << "：const 缺少标识符" << endl;
		/*ann++;
		test = group[ann];
		cout << test.cate << "kkk" << endl;
		cout << test.value << "yyy" << endl;*/
		//if(test.)
		if (test.value == ":=")
		{
			ann++;
			test = group[ann];
			/*cout << test.cate << "kkk"<<endl;
			cout << test.value << "yyy"<<endl;*/
			if (test.cate == "INT")
			{
				ann++;

			}
			else
			{
				if (test.cate == "ID")
				{
					ann++;
					test = group[ann];
					cout << "Row" << test.Row << ",Column" << test.Column << ": const 赋值后不是数字" << endl;
				}
				else if (test.value == "," || test.value == ";")
				{
					cout << "Row" << test.Row << ",Column" << test.Column << ": const 赋值后缺少数字" << endl;
				}
			}
		}
	}
}



// <condecl> → const <const>{, <const>};
void condecl()
{
	cout << "开始分析condecl" << endl;
	ann++;
	const_();
	test = group[ann];
	while (test.value == ",")
	{
		ann++;
		const_();
		test = group[ann];
	}
	if (test.value == ";")
	{
		ann++;
		test = group[ann];
	}
	else
	{
		/*cout << test.cate << "kkk" << endl;
		cout << test.value << "yyy" << endl;*/
		test = group[ann-1];
		cout << "Row" << test.Row << ",Column" << test.Column << "分号错误" << endl;
	}
	test = group[ann];
	cout << "condecl分析结束" << endl;

}

/*
<block> → [<condecl>][<vardecl>][<proc>]<body>
<vardecl> → var <id>{,<id>};
<proc> → procedure <id>（[<id>{,<id>]}）;<block>{;<proc>}
*/

//block进出后，会前进一个单词,即end的下一个词语
void block()
{
	//flag为true,则block不止包含body
	bool flag = false;
	cout << "--------------开始分析block--------------" << endl;
	test = group[ann];
	
	if (test.cate == "const")
	{
		condecl();
		flag = true;
	}
	if (test.value == "var")
	{
		vardecl();
		flag = true;
	}
	if (test.value == "procedure")
	{
		proc();
		flag = true;
	}
	//cout << test.value << "%%%%%%%" << endl;
	/*if (flag)
	{
		ann++;
		test = group[ann];
	}*/
	//cout << test.value << "%%%%%%%" << endl;
	if (test.value == "begin")
	{
		body();
	}
	else
	{
		cout << "Row" << test.Row << ",Column" << test.Column << ": body模块 begin 缺失" << endl;
	}
	
	cout << "--------------block分析结束--------------" << endl;
}

void prog()
{
	ann = 0;
	test = group[ann];
	cout << "开始分析prog" << endl;
	if (test.value != "program")
	{
		ann++;
		test = group[ann];
		//有两种情况 
		if (test.value == ";")
		{
			ann--;
			cout << "Row" << test.Row << ",Column" << test.Column << ": program 缺失" << endl;
		}
		else if(test.cate=="ID")
		{
			cout << "Row" << test.Row << ",Column" << test.Column << ": program拼写错误" << endl;
		}
	}
	else
	{
		ann++;
	}
	test = group[ann];
	if (test.cate != "ID")
	{
		cout << "Row" << test.Row << ",Column" << test.Column << ": 标识符拼写错误" << endl;

	}
	else
	{
		ann++;
	}
	test = group[ann];
	if (test.value != ";")
	{
		cout << "[GRAMMAR ERROR] " << "  分号缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
		//cout << "Row" << test.Row << ",Column" << test.Column << ": 分号缺失" << endl;

	}
	else
	{
		ann++;
	}
	cout << "prog分析结束" << endl;
	block();

}

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
	//input.open("test.txt", ios::in);
	input.open("new.txt", ios::in);

	//打开保存结果数据流的文件
	output.open("result.txt", ios::out);

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
			
				group[nn].value = word;
				group[nn].cate = word;     //关键字类别即其本身
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;

				output << "{  " << word << "  }" << endl;
			}

			//非关键字，而是标识符
			else 
			{
				group[nn].value = word;
				group[nn].cate = "ID";
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;

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
				cout << "[LEXICAL ERROR]  Invalid ID :";
				output << "[LEXICAL ERROR]  Invalid ID :";

				while (LetterCheck(ch) || NumberCheck(ch))
				{
					word = Concat(word, ch);
					Column++;
					ch = input.get();
				}

				cout << word << " (Row " << Row << ", Column " << Column << ")" << endl;
				output << word << " (Row " << Row << ", Column " << Column << ")" << endl;

			}
			else 
			{
				group[nn].value = word;
				group[nn].cate = "INT";
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;
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
            //关系运算符为lop

				//首字符为等号
			case '=':

				group[nn].value = ch;
				group[nn].cate = "=";
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;
				output << "{ = }" << endl;
				break;

				//首字符为>
			case'>':
				Column++;
				ch = input.get();
				if (ch == '=')
				{

					group[nn].value = ">=";
					group[nn].cate = "lop";
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
					Column++;
					output << "{ >= }" << endl;
				}
				else {

					group[nn].value = ">";
					group[nn].cate = "lop";
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
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
					group[nn].value = "<=";
					group[nn].cate = "lop";
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
					Column++;
					output << "{ <= }" << endl;
				}
				else if (ch == '>')
				{
					group[nn].value = "<>";
					group[nn].cate = "lop";
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
					Column++;
					output << "{ <> }" << endl;
				}
				else
				{
					
					group[nn].value = "<";
					group[nn].cate = "lop";
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
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
					group[nn].value = ":=";
					group[nn].cate = "state"; //句子表达式中
					group[nn].Row = Row;
					group[nn].Column = Column;
					nn++;
					output << "{ := }" << endl;
				}
				else
				{
					cout << "[LEXICAL ERROR]  Missing “=”near the “:” (Row " << Row << ", Column " << Column << ")" << endl;
					output << "[LEXICAL ERROR] Missing “=”near the “：”  (Row " << Row << ", Column " << Column << ")" << endl;
					cout << ch << "before" << endl;
					input.seekg(-1, ios::cur);
				    
					//ch = input.get();
					//cout << ch << "after" << endl;
				}
				break;

				//其他字符
			case '-':
			case '*':
			case '+':
			case '/':
				Column++;
				group[nn].value = ch;
				group[nn].cate = "ope";  //operator
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;
				output << "{  " << ch << "  }" << endl;
				break;
			case ';':
			case '(':
			case ')':
			case ',':
				Column++;
				group[nn].value = ch;
				group[nn].cate = "biaodianfu";  //operator
				group[nn].Row = Row;
				group[nn].Column = Column;
				nn++;
				output << "{  " << ch << "  }" << endl;

				/*ch = input.get();
				Retract();*/
				break;
				//遇到非法字符
			default:

				Column++;
				//判断是否为全角
				if ((ch >= 0xA1A1) && (ch <= 0xA1FE))
					cout << "[LEXICAL ERROR]  2 Byte :" << ch << "(Row " << Row << ", Column:" << Column << ")" << endl;
				else
				{
					/*input.seekg(-3, ios::cur);
					ch = getchar();*/
					/*if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')*/
					cout << "[LEXICAL ERROR]  UNKOWN :" << ch << "(Row " << Row << ", Column " << Column << ")" << endl;
					
					
				}

			}
		}


	}
	input.close();
	output.close();

	//输出保存的单词组
	/*for (int i = 0; i < nn; i++)
	{
		cout<<group[i].value<<" ";
		cout<<group[i].cate <<endl;
	
	}*/
	prog();
	cout << endl;
	cout << "Save results successfully" << endl;
	return 0;

}