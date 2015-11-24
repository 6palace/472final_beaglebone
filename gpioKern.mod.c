#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x6d7beb84, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x17c19aba, __VMLINUX_SYMBOL_STR(cdev_alloc) },
	{ 0xfbc74f64, __VMLINUX_SYMBOL_STR(__copy_from_user) },
	{ 0x7ffc8718, __VMLINUX_SYMBOL_STR(gpio_set_debounce) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x1afae5e7, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x65d6d0f0, __VMLINUX_SYMBOL_STR(gpio_direction_input) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0x82f776b7, __VMLINUX_SYMBOL_STR(gpio_export) },
	{ 0x492aaf19, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x194bf076, __VMLINUX_SYMBOL_STR(pid_task) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x11f447ce, __VMLINUX_SYMBOL_STR(__gpio_to_irq) },
	{ 0x2b5d5491, __VMLINUX_SYMBOL_STR(init_pid_ns) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x9d798dfa, __VMLINUX_SYMBOL_STR(send_sig_info) },
	{ 0x6c8d5ae8, __VMLINUX_SYMBOL_STR(__gpio_get_value) },
	{ 0x4be7fb63, __VMLINUX_SYMBOL_STR(up) },
	{ 0xe61a6d2f, __VMLINUX_SYMBOL_STR(gpio_unexport) },
	{ 0x3e26490d, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x8c4faf47, __VMLINUX_SYMBOL_STR(find_pid_ns) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D57801DB952E40BC9C46EAF");
