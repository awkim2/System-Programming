/**
 * Finding Filesystems
 * CS 241 - Fall 2019
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
//#define MAX( a, b ) ( ( a > b ) ? a : b )
//#define MIN( a, b ) ( ( a < b ) ? a : b )
/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode *node = get_inode(fs, path);
    //node dont exist
    if (!node) {
        errno = ENOENT;
        return -1;
    }
    uint16_t high_bit = node->mode >> RWX_BITS_NUMBER;
    node->mode = new_permissions | (high_bit << RWX_BITS_NUMBER);

    //update meta time
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    inode *node = get_inode(fs, path);
    //node dont  exit
    if (!node) {
        errno = ENOENT; 
        return -1;
    }
    //change user
    if (owner != ((uid_t)-1)){
        node->uid = owner;
    }
    //change group
    if (group != ((gid_t)-1)){
        node->gid = group;
    }
     //update meta time
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

void* get_block(file_system* fs, inode* parent, uint64_t index){
  data_block_number* target;
  if(index < NUM_DIRECT_BLOCKS){
    target = parent->direct;
  } else {
    target = (data_block_number*)(fs->data_root + parent->indirect);
    index -= NUM_DIRECT_BLOCKS;
  }
  return (void*) (fs->data_root + target[index]);
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    //path not valid
    if(!valid_filename(path)){
        return NULL;
    }
    inode *node = get_inode(fs, path);
    //node already exist
    if(node){
        clock_gettime(CLOCK_REALTIME, &(node->ctim));
        return NULL;
    }
    const char *filename = NULL;
    inode *parent_node = parent_directory(fs, path, &filename);
    //parent dont exist or not a directory
    if (!parent_node || !is_directory(parent_node)){
        return NULL;
    }
    // no more node is fs
    if (first_unused_inode(fs) == -1){
        return NULL;
    }
    inode *new_node = fs->inode_root + first_unused_inode(fs);
    init_inode(parent_node, new_node);
    //create new dirent
    minixfs_dirent dirent_;
    dirent_.name = (char*) filename;
    dirent_.inode_num = first_unused_inode(fs);
    int index = parent_node->size / sizeof(data_block);
    if (index >= NUM_DIRECT_BLOCKS){
        return NULL;
    }
    int offset = parent_node->size % sizeof(data_block);
    if (!offset && add_data_block_to_inode(fs, parent_node) == -1) {
        return NULL;
    }
    void *block_start = get_block(fs, parent_node, index) + offset;
    memset(block_start, 0, sizeof(data_block));
    make_string_from_dirent(block_start, dirent_);
    //update parent size
    parent_node->size += MAX_DIR_NAME_LEN;
    clock_gettime(CLOCK_REALTIME, &parent_node->mtim);
    return new_node;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        ssize_t used = 0;
        uint64_t i = 0;
        char *map = GET_DATA_MAP(fs->meta);
        for (; i < fs->meta->dblock_count; i++) {
            if (map[i] == 1) used++;
        }
        //get info string
        char *info_str = block_info_string(used);
        size_t len = strlen(info_str);
        if (*off > (int)len) return 0;
        if (count > len - *off) count = len - *off;
        memmove(buf, info_str + *off, count);
        *off += count;
        return count;
    }
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    uint64_t max = sizeof(data_block) * (NUM_DIRECT_BLOCKS + NUM_INDIRECT_BLOCKS);
    //count exceeds max capicity
    if (count + *off > max) {
        errno = ENOSPC;
        return -1;
    }
    //exceed limit
    int block_count = (count + *off + sizeof(data_block) - 1) / sizeof(data_block);
    if (minixfs_min_blockcount(fs, path, block_count) == -1) {
        errno = ENOSPC;
        return -1;
    }
    inode *target_file = get_inode(fs, path);
    uint64_t index = *off / sizeof(data_block);
    size_t offset = *off % sizeof(data_block);
    //uint64_t size = MIN(count, sizeof(data_block) - offset);
    uint64_t size = 0;
    if(count > sizeof(data_block) - offset){
        size = sizeof(data_block) - offset;
    }else{
        size = count;
    }
    void *mem_block = get_block(fs, target_file, index) + offset;
    memcpy(mem_block, buf, size);
    uint64_t w_count = size;
    *off += size;
    index ++;
    while (w_count < count) {
        //size = MIN(count - w_count, sizeof(data_block));
        if(count - w_count > sizeof(data_block)){
            size = sizeof(data_block);
        }else{
            size = count - w_count;
        }
        mem_block = get_block(fs, target_file, index);
        memcpy(mem_block, buf + w_count, size);
        w_count += size;
        *off += size;
        index ++;
    }
    //target_file->size = MAX(count + *off, target_file->size);
    if(count + *off > target_file->size){
        target_file->size  = count + *off;
    }
    //update atim and mtim
    clock_gettime(CLOCK_REALTIME, &target_file->atim);
    clock_gettime(CLOCK_REALTIME, &target_file->mtim);
    return w_count;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    //if it is virtual, using virtual read
    if (virtual_path) return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    //buffer is NULL 
    if(!buf){
      errno = ENOENT;
      return -1;
    }
    inode* target_file = get_inode(fs, path);
    //cannot find inode
    if(!target_file){
      errno = ENOENT;
      return -1;
    }

    if ((uint64_t)*off > target_file->size) return 0;
    if(target_file->size - *off < count){
        count = target_file->size - *off;
    }
    //count = MIN(count, target_file->size - *off);
    uint64_t index = *off / sizeof(data_block);
    size_t offset = *off % sizeof(data_block);
    //uint64_t size = MIN(count, sizeof(data_block) - offset);
    uint64_t size = 0;
    if(count > sizeof(data_block) - offset){
        size =  sizeof(data_block) - offset;
    }else{
        size = count;
    }
    void *mem_block = get_block(fs, target_file, index) + offset;
    memcpy(buf, mem_block, size);
    uint64_t r_count = size;
    *off += size;
    index++;
    while (r_count < count) {
        //size = MIN(count - r_count, sizeof(data_block));
        if(count - r_count > sizeof(data_block)){
            size = sizeof(data_block);
        }else{
            size = count - r_count;
        }
        mem_block = get_block(fs, target_file, index);
        memcpy(buf + r_count, mem_block, size);
        r_count += size;
        *off += size;
        index++;
    }
    //update atim
    clock_gettime(CLOCK_REALTIME, &target_file->atim);
    return r_count;
}
