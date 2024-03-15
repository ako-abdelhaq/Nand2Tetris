#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <bitset>
#include <unordered_map>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stack>

using namespace std;

int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    if( S_ISDIR(path_stat.st_mode) ){
        return 1;
    }
    else{
        return 0;
    }
}

bool is_Jack_file(string file){
    int n = file.size() - 1;

    if( n < 5 ){
        return false;
    } 
    else if( file[n] == 'k' && file[n-1] == 'c' && file[n-2] == 'a' && file[n-3] == 'j' && file[n-4] == '.' ){
        return true;
    }
    else if( file == "Main.jack" ){
        return true;
    }
    else{
        return false;
    }
}

void get_Jack_files( const char *path , vector<string>& files){
    struct dirent *d;
    DIR *dr = NULL;
    dr = opendir(path);
    string dir_name = path;
    string file;
    if(dr != NULL){
        for( d = readdir(dr) ; d != NULL ; d = readdir(dr) ){
            
            if( !is_directory( d->d_name ) ){
                file = dir_name ;
                if(file.back() != '/'){
                    file += "/";
                }
                file += d->d_name;
                
                if( is_Jack_file(file) ){
                    cout << "jack detected : \"" << file << "\"" << endl;
                    files.push_back(file);
                }
            }
        }
        closedir(dr);
    }
}

typedef pair<string,string> token; 

string HT = "  "; // Horizontal tab

bool is_symbol( char c ){
    if( c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
        c == '.' || c == ',' || c == ';' || c == '+' || c == '-' || c == '*' ||
        c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' ||
        c == '~' ) {
        return true;
    }
    return false;
}

vector<string> handle_white_spaces(vector<string> Jack_file){
    vector<string> handled;
    bool block_comment = false;
    for(int i = 0 ; i < (int)Jack_file.size() ; i++){
        string line = "";
        bool block_end = false;

        for(int j = 0 ; j < (int)Jack_file[i].size() ; j++){
            
            if( ( j + 1 < (int)Jack_file[i].size() ) && Jack_file[i][j] == '*' && Jack_file[i][j+1] == '/' && block_comment){
                block_end = true;
            }

            if( block_end ){
                block_comment = false;
                continue;
            }

            // Regular comment
            if( ( j + 1 < (int)Jack_file[i].size() ) && Jack_file[i][j] == '/' && Jack_file[i][j+1] == '/' && !block_comment){
                break;
            }

            // Block comments
            else if( ( j + 1 < (int)Jack_file[i].size() ) && Jack_file[i][j] == '/' && Jack_file[i][j+1] == '*' ){
                block_comment = true;
            }

            else if( ( j + 2 < (int)Jack_file[i].size() ) && Jack_file[i][j] == '/' && Jack_file[i][j+1] == '*' && Jack_file[i][j+2] == '*' ){
                block_comment = true;
            }

            // Tab
            if( int(Jack_file[i][j]) == 13 && !block_comment ){
                break;
            }

            else{
                if( !block_comment ){
                    string to_append = string( 1 , Jack_file[i][j] ) ;

                    if( to_append == "*" ){
                        line += "*";
                    }
                    else if( is_symbol( line.back() ) && to_append == " " ){
                        // dont do 
                    }
                    else{
                        line += to_append ;
                    }
                }
            }
        }
        string out = "";
        if( !line.empty() ){
            int k = 0;
            
            while( ( line[k] == ' ' || int( line[k] ) == 13 || int( line[k] ) == 9 || int( line[k] ) == 32 ) && k < (int)line.size() ){
                k++;
            }
            
            out = line.substr( k , line.size() );
            out.shrink_to_fit();

            
            while(  out.back() == ' ' || int( out.back() ) == 13 || int( out.back() ) == 9 || int( out.back() ) == 32 ){
                out.pop_back();
            }
            

            
            if( !out.empty() ){
                handled.push_back(out);
            }
            
        }
    }
    
    return handled;
}

enum Tokens { KEYWORD, SYMBOL, IDENTIFIER, CONST, CONST_STRING, ERROR };

struct block{
    int start;
    int end;
    block(){
        start = -1;
        end = -1;
    }
    block(int s , int e){
        end = e;
        start = s;
    }
};

struct symbol {
    string name;
    string kind;
    string type;
    string vm;
    int num;
    symbol(){
        name = "";
        kind = "";
        type = "";
        vm = "";
        num = -1;
    }
    symbol(string n , string k , string t , int nu){
        name = n;
        kind = k;
        type = t;
        num = nu;
        if( k == "field" ){
            vm = "this";
        }
        else {
            vm = k;
        }
    }
};


bool is_CONST( string num ){
    int n = stoi(num);
    if( n >= 0 && n <= 32767 ){
        return true;
    }
    return false;
}

bool is_STRING( string s ){
    if(s[0] == '"' && s[s.size()-1] == '"'){
        return true;
    }
    return false;
}

bool is_IDENTIFIER( string name ){
    if( name[0] != '0' && name[0] != '1' && name[0] != '2' && name[0] != '3' && name[0] != '4' && name[0] != '5'
	&&  name[0] != '6' && name[0] != '7' && name[0] != '8' && name[0] != '9' ){
        return true;
    }
    return false;
}

