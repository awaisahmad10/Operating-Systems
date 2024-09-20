#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// define the page and TLB size
const int PAGE_SIZE = 256;
const int TLB_SIZE = 16;

int main(int argc, char *argv[])
{
	// check if 2 arguments were given in the terminal
	if (argc < 2)
	{
		printf("Please provide file as the arugment!\n");
		return 0;
	}

	// open the addresses file for reading
	FILE *fp;
	fp = fopen(argv[1], "r");
	// throw an error if the file doesnt exist
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		return 0;
	}
	
	// open the BACKING_STORE file for reading
	FILE *backing_store;
	backing_store = fopen("BACKING_STORE.bin", "rb");
	// throw an error if the file doesnt exist
	if (backing_store == NULL)
	{
		printf("Error opening binary file!\n");
		return 0;
	}
	// get the fault value from the binary file
	char fault_value[PAGE_SIZE];
	int fval;
	
	// initalize the variables page number, offset, pages etc.
	char *value = NULL;
	size_t len = 0;
	ssize_t read;
	long long page_no, offset, totalhits = 0, fault = 0, pages = 0;

	// helo maintain the queue position
	int qp = 0;
	int physicalad = 0, frame, logicalad;

	// create 2 arrays of integers fro tlb and page table
	int tlb[TLB_SIZE][2];
	int pagetable[PAGE_SIZE];

	memset(tlb, -1, TLB_SIZE * 2 * sizeof(tlb[0][0]));
	memset(pagetable, -1, sizeof(pagetable));

	int mask = 255, mask1 = 62580, i, hit;
	// get the lines of addresses from addresses.txt
	while ((read = getline(&value, &len, fp)) != -1)
	{
		// get total number of addresses
		pages++;
		// get page number
		page_no = atoi(value);
		page_no = page_no >> 8;
		page_no = page_no & mask;

		// offset the address
		offset = atoi(value);
		offset = offset & mask;

		logicalad = atoi(value);
		// printf("%lld %lld\n",page_no,offset);
		frame = 0, physicalad = 0;
		// set to 1 if found in the TLB
		hit = 0;

		// CHECK IN TLB
		for (i = 0; i < TLB_SIZE; i++)
		{
			// check if the page number exists in the TLB
			if (tlb[i][0] == page_no)
			{
				// if it does, set hit to 1 and increment the totalhits counter
				hit = 1;
				totalhits++;
				frame = tlb[i][1];
				break;
			}
		}

		// if hit not present in tlb, search in pagetable
		if (hit != 1)
		{
			int f = 0;
			for (i = 0; i < PAGE_SIZE; i++)
			{
				if (pagetable[i] == page_no)
				{
					// if a page fault occured
					frame = i;
					// increment the counter
					fault++;
					// get the value from the binary file and store it in the memory
					fread(fault_value, sizeof(fault_value), 1, backing_store);
					fval = fault_value[pages];
					break;
				}
				if (pagetable[i] == -1)
				{
					f = 1;

					break;
				}
			}
			if (f == 1)
			{
				pagetable[i] = page_no;
				frame = i;
			}
			// replace in tlb using FIFO
			tlb[qp][0] = page_no;
			tlb[qp][1] = i;
			qp++;
			qp = qp % 15;
		}
		// print the virtual address from addresses.txt
		printf("VIRTUAL ADDRESS: %d ", logicalad);
		// print the physical address of the memory
		physicalad = frame * PAGE_SIZE + offset;
		printf("PHYSICAL ADDRESS: %d ", physicalad);
		// print the value
		printf("Value: %d\n", fval);
	}
	// print total number of addresses
	printf("Number of Translated Addresses = %lld\n", pages);
	// print the page faults
	printf("Page Faults = %lld\n", fault);
	// calculate the fault rate
	double faultrate = (double)fault / pages;
	// print the fault rate
	printf("Page Fault Rate = %.3f\n", faultrate);
	// print the number of hits
	printf("TLB Hits = %lld\n", totalhits);
	// calculate the hitrate
	double hitrate = (double)totalhits / pages;
	// print the hitrate
	printf("TLB HIT RATE = %.3f\n", hitrate);
}
