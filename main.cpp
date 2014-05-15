// Desmond Preston
// Compiler Construction Fall 2011 
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <cctype>

using namespace std;

void scanner();
void resize();
void f();
void t();
void e();
void l();
void a();
void r();
void exp();
void funcall();
void arglist();
void ret();
void stmt();
void stmtlist();
void IF();
void WHILE();
void DO();
void parameter();
void params();
void paramlist();
void rettype();
void fundecl();
void decl();
void gdecl();
void body(); 
void decllist();
bool alphanumCheck(char);
string * litArray;
string * tokenList;
void parser();
int maxsize = 1000;
int current; // current token placement in token array ie) current = 0 is first token in tokenList
int scaler = 10000;
int current_lvl = 0;
int maxTokens;
int i=0;
int declcount = 0; // How ever many variables there are in a function
ofstream myfile;
string errorMessage;
void scannerfunction();
int relcounter = 0;
int logcounter = 0;
int whilecounter = 0;
int ifcounter = 0;
int paramnum = 0;
int argnum = 0;

struct LList {
	LList *FindItem(string);
	string ident;
	int level;
	LList *next;
	int offset;
	string return_type;
	int paramnum;
	bool isglobal;
	bool isfunction;
};

class symbolTable {
public:
	int lvl;
	string ident;
	LList *table[26];

	void output();
	void DeleteLevel(void);
	void AddLocalItem(string ident);
	void AddGlobalItem(string ident, string return_type, int paramnum);
	bool CheckDec(string ident);
	bool CheckDec(string ident, int lvl);
	int getGlobalStatus(string ident);
	int FindItem(string ident);
	int hash(string ident);
	int numParams(string ident);
};

int symbolTable::hash(string ident)
{
	return ident[0] % 26;
}

int symbolTable::numParams(string ident)
{
	LList *ListToSearch;
	ListToSearch = table[hash(ident)];
	if(ListToSearch == NULL)
		return NULL;
	else 
	{
		while(ListToSearch != NULL)
		{
			if((ListToSearch->ident == ident)) 
				return ListToSearch->paramnum;
			else 
				ListToSearch = ListToSearch->next;
		}
	}
	return NULL;
}

int symbolTable::FindItem(string ident)
{
	LList *ListToSearch;
	ListToSearch = table[hash(ident)];
	if(ListToSearch == NULL)
		return NULL;
	else 
	{
		while(ListToSearch != NULL)
		{
			if((ListToSearch->ident == ident)) 
				return ListToSearch->offset;
			else 
				ListToSearch = ListToSearch->next;
		}
	}
	return NULL;
}

int symbolTable::getGlobalStatus(string ident)
{
	LList *ListToSearch;
	ListToSearch = table[hash(ident)];
	if(ListToSearch == NULL)
		return NULL;
	else 
	{
		while(ListToSearch != NULL)
		{
			if((ListToSearch->ident == ident)) 
				return ListToSearch->level;
			else 
				ListToSearch = ListToSearch->next;
		}
	}
	return NULL;
}

bool symbolTable::CheckDec(string ident)
{
	LList *ListToCheck;
	ListToCheck = table[hash(ident)];
	if(ListToCheck == NULL) return false;
	else {
		while(ListToCheck != NULL) {
			if(ListToCheck->ident == ident) return true;
			else ListToCheck = ListToCheck->next;
		}
		return false;
	}
}

bool symbolTable::CheckDec(string ident, int level)
{
	LList *ListToCheck;
	ListToCheck = table[hash(ident)];
	if(ListToCheck == NULL) return false;
	else {
		while(ListToCheck != NULL) {
			if(ListToCheck->ident == ident && ListToCheck->level == level) {
				return true;
			}
			else {
				ListToCheck = ListToCheck->next;
			}
		}
		return false;
	}
}

void symbolTable::AddGlobalItem(string ident, string return_type, int paramnum)
{
	LList *ListToAddTo;
	if(CheckDec(ident)) {
		cout << litArray[current] << ": ";
		errorMessage = "Error: Redeclared variable";
		throw errorMessage;
	}

	ListToAddTo = table[hash(ident)];
	if(ListToAddTo == NULL) {
		table[hash(ident)] = new LList();
		table[hash(ident)]->ident = ident;
		table[hash(ident)]->offset = declcount*4;
		table[hash(ident)]->return_type = return_type;
		table[hash(ident)]->paramnum = paramnum;
		table[hash(ident)]->isglobal = true;
		table[hash(ident)]->level = 0;
		table[hash(ident)]->next = NULL;
	}
	else {
		while(ListToAddTo->next != NULL) {
			ListToAddTo = ListToAddTo->next;
		}
		ListToAddTo->next = new LList;
		ListToAddTo = ListToAddTo->next;
		ListToAddTo->ident = ident;
		ListToAddTo->level = 0;
		ListToAddTo->next = NULL;
	}
}

