#include "csapp.h"
//#include "csapp.cpp"
#include "stdio.h"
#include <opencv2/opencv.hpp>

using namespace cv;
//void popimage();

int main(int argc, char **argv)
{
	int clientfd, port;
	char *host, *image;
	rio_t rio;
    FILE *fp;
	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <image>\n", argv[0]);
		exit(0);
	}
	host = argv[1];
	port = atoi(argv[2]);
    image= argv[3];
	fp = fopen(image,"rb");
	clientfd = Open_clientfd(host, port);
	//unsigned char array = (char *) malloc(sizeof (char) * 256);
	//array[256] = fp;
	Rio_readinitb(&rio, clientfd);
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
    printf("Size of the file is %u", size);
	Rio_writen(clientfd, &size, sizeof(size));
	fseek(fp, 0, SEEK_SET);
	
	while(1)
	{
        /* First read file in chunks of 256 bytes */
        //printf("%d\n",size );
        unsigned char buff[256];

        //fseek(fp, 0, SEEK_SET);

        int nread = fread(buff,1,256,fp);
        //printf("File read :%d bytes\n", nread);

        /* If read was success, send data. */
        if(nread > 0)
        {
            Rio_writen(clientfd, buff, nread);
            //printf("Sending %d\n",nread);
            // write(clientfd, buff, nread);
        }

        /*
         * There is something tricky going on with read 
         * Either there was error, or we reached end of file.
         */
        if (nread < (256))
        {
            if (feof(fp))
                printf("\nFile Sent\n");
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }

    }
    fclose(fp);



int bytesReceived = 0;
char recvBuff[256];
char processed_image[20];
sprintf(processed_image,"GrayImage.jpg");

FILE *fp1;
fp1 = fopen(processed_image,"wb");

fseek(fp, 0, SEEK_SET);
int recData = 256;

/* Receive data in chunks of 256 bytes untill size data is received*/
    while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && size>0)
    {
        //printf("Bytes received %d\n",bytesReceived);    
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp1);
        // printf("%s \n", recvBuff);
        size -= bytesReceived;
        //printf("remaining bytes :%d\n",size);
        if (size<256)	//if remaining bytes is less than 256, read only remaining bytes of data
        	recData = size;
    }

    fclose(fp1);
    printf("File received\n"); 
    close(clientfd);

    //Mat image, gray_image;
    Mat gray_image = imread( "GrayImage.jpg", CV_LOAD_IMAGE_UNCHANGED );
    namedWindow( "GrayImage.jpg", CV_WINDOW_AUTOSIZE);
    imshow( "GrayImage.jpg", gray_image );
    Mat image1 = imread( argv[3], CV_LOAD_IMAGE_UNCHANGED );
    namedWindow( "image.jpg", CV_WINDOW_AUTOSIZE);
    imshow( "image.jpg", image1 );
    waitKey(3000);
    cvDestroyWindow("GrayImage.jpg");
    cvDestroyWindow("image.jpg");

    exit(0);
    //popimage();
    }

/**************************************************************Diaplaying the processed image on client side******************************************************************************/


    
    

    