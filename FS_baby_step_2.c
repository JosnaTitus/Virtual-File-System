#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

extern int errno;
int cntFiles = 0, fd = 0, noOfBlocks = 0;
off_t disksize;

//Function declarations
void writeDiskBlock(int, int);                   
void readDiskBlock(int, int);
int findEmptyBlock(int);                                
int isFilePresentOnVD(int, char*);
int Vdcpto(char*, char*);      
int vdls(char*);                             
int VdcpFrom(char*, char*);   

//Entry point function
int main()
{	
	int ch, n;
	char filesystem[20], filedisk[20], buffer[300];
	disksize = 104857600L - 1L;
	fd = open("disk.teasage", O_RDWR | O_CREAT,0664);                     //creating disk of size 100MB	
	if(fd == -1)
	{
		printf("Error: cannot create the grand disk.\n Errorno: %d\n", errno);
		perror("Hard Disk error.");
		exit(1);
	}
	//reserving first block for storing files details
	disksize -= 512;                                    

	//stat command is used to display file or file system status.
	struct stat filestat;
	stat("disk.teasage",&filestat); 
	 
	//calculating empty disk space after storing file details in hard disk       	
	disksize = disksize - filestat.st_size;            
	printf("Choose any option :\n1]vdls\n2]vdcpto(copy into virtual disk from system's hard disk)\n3]vdcpfrom(copy from virtual disk on system's hard disk)\n4]exit\n\n");
	while(1)
	{
		printf("\nEnter your option(number only) : ");
		scanf("%d",&ch);
		//printf("hello");
		switch(ch)
		{
			case 1: 
					n = vdls(buffer);
					printf("%s\n",buffer);
					break;
			case 2: 
					cntFiles=vdls(buffer);                    // calculating number of files in VD
					printf("Enter a file name which present in your system's disk : ");
					scanf("%s",filesystem);
					printf("Enter a file name for copy onto disk : ");
					scanf("%s",filedisk);
					//scanf("%[^\n]",filepath);
					++cntFiles;
					if(Vdcpto(filesystem, filedisk)!=1)
					{
						printf("\nError!!\nErrno=%d\n",errno);
						perror("File Error: ");
						exit(1);
					}
					break;
			case 3: 
					printf("Enter a file name to copy file from vd: ");
					scanf("%s",filedisk);
					//scanf("%[^\n]",filepath);
					printf("Enter a file path to store a file %s on system disk : ",filedisk);
					scanf("%s",filesystem);
					if(VdcpFrom(filesystem, filedisk)!=1)
					{
						printf("\nError!!\nErrno=%d\n",errno);
						perror("File Error: ");
						exit(1);
					}
					break;
			case 4:	
					printf("Exit\n");
					close(fd);
					exit(1);
					break;

			default:printf("\n\nInvalid Choice!!! Please Enter a correct choice : \n");
				break;
		}
	}
}

void writeDiskBlock(int sourcefile, int disk)                   //writting onto disk 
{
	char buff[512];
	int n;
	while((n=read(sourcefile, buff, 512)))
	{
		write(disk,buff,n);
		++noOfBlocks;
		disksize-=n;
	}
}

void readDiskBlock(int disk,int file_copy)
{
	int count=0,n;
	char buff[512];
	while(count <= noOfBlocks )                           //copying only file insted of whole disk
	{
		n=read(disk, buff, 512);
		write(file_copy,buff,n);
		++count;
	}

}

int findEmptyBlock(int disk)                                 //find and returning empty disk block
{
	int cnt;
	cnt=lseek(disk,0,SEEK_END);
	if(cnt<512)
		return 2;
	return ((cnt/512)+((cnt%512)!=0));
}

int isFilePresentOnVD(int disk,char filename[24])
{
	int blk=0;
	char buff[64];
	char *token;
	lseek(disk,0,SEEK_SET);
	for(int i=1; blk <= 512; i++)                                            //checking file is present or not
	{
		read(disk,buff,32);
		token=strtok(buff,"\t");
		if(token!=NULL)
		{
			if(strcmp(filename,token)==0)
			{
				return 1;
			}
		}
		blk+=32;
	}
	return -1;
}

int Vdcpto(char filepath[24],char filename[24])      //storing file details on first block....
{
	int n=0,m,p;
	struct stat filestat;
	stat(filepath,&filestat);                               //getting file details which stores on system disk i.e. input file
	char noOfFileBlocks[20],stblock[20],buff[64],filedetails[50];

	if((p=isFilePresentOnVD(fd,filename))==1)                                         //checking file is already present on VD or not
	{
		errno=17;
		return(errno);
	}

	if(filestat.st_size > disksize)
	{
		errno=27;
		return(errno);
	}

	int sourcefile = open(filepath, O_RDONLY);                  //opening file on system's filesystem
	if (sourcefile ==-1)                                 //some error
	{
		return errno;
	}

	strcpy(filedetails,filename);
	n=(filestat.st_size/512)+((filestat.st_size%512)!=0);                           //calculating number of file blocks
	m=findEmptyBlock(fd);
	sprintf(noOfFileBlocks,"%d", n);
	sprintf(stblock,"%d",m);
	strcat(strcat(filedetails,"\t"),(strcat(strcat(stblock,"\t"),strcat(noOfFileBlocks,"\n"))));//storing all file details in a one char arry
	lseek(fd,0,SEEK_SET);
	for(int i=0 ; i < cntFiles ; i++)
	{
		read(fd,buff,32);	
	}
	write(fd,filedetails,strlen(filedetails));	//writting a file information on the first block of the disk

	lseek(fd,m*512,SEEK_SET);                                  
	writeDiskBlock(sourcefile,fd);
	close(sourcefile);
	return 1;
}

int vdls(char *buf_ptr)                                   //listing file names stored onto VD...
{	
	int n=0,blk=0,j=0;
	char buff[64];
	char *token;
	lseek(fd,0,SEEK_SET);
	for(int i=0;blk<512;i++)
	{
		read(fd,buff,32);
		token=strtok(buff,"\t");
		if(token!=NULL)
		{
			strcat(buf_ptr,strcat(token,", "));                   //storing file names...
			j++;
		}
		blk+=32;
	}
	return j;
}

int VdcpFrom(char filepath[24],char filename[24])                                            //reading from disc
{
	int n,blk=0;
	char buff[64];
	char *token;

	if((n=isFilePresentOnVD(fd,filename)==-1))                                         //checking file is already present on VD or not
	{
		errno=2;
		return(errno);
	}

	lseek(fd,0,SEEK_SET);
	for(int i=0;blk<512;i++)                                            //getting file details
	{                                                                                                
		read(fd,buff,32);
		token=strtok(buff,"\t");
		if(token!=NULL)
			if(strcmp(filename,token)==0)
			{
				break;
			}
		blk+=32;
	}

	int st_block=atoi(strtok(NULL,"\t"));                                    //getting file details....
	noOfBlocks=atoi(strtok(NULL,"\t"));
	lseek(fd,st_block*512,SEEK_SET);
	int fd1 = open(filepath,O_WRONLY | O_CREAT,0664);    //opening file_copy.teasage file...
	if(fd1 == -1)
	{
		return(errno);
	}
	readDiskBlock(fd,fd1);
	return 1;
}

