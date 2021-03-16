								Virtual File System

In both project, I have made a virtual disk i.e; it works like a hard disk.
In this project i have used system calls only.

For FS_Baby_Step_1:-
After you run the code the disk is created and then it ask the user if they want to store any file on disk. If yes, a copy of it is made on the new disk. 

Commands for running the file-
			
	gcc FS_Baby_Step_1.c -o myexe
	./myexe

For FS_Baby_Step_2:-
After you run the code the disk is created and then it ask the user for different options like to store data from actual disk to virtual disk and vice versa. To perform listing operation like ls command in Linux.

Commands for running the file:

	gcc FS_baby_step_2.c -o myfile
	./myfile
	
Note:- I have reserved 512bytes for each file, just for my comfort. it can be changed according to the requirement.
