// GA_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//opr 0 1  栈顶取反
//opr 0 2  栈顶加栈顶之下的和
//opr 0 3  栈顶下减栈顶
//opr 0 4  栈顶乘以栈顶下
//opr 0 5   栈顶下除以栈顶上
//opr 0 6   栈顶取余，1为奇，0为偶
//opr 0 7   栈顶下是否和栈顶元素相同
//opr 0 8   栈顶下是否和栈顶不相等
//opr 0 9   栈顶下是否小于栈顶元素
//opr 0 10   栈顶下是否小于等于栈顶元素
//opr 0 11   栈顶下是否大于栈顶元素
//opr 0 12   栈顶下是否大于等于栈顶元素

//opr 0 13  移动栈顶的形式参数到相应位置
//opr 0 14  输出换行符
//#pragma warning(disable:4996)；
//#define _CRT_SECURE_NO_WARNINGS；
#include "pch.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>

using namespace std;

//全局行计数器
long long int Row = 1;
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
}test, group[500];
int nn = 0;       //词法分析计数
int ann = 0;    //语法分析计数
int mia = 1; //层数计数器

//设置保留字部分
const string ReserveWord[15] =
{
	"program", "const", "var", "procedure", "begin", "if", "end", "while", "call", "read", "write", "then", "odd", "do"
};

#define table_max 100   //定义符号表最多个数
#define codemax 1000
#define levmax  3  //假定pl0定义的嵌套层次最多为3
#define stacksize 1000 


long tx;		// current table index 此处定义为全局
//int code_number = 0; //标志代码位置
int cx = 0;       //初始化为0

long dx;		// data allocation index
long lev;		// current depth of block nesting
long table_number=0;  //总的符号表个数 
long mother_table = 0;
long rn_table = 0; //目前的table
int stack[stacksize];    
bool run_flag = true;
//int father_t[table_max];

enum object {
	constant,
	variable,
	procedure,
	formal_parameter//形参
};


struct tablestruct 
{
	char  name[15];
	enum object kind;
	int val;    //值
	int adr;    //地址
	int size;   //存储空间
	int level;	//所处层
	//struct table *next;	
	long mother_t;  //保存定义的直接外层符号表名
	long t_number;
};

struct tablestruct table[table_max];


enum  function {
	LIT,
	OPR,
	LOD,
	STO,
	CAL,
	INT,
	JMP,
	JPC,
	RED,
	WRT
};


struct code_struct
{
	enum function f;            //代码功能，伪操作码
	int l;                     //变量引用层和声明层的层次差
	int a;                     //根据f的不同而不同
};
struct code_struct code[codemax];



//函数声明

/*lev :层数
  tx ：table表的下标指针,是以值参数形式使用的。
  dxx: 如果有形参，从new_dx开始
  txx: procedure 在table中定义的位置
  */


void Retract();
string Concat(string word, char ch);
int NumberCheck(char ch);
int LetterCheck(char ch);
int ReserveCheck(string word);
int Blankcheck(char ch);
int main();
void prog();
void block(int proc_t);
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

//new
//oid new_Block(int lev, int &table_flag, int new_dx, int txx);
int  gen(enum function x, int y, int z);
void Enter(enum object which);
/*形参专用登记函数*/
void Enter_fp(enum object which);
void Error(int type);
long position(const char *id);
void interpreter();
void printcode();
void printstack();
int  base(int b, int l);
void printtable();

