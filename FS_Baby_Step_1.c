#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define BUFSIZE 512

extern int errno;
int noOfBlocks = 0;

//Function declarations
void readDiskBlock(int,int);
void writeDiskBlock(int,int);
void getFileFromDisk();
void storeFileOntoDisk(char*);

//entry point function
int main()
{
    off_t disksize = 10485760L - 1L;
    int fd = open("disk.teasage", O_RDWR | O_CREAT,0777);
    char fname[20];
    char ch;
    if(fd ==-1)
    {
        printf("Error: cannot create the grand disk.\n Errorno: %d\n", errno);
        perror("Open error.");
        exit(1);
    }
	printf("\nCongratulations!\nYour brand new harddisk is created fresh just now!! Enjoy!!!\n\n");
	close(fd);

	printf("Enter filename which you want to store on your virtual disk :\n");
	scanf("%s",fname);
	storeFileOntoDisk(fname);

	printf("Do you want file from your virtual disk ?[y/n] :\n");
	getchar();
	scanf("%c",&ch);

	if(ch == 'y' || ch == 'Y')
		getFileFromDisk();
	else
		printf("Thank You..!\n");
    return 0;
}

//Store the file on virtual disk
void storeFileOntoDisk(char fname[20])
{	int sourcefile = open(fname,O_RDONLY);
	if(sourcefile==-1)
	{
		perror("SOURCE FILE ERROR");
		exit(1);
    }
    int mydisk = open("disk.teasage",O_RDWR, 0777);
    if(mydisk == -1)
    {
    	perror("DESTINATION FILE ERROR");
        exit(1);
    }
    else
    {
    	writeDiskBlock(sourcefile,mydisk);
    	printf("Write Done..!\n");
    }
}


//Get the copy of file which is stored on virtual disk
void getFileFromDisk()
{
	int mydisk = open("disk.teasage",O_RDONLY,0777);
	if (mydisk == -1)
	{
		perror("Disk Open error.");
		exit(1);
	}

	int fcopy=open("file_copy.teasage",O_WRONLY | O_CREAT,0777);
	if(fcopy == -1)
	{
		perror("File open error.");
		exit(1);
	}
	else
	{
		readDiskBlock(fcopy,mydisk);
		printf("Copy Done..!\n");
		printf("Name of copied file : file_copy.teasage\n");
	}
}

//Write the file
void writeDiskBlock(int sourcefile,int mydisk)
{
	char buf[BUFSIZE];
	int n = 0;
	while((n = read(sourcefile,buf,BUFSIZE)) > 0)
	{
		write(mydisk, buf, n);
		++noOfBlocks;
	}
	close(sourcefile);
	close(mydisk);
}

//Read the file
void readDiskBlock(int fcopy,int mydisk) 
{
	char buf[BUFSIZE];
	int cnt = 0, n = 0;
	while(cnt <= noOfBlocks)
	{
		n = read(mydisk, buf, BUFSIZE);
		write(fcopy,buf,n);
		++cnt;
	}
	close(fcopy);
	close(mydisk);   
}
