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
    return S_ISDIR(path_stat.st_mode);
}

void handle( const char *path ){
    struct dirent *d;
    DIR *dr = NULL;
    dr = opendir(path);
    string dir_name = path;
    string file_name;
    vector<string> files ;
    if(dr != NULL){

        //cout<<"List of Files & Folders:-\n";
        for( d = readdir(dr) ; d != NULL ; d = readdir(dr) ){
            if( is_directory( d->d_name ) ){
                continue;

            }else{
                
                //handle( d->d_name );
                file_name = d->d_name;
                string full = dir_name + file_name;
                files.push_back(full);
                //cout<<full<<endl;
            }
        }
        closedir(dr);
    }
    else
        cout << "\nError Occurred!";
    cout<<endl;

    cout << " files : " << endl;
    for(auto f : files){
        cout << f << endl;
    }
    cout << endl;
    
}

int main(int argc, char *argv[])
{

    char* path ;
    path =  argv[1];
    string path_str = path;
    cout << path_str << endl;

    /*
    int n = path_str.size();
    if( is_directory(path) ){
        handle(path);
    }
    else if( path_str[n-1] == 'm' && path_str[n-2] == 'v' && path_str[n-3] == '.'){
        cout << " Vm file !" << endl;
        //handle_file(path_str);
    }

    */
    
    return 0;
}