void symbolTable::AddLocalItem(string ident)
{
	LList *ListToAddTo;
	if(CheckDec(ident, lvl))
	{
		cout << litArray[current] << ": ";
		errorMessage = "Error: Redeclared variable";
		throw errorMessage;
	}
	ListToAddTo = table[hash(ident)];
	if(ListToAddTo == NULL) {
		table[hash(ident)] = new LList;
		table[hash(ident)]->ident = ident;
		table[hash(ident)]->offset = declcount*4;
		table[hash(ident)]->isglobal = false;
		table[hash(ident)]->level = lvl;
		table[hash(ident)]->next = NULL;
	}
	else {
		while(ListToAddTo->next != NULL) {
			ListToAddTo = ListToAddTo->next;
		}
		ListToAddTo->next = new LList;
		ListToAddTo = ListToAddTo->next;
		ListToAddTo->ident = ident;
		ListToAddTo->level = lvl;
		ListToAddTo->next = NULL;
	}
}

void symbolTable::DeleteLevel(void)
{
	LList *here;
	LList *next;
	for(int i=0; i<26; i++) {
		if(table[i] != NULL) {
			here = table[i];
			next = table[i]->next;
			if((next == NULL) && (table[i]->level == current_lvl)) { //if "here" is the last link
				delete here;
			}
			else if(next != NULL)
			{
				do {
					if(here->level == current_lvl) { //if "here" is the correct level to delete
						delete here;
					}
					here = next;
					next = here->next; 
				}
				while(next != NULL); //while there is another "next" link
			}
		}
	}
	cout << "Level " <<current_lvl << " deleted" << endl;
	current_lvl--; //decrease level by one			
}

symbolTable sb;

/*
void symbolTable::output()
{
	LList *next;
	LList *here;
	here = table[i];
	next = table[i]->next;
	while(next != NULL) {
		cout << "Item: " << table[i]->ident;
		next = table[i]->next;
		i++;
	}

}
*/

void scannerfunction()
{
	litArray = new string[maxsize];
	tokenList = new string[maxsize];
}

void resize()
{
	string *tempS;
	string *tempT;
	tempS = new string[maxsize];
	tempT = new string[maxsize];
	maxsize += scaler;

	for(int i=0; i<current; i++)
	{
		tempS[i] = litArray[i];
		tempT[i] = tokenList[i];
	}

	delete[] litArray;
	delete[] tokenList;
	
	litArray = new string[maxsize];
	tokenList = new string[maxsize];

	for(int i=0; i<current; i++)
	{
		litArray[i] = tempS[i];
		tokenList[i] = tempT[i];
	}
}

void main()
{
	scannerfunction();
	scanner();
	try {
		myfile.open ("U:\\Compiler\\Compiler_ALPHA\\Debug\\code.asm");
		if(!myfile.good())
		{
			errorMessage = "Error writing to ASM file.";
			throw errorMessage;
		}
		parser();
		myfile.close();
		cout << "\nParsed successfully!" << endl;
		//sb.output();
	}
	catch(string errorMessage) {
		cout << errorMessage << endl;
	}

	
}

bool alphanumCheck(char c) // return true if c is alphanumeric, otherwise false
{
	if(( (int)c < 48) || (((int)c >= 58) && ((int)c < 65)) || (((int)c >= 91) && ((int)c < 97)) || ((int)c > 122))
	{
		return false;
	}
	else
		return true;
}

