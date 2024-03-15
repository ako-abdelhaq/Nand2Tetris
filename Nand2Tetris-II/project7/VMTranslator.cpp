#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <bitset>
#include <unordered_map>


using namespace std;

struct command{
    int push_pop;
    string segment;
    int i;
    
    command( int p , string s , int in ){
        push_pop = p;
        segment = s;
        i = in;
    }
    
};



class VMImplementation{
  private:
    vector<string> VMcode;
    string VMfile;
    unordered_map<string , string> VMtable = { {"local" , "LCL"} , {"argument" , "ARG" } , {"this" , "THIS" } , { "that" , "THAT" } };
    
    vector<string> address_code( string base , string i ){
        vector<string> assembly;
        string line;
        line = "@";
        line += base;
        assembly.push_back(line);
        assembly.push_back("D=M");
        line = "@";
        line += i;
        assembly.push_back(line);
        assembly.push_back("D=D+A");
        assembly.push_back("@R13");
        assembly.push_back("M=D");
        return assembly;
    }
    
    
    
    vector<string> pointer_pointer( string left , string right ){
        vector<string> assembly;
        string line;
        line = "@";
        line += right;
        assembly.push_back(line);
        assembly.push_back("A=M");
        assembly.push_back("D=M");
        line = "@";
        line += left;
        assembly.push_back(line);
        assembly.push_back("A=M");
        assembly.push_back("M=D");
        return assembly;
    }
    
    vector<string> pointer( int ptr ){
        vector<string> assembly;
        string line;
        line = "@";
        line += ptr ? "THAT" : "THIS" ;
        assembly.push_back(line);
        
        assembly.push_back("D=M");
        assembly.push_back("@SP");
        assembly.push_back("A=M");
        assembly.push_back("M=D");
        return assembly;
    }
    
    vector<string> value( int ptr ){
        vector<string> assembly;
        assembly.push_back("@SP");
        assembly.push_back("A=M");
        assembly.push_back("D=M");
        
        string line;
        line = "@";
        line += ptr ? "THAT" : "THIS" ;
        assembly.push_back(line);
        
        assembly.push_back("M=D");
        return assembly;
    }
    
    vector<string> incrSP(){
        return { "@SP" , "M=M+1"};
    }
    
    vector<string> decrSP(){
        return { "@SP" , "M=M-1"};
    }
    
  public:
    VMImplementation() {}
    
    VMImplementation(vector<string> code , string file_name){
        VMcode = code;
        VMfile = file_name;
    }
    
    vector<string> getVMcode(){
        return VMcode;
    }
    
    vector<string> push_constant( int i ){
        vector<string> assembly;
        string line;
        line = "@";
        line += to_string(i);
        assembly.push_back(line);
        assembly.push_back("D=A");
        assembly.push_back("@SP");
        assembly.push_back("A=M");
        assembly.push_back("M=D");
        assembly.push_back("@SP");
        assembly.push_back("M=M+1");
        return assembly;
    }
    
    vector<string> push_segment( string segmentName , int i ){
        
        vector<string> assembly;
        string segmentPointer = VMtable[segmentName];
        string str = to_string(i);
        vector<string> command1 = address_code( segmentPointer , str );
        vector<string> command2 = pointer_pointer("SP" , "R13");
        vector<string> command3 = incrSP();
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        assembly.insert(assembly.end(), command3.begin(), command3.end());
        return assembly;
    }
    
    vector<string> pop_segment( string segmentName , int i ){
        
        vector<string> assembly;
        string segmentPointer = VMtable[segmentName];
        string str = to_string(i);
        vector<string> command1 = address_code( segmentPointer , str );
        vector<string> command2 = decrSP();
        vector<string> command3 = pointer_pointer("R13" , "SP");
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        assembly.insert(assembly.end(), command3.begin(), command3.end());
        return assembly;
    }
    
