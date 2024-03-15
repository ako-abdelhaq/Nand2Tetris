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
   
   vector<string> sss = assembler(noSpaceNoComment);
   
   for( auto s : sss){
       cout << s << endl;
   }
   return 0;
}