bool is_KEYWORD( string t ){
    if(t == "class" || t == "constructor" || t == "function" || t == "method" ||
        t == "field" || t == "static" || t == "var" || t == "int" || t == "char" ||
        t == "boolean" || t == "void" || t == "true" || t == "false" || t == "null" ||
        t == "this" || t == "let" || t == "do" || t == "if" || t == "else" || t == "while" || t == "return" ){
        return true;
    }
    return false;
}

bool is_keyword( string t ){
    if( t == "true" || t == "false" || t == "null" || t == "this"){
        return true;
    }
    return false;
}

Tokens get_t_type ( string t ){

    if( is_KEYWORD(t) ){
        return KEYWORD;
    }
    
    else if( is_symbol(t[0]) ) {
        return SYMBOL;
    }
    
    else if( is_STRING(t) ){
        return CONST_STRING;
    }

    else if( is_IDENTIFIER(t) ){
        return IDENTIFIER;
    }

    else if( is_CONST(t) ){
        return CONST;
    }

    else{
        return ERROR;
    }
}


vector<token> get_tokens(vector<string> input) {
    vector<token> t;
    token p;
    Tokens t_type;
    for( int i = 0 ; i < input.size() ; i++ ){
        int j = 0 ;
        while( j < input[i].size() ){
            string command = "";
            if( input[i][j] == ' ' || int(input[i][j]) == 9 || int(input[i][j]) == 13 || int(input[i][j]) == 32 ){
                j++;
                continue;
            }
            else if( is_symbol(input[i][j]) ){
                if( input[i][j] == '<'){
                    command = "&lt;";
                }
                else if( input[i][j] == '>'){
                    command = "&gt;";
                }
                else if( input[i][j] == '&'){
                    command = "&amp;";
                }
                else if( input[i][j] == '*' ){
                    command = "*";
                }
                
                else{
                    command = input[i][j];
                }
                j++;
            }
            else if( input[i][j] == '"' ){
                command = input[i][j] ;
                j++;
                while( j < input[i].size() && input[i][j] != '"'){
                    command += input[i][j];
                    j++;
                }
                command += "\"";
                j++;
            }
            else{
                while( j < input[i].size() && input[i][j] != ' ' && !is_symbol(input[i][j]) ){
                    command += input[i][j];
                    j++;
                }
            }
            p.second = command;
            t_type = get_t_type(command);
            switch ( t_type ){
                case KEYWORD:
                    p.first = "keyword";
                    break;
                case SYMBOL:
                    p.first = "symbol";
                    break;
                case IDENTIFIER:
                    p.first = "identifier";
                    break;
                case CONST:
                    p.first = "integerConstant";
                    break;
                case CONST_STRING:
                    p.first = "stringConstant";
                    p.second.erase(0,1);
                    p.second.pop_back();
                    break;
                default:
                    break;
            }
            t.push_back(p);
        }

    }
    return t;
}

struct compilationEngine{
    vector<token> t;
    string class_name;
    unordered_map<int,int> curly_brackets;
    unordered_map<string,symbol> class_symbols , subRoutine_symbols;
    int static_counter;
    int field_counter;
    int local_counter;
    int arg_counter;
    int label;

    vector<string> handle_statement( int start , int end , bool is_void){
        vector<string> handeled;
        string line = "";
        
        int i = start , temp_s , temp_e ;
        temp_s = i;
        temp_e = temp_s;
        while( i < end ){
            //cout << "in statement : " << t[i].second << endl; 

            if(t[i].second == "if"){
                vector<string> temp;
                temp_s = i;
                
                while( t[i].second != "{" ){
                    i++;
                }
                temp_e = curly_brackets[i] + 1;
                if( t[temp_e].second == "else" ){
                    temp_e++;
                    temp_e = curly_brackets[temp_e] + 1;
                }
                
                temp = handle_if( temp_s , temp_e );
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e;
            }

            else if(t[i].second == "while"){
                vector<string> temp;
                temp_s = i;
                while( t[i].second != "{" ){
                    i++;
                }
                temp_e = curly_brackets[i] + 1;
                temp = handle_while( temp_s , temp_e );
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e ;
            }
            
            else if( t[i].second == "let" ){
                vector<string> temp;
                i++;
                temp_s = i;
                temp_e = temp_s;
                
                while( t[i].second != ";" ){
                    i++;
                    temp_e++;
                }
                
                temp = handle_let( temp_s , temp_e );
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                temp_e++;
                i = temp_e ;
            }
            
            else if(t[i].second == "do"){
                vector<string> temp;
                temp_s = i;
                temp_e = temp_s;
                i++;
                while( t[i].second != ";" ){
                    i++;
                    temp_e++;
                }
                temp_e++;
                temp = handle_do( temp_s , temp_e);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e + 1;
            }

            else if(t[i].second == "return"){
                
                vector<string> temp;
                temp_s = i;
                temp_e = temp_s;
                i++;
                while( t[i-1].second != ";" ){
                    i++;
                    temp_e++;
                }
                temp_e++;
                temp = handle_return( temp_s , temp_e , is_void );
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e + 1;
            }

            else{
                break;
            }

        }
        
        return handeled;
    }