void printstack(int end)
{
	cout << endl;
	for (int i = end; i > 0; i--)
	{
		cout << "——————" << endl;
		cout<<"    "<< stack[i] << endl;

	}
	cout << "********************" << endl;
	cout  << endl;
}
void printtable()
{
	string test;
	//int mm = tx;
	cout << " \t " << "index" << " \t " <<"name" << " \t " << "\t" << "kind" << " \t " << "level" << "\t" << "val" <<"\t"<<"adr"<< endl;
	for (int i = 1; i <= tx; i++)
	{
		test = table[i].name;
		if(test=="fibonacci")
		cout << " \t " << i << " \t " << test << "\t" << " "<< table[i].kind << " \t " << table[i].level << "\t" << table[i].val << "\t" << table[i].adr << endl;
		else
		cout << " \t " << i << " \t " << test << "\t"<< " \t "<< table[i].kind << " \t " << table[i].level << "\t" << table[i].val <<"\t"<< table[i].adr<<endl;
	}
		

}
//通过静态链找变量所在层的基地址
//l 为层差,b为此次过程基址 
int base(int b, int l)
{
	int make;
	make = b;
	//迭代一层就跑到前一个
	if (l > 0)
	{
		while (l > 0)
		{
			make = stack[make];

			l = l - 1;
		}

	}
	else if (l < 0)
	{
		while (l <0)
		{
			make = stack[make];

			l = l +1;
		}
	}
	



	return make;
}

void interpreter()
{

	long PR;             //下一条要执行的程序地址寄存器
	long sp;             //存放当前活动记录的基地址 sp
	long top;             //数据栈栈顶top
	code_struct  IR;    //指令寄存器

	top = 0; sp = 1; PR = 0;  //Stack从1开始存储
	//t      b       p
	stack[1] = 0; stack[2] = 0; stack[3] = 0;
	int move = 3; //用在形参上
	do
	{
		IR = code[PR]; 
		PR = PR + 1;
		if (top >= stacksize)
		{
			Error(12);
			return;
		}
		switch (IR.f)
		{
		case LIT:
			top = top + 1; 
			stack[top] = IR.a;
			break;
		case OPR:
			switch (IR.a)
			{
			case 0://return
				//printstack(top);
				top = sp - 1;//top指到前一个活动记录
				PR = stack[top + 3];   //程序回到调用点，即返回地址RA
				sp = stack[top + 2];      // 根据动态链找调用前的基地址
				break;
			case 1://opr 0 1  栈顶取反
				stack[top] = stack[top];
				break;
			case 2: //opr 0 2  栈顶加栈顶之下的和
				top = top - 1;
				stack[top] = stack[top] + stack[top + 1];
				break;
			case 3:  //opr 0 3  栈顶下减栈顶
				top = top - 1;
				stack[top] = stack[top] - stack[top + 1];
				break;
			case 4:  //opr 0 4  栈顶乘以栈顶下
				top = top - 1;
				stack[top] = stack[top] * stack[top + 1];
				break;
			case 5:   //opr 0 5   栈顶下除以栈顶上
				top = top - 1;
				if (stack[top] == 0)
				{
					Error(11);
					return;
				}
				stack[top - 1] = stack[top - 1] / stack[top];
				break;
			case 6:   //opr 0 6   栈顶取余，1为奇，0为偶
				top = top - 1;
				stack[top] = stack[top] % 2;
			case 7:

				top = top - 1;
				stack[top] = (stack[top] == stack[top + 1]);
				break;
			case 8:
				top = top - 1;
				stack[top] = (stack[top] != stack[top + 1]);
				break;
			case 9:
				top = top - 1;
				stack[top] = (stack[top] < stack[top + 1]);
				break;
			case 10:
				top = top - 1;
				stack[top] = (stack[top] <= stack[top + 1]);
				break;
			case 11:
				top = top - 1;
				stack[top] = (stack[top] > stack[top + 1]);
				break;
			case 12:
				top = top - 1;
				stack[top] = (stack[top] >= stack[top + 1]);
				break;
			case 13:
				if (top + move >= stacksize)
				{
					Error(12);
					return;
				}
				//根据栈顶位置来存放
				//cout << stack[top] << "test" << endl;
				stack[top + move] = stack[top];
				top = top - 1;
				move++;
				break;
			case 14:
				std::cout << '\n';
				break;
			}
			break;
		case LOD:
			top = top + 1;
			stack[top] = stack[base(sp, IR.l) + IR.a];
			//cout << stack[top] << "wuwu" << endl;
			break;
		case STO:
			stack[base(sp, IR.l) + IR.a] = stack[top];
			//cout << stack[top] << "wuwu" << endl;
			top = top - 1;
			break;
		case CAL:   //入口地址为a,层次差为l
			/*s[t+1] 中存放前一个层级的基地址位置，
			s[t+2] 中存放当前的静态基地址 b，
			s[t+3] 中存放当前的程序指针 p。
			s[t+1] 这个位置是像链表一样指向前一个层级的基地址位置，
			以应用层级差来找到变量的存储位置*/
			if (top + 3 >= stacksize)
			{
				Error(12);
				return;
			}

			//静态链，保存定义者的基地址
			stack[top + 1] = base(sp, IR.l);
			//动态链，前一活动记录的基地址
			stack[top + 2] = sp;
			//返回地址
			stack[top + 3] = PR; 
			sp = top + 1;  //sp在此处改 然后int改Top
			//下一条指令执行位置
			PR = IR.a;
			move = 3;
			break;
		case INT:
			top += IR.a;
			break;
		case JMP:
			 PR = IR.a;
			 break;
		case JPC:
			if (stack[top] == 0)
			{
				PR = IR.a;
			}
			top = top - 1;
			break;
		case RED:
			cout << "请输入";
			cin >> stack[top+1];
			stack[base(sp, IR.l) + IR.a] = stack[top+1];
			break;
		case WRT:
			cout << stack[top];

				
		}
	}while (PR != 0);  //指令地址寄存器指向0 ，即前一个活动记录退出，程序结束


}

