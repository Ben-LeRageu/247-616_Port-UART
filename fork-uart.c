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
#include <stdlib.h>
#include <sys/wait.h>
void initPortSerie(void);
void child_write_process(void);
void parent_read_process(void);

const char *portTTY = "/dev/ttyS1";
int fd; // File Descriptor

int main()
 {
    pid_t pid;
    printf("\nInitialisation du port série...\n");
    // Initialiser le port série
    initPortSerie();
    // Créer un processus enfant
    pid = fork();
   // if (child_pid == -1) 
    //{ // Une erreur s'est produite
      //  perror("fork");
        //return -1;
    //}

    if (pid == 0)
    { 
        printf("Je suis le processus Fils, j'écrit sur le port série ce que j'entends sur la console (terminal)...\n");
        // Code du processus enfant : écriture des données
      child_write_process();
     // exit(0);
     printf("Fin du Fils\n");
    }
    else
    {
        printf("Je suis le processus Père, j'écrit sur la console (terminal) ce que j'entends sur le port série...\n");  
        // Code du processus parent : lecture des données
        parent_read_process();
        printf("Fin du Processus pere\n");
        wait(NULL);
    }
    close(fd);
}
void initPortSerie(void)
 {
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
    tcflush(fd, TCIFLUSH);  // Discards old data in the rx buffer
}

void parent_read_process(void)
 {
    char read_buffer[32];
    int bytes_read;
    int i = 0;
      while(1)
      {
        bytes_read = read(fd, read_buffer, sizeof(read_buffer)); // Lire les données depuis le port série
        if(bytes_read < 0)
        {
         perror("Erruer de lecture");
         exit(EXIT_FAILURE);
        }
      
        read_buffer[bytes_read] = '\0' ;
	     printf("processus Père: nombre d'octets reçus : %d --> %s\n", bytes_read, read_buffer);
            if(read_buffer[0] == '!')
            {
                break;
            }
      }

                // Afficher chaque caractère
  //              printf("%c", read_buffer[i]);
    //            fflush(stdout);

                    // Terminer le processus enfant
      //              printf("\nFin du Fils\n");
                  //  kill(child_pid, SIGTERM); // Envoyer le signal de terminaison à l'enfant
                    // Attendre que l'enfant se termine
        //            wait(NULL);
              //      close(fd);
                //    return;
}

void child_write_process(void)
 {
     char write_buffer[32];
    char cCharWritten;
    // char cTruc = 0;
    // int bytes_written = 0;
    while(1)
    {
         cCharWritten = getchar();
     if(write(fd, &cCharWritten, 1) <0)
     {
        perror("Erreur d'écriture");
        exit(EXIT_FAILURE);
     }
     if(cCharWritten == 'q')
     {
        break;
     }
    }

    //  fgets(write_buffer, sizeof(write_buffer), stdin);
      //write_buffer[strcspn(write_buffer, "\n")] = 0;
     
      //while (cCharWritten != 'q')
      //{
        //write_buffer[cTruc] = cCharWritten ;
        //cTruc ++ ;
        //cCharWritten = getchar();
        //bytes_written = write(fd, write_buffer, cTruc) ;
      //}
   
    
}