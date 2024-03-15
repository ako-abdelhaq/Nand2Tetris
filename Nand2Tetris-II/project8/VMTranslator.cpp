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

bool is_vm_file(string file){
    int n = file.size() - 1;

    if( n < 3 ){
        return false;
    } 
    else if( file[n] == 'm' && file[n-1] == 'v' && file[n-2] == '.'){
        return true;
    }
    else{
        return false;
    }
}

void get_vm_files( const char *path , vector<string>& files){
    struct dirent *d;
    DIR *dr = NULL;
    dr = opendir(path);
    string dir_name = path;
    string file_name;
    if(dr != NULL){
        for( d = readdir(dr) ; d != NULL ; d = readdir(dr) ){
            string bb = d->d_name ;
            if( is_directory( d->d_name ) ){
                continue;
            }
            else{
                file_name = d->d_name;
                string full = dir_name ;
                if(full.back() != '/'){
                    full += '/';
                } 
                full += file_name;
                if( is_vm_file(full) ){
                    files.push_back(full);
                }
            }
        }
        closedir(dr);
    }
}


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

struct b_command{
    string control;
    string label;
    
    b_command( string c , string l ){
        control = c;
        label = l;
    }
    
};

struct f_command{
    string function;
    string name;
    int n;
    
    f_command( string f , string f_n , int n_ ){
        function = f;
        name = f_n;
        n = n_;
    }
    
};

class VMImplementation{
  private:
    vector<string> VMcode;
    string VMfile;
    unordered_map<string , string> VMtable = { {"local" , "LCL"} , {"argument" , "ARG" } , {"this" , "THIS" } , { "that" , "THAT" } };
    int nReturn;
    int nGT;
    int nEQ;
    int nLT;
    map<int,string> line_to_file;
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
    
    VMImplementation(vector<string> code , string file_name , map<int,string> m){
        VMcode = code;
        VMfile = file_name;
        nReturn = 1;
        nGT = 1;
        nLT = 1;
        nEQ = 1;
        line_to_file = m;
    }
    
    vector<string> getVMcode(){
        return VMcode;
    }
    
    string get_file_name( int i ){
        string file = VMfile;
        for(auto e : line_to_file){
            if( i <= e.first ){
                file = e.second;
                break;
            }
        }
        return file;
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
    
    vector<string> push_static( int i , int line){
        vector<string> assembly;
        string address = "@static." ;
        address += get_file_name(line);
        address += ".";
        address += to_string(i);
        assembly.push_back(address);
        assembly.push_back("D=M");
        assembly.push_back("@SP");
        assembly.push_back("A=M");
        assembly.push_back("M=D");
        assembly.push_back("@SP");
        assembly.push_back("M=M+1");
        
        return assembly;
    }
    
    vector<string> pop_static( int i , int line){
        vector<string> assembly;
        string address = "@static." ;
        address += get_file_name(line);
        address += ".";
        address += to_string(i);

        assembly.push_back("@SP");
        assembly.push_back("A=M-1");
        assembly.push_back("D=M");
        
        assembly.push_back(address);
        assembly.push_back("M=D");

        assembly.push_back("@SP");
        assembly.push_back("M=M-1");

        return assembly;
    }
    
    vector<string> push( string s ){
        vector<string> assembly;
        string line = "@" + s;
        assembly.push_back(line);
        assembly.push_back("D=M");
        assembly.push_back("@SP");
        assembly.push_back("A=M");
        assembly.push_back("M=D");
        assembly.push_back("@SP");
        assembly.push_back("M=M+1");
        return assembly;
    }
    

    
    command parse_command(string s){
        string operation ;
        string segment ;
        string num;
        int i = 0;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
            operation += s[i];
            i++;
        }
        i++;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
            segment += s[i];
            i++;
        }
        i++;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
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

