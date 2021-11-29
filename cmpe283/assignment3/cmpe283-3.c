#include <stdio.h>
#include <stdint.h>

void totalExitCountWithReason(int reason)
{
	unsigned a, b, c, d;
	unsigned dreason = reason;
	asm("mov %0,%%ecx \n\t" ::"r"(dreason));
	asm("mov $0x4ffffffd,%eax \n\t");
	asm("cpuid\n");
	asm("mov %%eax, %0\n\t" : "=r"(a));
	asm("mov %%ebx, %0\n\t" : "=r"(b));
	asm("mov %%ecx, %0\n\t" : "=r"(c));
	asm("mov %%edx, %0\n\t" : "=r"(d));

	if (a == 0 && b == 0 && c == 0 && d == 0) {
		printf("Reason %d not Implemented in KVM\n", dreason);
	} else if (a == 0 && b == 0 && c == 0 && d == 0xffffffff) {
		printf("Reason %d not defined in sdm\n", dreason);
	} else {
		printf("Exit Count for the reason  %d is %d \n", dreason, a);
	}
}

void totalExitTimeWithReason(int reason)
{
	unsigned a, b, c, d;
	uint32_t ebx, ecx;
	uint64_t time;
	unsigned dreason = reason;

	asm("mov %0,%%ecx \n\t" ::"r"(dreason));
	asm("mov $0x4ffffffc,%eax \n\t");
	asm("cpuid\n");
	asm("mov %%eax, %0\n\t" : "=r"(a));
	asm("mov %%ebx, %0\n\t" : "=r"(b));
	asm("mov %%ecx, %0\n\t" : "=r"(c));
	asm("mov %%edx, %0\n\t" : "=r"(d));
	ebx = (uint32_t)b;
	ecx = (uint32_t)c;
	time = (uint64_t)ebx << 32 | ecx;
    if (a == 0 && b == 0 && c == 0 && d == 0) {
		printf("Reason %d not Implemented in KVM\n", dreason);
	} else if (a == 0 && b == 0 && c == 0 && d == 0xffffffff) {
		printf("Reason %d not defined in sdm\n", dreason);
	} else {
		printf("time spent by vmm for the reason  %d is %ld cycles\n", dreason, time);
	}
}
void getCpuIDTimewithReason(int reason)
{
	asm("xor %eax , %eax\n\t");
	asm("xor %ebx , %ebx\n\t");
	asm("xor %ecx , %ecx\n\t");
	asm("xor %edx , %edx\n\t");
	totalExitTimeWithReason(reason);
}
void getCpuIDCountwithReason(int reason)
{
	asm("xor %eax , %eax\n\t");
	asm("xor %ebx , %ebx\n\t");
	asm("xor %ecx , %ecx\n\t");
	asm("xor %edx , %edx\n\t");
	totalExitCountWithReason(reason);
}
void totalExitCount()
{
	int b;
	asm("mov $0x4fffffff,%eax \n\t");
	asm("cpuid\n");
	asm("mov %%eax, %0\n\t" : "=r"(b));
	printf("%d\n", b);
}
void getTotalTime()
{
	unsigned b, c;
	uint32_t ebx, ecx;
	uint64_t time;
	asm("mov $0x4FFFFFFE,%eax \n\t");
	asm("cpuid\n");
	asm("mov %%ebx, %0\n\t" : "=r"(b));
	asm("mov %%ecx, %0\n\t" : "=r"(c));
	ebx = (uint32_t)b;
	ecx = (uint32_t)c;
	time = (uint64_t)ebx << 32 | ecx;
	printf("%ld\n", time);
}
void getCpuID()
{
	asm("xor %eax , %eax\n\t");
	asm("xor %ebx , %ebx\n\t");
	asm("xor %ecx , %ecx\n\t");
	asm("xor %edx , %edx\n\t");
	printf("total exit count is ");
	totalExitCount();
	asm("xor %eax , %eax\n\t");
	asm("xor %ebx , %ebx\n\t");
	asm("xor %ecx , %ecx\n\t");
	asm("xor %edx , %edx\n\t");
	printf("total cpu cycles spent in vmexits  ");
	getTotalTime();
	printf("\n");
}
int main(int argc, char *argv[])
{
	int reason = 0;
	if (argc == 1) {
        getCpuID();
	} else if (argc == 2) {
		if (sscanf(argv[1], "%i", &reason) != 1) {
			fprintf(stderr, "argument should be integer");
		} else {
			getCpuIDCountwithReason(reason);
			getCpuIDTimewithReason(reason);
		}
	} else {
		printf("Invalid Amount of Arguments");
	}
}