    pair< pair<int,int> , pair<int,int> > decompose( int start , int end ){
        pair< pair<int,int> , pair<int,int> > dual;
        int i = start;
        if( t[i].second != "if" && t[i].second != "while" ){
            return { {-1 , -1} , {-1 , -1} };
        }
        i++;
        stack<int> st;
        st.push(i);
        i++;
        dual.first.first = i;
        while(i < end){                
            if( t[i].second == "(" ){
                st.push(i);
                i++;
            }
            else if( t[i].second == ")" ){
                st.pop();
                
                if( st.empty() ){
                    dual.first.second = i ;
                    i++;
                    break;
                }
                i++;
            }
            else{
                i++;
            }
        }

        stack<int> st_;
        st_.push(i);
        i++;
        dual.second.first = i;

        while(i < end){                
            if( t[i].second == "{" ){
                st_.push(i);
                i++;
            }
            else if( t[i].second == "}" ){
                st_.pop();
                
                if( st_.empty() ){
                    dual.second.second = i ;
                    i++;
                    break;
                }
                i++;
            }
            else{
                i++;
            }
        }

        return dual;
    }

    vector<string> handle_if( int start , int end ){
        auto p = decompose( start , end );
        int start_e = p.first.first , end_e = p.first.second ;
        int start_s = p.second.first , end_s = p.second.second;
        vector<string> handeled , exp , stm;
        string line = "" ;
        int curr_label = label;

        handle_expression( start_e , end_e , exp );

        exp.push_back("not");

        handeled.insert(handeled.end() , exp.begin() , exp.end() );
        line = "if-goto L";
        line += to_string(label);
        label += 2;
        handeled.push_back(line);
        
        stm = handle_statement( start_s , end_s , 0);
        handeled.insert(handeled.end() , stm.begin() , stm.end());
        
        line = "goto L";
        line += to_string(curr_label + 1);
        handeled.push_back(line);

        line = "label L";
        line += to_string(curr_label);
        handeled.push_back(line);

        int i = end_s + 1 ;
        if( t[i].second == "else" ){
            
            i++;
            i++;
            
            stm.clear();
            stm = handle_statement( i , end , 0);
            handeled.insert(handeled.end() , stm.begin() , stm.end());
            
        }

        line = "label L";
        line += to_string(curr_label + 1);
        handeled.push_back(line);
        
        return handeled;
    }


    vector<string> handle_while( int start , int end ){
        auto p = decompose( start , end );
        int start_e = p.first.first , end_e = p.first.second ;
        int start_s = p.second.first , end_s = p.second.second;
        vector<string> handeled , exp , stm;
        string line = "" ;
        int curr_label = label;

        line = "label L";
        line += to_string(curr_label);
        handeled.push_back(line);
        label += 2;
        
        handle_expression( start_e , end_e , exp );

        exp.push_back("not");

        handeled.insert(handeled.end() , exp.begin() , exp.end() );
        line = "if-goto L";
        line += to_string(curr_label + 1);
        handeled.push_back(line);
        
        stm = handle_statement( start_s , end_s , 0);
        handeled.insert(handeled.end() , stm.begin() , stm.end());
        
        line = "goto L";
        line += to_string(curr_label);
        handeled.push_back(line);

        line = "label L";
        line += to_string(curr_label + 1);
        handeled.push_back(line);
        
        return handeled;
    }

    int check_for_bracket( int start , int end ){
        int n = 0;
        for(int i = start ; i < end ; i++ ){
            if(t[i].second == "="){
                break;
            }

            if(t[i].second == "["){
                n++;
            }
        }
        if( n == 0 ){
            return -1;
        }
        else {
            return n;
        }
    }

    vector<string> handle_let( int start , int end ){
        vector<string> handeled , exp0 , exp1;
        string line = "";
        int temp_s , temp_e;
        int i = start ;

        if( check_for_bracket(start , end) != -1 ){
            int exp1_s = i , exp1_e = i , exp2_s = end , exp2_e = end;

            symbol s;
            if( subRoutine_symbols.find(t[exp1_s].second) != subRoutine_symbols.end() ){
                s = subRoutine_symbols[t[exp1_s].second];
            }
            else if( class_symbols.find(t[exp1_s].second) != class_symbols.end() ){
                s = class_symbols[t[exp1_s].second];
            }
            
            i += 2;

            exp1_s = i;
            exp1_e = exp1_s;

            while( i < end && t[i].second != "=" ){
                i++;
                exp1_e++;
            }
            exp2_s = exp1_e + 1;

            handle_expression(exp1_s , exp1_e - 1 , exp0);
            handeled.insert(handeled.end() , exp0.begin() , exp0.end() );

            string line = "push " + s.vm + " ";
            line += to_string(s.num);
            handeled.push_back(line);

            handeled.push_back("add");
            
            if(exp2_s < exp2_e){

                handle_expression(exp2_s , exp2_e , exp1);
                handeled.insert(handeled.end() , exp1.begin() , exp1.end() );
                
                handeled.push_back("pop temp 0");
                handeled.push_back("pop pointer 1");
                handeled.push_back("push temp 0");

                handeled.push_back("pop that 0");
            }
            return handeled;
        }

        while( t[i].second != "=" && i < end ){
            i++;
        }
        temp_s = i+1;
        temp_e = end;
        handle_expression(temp_s , temp_e , exp1);
        handeled.insert(handeled.begin() , exp1.begin() , exp1.end() );
        
        i = start ;

        line = "pop ";
        if( class_symbols.find(t[i].second) != class_symbols.end() ){
            line += class_symbols[t[i].second].vm + " " ;
            line += to_string(class_symbols[t[i].second].num);
        }
        else if( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() ){
            line += subRoutine_symbols[t[i].second].vm + " " ;
            line += to_string(subRoutine_symbols[t[i].second].num);
        }
        handeled.push_back(line);
        
        return handeled;
    }

