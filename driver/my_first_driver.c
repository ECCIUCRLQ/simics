// Writing a PCI device driver for Linux
// by Oleg Kutkov
// January 7, 2021
// https://olegkutkov.me/2021/01/07/writing-a-pci-device-driver-for-linux/


// Simple Linux character device driver
// by Oleg Kutkov
// March 14, 2018
// https://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/

#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/notifier.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/uaccess.h>


u_int16_t bar0, bar1;


// Herramienta util para testeos tempranos del driver
#define DRIVER_VERBOSE

// Definicion driver
#define MY_DRIVER "my_pci_driver"


static struct  
{
    struct pci_dev *pdev;
    struct class*  charClass; ///< The device-driver class struct pointer
    struct device* charDevice; ///< The device-driver device struct pointer

    // Hardware interface
    void __iomem  *bar0_base_addr;   // BAR0: Registros de control 
    
    void __iomem  *bar1_base_addr;   // BAR2: embedded memory
    // Runtime values
    uint32_t fb_base_offset;  // offset into the device memory for the framebuffer

} device;

#define BAR0


// tabla de ID del PCI device
static struct pci_device_id my_driver_id_table[] =
{
    { PCI_DEVICE(0X04, 0x03) } , {0,}
};


//----------------------------------------------------------
//
// CHAR DEVICE INTERFACE of the device
//
//----------------------------------------------------------
// Char device file operation functions
//----------------------------------------------------------
static int      chari_open(struct inode *, struct file *);
static int      chari_release(struct inode *, struct file *);
static ssize_t  chari_read(struct file *, char *, size_t, loff_t *);
static ssize_t  chari_write(struct file *, const char *, size_t, loff_t *);
static int      chari_mmap(struct file *, struct vm_area_struct *);
static long     chari_ioctl(struct file *f, unsigned int cmd, unsigned long arg);
// Its not a prototip 


//----------------------------------------------------------
// Char device: Funcion open 
//----------------------------------------------------------
// Esta funcion abre el dispositivo y muestra por 
//  pantalla un mensaje al hacerlo
static int chari_open(struct inode *inodep, struct file *filep)
{
    printk("Dispositivo abierto");
    return 0; 
}
//----------------------------------------------------------
// Char device: Funcion release 
//----------------------------------------------------------
// Esta funcion cierra el dispositivo y muestra por 
//  pantalla un mensaje al hacerlo
static int chari_release(struct inode *inodep, struct file *filep)
{
    printk("Dispositivo Cerrado");
    return 0;
}
//----------------------------------------------------------
// Char device: Funcion read
//----------------------------------------------------------
// Esta funcion lee un registro del BAR 0, especificamente
//  la que tiene un offset de 0x04
static ssize_t chari_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    uint8_t *data = "Hello from the kernel world!\n";
    size_t datalen = strlen(data);

    if (len > datalen) 
    {
        len = datalen;
    }

    if (copy_to_user(buffer, data, len)) 
    {
        return -EFAULT;
    }

    return len;
}
//----------------------------------------------------------
// Char device: Funcion write 
//----------------------------------------------------------
// Esta funcion escribe un 1 en un registro del BAR 0, 
// especificamente la que tiene un offset de 0x04
static ssize_t chari_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    size_t maxdatalen = 30, ncopied;
    uint8_t databuf[maxdatalen];

    if (len < maxdatalen) {
        maxdatalen = len;
    }

    ncopied = copy_from_user(databuf, buffer, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    databuf[maxdatalen] = 0;

    printk("Data from the user: %s\n", databuf);

    return len;
}
//----------------------------------------------------------
// Char device: Funcion mmap
//----------------------------------------------------------
// Estas funciones hacen un mapeo de puertos de Linux


// Este set de 3 funciones son necesarias para chari_map
void vm_open(struct vm_area_struct *vma) {
    pr_info(//DISPOSITIVO
            ": (mmap) VMA Open. virtual: 0x%lx, physical: 0x%llx\n",
            vma->vm_start, 
            device.pdev->resource[0].start);
}

void vm_close(struct vm_area_struct *vma) {
    pr_info( ": (mmap) VMA closed.\n");
}

static struct vm_operations_struct vm_ops = {
    .open = vm_open,
    .close = vm_close,
};


// Estas funciones hacen un mapeo de puertos de Linux
static int chari_mmap(struct file *filp, struct vm_area_struct *vma){
    pr_info(//DISPOSITIVO
            ": chari_mmap called!\n");
    remap_pfn_range(vma, vma->vm_start,
                    device.pdev->resource[0].start>>PAGE_SHIFT,
                    vma->vm_end - vma->vm_start,
                    vma->vm_page_prot);
    vma->vm_ops = &vm_ops;
    vm_open(vma);
    return 0;		
}
//---------------------------- end MMAP -----------------------------------