        return cmd;
    }
    
    bool is_branching(string code){
        string op;
        int i = 0;
        while( i < code.size() && code[i] != ' ' && code[i] != '\0' ){
            op += code[i];
            i++;
        }
        if( op == "goto" || op == "if-goto" || op == "label" ){
            return true;
        }
        else{
            return false;
        }

    }

    b_command parse_branching(string s){
        string control ;
        string label ;
        int i = 0;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
            control += s[i];
            i++;
        }
        i++;
        while( i < s.size() &&  s[i] != '\0' ){
            label += s[i];
            i++;
        }
        b_command cmd(control , label);
        return cmd;
    }

    bool is_function(string code){
        string op;
        int i = 0;
        
        while( code[i] != ' ' && code[i] != '\0' && i < code.size() ){
            op += code[i];
            i++;
        }
        
        if( op == "function" || op == "call" || op == "return" ){
            return true;
        }
        else{
            return false;
        }
    }

    f_command parse_function(string s){

        string function ;
        string name ;
        string num ;
        int n = 0;
        int i = 0;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
            function += s[i];
            i++;
        }

        if( function == "return" ){
            f_command cmd(function , "" , 0);
            return cmd;
        }

        i++;
        while( i < s.size() && s[i] != ' ' && s[i] != '\0' ){
            name += s[i];
            i++;
        }
        i++;
        while( i < s.size() && s[i] != '\0' ){
            num += s[i];
            i++;
        }
        n = stoi(num);
        f_command cmd(function , name , n);

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
            else if(VMcode[i]== "gt"){
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
                
                string next_true = "@GT.true.";
                next_true += to_string(nGT);
                
                implementation.push_back(next_true);
                implementation.push_back("D;JGT");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                string end = "@GT.end.";
                end += to_string(nGT);
                
                implementation.push_back(end);
                implementation.push_back("0;JMP");
                string next = "(GT.true." + to_string(nGT);
                next += ")";

                implementation.push_back(next);
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                string end_label = "(GT.end." + to_string(nGT);
                end_label += ")";
                implementation.push_back(end_label);
                nGT++;
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i]== "eq"){
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
                
                string next_true = "@EQ.true.";
                next_true += to_string(nEQ);
                
                implementation.push_back(next_true);
                implementation.push_back("D;JEQ");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                string end = "@EQ.end.";
                end += to_string(nEQ);
                
                implementation.push_back(end);
                implementation.push_back("0;JMP");
                string next = "(EQ.true." + to_string(nEQ);
                next += ")";

                implementation.push_back(next);
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                string end_label = "(EQ.end." + to_string(nEQ);
                end_label += ")";
                implementation.push_back(end_label);
                nEQ++;
                implementation.push_back("@SP");
                implementation.push_back("M=M+1");
            }
            else if(VMcode[i]== "lt"){
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
                
                string next_true = "@LT.true.";
                next_true += to_string(nLT);
                
                implementation.push_back(next_true);
                implementation.push_back("D;JLT");
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=0");
                string end = "@LT.end.";
                end += to_string(nLT);
                
                implementation.push_back(end);
                implementation.push_back("0;JMP");
                string next = "(LT.true." + to_string(nLT);
                next += ")";

                implementation.push_back(next);
                implementation.push_back("@SP");
                implementation.push_back("A=M");
                implementation.push_back("M=-1");
                string end_label = "(LT.end." + to_string(nLT);
                end_label += ")";
                implementation.push_back(end_label);
                nLT++;
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
            else if( is_branching(VMcode[i]) ){
                b_command parsed = parse_branching(VMcode[i]);
                string control = parsed.control;
                string label = parsed.label;

                if( control == "label" ){
                    string code ;
                    code = "(" + label + ")";
                    implementation.push_back(code);
                }
                else if(control == "goto"){
                    string code ;
                    code = "@" + label ;
                    implementation.push_back(code);
                    implementation.push_back("0;JMP");
                }
                else if(control == "if-goto"){
                    implementation.push_back("@SP");
                    implementation.push_back("AM=M-1");
                    implementation.push_back("D=M");
                    implementation.push_back("A=A-1");
                    string code ;
                    code = "@" + label ;
                    implementation.push_back(code);
                    implementation.push_back("D;JNE");
                }

            }

            else if( is_function( VMcode[i] ) ){
                f_command parsed = parse_function( VMcode[i] );
                string function = parsed.function;
                string name ;
                int n ;
                string num;
                name = parsed.name;
                n = parsed.n;
                num = to_string(n); // number of arguments of the collee

                if(function == "function"){
		            // Make a label that's going to serve as the entry point to the code of this function (here the function starts its execution)
                    string label = "(" + name + ")";
                    implementation.push_back(label);                    

                    // push 0 ( Initialize local variables with value 0 )
                    for(int j = 0 ; j < n ; j++){
                        implementation.push_back("@SP");
                        implementation.push_back("A=M");
                        implementation.push_back("M=0");
                        implementation.push_back("@SP");
                        implementation.push_back("M=M+1"); 
                    }

                }
                else if(function == "call"){
                    // Push the return address onto the stack (we use this return address to determine where to return after the callee terminates its execution)
                    string return_address = "return" ;
                    return_address += to_string(nReturn);
                    nReturn++;
                    vector<string> push_return ;
                    string temp = "@" + return_address;
                    push_return.push_back(temp);
                    push_return.push_back("D=A");
                    push_return.push_back("@SP");
                    push_return.push_back("A=M");
                    push_return.push_back("M=D");
                    push_return.push_back("@SP");
                    push_return.push_back("M=M+1");
                    implementation.insert(implementation.end() , push_return.begin() , push_return.end() );

		            // Push the caller's state by pushing the LCL, ARG, THIS, THAT segments 
                    vector<string> push_LCL = push("LCL");
                    vector<string> push_ARG = push("ARG");
                    vector<string> push_THIS = push("THIS");
                    vector<string> push_THAT = push("THAT");
                    implementation.insert(implementation.end() , push_LCL.begin() , push_LCL.end() );
                    implementation.insert(implementation.end() , push_ARG.begin() , push_ARG.end() );
                    implementation.insert(implementation.end() , push_THIS.begin() , push_THIS.end() );
                    implementation.insert(implementation.end() , push_THAT.begin() , push_THAT.end() );

                    /* Repositioning the ARG segment to point to the base address of the arguments that are going to be used by the collee
		            And we can do that easily cuz we now how many things we pushed onto the stack : we pushed 5 values ( the return address, and the caller's state LCL, ARG, THIS, THAT )
		            So new ARG will be positioned following this equation ARG = SP - 5 - the number of arguments.
			        Note !! : if the function has 0 argument then ARG will point to (SP - 5) which is the address of the cell holding the return address */
                    implementation.push_back("@5");
                    implementation.push_back("D=A");
                    string line = "@" + num;
                    implementation.push_back(line);
                    implementation.push_back("D=D+A");
                    implementation.push_back("@SP");
                    implementation.push_back("D=M-D");
                    implementation.push_back("@ARG");
                    implementation.push_back("M=D");

		            // Here we'll set the new LCL the one of the collee (the collee's LCL) starts here
                    implementation.push_back("@SP");
                    implementation.push_back("D=M");
                    implementation.push_back("@LCL");
                    implementation.push_back("M=D");

                    // goto function
                    string code = "@" + name ;
                    implementation.push_back(code);
                    implementation.push_back("0;JMP");

		            // We return here after the collee terminates its execution
                    code = "(" + return_address + ")";
                    implementation.push_back(code);

                }
                else if(function == "return"){
                    /* The return address is stored in the cell that precedes the collee's LCL by 5 places (these places are preserved for the caller's state) 
                    so the return address is the content of cell of address (LCL - 5) so return_address = *(LCL - 5) 
                    -- Here we saved the return address in the register R15 --
                    */
                    implementation.push_back("@LCL");
                    implementation.push_back("D=M");
                    implementation.push_back("@R14");
                    implementation.push_back("M=D"); // Save the value of the current LCL in R14 (in order to avoid problems when we want to modify LCL when recovering the caller's state)

                    implementation.push_back("@R14");
                    implementation.push_back("D=M");
                    implementation.push_back("@5");
                    implementation.push_back("A=D-A");
                    implementation.push_back("D=M"); // D-register holds the content of the cell with address (LCL-5), so D = *(LCL-5) --> D holds the return address
                    implementation.push_back("@15"); // R15 holds the return address
		            implementation.push_back("M=D");
                    /* We copied the return address into R15 because if the function has no arguments the next command which pops the return value and copies it onto ARG will override the return address
                    because if the function has 0 arguments ARG will point to the cell that holds the return address and the next command will modify the content of ARG and set it to the returned value
                    */
                    

                    // Pop the return value and copy it onto ARG (the base address of the arguments of the collee) so ARG will be holding the return value which is by the way the topmost value in the stack
                    implementation.push_back("@SP");
                    implementation.push_back("A=M-1");
                    implementation.push_back("D=M");
                    implementation.push_back("@ARG");
                    implementation.push_back("A=M");
                    implementation.push_back("M=D");

		            // Repositioning SP to be just after ARG so  SP = ARG + 1
                    implementation.push_back("@ARG");
                    implementation.push_back("D=M+1");
                    implementation.push_back("@SP");
                    implementation.push_back("M=D");

		            // Restoring the callers state which resides just before the callee's LCL, and as we've already copied the content of LCL onto R14 we'll use R14 to recover the caller's state

		            // THAT = *(R14 - 1)
                    implementation.push_back("@R14");
                    implementation.push_back("A=M-1");
                    implementation.push_back("D=M");
                    implementation.push_back("@THAT");
                    implementation.push_back("M=D");
		            // THIS = *(R14 - 2)
                    implementation.push_back("@R14");
                    implementation.push_back("D=M");
                    implementation.push_back("@2");
                    implementation.push_back("A=D-A");
                    implementation.push_back("D=M");
                    implementation.push_back("@THIS");
                    implementation.push_back("M=D");
		            // ARG = *(R14 - 3)
                    implementation.push_back("@R14");
                    implementation.push_back("D=M");
                    implementation.push_back("@3");
                    implementation.push_back("A=D-A");
                    implementation.push_back("D=M");
                    implementation.push_back("@ARG");
                    implementation.push_back("M=D");
		            // LCL = *(R14 - 4)
                    implementation.push_back("@R14");
                    implementation.push_back("D=M");
                    implementation.push_back("@4");
                    implementation.push_back("A=D-A");
                    implementation.push_back("D=M");
                    implementation.push_back("@LCL");
                    implementation.push_back("M=D");
		    
		            // goto return address (which we already stored in R15)
                    implementation.push_back("@R15");
                    implementation.push_back("A=M");
                    implementation.push_back("0;JMP");

                }

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
                        implementation = push_static( n , i);
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
                        implementation = pop_static( n , i);
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
            if( ( ( j + 1 < (int)assembly[i].size() ) && assembly[i][j] == '/' && assembly[i][j+1] == '/' ) || int(assembly[i][j]) == 13  ){
                break;
            }
            else{
                line += assembly[i][j];
            }
        }
        if( !line.empty() && line != " " ){
            while( line.back() == ' ' || int( line.back() ) == 13){
                line.pop_back();
            }
            handled.push_back(line);
        }
    }
    
    return handled;
}