void printcode()
{
	//char  keeping[5];
	cout << "************  Intermediate code  ************" << endl;
	int i;
	char f[5];
	for (i = 0; i <= cx - 1; i++)
	{
		
			switch (code[i].f)
			{
			case LIT:
				strcpy(f, "lit");
				break;
			case OPR:
				strcpy(f, "opr");
				break;
			case LOD:
				strcpy(f, "lod");
				break;
			case STO:
				strcpy(f, "sto");
				break;
			case CAL:
				strcpy(f, "cal");
				break;
			case INT:
				strcpy(f, "int");
				break;
			case JMP:
				strcpy(f, "jmp");
				break;
			case JPC:
				strcpy(f, "jpc");
				break;
			case RED:
				strcpy(f, "red");
				break;
			case WRT:
				strcpy(f, "wrt");
				break;
			}
		cout << "(" << i << ")  "<< f << " " << code[i].l << " " << code[i].a << endl;
	}
	cout << "************  Intermediate code  ************" << endl;
}
void lexp()
{
	string lop;
	cout << "开始分析lexp模块" << endl;
	test = group[ann];
	if (test.value == "odd")
	{
		ann++;
		exp();
		gen(OPR, 0, 6);
		//最后返回栈顶为1或0
	}
	else
	{
		
		exp();
		test = group[ann];
		lop = test.value;
		if ((test.value != "=") && (test.value != "<>") && (test.value != "<") && (test.value != "<=") && (test.value != ">") && (test.value != "<="))
			cout << "Row" << test.Row << ",Column" << test.Column << "lexp比较运算符缺失" << endl;
		else
			ann++;

		exp();
		if (!strcmp(lop.c_str(), "="))
		{
			gen(OPR, 0, 7);
		}
		else if (!strcmp(lop.c_str(), "<>"))
		{
			gen(OPR, 0, 8);
		}
		else if (!strcmp(lop.c_str(), "<"))
		{
			gen(OPR, 0, 9);
		}
		else if (!strcmp(lop.c_str(), "<="))
		{
			gen(OPR, 0, 10);
		}
		else if (!strcmp(lop.c_str(), ">"))
		{
			gen(OPR, 0, 11);
		}
		else if (!strcmp(lop.c_str(), ">="))
		{
			gen(OPR, 0, 12);
		}
	}
	cout << "lexp模块分析结束" << endl;
}
/* <factor>→<id>|<integer>|(<exp>) */
void factor()
{
	long i;
	long num_keep;
	cout << "开始分析 factor" << endl;
	test = group[ann];
	//cout<< test.value<<"&&&&&&&&&&&&&&&&&&" <<endl;
	if (test.cate == "ID")
	{
		i = position(test.value.c_str());
	
		if (i == 0)
		{
			Error(3);

		}
		else
		{
			switch (table[i].kind)
			{
			case constant:
				gen(LIT, 0, table[i].val);
				break;
			case variable:
				gen(LOD, lev - table[i].level, table[i].adr);
				break;
			case procedure:
				Error(5);
				break;
			case formal_parameter:
				gen(LOD, lev - table[i].level, table[i].adr);
				break;
			}
		}



		ann++;

	}
		
	else if (test.cate == "INT")
	{
		num_keep =atoi( test.value.c_str());
		gen(LIT, 0, num_keep);
		ann++;
	}
	
	else if (test.value == "(")
	{
		ann++;
		exp();
		test = group[ann];
		if (test.value != ")")
			cout << "Row" << test.Row << ",Column" << test.Column << "factor的右括号缺失" << endl;
	}
	cout << " factor 分析结束" << endl;

}
void term()
{
	string mop;
	cout << "开始分析 term" << endl;
	test = group[ann];
	factor();
	test = group[ann];
	while (test.value == "*" || test.value == "/")
	{
		mop = test.value;
		ann++;
		factor();
		if (mop == "*")
		{
			gen(OPR, 0, 4);
		}
		else if (mop == "/")
		{
			gen(OPR, 0, 5);
		}
		test = group[ann];
	}
	cout << "term 分析结束" << endl;

}
void exp()
{
	string aop;

	cout << "开始分析 exp  模块" << endl;
	test = group[ann];
	if (test.value == "+" || test.value == "-")
	{
		aop = test.value;
		ann++;
		term();
		if (aop == "-")
		{
			gen(OPR, 0, 1);
		}

	}
	else
		term();
	test = group[ann];
	while (test.value == "+" || test.value == "-")
	{
		aop = test.value;
		ann++;
		term();
		if (aop == "+")
		{
			gen(OPR, 0, 2);
		}
		else{
			gen(OPR, 0, 3);
		}

		test = group[ann];
	}
	cout << "exp模块分析结束" << endl;

}



