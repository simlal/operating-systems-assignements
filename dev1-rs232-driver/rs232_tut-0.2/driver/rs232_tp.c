#include <asm/io.h>
#include <asm/uaccess.h>
#include "rs232_tut.h"
#include "rs232_tut_regs.h"
#include "cbuffer.h"

#define buffer_len 10

/**
 * Ensemble de variable de debuggage
 */
unsigned int isr_called = 0;
unsigned int last_iir = 0;
unsigned int int_send = 0;
unsigned int int_received = 0;
unsigned int int_error=0;
char system_failed = 0;

/**
 * Statut du pilote
 */
int uart_initialized = 0;
char rs232_isempty_thr;

/**
 * File d'attente d'evenement.
 */
static DECLARE_WAIT_QUEUE_HEAD(wq);

/**
 * Imprime dans le log noyau le status du pilote.
 */
void status(void)
{
    INFO("isr_called: %d\n", isr_called);
    INFO("last_iir: 0x%x\n", last_iir);
    INFO("int_received: %d\n", int_received);
    INFO("int_send: %d \n", int_send);
    INFO("int_error: %d \n", int_error);
}

/**
 * Ouverture du "character device".
 *
 * @param inode inode->i_cdev contient le pointeur vers votre cdev. Il peut etre
 * utilise pour trouver l'adresse de la structure du pilote.
 * @param file file->private_data peut etre utilise pour enregistrer la
 * structure du pilote.
 *
 * @return Code d'erreur ou 0.
 */
int rs232_tut_open (struct inode *inode, struct file *file)
{
    u8 speed_dll,speed_dlm;

    INFO("Trying to open the node...\n");

    uart_initialized++;
    if (uart_initialized == 1) {

        INFO("Inintializing buffers...\n");
        //IFT320 : Provoquez l'appel de votre routine d'initialisation de vos cbuffer
        //IFT320 : rs232_tut_dev.cbuf_in
        //IFT320 : rs232_tut_dev.cbuf_out 


        INFO("Inintializing UART...\n");
        // Need a param to configure this
        speed_dll = RS232_DLL_9K;
        speed_dlm = RS232_DLM_9K;

        // Setting UART
        inb( RS232_RBR(base_port) );
        //IFT320 : Compl�tez l'initialisation du UART. 
        //IFT320 : Vous ne devez pas utiliser disable()/enanble().
        //IFT320 : Vous ne devez pas g�rer le PIC.
        //outb(         ,   RS232_LCR(base_port) );
        //outb(speed_dll,         RS232_DLL(base_port) );
        //outb(speed_dlm,         RS232_DLM(base_port) );
        //outb(         ,    RS232_LCR(base_port) );
        //outb(         ,      RS232_IER(base_port) );
        //outb(         ,  RS232_FCR(base_port) );
        //outb(         ,  RS232_MCR(base_port) );
        // etc.

        INFO("UART initialized.\n");
    }        

    INFO("Node open correctly.\n");

    return 0;
}

/**
 * Fermeture du "character device".
 *
 * @param inode inode->i_cdev contient le pointeur vers votre cdev. Il peut etre
 * utilise pour trouver l'adresse de la structure du pilote.
 * @param file file->private_data peut etre utilise pour retrouver la structure
 * du pilote et decrementer sont usage.
 *
 * @return Code d'erreur ou 0.
 */
int rs232_tut_release (struct inode *inode, struct file *file)
{
    INFO("Trying to closing node ...\n");

    uart_initialized--; 
    if (uart_initialized == 0) {

        INFO("Shutting down interruptions.\n");
        outb( RS232_INT_OFF, RS232_IER(base_port) );

        INFO("Desallocating buffers.\n");
        //IFT320 : Provoquez l'appel de votre routine de de-initialisation (release) de votre cbuffer
        //IFT320 : rs232_tut_dev.cbuf_in
        //IFT320 : rs232_tut_dev.cbuf_out 


        status();
        INFO("System failed:    %d\n", system_failed);
    }

    INFO("Node closed ...\n");

    return 0;
}

/**
 * Lecture du "character device".
 *
 * @param file file->private_data peut etre utilise pour retrouver la structure
 * du pilote et decrementer sont usage.
 * @param userbuffer contient l'addresse du buffer dans laquelle l'usage veut
 * lire. Vous devez utiliser copy_to_user pour ecrire a cette addresse.
 * @param size la longueur que vous pouvez ecrire dans le userbuffer.
 * @param o le nombre d'octets deja demander lors d'un appel precedent.
 *
 * @return Code d'erreur (negatif) ou le nombre d'octets ecrit dans le
 * userbuffer.
 */
