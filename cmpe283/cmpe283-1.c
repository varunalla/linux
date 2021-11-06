/*  
 *  cmpe283-1.c - Kernel module for CMPE283 assignment 1
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <asm/msr.h>

#define MAX_MSG 80

/*
 * Model specific registers (MSRs) by the module.
 * See SDM volume 4, section 2.1
 */
#define IA32_VMX_PINBASED_CTLS	0x481
#define IA32_VMX_PROCBASED_CTLS  0x482
#define IA32_VMX_PROCBASED_CTLS2  0x48B
#define IA32_VMX_EXIT_CTLS  0x483
#define IA32_VMX_ENTRY_CTLS  0x484
/*
 * struct capability_info
 *
 * Represents a single capability (bit number and description).
 * Used by report_capability to output VMX capabilities.
 */
struct capability_info {
	uint8_t bit;
	const char *name;
};


/*
 * Pinbased capabilities
 * See SDM volume 3, section 24.6.1
 */
struct capability_info pinbased[5] =
{
	{ 0, "External Interrupt Exiting" },
	{ 3, "NMI Exiting" },
	{ 5, "Virtual NMIs" },
	{ 6, "Activate VMX Preemption Timer" },
	{ 7, "Process Posted Interrupts" }
};

struct capability_info procbased[21] =
{
	{2, "Interrupt-window Exiting"},
	{3, "Use TSC Offsetting"},
	{7, "HLT Exiting"},
	{9, "INVLPG Exiting"},
	{10, "MWAIT Exiting"},
	{11, "RDPMC Exiting"},
	{12, "RDTSC Exiting"},
	{15, "CR3-Load Exiting"},
	{16, "CR3-Store Exiting"},
	{19, "CR8-Load Exiting"},
	{20, "CR8-Store Exiting"},
	{21, "Use TPR Shadow"},
	{22, "NMI-Window Exiting"},
	{23, "MOV-DR Exiting"},
	{24, "Unconditional I/O Exiting"},
	{25, "Use I/O bitmaps"},	
	{27, "Monitor trap flag"},
	{28, "use MSR bitmaps"},
	{29, "MONITOR Exiting"},
	{30, "PAUSE Exiting"},                       
	{31, "Activate Secondary Controls"}
};
struct capability_info procbased2[23]=
{	
	{0, "Virtualize APIC Access"},
	{1, "Enable EPT"},
	{2, "Descriptor-table Exiting"},
	{3, "Enable RDTSCP"},
	{4, "Virtualize x2APIC"},
	{5, "Enable VPID"},
	{6, "WBINVD Exiting"},
	{7, "Unrestricted guest"},
	{8, "APIC-Register Virtualization"},
	{9, "Virtual-Interrupt Delivery"},
	{10, "Pause-loop Exiting"},
	{11, "RDRAND Exiting"},
	{12, "Enable INVPCID"},
	{13, "Enable VM Functions"},
	{14, "VMCS shadowing"},
	{15, "Enable ENCLS Exiting"},
	{16, "RDSEED Exiting"},
	{17, "Enable PML"},
	{18, "EPT-Violation #VE"},
	{19, "Conceal VMX Non-root Operation from Intel PT"},
	{20, "Enable XSAVES/XRSTORS"},
	{22, "Mode-based execute control for EPT"},
	{25, "Use TSC Scaling"}

};

struct capability_info exitsctls[11]=
{
	{2, "Save Debug controls"},
	{9, "Host Address Space Size"},
	{12, "Load IA32_PERF_GLOBAL_CTRL"},
	{15, "Acknowledge interrupt on exit"},
	{18, "Save IA32_PAT"},
	{19, "Load IA32_PAT"},
	{20, "Save IA32_EFER"},
	{21, "Load IA32_EFER"},
	{22, "Save VMC-preemption timer value"},
	{23, "Clear IA32_BNDCFGS"},
	{24, "Conceal VM exits from Intel PT"}
};
struct capability_info entryctls[9]=
{
	{2, "Load Debug controls"},
	{9, "IA-32e mode guest"},
	{10, "Entry to SMM"},
	{11, "Deactivate dual-monitor treatment"},
	{13, "Load IA32_PERF_GLOBAL_CTRL"},
	{14, "Load IA32_PAT"},
	{15, "Load IA32_EFER"},
	{16, "Load IA32_BNDCFGS"},
	{17, "Conceal VM entries from Intel PT"}
};

/*
 * report_capability
 *
 * Reports capabilities present in 'cap' using the corresponding MSR values
 * provided in 'lo' and 'hi'.
 *
 * Parameters:
 *  cap: capability_info structure for this feature
 *  len: number of entries in 'cap'
 *  lo: low 32 bits of capability MSR value describing this feature
 *  hi: high 32 bits of capability MSR value describing this feature
 */
void report_capability(struct capability_info *cap, uint8_t len, uint32_t lo, uint32_t hi)
{
	uint8_t i;
	struct capability_info *c;
	char msg[MAX_MSG];

	memset(msg, 0, sizeof(msg));

	for (i = 0; i < len; i++) {
		c = &cap[i];
		snprintf(msg, 79, "  %s: Can set=%s, Can clear=%s\n",
		    c->name,
		    (hi & (1 << c->bit)) ? "Yes" : "No",
		    !(lo & (1 << c->bit)) ? "Yes" : "No");
		printk(msg);
	}
}

/*
 * detect_vmx_features
 *
 * Detects and prints VMX capabilities of this host's CPU.
 */
void
detect_vmx_features(void)
{
	uint32_t lo, hi;
	bool isSecondaryActive=false;
	/* Pinbased controls */
	rdmsr(IA32_VMX_PINBASED_CTLS, lo, hi);
	pr_info("Pinbased Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(pinbased, 5, lo, hi);
	
	/*Procbased Controls*/
	rdmsr(IA32_VMX_PROCBASED_CTLS,lo,hi);
	pr_info("Proc Based Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(procbased, 21, lo, hi);

	isSecondaryActive=(hi>>31&1);
	/*pr_info("secondary Proc Based Controls: %s",isSecondaryActive?"true":"false");*/
	if(isSecondaryActive){
		/*Secondary Procbased Controls*/
		rdmsr(IA32_VMX_PROCBASED_CTLS2,lo,hi);
		pr_info("Secondary Proc Based Controls MSR: 0x%llx\n",
			(uint64_t)(lo | (uint64_t)hi << 32));
		report_capability(procbased2,23,lo,hi);
	}
	/*VM-Exit based Controls*/
	rdmsr(IA32_VMX_EXIT_CTLS,lo,hi);
	pr_info("VM-Exit Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
        report_capability(exitsctls, 11, lo, hi);
	/*VM-Exit based Controls*/
	rdmsr(IA32_VMX_ENTRY_CTLS,lo,hi);
	pr_info("VM-Entry Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(entryctls, 9, lo, hi);
}

/*
 * init_module
 *
 * Module entry point
 *
 * Return Values:
 *  Always 0
 */
int
init_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Start\n");

	detect_vmx_features();

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

/*
 * cleanup_module
 *
 * Function called on module unload
 */
void
cleanup_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Exits\n");
}
MODULE_LICENSE("GPL v2");
