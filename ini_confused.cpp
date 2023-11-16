
/*
	Tobby Rw-ini confused
	铁锈ini代码混淆
	修改自ini-parse
	upd 23-2-12
	禁止商用
*/

/*
	Why in English, you ask? 
	Because the Dev code used in writing is ANSI code,
	the Chinese part is garbled,
	so now all changed to English.
*/

/*	
	更新内容:
	1.0 beta 22-8-28
	基本支持随机数混淆
	1.1 beta 22-8-28
	修复一些bug
	支持多ini自定义文件(实用化)!!!
	1.2 beta 22-8-28
	紧急修复多ini未重置的bug
	增加忽略设置
	取消.bak和_c机制 直接覆写
	1.3 beta 22-11-?
	修复节判断大bug
	修复设置#6,7读取错误	
	1.4 beta 22-12-02
	新功能:文件拆分(可选,大幅降低可读性)
	1.5 beta 22-12-19
	修复@memory和@global无法判断为主要键
	略微增加效率
	1.6 beta 23-2-12
	在多处影响效率的地方换用STL,较大的增加了效率(单文件(10~50)+(1~10)ms)
	更换中文
*/

#include<bits/stdc++.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>

#include <sstream>
#include <random>
#include <windows.h>
#include <map>
#include <queue>
#include <stack>
#include <conio.h> //_kbhit(),getch()
#include <thread> //多线程 
#include <chrono>
#include <io.h> //文件遍历

using namespace std;

const string version = "v1.6c beta";

ifstream fin;
ofstream fout;

bool keep_comments;
bool keep_empty_line;
bool randomly_add_comments;
bool ignore_error;
bool split_file;
bool disable_rnd_sec;

const int MAXN_INT = 1ll*2<<31-1;
const int max_split_file = 5;

const string debug_level = "warning";
const bool debug_open = false;

uniform_int_distribution<int> big_rand(0,MAXN_INT);
default_random_engine defult_rand;
default_random_engine time_rand(time(0));
/*
	高级随机数调用方式:big_rand(time_rand)%k 
*/ 

void Reset_Color(){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}