    vector<string> handle_do( int start , int end ){
        vector<string> handeled , exp_list;
        string line = "";
        int temp_s , temp_e;
        
        int i = start ;
        temp_s = i;
        temp_e = temp_s;

        string f = "call ";
        string push = "push ";
        bool method = false ;
        while( i < end ){
            if(t[i].second == "("){
                temp_s = i+1;
                break;
            }

            if( ( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() || class_symbols.find(t[i].second) != class_symbols.end() ) && t[i+1].second == "." && t[i+2].second != "new" ) {
                symbol s;
                if( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() ){
                    s = subRoutine_symbols[t[i].second];
                    push = "push " + s.vm + " ";
                    push += to_string(s.num);
                    handeled.push_back(push);
                    method = true;
                    f += s.type;
                    f += t[i+1].second;
                    f += t[i+2].second;
                    i += 3;
                }
                else if( class_symbols.find(t[i].second) != class_symbols.end() ){
                    s = class_symbols[t[i].second];
                    push = "push " + s.vm + " ";
                    push += to_string(s.num);
                    handeled.push_back(push);
                    method = true;
                    f += s.type;
                    f += t[i+1].second;
                    f += t[i+2].second;
                    i += 3;
                }
                
            }
            else if( t[i].first == "identifier" && t[i+1].second == "." ) {
                f += t[i].second;
                f += t[i+1].second;
                f += t[i+2].second;
                i += 3;
            }
            else if(t[i].first == "identifier" && t[i+1].second == "(") {
                handeled.push_back("push pointer 0");
                f += class_name;
                f += ".";
                f += t[i].second;
                i++;
                method = true;
            }
            else {
                i++;
            }
            
        }
        temp_e = end;
        int n_args = handle_expressionList(temp_s , temp_e , exp_list) ;
        handeled.insert(handeled.end() , exp_list.begin() , exp_list.end() );
        if( method ){
            n_args++;
        }
        string f_args = to_string(n_args);
        f += " ";
        f += f_args;
        handeled.push_back(f);
        handeled.push_back("pop temp 0");

        return handeled;
    }

    vector<string> handle_call( int start , int end ){
        vector<string> handeled , exp_list;
        string line = "";
        int temp_s , temp_e;
        
        int i = start ;
        temp_s = i;
        temp_e = temp_s;

        string f = "call ";
        string push = "";
        bool method = false;
        while( i < end ){
            if(t[i].second == "("){
                temp_s = i+1;
                break;
            }

            if( ( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() || class_symbols.find(t[i].second) != class_symbols.end() ) && t[i+1].second == "." && t[i+2].second != "new" ) {
                symbol s;
                if( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() ){
                    s = subRoutine_symbols[t[i].second];
                    push = "push " + s.vm + " ";
                    push += to_string(s.num);
                    handeled.push_back(push);
                    method = true;
                    f += s.type;
                    f += t[i+1].second;
                    f += t[i+2].second;
                    i += 3;
                }
                else if( class_symbols.find(t[i].second) != class_symbols.end() ){
                    s = class_symbols[t[i].second];
                    push = "push " + s.vm + " ";
                    push += to_string(s.num);
                    handeled.push_back(push);
                    method = true;
                    f += s.type;
                    f += t[i+1].second;
                    f += t[i+2].second;
                    i += 3;
                }
                
            }
            else if( t[i].first == "identifier" && t[i+1].second == ".") {
                f += t[i].second;
                f += t[i+1].second;
                f += t[i+2].second;
                i += 3;
            }
            else if(t[i].first == "identifier" && t[i+1].second == "(") {
                handeled.push_back("push pointer 0");
                f += class_name;
                f += ".";
                f += t[i].second;
                i++;
                method = true;
            }
            else {
                i++;
            }
            
        }
        temp_e = end;
        int n_args = handle_expressionList(temp_s , temp_e , exp_list) ;
        handeled.insert(handeled.end() , exp_list.begin() , exp_list.end() );
        if( method ){
            n_args++;
        }
        string f_args = to_string(n_args);
        f += " ";
        f += f_args;
        handeled.push_back(f);

        return handeled;
    }

    int handle_expressionList(int start , int end , vector<string>& exp){
        string line = "";
        int temp_s , temp_e , i = start , n = 0;
        temp_s = i;
        temp_e = temp_s;
        
        
        while( i < end ){
            if( t[i].second == "," ){
                handle_expression(temp_s , temp_e , exp);
                i++;
                temp_s = i;
                temp_e = temp_s ;
                n++;
            }
            else{
                i++;
                temp_e++;
            }
            
        }
        temp_e = end;
        if( temp_e - 1 > temp_s ){
            handle_expression(temp_s , temp_e , exp);
            n++;
        }
        
        return n;
    } 