void scanner()
{
	string curstate = "sinit"; //initialize state
	char c; //character being read from file
	bool flag=false;
	
	fstream file;
	file.open("U:\\Compiler\\Compiler_ALPHA\\Debug\\code.txt");
	if(!file.good())
	{
		cout << "Error: File could not be read" << endl;
	}
	else
	{
		resize();
		while(!file.eof())
		{
	
			if(!flag)
			{
				c = file.get();
				
			}
			else
			{
				flag=false;
			}
			
		
			// Take care of operators first
			if((curstate == "sinit") && (c == '>')) {curstate = "sgreaterthan"; litArray[current] += c;}
			else if((curstate == "sinit") && (c == '<')) {curstate= "slessthan"; litArray[current] += c;}
			else if((curstate == "sinit") && (c == '=')) {curstate = "sass"; litArray[current] += c;}
			else if((curstate == "sinit") && (c == '!')) {curstate = "snot"; litArray[current] += c;}
			else if((curstate == "sinit") && (c == '+')) {litArray[current] += c; tokenList[current] = "tplus"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '-')) {litArray[current] += c; tokenList[current] = "tminus"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '/')) {litArray[current] += c; tokenList[current] = "tdiv"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '*')) {litArray[current] += c; tokenList[current] = "tmult"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == ';')) {litArray[current] += c; tokenList[current] = "tsemi"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '{')) {litArray[current] += c; tokenList[current] = "tocurl"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '}')) {litArray[current] += c; tokenList[current] = "tccurl"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == '(')) {litArray[current] += c; tokenList[current] = "toparen"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == ')')) {litArray[current] += c; tokenList[current] = "tcparen"; current++; curstate = "sinit";}
			else if((curstate == "sinit") && (c == ',')) {litArray[current] += c; tokenList[current] = "tcomma"; current++; curstate = "sinit";}

			else if((curstate == "sinit") && (c == '&'))
			{
				curstate = "sand";
				litArray[current] += c;
			}
			else if((curstate == "sand") && (c == '&'))
			{
				tokenList[current] = "tand";
				current++;
				curstate = "sinit";
			}
			else if((curstate == "sinit") && (c == '|'))
			{
				curstate = "sor";
				litArray[current] += c;
			}
			else if((curstate == "sor") && (c == '|'))
			{
				tokenList[current] = "tor";
				current++;
				curstate = "sinit";
			}
			else if((curstate == "sinit") && (c == 'i'))
			{
				curstate = "si";
				litArray[current] += c;
			}
			else if((curstate == "sgreaterthan") && (c != '='))
			{
				tokenList[current] = "tgreaterthan";
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "sgreaterthan") && (c == '='))
			{
				tokenList[current] = "tgreatorequal";
				litArray[current] += c;
				current++; 
				curstate = "sinit";
			}
			else if((curstate == "slessthan") && (c != '='))
			{
				tokenList[current] = "tlessthan";
				current++;
				curstate = "sinit";
			}
			else if((curstate == "slessthan") && (c == '='))
			{
				tokenList[current] = "tlessorequal";
				litArray[current] += c;
				current++;
				curstate = "sinit";
			}
			else if((curstate == "sass") && (c != '='))
			{
				tokenList[current] = "tass";
				litArray[current] += c;
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "snot") && (c != '='))
			{
				tokenList[current] = "tnot";
				current++;
				curstate = "sinit";
			}
			else if((curstate == "snot") && (c == '='))
			{
				tokenList[current] = "tnotequiv";
				litArray[current] += c;
				current++;
				curstate = "sinit";
			}
			else if((curstate == "sass") && (c == '='))
			{
				tokenList[current] = "tequiv";
				litArray[current] += c;
				current++;
				curstate = "sinit";
			}
			else if((curstate == "si") && (c == 'n'))
			{
				curstate = "sin";
				litArray[current] += c;
			}
			else if((curstate == "sin") && (c == 't'))
			{
				curstate = "sint";
				litArray[current] += c;
			}
			else if((curstate == "sin") && (c != 't'))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sint") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sint") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "tint";
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "si") && (c == 'f'))
			{
				curstate = "sif";
				litArray[current] += c;
			}
			else if((curstate == "sif") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sif") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "tif";
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "sinit") && (c == 'e'))
			{
				curstate = "se";
				litArray[current] += c;
			}
			else if((curstate == "se") && (c == 'l'))
			{
				curstate = "sel";
				litArray[current] += c;
			}
			else if((curstate == "sel") && (c == 's'))
			{
				curstate = "sels";
				litArray[current] += c;
			}
			else if((curstate == "sels") && (c == 'e'))
			{
				curstate = "selse";
				litArray[current] += c;
			}
			else if((curstate == "selse") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "selse") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "telse";
				current++;
				curstate = "sinit";
			}
		    else if((curstate == "sinit") && (c == 'w'))
			{
				curstate = "sw";
				litArray[current] += c;
			}
			else if((curstate == "sw") && (c == 'h'))
			{
				curstate = "swh";
				litArray[current] += c;
			}
			else if((curstate == "swh") && (c == 'i'))
			{
				curstate = "swhi";
				litArray[current] += c;
			}
			else if((curstate == "swhi") && (c == 'l'))
			{
				curstate = "swhil";
				litArray[current] += c;
			}
			else if((curstate == "swhil") && (c == 'e'))
			{
				curstate = "swhile";
				litArray[current] += c;
			}
			else if((curstate == "swhile") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "swhile") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "twhile";
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "sinit") && (c == 'r'))
			{
				curstate = "sr";
				litArray[current] += c;
			}
			else if((curstate == "sr") && (c == 'e'))
			{
				curstate = "sre";
				litArray[current] += c;
			}
			else if((curstate == "sre") && (c == 't'))
			{
				curstate = "sret";
				litArray[current] += c;
			}
			else if((curstate == "sret") && (c == 'u'))
			{
				curstate = "sretu";
				litArray[current] += c;
			}
			else if((curstate == "sretu") && (c == 'r'))
			{
				curstate = "sretur";
				litArray[current] += c;
			}
			else if((curstate == "sretur") && (c == 'n'))
			{
				curstate = "sreturn";
				litArray[current] += c;
			}
			else if((curstate == "sreturn") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sreturn") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "treturn";
				current++;
				curstate = "sinit";
			}
			else if((curstate == "sinit") && (c == 'v'))
			{
				curstate = "sv";
				litArray[current] += c;
			}
			else if((curstate == "sv") && (c == 'o'))
			{
				curstate = "svo";
				litArray[current] += c;
			}
			else if((curstate == "svo") && (c == 'i'))
			{
				curstate = "svoi";
				litArray[current] += c;
			}
			else if((curstate == "svoi") && (c == 'd'))
			{
				curstate = "svoid";
				litArray[current] += c;
			}
			else if((curstate == "svoid") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "svoid") && (alphanumCheck(c) == false))
			{
				tokenList[current] = "tvoid";
				current++;
				curstate = "sinit";
				flag = true;
			}
			else if((curstate == "sinit") && (isalpha(c)))
			{
				
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sstring") && (alphanumCheck(c) == true))
			{
				curstate = "sstring";
				litArray[current] += c;
			}
			else if((curstate == "sstring") && (alphanumCheck(c) == false))
			{
				curstate = "sinit";
				tokenList[current] = "tstring";
				flag = true;
				current++;
			}
			else if((curstate == "sinit") && (alphanumCheck(c) && !(isalpha(c))))
			{
				curstate = "snum";
				litArray[current] += c;
			}
			else if((curstate == "snum") && (alphanumCheck(c) && !(isalpha(c))))
			{
				curstate = "snum";
				litArray[current] += c;
				
			}
			else if((curstate == "snum") && !(alphanumCheck(c) && !(isalpha(c))))
			{
				curstate = "sinit";
				tokenList[current] = "tnum";
				flag = true;
				current++;

			}
			//cout << "Character read: " << c << " State: " << curstate << endl;
		}
		
		
		
		for(int i=0; i<current; i++) // output current token list
		{
			cout << "Value: " << litArray[i] << " | Token: " << tokenList[i] << endl;
		}
		
		
		cout << endl;
		cout << endl;

	}

	cout << "Scanned successfully!\n" << endl;

	
	
}

