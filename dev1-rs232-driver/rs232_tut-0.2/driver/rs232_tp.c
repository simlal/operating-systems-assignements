// Test from deck
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
 * Reception state 
*/
// u8 received_character;
// int character_received_flag = 0;

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

        INFO("Initializing buffers...\n");
        //IFT320 : Provoquez l'appel de votre routine d'initialisation de vos cbuffer
        rs232_tut_dev.cbuf_in = cbuffer_init(80);
        rs232_tut_dev.cbuf_out = cbuffer_init(40);

        cbuffer_info(rs232_tut_dev.cbuf_in);
        cbuffer_info(rs232_tut_dev.cbuf_out);

        // Setting UART
        INFO("Initializing UART...\n");
        
        INFO("Initial read on RBR to clear garbage data.\n");
        inb( RS232_RBR(base_port) );
        
        // Define communication speed
        speed_dll = RS232_DLL_9K;
        speed_dlm = RS232_DLM_9K;
        
        outb(RS232_CMD_SPEED, RS232_LCR(base_port));
        outb(speed_dll, RS232_DLL(base_port));
        outb(speed_dlm, RS232_DLM(base_port));

        // LCR: 8bits-size, 1-stopbit, no-parity, no-break, normal-addressing
        INFO("Configuring LCR with: 8bits-size, 1-stopbit, no-parity, no-break, normal-addressing.\n");
        outb(RS232_CFG_LINE, RS232_LCR(base_port));
        
        // IER: Enable interrupts
        INFO("Enabling interrupts on serial port.\n");
        outb(RS232_INT_ON, RS232_IER(base_port));
        
        // FCR: Enabled/disable buffers
        u8 fcr_buffer_config;
        fcr_buffer_config = RS232_CFG_NOBUFFER;
        INFO("FCR WITH FIFO: %s\n", fcr_buffer_config == RS232_CFG_BUFFER ? "true" : "false");
        outb(RS232_CFG_NOBUFFER, RS232_FCR(base_port));

        // MCR: Allow interrupts from IER
        INFO("Allowing interrupts from IER.\n");
        outb(RS232_HAND_SHAKE, RS232_MCR(base_port));
        
        // Initial LSR read
        unsigned char lsr_read;
        char* lsr_msg;
        lsr_read = inb(RS232_LSR(base_port));
        if (lsr_read & RS232_READY_TO_SEND) {
            lsr_msg = "Ready to send";
        } else if (lsr_read & RS232_DATA_AVAILABLE) {
            lsr_msg = "Data available to read";
        } else {
            lsr_msg = "No data available";
        }
        INFO("LSR read value: 0x%x (%s)\n", lsr_read, lsr_msg);

        // Initial MSR read
        unsigned char msr_read; 
        msr_read = inb(RS232_MSR(base_port));        
        INFO("MSR read value: 0x%x\n", msr_read);

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
 * du pilote et decrementer sont 
 * usage.
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
        cbuffer_free(rs232_tut_dev.cbuf_in);
        cbuffer_free(rs232_tut_dev.cbuf_out);

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

    wait_event_interruptible(wq, !cbuffer_is_empty(rs232_tut_dev.cbuf_in));
    INFO("WOKEN UP TO READ!\n");
    size_t  size = 0;
    INFO("READ : Lecture d'un maximum de %i octets.\n", maximum_size);
    
    // Getting data from cbuff with ISR wake-up with data available
    u8 received_character;
    int dequeue_status;
    dequeue_status = cbuffer_dequeue(rs232_tut_dev.cbuf_in, &received_character);
    if (dequeue_status != 0) {
        INFO("dequeue operation failed inside READ kernel space.\n");
    }

    // Transfering from cbuff in kernel to userspace
    int copy_to_status;
    copy_to_status = copy_to_user(userbuffer, &received_character, 1);
    if (copy_to_status != 0) {
        INFO("Error copying to user.\n");
        return 0;
    }
    size++;

    // if (character_received_flag == 1) {
        
    //     // Get the byte from RBR
    //     u8 received_character;
        

    //     // Send it to the user
    //     int read_offset = 0;

    //     int copy_to_status;
    //     copy_to_status = copy_to_user(userbuffer + read_offset, &received_character, 1);
    //     if (copy_to_status != 0) {
    //         INFO("Error copying to user.\n");
    //     }
        
    //     character_received_flag--;
    // }

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
    size_t size;
    INFO("WRITE : Ecriture d'un maximum de %i octets.\n", maximum_size);

    // Write call from EMPTY_THR interrupt but empty buffer
    

    // Copying userbuffer to driver cbuffer
    u8 char_to_enqueue;
    int copy_from_status;
    int enqueue_status;
    for (size = 0; size < maximum_size; size++) {
        // Per byte transfer of writeline from user to kernel space
        copy_from_status = copy_from_user(&char_to_enqueue, userbuffer + size, 1);
        if (copy_from_status != 0) {
            INFO("Error. Could not write to userbuffer[%i] to THR!\n", size);
            return -1;
        }
        // Enqueue current byte to cbuffer
        enqueue_status = cbuffer_enqueue(rs232_tut_dev.cbuf_out, char_to_enqueue);
        if (enqueue_status != 0) {
            INFO("Error. Could not enqueue to cbuffer_out!\n");
        }
    }
    cbuffer_info(rs232_tut_dev.cbuf_out);
    //IFT320 :  Une t�che vous demande d'aller transmettre un certain nombre de lettre vers le UART (� travers la ISR).
    //IFT320 :  Attention, le param�tre 'userbuffer' est d�finit dans l'espace m�moire 'usager'. Pour obtenir une lettre,
    //IFT320 :  faites appel � 'copy_from_user(&byte, userbuffer + i, 1)'. Un code de retour vous indique si l'op�ration
    //IFT320 :  fut bien r�ussie.
    //IFT320 :  Pour signaler � Linux que vous ne souhaitez plus recevoir de temps CPU,
    //IFT320 :  faite un appel � 'wait_event_interruptible(wq,CONDITION);'. Le terme 'CONDITION' pourrait �tre 
    //IFT320 :  quelques chose comme '!cbuffer_isfull(rs232_tut_dev.cbuf_out)';

    INFO("WRITE : Ecriture effective de %i octets.\n", size);
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
    INFO("ISR called %i times so far!\n", isr_called);
    // Execute until no more pending interrupt
    while( (( RS232_INT_PENDING & 
              (tmp_iir = inb( RS232_IIR(base_port) ))) == 0)) 
    {
        INFO("Handling pending interrupt...\n");
        last_iir = tmp_iir;
        switch ( RS232_IIR_MASK & tmp_iir ) {
            case RS232_DATA_AVAIL:
                INFO("RBR has data. Ready to read.\n");
                int_received++;
                // character_received_flag++;
                
                // Reading from RBR and enqueuing in cbuffer
                u8 received_character;
                int enqueue_status;
                received_character = inb(RS232_RBR(base_port));
                if (!cbuffer_is_full(rs232_tut_dev.cbuf_in)) {
                    enqueue_status = cbuffer_enqueue(rs232_tut_dev.cbuf_in, received_character);
                    if (enqueue_status != 0) {
                        INFO("enqueue operation failed inside DATA_AVAIL interrupt.\n");
                    }
                } else {
                    INFO("cbuffer is full, cannot enqueue!\n");
                }

                // Wake up all processes waiting for data to be read
                INFO("Received: %c and transfered to cbuff. Waking up interruptible!\n", received_character);
                wake_up_interruptible(&wq);
                //IFT320 :  Une ou plusieurs lettres sont disponibles pour lecture en provenance du UART
                //IFT320 :  Pour acc�der � une lettre faite un appel � 'byte = inb( RS232_RBR(base_port) ));'
                //IFT320 :  Pour signaler � Linux que du temps CPU peut �tre accord� aux routines read/write du pilote,
                //IFT320 :  faite un appel � 'wake_up_interruptible(&wq);'

                break;
            case RS232_EMPTY_THR:
                int_send++;
                // Sending data from cbuffer to THR
                if (!cbuffer_is_empty(rs232_tut_dev.cbuf_out)) {
                    u8 character_to_send;
                    int dequeue_status;
                    dequeue_status = cbuffer_dequeue(rs232_tut_dev.cbuf_out, &character_to_send);
                    if (dequeue_status != 0) {
                        INFO("dequeue operation failed inside EMPTY_THR interrupt.\n");
                    } 
                    outb(character_to_send, RS232_THR(base_port));
                    INFO("Sent: %c from cbuffer to THR.\n", character_to_send);
                } else {
                    INFO("Empty THR interrupt, but cbuffer is empty. Cannot dequeue and write to THR!\n");
                }

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
