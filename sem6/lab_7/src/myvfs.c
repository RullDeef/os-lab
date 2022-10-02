#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>

MODULE_AUTHOR("Klimenko Alexey");
MODULE_LICENSE("GPL");

#define FS_MAGIC            0x12341234
#define ROOT_INODE_PERM     (S_IRWXU | S_IRWXG | S_IRWXO)
#define CACHE_NAME          "myvfs_cache"

static struct kmem_cache *slab_cache;

struct dentry *myvfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data);
int myvfs_fill_super(struct super_block *sb, void *data, int silent);
void myvfs_kill_sb(struct super_block *sb);
struct inode *myvfs_alloc_inode(struct super_block *sb);
int myvfs_delete_inode(struct inode *inode);

static struct file_system_type myvfs_type = {
    .name = "myvfs",
    .owner = THIS_MODULE,
    .mount = myvfs_mount,
    .kill_sb = myvfs_kill_sb,
};

static struct super_operations myvfs_sops = {
    .statfs = simple_statfs,
    .alloc_inode = myvfs_alloc_inode,
    .drop_inode = myvfs_delete_inode
};

struct dentry *myvfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
    printk(KERN_INFO "myvfs: myvfs_mount\n");
    return mount_nodev(fs_type, flags, data, myvfs_fill_super);
}

int myvfs_fill_super(struct super_block* sb, void *data, int silent)
{
    struct inode *root;

    printk(KERN_INFO "myvfs: myvfs_fill_super\n");

    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = FS_MAGIC;
    sb->s_op = &myvfs_sops;

    // root node creation
    root = new_inode(sb);
    if (!root)
    {
        printk(KERN_ERR "myvfs: root inode is null!\n");
        return -ENOMEM;
    }

    root->i_ino = 1;
    root->i_uid.val = 0U;
    root->i_gid.val = 0U;
    root->i_mode = S_IFDIR | ROOT_INODE_PERM;
    root->i_blocks = 0;
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    // root dentry creation
    sb->s_root = d_make_root(root);
    if (!sb->s_root)
    {
        printk(KERN_ERR "myvfs: root dentry is null!\n");
        return -ENOMEM;
    }

    return 0;
}

void myvfs_kill_sb(struct super_block *sb)
{
    printk(KERN_INFO "myvfs: kill sb\n");

    if (sb->s_root)
        d_genocide(sb->s_root);
    // printk(KERN_INFO "myvfs: sb genocide successful\n");

    myvfs_delete_inode(sb->s_root->d_inode);
    // printk(KERN_INFO "myvfs: sb killed successfully\n");
}

struct inode *myvfs_alloc_inode(struct super_block *sb)
{
    struct inode *inode;

    printk(KERN_INFO "myvfs: alloc inode\n");
    inode = kmem_cache_alloc(slab_cache, GFP_KERNEL);
    if (inode == NULL)
        printk(KERN_ERR "myvfs: kmem_cache_alloc failed!\n");

    return inode;
}

int myvfs_delete_inode(struct inode *inode)
{
    printk(KERN_INFO "myvfs: delete inode with ino=%lu\n", inode->i_ino);
    kmem_cache_free(slab_cache, inode);
    printk(KERN_INFO "myvfs: freed from cache successfully\n");

    return 1;
}

void slab_ctor(void *obj)
{
    printk(KERN_INFO "myvfs: kmem ctor used\n");
}

static int __init myvfs_init(void)
{
    int status;

    printk(KERN_INFO "myvfs: init\n");

    slab_cache = kmem_cache_create(CACHE_NAME, sizeof(struct inode), 0, 0, slab_ctor);
    if (slab_cache == NULL)
    {
        printk(KERN_ERR "myvfs: slab cache was not created\n");
        return -ENOMEM;
    }

    status = register_filesystem(&myvfs_type);
    if (status != 0)
        printk(KERN_ERR "myvfs: filesystem was not registered\n");

    return status;
}

static void __exit myvfs_exit(void)
{
    printk(KERN_INFO "myfvs: exit\n");
    if (unregister_filesystem(&myvfs_type) != 0)
        printk(KERN_ERR "myvfs: could not unregister filesystem\n");

    kmem_cache_destroy(slab_cache);
}

module_init(myvfs_init);
module_exit(myvfs_exit);
