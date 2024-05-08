/**
 * rs232_module.c
 * Ce fichier contient l'initialisation d'un module tutoriel pour le rs232. Vos
 * modifications doivent etre apporte au fichier rs232_tp.c et rs232_tut.h, ce
 * fichier ne devrait pas etre modifie.
 * 
 * Les fonction rs232_tut_init et rs232_tut_exit sont respectivement les points
 * d'entres et de sortie du module.
 */

#include <linux/kernel.h>           // Nous sommes dans le noyau.
#include <linux/module.h>           // Nous faisons un module.
#include <linux/interrupt.h>        // On utilise une interruption.
#include <linux/moduleparam.h>      // On a des parametre au module.

#include "rs232_tut.h"              // Interface avec le fichier rs232_tp.c.
#include "rs232_tut_regs.h"         // Ports d'ES et autre contantes du rs232.

/**
 * Definitions des parametres au module.
 * irq: Le numero d'irq
 * base_port: Le port de base du uart.
 */
int             irq       = RS232_DEFAULT_IRQ_NO;
unsigned long   base_port = RS232_DEFAULT_BASE_PORT;

module_param (irq, int, S_IRUGO);
module_param (base_port, ulong, S_IRUGO);

/**
 * rs232_tut_dev: La structure de notre peripherique.
 */
struct rs232_tut_dev rs232_tut_dev;

/**
 * Le file_operations contients les pointeurs vers les fonctions qui sont
 * utilise pour faire le pond avec l'espace utilisateur. Il sont appelle par
 * les appels systeme respectif.
 */
struct file_operations rs232_tut_fops = {
    .owner      = THIS_MODULE,
    .open       = rs232_tut_open,
    .release    = rs232_tut_release,
    .read       = rs232_tut_read,
    .write      = rs232_tut_write,
};

/**
 * Enregistrement de la routine d'interruption.
 *
 * @return Code d'erreur ou 0.
 */
static int __init init_irq (void)
{
    if (request_irq (irq, rs232_tut_isr, 0, MODULE_NAME, NULL)) {
        ERR("L'irq %i ne peut etre reserve.\n", irq);
        INFO("Verifier que le module serial n'est pas charge.\n");
        return -ENXIO;
    }
    return 0;
}

/**
 * Liberation de l'irq.
 */
static void clean_irq (void)
{
    free_irq (irq, NULL);
}

/**
 * Reservation de port d'entree/sortie.
 *
 * @return Code d'erreur ou 0.
 */
static int __init init_ports (void)
{
    rs232_tut_dev.resource = request_region (base_port, RS232_NB_PORT, MODULE_NAME);
    if (!rs232_tut_dev.resource) {
        ERR("Ne peut reserver les ports d'e/s ( 0x%lX ).\n", base_port);
        INFO("Verifier que le module serial n'est pas charge.\n");
        return -ENXIO;
    }
    return 0;
}

/**
 * Liberation des port d'entree/sortie.
 */
static void clean_ports (void)
{
    release_region (base_port, RS232_NB_PORT);
}

/**
 * Initialisation du "character device" pour l'interface avec le monde usager.
 *
 * @return Code d'erreur ou 0.
 */
static int __init init_cdev (void)
{
    int ret;
    ret = alloc_chrdev_region (&rs232_tut_dev.num, 0, 1, MODULE_NAME);
    if (!ret) { 
        cdev_init (&rs232_tut_dev.cdev, &rs232_tut_fops); 
        rs232_tut_dev.cdev.owner = THIS_MODULE;
        rs232_tut_dev.cdev.ops = &rs232_tut_fops;
        ret = cdev_add (&rs232_tut_dev.cdev, rs232_tut_dev.num, 1);
        if (!ret) {

            INFO("Received major number %i.\n", MAJOR (rs232_tut_dev.num));
            return 0;
        }
        unregister_chrdev_region (rs232_tut_dev.num, 1);
    }
    return ret;
}

/**
 * Retrait du "character device".
 */
static void clean_cdev (void)
{
    cdev_del (&rs232_tut_dev.cdev);
    unregister_chrdev_region (rs232_tut_dev.num, 1);
}

/**
 * Definir MIN s'il n'est pas definit.
 */
#ifndef MIN
# define MIN(a,b) (a)<(b)?(a):(b)
#endif

/**
 * Grandeur du buffer utiliser pour l'entree dev dans sysfs.
 */
#define NODE_BUFFER_SIZE 7

/**
 * (Callback) Lecture de l'entree dev sysfs.
 * @param kobj Le kobj associe avec le noeud dans sysfs.
 * @param buffer Le buffer ou ecrire "MAJOR:MINOR".
 * @param pos Le nombre d'octets ecrit lors d'un appel precedent.
 * @param size Le nombre d'octets a ecrire dans buffer.
 *
 * @return Le nombre d'octets ecrit ou un code d'erreur negatif.
 */
ssize_t node_attr_read (struct kobject *kobj, char *buffer, loff_t pos, size_t size)
{
    char buf[NODE_BUFFER_SIZE];

    sprintf (buf, "%hu:%hu\n", MAJOR(rs232_tut_dev.num), MINOR(rs232_tut_dev.num));
    size = MIN(size,NODE_BUFFER_SIZE);
    memcpy (buffer, buf + pos, size);
    return size;
}

/**
 * Structure pour l'entree dev dans sysfs.
 */
struct bin_attribute node_attr = {
    .attr = {
        .name = "dev",
        .owner = THIS_MODULE,
        .mode = S_IRUGO | S_IWUGO,
    },
    .size = NODE_BUFFER_SIZE,
    .read = node_attr_read,
};

/**
 * Creation de l'entree dev dans sysfs (pour la lecture des numeros major et
 * minor.
 *
 * @return Code d'erreur ou 0.
 */
static int init_node (void)
{
    return sysfs_create_bin_file(&THIS_MODULE->mkobj.kobj, &node_attr);
}

/**
 * Retrait de l'entree dev dans sysfs.
 */
static void clean_node (void)
{
    sysfs_remove_bin_file (&THIS_MODULE->mkobj.kobj, &node_attr);
}

/**
 * Initialisation du pilote, appelle par insmod.
 */
static int __init rs232_tut_init (void)
{
    int ret;

    ret = init_irq ();
    if (ret == 0) {
        ret = init_ports ();
        if (ret == 0) {
            ret = init_cdev ();
            if (ret == 0) {
                ret = init_node ();
                if (ret == 0) {
                    ret = init_rs232 ();
                    if (ret == 0) {
                        return 0;
                    }
                    clean_rs232 ();
                }
                clean_node ();
            }
            clean_ports ();
        }
        clean_irq ();
    }
    return ret;
}

/**
 * Nettoyage du pilote, appelle par rmmod.
 */
static void __exit rs232_tut_exit (void)
{
    INFO("Dechargement ...\n");
    clean_rs232 ();
    clean_node ();
    clean_cdev ();
    clean_ports ();
    clean_irq ();
}

/**
 * Enregistrement du module.
 */
module_init (rs232_tut_init);
module_exit (rs232_tut_exit);

MODULE_AUTHOR       (RS232_TUT_AUTHOR);
MODULE_DESCRIPTION  (RS232_TUT_DESC);
MODULE_VERSION      (RS232_TUT_VERSION);
MODULE_LICENSE      ( "GPL v2" );
