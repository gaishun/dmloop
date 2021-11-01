#include <string>
#include <sys/fcntl.h>
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

using namespace std;

string a(40960,'0');

int main (){

    int fd = open("/mnt/test/pre_hole_file",O_RDWR | O_SYNC);
    if(fd<0){
        cout<<"open err"<<endl;
        return -1;
    }

    lseek(fd,23452435,SEEK_SET);
    int ans = write(fd,a.c_str(),sizeof(a));
    if(ans<0){
        cout<<"write err"<<endl;
        return -1;
    }
    close(fd);
    return 0;
}