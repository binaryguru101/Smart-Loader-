#include "loader.h"
#include<signal.h>
#include<math.h>
#include <stdbool.h>

#define ELF_NUMBER 0x7f

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * Curr_Segment; 

int fd=-1;

int (*_address)();

int **PHDR_Table;


int phsize=sizeof(Elf32_Phdr);
void *buffer[1000];
int ind =0;

void SIGSEVG_HANDLER(int signal, siginfo_t *info, void *state);

void setup() {
    
    PHDR_Table = malloc(1000 * sizeof(int *));
    for (int i = 0; i < 1000; i++) {
        PHDR_Table[i] = malloc(2 * sizeof(int));
    }

}
void cleanup() {
    
    for (int i = 0; i < 1000; i++) {
        free(PHDR_Table[i]);
    }
    free(PHDR_Table);
}

void safe_lseek(int fd, off_t offset, int whence) {
    if (lseek(fd, offset, whence) == -1) {
        perror("Error going to start of the file");
        exit(1); 
    }
}

void load_and_run_elf(char **exe)
{
	fd = open(exe[1], O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open file");
		loader_cleanup();
		exit(1);
	}


	ehdr = malloc(sizeof(Elf32_Ehdr));
	int bytesRead = read(fd, ehdr, sizeof(Elf32_Ehdr));
	if (bytesRead != sizeof(Elf32_Ehdr)) {
    perror("Error in reading ELF header");
    loader_cleanup();
    exit(1);
    }

    if(ehdr->e_ident[0]!=ELF_NUMBER){
        perror("NOT a Valid ELF FILE");
        loader_cleanup();
        exit(1);
    } 


	printf("\nEntry Point Address: %p\n\n", (void *)ehdr->e_entry);
	int (*start)() = (int (*)())(ehdr->e_entry);
	int result = start();
	printf("\nUser return value = %d\n", result);
	
	
}


int Max_Mem = 0;
int Counter = 0;
int Internal_Frag = 0;
void *Vir_Mem;
int P_Alloc = 0;


int main(int argc, char **exe)

{
	setup();
	if (argc != 2)
	{
		printf("Incorrect input format pls enter \"./loader sum\"");
		exit(1);
	}

	struct sigaction SIGNAL_HANDLER;
	SIGNAL_HANDLER.sa_flags = SA_SIGINFO;
	SIGNAL_HANDLER.sa_sigaction = SIGSEVG_HANDLER;

	if (sigaction(SIGSEGV, &SIGNAL_HANDLER, NULL) == -1)
	{
		perror("error with sigsegv fault");
		return 1;
	}

	load_and_run_elf(exe);
	loader_cleanup();

	printf("Total Page-Faults -->%d\n",Counter);
	printf("Total Internal Fragmentation -->%f KB\n",(float)Internal_Frag/1000.000);
	printf("Total Pages -->%d\n",P_Alloc);
	printf("----------------------------\n");

	cleanup();
	return 0;
}

void loader_cleanup() 
{
	if (ehdr != NULL) 
	{
		free(ehdr);
		ehdr = NULL;
	}
	if (phdr != NULL) 
	{
		free(phdr);
		phdr = NULL;
	}

    if(Curr_Segment!=NULL){
        free(Curr_Segment);
    }

    for(int i = 0; i < Counter; i ++){
        munmap(buffer[i],4096);
    }
	if (fd != -1) 
	{
		close(fd);
		fd = -1;
	}

}

int Off_Entr;
void *Last_Addr;
int Off_size;

bool adr_checker(uintptr_t fault_addr, Elf32_Phdr* phdr) {
    return (phdr->p_vaddr <= fault_addr && fault_addr < phdr->p_vaddr + phdr->p_memsz);
}

void SIGSEVG_HANDLER(int signal, siginfo_t *info, void *context){
    safe_lseek(fd,ehdr->e_phoff,SEEK_SET);

    printf("_________________________________________\n");
    printf("SEGMENTATION FAULT AT %p\n",(void*)info->si_addr);
    printf("_________________________________________\n");
    printf("\n");


    Curr_Segment=NULL;

    for(int i=0;i<ehdr->e_phnum;i++){
        phdr=malloc(ehdr->e_phnum*phsize);
        if(!phdr){
        printf("Failed to allocate");
        close(fd);
        loader_cleanup();
        exit(1);
      }
        int buff=read(fd,phdr,phsize);

        if(buff==-1){
            printf("ERROR READING");
            close(fd);
            loader_cleanup();
            exit(1);
        }


        if (adr_checker((uintptr_t)info->si_addr, phdr)) {
            Curr_Segment = phdr;
            printf("SEGMENT START ADDRESS : %p\n", (void*)Curr_Segment->p_vaddr);
        }
    }
        if(Curr_Segment){
            int Palloc_Size=Curr_Segment->p_memsz/4096+1;
            
            for(int i=0;i<Palloc_Size;i++){
                if(i==Palloc_Size-1){
                    Internal_Frag+=(4096-(Curr_Segment->p_memsz)%4096);
                }

                int start=Curr_Segment->p_vaddr+i*4096;
                int end=start+4096;


                if ((uintptr_t)info -> si_addr >= start && (uintptr_t) info -> si_addr < end){
                    Vir_Mem = mmap((void*)start, 4096 , PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE, fd, Curr_Segment -> p_offset);
                    buffer[Counter] = Vir_Mem;

                    printf("Total Segment Size-->%fKB\n",Curr_Segment->p_memsz/1000.0);
                    printf("TOTAL PAGE SIZE --> %d\n",end-start);
                    printf("Virtual Memory Adress--> %p\n",Vir_Mem);
                    if (Vir_Mem == MAP_FAILED) {
                          perror("mmap failed");
                          exit(1);
                      }
                    Counter++;
                    P_Alloc++;
                    break;
                }  
            }
            
        }
        printf("\n");
    }
    
    