    vector<string> handle_return( int start , int end , bool is_void){
        
        vector<string> handeled , exp1;
        string line = "";
        int temp_s , temp_e;
        
        int i = start + 1 ;
        temp_s = i;
        temp_e = temp_s;
        while( t[i].second != ";" ){
            i++;
            temp_e++;
        }

        if( temp_s < temp_e ){
            handle_expression(temp_s , temp_e , exp1 );
            handeled.insert(handeled.end() , exp1.begin() , exp1.end() );
        }
        else{
            handeled.push_back("push constant 0");
        }
        handeled.push_back("return");
        
        return handeled;
    }

    bool is_op(string c){
        if(c == "+" || c == "-" || c == "*" || c == "/" || c == "&amp;" || c == "|" || c == "&lt;" || c == "&gt;" || c == "="){
            return true ;
        }
        return false;
    }

    bool is_unary(string c){
        if( c == "-" || c == "~" ){
            return true;
        }
        return false;
    }

    unordered_map<int,int> handle_brackets( int start , int end ){
        unordered_map<int,int> handled;
        stack<int> st;
        int i = start;

        while( i < end ){
            if( t[i].second == "(" || t[i].second == "[" ){
                st.push(i);
            }
            else if( t[i].second == ")" || t[i].second == "]"){
                handled[st.top()] = i;
                st.pop();
            }
            i++;
        }

        return handled;
    }

    bool check_for_subRoutine( int start , int end ){
        int i = start;
        while( i < end ){
            if( i + 1 < end && t[i].first == "identifier" && t[i+1].second == "(" ){
                return true;
            }
        }
        return false;
    }

    pair<int ,int> get_term(int start , int end){
        
        int i = start;

        if( t[i].first == "stringConstant" && is_op(t[i+1].second) && i+1 < end){
            return { start , start + 1 };
        }
        else if( ( t[i].first == "integerConstant" || t[i].second == "false" || t[i].second == "ture" || t[i].second == "null" ) && is_op(t[i+1].second) && i+1 < end){
            return { start , start + 1 };
        }
        
        else if( t[i].first == "identifier" && t[i+1].second == "[" ){
            stack<int> st;
            int temp_s = i , temp_e = i + 2;
            
            i++;
            st.push(i);
            i++;

            while(i < end){                

                if( t[i].second == "[" ){
                    st.push(i);
                    i++;
                }
                else if( t[i].second == "]" ){
                    st.pop();
                    i++;
                    if( st.empty() ){
                        temp_e = i ;
                        if( !is_op(t[i].second) || end <= i ){
                            return { -1 , -1 };
                        }
                        else{
                            /*
                            cout << " after handeling brack " << t[i+1].second << endl;
                            for(int j = temp_s ; j < temp_e ; j++){
                                cout << t[j].second << endl;
                            }
                            cout << " kkkk " << t[temp_e].second << endl;
                            */
                        }
                        break;
                    }
                }
                else{
                    i++;
                }
            }
            
            return { temp_s , temp_e };

        }
        else if( t[i].first == "identifier" && t[i+1].second == "(" ){     
            stack<int> st;
            int temp_s = i + 1 , temp_e = i + 2;
            
            i++;
            st.push(i);
            i++;

            while(i < end){                

                if( t[i].second == "(" ){
                    st.push(i);
                    i++;
                }
                else if( t[i].second == ")" ){
                    st.pop();
                    i++;
                    if( st.empty() ){
                        temp_e = i ;
                        /*
                        cout << " st  is empty " << endl;
                        cout << t[i].second << endl;
                        cout << " ----- " << endl;
                        */
                        if( !is_op(t[i].second) ){
                            return { -1 , -1 };
                        }
                        break;
                    }
                }
                else{
                    i++;
                }
            }
            /*
            cout << " after handeling pars" << endl;
            for(int j = temp_s ; j < temp_e ; j++){
                cout << t[j].second << endl;
            }
            */
            return { temp_s , temp_e };

        }
        else if( t[i].first == "identifier" && t[i+1].second == "." ){
            //cout << "entered term in class " << endl;
            //cout << t[i].second << endl;
            i += 3;
            stack<int> st;
            int temp_s = i + 1, temp_e = i + 1;
            /*
            cout << " before handeling " << endl;
            for(int j = i ; j < end ; j++ ){
                cout << t[j].second << endl;
            }
            cout << " in handeling " << endl;
            */
            st.push(i);
            i++;
            while(i < end){                
                //cout << t[i].second << endl;
                if( t[i].second == "(" ){
                    st.push(i);
                    i++;
                }
                else if( t[i].second == ")" ){
                    st.pop();
                    i++;
                    if( st.empty() ){
                        temp_e = i ;
                        /*
                        cout << " st  is empty " << endl;
                        cout << t[i].second << endl;
                        cout << " ----- " << endl;
                        */
                        if( !is_op(t[i].second) ){
                            return { -1 , -1 };
                        }
                        break;
                    }
                }
                else{
                    i++;
                }
            }
            /*
            cout << "after handling class " << endl;
            for(int j = temp_s ; j < temp_e ; j++){
                cout << t[j].second << endl;
            }
            */
            return { temp_s , temp_e };
        }
        else if( t[i].first == "identifier" && is_op(t[i+1].second) && i + 1 < end ){
            return { i , i + 1 };
        }
        
        else if( t[i].second == "(" ){
            //cout << "entered term in par " << endl;
            //cout << t[i].second << endl;
            stack<int> st;
            int temp_s = i + 1 , temp_e = i + 2;
            
            st.push(i);
            i++;
            while(i < end){                

                if( t[i].second == "(" ){
                    st.push(i);
                    i++;
                }
                else if( t[i].second == ")" ){
                    st.pop();
                    i++;
                    if( st.empty() ){
                        temp_e = i ;
                        if( !is_op(t[i].second) ){
                            return { -1 , -1 };
                        }
                        break;
                    }
                }
                else{
                    i++;
                }
            }
            /*
            cout << "after handling par " << endl;
            for(int j = temp_s ; j < temp_e ; j++){
                cout << t[j].second << endl;
            }
            */
            return { temp_s , temp_e };
        }
        
        else {
            //cout << "not defined grammar " << endl;
            /*
            for(int j = start ; j < end ; j++){
                cout << t[j].second << endl;
            }
            */
            return { -1 , -1 };
        }

    }

