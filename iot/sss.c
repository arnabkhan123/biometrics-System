#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libfprint/fprint.h>
//#include "serv.c"
//#include "cli.c" 
   #include <string.h>
   #include <sys/socket.h>
   #include <arpa/inet.h>   
   #include <unistd.h>  
//#define TRUE 1
 
#define TRIG 14//raspberry pi 23 pin
#define ECHO 10//raspberry pi 24 pin
 
void setup() {
	    
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
           //printf("..........\n");
}
 
int getCM() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        while(digitalRead(ECHO) == LOW);//{printf("+++++++++++++++++\n");}
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);//{ printf("..........\n");}
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
}
struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
	struct fp_dscv_dev *ddev = discovered_devs[0];
	struct fp_driver *drv;
	if (!ddev)
		return NULL;
	
	drv = fp_dscv_dev_get_driver(ddev);
	//printf("Found device claimed by %s driver\n", fp_driver_get_full_name(drv));
	return ddev;
}

struct fp_print_data *enroll(struct fp_dev *dev) {
	struct fp_print_data *enrolled_print = NULL;
		struct fp_print_data *data;
	int r,f;

	//printf("You will need to successfully scan your finger %d times to "
		//"complete the process.\n", fp_dev_get_nr_enroll_stages(dev));

	do {
		struct fp_img *img = NULL;
	
		sleep(1);
		printf("\nScan your finger now.\n");

		r = fp_enroll_finger_img(dev, &enrolled_print, &img);
		printf("%d\n",r);
		
		//f=verify(dev,data);
	//	printf("verify::::::%d\n",f);
		if (img) {
			fp_img_save_to_file(img, "img.pgm");
			printf("Wrote scanned image to img.pgm\n");
			fp_img_free(img);
		}
		if (r < 0) {
			printf("Enroll failed with error %d\n", r);
			return NULL;
		}

		switch (r) {
		case FP_ENROLL_COMPLETE:
			printf("Enroll complete!\n");
			break;
		case FP_ENROLL_FAIL:
			printf("Enroll failed, something wen't wrong :(\n");
			return NULL;
		case FP_ENROLL_PASS:
			printf("Enroll stage passed. Yay!\n");
			break;
		case FP_ENROLL_RETRY:
			printf("Didn't quite catch that. Please try again.\n");
			break;
		case FP_ENROLL_RETRY_TOO_SHORT:
			printf("Your swipe was too short, please try again.\n");
			break;
		case FP_ENROLL_RETRY_CENTER_FINGER:
			printf("Didn't catch that, please center your finger on the "
				"sensor and try again.\n");
			break;
		case FP_ENROLL_RETRY_REMOVE_FINGER:
			printf("Scan failed, please remove your finger and then try "
				"again.\n");
			break;
		}
	} while (r != FP_ENROLL_COMPLETE);

	if (!enrolled_print) {
		fprintf(stderr, "Enroll complete but no print?\n");
		return NULL;
	}

	printf("Enrollment completed!\n\n");
	return enrolled_print;
}
 int send_image(int socket){

   FILE *picture;
   int size, read_size, stat, packet_index;
   char send_buffer[10240], read_buffer[256];
   packet_index = 1;

   picture = fopen("img.pgm", "r");
   printf("Getting Picture Size\n");   

   if(picture == NULL) {
        printf("Error Opening Image File"); } 

   fseek(picture, 0, SEEK_END);
   size = ftell(picture);
   fseek(picture, 0, SEEK_SET);
   printf("Total Picture size: %i\n",size);

   //Send Picture Size
   printf("Sending Picture Size\n");
   write(socket, (void *)&size, sizeof(int));

   //Send Picture as Byte Array
   printf("Sending Picture as Byte Array\n");

   do { //Read while we get errors that are due to signals.
      stat=read(socket, &read_buffer , 255);
      printf("Bytes read: %i\n",stat);
   } while (stat < 0);

   printf("Received data in socket\n");
   printf("Socket data: %c\n", read_buffer);

   while(!feof(picture)) {
   //while(packet_index = 1){
      //Read from the file into our send buffer
      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

      //Send data through our socket 
      do{
        stat = write(socket, send_buffer, read_size);  
      }while (stat < 0);

      printf("Packet Number: %i\n",packet_index);
      printf("Packet Size Sent: %i\n",read_size);     
      printf(" \n");
      printf(" \n");


      packet_index++;  

      //Zero out our send buffer
      bzero(send_buffer, sizeof(send_buffer));
     }
     return;
    }