    vector<string> push_temp( int i ){
        vector<string> assembly;
        int tempAddress = i + 5;
        string address = "@" ;
        address += to_string(tempAddress);
        assembly.push_back(address);
        assembly.push_back("D=A");
        assembly.push_back("@R13");
        assembly.push_back("M=D");
        vector<string> command1 = pointer_pointer("SP" , "R13");
        vector<string> command2 = incrSP();
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    vector<string> pop_temp( int i ){
        vector<string> assembly;
        int tempAddress = i + 5;
        string address = "@" ;
        address += to_string(tempAddress);
        assembly.push_back(address);
        assembly.push_back("D=A");
        assembly.push_back("@R13");
        assembly.push_back("M=D");
        vector<string> command1 = decrSP();
        vector<string> command2 = pointer_pointer("R13" , "SP");
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    vector<string> push_pointerSegment( int ptr ){
        vector<string> assembly;
        vector<string> command1 = pointer( ptr );
        vector<string> command2 = incrSP();
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    vector<string> pop_pointerSegment( int ptr ){
        vector<string> assembly;
        vector<string> command1 = decrSP();
        vector<string> command2 = value( ptr );
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    vector<string> push_static( int i ){
        vector<string> assembly;
        int tempAddress = i + 16;
        string address = "@" ;
        address += to_string(tempAddress);
        assembly.push_back(address);
        assembly.push_back("D=A");
        assembly.push_back("@R13");
        assembly.push_back("M=D");
        vector<string> command1 = pointer_pointer("SP" , "R13");
        vector<string> command2 = incrSP();
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    vector<string> pop_static( int i ){
        vector<string> assembly;
        int tempAddress = i + 16;
        string address = "@" ;
        address += to_string(tempAddress);
        assembly.push_back(address);
        assembly.push_back("D=A");
        assembly.push_back("@R13");
        assembly.push_back("M=D");
        vector<string> command1 = decrSP();
        vector<string> command2 = pointer_pointer("R13" , "SP");
        assembly.insert(assembly.end(), command1.begin(), command1.end());
        assembly.insert(assembly.end(), command2.begin(), command2.end());
        return assembly;
    }
    
    
    
    
    command parse_command(string s){
        string operation ;
        string segment ;
        string num;
        int i = 0;
        while( s[i] != ' ' && s[i] != '\0' ){
            operation += s[i];
            i++;
        }
        i++;
        while( s[i] != ' ' && s[i] != '\0' ){
            segment += s[i];
            i++;
        }
        i++;
        while( s[i] != ' ' && s[i] != '\0' ){
            num += s[i];
            i++;
        }
        
        int p;
        
        if(operation == "push"){
            p = 1;
        }else{
            p = 0;
        }
        
        int in = stoi( num );
        
        command cmd( p , segment , in );
        //cout << p << "  " << segment << "  " << in << endl;
        return cmd;
    }
    
    vector<string> machine_code(){
        vector<string> translated;
        vector<string> implementation;
        for( int i = 0 ; i < (int)VMcode.size() ; i++ ){
            implementation = {};
            if( VMcode[i] == "add" ){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=D+M");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "sub" ){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M-D");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "neg"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=-M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "gt"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M-D");
                
                string next = "@" + to_string( int(translated.size()) + int(implementation.size() + 7 ));
                implementation.push_back(next);
                implementation.push_back("D;JGT");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                next = "@" + to_string( int(translated.size()) + int(implementation.size() + 5 ));
                implementation.push_back(next);
                implementation.push_back("0;JMP");
                
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "eq"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M-D");
                
                string next = "@" + to_string( int(translated.size()) + int(implementation.size() + 7 ));
                implementation.push_back(next);
                implementation.push_back("D;JEQ");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                next = "@" + to_string( int(translated.size()) + int(implementation.size() + 5 ));
                implementation.push_back(next);
                implementation.push_back("0;JMP");
                
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "lt"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M-D");
                
                string next = "@" + to_string( int(translated.size()) + int(implementation.size() + 7 ));
                implementation.push_back(next);
                implementation.push_back("D;JLT");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                next = "@" + to_string( int(translated.size()) + int(implementation.size() + 5 ));
                implementation.push_back(next);
                implementation.push_back("0;JMP");
                
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "and"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=D&M");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "or"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=M");
                vector<string> command1 = decrSP();
                implementation.insert(implementation.end() , command1.begin() , command1.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=D|M");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i] == "not"){
                vector<string> command0 = decrSP();
                implementation.insert(implementation.end() , command0.begin() , command0.end() );
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("D=!M");
                implementation.push_back("M=D");
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else{
                command parsed = parse_command( VMcode[i] );
                int p = parsed.push_pop;
                string segment = parsed.segment;
                int n = parsed.i;
                if( p ){
                    // push
                    if( segment == "constant" ){
                        implementation = push_constant( n );
                    }
                    else if( segment == "temp" ){
                        implementation = push_temp( n );
                    }
                    else if( segment == "pointer" ){
                        implementation = push_pointerSegment( n );
                    }
                    else if( segment == "static" ){
                        implementation = push_static( n );
                    }
                    else{
                        implementation = push_segment( segment , n );
                    }
                    
                }
                else{
                    // pop
                    
                    if( segment == "temp" ){
                        implementation = pop_temp( n );
                    }
                    else if( segment == "pointer" ){
                        implementation = pop_pointerSegment( n );
                    }
                    else if( segment == "static" ){
                        implementation = pop_static( n );
                    }
                    else{
                        implementation = pop_segment( segment , n );
                    }
                }
            }
            translated.insert(translated.end() , implementation.begin() , implementation.end() );
        }
        return translated;
    }
};

vector<string> handle_white_spaces(vector<string> assembly){
    vector<string> handled;
    for(int i = 0 ; i < (int)assembly.size() ; i++){
        string line;
        for(int j = 0 ; j < (int)assembly[i].size() ; j++){
            if( ( ( j + 1 < (int)assembly[i].size() ) && assembly[i][j] == '/' && assembly[i][j+1] == '/' ) || int(assembly[i][j]) == 13 ){
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

void handle_file(string directory , string file){
    
    vector<string> inputCode , VMcode;
    string full_path = directory + file + ".vm";
    ifstream VMfile;
    VMfile.open( full_path );
    string line;
    while( std::getline( VMfile , line ) ){
        
        if( !line.empty() ){
            inputCode.push_back(line);
        }
        
    }
    
    VMcode = handle_white_spaces(inputCode);
    

    VMImplementation VM(VMcode , file);
    
    vector<string> machine_code = VM.machine_code();
    
    string outdirectory = directory + file + ".asm";
    
    ofstream VMI( outdirectory );
    

    for(auto s : machine_code){
        VMI << s << "\n";
    }

    VMI.close();

}


pair<string,string> get_file(string directory){
	
	string file_extension = "" , file_name = "" , output_directory = "";
	int n = directory.size() - 1 ;
	
	file_extension += directory[n-1];
	file_extension += directory[n];
	
	if( file_extension != "vm" || directory.empty() ){
		return {output_directory , "Invalid file!"};
	}
	
	int i = n-3;
	while( int(directory[i]) != 47 && i > -1 ){
		file_name += directory[i] ;
		i--;
	}
    
	while( i > -1 ){
		output_directory += directory[i] ;
		i--;
	}
	
	std::reverse( file_name.begin() , file_name.end() );
	
	std::reverse( output_directory.begin() , output_directory.end() );

	return {output_directory,file_name};
}

int main()
{

    string directory;
    
    vector<string> files = { "BasicTest/BasicTest.vm" , "PointerTest/PointerTest.vm" , "StaticTest/StaticTest.vm" , "SimpleAdd/SimpleAdd.vm" , "StackTest/StackTest.vm" };

    for(auto f : files){
        directory = f;
        //cout << directory << endl; 
        if( !directory.empty()  ){

            pair<string,string> p = get_file(directory);
                        
            if( p.second != "Invalid file!" ){
                handle_file(p.first , p.second);
            }
            
        }
    
    }
    return 0;
}