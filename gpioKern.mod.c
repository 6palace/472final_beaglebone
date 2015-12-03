#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x8e8969cc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6cbe003e, __VMLINUX_SYMBOL_STR(cdev_alloc) },
	{ 0x60787e, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x34d4df53, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x1afae5e7, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0xfaed932, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x275ef902, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa5716c28, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x18a05260, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xfb518b3c, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x341ce9f6, __VMLINUX_SYMBOL_STR(gpiod_set_debounce) },
	{ 0x61c7782f, __VMLINUX_SYMBOL_STR(gpiod_unexport) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x1f77d11c, __VMLINUX_SYMBOL_STR(gpiod_export) },
	{ 0xe322329b, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x4be7fb63, __VMLINUX_SYMBOL_STR(up) },
	{ 0xc0420fec, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xd47e61e0, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xd37140de, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