int verify(struct fp_dev *dev, struct fp_print_data *data)
{
	int r;

	do {
		struct fp_img *img = NULL;

		sleep(1);
		printf("\nScan your finger now.\n");
		//printf("\nhelloooooo::: %s\n",fp_img.img);
		r = fp_verify_finger_img(dev, data, &img);
		if (img) {
			fp_img_save_to_file(img, "img.pgm");
			printf("Wrote scanned image to img.pgm\n");
			fp_img_free(img);
		}
		if (r < 0) {
			printf("verification failed with error %d :(\n", r);
			return r;
		}
		switch (r) {
		case FP_VERIFY_NO_MATCH:
			printf("NO MATCH!\n");
			return 0;
			
		case FP_VERIFY_MATCH:
			printf("MATCH!\n");
			return 0;
		case FP_VERIFY_RETRY:
			printf("Scan didn't quite work. Please try again.\n");
			break;
		case FP_VERIFY_RETRY_TOO_SHORT:
			printf("Swipe was too short, please try again.\n");
			break;
		case FP_VERIFY_RETRY_CENTER_FINGER:
			printf("Please center your finger on the sensor and try again.\n");
			break;
		case FP_VERIFY_RETRY_REMOVE_FINGER:
			printf("Please remove finger from the sensor and try again.\n");
			break;
		}
	} while (1);
	
}

 
    int main(int argc , char *argv[])
{
	 int socket_desc , new_socket , c, read_size,buffer = 0;
      struct sockaddr_in server , client;
      char *readin;
	        setup();
		while(1)
		{
			delay(1000);
			printf("Distance: %dcm\n", getCM());
			int r1=getCM();
			if(r1<=40)
			{
				
	int p,r = 1;
	struct fp_dscv_dev *ddev;
	struct fp_dscv_dev **discovered_devs;
	struct fp_dev *dev;
	struct fp_print_data *data;

	//printf("This program will enroll your right index finger, "
	//	"unconditionally overwriting any right-index print that was enrolled "
	//	"previously. If you want to continue, press enter, otherwise hit "
	//	"Ctrl+C\n");
	//getchar();

	r = fp_init();
	if (r < 0) {
		fprintf(stderr, "Failed to initialize libfprint\n");
		exit(1);
	}
	fp_set_debug(3);

	discovered_devs = fp_discover_devs();
	if (!discovered_devs) {
		fprintf(stderr, "Could not discover devices\n");
		//goto out;
	}

	ddev = discover_device(discovered_devs);
	if (!ddev) {
		fprintf(stderr, "No devices detected.\n");
		//goto out;
	}

	dev = fp_dev_open(ddev);
	fp_dscv_devs_free(discovered_devs);
	if (!dev) {
		fprintf(stderr, "Could not open device.\n");
		//goto out;
	}

	printf("Opened device. It's now time to enroll your finger.\n\n");
	data = enroll(dev);
	printf("%d",data);
	if (data)
	{
		 socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
         printf("Could not create socket");
      }

      //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons( 8889);

      //Bind
     if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
     {
       puts("bind failed");
       //return 1;
     }

     puts("bind done");

     //Listen
     listen(socket_desc , 3);

      //Accept and incoming connection
      puts("Waiting for incoming connections...");
      c = sizeof(struct sockaddr_in);

     if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c))){
puts("Connection accepted");
         }

    fflush(stdout);

    if (new_socket<0)
    {
      perror("Accept Failed");
      return 1;
    }

    send_image(new_socket);

    close(socket_desc);
    fflush(stdout);
   // continue;
	}
		//goto out_close;
     
	 p= fp_print_data_save(data, RIGHT_INDEX);
	if (p < 0)
		fprintf(stderr, "Data save failed, code %d\n", p);
printf("Print loaded. Time to verify!\n");
	do {
		char buffer[20];

		verify(dev, data);
		printf("Verify again? [Y/n]? ");
		fgets(buffer, sizeof(buffer), stdin);
		if (buffer[0] != '\n' && buffer[0] != 'y' && buffer[0] != 'Y')
			break;
	} while (1);

	fp_print_data_free(data);
out_close:
	fp_dev_close(dev);
out:
	fp_exit();
	//return r;
}

	}
        return 0;
}
