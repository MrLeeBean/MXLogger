//
//  file_helper.cpp
//  Logger
//
//  Created by 董家祎 on 2022/3/4.
//

#include "mx_file.hpp"
#include <sys/stat.h>
#include <thread>
#include <dirent.h>
#include <vector>
#include <stdio.h>
#include "mxlogger_helper.hpp"
#include <map>
#ifdef  __ANDROID__
#include <android/log.h>
#endif

namespace mxlogger{
namespace details{

mx_file::mx_file() : max_disk_size_(0),max_disk_age_(0){

}

mx_file::~mx_file(){
    printf("mx_file 释放\n");
    close();
}


void mx_file::set_max_disk_age(long long max_age){
    max_disk_age_ = max_age;
}


void mx_file::set_max_disk_size(long long max_size){
    max_disk_size_ = max_size;
}

void mx_file::open(){
    close();
    
    std::string mode = "ab";

    for (int tries = 0; tries < open_tries_; tries ++ ) {
        create_dir_(dir_);
        
        auto filepath = dir_ + filename_;
      
       fd_ =  fopen(filepath.c_str(), mode.c_str());
        if (fd_ != nullptr) {
          
            if (header_buffer_.data() != nullptr && strcmp(header_buffer_.data(), "")!=0) {
                struct stat statbuf;
                
                lstat(filepath.c_str(), &statbuf);
                // 保证只在首次创建文件的时候写入
                if (statbuf.st_size == 0) {
                    write(header_buffer_, filename_);
                }
                
                
            }
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string &mx_file::filename() {
    return filename_ ;
}

void mx_file::flush(){
    std::fflush(fd_);
}
void mx_file::close(){
    if (fd_ != nullptr) {
        std::fclose(fd_);
        fd_ = nullptr;
    }
}
long  mx_file::file_size() const{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    long   current_cache_size=0;
    if ((dir = opendir(dir_.c_str())) == nullptr){
        
        fprintf(stderr, "Cannot open dir: %s\n", dir_.c_str());
        return 0;
    }
    
    while ((entry = readdir(dir)) != nullptr) {
    
        if (strcmp(".DS_Store", entry->d_name) == 0 || strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
            continue;
        }
        char subdir[256];
        sprintf(subdir, "%s%s", dir_.c_str(), entry->d_name);
        
        lstat(subdir, &statbuf);
        
        current_cache_size = current_cache_size + statbuf.st_size;
    }
    closedir(dir);
    
    return  current_cache_size;
    
  
}
void mx_file::remove_all(){
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(dir_.c_str())) == nullptr){
        
        fprintf(stderr, "Cannot open dir: %s\n", dir_.c_str());
        return;
    }
    
    while ((entry = readdir(dir)) != nullptr) {
    
        if (strcmp(".DS_Store", entry->d_name) == 0 || strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
            continue;
        }
        
        char subdir[256];
        sprintf(subdir, "%s%s", dir_.c_str(), entry->d_name);
        remove(subdir);
    }
    
    closedir(dir);
}

void mx_file::remove_expire_data(){
   

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    long long  current_cache_size=0;
  
    std::vector<char*> urls_delete;
  

    std::map<long long ,long long ,std::less<long long >>  cache_size_files_;
    
    std::map<long long ,char*,std::less<long long >>  cache_name_files_;
    
    std::tm tm_time = mxlogger_helper::now();
    
    long long int timestamp =  mktime(&tm_time);
    long long int expiration_tp = timestamp - max_disk_age_;
    
    
    if ((dir = opendir(dir_.c_str())) == nullptr){
        
        fprintf(stderr, "Cannot open dir: %s\n", dir_.c_str());
        return;
    }
 
  
    while ((entry = readdir(dir)) != nullptr && max_disk_age_ > 0) {
        char subdir[256];
       
        sprintf(subdir, "%s%s", dir_.c_str(), entry->d_name);
        
        lstat(subdir, &statbuf);
       
        if (strcmp(".DS_Store", entry->d_name) == 0 || strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
            continue;
        }else{
         
            time_t last_time = statbuf.st_ctime;
            if (last_time < expiration_tp) {
                /// 过期文件
                urls_delete.push_back(subdir);
                continue;
            }
        
            current_cache_size = current_cache_size + statbuf.st_size;
            
            cache_size_files_[last_time] = statbuf.st_size;
            cache_name_files_[last_time] = entry->d_name;
           
        }
    }
    
    
    if (max_disk_size_ > 0 && current_cache_size > max_disk_size_) {
        
        for (auto &key : cache_size_files_) {
            
            char * file_name = cache_name_files_[key.first];
            
            long long  file_size = key.second;
            
            char delete_path[256];
           
            sprintf(delete_path, "%s%s", dir_.c_str(), file_name);
       
            if (remove(delete_path) == 0) {
                current_cache_size = current_cache_size - file_size;
           
                if (max_disk_size_ >= current_cache_size) {
                    break;
                }
            }
            
        }
        
    }
    
    closedir(dir);
    
    
}


void mx_file::write(const std::string &buf,const std::string &fname){
    
    if (fd_ == nullptr || filename_.compare(fname) != 0 || path_exists(dir_ + filename_) == false) {
        filename_ = fname;
        open();
    }
    
    size_t msg_size = buf.size();
    auto data = buf.data();

    if( std::fwrite(data, 1, msg_size, fd_) != msg_size){
#ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_ERROR, "file_helper::fwrite", "%s", "文件写入异常");
#elif __APPLE__
        printf("文件写入异常");
#endif

    }
}
void mx_file::set_header(std::string header){
    header_buffer_ = std::move(header) + "\n";
    
}
void mx_file::set_dir(const std::string dir){
    dir_ = dir;
    create_dir_(dir);
}
bool mx_file::create_dir_(const std::string &path){
    
    auto pos = path.find_last_of("/");

    std::string dir_name = pos != std::string::npos ? path.substr(0,pos) : std::string{};
    
    
    if (path_exists(dir_name))  return  true;
    
    if (dir_name.empty())  return  false;
    
    size_t search_offset = 0;
    do {
       auto token_pos =  dir_name.find_first_of("/",search_offset);
        if (token_pos == std::string::npos) {
            token_pos = dir_name.size();
        }
        auto subdir = dir_name.substr(0,token_pos);
        if (!subdir.empty() && !path_exists(subdir) && makedir_(subdir)!= 0) {
            
            return  false;
            
        }
        search_offset = token_pos + 1;
        
    } while (search_offset < dir_name.size());
    
    
    return true;
}

bool mx_file::path_exists(const std::string &path){
    struct stat buffer;
    return (::stat(path.c_str(), &buffer) == 0);
}
bool mx_file::makedir_(const std::string &path){
    
    return ::mkdir(path.c_str(),mode_t(0755)) == 0;
}


}
}