inline bool file_alive(const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

void do_parsing(string file_path,string fld,string fln);

void File_Search(queue<string> &qu){
	//抄自 https://www.bilibili.com/read/cv3752961/
	_finddata_t FileInfo;
	intptr_t handle;
//	fstream fout;
//	fout.open("D:\\DevCode\\AllFilesName.txt");//如果是直接输出到控制台可忽略
	while (!qu.empty()){/*进行广搜*/
		string quename = qu.front()+"\\*";//加上通配符，遍历所有子文件
		handle = _findfirst(quename.c_str(), &FileInfo);
		if (handle!= -1){
			if((strcmp(FileInfo.name,".")!=0)&&(strcmp(FileInfo.name,"..")!=0))//判断是否为当前目录或者上级目录	{
			{
			
				//fout << FileInfo.name << ' '<<std::endl;
				//cout << "DEBUG:File Find" << FileInfo.name << ' '<<std::endl;
				string path=qu.front()+"\\"+FileInfo.name;
				string fn=FileInfo.name;
				if(fn.find(".ini")!=string::npos&&fn.find(".bak")==string::npos&&(fn.find("rnd_cp_")==string::npos||fn.size()<6)){//
					do_parsing(path,qu.front(),fn);
				}
				if(FileInfo.attrib==_A_SUBDIR)//判断是否为文件夹
				{
					
					//cout<<"find fld "<<path<<endl;
					qu.push(path);
				}
			}	
		}
		while (_findnext(handle, &FileInfo)==0)//遍历当前文件夹下面的所有文件	
		{
			if((strcmp(FileInfo.name,".")!=0)&&(strcmp(FileInfo.name,"..")!=0)){
				string path=qu.front()+"\\"+FileInfo.name;
				//fout << FileInfo.name << ' '<<std::endl;
				
					//cout << "DEBUG:File Find" << FileInfo.name << ' '<<std::endl;
					//cout<<"find "<<path<<endl;
				string fn=FileInfo.name;
				if(fn.find(".ini")!=string::npos&&fn.find(".bak")==string::npos&&(fn.find("rnd_cp_")==string::npos||fn.size()<6)){
					do_parsing(path,qu.front(),fn);
				}
				if(FileInfo.attrib==_A_SUBDIR){
					
					qu.push(path);
				}
			}
		}
		qu.pop();
	}
	_findclose(handle);
//	fout.close();
}

string filename_extension_list[5001];
int filename_extension_list_cnt=0;

string int_to_str(int n){
    string rt;
    while (n>0) {
        rt=char(n%10+'0') + rt;
        n/=10;
    }
    return rt;
}

void add_filename_extension_list(){//文件扩展名列表(忽略大小写)
	filename_extension_list[++filename_extension_list_cnt]="txt";
	filename_extension_list[++filename_extension_list_cnt]="png";
	filename_extension_list[++filename_extension_list_cnt]="ini";
	filename_extension_list[++filename_extension_list_cnt]="jpg";
	filename_extension_list[++filename_extension_list_cnt]="template";
	filename_extension_list[++filename_extension_list_cnt]="ogg";
	filename_extension_list[++filename_extension_list_cnt]="mp3";
	filename_extension_list[++filename_extension_list_cnt]="md";
	//待补充
}

// string msth[2001];
// int msth_p=0;
set<string> msth;

void add_msth_list(){
	/*
	msth[++msth_p]="name";
	//msth[++msth_p]="maxHp";
	msth[++msth_p]="price";
	msth[++msth_p]="radius";
	msth[++msth_p]="mass";
	msth[++msth_p]="image";
	//msth[++msth_p]="canAttack";
	//msth[++msth_p]="movementType";
	msth[++msth_p]="x";
	msth[++msth_p]="y";
	msth[++msth_p]="@memory";
	*/
	msth.insert("name");
	msth.insert("price");
	msth.insert("radius");
	msth.insert("mass");
	msth.insert("image");
	msth.insert("x");
	msth.insert("y");
	msth.insert("@memory");
	msth.insert("@global");
}

bool all_include(string a,string b){
	//printf("[Debug] A-I %s %s\n",a.c_str(),b.c_str());
	if(b.find(a)!=string::npos||a.find(b)!=string::npos){
		return true;
	}
	return false;//STL快速判断包含
}

int get_number(bool &rs,string s){//取出整数
	rs=false;
	int ans=0;
	int p=1;
	for(int i=0;i<s.size();i++){
		if(s[i]>='0'&&s[i]<='9'){
			ans*=10;
			ans+=s[i]-'0';
		}else{
			if(s[i]!=' '){
				if(s[i]=='-'&&ans==0){
					p=-1;
					continue;//负数
				}
				rs=false;
				return 114514;
			}
		}
	}
	rs=true;
	return p*ans;
}

double get_float(bool &rs,string s){//取出浮点数
	rs=false;
	double ans=0;
	int ws=0;//小数点位数
	bool xf=false;
	int p=1;
	for(int i=0;i<s.size();i++){
		//printf("# s[i]:%c s[i]-\'0\':%d xf:%d ans:%d\n",s[i],s[i]-'0',xf,ans);
		if(s[i]>='0'&&s[i]<='9'){
			if(xf==true) ws++;
			ans*=10;
			ans+=s[i]-'0';
			//printf("ws:%d\n",ws);
		}else{
			if(s[i]=='.'){
				xf=true;
			}else{
				if(s[i]!=' '){
					if(s[i]=='-'&&ans==0){
						p=-1;
						continue;//负数
					}
					rs=false;
					return 114514;
				}	
			}
		}
	}
	rs=true;
	ans/=pow(10,ws);//小数点
	return p*ans;
}

bool get_boolean(bool &rs,string s){
	rs=false;
	for(int i=0;i<s.size();i++){
		if(s[i]>='A'&&s[i]<='Z'){
			s[i]=s[i]-'A'+'a';//统一转小写
		}else{
			if(!(s[i]>='a'&&s[i]<='z')&&s[i]!=' '){
				rs=false;
				return false;
			}
		}
	}
	if(s.find("true")!=string::npos){
		rs=true;
		return true;
	}else if(s.find("false")!=string::npos){
		rs=true;
		return false;
	}else{
		rs=false;
		return false;
	}
}

string rem_sp(string s){//去除首尾空格
	s=s;
	int wz=-1;
	for(int i=0;i<s.size();i++){
		if(s[i]!=' '){
			wz=i;
			break;
		}
	}
	if(wz==-1) return "";//只有空格
	int kz=-1;
	for(int i=s.size();i>0;i--){//22-12-19 fix 
		if(i==s.size()&&(s[i]=='\n'||s[i]=='\r')) continue;//fix /n /r
		if(s[i]!=' '){
			kz=i;
			break;
		}
	}
	if(kz==-1) return "";//只有空格
	//理论上不会出现kz=-1
	return s.substr(wz,kz);
}

string get_file_type(bool &rs,string s){
	rs=false;
	for(int i=0;i<s.size();i++){
		if(s[i]>='A'&&s[i]<='Z'){
			s[i]=s[i]-'A'+'a';//统一转小写
		}
	}
	//printf("[Debug] file type s:%s \n",s.c_str());
	for(int i=1;i<=filename_extension_list_cnt;i++){
		if(all_include(s,filename_extension_list[i])){
			rs=true;
			return filename_extension_list[i];
		}
	}
	return "null";
}

struct Key{
	string name;//键名称
	string value;//键内容
	string type;//键类型
	/*
	normal - 普通
	comment - 注释
	MLA - 多行赋值
	empty - 空行
	*/
	string data_type;
	int data_num;
	double data_float;
	bool data_boolean;
	string data_file;
	/*
	数据类型
	*unknown - 未知
	*float - 浮点数
	*number - 整数
	string - 字符串
	*boolean - 布尔值
	enum - 特定值
	unit - 单位
	marker - 参考
	*file - 文件
	*/
	int rnd_sort;//随机数混淆
	int rnd_file;//随机文件拆分
	bool main_k;
	Key () {}
	Key (string a,string b){
		name=a,value=b;
	}
};

bool operator < (const Key& a,const Key& b)
{
    if(a.rnd_sort<b.rnd_sort) return true;
    else return false;
}

struct Section{
	string name;//节名称
	int key_cnt=0;//键数量
	Key keys[5001];//节内容(键)
	int rnd_sort;//随机数混淆
	int rnd_file;//随机文件拆分(用于判定编号文件有无这个节)
	int copyfrom_f;
	int dont_load_f;
};
Section secs[801];

bool operator < (const Section& a,const Section& b)
{
    if(a.rnd_sort<b.rnd_sort) return true;
    else return false;
}

bool section_cmp(Section a,Section b){
	return a.rnd_sort < b.rnd_sort;
}

bool Key_cmp(Key a,Key b){
	return a.rnd_sort < b.rnd_sort;
}

string section_now_name;
int secp=1;
int line_cnt=0;
int section_cnt=0;
int key_cnt=0;
int empty_key_cnt=0;
int comment_key_cnt=0;
bool in_cp=false;//多行赋值

void code_sort(int sep,int kep){
	//secs[sep].keys[kep].name=rem_sp(secs[sep].keys[kep].name);
	//secs[sep].keys[kep].value=rem_sp(secs[sep].keys[kep].value);
	
	string code_name=secs[sep].keys[kep].name;
	string code_value=secs[sep].keys[kep].value;
	
	if(secs[sep].keys[kep].type!="normal"){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
		if(debug_level=="error") printf("[Error] Code type error when sorting: %s\n",code_name.c_str());
		Reset_Color();
		return;
	}
	bool st=false;
	int num_g=get_number(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="number";
		secs[sep].keys[kep].data_num=num_g;
		return;
	}
	double float_g=get_float(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="float";
		secs[sep].keys[kep].data_float=float_g;
		return;
	}
	bool boolean_g=get_boolean(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="boolean";
		secs[sep].keys[kep].data_boolean=boolean_g;
		return;
	}
	string file_g=get_file_type(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="file";
		secs[sep].keys[kep].data_file=file_g;
		return;
	}
	//到这里还没return说明类型为unknown
	secs[sep].keys[kep].data_type="unknown";
	return;
}

bool is_section(string s,string &r){
	int lcn=-1,rcn=-1;
	r="null";
	for(int i=0;i<s.size();i++){
		if(s[i]=='['){
			lcn=i;
		}else if(s[i]==']'){
			rcn=i;
		}else{
			if(s[i]!=' '&&lcn==-1){
				return false;
			}
			if(s[i]!=' '&&rcn!=-1){
				return false;
			}
		}
	}
//	printf("s:%s l:%d r:%d\n",s.c_str(),lcn,rcn);
	if(lcn!=-1&&rcn!=-1){
		r=s.substr(lcn+1,rcn-1);
		if(r[r.size()-1]==']'){
			r=r.substr(0,r.size()-1);
			// core] 特判
		}
		return true;
	}else{
		return false; 
	} 
}

bool is_empty_str(string s){
	for(int i=0;i<s.size();i++){
		if(s[i]!=' ') return false;
	}
	return true;
} 

int core_p=-1;
bool dtl_f;

void add_section(string sec_n){
	section_cnt++;
	secs[section_cnt].name=sec_n;
	secs[section_cnt].rnd_sort=big_rand(time_rand)+1;
}

//void add_key(string key,string value,string key_type,int rnd_sort,int rnd_file){}

void parse(string s){
//	cout<<"[DEBUG] s: ["<<s<<"]\n";
	line_cnt++;
	string sct;
	int dtl_p=-1;
//	cout<<"rnd:"<<randomly_add_comments<<": ing:"<<ignore_error<<": \n";
	if(is_section(s,sct)){//s[0]=='['&&s[s.size()-1]==']'
		//节
		string sec_name=sct;
		//判重
		bool rpt=false;
		for(int i=1;i<=section_cnt;i++){
			if(secs[i].name==sec_name){
				//节已经存在
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
				if(debug_level=="warn"||debug_level=="info") printf("[Waring] Reapet section: %s\n",sec_name.c_str());
				Reset_Color();
				secp=i;
				rpt=true;
				break;
			}
		}
		if(!rpt){
			if(debug_level=="info") printf("[Log] Found new section: %s\n",sec_name.c_str());
			add_section(sec_name);
			secp=section_cnt;
			if(sec_name.find("core")!=string::npos){
				core_p=section_cnt;
			}
			if(debug_level=="info") printf("[Log] Section Get Random id: %d\n",secs[section_cnt].rnd_sort);
		}
		section_now_name=sec_name;
	}else{
		//代码
		key_cnt++;
		string key_name;
		string key_value;
		int fs_c=-1;
		int cmf=-1;
		bool fs=false;
		bool emp_l=true;
		bool is_comment=false;
		int dcnt=0;
		for(int i=0;i<s.size();i++){
  			//printf("[DEBUG] i:%d fs_c:%d fs:%s emp_l:%s s[i]:%c name:%s value:%s\n",i,fs_c,fs?"true":"false",emp_l?"true":"false",s[i],key_name.c_str(),key_value.c_str());
			if(s[i]=='\"'){
				if(s[i-1]==s[i]){
					dcnt++;
				}else{
					dcnt=0;
				}
			}
			if(s[i]!=' '){
				emp_l=false;//空行
			}
			if(s[i]=='#'&&fs_c==-1){
				is_comment=true;
				cmf=i;
				break;
			}
			if(s[i]==':'&&fs_c==-1){
				fs_c=i;
				continue;
			}
			if(fs_c==-1&&s[i]==' '){
				key_name+=s[i];
			}
			if(fs_c==-1&&s[i]!=' '){
				key_name+=s[i];
			}else{
				if(!fs&&s[i]!=' '){
					fs=true;
				}
				if(fs){//&&s[i]!=':'
					key_value+=s[i];
				}//消除ini冒号前后的空格
			}
		}
		if(dcnt==2&&fs_c==-1&&in_cp){//多行赋值结尾
			if(debug_level=="info") printf("[Log] Multi-line assignment ended\n");
			in_cp=false;
			/*secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("null",s);
			secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
			secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
			secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
			secs[secp].keys[secs[secp].key_cnt].type="MLA";*/
			if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
			goto pdend;
		}
		if(in_cp&&fs_c==-1){//多行赋值中间
			/*secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("null",s);
			secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
			secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
			secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
			secs[secp].keys[secs[secp].key_cnt].type="MLA";*/
			secs[secp].keys[secs[secp].key_cnt].value+=s;
			if(debug_level=="info") printf("[Log] MLA added:%s\n",s.c_str());
			if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
			goto pdend;
		}
		if(!is_comment){
			if(fs_c==-1){
				if(emp_l){
					if(debug_level=="info") printf("[Log] Find empty line: Line %d\n",line_cnt);
					secs[secp].key_cnt++;
					secs[secp].keys[secs[secp].key_cnt]=Key("null","empty_line");
					secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
					secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
					secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
					secs[secp].keys[secs[secp].key_cnt].type="empty";
					if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
					empty_key_cnt++;
				}else{
					if(ignore_error){
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
						if(debug_level=="info"||debug_level=="warn") printf("[Warning](ignored) No colon found in: %s\n",s.c_str());
						Reset_Color();
						if(is_empty_str(s)){
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
							if(debug_level=="info"||debug_level=="warn"||debug_level=="error") printf("[Error] unignored: empty line\n");
							Reset_Color();
						}
						/*---复制的下面---*/
						key_name=rem_sp(key_name);
						key_value=rem_sp(key_value);
						if(debug_level=="info") cout<<"[Log] Found Key - name: "<<key_name<<" value: "<<key_value;//<<"\n"
						secs[secp].key_cnt++;
						secs[secp].keys[secs[secp].key_cnt]=Key(key_name,key_value);//装入结构体方便调用
						bool mh=false;
						if(msth.count(rem_sp(key_name))==1){
							mh=true;
						}
						/*for(int i=1;i<=msth_p;i++){
							if(all_include(key_name,msth[i])){
								if(debug_level=="info") printf("[Debug] msth[%d]:%s key_name:%s \n",i,msth[i].c_str(),key_name.c_str());
								mh=true;
								break;
							}
						}*/
						secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
						if(mh){
							secs[secp].keys[secs[secp].key_cnt].rnd_file=1;
							if(debug_level=="info"&&debug_open) printf("[Debug] New main key found %s .\n",secs[secp].keys[secs[secp].key_cnt].name.c_str());
							secs[secp].keys[secs[secp].key_cnt].main_k=true;
						}else{
							secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
						}
						
						secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
						secs[secp].keys[secs[secp].key_cnt].type="normal";
						if(key_name.find("copyFrom")!=string::npos){
							secs[secp].copyfrom_f=secs[secp].key_cnt;
							secs[secp].keys[secs[secp].key_cnt].rnd_sort=1;
							secs[secp].keys[secs[secp].key_cnt].rnd_file=1;
						}
						if(key_name.find("dont_load")!=string::npos){
							dtl_p=secs[secp].key_cnt;
							secs[secp].dont_load_f=secp;  
							dtl_f=true;
						}
						code_sort(secp,secs[secp].key_cnt);
						if(debug_level=="info") cout<<" type:"<<secs[secp].keys[secs[secp].key_cnt].data_type<<" \n";
						if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
						/*------*/
					}else{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
						if(debug_level=="info"||debug_level=="warn"||debug_level=="error") printf("[Error] No colon found in: %s\n",s.c_str());
						Reset_Color();
					}
				}
			}else{
				bool cfj=false;
				for(int i=0;i<=secs[secp].key_cnt;i++){
					if(secs[secp].keys[i].name==key_name){
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
						if(debug_level=="info"||debug_level=="warn") printf("[Waring] Reapet Key: %s\n",key_name.c_str());
						Reset_Color();
						secs[secp].keys[i].value=key_value;
						cfj=true;
						break;
					}
				}
				if(!cfj){
					if(rem_sp(key_value)=="\"\"\""){//"\'\'\'"
						if(in_cp==false){
							in_cp=true;//多行赋值
							secs[secp].key_cnt++;
							secs[secp].keys[secs[secp].key_cnt]=Key(key_name,"");
							secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
							secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
							secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
							secs[secp].keys[secs[secp].key_cnt].type="normal";
							if(debug_level=="info") printf("[Log] Find Multi-line assignment\n");
							if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
							
						}else{
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
							if(debug_level=="info"||debug_level=="warn"||debug_level=="error") printf("[Error] Already in Multi-line assignment\n");
							Reset_Color();
							
						}
					}else{
						key_name=rem_sp(key_name);
						key_value=rem_sp(key_value);
						if(debug_level=="info") cout<<"[Log] Found Key - name: "<<key_name<<" value: "<<key_value;//<<"\n"
						secs[secp].key_cnt++;
						secs[secp].keys[secs[secp].key_cnt]=Key(key_name,key_value);//装入结构体方便调用
						bool mh=false;
						if(msth.count(rem_sp(key_name))==1){
							mh=true;
						}
						/*
						for(int i=1;i<=msth_p;i++){
							if(all_include(key_name,msth[i])){
								mh=true;
								break;
							}
						}
						*/
						secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
						if(mh){
							secs[secp].keys[secs[secp].key_cnt].rnd_file=1;
							secs[secp].keys[secs[secp].key_cnt].main_k=true;
						}else{
							secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
						}
						secs[secp].keys[secs[secp].key_cnt].type="normal";
						if(key_name.find("copyFrom")!=string::npos){
							secs[secp].copyfrom_f=secs[secp].key_cnt;
							secs[secp].keys[secs[secp].key_cnt].rnd_sort=1;
							secs[secp].keys[secs[secp].key_cnt].rnd_file=1;
						}
						if(key_name.find("dont_load")!=string::npos){
							dtl_p=secs[secp].key_cnt;
							secs[secp].dont_load_f=secp;  
							dtl_f=true;
						}
						code_sort(secp,secs[secp].key_cnt);
						if(debug_level=="info") cout<<" type:"<<secs[secp].keys[secs[secp].key_cnt].data_type<<" \n";
						if(debug_level=="info") printf("[Log] Key Get Random id: %d\n",secs[secp].keys[secs[secp].key_cnt].rnd_sort);
					}
				}
			}
		}else{
			if(debug_level=="info") printf("[Log] Find Comment line: %s\n",s.c_str());
			comment_key_cnt++;
			secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("#",s.substr(cmf+1,s.size()));
			secs[secp].keys[secs[secp].key_cnt].type="comment";
			secs[secp].keys[secs[secp].key_cnt].rnd_sort=big_rand(time_rand)+1;
			secs[secp].keys[secs[secp].key_cnt].rnd_file=big_rand(time_rand)%max_split_file+1;
			secs[secp].rnd_file=max(secs[secp].rnd_file,secs[secp].keys[secs[secp].key_cnt].rnd_file);
		}
		pdend:int pd_end;
	}
	if(core_p==-1){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
		if(debug_level=="info"||debug_level=="warn"||debug_level=="error") printf("[Error] No Core Section Found.Setting split_file is close.\n");
		split_file=false;
		Reset_Color();
	}
	if (secs[core_p].copyfrom_f==-1){
		secs[core_p].key_cnt++;
		secs[core_p].keys[secs[core_p].key_cnt]=Key("copyFrom","null");
		secs[core_p].keys[secs[core_p].key_cnt].type="normal";
		secs[core_p].keys[secs[core_p].key_cnt].rnd_sort=1;
		secs[core_p].keys[secs[core_p].key_cnt].rnd_file=1;
		secs[core_p].copyfrom_f=secs[core_p].key_cnt++;
	}
}

int space_before_colon;
int space_after_colon;
int enter_after_section;

void sort_input(int file_number,string fld,string fln){
	if(file_number==1) {
		fout.open(fld+"\\"+fln);
		if(debug_level=="info") printf("[Log] Open File: %s\\%s\n",fld.c_str(),fln.c_str());
	}else{
		fout.open(fld+"\\"+"rnd_cp_"+int_to_str(file_number)+"_"+fln);
		if(debug_level=="info") printf("[Log] Open File: %s\\rnd_cp_%d_%s\n",fld.c_str(),file_number,fln.c_str());
	}
	 
	
//	cout<<"rnd:"<<randomly_add_comments<<": ing"
	/*printf("[debug]section: ");
	for(int i=1;i<=secp;i++){
		printf("%d ",secs[i].rnd_sort);
	}
	printf("\n");*/


	if(disable_rnd_sec){
		if(debug_level=="info") printf("[Log] Disable rand sort for sections.\n");
	}else{
		sort(secs+1,secs+secp+1);//随机排序节
	}


	/*printf("[debug]sorted_section: ");
	for(int i=1;i<=secp;i++){
		printf("%d ",secs[i].rnd_sort);
	}
	printf("\n");*/

	/*
	for(int i=1;i<=secp;i++){
			for(int j=i+1;j<=secp-1;j++){
				if(secs[i].rnd_sort>secs[j].rnd_sort){
					swap(secs[i],secs[j]);
				}//反正数据量不大 懒得写快排了 写个选择凑数
			}
		}
	*/

	if(debug_level=="info") printf("[Log] Random Sorted Sections\n");
	fout<<"# This file is automatically confused by Tobby RW-INI Confused\n";
	for(int k=1;k<=section_cnt;k++){
		//输出节名称
		string n_sec=secs[k].name;
		fout<<"["<<n_sec<<"]"<<endl;
		//输出键
		
		sort(secs[k].keys+1,secs[k].keys+secs[k].key_cnt+1);//随机排序键
		
		/*
		for(int i=1;i<=secs[k].key_cnt;i++){
			for(int j=i+1;j<=secs[k].key_cnt-1;j++){
				if(secs[k].keys[i].rnd_sort>secs[k].keys[j].rnd_sort){
					swap(secs[k].keys[i],secs[k].keys[j]);
				}//反正数据量不大 懒得写快排了 写个冒泡凑数
			}
		}
		*/
		
		if(debug_level=="info") printf("[Log] Random Sorted Section %s 's Keys\n",n_sec.c_str());
		bool dtn_pr=false;
		for(int j=1;j<=secs[k].key_cnt;j++){
			if(dtl_f&&!dtn_pr&&secs[k].name.find("core")!=string::npos&&file_number==1){
				dtn_pr=true;
				fout<<"dont_load:true\n";
				continue;
			}
			if(secs[k].name.find("core")!=string::npos){
//				if(secs[k].dont_load_f==-1){
					if(split_file&&file_number>1&&!dtn_pr){
//						if(!secs[k].dtl_f){
							fout<<"dont_load:true\n";
							dtn_pr=true;
//						}
					}
//				}
			}
			if(dtn_pr&&secs[k].keys[j].name.find("dont_load")!=string::npos){
				continue;
			}
			if(split_file){
				bool mh=false;
				string sp_tmp=rem_sp(secs[k].keys[j].name);
				if(msth.count(sp_tmp)>=1){
					mh=true;
				}else{
					if(sp_tmp.substr(0,6)=="@memory"||all_include(secs[k].keys[j].name,"@memory")){
						mh=true;
					}
					if(sp_tmp.substr(0,6)=="@global"||all_include(secs[k].keys[j].name,"@global")){
						mh=true;
					}
				}
				/*
				for(int i=1;i<=msth_p;i++){
//					if(all_include(secs[k].keys[j].name,msth[i])) printf("[D-Debug]rem_sp: {%s} -> {%s} f:%s\n",secs[k].keys[j].name.c_str(),msth[i].c_str(),(rem_sp(secs[k].keys[j].name) == msth[i])?"F_TRUE":"F_FALSE");
					if(rem_sp(secs[k].keys[j].name) == msth[i]){
					//all_include(secs[k].keys[j].name,msth[i])
					//secs[k].keys[j].name.find(msth[i])!=string::npos
//						printf("[Debug] msth[%d]:%s key_name:%s \n",i,msth[i].c_str(),secs[k].keys[j].name.c_str());
						mh=true;
						break;
					}
					if(rem_sp(secs[k].keys[j].name).substr(0,6)=="@memory"||all_include(secs[k].keys[j].name,"@memory")){
//						printf("[D-Debug] find @memory -> main key\n");
						mh=true;
						break;
					}
					if(rem_sp(secs[k].keys[j].name).substr(0,6)=="@global"||all_include(secs[k].keys[j].name,"@global")){
//						printf("[D-Debug] find @global -> main key\n");
						mh=true;
						break;
					}
				}
				*/
				if(mh){//secs[k].keys[j].main_k==true
//					printf("[Debug] Main Key %s\n",secs[k].keys[j].name.c_str());
					// 不输出
				}
				
				else{
					if(secs[k].keys[j].rnd_file!=file_number){
						if(!(core_p==k&&secs[k].copyfrom_f!=-1)) continue;
					}
				}
				
			}
			
			if(split_file){
				if(secs[k].keys[j].name.find("copyFrom")!=string::npos&&file_number>1){
					continue;
				}
			}
			if(secs[k].keys[j].type=="comment"){
				if(!keep_comments) continue;
				fout<<secs[k].keys[j].name<<" "<<secs[k].keys[j].value<<endl;//注释行
			}else if(secs[k].keys[j].type=="normal"){
				fout<<secs[k].keys[j].name;
				for(int u=1;u<=space_before_colon;u++) fout<<" ";
				fout<<":";
				for(int u=1;u<=space_after_colon;u++) fout<<" ";
				fout<<secs[k].keys[j].value<<endl;
			}else if(secs[k].keys[j].type=="MLA"){
				fout<<secs[k].keys[j].value<<endl;
			}else if(secs[k].keys[j].type=="empty"){
				if(!keep_empty_line) continue;
				fout<<"\n";
			}else{
				if(ignore_error){
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
					if(debug_level=="info"||debug_level=="warn") printf("[Warning](ignored) Unknow key type:%s (bug,unignored)\n",secs[k].keys[j].type.c_str());
					Reset_Color();
					/*
					fout<<secs[k].keys[j].name;
					fout<<":";
					fout<<secs[k].keys[j].value<<endl;
					*/
				}else{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
					if(debug_level=="info"||debug_level=="warn"||debug_level=="error") printf("[Error] Unknow key type: %s\n",secs[k].keys[j].type.c_str());
					//version question (char*<-string)
					Reset_Color();
				}				
			}
			if(randomly_add_comments==true){
				if(big_rand(time_rand)%2==0){
					//随机注释
					fout<<"#";
					fout<<"Do not plagiarize or copy other people's labor achievements(";
					fout<<big_rand(time_rand)+1;
					fout<<")";
					fout<<"\n";
				}
			}
		}
		for(int u=1;u<=enter_after_section;u++) if(k!=section_cnt) fout<<"\n";//节后换行
	}
	//fout<<"# Tobby RW-INI Parse collates documents that are prohibited for commercial use\n";
	fout.close();
}

void do_parsing(string file_path);

void in_it(){
	Reset_Color();
	//system("chcp 65001");//防乱码
	add_filename_extension_list();//补充扩展名列表
	add_msth_list();
	printf("Tobby rw ini confused %s\n",version.c_str());
	printf("USE UTF-8\n");
	//printf("Why in English, you ask? Because the Dev code used in writing is ANSI code, the Chinese part is garbled, so now all changed to English.\n");
	//printf("Input File:test.ini  Output File:result.ini  Setting File:set.txt\n");
	if(file_alive("set.txt")){
		ifstream ctr;
		ctr.open("set.txt");
		//1.保留空行
		//2.保留注释
		//3.冒号前空格数量
		//4.冒号后空格数量
		//5.节之间换行数量
		//6.乱加注释
		//7.忽略冒号错误(用于屏蔽某些bug 建议开启)
		//8.拆分文件(实验性功能 可能导致兼容性错误)
		//9.不进行节之间的随机交换(用于屏蔽潜在的兼容性错误 建议开启)
		string ct1,ct2;
		ctr>>ct1;
		if(ct1=="1"){
			printf("[Log] 读取设置 : 保留空行 keep_empty_line = true\n");
			keep_empty_line=true;
		}else{
			keep_empty_line=false;
			if(ct1!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 保留空行 keep_empty_line\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 保留空行 keep_empty_line = false\n");
			}
		}
		ctr>>ct2;
		if(ct2=="1"){
			printf("[Log] 读取设置 : 保留注释 keep_comments = true\n");
			keep_comments=true;
		}else{
			keep_comments=false;
			if(ct2!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 保留注释 keep_comments\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 保留注释 keep_comments = false\n");
			}
		}
		string ct3,ct4;
		ctr>>ct3;
		bool gt1=false;
		int gt1_i=get_number(gt1,ct3);
		if(gt1){
			space_before_colon=gt1_i;
			printf("[Log] 读取设置 : 冒号前空格数量 space_before_colon = %d\n",gt1_i);
		}else{
			space_before_colon=0;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] 读取设置错误 : 冒号前空格数量 space_before_colon\n");
			Reset_Color();
		}
		ctr>>ct4;
		bool gt2=false;
		int gt2_i=get_number(gt2,ct4);
		if(gt2){
			space_after_colon=gt2_i;
			printf("[Log] 读取设置 : 冒号后空格数量 space_after_colon = %d\n",gt2_i);
		}else{
			space_after_colon=1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] 读取设置错误 : 冒号后空格数量 space_after_colon\n");
			Reset_Color();
		}
		string ct5;
		ctr>>ct5;
		bool gt3=false;
		int gt3_i=get_number(gt3,ct5);
		if(gt3){
			enter_after_section=gt3_i;
			printf("[Log] 读取设置 : 节后换行数量 enter_after_section = %d\n",gt3_i);
		}else{
			enter_after_section=0;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] 读取设置错误 : 节后换行数量 enter_after_section\n");
			Reset_Color();
		}
		string ct6;
		ctr>>ct6;
		//printf("[DEBUG] ct6:%s\n",ct6.c_str());
