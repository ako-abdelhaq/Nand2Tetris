#include <bits/stdc++.h>
#include <algorithm>
#include <string>
#include <bitset>
#include <unordered_map>

using namespace std;


unordered_map<string,string> dest_code = { {"" , "000"} , {"0" , "000" } ,{ "M" , "001" } , { "D" , "010" } , { "MD" , "011" } , { "A" , "100" } ,{ "AM" , "101" } ,{ "AD" , "110" } ,{ "AMD" , "111" } };
    
unordered_map<string,string> comp_code = {  {"0","101010" }, { "1","111111"}, { "-1","111010"},{ "D","001100"} , { "A" , "110000" } , { "!D","001101" }, {"!A","110001"}, {"-D","001111"}, {"-A","110011"}, {"D+1","011111"} , {"A+1","110111"} , { "D-1","001110" }, { "A-1","110010" } , {"D+A","000010"} , {"D-A","010011"}, {"A-D","000111"} , { "D&A","000000" } , { "D|A","010101"} , { "M","110000"}, { "!M","110001"} , { "-M","110011"} , { "M+1" , "110111" }, { "M-1","110010"} ,{"D+M","000010"},{"D-M","010011"},{"M-D","000111"},{"D&M","000000"}, {"D|M","010101"} };

    
unordered_map<string,string> jump_code = { { "" , "000"},{ "JGT" , "001"},{ "JEQ" , "010"},{ "JGE" , "011"}, {"JLT" , "100" }, {"JNE" , "101"}, {"JLE" , "110"}, {"JMP" , "111"} };


vector<string> handle_white_spaces(vector<string> assembly){
    vector<string> handled;
    for(int i = 0 ; i < assembly.size() ; i++){
        string line;
        for(int j = 0 ; j < assembly[i].size() ; j++){
            if( assembly[i][j] == ' '  ){
                continue;
            }
            else if( ( ( j + 1 < assembly[i].size() ) && assembly[i][j] == '/' && assembly[i][j+1] == '/' ) || int(assembly[i][j]) == 13 ){
                break;
            }
            else{
                line += assembly[i][j];
            }
        }
        if( !line.empty() && line != " "){
            handled.push_back(line);
        }
    }
    return handled;
}

string Ainstruction(string a){
    string temp = a , machine_code;
    temp.erase (temp.begin() );
    bitset<16> bin = stoi(temp);
    for(int i = 0 ; i < 16 ; i++){
        if( bin[i] == 0 ){
            machine_code += '0';
        }
        else{
            machine_code += '1';
        }
    }
    return machine_code;
}

vector<string> parse_Cinstruction(string c){
    string dest , comp , jump;
    int i = 0;
    
    while( c[i] != '=' && c[i] != ';' && c[i] != '\0'){
        dest += c[i];
        i++;
    }
    if(c[i] == '='){
        i++;
    }
    while( c[i] != ';' && c[i] != '\0'){
        comp += c[i];
        i++;
    }
    if(comp.empty()){
        comp = dest;
        dest = "";
    }
    if(c[i] == ';'){
        i++;
    }
    while( c[i] != '\0' ){
        jump += c[i];
        i++;
    }
    return { dest , comp , jump };
}

string Cinstruction(string c){

    string machine_code ;
    
    machine_code = "111";
    
    vector<string> parsed = parse_Cinstruction(c);
    
    string dest = parsed[0];
    string comp = parsed[1];
    string jump = parsed[2];
    
    //cout << dest << "," << comp << "," << jump << endl;
    //cout << dest_code[dest] << "," << comp_code[comp] << "," << jump_code[jump] << endl;
    char a = '0';
    for(int i = 0 ; i < comp.size() ; i++ ){
        if(comp[i] == 'M'){
            a = '1';
            break;
        } 
    }
    machine_code += a;
    
    machine_code += comp_code[comp];
    machine_code += dest_code[dest];
    machine_code += jump_code[jump];

    return machine_code;
}

vector<string> assembler(vector<string> assembly){
    
    vector<string> binary;
    
    for( int i = 0 ; i < assembly.size() ; i++ ){
        string bin;
        if(assembly[i][0] == '@'){
            bin = Ainstruction( assembly[i] );
            reverse(bin.begin() , bin.end());
        }else{
            bin = Cinstruction( assembly[i] );
        }
        binary.push_back(bin);
    }

    return binary;
}

vector<string> handle_symbols(vector<string>& assm){
    vector<string> handled;
    unordered_map<string,int> symbols = { {"SP",0}, {"LCL",1}, {"ARG",2}, {"THIS",3}, {"THAT",4},{"R0",0}, {"R1",1}, {"R2",2}, {"R3",3}, {"R4",4}, {"R5",5}, {"R6",6}, {"R7",7},{"R8",8}, {"R9",9}, {"R10",10}, {"R11",11}, {"R12",12}, {"R13",13}, {"R14",14}, {"R15",15},{"SCREEN",16384}, {"KBD",24576} };
    int address = 16;
    int line_number = 0;
    for( int i = 0 ; i < assm.size() ; i++ ){
        if(assm[i][0] == '('){
            int j = 1;
            string symbol = "";
            while( assm[i][j] != ')' ){
                symbol += assm[i][j];
                j++;
            }
            //cout << symbol << " , " << line_number << endl;
            symbols[symbol] = line_number ;
        }
        else{
            line_number++;
        }
        
    }
    /*
    for(auto e : symbols){
        cout << e.first << "  " << e.second << endl;
    }
    */
    for( int i = 0 ; i < assm.size() ; i++ ){
        string line;
        if(assm[i][0] == '@' && ( ( assm[i][1] > 64 && assm[i][1] < 91 ) || ( assm[i][1] > 96 && assm[i][1] < 123 ) ) ){
            int j = 1;
            string symbol = "";
            while( assm[i][j] != '\0' ){
                symbol += assm[i][j];
                j++;
            }
            if( symbols.find(symbol) == symbols.end() ){
                symbols[symbol] = address ;
                address++;
            }
            line += "@";
            line += to_string(symbols[symbol]);
            handled.push_back(line);
        }
        else if( assm[i][0] == '(' ){
            continue;
        }
        else{
            handled.push_back(assm[i]);
        }
    }

    return handled;
}

int main()
{

   vector<string> assm ;
   
   string line;
    while( std::getline( std::cin , line ) )
    {
        if( !line.empty() ){
            assm.push_back(line);
        }
    }
    
    vector<string> noSpaceNoComment = handle_white_spaces( assm );
   /*
   for( auto s : noSpaceNoComment){
       cout << s << endl;
   }
   cout << "--------" << endl;
   */
   vector<string> symbols_handled = handle_symbols(noSpaceNoComment);
   vector<string> sss = assembler(symbols_handled);
   
   for( auto s : sss){
       cout << s << endl;
   }
   return 0;
}