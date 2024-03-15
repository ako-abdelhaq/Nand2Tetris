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
            string bb = d->d_name ;
            cout << "bb :   " << bb << endl;
            if( !is_directory( d->d_name ) ){
                file = dir_name ;
                if(file.back() != '/'){
                    file += "/";
                }
                file += d->d_name;
                //cout << "in full -> "<< file << endl;
                if( is_Jack_file(file) ){
                    files.push_back(file);
                }
            }
        }
        closedir(dr);
    }
}

typedef pair<string,string> token; 

string HT = "  "; // Horizontal tab

vector<string> handle_white_spaces(vector<string> Jack_file){
    vector<string> handled;
    bool block_comment = false;
    for(int i = 0 ; i < (int)Jack_file.size() ; i++){
        string line;
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
                    line += Jack_file[i][j];
                }
            }
        }
        string out = "";
        if( !line.empty() && line != " " ){
            int i = 0;
            while( line[i] == ' ' || int( line[i] ) == 13 || int( line[i] ) == 9 ){
                i++;
            }

            while( i < line.size() ){
                out += line[i];
                i++;
            }

            while(  out.back() == ' ' || int( out.back() ) == 13){
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

bool is_symbol( char c ){
    if(c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
        c == '.' || c == ',' || c == ';' || c == '+' || c == '-' || c == '*' ||
        c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' ||
        c == '~') {
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
            if( input[i][j] == ' ' ){
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
                
                else{
                    command = input[i][j];
                }
                j++;
            }
            else if( input[i][j] == '"' ){
                command = "\"";
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
    unordered_map<int,int> curly_brackets;

    vector<string> handle_statement( int start , int end , string tab ){
        vector<string> handeled;
        string line = tab , end_ = tab;
        line += "<statements>";
        end_ += "</statements>";
        handeled.push_back(line);
        int i = start , temp_s , temp_e ;
        temp_s = i;
        temp_e = temp_s;
        while( i < end ){
            if(t[i].second == "if"){
                vector<string> temp;
                string n_tab = tab;
                n_tab += "  ";
                temp_s = i;
                
                while( t[i].second != "{" ){
                    i++;
                }
                temp_e = curly_brackets[i] + 1;
                if( t[temp_e].second == "else" ){
                    temp_e++;
                    temp_e = curly_brackets[temp_e] + 1;
                }
                
                temp = handle_if( temp_s , temp_e , n_tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e;
            }

            else if(t[i].second == "while"){
                vector<string> temp;
                string n_tab = tab;
                n_tab += "  ";
                temp_s = i;
                while( t[i].second != "{" ){
                    i++;
                }
                temp_e = curly_brackets[i] + 1;
                temp = handle_while( temp_s , temp_e , n_tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e;
            }
            
            else if( t[i].second == "let" ){
                vector<string> temp;
                string n_tab = tab;
                n_tab += "  ";
                temp_s = i;
                temp_e = temp_s;
                i++;
                while( t[i-1].second != ";" ){
                    i++;
                    temp_e++;
                }
                temp_e++;
                temp = handle_let( temp_s , temp_e , n_tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e ;
            }
            
            else if(t[i].second == "do"){
                vector<string> temp;
                string n_tab = tab;
                n_tab += "  ";
                temp_s = i;
                temp_e = temp_s;
                i++;
                while( t[i-1].second != ";" ){
                    i++;
                    temp_e++;
                }
                temp_e++;
                temp = handle_do( temp_s , temp_e , n_tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e ;
            }

            else if(t[i].second == "return"){
                vector<string> temp;
                string n_tab = tab;
                n_tab += "  ";
                temp_s = i;
                temp_e = temp_s;
                i++;
                while( t[i-1].second != ";" ){
                    i++;
                    temp_e++;
                }
                temp_e++;
                temp = handle_return( temp_s , temp_e , n_tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                i = temp_e ;
            }

            else{
                break;
            }

        }

        vector<string> temp ;
        
        handeled.push_back(end_);

        return handeled;
    }

    vector<string> handle_if( int start , int end , string tab ){
        pair<block,block> p = eat( start , end );
        int start_e = p.first.start , end_e = p.first.end ;
        int start_s = p.second.start , end_s = p.second.end;
        vector<string> handeled , exp , stm;
        string line = tab , end_ = tab;
        unordered_map<int,int> e_brackets = handle_brackets(start_e , end_e);
        line += "<ifStatement>";
        end_ += "</ifStatement>";
        handeled.push_back(line);

        tab += "  ";
        line = tab ;
        line += "<keyword> if </keyword>" ;
        handeled.push_back(line);
        line = tab ;
        line += "<symbol> ( </symbol>" ;
        handeled.push_back(line);
        exp = handle_expression( start_e , end_e , tab , e_brackets);
        line = tab ;
        line += "<symbol> ) </symbol>" ;
        exp.push_back(line);
        line = tab ;
        line += "<symbol> { </symbol>" ;
        exp.push_back(line);
        handeled.insert(handeled.end() , exp.begin() , exp.end());
        
        stm = handle_statement( start_s , end_s , tab );
        line = tab ;
        line += "<symbol> } </symbol>" ;
        stm.push_back(line);
        handeled.insert(handeled.end() , stm.begin() , stm.end());
        int i = end_s + 1 ;

        if( t[i].second == "else" ){
            line = tab ;
            line += "<keyword> else </keyword>" ;
            handeled.push_back(line);
            i++;
            line = tab ;
            line += "<symbol> { </symbol>" ;
            handeled.push_back(line);
            i++;
            stm = handle_statement( i , end , tab );
            line = tab ;
            line += "<symbol> } </symbol>" ;
            stm.push_back(line);
            handeled.insert(handeled.end() , stm.begin() , stm.end());

        }

        handeled.push_back(end_);
        
        return handeled;
    }


    vector<string> handle_while( int start , int end , string tab ){
        pair<block,block> p = eat( start , end );
        int start_e = p.first.start , end_e = p.first.end ;
        int start_s = p.second.start , end_s = p.second.end;
        vector<string> handeled , exp , stm;
        string line = tab , end_ = tab;
        
        unordered_map<int,int> e_brackets = handle_brackets(start_e , end_e);
        
        line += "<whileStatement>";
        end_ += "</whileStatement>";
        handeled.push_back(line);

        tab += "  ";
        line = tab ;
        line += "<keyword> while </keyword>" ;
        handeled.push_back(line);
        line = tab ;
        line += "<symbol> ( </symbol>" ;
        handeled.push_back(line);
        exp = handle_expression( start_e , end_e , tab , e_brackets);
        line = tab ;
        line += "<symbol> ) </symbol>" ;
        exp.push_back(line);
        line = tab ;
        line += "<symbol> { </symbol>" ;
        exp.push_back(line);
        handeled.insert(handeled.end() , exp.begin() , exp.end());     
     
        stm = handle_statement( start_s , end_s , tab );
        line = tab ;
        line += "<symbol> } </symbol>" ;
        stm.push_back(line);
        handeled.insert(handeled.end() , stm.begin() , stm.end());

        handeled.push_back(end_);
        
        return handeled;
    }

    vector<string> handle_let( int start , int end , string tab){
        vector<string> handeled , exp0 , exp1;
        unordered_map<int,int> b = {};
        string line = tab , end_ = tab;
        int temp_s , temp_e;
        line += "<letStatement>";
        end_ += "</letStatement>";
        handeled.push_back(line);
        tab += "  ";
        line = tab;
        line += "<keyword> let </keyword>";
        handeled.push_back(line);
        int i = start + 1 ;
        line = tab ;
        line += "<" ;
        line += t[i].first ;
        line += "> " ;
        line += t[i].second ;
        line += " </" ;
        line += t[i].first ;
        line += ">" ;
        handeled.push_back(line);
        line = "";
        i++;
        if( t[i].second == "[" ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
            temp_s = i;
            temp_e = temp_s;
            while( t[i].second != "]" ){
                i++;
                temp_e++;
            }
            
            exp0 = handle_expression(temp_s , temp_e , tab , b);

            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            exp0.push_back(line);
            line = "";
            i++;

            handeled.insert(handeled.end() , exp0.begin() , exp0.end() );
        }
        line = tab ;
        line += "<" ;
        line += t[i].first ;
        line += "> " ;
        line += t[i].second ;
        line += " </" ;
        line += t[i].first ;
        line += ">" ;
        handeled.push_back(line);
        line = "";
        i++;

        temp_s = i;
        temp_e = i;
        while( t[i].second != ";" && i < end ){
            i++;
            temp_e++;
        }
        b = handle_brackets(temp_s , temp_e);
        exp1 = handle_expression(temp_s , temp_e , tab , b);
        handeled.insert(handeled.end() , exp1.begin() , exp1.end() );
        
        line = tab ;
        line += "<" ;
        line += t[i].first ;
        line += "> " ;
        line += t[i].second ;
        line += " </" ;
        line += t[i].first ;
        line += ">" ;
        handeled.push_back(line);
        line = "";
        
        handeled.push_back(end_);

        
        return handeled;
    }

    vector<string> handle_do( int start , int end , string tab){
        vector<string> handeled , exp_list;
        string line = tab , end_ = tab;
        int temp_s , temp_e;

        line += "<doStatement>";
        end_ += "</doStatement>";
        handeled.push_back(line);
        tab += "  ";
        int i = start ;
        temp_s = i;
        temp_e = temp_s;
        while( t[i-1].second != "(" ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
            temp_s++;
        }

        temp_e = end - 2;
        line = tab;
        line += "<expressionList>";
        handeled.push_back(line);
        
        exp_list = handle_expressionList(temp_s , temp_e , tab);
        handeled.insert(handeled.end() , exp_list.begin() , exp_list.end() );
        
        line = tab;
        line += "</expressionList>";
        handeled.push_back(line);
        i = temp_e ;
        while( i < end ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
        }

        handeled.push_back(end_);

        return handeled;
    }

    vector<string> handle_expressionList(int start , int end , string tab){
        vector<string> handeled , exp;
        unordered_map<int,int> b;
        string line = tab , cur = tab;
        int temp_s , temp_e , i = start;
        tab += "  ";
        temp_s = i;
        temp_e = temp_s;
        
        while( i < end ){
            if( t[i].second == "," ){
                b = handle_brackets(temp_s , temp_e);
                exp = handle_expression(temp_s , temp_e , tab , b);
                handeled.insert(handeled.end() , exp.begin() , exp.end());
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
                temp_s = i;
                temp_e = temp_s ;
            }
            else{
                i++;
                temp_e++;
            }
            
            
        }

        if( temp_e > temp_s ){
            b = handle_brackets(temp_s , temp_e);
            exp = handle_expression(temp_s , temp_e , tab , b);
            handeled.insert(handeled.end() , exp.begin() , exp.end());
        }
            
        
        return handeled;
    } 

    vector<string> handle_return( int start , int end , string tab){
        vector<string> handeled , exp1;
        unordered_map<int,int> b = {};
        string line = tab , end_ = tab;
        int temp_s , temp_e;
        line += "<returnStatement>";
        end_ += "</returnStatement>";
        handeled.push_back(line);
        tab += "  ";
        line = tab;
        line += "<keyword> return </keyword>";
        handeled.push_back(line);
        int i = start + 1 ;
        temp_s = i;
        temp_e = temp_s;
        while( t[i].second != ";" ){
            i++;
            temp_e++;
        }
        b = handle_brackets(temp_s , temp_e);
        if( temp_s < temp_e ){
            exp1 = handle_expression(temp_s , temp_e , tab , b);
            handeled.insert(handeled.end() , exp1.begin() , exp1.end() );
        }
        
        line = tab ;
        line += "<" ;
        line += t[i].first ;
        line += "> " ;
        line += t[i].second ;
        line += " </" ;
        line += t[i].first ;
        line += ">" ;
        handeled.push_back(line);
        line = "";

        handeled.push_back(end_);
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

    vector<string> handle_term( int start , int end , string tab , unordered_map<int,int> brackets){
        vector<string> handeled;
        vector<string> temp;
        string line = tab , end_ = tab;
        line += "<term>";
        end_ += "</term>";
        handeled.push_back(line);
        int i = start;
        tab += "  ";
        
        while( i < end ){
            if( t[i].second == "(" || t[i].second == "[" ){
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);

                temp = handle_expression( i + 1 , brackets[i] , tab , brackets);
                handeled.insert(handeled.end() , temp.begin() , temp.end() );
                line = tab ;
                line += "<" ;
                line += t[brackets[i]].first ;
                line += "> " ;
                line += t[brackets[i]].second ;
                line += " </" ;
                line += t[brackets[i]].first ;
                line += ">" ;
                handeled.push_back(line);
                break;
            }
            else if( t[i].first == "integerConstant" || t[i].first == "stringConstant" || t[i].first == "identifier" || is_keyword( t[i].second ) ){
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
            }
            else{
                exit(1);
                break;
            }
        }

        handeled.push_back(end_);

        return handeled;
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

    vector<string> handle_unary( int start , int end , string tab , unordered_map<int,int> brackets){
        vector<string> handeled;
        vector<string> temp;
        string line = tab , end_ = tab;
        line += "<term>";
        end_ += "</term>";
        handeled.push_back(line);
        int i = start;
        tab += "  ";

        line = tab ;
        line += "<" ;
        line += t[i].first ;
        line += "> " ;
        line += t[i].second ;
        line += " </" ;
        line += t[i].first ;
        line += ">" ;
        
        handeled.push_back(line);
        line = "";
        i++;
        
        temp = handle_term( i , end , tab , brackets );
        handeled.insert(handeled.end() , temp.begin() , temp.end());

        
        handeled.push_back(end_);

        return handeled;
    }

    vector<string> handle_expression( int start , int end , string tab , unordered_map<int,int> brackets){
        vector<string> handeled;
        vector<string> sub_expression;
        vector<string> term;
        string line = tab , end_ = tab;
        line += "<expression>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";
        
        int i = start;

        while( i < end ){
            
            if( ( t[i-1].first != "identifier" && t[i-1].second != ")" ) && is_unary( t[i].second ) && t[i+1].second == "(" ){
                sub_expression = handle_unary( i , brackets[i+1] + 1, tab , brackets);
                handeled.insert(handeled.end() , sub_expression.begin() , sub_expression.end());
                i = brackets[i+1] + 1;
            }
            else if( ( t[i-1].first != "identifier" && t[i-1].second != ")" ) && is_unary( t[i].second ) && ( t[i+1].first == "identifier" || t[i+1].first == "integerConstant" ) ){
                sub_expression = handle_unary( i , i+2 , tab , brackets);
                handeled.insert(handeled.end() , sub_expression.begin() , sub_expression.end());
                i += 2;
            }
            else if( t[i].first == "identifier" && t[i+1].second == "(" && i + 1 < end ){
                vector<string> exp_list;
                int temp_s , temp_e;

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i;
                temp_e = temp_s;
                while( t[i].second != ")" ){
                    i++;
                    temp_e++;
                }
                line = tab;
                line += "<expressionList>";
                handeled.push_back(line);

                exp_list = handle_expressionList(temp_s , temp_e , tab);
                handeled.insert(handeled.end() , exp_list.begin() , exp_list.end() );
                
                line = tab;
                line += "</expressionList>";
                handeled.push_back(line);

                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";

                i++;

            }
            else if(t[i].first == "identifier" && t[i+1].second == "." && i + 1 < end){
                vector<string> exp_list;
                int temp_s , temp_e;
                string s_s = tab , s_e = tab;

                s_s += "<term>";
                s_e += "</term>";
                tab += "  ";
                handeled.push_back(s_s);

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i ;
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;

                temp_s = i;
                temp_e = temp_s;
                while( t[i].second != ")" ){
                    i++;
                    temp_e++;
                }
                line = tab;
                line += "<expressionList>";
                handeled.push_back(line);

                exp_list = handle_expressionList(temp_s , temp_e , tab);
                handeled.insert(handeled.end() , exp_list.begin() , exp_list.end() );
                
                line = tab;
                line += "</expressionList>";
                handeled.push_back(line);

                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";

                handeled.push_back(s_e);
                i++;
            }
            else if( t[i].first == "identifier" && t[i+1].second == "[" ){
                term = handle_term(i , brackets[ i + 1 ] , tab , brackets);
                handeled.insert(handeled.end() , term.begin() , term.end());
                i = brackets[i + 1] + 1;
            }
            else if(t[i].second == "("){
                sub_expression = handle_term( i , brackets[i] , tab , brackets);
                handeled.insert(handeled.end() , sub_expression.begin() , sub_expression.end());
                i = brackets[i] + 1;
            }
            else if(t[i].first == "symbol"){
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
            }
            else{
                term = handle_term(i , i + 1 , tab , brackets);
                handeled.insert(handeled.end() , term.begin() , term.end());
                i++;
            }
        }
        
        line = end_;
        line += "</expression>";
        handeled.push_back(line);
        return handeled;
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

    vector<string> handle_classVar(int start , int end , string tab){
        vector<string> handeled;
        int i = start ;
        string line = tab , end_ = tab;
        line += "<classVarDec>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";
        while( i < end ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
        }

        line = end_;
        line += "</classVarDec>";
        handeled.push_back(line);
        return handeled;
    }

    vector<string> handle_parameterList(int start , int end , string tab){
        vector<string> handeled;
        int i = start ;
        string line = tab , end_ = tab;

        line += "<parameterList>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";
        while( i < end ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
        }
        line = end_;
        line += "</parameterList>";
        handeled.push_back(line);

        return handeled;
    }

    vector<string> handle_subRoutine(int start , int end , string tab ){
        vector<string> handeled , temp;
        int i = start , temp_s , temp_e;
        string line = tab , end_ = tab;
        line += "<subroutineDec>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";

        while( i < end ){

            if( t[i].second == "(" ){
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
                
                temp_s = i;
                temp_e = temp_s;
                while( t[i].second != ")" ){
                    i++;
                    temp_e++;
                }
                temp = handle_parameterList(temp_s , temp_e , tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end());

                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
                
            }
            else if( t[i].second == "{" ){
                temp_s = i;
                temp_e = end ;
                
                temp = handle_body(temp_s , temp_e , tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                
                i = temp_e;
            }
            else{
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
            }

        }
        
        line = end_;
        line += "</subroutineDec>";
        handeled.push_back(line);

        return handeled;
    }

    vector<string> handle_Var(int start , int end , string tab){
        vector<string> handeled;
        int i = start ;
        string line = tab , end_ = tab;
        line += "<varDec>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";
        while( i < end ){
            line = tab ;
            line += "<" ;
            line += t[i].first ;
            line += "> " ;
            line += t[i].second ;
            line += " </" ;
            line += t[i].first ;
            line += ">" ;
            handeled.push_back(line);
            line = "";
            i++;
        }
        line = end_;
        line += "</varDec>";
        handeled.push_back(line);
        return handeled;
    }

    vector<string> handle_body(int start , int end , string tab ){
        vector<string> handeled , temp;
        int i = start , temp_s , temp_e;
        string line = tab , end_ = tab;
        line += "<subroutineBody>";
        handeled.push_back(line);
        line = tab;
        tab += "  ";

        while( i < end ){

            if( t[i].second == "var"){
                temp_s = i;
                while( t[i].second != ";" ){
                    i++;
                }
                temp_e = i + 1;
                temp = handle_Var( temp_s , temp_e , tab );
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                i = temp_e ;
            }
            else if( t[i].second == "{" || t[i].second == "}" ){
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = "";
                i++;
            }
            else {
                temp_s = i;
                temp_e = end - 1;

                temp = handle_statement( temp_s , temp_e , tab);
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                i = temp_e ;
            }
            
        }

        line = end_;
        line += "</subroutineBody>";
        handeled.push_back(line);

        return handeled;
    }

    vector<string> compile(){
        get_curly_brackets();
        vector<string> handeled , temp;
        int i = 0 , end = t.size();
        int temp_s , temp_e;
        string tab = "" , line ;
        handeled.push_back("<class>");
        line = tab;
        tab += "  ";
        while( i < end ){

            if( t[i].second == "static" || t[i].second == "field" ){
                temp_s = i;
                while( t[i].second != ";" ){
                    i++;
                }
                temp_e = i + 1;
                temp = handle_classVar( temp_s , temp_e , tab );
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                
                i = temp_e ;
            }

            else if( t[i].second == "function" || t[i].second == "method" || t[i].second == "constructor" ){
                temp_s = i;
                int b = i;
                while( t[b].second != "{" ){
                    b++;
                }
                temp_e = curly_brackets[b] + 1;

                temp = handle_subRoutine(temp_s , temp_e , tab);
               
                handeled.insert(handeled.end() , temp.begin() , temp.end());
                i = temp_e ;
            }
            
            else{
                line = tab ;
                line += "<" ;
                line += t[i].first ;
                line += "> " ;
                line += t[i].second ;
                line += " </" ;
                line += t[i].first ;
                line += ">" ;
                handeled.push_back(line);
                line = tab;
                i++;
            }

        }
        
        handeled.push_back("</class>");
        
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

    }

    void compile_file( string full_path , string out_directory ){
        ifstream file;
        string line;
        vector<string> inputCode;
        file.open( full_path );
        while( std::getline( file , line ) ){
            //cout << line << endl;
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
    }

};

string handle_file(string file){
    string directory ;
    if( !is_Jack_file(file) ){
        cout << "not jack file : " << file << endl;
        return "" ;
    }
    int i = 0;
    while( file[i-1] != '.' && i < file.size() ){
        directory += file[i];
        i++;
    }
    directory += "xml";
    //cout << "got directory :  " << directory << endl;
    return directory;
}

int main(int argc, char *argv[])
{
    char* path ;
    path =  argv[1];
    string full_path = path;
    vector<string> files;
    compilationEngine analyzer;
    if( is_Jack_file(full_path) ){
        string out_directory = handle_file(full_path);
        //cout << full_path << endl;
        //cout << out_directory << endl;
        analyzer.compile_file(full_path , out_directory);
    }
    else if( is_directory(path) ){
        get_Jack_files( path , files);
        for( auto f : files ){
            string out_directory = handle_file(f);
            //cout << f << endl;
            //cout << out_directory << endl;
            analyzer.compile_file(f , out_directory);
        }
    }

    return 0;
}