void Error(int type)
{
	if (type < 11)
	{
		run_flag = false;
	}

	cout << "第" << group[ann].Row << "行  第" << group[ann].Column << "列";
	switch (type)
	{
	case 0:
		cout << "[Semantic Error] you have defined too many levels " << endl;
		break;
	case 1:
		cout << " [Semantic Error] you have redeclared procedure " << endl;
		break;
	case 2:
		cout << " [Semantic Error] you have redeclared variable or constant " << endl;
		break;
	case 3:
		cout << "  [Semantic Error] you have Undeclared identifier " << endl;
		break;
	case 4:
		cout << "  [Semantic Error] you have Illegal assignment " << endl;
		break;
	case 5:
		cout << "  [Semantic Error5]  Illegal use of Procedure " << endl;
		break;
	case 6:
		cout << "  [Semantic Error] you have Undeclared procedure" << endl;
		break;
	case 7:
		cout << "  [Semantic Error] formal parameter mismatch" << endl;
		break;
	case 8:
		cout << "  [Semantic Error] constant or variable can not be called" << endl;
		break;
	case 9:
		cout << "  [Semantic Error] constant or procedure can not be read" << endl;
		break;
	case 10:
		cout << "  [Semantic Error] you have redeclared formal parameter" << endl;
		break;
	case 11:
		cout << "  [Running Error]  division error" << endl;
		break;
	case 12:
		cout << "  [Running Error]  stack overflows" << endl;
		break;
	}


}


