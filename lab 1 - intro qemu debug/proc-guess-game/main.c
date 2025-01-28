#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#include <linux/random.h>
#include <linux/kstrtox.h>

enum state
{
    BEGIN = 0,
    LOH = 1,
    GOOD = 2,
};

static unsigned int current_number = 0;
enum state current_state = BEGIN;

static ssize_t guess_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{

    unsigned int user_number = 0;
    int rc = kstrtouint_from_user(ubuf, count, 10, &user_number);
    if (rc != 0)
        return rc;

    if (user_number == current_number)
    {
        current_state = GOOD;
        current_number = get_random_u32();
    }
    else
    {
        current_state = LOH;
    }

    *ppos = count;
    return count;
}

static ssize_t guess_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    static const char prompt[] = "Hello, I'm thinking of a number. Try to guess it!\nUse echo XXX > /proc/guess\nThen read again\n";
    static const char loh[] = "LOH, try again!\n";
    static const char good[] = "Great, good Linux user\n";

    const char *str = prompt;
    size_t str_len = sizeof(prompt);

    switch (current_state)
    {
    case LOH:
        str = loh;
        str_len = sizeof(loh);
        break;
    case GOOD:
        str = good;
        str_len = sizeof(good);
        break;
    default:
        break;
    }

    if (*ppos > 0 || count < str_len)
        return 0;

    if (copy_to_user(ubuf, str, str_len))
        return -EFAULT;

    if(current_state == GOOD)
        current_state = BEGIN;

    *ppos = str_len;
    return str_len;
}

static struct proc_ops guess_ops = {
    .proc_read = guess_read,
    .proc_write = guess_write,
    .proc_lseek	= default_llseek,
};

static struct proc_dir_entry *ent;

static int guess_init(void)
{
    ent = proc_create("guess", 0666, NULL, &guess_ops);
    // printk(KERN_ALERT "guess init...\n");
    current_number = get_random_u32();
    return 0;
}

static void guess_cleanup(void)
{
    proc_remove(ent);
    // printk(KERN_ALERT "guess cleanup...\n");
}

module_init(guess_init);
module_exit(guess_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniil159x & ksen-lin");