    int find_op(int start , int end){
        for(int i = start ; i < end ; i++ ){
            if( is_unary(t[i].second) || is_op(t[i].second) ){
                return i;
            }
        }
        return -1;
    }

    string get_op(int op){
        if( t[op].second == "+" ){
            return "add";
        }
        else if( t[op].second == "-" ){
            return "sub";
        }
        else if( t[op].second == "*" ){
            return "call Math.multiply 2";
        }
        else if( t[op].second == "/" ){
            return "call Math.divide 2";
        }
        else if( t[op].second == "=" ){
            return "eq";
        }
        else if( t[op].second == "&lt;" ){
            return "lt";
        }
        else if( t[op].second == "&gt;" ){
            return "gt";
        }
        else if( t[op].second == "&amp;" ){
            return "and";
        }
        else if( t[op].second == "|" ){
            return "or";
        }
        else{
            return "undefined symbol !  ->  " + t[op].second;
        }
    }

    int function_call( int start , int end ){
        int i = start;
        if( t[i].first == "identifier" && t[i+1].second == "." && i+1 < end){
            return i;
        }
        else if( t[i].first == "identifier" && t[i+1].second == "." && i + 2 < end ){
            return i;
        }
        
        return -1;
    }

    vector<pair<int,int>> get_expressions(int start , int end){
        int i = start , s = start;
        vector<pair<int,int>> v;
        while( i < end ){
            if(t[i].second == "," ){
                v.push_back({s , i});
                i++;
                s = i;
            }
            else{
                i++;
            }
        }
        if( end > s ){
            v.push_back( { s , end } );
        }
        return v; 
    }

    bool after_eq(int start , int end){
        for(int i = start ; i < end ; i++ ){
            if(t[i].second == "="){
                return false;
            }
        }
        return true;
    }

    void handle_expression( int start , int end , vector<string>& code ){
        if(start >= end){
            return;
        }

        int i = start;
        //cout << " in expression : " << t[start].second << "   " << t[end-1].second << endl;
        pair<int,int> po = get_term(start , end);
        
        if( po.first != -1 && po.second < end - 1 ){
            
            int op = po.second ;
            handle_expression(po.first , op , code);
            handle_expression(op+1 , end , code);
            code.push_back( get_op(op) );

        }
        else if( is_unary( t[i].second ) ){
            handle_expression( i+1 , end , code );
            if( t[i].second == "~" ){
                code.push_back("not");
            }
            else if( t[i].second == "-" ){
                code.push_back("neg");
            }
        }
        else if( function_call( start , end ) != -1 ){
            //cout << "function called " << t[start].second << " , " << t[end-1].second << endl;
            auto code_ = handle_call(start , end);
            code.insert(code.end() , code_.begin() , code_.end() );
        }
        else if( t[i].first == "identifier" && check_for_bracket(i , end) != -1 ){
            /*
            cout << "with brackets  ->  " << endl;
            for(int j = i ; j < end ; j++ ){
                cout << t[j].second << endl;
            }
            cout << "brackets ended" << endl;
            */
            symbol s;
            if( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() ){
                s = subRoutine_symbols[t[i].second];
            }
            else if( class_symbols.find(t[i].second) != class_symbols.end() ){
                s = class_symbols[t[i].second];
            }

            i += 2;
            handle_expression( i , end , code );

            string line = "push " + s.vm + " ";
            line += to_string(s.num);
            code.push_back(line);

            code.push_back("add");
            code.push_back("pop pointer 1");
            code.push_back("push that 0");
            
        }
        else if( t[i].first == "identifier" ){
            symbol s;
            if( subRoutine_symbols.find(t[i].second) != subRoutine_symbols.end() ){
                s = subRoutine_symbols[t[i].second];
            }
            else if( class_symbols.find(t[i].second) != class_symbols.end() ){
                s = class_symbols[t[i].second];
            }
            string line = "push " + s.vm + " ";
            line += to_string(s.num);
            code.push_back(line);
        }
        else if( t[i].first == "stringConstant" ){
            string s = t[i].second;
            string line = "push constant ";
            line += to_string(s.size());
            code.push_back(line);
            code.push_back("call String.new 1");
            int cur_char = 0;
            for(auto c : s){
                line = "push constant ";
                cur_char = int(c);
                line += to_string(cur_char);
                code.push_back(line);
                code.push_back("call String.appendChar 2");
            }
        }
        else if( t[i].first == "integerConstant" || t[i].second == "false" || t[i].second == "true" || t[i].second == "null" ){
            string line = "push constant ";
            if(t[i].second == "false" || t[i].second == "true" || t[i].second == "null" ){
                line += "0";
            }
            else if(t[i].second == "true"){
                line += "-1";
            }
            else{
                line += t[i].second;
            }
            code.push_back(line);
            if(t[i].second == "true"){
                code.push_back("not");
            }
        }
        else if( t[i].second == "this" ){
            code.push_back("push pointer 0");
        }
        else if( t[i].second == "("){
            stack<int> st;
            int temp_s = i + 1 , temp_e = i + 2;
            //cout << " we are in parrrs !!  " << i << "   " << end << endl;
            st.push(i);
            i++;
            while(i < end){                
                if( t[i].second == "(" ){
                    st.push(i);
                    i++;
                }
                else if( t[i].second == ")" ){
                    st.pop();
                    
                    if( st.empty() ){
                        temp_e = i ;
                        break;
                    }
                    i++;
                }
                else{
                    i++;
                }
            }
            /*
            cout << "with parentheses : " << endl;
            for(int j = temp_s ; j <= temp_e ; j++ ){
                cout << t[j].second << " ";
            }
            cout << endl;
            cout << "end parentheses " << endl;
            */
            handle_expression( temp_s , temp_e , code);
        }
        else{
            cout << "undefined  " << t[i].second << endl;
            return;
        }
        
    }