//		cout<<"ct6:"<<ct6<<":\n"; 
		if(ct6=="1"){
			printf("[Log] 读取设置 : 增加随机注释 randomly_add_comments = true\n");
			randomly_add_comments=true;
		}else{
			randomly_add_comments=false;
			if(ct6!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 增加随机注释 randomly_add_comments\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 增加随机注释 randomly_add_comments = false\n");
			}
		}
		string ct7;
		ctr>>ct7;
//		cout<<"ct7:"<<ct7<<":\n"; 
		if(ct7=="1"){
			printf("[Log] 读取设置 : 忽略一些报错 ignore_error = true\n");
			ignore_error=true;
		}else{
			ignore_error=false;
			if(ct7!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 忽略一些报错 ignore_error\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 忽略一些报错 ignore_error = false\n");
			}
		}
		string ct8;
		ctr>>ct8;
//		cout<<"ct7:"<<ct7<<":\n"; 
		if(ct8=="1"){
			printf("[Log] 读取设置 : 分割文件 split_file = true\n");
			split_file=true;
		}else{
			split_file=false;
			if(ct8!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 分割文件 split_file\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 分割文件 split_file = false\n");
			}
		}
		string ct9;
		ctr>>ct9;
		if(ct9=="0"){
			printf("[Log] 读取设置 : 不交换节 disable_rnd_sec = false\n");	
			disable_rnd_sec=false;
		}else{
			disable_rnd_sec=true;
			if(ct9!="1"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] 读取设置错误 : 不交换节 disable_rnd_sec\n");
				Reset_Color();
			}else{
				printf("[Log] 读取设置 : 不交换节 disable_rnd_sec = true\n");
			}
		}
		ctr.close();
	}else{
		keep_comments=false;
		keep_empty_line=false;
		space_before_colon=0;
		space_after_colon=1;
		disable_rnd_sec=true;
		ofstream ctw;
		ctw.open("set.txt");
		ctw<<"0\n1\n0\n1\n1\n0\n1\n0\n1\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
		printf("[Waring] 缺少设置文件: set.txt , 使用默认设置.\n");
		Reset_Color();
		ctw.close();
	}
}