ssize_t rs232_tut_read (struct file *file, char __user *userbuffer, size_t maximum_size, loff_t *o)
{   
    //IFT320 : u8 byte;
    size_t  size = 0;

    // INFO("READ : Lecture d'un maximum de %i octets.\n", maximum_size);

    //IFT320 :  Une t�che vous demande d'aller lire un certain nombre de lettre en provenance du UART (� travers la ISR).
    //IFT320 :  Attention, le param�tre 'userbuffer' est d�finit dans l'espace m�moire 'usager'. Pour obtenir une lettre,
    //IFT320 :  faites appel � 'copy_from_user(&byte, userbuffer + i, 1)'. Un code de retour vous indique si l'op�ration
    //IFT320 :  fut bien r�ussie.
    //IFT320 :  Pour signaler � Linux que vous ne souhaitez plus recevoir de temps CPU,
    //IFT320 :  faite un appel � 'wait_event_interruptible(wq,CONDITION);'. Le terme 'CONDITION' pourrait �tre 
    //IFT320 :  quelques chose comme '!cbuffer_isempty(rs232_tut_dev.cbuf_in)';



    // INFO("READ : Lecture effective de %i octets.\n", size);
    return size;
}

/**
 * Ecriture du "character device".
 *
 * @param file file->private_data peut etre utilise pour retrouver la structure
 * du pilote et decrementer sont usage.
 * @param userbuffer contient l'addresse du buffer contenant les donnees que
 * l'usage veut ecrire. Vous devez utiliser copy_from_user pour lire a cette
 * addresse.
 * @param size la longueur (en octets) des donnees se trouvant dans le
 * userbuffer.
 * @param o le nombre d'octets deja demander lors d'un appel precedent.
 *
 * @return Code d'erreur (negatif) ou le nombre d'octets lus du userbuffer.
 */
ssize_t rs232_tut_write (struct file *file, const char __user *userbuffer, size_t maximum_size, loff_t *o)
{
    //IFT320 : u8 byte;
    size_t  size = 0;

    INFO("WRITE : �criture d'un maximum de %i octets.\n", maximum_size);

    //IFT320 :  Une t�che vous demande d'aller transmettre un certain nombre de lettre vers le UART (� travers la ISR).
    //IFT320 :  Attention, le param�tre 'userbuffer' est d�finit dans l'espace m�moire 'usager'. Pour obtenir une lettre,
    //IFT320 :  faites appel � 'copy_from_user(&byte, userbuffer + i, 1)'. Un code de retour vous indique si l'op�ration
    //IFT320 :  fut bien r�ussie.
    //IFT320 :  Pour signaler � Linux que vous ne souhaitez plus recevoir de temps CPU,
    //IFT320 :  faite un appel � 'wait_event_interruptible(wq,CONDITION);'. Le terme 'CONDITION' pourrait �tre 
    //IFT320 :  quelques chose comme '!cbuffer_isfull(rs232_tut_dev.cbuf_out)';



    INFO("WRITE : �criture effective de %i octets.\n", size);
    return size;
}

/**
 * La routine execute lors d'une interruption.
 *
 * @param irq le numero de l'interruption recu.
 * @param dev_id un pointeur vers la structure global nomme rs232_tut_dev.
 * @param state l'etat des registres avant l'interruption.
 *
 * @return IRQ_HANDLED pour dire que l'interruption a ete traite.
 */
irqreturn_t rs232_tut_isr (int irq, void *dev_id, struct pt_regs *state)
{
    u8 tmp_iir;

    if (system_failed) return IRQ_HANDLED;

    isr_called++;
    while( (( RS232_INT_PENDING & 
              (tmp_iir = inb( RS232_IIR(base_port) ))) == 0)) 
    {
        last_iir = tmp_iir;
        switch ( RS232_IIR_MASK & tmp_iir ) {
            case RS232_DATA_AVAIL:
                int_received++;
                //IFT320 :  Une ou plusieurs lettres sont disponibles pour lecture en provenance du UART
                //IFT320 :  Pour acc�der � une lettre faite un appel � 'byte = inb( RS232_RBR(base_port) ));'
                //IFT320 :  Pour signaler � Linux que du temps CPU peut �tre accord� aux routines read/write du pilote,
                //IFT320 :  faite un appel � 'wake_up_interruptible(&wq);'


                break;
            case RS232_EMPTY_THR:
                int_send++;
                //IFT320 :  Une ou plusieurs cases sont disponibles pour �criture vers le UART
                //IFT320 :  Pour acc�der � une lettre faite un appel � 'outb (byte, RS232_THR(base_port) );'
                //IFT320 :  Pour signaler � Linux que du temps CPU peut �tre accord� aux routines read/write du pilote,
                //IFT320 :  faite un appel � 'wake_up_interruptible(&wq);'



                break;
            case RS232_ERROR:
                int_error++;
                inb( RS232_LSR(base_port) );
                break;
        }
    }
    return IRQ_HANDLED;
}

/**
 * Initialisation du pilote. 
 * Initialiser ici les structure que vous aurez besoin tant que votre pilote
 * sera charge dans le noyau.
 *
 * @return code d'erreur
 */
int init_rs232 (void)
{
    return 0;
}

/**
 * Nettoyage du pilote. 
 * Liberer ici les ressource que vous avez utiliser dans votre pilote.
 */
void clean_rs232 (void)
{
}