#include<stdio.h>
#include<stdint.h>

void totalExitCount()
{
        int b;
        asm("mov $0x4fffffff,%eax \n\t");
        asm("cpuid\n");
        asm("mov %%eax, %0\n\t":"=r" (b));
        printf("%d\n",b);
        
}
void getTotalTime(){
        int32_t ebx,ecx;
        int64_t result;
        asm("mov $0x4FFFFFFE,%eax \n\t");
        asm("cpuid\n");
        asm("mov %%ebx, %0\n\t":"=r" (ebx));
        asm("mov %%ecx, %0\n\t":"=r" (ecx));
        result=((int64_t)ebx)<<32|ecx;
        printf("%ld\n",result);
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

int main(){
            getCpuID();
}