void do_parsing(string file_path,string fld,string fln){
	clock_t pr_start=clock();
	secp=1;
	line_cnt=0;
	section_cnt=0;
	key_cnt=0;
	empty_key_cnt=0;
	comment_key_cnt=0;
	core_p=0; 
	dtl_f=false;
	//memset(&secs,0,sizeof(struct Section));
	for(int i=0;i<=100;i++){
		secs[i].key_cnt=0;
		secs[i].name="";
		secs[i].rnd_sort=0;
		secs[i].rnd_file=-1;
		secs[i].copyfrom_f=-1;
		secs[i].dont_load_f=-1;
		//memset(&secs[i].keys,0,sizeof(struct Key));
		//memset有bug
		for(int j=0;j<=300;j++){
			secs[i].keys[j].name="";
			secs[i].keys[j].type="";
			secs[i].keys[j].data_type="";
			secs[i].keys[j].rnd_sort=0;
			secs[i].keys[j].value="";
			secs[i].keys[j].rnd_file=0;
			secs[i].keys[j].main_k=false;
		}
	}
	//1.1没重置这里
	//理论上不重置也是可以的但保险重置一下
//	printf("[Log] Open File: %s %s\n",fld.c_str(),fln.c_str());
	
//	string cmd_r_t="copy "+file_path+" "+file_path+".bak";
//	char cmd_r[20001];
//	for(int i=0;i<cmd_r_t.size();i++){
//		cmd_r[i]=cmd_r_t[i];
//	}
//	system(cmd_r);
//	char cmd_r1[20001];
//	cmd_r_t="cd "+fld;
//	for(int i=0;i<cmd_r_t.size();i++){
//		cmd_r1[i]=cmd_r_t[i];
//	}
//	system(cmd_r1);
	//fin.open(fln);
	fin.open(file_path);
	clock_t pr_end=clock();
	clock_t start=clock();
	//fout.open(fld+fln);
	//fout.open(fld+"c_"+fln);
	//printf("fin:%s%s fout:%sc_%s \n",fld.c_str(),fln.c_str(),fld.c_str(),fln.c_str());
	//printf("fin:%s fout:%s\\%s \n",file_path.c_str(),fld.c_str(),fln.c_str());
	char now[800001];
	while(fin.getline(now,20001)){
//		printf("[DEBUG] Get %s\n",now);
		parse(now);
	}
	fin.close();
	clock_t end=clock();
	bool gt_f;
	double endtime=(double)(end-start)/CLOCKS_PER_SEC;
	double pr_endtime=(double)(pr_end-pr_start)/CLOCKS_PER_SEC;
	
	clock_t o_st=clock();
//  	fout.open(fld+"\\"+fln);
	if(!split_file){
		sort_input(1,fld,fln);
	}else{
		if(debug_open) printf("[Debug] core_p = %d ,copyfrom_f = %d,copyFrom = %s\n",core_p,secs[core_p].copyfrom_f,secs[core_p].keys[secs[core_p].copyfrom_f].value.c_str());
		for(int i=2;i<=max_split_file;i++){
			if(secs[core_p].keys[secs[core_p].copyfrom_f].value=="null"){
				secs[core_p].keys[secs[core_p].copyfrom_f].value="rnd_cp_"+int_to_str(i)+"_"+fln;
			}else{
				secs[core_p].keys[secs[core_p].copyfrom_f].value+=",rnd_cp_"+int_to_str(i)+"_"+fln;
			}
			secs[core_p].keys[secs[core_p].copyfrom_f].rnd_file=1;
		}
		if(debug_open) printf("[Debug] copyFrom -> %s \n",secs[core_p].keys[secs[core_p].copyfrom_f].value.c_str());
		for(int i=1;i<=max_split_file;i++){
			sort_input(i,fld,fln);
		}
	}
	char cmd_r2[20001];
//	cmd_r_t="move "+fld+"c_"+fln+" "+file_path;
//	for(int i=0;i<cmd_r_t.size();i++){
//		cmd_r2[i]=cmd_r_t[i];
//	}
//	system(cmd_r2);
	clock_t o_en=clock();
	double out_endtime=(double)(o_en-o_st)/CLOCKS_PER_SEC;
	printf("文件 %s 混淆完成\n花费 %.2lfms(预处理 %.2lf ms + 混淆 %.2lf ms + 输出 %.2lf ms).\n",file_path.c_str(),(endtime*1000+pr_endtime*1000+out_endtime*1000),pr_endtime*1000,endtime*1000,out_endtime*1000);
	printf("统计:\n节:%d\n键:%d (空键:%d 注释键:%d)\n",section_cnt,key_cnt,empty_key_cnt,comment_key_cnt);
}