//----------------------------------------------------------
// Char device: Funcion ioctl 
//----------------------------------------------------------
// Permite al programa de usuario manejar interrupciones
static long chari_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    printk("Se llamó el chari_ioctl");
    return 0;
}

MODULE_DEVICE_TABLE(pci, my_driver_id_table);

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);

// Driver registration structure 
static struct pci_driver my_driver = 
{
    .name = MY_DRIVER, 
    .id_table = my_driver_id_table, 
    .probe = my_driver_probe, 
    .remove = my_driver_remove
};

// This is a "private" data structure
// You can store there any data that should be passed between driver's functions
struct my_driver_priv 
{
    u8 __iomem *hwmem; // WHAT IS THIS
};

static int __init mypci_driver_init(void)
{
    // Seteo de las direcciones del BAR0 y BAR1
    u_int64_t offset_bar0 = 0x10;
    pci_read_config_word(device.pdev, offset_bar0, &bar0);

    u_int64_t offset_bar1 = 0x14;
    pci_read_config_word(device.pdev, offset_bar1, &bar1);  

    //Register new PCI driver
    return pci_register_driver(&my_driver);

}

static void __exit mypci_driver_exit(void)
{
    // Unregister
    pci_unregister_driver(&my_driver);
}

void release_device(struct pci_dev *pdev)
{
    // Disable IRQ #42
    free_irq(42, pdev);
    // Free memory region
    pci_release_region(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
    // And disable device 
    pci_disable_device(pdev);
}

static irqreturn_t irq_handler(int irq, void *cookie)
{
    (void) cookie;
    printk("Handle IRQ #%d\n", irq);
    return IRQ_HANDLED; 
}

// Request interrupt and setup handler
int set_interrupts(struct pci_dev *pdev)
{
    // We want MSI interrupt, 3 lines (just an example)
    int ret = pci_alloc_irq_vectors(pdev, 3, 3, PCI_IRQ_MSI);
    if(ret < 0)
    {
        return ret;
    }
    // Request IRQ #42
    return request_threaded_irq(42, irq_handler, NULL, 0, "TEST IRQ", pdev);
}

// Write some data to the device 
void write_sample_data(struct pci_dev *pdev)
{
    int data_to_write = 0xDEADEEF; // Just a random trash
    struct my_driver_priv *drv_priv = (struct my_driver_priv *) pci_get_drvdata(pdev);
    if(!drv_priv)
    {
        return;
    }

    // Write 32-bit data to the device memory
    iowrite32(data_to_write, drv_priv->hwmem);
}

// This function is called by the kernel
static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int bar, err;
    u16 vendor, device;
    unsigned long mmio_start, mmio_len;
    struct my_driver_priv *drv_priv;

    // Let's read data from the PCI device configuration registers
    pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
    pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

    printk(KERN_INFO "Device vid: 0x%X pid: 0x%X\n", vendor, device);

    // Request IO BAR
    bar = pci_select_bars(pdev, IORESOURCE_MEM);

    // Enable device memory
    err = pci_enable_device_mem(pdev);

    if(err)
    {
        return err;
    }

    // Request memory region for the BAR
    err = pci_request_region(pdev, bar, MY_DRIVER);
    
    if(err)
    {
        pci_disable_device(pdev);
        return err;
    }

    // Get start and stop memory offsets
    mmio_start = pci_resource_start(pdev, 0);
    mmio_len = pci_resource_len(pdev, 0); 

    // Allocate memory for the driver private data
    drv_priv = kzalloc(sizeof(struct my_driver_priv), GFP_KERNEL);
    if(!drv_priv)
    {
        release_device(pdev);
        return -ENOMEM;
    }

    // Remap BAR to the local pointer
    drv_priv -> hwmem = ioremap(mmio_start, mmio_len);
    
    if(!drv_priv->hwmem)
    {
        release_device(pdev);
        return -EIO;
    }

    // Set driver private data
    // Now we van access mapped "hwmem" from the any driver's function
    pci_set_drvdata(pdev, drv_priv);

    write_sample_data(pdev);
    
    return set_interrupts(pdev);
}

// Clean up 
static void my_driver_remove(struct pci_dev *pdev)
{
    struct my_driver_priv *drv_priv = pci_get_drvdata(pdev);

    if(drv_priv)
    {
        if(drv_priv->hwmem)
        {
            iounmap(drv_priv->hwmem);
        }
        pci_free_irq_vectors(pdev);
        kfree(drv_priv);
    }

    release_device(pdev);
}

//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("Oleg Kutkov <contact@olegkutkov.me>");
//MODULE_DESCRIPTION("Test PCI driver");
//MODULE_VERSION("0.1");

module_init(mypci_driver_init);
module_exit(mypci_driver_exit);