void parser()
{
	myfile << "INCLUDE Irvine32.inc" << endl;
	cout << "INCLUDE Irvine32.inc" << endl;
	myfile << ".data" << endl;
	cout << ".data" << endl;

	maxTokens = current;
	current = 0;

	while(((current+1) < maxTokens) && ((tokenList[current+2] == "tsemi")))
	{
		decl();
	}
		myfile << ".code\n"; 
			cout << ".code\n";
			myfile << "input:\n";
			cout << "input:\n"; 
			myfile << "call ReadInt\n"; 
			cout << "call ReadInt\n"; 
			myfile << "ret\n"; 
			cout << "ret\n"; 
			myfile << "output:\n"; 
			cout << "output:\n"; 
			myfile << "call WriteInt\n"; 
			cout << "call WriteInt\n"; 
			myfile << "ret\n"; 
			cout << "ret\n"; 
			sb.AddGlobalItem("output", "", 0);
			sb.AddGlobalItem("input", "", 0);

	while(current<maxTokens)
	{
		fundecl();
	}
}

void f()
{
	if((current+1<maxTokens) && (tokenList[current] == "tstring"))
	{
		if(litArray[current] == "true")
		{
			current++;
			myfile << "push 1\n";
			cout << "push 1\n";
		}
		else if(litArray[current] == "false")
		{
			current++;
			myfile << "push 0\n";
			cout << "push 0\n";
		}
		else if(tokenList[current+1] == "toparen")
		{
			funcall();
		}
		else
		{
			if(!sb.CheckDec(litArray[current]))
			{
				cout << litArray[current] << ": ";
				errorMessage = "Undeclared variable";
				throw errorMessage;
			}
			if(sb.getGlobalStatus(litArray[current]) == 0)
			{
				myfile << "push " << litArray[current] << endl;
				cout << "push " << litArray[current] << endl;
			}
			myfile << "mov eax, [ebp- " << sb.FindItem(litArray[current]) << "]" << endl;
			cout << "mov eax, [ebp- " << sb.FindItem(litArray[current]) << "]" << endl;
			myfile << "push eax\n";
			cout << "push eax\n";
			current++; 
		}
	}
	else if((current+1<maxTokens) && (tokenList[current] == "tnum"))
	{
		myfile << "push " << litArray[current] << "\n";
		cout << "push " << litArray[current] << "\n";
		current++; // consume number
	}
	else if((current+1<maxTokens) && (tokenList[current] == "toparen"))
	{
		current++; // consume open paren
		l();
		if(tokenList[current] != "tcparen")
		{
			errorMessage = "Error: Expected a closed paren";
			throw errorMessage;
		}
		current++; // consume the close paren
	}
	else
	{
		errorMessage = "Error: Unexpected Token";
		throw errorMessage;
	}
}

