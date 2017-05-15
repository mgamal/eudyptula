#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB Keyboard Eudyptula Example");

static struct usb_device_id usbkb_id_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,		
		USB_INTERFACE_PROTOCOL_KEYBOARD)},
		// Had no USB KB, so used mouse for testing purposes
                //USB_INTERFACE_PROTOCOL_MOUSE)},
	{ } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usbkb_id_table);

int __init hello_init(void)
{
	pr_info("Hello World!\n");
	return 0;
}

void __exit hello_exit(void)
{
	pr_info("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

