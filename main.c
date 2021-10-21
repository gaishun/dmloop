#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>


#define OUT_OF_MEMORY   3

/***
 *
 * @param fie               saved metadata of file
 * @param source_device     same to func dmloop
 * @param target_device     same to func dmloop
 * @param _sector_size      same to func dmloop, need to transfer int int
 * @return
 */
int execute_cmd (struct fiemap * fie,char* source_device, char* target_device, char* _sector_size){
    /**transfer char* to int*/
    __u32 sector_size = atoi(_sector_size);

    /** 记录几组offset和length */
    __u64 disk_offset[fie->fm_extent_count];
    __u64 portion_length[fie->fm_extent_count];
    __u32 i;//index
    for(i=0;i<fie->fm_extent_count;i++){
        disk_offset[i]=fie->fm_extents[i].fe_physical;
        portion_length[i]=fie->fm_extents[i].fe_length;
        printf("%llu,%llu\n",disk_offset[i],portion_length[i]);
    }


    /**不足条件：
     * 一部分长度不超过100字符串长度**/
    char* cmd = NULL;
    if((cmd = (char *)malloc(50*sizeof(char)))==NULL){
        fprintf(stderr,"Out of Memory in malloc space for cmd");
        return OUT_OF_MEMORY;
    }
    memset(cmd,0,sizeof(char)*50);
    char temp[100];

    sprintf(cmd,"dmsetup create %s --table \"",target_device);
    __u64 log_offset; /** source dev logical offset*/
    for(i=0,log_offset=0;i<fie->fm_extent_count;i++){
        //    sprintf(temp,"%llu %llu %s %s %llu\n",logical_start,length,tardev_type,dev,source_start);
        sprintf(temp," %llu %llu linear %s %llu\n",log_offset/sector_size,portion_length[i]/sector_size,
                source_device,disk_offset[i]/sector_size);
        log_offset+=portion_length[i];
//        printf("%s\n",temp);

        if((cmd = (char*)realloc(cmd, strlen(cmd)+sizeof(char)* strlen(temp)))==NULL){
            fprintf(stderr,"Out of Memory in realloc space for cmd");
            return OUT_OF_MEMORY;
        }
        strcat(cmd,temp);
//        printf("%s\n",cmd);
    }
    strcat(cmd,"\"");
    printf("%s\n",cmd);
    system(cmd);
    return 0;

}


/**
 *
 * @param file_path         file which need to be mapped
 * @param source_device     which device is the file saved
 * @param target_device     device name file mapped; saved in /dev/mapper/
 * @param sector_size       sector size is 512 B in common
 * @return
 */
int dmloop (char* file_path,char* source_device, char* target_device, char* sector_size){
    //define the file descriptor
    int input_fd = open(file_path, O_RDONLY);
    if(input_fd < 0 ){
        printf("open %s error \n",file_path);
        return 2;
    }


    struct fiemap *fie = NULL;
    if ((fie = (struct  fiemap*) malloc(sizeof(struct fiemap))) == NULL){
        fprintf(stderr, "Out of Memory allocating fiemap\n");
        return OUT_OF_MEMORY;
    }

    /** used to realloc space for struct fiemap_extents */
    __u32 extents_size = 0;

    memset(fie,0,sizeof(struct fiemap));
    /** excepted length of this file,
     * could be replaced by file_size in bytes,
     * but it also could be the max value*/
    fie->fm_length  = ~0;

    /** get the mapped extents*/
    if (ioctl(input_fd, FS_IOC_FIEMAP, fie) < 0){
        fprintf(stderr, "fiemap ioctl() failed \n");
        return 4;
    }

    /** realloc space for extents*/
    fie->fm_extent_count = fie->fm_mapped_extents;
    extents_size = sizeof(struct fiemap_extent)*(fie->fm_extent_count);
    if((fie = (struct fiemap*) realloc(fie,sizeof(struct fiemap)+extents_size))==NULL){
        fprintf(stderr, "Out of Memory allocating fiemap2\n");
        return OUT_OF_MEMORY;
    }
    /** init the array fm_extents*/
    memset(fie->fm_extents,0,sizeof(extents_size));

    /** get the information of the extents*/
    /** revoking twice ioctl to alloc enough space for array fm_extents */
    if (ioctl(input_fd, FS_IOC_FIEMAP, fie) < 0){
        fprintf(stderr, "fiemap ioctl() failed2 \n");
        return 4;
    }
//    __u64 file_size = lseek(input_fd,0,SEEK_END);
//    printf("file_size is %lluB\n",file_size);
    close(input_fd);

    int ret = execute_cmd(fie,source_device,target_device,sector_size);
    free(fie);
    return ret;

}





int main(int argc, char* argv[]) {
    if(argc != 5 ){
        printf("Usage: file_path source_device target_device_name sector_size\n");
        return 1;
    }
    return dmloop(argv[1],argv[2],argv[3],argv[4]);

//    fie->fm_start   = 0;  file logical start
//    fie->fm_flags   = 0;  file_flags
//    fie->fm_extent_count = 0; the capacity of fm_extents
//    fie->fm_mapped_extents = 0;   the number of mapped extents, fm_extent_count should be equal to this para.

    return 0;
}