    pair<block,block> eat( int i , int n){
        pair<block,block> p;
        stack<string> s;
        int start_e , end_e , start_s , end_s;
        if(t[i].second != "while" && t[i].second != "if" && i < n ){
            return p;
        }
        else{
            i++ ;
            if( t[i].second == "(" ){
                s.push(t[i].second);
                i++;
            }

            start_e = i;
            end_e = start_e;

            while( !s.empty() && i < n ){
                if(t[i].second == "("){
                    s.push(t[i].second);
                }
                else if(t[i].second == ")"){
                    s.pop();
                }
                
                if(s.empty()){
                    break;
                }
                else{
                    end_e++;
                }
                i++;
            }
            block e(start_e , end_e);

            if( i + 1 < n && t[i].second == ")" ){
                i++;
            }
            
            if( t[i].second == "{" ){
                s.push(t[i].second);
                i++;
            }
            start_s = i;
            end_s = start_s;
            while( !s.empty() && i < n ){
                if(t[i].second == "{"){
                    s.push(t[i].second);
                }
                else if(t[i].second == "}"){
                    s.pop();
                }
                
                if(s.empty()){
                    break;
                }
                else{
                    end_s++;
                }
                i++;
            }
            block s(start_s , end_s);
            p.first = e;
            p.second = s;
            
            return p;
        }

    }

    void handle_classVar(int start , int end){
        if(t[start].second == "field"){
            symbol s(t[end-1].second , t[start].second , t[start+1].second , field_counter);
            //cout << "classVar : " << t[end-1].second << t[start].second << t[start+1].second << endl;
            class_symbols[t[end-1].second] = s;
            field_counter++;
        }
        else if(t[start].second == "static"){
            symbol s(t[end-1].second , t[start].second , t[start+1].second , static_counter);
            //cout << "classVar : " << t[end-1].second << t[start].second << t[start+1].second << endl;
            s.vm = "static";
            class_symbols[t[end-1].second] = s;
            static_counter++;
        }
    }

    int handle_parameterList(int start , int end ){
        int i = start ;
        string line = "";
        vector<pair<int,int>> args = get_expressions( start , end );
        for(auto p : args ){
            handle_args( p.first , p.second );
        }
        //cout << " in parlist :  " << arg_counter << endl;
        return args.size();
    }

    pair<vector<string>,string> handle_subRoutine(int start , int end ){
        vector<string> handeled , temp;
        int i = start , temp_s , temp_e;
        bool is_void = false;
        string line = "";
        local_counter = 0;
        arg_counter = 0;
        subRoutine_symbols.clear();
        int n_args = 0;
        
        if(t[i].second == "method"){
            handeled.push_back("push argument 0");
            handeled.push_back("pop pointer 0");
        }
        else if( t[i].second == "constructor" ){
            line = "push constant ";
            line += to_string(field_counter);
            handeled.push_back(line);
            handeled.push_back("call Memory.alloc 1");
            handeled.push_back("pop pointer 0");
        }

        while( i < end ){
            if( t[i].second == "(" ){
                
                i++;
                
                temp_s = i;
                temp_e = temp_s;
                while( t[i].second != ")" ){
                    i++;
                    temp_e++;
                }
                n_args = handle_parameterList( temp_s , temp_e );
                
                i++;
                
            }
            else if( t[i].second == "{" ){
                
                temp_s = i;
                temp_e = end ;
                
                temp = handle_body(temp_s , temp_e , is_void);
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                
                i = temp_e;
            }
            else{
                if( t[i].second == "void" ){
                    is_void = true;
                }
                i++;
            }

        }
        
        return {handeled , to_string(n_args)};
    }