/*查找符号在符号表的位置，若返回i为0表示没有查找到*/
/*改进为可以定位到局部变量版*/
long position(const char *id)
{
	long keep_lev = lev;

	long i;
	strcpy(table[0].name, id);
	i = tx;


	while (i != 0)
	{
		if (keep_lev - 1 == table[i].level)
			keep_lev--;

		//if (table[i].level == 1)
			//cout << i << "???"<<endl;

		if ((strcmp(table[i].name, id) == 0) && (keep_lev == table[i].level))
		{

			break;
		}

		i = i - 1;
	}
	return i;

}

void Enter(enum object which)
{
	int i;
	if (which == procedure)
	{

		i = position(group[ann].value.c_str());
		if (i != 0 && table[i].kind == procedure)
			Error(1);
	}
	else
	{
		i = position(group[ann].value.c_str());
		if (i != 0 && (table[i].level == lev))
		{
			if (table[i].kind == constant || table[i].kind == variable|| table[i].kind == formal_parameter)
			{
			    Error(2);
			}
		}
			
			
	}
	tx = tx + 1;    //符号表新增一项
	strcpy(table[tx].name, group[ann].value.c_str());
	table[tx].kind = which;
	
	switch (which)
	{
	case constant:
		table[tx].level = lev;	
		//字符串转为数字
		table[tx].val = atoi(group[ann].value.c_str()) ;
		strcpy(table[tx].name, group[ann-2].value.c_str());
		table[tx].t_number = table_number;
		table[tx].mother_t = mother_table;
		break;
	case variable:
		table[tx].level = lev;
		table[tx].adr = dx;    //设置为相对该层基地址偏移地址
		dx++;
		table[tx].t_number = table_number;
		table[tx].mother_t = mother_table;
		break;
	case procedure:
		table[tx].level = lev;
		table[tx].t_number = table_number;
		table[tx].mother_t = mother_table;
		//procedure的size和adr后面补填
		break;
	}

}

void Enter_fp(enum object which)
{
	int i;
	
		i = position(group[ann].value.c_str());
		if (i != 0 && (table[i].level == lev))
			if (table[i].kind == constant || table[i].kind == variable|| table[i].kind==formal_parameter)
			{
				Error(10);
			}
			
	
	    tx = tx + 1;    //符号表新增一项
	    strcpy(table[tx].name, group[ann].value.c_str());
	    table[tx].kind = which;
		//cout << "~~~~~~~~~~~~~~~~~" << tx << endl;
		table[tx].t_number = table_number;
		table[tx].level = lev;
		table[tx].adr = dx;    //设置为相对该层基地址偏移地址
		table[tx].mother_t = mother_table;
		dx++;

}