int main() {
	
	srand(time(0));
	system("@echo on");
	in_it();
	
//	string stmp;
//	if(is_section(" [action_ai]   ",stmp)){
//		cout<<"stmp-"<<stmp<<"-"<<endl;
//	}
	
	printf("请输入mod根路径(eg. D:\\rwtest\\test\\ini ,错误路径可能导致其他文件损坏,不能有空格):");
	string pth;
//	pth="D:\\rwtest\\test\\ini";//D:\\桌面\\rwini-confused\\test
	getline(cin,pth);
	clock_t m_st=clock();
	queue <string> pth_t;
	
//	for(int i=0;i<pth.size();i++){
//		pth_t.push(pth[i]);
//	}

	pth_t.push(pth);
	
	//puts("搜索前"); 
	File_Search(pth_t);
	//puts("搜索后"); 
	
//	do_parsing("D:\\rwtest\\test\\ini\\test.ini","D:\\rwtest\\test\\ini\\","test.ini");
	clock_t m_en=clock();
	system("PAUSE");
	double main_endtime=(double)(m_en-m_st)/CLOCKS_PER_SEC;
	//printf("[Debug] 114514:%d 114.154:%.3lf \n",get_number(gt_f,"114514"),get_float(gt_f,"114.514"));
	printf("总用时 %.2lf ms\n",main_endtime*1000);
	printf("混淆完成\nBy Tobby.\n");
	
	system("PAUSE");
	return 0;
}
