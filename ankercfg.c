#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define INTERFACE 2

void end_usb(libusb_device_handle* device)
{
    libusb_release_interface(device,INTERFACE);
    libusb_attach_kernel_driver(device,INTERFACE);
    libusb_close(device);
    libusb_exit(NULL);    
}

void error(libusb_device_handle* device,char *message, int error_code, int err_no)
{
    printf("%s (%s)(%s)\n",message, libusb_strerror(error_code), strerror(err_no));
    end_usb(device);
    exit(error_code);    
}

libusb_device_handle* init_usb(uint16_t vendor_id, uint16_t product_id)
{
    int r;
    libusb_device_handle* device;

    r = libusb_init(NULL);
    if (r < 0){
        printf("Error init libusb\n");
        exit(r);
    }

    libusb_set_debug(NULL,4);
    
    device = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
    if(device == NULL){
        printf("Device %x:%x not found\n", vendor_id, product_id);
        exit(-1);
    }

    libusb_detach_kernel_driver(device,INTERFACE);

    r = libusb_claim_interface(device,INTERFACE);
    if (r < 0){
        error(device,"Error claim interfase",r,errno);
    }
    
    return device;
}

void change_color(libusb_device_handle* device, unsigned char cyan,unsigned char magenta, unsigned char yellow)
{
    unsigned char data[16] = {0x02,0x04,0xff,0xff,0xff,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int r;
    unsigned char request_type = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT;
    unsigned char bRequest = 9;
    uint16_t wValue = 0x0302;
    uint16_t wIndex = 2;
	uint16_t wLength = 16;
    unsigned int timeout = 0;
    
    data[2] = cyan;
    data[3] = magenta;
    data[4] = yellow;

//    r = libusb_control_transfer (device, 0x21, 9, 0x0302, 2, data, 16, 0);
    r = libusb_control_transfer (device, request_type, bRequest, wValue, wIndex, data, wLength, timeout);
    if (r < 0){
        error(device,"Error in transfer",r,errno);
    }

}


int main()
{
    uint16_t vendor_id = 0x04d9;
    uint16_t product_id = 0xfa50;
    libusb_device_handle* device;    

    device = init_usb(vendor_id,product_id);
    
    //change_color(device,0xf0,0xf0,0x42); //blue
    change_color(device,0xff,0xff,0x00); 

    end_usb(device);
    
    return 0;
}