void t()
{
	f();
	if((current+1<maxTokens) && (tokenList[current] == "tmult") || (tokenList[current] == "tdiv"))
	{
		string op = tokenList[current];
		current++; // consume an asterisk or a division sign
		t();
		if(op == "tmult")
		{
			myfile << "pop ebx\n";
			cout << "pop ebx\n";
			myfile << "pop eax\n";
			cout << "pop eax\n";
			myfile << "imul eax\n";
			cout << "imul eax\n";
			myfile << "push eax";
			cout << "push eax";
		}
		if(op == "tdiv")
		{
			myfile << "\npop ebx\n";
			cout << "\npop ebx\n";
			myfile << "pop eax\n";
			cout << "pop eax\n";
			myfile << "cdq\n";
			cout << "cdq\n";
			myfile << "idiv ebx\n";
			cout << "idiv ebx\n";
			myfile << "push eax\n";
			cout << "push eax\n";
		}
	}
}

void e()
{
	t();
	if((current+1<maxTokens) && (tokenList[current] == "tplus") || (tokenList[current] == "tminus"))
	{
		string op = tokenList[current];
		current++;
		e();

		if(op == "tplus")
		{ 
			myfile << "\npop ebx\n";
			cout << "\npop ebx\n";
			myfile << "pop eax\n";
			cout << "pop eax\n";
			myfile << "add eax, ebx\n";
			cout << "add eax, ebx\n";
			myfile << "push eax\n";
			cout << "push eax\n";
		}
		else if(op == "tminus")
		{
			myfile << "\npop ebx\n";
			cout << "\npop ebx\n";
			myfile << "pop eax\n";
			cout << "pop eax\n";
			myfile << "sub eax, ebx\n";
			cout << "sub eax, ebx\n";
			myfile << "push eax\n";
			cout << "push eax\n";
		}
	}
}

void l()
{
	if((current+1<maxTokens) && (tokenList[current] == "tnot"))
	{
		current++; //consume not
		r();
		myfile << "pop eax\n";
		cout << "pop eax\n";
		myfile << "cmp eax,0\n";
		cout << "cmp eax,0\n";
		myfile << "mov eax,1\n";
		cout << "mov eax,1\n";
		myfile << "push eax\n";
		cout << "push eax\n";
		myfile << "je log" << logcounter << endl;
		cout << "je log" << logcounter << endl;
		myfile << "pop eax\n";
		cout << "pop eax\n";
		myfile << "mov eax,0\n";
		cout << "mov eax0\n";
		myfile << "push eax\n";
		cout << "push eax\n";
		myfile << "log" << logcounter << endl;
		cout << "log" << logcounter << endl;
		logcounter++;
	}
	else
	{
		r();
		if((tokenList[current] == "tand") || (tokenList[current] == "tor"))
		{
			string op = tokenList[current];
			current++;
			r();
			myfile << "pop ebx\n";
			cout << "pop ebx\n";
			myfile << "pop eax\n";
			cout << "pop eax\n";
			if(op == "tand")
			{
				myfile << "\npush 0\n";
				cout << "\npush 0\n";
				myfile << "cmp eax,0\n";
				cout << "cmp eax,0\n";
				myfile << "je log" << logcounter << endl;
				cout << "je log" << logcounter << endl;
				myfile << "cmp ebx, 0\n";
				cout << "cmp ebx, 0\n";
				myfile << "je log" << logcounter << endl;
				cout << "je log" << logcounter << endl;
				myfile << "pop eax\n";
				cout << "pop eax\n";
				myfile << "mov eax,1\n";
				cout << "mov eax,1\n";
				myfile << "push eax\n";
				cout << "push eax\n";
				myfile << "log" << logcounter << ":\n";
				cout << "log" << logcounter << ":\n";
				
			}
			else if(op == "tor")
			{
				myfile << "push 1" << endl;
				cout << "push 1" << endl;
				myfile << "cmp eax, 0" << endl;
				cout << "cmp eax,0" << endl;
				myfile << "jne log" << logcounter << endl;
				cout << "jne log" << logcounter << endl;
				myfile << "cmp ebx, 0" << endl;
				cout << "cmp ebx, 0" << endl;
				myfile << "jne log" << logcounter << endl;
				cout << "jne log" << logcounter << endl;
				myfile << "pop eax" << endl;
				cout << "pop eax" << endl;
				myfile << "push 0" << endl;
				cout << "push 0" << endl;
				myfile << "log" << logcounter << ":" << endl;
				cout << "log" << logcounter << ":" << endl;
			}
		}
	}
}