void handle_file(string directory , string file , map<int,string> m){
    
    vector<string> inputCode , VMcode;
    string full_path = directory + file + ".vm";
    const char* c_full_path = full_path.c_str();
    fstream VMfile;
    VMfile.open(c_full_path);
    //cout << full_path << endl;
    string line;

    while( VMfile ){
        std::getline( VMfile , line );
        if( !line.empty() ){
            inputCode.push_back(line);
        }
        
    }
    
    
    vector<string> temp = handle_white_spaces(inputCode);
    
    VMcode.push_back("call Sys.init 0");

    VMcode.insert(VMcode.end() , temp.begin() , temp.end());

    VMImplementation VM(VMcode , file , m);
    
    vector<string> machine_code = VM.machine_code();
    
    string outdirectory = directory + file + ".asm";
    
    ofstream VMI( outdirectory );
    
    vector<string> Bootstrap;

    Bootstrap.push_back("@256");
    Bootstrap.push_back("D=A");
    Bootstrap.push_back("@SP");
    Bootstrap.push_back("M=D");

    for(auto s : Bootstrap){
        VMI << s << "\n";
    }

    for(auto s : machine_code){
        VMI << s << "\n";
    }

    VMI.close();

}

void handle_file_(string directory , string file , map<int,string> m){
    
    vector<string> inputCode , VMcode;
    string full_path = directory + file + ".vm";
    cout << "handle file : " << full_path << endl;
    const char* c_full_path = full_path.c_str();

    fstream VMfile;
    VMfile.open(c_full_path);
    string line;

    while( VMfile ){
        std::getline( VMfile , line );
        if( !line.empty() ){
            inputCode.push_back(line);
        }
        
    }
    
    vector<string> temp = handle_white_spaces(inputCode);
    
    VMcode.insert(VMcode.end() , temp.begin() , temp.end());

    VMImplementation VM(VMcode , file , m);
    
    vector<string> machine_code = VM.machine_code();
    
    string outdirectory = directory + file + ".asm";
    cout << "file handled :  " << outdirectory << endl;
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

void copy_file( string directory , string origin , string new_ ){
    string line;
    //For writing text file
    //Creating ofstream & ifstream class object
    string old_file = directory + origin + ".asm";
    string new_file = directory + new_ + ".asm";
    ifstream ini_file {old_file};
    ofstream out_file {new_file};
 
    if(ini_file && out_file){
 
        while(getline(ini_file,line)){
            out_file << line << "\n";
        }
  
    } else {
        //Something went wrong
        printf("Cannot read File");
    }
 
    //Closing file
    ini_file.close();
    out_file.close();
}

void append(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

int main(int argc, char *argv[])
{

    char* path ;
    path =  argv[1];
    if(path[strlen(path)-1] != '/'){
        append(path , '/');
    }
    string file = path;
    if( file[file.size()-2] == 'm' && file[file.size()-3] == 'v'){
        file.pop_back();
    }
    string file_name;
    vector<string> files ;
    map<int,string> m;
    
    cout << "file is : "<< file << endl;

    if( is_directory(path) ){
        get_vm_files(path , files);
    }
    /*
    for(auto s : files){
        cout << "vm file -->  " << s << endl;
    }
    */
    string directory = file;

    int i = file.size() - 1;
    

    if( file[i] == '/' ){
        i--;
    }
    while( file[i] != '/' && i > -1){
        file_name += file[i];
        i--;
    }

    reverse(file_name.begin() , file_name.end());  
    
    string outdirectory = directory ;

    if( outdirectory.back() != '/' ){
        outdirectory += '/';
    }

    outdirectory += file_name + ".vm";

    

    ofstream VMI;

    if( !is_vm_file(file) && is_directory(path) && files.size() > 1){
        cout << outdirectory << endl;
        VMI.open( outdirectory );
    }

    // cout << " --------------------------------- " << endl;

    vector<string> inputCode;


    if( !is_vm_file(file) && is_directory(path) && files.size() > 1){
        int l = 0;
        for(auto f : files){
            directory = f; 
            ifstream VMfile;
            VMfile.open( directory );
            string line;
            while( std::getline( VMfile , line ) ){
                if( !line.empty() ){
                    VMI << line << "\n";
                    inputCode.push_back(line);

                    if( line[0] != '/' && line[1] != '/' && int(line[0]) != 13 ){
                        l++;
                    }
                }
            }
            pair<string,string> p = get_file(directory);
            
            m[l] = p.second;
            VMfile.close();
        }
    }

    VMI.close();


    if( !outdirectory.empty() && is_directory(path) ){

        pair<string,string> p = get_file(outdirectory);
        
        if( p.second != "Invalid file!" && files.size() > 1 ){
            handle_file(p.first , p.second , m);
        }
        else if( p.second != "Invalid file!" && files.size() > 0){
            pair<string,string> p = get_file(files[0]);
            cout << p.first << " " << p.second << " " << file_name << endl;
            cout << p.first <<"/" << p.second << ".vm" << endl;
            cout << p.first <<"/" << file_name << ".vm" << endl;
            map<int,string> m_;
            int o = 10000000;
            m_[o] = file_name;
            handle_file_(p.first,p.second,m_);
            if(p.second != file_name){
                copy_file( p.first , p.second , file_name );
            }
        }
    }
    else if( is_vm_file(file) ){
        pair<string,string> p = get_file(file);
        map<int,string> m_;
        int o = 10000000;
        m_[o] = p.second;
        
        if( p.second != "Invalid file!" ){
            handle_file_(p.first , p.second , m_);
        }
        
    }

    return 0;
}