    void handle_Var(int start , int end ){
        
        symbol s(t[end-1].second , t[start].second , t[start+1].second , 0);
        s.vm = "local" ;
        s.num = local_counter++;
        //cout << "handeling var  :  " << t[end-1].second << " , " << t[start].second << " , " << t[start+1].second << endl;
        if(subRoutine_symbols.find(t[end-1].second) == subRoutine_symbols.end()){
            subRoutine_symbols[t[end-1].second] = s;
        }
        //cout << subRoutine_symbols[t[end-1].second].vm << "  " << subRoutine_symbols[t[end-1].second].num << endl;
    }

    void handle_args(int start , int end ){
        symbol s(t[end-1].second , t[start].second , t[start+1].second , 0);
        s.vm = "argument" ;
        s.num = arg_counter++;

        if(subRoutine_symbols.find(t[end-1].second) == subRoutine_symbols.end()){
            subRoutine_symbols[t[end-1].second] = s;
        }
    }

    vector<string> handle_body(int start , int end , bool is_void){
        vector<string> handeled , temp;
        int i = start , temp_s , temp_e;
        string line = "";
        //cout << " body  :  " << t[i].second << "   " << t[end].second << endl;
        while( i < end ){
            if( t[i].second == "var"){
                temp_s = i;
                while( t[i].second != ";" ){
                    if( t[i].second == "," ){
                        handle_Var( temp_s , i );
                    }
                    i++;
                }
                temp_e = i;
                handle_Var( temp_s , temp_e );
                i = temp_e + 1;
            }
            else if( t[i].second == "{" || t[i].second == "}" ){
                i++;
            }
            else {
                temp_s = i;
                temp_e = end - 1;
                temp = handle_statement( temp_s , temp_e , is_void);
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                i = temp_e ;
            }
            
        }

        return handeled;
    }

    vector<string> compile(){
        get_curly_brackets();
        static_counter = 0;
        field_counter = 0;
        local_counter = 0;
        arg_counter = 0;
        label = 0;
        vector<string> handeled , temp;
        int i = 0 , end = t.size();
        int temp_s , temp_e;
        string line = "";

        while( i < end ){
            if( t[i].second == "static" || t[i].second == "field" ){
                temp_s = i;
                while( t[i].second != ";" ){
                    if( t[i].second == "," ){
                        handle_classVar( temp_s , i );
                    }
                    i++;
                }
                temp_e = i;
                handle_classVar( temp_s , temp_e );
                
                i = temp_e ;
            }

            else if( t[i].second == "function" || t[i].second == "method" || t[i].second == "constructor" ){

                temp_s = i;
                int b = i;
                while( t[b].second != "{" ){
                    b++;
                }
                temp_e = curly_brackets[b] + 1;

                auto temp_ = handle_subRoutine(temp_s , temp_e );
                temp = temp_.first;
                line = "function ";
                line += class_name;
                line += ".";
                line += t[i+2].second;
                line += " ";
                line += to_string(local_counter);

                handeled.push_back(line);
               
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                i = temp_e ;
            }
            
            else{
                if( i > 0 && t[i-1].second == "class"){
                    class_name = t[i].second;
                }
                i++;
                if( i >= end ){
                    break;
                }
            }

        }
        /*
        cout << "after compile" << endl;
        for(auto& sm : handeled){
            cout << sm << endl;
        }
        */
        return handeled;
    }

    void get_curly_brackets(){
        int i = 0 , end = t.size();
        stack<int> st;
        while( i < end ){
            if( t[i].second == "{" ){
                st.push(i);
            }
            else if( t[i].second == "}"){
                curly_brackets[st.top()] = i;
                st.pop();
            }
            i++;
        }
        //cout << "finished curly" << endl;

    }

    void compile_file( string full_path , string out_directory ){
        ifstream file;
        string line;
        vector<string> inputCode;
        file.open( full_path );
        while( std::getline( file , line ) ){
            if( !line.empty() ){
                inputCode.push_back(line);
            }
        }

        vector<string> handled = handle_white_spaces(inputCode);
        
        vector<token> tokens = get_tokens(handled);

        t = tokens;
        
        auto a = compile();

        ofstream output;

        output.open( out_directory );
        
        line = "" ;

        for( auto p : a ){
            line = p ;
            line += "\n" ;
            output << line ;
        }

        output.close();

        return ;
    }

};

string handle_file(string file){
    string directory = "";
    if( !is_Jack_file(file) ){
        cout << "not jack file : " << file << endl;
        return "" ;
    }
    int i = 0;
    while( file[i-1] != '.' && i < file.size() ){
        directory += file[i];
        i++;
    }
    directory += "vm";
    //cout << "got directory :  " << directory << endl;
    return directory;
}

int main(int argc, char *argv[])
{
    char* path ;
    path =  argv[1];
    string full_path = path;
    cout << "handling :  " << full_path << endl;
    vector<string> files;
    compilationEngine analyzer;
    
    if( is_directory(path) ){
        get_Jack_files( path , files);
        for( auto& f : files ){
            string out_directory = handle_file(f);
            analyzer.compile_file(f , out_directory);
        }
    }
    else if( is_Jack_file(full_path) ){
        string out_directory = handle_file(full_path);
        analyzer.compile_file(full_path , out_directory);
    }

    return 0;
}