void r()
{
	e();
	if((current+1<maxTokens) && 
		(tokenList[current] == "tlessthan") || 
		(tokenList[current] == "tgreaterthan") || 
		(tokenList[current] == "tequiv") || 
		(tokenList[current] == "tgreatorequal") || 
		(tokenList[current] == "tlessorequal") ||
		(tokenList[current] == "tnotequiv"))
	{
		string op = tokenList[current];
		current++; // consume relational operator
		e();
		myfile << "\npop ebx\n";
		cout << "\npop ebx\n";
		myfile << "pop eax\n";
		cout << "pop eax\n";
		myfile << "cmp eax,ebx\n";
		cout << "cmp eax,ebx\n";
		myfile << "push 1\n";
		cout << "push 1\n";
		
		if(op == "tequiv")
		{
			myfile << "je rel" << relcounter << "\n";
			cout << "je rel" << relcounter << "\n";
		}
		else if(op == "tlessthan")
		{
			myfile << "jl rel" << relcounter << "\n";
			cout << "jl rel" << relcounter << "\n";
		}
		else if(op == "tgreaterthan")
		{
			myfile << "jg rel" << relcounter << "\n";
			cout << "jg rel" << relcounter << "\n";
		}
		else if(op== "tgreatorequal")
		{
			myfile << "jge rel" << relcounter << "\n";
			cout << "jge rel" << relcounter << "\n";
		}
		else if(op == "tlessorequal")
		{
			myfile << "jle rel" << relcounter << "\n";
			cout << "jle rel" << relcounter << "\n";
		}
		else if(op == "tnotequiv")
		{
			myfile << "jne rel" << relcounter << "\n";
			cout << "jne rel" << relcounter << "\n";
		}
		myfile << "pop eax\n";
		cout << "pop eax\n";
		myfile << "push 0\n";
		cout << "push 0\n";
		myfile << "rel" << relcounter << ":" << endl;
		cout << "rel" << relcounter << ":" << endl;
		relcounter++;
	}
}

void a()
{
	if((current+1<maxTokens) && (tokenList[current+1] == "tass"))
	{
		if(tokenList[current] != "tstring")
		{
			errorMessage = "Error: expected a tstring";
			throw errorMessage;
		}

		if(!sb.CheckDec(litArray[current]))
		{
			cout << litArray[current] << ": ";
			errorMessage = "Error: Undeclared variable";
			throw errorMessage;
		}

		current++; // consume the tstring
		current++; // consume the equals sign
		a();
		myfile << "pop ebx\n";
		cout << "pop ebx\n";
		myfile << "mov[ebp-" << sb.FindItem(litArray[current]) << "],ebx\n";
		cout << "mov[ebp-" << sb.FindItem(litArray[current]) << "],ebx\n";
		myfile << "push ebx\n";
		cout << "push ebx\n";

	}
	else if((current+1<maxTokens) && (tokenList[current+1] != "tass"))
		l();
	else
	{
		errorMessage = "Out of bounds!";
		throw errorMessage;
	}
}

void exp()
{
	if((current+1<maxTokens) && (tokenList[current] == "tsemi"))
	{
		current++; // consume the semicolon
	}
	else if((current+1<maxTokens) && (tokenList[current] != "tsemi"))
	{
		a();
		current++;
		myfile << "pop eax\n";
		cout << "pop eax\n";
	}
	else
	{
		errorMessage = "Error: expected a semicolon!";
		throw errorMessage;
	}
}

void funcall()
{
	argnum = 0;
	string temp = litArray[current];
	if(!sb.CheckDec(litArray[current]))
	{
		cout << litArray[current] << ": ";
		errorMessage = "Error: Undeclared variable";
		throw errorMessage;
	}

	string funname = litArray[current];

	current++; // consume tstring
	current++; // consume open paren

	if((current+1<maxTokens) && (tokenList[current] == "tcparen"))
	{
		current++; // consume tcparen
	}
	else
	{
		arglist();
		if(sb.numParams(temp) != argnum)
		{

			errorMessage = temp + " requires more/less arguments\n";
			throw errorMessage; 
		}
		if(tokenList[current] != "tcparen")
		{
			errorMessage = "Error: expected a tcparen";
			throw errorMessage;
		}
		current++; // consume tcparen
	}	
	myfile << "call " << funname << endl;
	cout << "call " << funname << endl;
	if(tokenList[current] == "tint")
	{
		myfile << "mov ebx, eax\n";
		cout << "mov ebx, eax\n";
	}

}

