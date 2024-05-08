#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x9ebe240b, "struct_module" },
	{ 0x1a1a4f09, "__request_region" },
	{ 0xce469351, "cdev_del" },
	{ 0xecbcf677, "cdev_init" },
	{ 0x1e98cf99, "sysfs_remove_bin_file" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x66eaa8a9, "param_set_ulong" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x865ebccd, "ioport_resource" },
	{ 0x1b7d4074, "printk" },
	{ 0x73f06405, "cdev_add" },
	{ 0x26e96637, "request_irq" },
	{ 0xd49501d4, "__release_region" },
	{ 0x7ded8240, "param_get_ulong" },
	{ 0xd0129f6, "sysfs_create_bin_file" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C8EB6721268A467E8988FFB");