/*目标代码序号从零开始*/
int  gen(enum function x, int y, int z)
{
	if (cx >= codemax)
	{
		cout << "program too long" << endl;
		return 0;
	}
	else
	{
		code[cx].f = x;
		code[cx].l = y;
		code[cx].a = z;
		cx++;
		return 0;
	}


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
	long i;
	long cx1,cx2;  //用在地址回填里
	//cout << "*****进入 statement 模块*****" << endl;

	test = group[ann];

	// <id> := <exp>
	if (test.cate == "ID")
	{
		i = position(test.value.c_str());
		//cout << "@@@@@@@@@@@@@@@@@@@@" << i << endl;
		if (i == 0)
		{
			Error(3);
		}
		else if (table[i].kind != variable&&(table[i].kind != formal_parameter))
		{
			cout << table[i].kind << endl;
			Error(4); i = 0;
			//cout << "$$$$$$$$$$" << endl;
		}
		ann++;
		test = group[ann];
		if (test.value != ":=")
		{
			cout << ":=赋值符缺失" << endl;
		}
		//即使:=缺失也可以继续往下分析
		else ann++;
		exp();
		if (i != 0)
		{
			gen(STO, lev - table[i].level, table[i].adr);
		}



	}
	else if (test.value == "if")
	{
		cout << "开始分析 if " << endl;
		ann++;
		lexp();
		test = group[ann];
		//cout << cx << "&&&&&&&&&" << endl;
		cx1 = cx;   //此时cx对应JPC那条
		gen(JPC, 0, 0);
		if (test.value != "then")
		{
			cout << "Row" << test.Row << ",Column" << test.Column << "if判断的then缺失" << endl;
		}
		else
		{
			ann++;
		}
		statement();
		//cout << cx << "&&&&&&&&&" << endl;
		code[cx1].a = cx;
		test = group[ann];
		if (test.value == "else")
		{
			cx2 = cx;
			gen(JMP, 0, 0);//是指条件符合时 无条件跳转
			code[cx1].a = cx;
			ann++;
			test = group[ann];
			statement();
			code[cx2].a = cx;
		}
		cout << " if 分析结束 " << endl;

	}
	else if (test.value == "while")
	{
		cout << "开始分析 while " << endl;
		cx1 = cx;
		ann++;
		test = group[ann];
		lexp();
		cx2 = cx;  gen(JPC, 0, 0);
		test = group[ann];
		if (test.value != "do")
		{
			cout << "Row" << test.Row << ",Column" << test.Column << "while模块的do缺失" << endl;
		}
		else
		{
			ann++;
			statement();
			gen(JMP, 0, cx1);
			code[cx2].a = cx;
		}
		cout << "while分析结束" << endl;
	}
	else if (test.value == "call")
	{
		cout << "开始分析 call" << endl;
		ann++;
		test = group[ann];
		if (test.cate != "ID")
			cout << "Row" << test.Row << ",Column" << test.Column << "call 模块 ID 缺失" << endl;
		else
		{
			i = position(test.value.c_str());
			if (i == 0)
			{
				Error(6);
			}
			else if (table[i].kind != procedure)
			{
				Error(8);
			}

			//调用层减定义层的层差 若大于零 说明调用层在后面，层次要减，若小于零，说明调用层在外面，要加层次
	
			ann++;
			test = group[ann];
		}
		if (test.value != "(")
			cout << "Row" << test.Row << ",Column" << test.Column << "while模块的(缺失" << endl;
		else
		{
			ann++;
			test = group[ann];
		}
		cx1 = table[i].val;  //参数个数
		//有传递参数
		if (test.value != ")")
		{
			if ((test.value != "end") && (test.value != ";"))
			{

				//ann++;
				//test = group[ann];
				exp();
				gen(OPR, 0, 13);
				test = group[ann];
				cx1--;
				while (test.value == ",")
				{
					ann++;
					test = group[ann];
					exp();
					gen(OPR, 0, 13);
					cx1--;
					test = group[ann];
				}
			}
			else
			{
				cout << "[GRAMMAR ERROR] " << " call 模块的 ) 缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
				//cout << "Row" << test.Row << ",Column" << test.Column << "call 模块的 ) 缺失" << endl;
			}
		}
		if (cx1 != 0)
			Error(7);
		//gen(CAL, lev - table[i].level, table[i].adr);
		if (test.value != ")")
			cout << "[GRAMMAR ERROR] " << " call 模块的 ) 缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
		//cout << "Row" << test.Row << ",Column" << test.Column << "call 模块的 )缺失" << endl;
		else
		{
			gen(CAL, lev - table[i].level, table[i].adr);
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
	else if (test.value == "read")
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
		if (test.cate != "ID")
			cout << "Row" << test.Row << ",Column" << test.Column << "read模块 ID 缺失" << endl;
		else
		{
			i = position(test.value.c_str());
			if (i == 0)
			{
				Error(3);
			}
			else if (table[i].kind != variable)
			{
				Error(9);
			}
			gen(RED, lev - table[i].level, table[i].adr);
			ann++;
			test = group[ann];
		}
		while (test.value == ",")
		{
			ann++;
			test = group[ann];
			if (test.cate == "ID")
			{
				i = position(test.value.c_str());
				if (i == 0)
				{
					Error(3);
				}
				else if (table[i].kind != variable)
				{
					Error(9);
				}
				gen(RED, lev - table[i].level, table[i].adr);
				ann++;
				test = group[ann];
			}
			else
			{
				cout << "Row" << test.Row << ",Column" << test.Column << "read模块 ,后变量缺失" << endl;
			}
		}
		if (test.value != ")")
			cout << "[GRAMMAR ERROR] " << " read模块 )缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
		// cout << "Row" << test.Row << ",Column" << test.Column << "read模块 )缺失" << endl;
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
	    //lod指令在exp里输出
		gen(WRT, 0, 0);
		gen(OPR, 0, 14);  //输出换行符

		test = group[ann];
		while (test.value == ",")
		{
			ann++;
			exp();
			test = group[ann];
			gen(WRT, 0, 0);
			gen(OPR, 0, 14);  //输出换行符
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


	//cout << "*****statement 分析结束*****" << endl;


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
	//statament进入时前已多读一位 出来时也多读一位
	while (test.value == ";" )
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
	else
	{
		if ((test.cate == "ID" || ReserveCheck(test.value)) && (test.Row != 0))
		{
			//std::cout << test.value << "hhhhhhhh" << endl;
			std::cout << "[Gramar Error ]" << "Row" << test.Row << ",Column" << test.Column << "： statement缺少分号" << endl;
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
			cout << "[GRAMMAR ERROR] " << "  body模块 end缺失" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
			//std::cout << "Row" << test.Row << ",Column" << test.Column << "： body模块 end缺失" << endl;
		}

	}


}

//proc在进入和出去时都会多分析一个词
void proc()
{
	int keep_tx = tx+1;  //用来保存现有指向procedure的符号表指针
	int proc_pos=0;//保存形参所在位置
	long mother_keep;
	long rn_keep;
	cout << "------------开始分析proc----------" << endl;
	ann++;
	test = group[ann];
	if (test.cate == "ID")
	{
		Enter(procedure);
		rn_keep =rn_table;
		 mother_keep =mother_table;
		//登记到原符号表后开始将嵌套层次加一，偏移量重置
		lev = lev + 1;
		dx = 3;
		mother_table = table_number;
		table_number++;
		rn_table = table_number;
		ann++;
		test = group[ann];
		if (test.value == "(")
		{
			ann++;
			test = group[ann];
			if (test.cate == "ID")
			{

				/*形参直接放到新符号表最前方，加一后dx加一*/
				Enter_fp(formal_parameter);
				ann++;
				test = group[ann];
			}
			while (test.value == ",")
			{
				ann++;
				test = group[ann];
				if (test.cate == "ID")
				{
					Enter_fp(formal_parameter);
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
			table[keep_tx].val = dx - 3;    //用来保存形参个数
			proc_pos = dx-3;
		}

	}


	test = group[ann];
	if (test.value != ";")
	{
		//cout << test.value << "^^^^^^^^^^^" << endl;
		cout << "[GRAMMAR ERROR] " << ": proc分号缺失" << "  ( Row" << test.Row << ",Column" << test.Column << ")" << endl;
	}
	else
	{
		ann++;
	}

	block(tx-proc_pos);
	/*ann++;
	test = group[ann];*/
	//cout << test.value << "what--------" << endl;
	
	rn_table = rn_keep;
	mother_table = mother_keep;
	lev = lev - 1;
	cout << "------------proc分析结束------------" << endl;
	if (test.value == "procedure")
	{
		//说明出现嵌套的procedure
		//cout << "!!!!!!!!!!!进入嵌套定义的procedure!!!!!!!!!!!" << endl;
		proc();



	}
}

void vardecl()
{
	cout << "开始分析vardecl" << endl;
	ann++;
	test = group[ann];
	if (test.cate == "ID")
	{
		Enter(variable);
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
			Enter(variable);
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
	if (test.cate == "ID")
	{
		test = group[ann - 1];
		cout << "Row" << test.Row << ",Column" << test.Column << "var定义缺少逗号" << endl;
		ann++;
		test = group[ann];
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
	//cout << group[ann].value<< "开始分析const" << endl;
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
				Enter(constant);
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
		else
		{
			if (test.cate == "INT")
			{
				//说明是缺失赋值号
				cout << "[GRAMMAR ERROR] " << "  const 缺少赋值号" << "  ( Row " << test.Row << ",Column " << test.Column << ")" << endl;
				ann++;

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
		test = group[ann - 1];
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
//proc_t   进block前procedure所在位置
void block(int proc_t)
{
	long tx0; //用来保存本层开始的符号表位置
	//long cx0;
	long dx1;//用来保存该层在进入process前偏移量
	long cx0; //用来记录该层目标代码个数

	

	tx0 = proc_t;
	table[proc_t].adr = cx;   //先在符号表记下当前层代码的开始地址
	gen(JMP, 0, 0);   //先写下jmp 之后再回填
	if (lev > levmax)
	{
		Error(1);
	}
	

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
	//这里才是嵌套
	if (test.value == "procedure")
	{
		dx1 = dx;
		proc();
		dx = dx1;
		flag = true;
	}
	code[table[tx0].adr].a = cx;   //table[tx0].adr此时为刚进入该层时目标代码的序号 就是jmp那个
	table[tx0].adr = cx;          //将adr改为过程体执行入口  符号表里过程的adr

	cx0 = cx;
	gen(INT, 0, dx);
	if (test.value == "begin")
	{
		body();
	}
	else
	{
		cout << "Row" << test.Row << ",Column" << test.Column << ": body模块 begin 缺失" << endl;
	}
	gen(OPR, 0, 0);
	cout << "--------------block分析结束--------------" << endl;
}

void prog()
{
	//int tx = 0; //table表的下标指针
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
		else if (test.cate == "ID")
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
	table_number = 1;
	dx = 3; //偏移基址改为3
	rn_table = 1;
	block(0);

	//new_Block(0, tx, 3, tx);

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
	cout << " <<<<<<<<<< Lexical Analysis START >>>>>>>>>>         " << endl;
	//输入源txt文件
	input.open("level2.txt", ios::in);
	//input.open("new.txt", ios::in);

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
					ch = input.get();

					if (ch == '=')
					{
						cout << "[LEXICAL ERROR]  多一个=  ” (Row " << Row << ", Column " << Column << ")" << endl;
					}
					else
					{
						Retract();
					}
				}
				else
				{
					cout << "[LEXICAL ERROR]  Missing “=”near the “:” (Row " << Row << ", Column " << Column << ")" << endl;
					output << "[LEXICAL ERROR] Missing “=”near the “：”  (Row " << Row << ", Column " << Column << ")" << endl;
					//cout << ch << "before" << endl;
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
				if ((ch == '#' || ch == '$'))
					cout << "[LEXICAL ERROR]  非法字符 :" << ch << "(Row " << Row << ", Column:" << Column << ")" << endl;
				else
				{
					/*input.seekg(-3, ios::cur);
					ch = getchar();*/
					/*if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')*/
					//cout << "[LEXICAL ERROR]  UNKOWN :" << ch << "(Row " << Row << ", Column " << Column << ")" << endl;


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
	cout << " <<<<<<<<<< Lexical Analysis END >>>>>>>>>>         " << endl;
	cout << endl;
	cout << " <<<<<<<<<< Grammar Analysis START >>>>>>>>>>         " << endl;
	prog();
	cout << " <<<<<<<<<< Grammar Analysis END >>>>>>>>>>         " << endl;
	printcode();
	printtable();

	if (run_flag)
	{
		cout << " <<<<<<<<<< Interpreter START >>>>>>>>>>         " << endl;
		interpreter();
		cout << " <<<<<<<<<< Interpreter END >>>>>>>>>>         " << endl;
	}


	//cout << endl;
	cout << "Save results successfully" << endl;
	return 0;

}
