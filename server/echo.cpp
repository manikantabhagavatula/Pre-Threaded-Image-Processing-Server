#include "csapp.h"
#include <opencv2/opencv.hpp>

void process();

void echo(int connfd) 
{
    //size_t n;
    int bytesReceived = 0; 
    char recvBuff[256]; 
    rio_t rio;
    Rio_readinitb(&rio, connfd);
    //Mat image, gray_image;
 
    //receive size of image
    int size;
    Rio_readnb(&rio,&size,sizeof(size));
    printf("Size of file %d\n",size );

    //receive process of image
    char client_image[20];
    sprintf(client_image,"CopiedImage.jpg");

    FILE *fp;
    fp = fopen(client_image, "wb");

    if(NULL == fp)
    {
        printf("Error opening file");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    int recData = 256;

    /* Receive data in chunks of 256 bytes untill size data is received*/
    while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && size>0)
    {
        //printf("Bytes received %d\n",bytesReceived);    
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        // printf("%s \n", recvBuff);
        size -= bytesReceived;
        //printf("remaining bytes :%d\n",size);
        if (size<256)	//if remaining bytes is less than 256, read only remaining bytes of data
        	recData = size;
    }

    fclose(fp);
    printf("File received\n");

    


/*******************************************************************OPEN CV FUNCTIONALITY ***********************************************************/
 process();

 //namedWindow( "Gray image", WINDOW_AUTOSIZE);
 //imshow( "Gray image", gray_image );
// waitKey(0);
// cvDestroyWindow("Gray image");
// remove("CopiedImage.jpg");
// remove("Gray_Image.jpg");
 

//int clientfd;
FILE *fp1;
fp1 = fopen("Gray_Image.jpg", "rb");
while(1)
    {
        
        unsigned char server_buff[256];

        int nread = fread(server_buff,1,256,fp1);
        //printf("Reading File :%d bytes\n", nread);

        /* If read was success, send data. */
        if(nread > 0)
        {
            Rio_writen(connfd, server_buff, nread);
            //printf("Sending %d\n",nread);
            // write(clientfd, server_buff, nread);
        }

        /*
         * There is something tricky going on with read 
         * Either there was error, or we reached end of file.
         */
        if (nread < (256))
        {
            if (feof(fp1))
                printf("File Sent\n");
            if (ferror(fp1))
                printf("Error reading\n");
            break;
        }

    }
    fclose(fp1);
    remove("CopiedImage.jpg");
    remove("Gray_Image.jpg");

 }