void arglist()
{

	l();
	argnum++;
	if((current+1<maxTokens) && (tokenList[current] == "tcomma"))
	{
		current++; // consume comma
		arglist();
	}
}

void ret()
{
	current++; // consume treturn
	if((current+1<maxTokens) && (tokenList[current] == "tsemi"))
	{
		current++; // consume tsemi
	}
	else
	{
		l();
		if((current+1<maxTokens) && (tokenList[current] != "tsemi"))
		{
			errorMessage = "Error: expected a semicolon";
			throw errorMessage;
		}
			current++; // consume tsemi
	}
}

void stmt()
{
	if((current+1<maxTokens) && (tokenList[current] == "treturn")) ret();
	else if((current+1<maxTokens) && (tokenList[current] == "tif")) 
	{
		IF();
		relcounter++;
	}
	else if((current+1<maxTokens) && (tokenList[current] == "twhile")) 
	{
		relcounter++;
		WHILE();
		whilecounter++;
	}
	else if((current+1<maxTokens) && (tokenList[current] == "tocurl")) body();
	else exp();
}

void stmtlist()
{
	if((current+1<maxTokens) && (tokenList[current] != "tccurl"))
	{
		stmt();
		stmtlist();
	}
}

void IF()
{
	current++; // consume tif
	if((current<maxTokens) && (tokenList[current] != "toparen")) 
	{
		errorMessage = "Error: expected a toparen";
		throw errorMessage;
	}
	current++;

	l();

	myfile << "pop eax\n";
	cout << "pop eax\n";
	myfile << "cmp eax, 0\n";
	cout << "cmp eax, 0\n";
	myfile << "je else" << ifcounter << endl;
	cout << "je else" << ifcounter << endl;

	if((current+1<maxTokens) && (tokenList[current] != "tcparen"))
	{
		errorMessage = "Error: expected a tcparen";
		throw errorMessage;
	}
	current++; // consume tcparen
	if((current+1<maxTokens) && (tokenList[current]!="tocurl"))
	{
		errorMessage = "Error: expected an open curly bracket!";
		throw errorMessage;
	}
	body();

	myfile << "jmp endif" << ifcounter << endl;
	cout << "jmp endif" << ifcounter << endl;
	myfile << "else" << ifcounter << ":" << endl;
	cout << "else" << ifcounter << ":" << endl;

	if((current+1<maxTokens) && (tokenList[current] == "telse"))
	{
		current++; // consume telse
		if(tokenList[current]!="tocurl")
		{
			errorMessage = "Error: expected an open curly bracket!";
			throw errorMessage;
		}
		body();
	}
	myfile << "endif" << ifcounter << ":" << endl;
	cout << "endif" << ifcounter << ":" << endl;
	ifcounter++;
}

void WHILE()
{
	myfile << "while" << whilecounter << ":" << endl;
	cout << "while" << whilecounter << ":" << endl;
	current++; // consume twhile
	if((current+1<maxTokens) && (tokenList[current] != "toparen")) 
	{
		errorMessage = "Error: expected a toparen";
		throw errorMessage;
	}
	current++;
	l();

	myfile << "pop eax" << endl;
	myfile << "cmp eax, 0" << endl;
	myfile << "je endwhile" << whilecounter << endl;

	if((current+1<maxTokens) && (tokenList[current] != "tcparen")) 
	{
		errorMessage = "Error: expected a tcparen";
		throw errorMessage;
	}
	current++;
	body();
	myfile << "jmp while" << whilecounter << endl;
	myfile << "endwhile" << whilecounter << ":" << endl;
	whilecounter++;
}

void DO()
{
	current++; // consume tdo
	body();
	current++; // consume twhile
	if((current+1<maxTokens) && (tokenList[current] != "toparen")) 
	{
			errorMessage = "Error: expected open paren";
			throw errorMessage;
	}
	current++;
	l();
	if((current+1<maxTokens) && (tokenList[current] != "tcparen")) 
	{
		errorMessage = "Error: expected closed paren";
		throw errorMessage;
	}
	current++;
}

void parameter()
{
	if((current+1<maxTokens) && ((tokenList[current] != "tint") && (tokenList[current] != "tvoid"))) 
	{
		errorMessage = "Error: expected parameter";
		throw errorMessage;
	}
	current++; // consume tint
	if((current+1<maxTokens) && (tokenList[current] != "tstring"))
	{
		errorMessage = "Error: expected a tstring";
		throw errorMessage;
	}
	paramnum++;
	declcount--;
	sb.AddGlobalItem(litArray[current], "int", 0);
	current++; // consume tstring
}

void params()
{
	parameter();
	if((current+1<maxTokens) && (tokenList[current] == "tcomma"))
	{
		current++; // consume tcomma
		params();
	}
}

