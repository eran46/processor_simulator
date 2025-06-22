// Libraries & Files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include "Queue.h"


// ----------- Data Structures ----------->
typedef struct ParsedLine {
	unsigned int opcode;
	unsigned int rd;
	unsigned int rs;
	unsigned int rt;
	int imm;
	char i_format;
}PL;


// ----------- Definitions ----------->
#define MEM_SIZE 4096 // lines
#define MEM_WIDTH 20 // bits
#define REG_NUM 16
#define MEMIN_HEX_LEN 5
#define HALT_DEC 86016
#define HW_REG_NUM 23
#define MAX_HWREGLABEL_LEN 12
#define X_PIXELS 256
#define Y_PIXELS 256
#define DISK_SECTOR_SIZE 128
#define DISK_SECTOR_NUM 128

// opcodes
#define add 0
#define sub 1
#define mul 2
#define and 3
#define or 4
#define xor 5
#define sll 6
#define sra 7
#define srl 8
#define beq 9
#define bne 10
#define blt 11
#define bgt 12
#define ble 13
#define bge 14
#define jal 15
#define lw 16
#define sw 17
#define reti 18
#define in 19
#define out 20
#define halt 21

// registers
#define $zero 0
#define $imm 1
#define $v0 2
#define $a0 3
#define $a1 4
#define $a2 5
#define $a3 6
#define $t0 7
#define $t1 8
#define $t2 9
#define $s0 10
#define $s1 11
#define $s2 12
#define $gp 13
#define $sp 14
#define $ra 15

// Command Line Arguments
#define MEMIN 1
#define DISKIN 2
#define IRQ2IN 3
#define MEMOUT 4
#define REGOUT 5
#define TRACE 6
#define HWREGTRACE 7
#define CYCLES 8
#define LEDS 9
#define DISPLAY7SEG 10
#define DISKOUT 11
#define MONITOR 12
#define MONITOR_YUV 13

// Hardware Registers
#define IRQ0ENABLE     0
#define IRQ1ENABLE     1
#define IRQ2ENABLE     2
#define IRQ0STATUS     3
#define IRQ1STATUS     4
#define IRQ2STATUS     5
#define IRQHANDLER     6
#define IRQRETURN      7
#define CLKS           8
#define TIMERENABLE    11
#define TIMERCURRENT   12
#define TIMERMAX       13
#define DISKCMD        14
#define DISKSECTOR     15
#define DISKBUFFER     16
#define DISKSTATUS     17
#define RESERVED1      18
#define RESERVED2      19
#define MONITORADDR    20
#define MONITORDATA    21
#define MONITORCMD     22

// input: file pointer and memory array pointer
// output: function reads memin to memory
void read_memin_to_memory(FILE* memin, int* memory);

// input: a Hexadecimal representation of a number as a string
// output: returns corresponding int value
int convert_hexStr_to_int(char* hex_str);

// input: ParsedLine pointer and integer representing the instruction line
// output: parsing the instruction in to structure ParsedLine
void parse_line(PL* new, int line);

// input: ParsedLine pointer
// output: returns 1 if immediate instruction, 0 otherwise
int is_I_Format(PL* inst);

// input: an integer and a starting and ending bit (inclusive end)
// output: returns the equivalent integer of the binary representation of the bit range
int extract_bits_range_sum(int sequence, int start, int end);

// input: an integer and a bit index
// output: returns the bit in the index
int extract_bit(int sequence, int bit);

// input: an integer representing the immediate value from memory
// output: returns the number in its 2's compliment representation in bits 0-19
int read_imm(int sequence);

// input: an integer
// output: returns itsnegative 2's compliment representation
int extract_negative_2s_comp(int sequence);

// input: the instruction structure and other relevant parameters
// output: function executes the instruction according to ISA
void execute_instruction(PL* inst, int* mem, int* reg, int* pc, int* pc_updated,
	int* hw_reg, int* hw_updated, int* leds_changed, int* disp_7seg_changed,
	int* disk_action);

// input: *pointer to* file pointer, local path, and reading mode
// output: function opens the file, checks for null file pointer and returns 0 for success,1 otherwise
int open_file_check(FILE** fp, char* file_name, char* mode);

// input: pointer to memout and pointer to memory array
// output: writes the memory to memout
void write_memory_to_memout(FILE* memout, int* memory);

// array of hardware registers
void init_hw_reg(int* hw_reg);

// array of strings of the names of hardware registers
void init_hwreg_labels(char** hwreg_labels);

// updates trace file
void update_trace(FILE* trace, int PC, int inst_dec, int* registers, PL* inst);

// updates hwregtrace file
void update_hwregtrace(FILE* hwregtrace, char* file_name, PL* inst, int* reg, int* hw_reg, int cycle);

// writes content of register array to regout file
void write_registers_to_regout(FILE* regout, int* registers);

// array of registers
void init_registers(int* reg);

// initialize frame buffer matrix to 0
void init_framebuffer(unsigned char framebuffer[][X_PIXELS]);

// update a single pixel in framebuffer
void update_framebuffer(unsigned char fb[][X_PIXELS], unsigned short px_addr, unsigned char px_data);

// copy framebuffer content to monitor file
void write_framebuffer_to_monitor(unsigned char fb[][X_PIXELS], FILE* monitor, FILE* monitor_yuv);

// read irqin file to Queue
void read_irq2in_to_queue(FILE* irq2in, Queue* queue);

// copy diskin to diskout
void hardcopy_diskin_to_diskout(FILE* file1, FILE* file2);

// implementation of DMA
void read_diskoutSector_to_mem(FILE* diskout, int* memory, int disk_sector, int mem_adress);

// implementation of DMA
void write_mem_to_diskoutSector(int* memory, FILE* diskout, int disk_sector, int mem_adress);

// input: a string
// output: function changes the string characters (digits ,special characters or letters) to uppercase.
// digits and special characters are unchanged.
void str_toupper(char* str);