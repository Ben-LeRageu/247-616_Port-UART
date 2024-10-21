/**
* @file 	fork-uart.c
* Auteur: Benjamin Tremblay
* Modification: ChatGpt
* programme pour lecture et écriture du port série UART
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>   // File Control Definitions
#include <termios.h> // POSIX Terminal Control Definitions 
#include <unistd.h>  // UNIX Standard Definitions 
#include <errno.h>   // ERROR Number Definitions
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

const char *portTTY = "/dev/ttyS1";
int fd; // File Descriptor
pid_t child_pid;
int pipefd[2];

void initPortSerie(void)
 {
    struct termios SerialPortSettings;
    tcgetattr(fd, &SerialPortSettings); // Obtenir les attributs actuels du port série
    
    // Configuration de la vitesse de transmission en bauds (Baud rate)
    cfsetispeed(&SerialPortSettings, B115200);
    cfsetospeed(&SerialPortSettings, B115200);
    
    // Mode 8N1
    SerialPortSettings.c_cflag &= ~PARENB;
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |= CS8;
    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    SerialPortSettings.c_oflag &= ~OPOST;

    // Configuration des délais
    SerialPortSettings.c_cc[VMIN] = 1;
    SerialPortSettings.c_cc[VTIME] = 0; // pas d'attente

    if ((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0)
    {
        printf("\nErreur! configuration des attributs du port série");
        exit(EXIT_FAILURE);
    }
}

void parent_read_process()
 {
    char read_buffer[32];
    int bytes_read;
    int i = 0;

    printf("Je suis le processus Père, j'écrit sur la console (terminal) ce que j'entends sur le port série...\n");    
      while(read_buffer[i] != '!' && i<32)
      {
        bytes_read = read(fd, &read_buffer, sizeof(read_buffer)); // Lire les données depuis le port série
        
       i++;
      }
	   printf("processus Père: nombre d'octets reçus : %d --> ", bytes_read);
            for (i = 0; i < bytes_read; i++)
             
                // Afficher chaque caractère
                printf("%c", read_buffer[i]);
                fflush(stdout);

                    // Terminer le processus enfant
                    printf("\nFin du Fils\n");
                    kill(child_pid, SIGTERM); // Envoyer le signal de terminaison à l'enfant
                    
                    // Attendre que l'enfant se termine
                    wait(NULL);

                    // Message de fin du processus père
                    printf("processus Père: nombre d'octets reçus : 1 --> !\n");
                    printf("Fin du Père\n");
                    close(fd);
                    return;
}

void child_write_process()
 {
    char write_buffer[32];
    char cCharWritten;
    char cTruc = 0;
    int bytes_written = 0;

    printf("Je suis le processus Fils, j'écrit sur le port série ce que j'entends sur la console (terminal)...\n");
    cCharWritten = getchar();
    while (cCharWritten != 'q')
    {
      write_buffer[cTruc] = cCharWritten ;
      cTruc ++ ;
      cCharWritten = getchar();
    }
   bytes_written = write(fd, write_buffer, cTruc) ;
    printf("Fin du Fils\n");
}

void signal_handler(int signo)
 {
    if (signo == SIGTERM)
    {
        close(fd);
        exit(0);
    }
}

void main(void)
 {
    signal(SIGTERM, signal_handler); // Enregistrer le gestionnaire de signal pour SIGTERM

    printf("\nInitialisation du port série...\n");
    
    // Ouvrir le port série
    fd = open(portTTY, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("\nErreur! ouverture de %s\n", portTTY);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("\nOuverture de %s réussie\n", portTTY);
    }

    // Initialiser le port série
    initPortSerie();
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }
    // Créer un processus enfant
    child_pid = fork();
    if (child_pid == -1) { // Une erreur s'est produite
        perror("fork");
        return -1;
    }

     if (child_pid == 0)
     {
        // Code du processus enfant : écriture des données
        child_write_process();
    }
    else
    {
        // Code du processus parent : lecture des données
        parent_read_process();
    }
}