void paramlist()
{
	declcount--;
	if((current+1<maxTokens) && (tokenList[current] == "tvoid"))
		current++; // consume tvoid
	else
		params();
}

void rettype()
{
	if((current+1<maxTokens) && ((tokenList[current] == "tvoid") || (tokenList[current] == "tint")))
	{
		current++; // consume the return type
	}
	else
	{
		errorMessage = "Error: expected a return type!";
		throw errorMessage;
	}
}

void fundecl()
{
	string return_type = litArray[current];
	rettype();
	string temp = litArray[current];

	if((current+1<maxTokens) && (tokenList[current] != "tstring")) 
	{
		errorMessage = "Error: expected a tstring";
		throw errorMessage;
	}

	if(litArray[current] == "main")
	{
		myfile << "main PROC\n";
		cout << "main PROC\n";
	}
	else
	{
		myfile << litArray[current] << ":\n"; // Generate label for function in ASM
		cout << litArray[current] << ":\n";
		myfile << "push ebp" << endl;
		cout << "push ebp" << endl;
		myfile << "mov ebp, esp" << endl;
		cout << "mov ebp, esp" << endl;
	}

	current++; // consume tstring

	if((current+1<maxTokens) && (tokenList[current] != "toparen")) 
	{
		errorMessage = "Error: expected a toparen";
		throw errorMessage;
	}

	current++; // consume open paren
	paramnum=0;
	paramlist(); 

	sb.AddGlobalItem(temp, return_type, paramnum);

	if((current+1<maxTokens) && (tokenList[current] != "tcparen")) 
	{
		errorMessage = "Error: expected a closed parenthesis!";
		throw errorMessage;
	}
	current++; //consume closed parenthesis
	if((current+1<maxTokens) && (tokenList[current] != "tocurl"))
	{
		errorMessage = "Error: expected an open curly bracket!";
		throw errorMessage;
	}
	body();

	if(temp == "main")
	{
		myfile << "exit\n";
		cout << "exit\n";
		myfile << "main ENDP\n";
		cout << "main ENDP\n";
		myfile << "END main\n";
		cout << "END main\n";
	}
	else
	{
		myfile << "pop ebp\n";
		cout << "pop ebp\n";
		myfile << "ret\n";
		cout << "ret\n";
	}
}

void gdecl()
{
	if(((current+1) < maxTokens) && ((tokenList[current+2] == "tsemi")))
		decl();
    else
	{
		if(current_lvl == 0) 
		{
			myfile << ".code\n"; 
			cout << ".code\n";
			myfile << "input:\n";
			cout << "input:\n"; 
			myfile << "call ReadInt\n"; 
			cout << "call ReadInt\n"; 
			myfile << "ret\n"; 
			cout << "ret\n"; 
			myfile << "output:\n"; 
			cout << "output:\n"; 
			myfile << "call WriteInt\n"; 
			cout << "call WriteInt\n"; 
			myfile << "ret\n"; 
			cout << "ret\n"; 
			sb.AddGlobalItem("output", "", 0);
			sb.AddGlobalItem("input", "", 0);
		}
	
		fundecl();
	}
}

void decl()
{

	 current++; //consume the type

     if((current >= maxTokens) || (tokenList[current] != "tstring"))
     {
        errorMessage = "Error: expected a tstring";
		throw errorMessage;
     }

	 if(sb.CheckDec(litArray[current], current_lvl))
	 {
		 cout << litArray[current] << ": ";
		 errorMessage = "Error: redeclared variable";
		 throw errorMessage;
	 }

	 if(current_lvl == 0)
	 {
		 myfile << litArray[current] << " DWORD 0\n";
		 cout << litArray[current] << " DWORD 0\n";
		 sb.AddGlobalItem(litArray[current], "int", 0);
		 declcount++;

	 }
	 else if(current_lvl > 0)
	 {
		 myfile << "sub esp,4\n";
		 cout << "sub esp,4\n";
		 cout << "Adding " << litArray[current] << endl;
		 sb.AddLocalItem(litArray[current]);
		 declcount++;
	 }

     current++; //consume tstring

     if((current >= maxTokens) || (tokenList[current] != "tsemi"))
     {
        errorMessage = "Error: expected a tsemi";
		throw errorMessage;
     }

     current++; //consume tsemi
}

void body()
{
	current_lvl++;
	current++; // consume the opening curly bracket
	decllist();
	stmtlist();
	current++; // consume the closing curly backet
	for(int i=0; i<declcount; i++)
	{
		myfile << "pop eax\n";
		cout << "pop eax\n";
	}
	sb.DeleteLevel();
	declcount = 0;
}

void decllist()
{
     while((current+1<maxTokens && tokenList[current] == "tint")) 
	 {
		 decl();
		 decllist();
	 }
}

