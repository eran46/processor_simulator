#define _CRT_SECURE_NO_WARNINGS
#include "functions.h"


void read_memin_to_memory(FILE* memin, int* memory) {
    char* str = (char*)malloc(sizeof(char) * (MEMIN_HEX_LEN + 2));
    if (str == NULL) {
        printf("error allocating memory for memin string.");
        exit(1);
    }
    int line = 0, val;
    while (fgets(str, MEMIN_HEX_LEN + +2, memin) != NULL && line < MEM_SIZE) {
        val = convert_hexStr_to_int(str);
        if (extract_bit(val, MEM_WIDTH - 1) == 1) {
            val = val | 0xFFF00000;
        }
        memory[line] = val;
        line++;
    }
    while (line < MEM_SIZE) {
        memory[line] = 0;
        line++;
    }
    free(str);
}

int convert_hexStr_to_int(char* hex_str) {
    int digit = MEMIN_HEX_LEN - 1, sum = 0, value = 0;
    
    while (digit >= 0) {
        // determin hex letter decimal value
        if (*(hex_str + digit) >= 48 && *(hex_str + digit) <= 57) { // 0-9
            value = *(hex_str + digit) - 48;
        }
        else if (*(hex_str + digit) >= 65 && *(hex_str + digit) <= 70) { // A-F
            value = (*(hex_str + digit) - 65) + 10;
        }
        else if (*(hex_str + digit) >= 97 && *(hex_str + digit) <= 122) { // A-F
            value = (*(hex_str + digit) - 97) + 10;
        }
        else {
            printf("non-Hexadecimal charactar in memin\n");
            return -1;
        }
        sum += (1 << (4 * (MEMIN_HEX_LEN - digit - 1))) * value;
        digit--;

    }
    return sum;
}

void parse_line(PL* new, int line) {
    new->opcode = extract_bits_range_sum(line, 12, 19);
    new->rd = extract_bits_range_sum(line, 8, 11);
    new->rs = extract_bits_range_sum(line, 4, 7);
    new->rt = extract_bits_range_sum(line, 0, 3);
}

int is_I_Format(PL* inst) {
    if (inst->rs == 1 || inst->rt == 1 || inst->rd == 1) {
        return 1;
    }
    return 0;
}

// inclusive range
int extract_bits_range_sum(int sequence, int start, int end) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += extract_bit(sequence, i) * (1 << (i - start));
    }
    return sum;
}

// bits indexed from 0
int extract_bit(int sequence, int bit) {
    int mask = 1;
    sequence = sequence >> bit;
    return sequence &= mask;
}

// inst->imm will contain sign extended 32bit representation of immediate
int read_imm(int sequence) {
    int msb = extract_bit(sequence, MEM_WIDTH - 1);
    if (msb) {  // if negative
        return extract_negative_2s_comp(sequence);
    }
    return extract_bits_range_sum(sequence, 0, MEM_WIDTH - 2);
}

int extract_negative_2s_comp(int sequence) {
    sequence = sequence | 0xFFF00000;
    return sequence;
}

void apply_immediate_to_instruction(PL* inst) {
    if (inst->rd == 1)
        inst->rd = inst->imm;
    if (inst->rs == 1)
        inst->rs = inst->imm;
    if (inst->rt == 1)
        inst->rt = inst->imm;
}

void execute_instruction(PL * inst, int* mem, int* reg, int* pc, int* pc_updated,
    int* hw_reg, int* hw_updated, int* leds_changed, int* disp_7seg_changed,
    int* disk_action) {
    int reg_rd_val = reg[inst->rd], reg_rs_val = reg[inst->rs],
        reg_rt_val = reg[inst->rt];
    if(inst->i_format){
        if (inst->rd == 1) {
            reg_rd_val = inst->imm;
        }
        if (inst->rs == 1) {
            reg_rs_val = inst->imm;
        }
        if (inst->rt == 1) {
            reg_rt_val = inst->imm;
        }
    }

    switch (inst->opcode) {
    case add:
        reg[inst->rd] = reg_rs_val + reg_rt_val;
        break;
    case sub:
        reg[inst->rd] = reg_rs_val - reg_rt_val;
        break;
    case mul:
        reg[inst->rd] = reg_rs_val * reg_rt_val;
        break;
    case and :
        reg[inst->rd] = reg_rs_val & reg_rt_val;
        break;
    case or :
        reg[inst->rd] = reg_rs_val | reg_rt_val;
        break;
    case xor :
        reg[inst->rd] = reg_rs_val ^ reg_rt_val;
        break;
    case sll:
        reg[inst->rd] = reg_rs_val << reg_rt_val;
        break;
    case sra:
        reg[inst->rd] = reg_rs_val >> reg_rt_val;
        break;
    case srl: // logical shift !
        // >> implementation on C is Arithmetic -> must cast to unsigned
        reg[inst->rd] = (int)((unsigned int)reg_rs_val >> reg_rt_val);
        break;
    case beq:
        if (reg_rs_val == reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case bne:
        if (reg_rs_val != reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case blt:
        if (reg_rs_val < reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case bgt:
        if (reg_rs_val > reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case ble:
        if (reg_rs_val <= reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case bge:
        if (reg_rs_val >= reg_rt_val) {
            *pc = reg_rd_val;
            *pc_updated = 1;
        }
        break;
    case jal:
        reg[inst->rd] = *pc + 1;
        *pc = reg_rs_val;
        *pc_updated = 1;
        break;
    case lw: // with sign extension
        reg[inst->rd] = mem[reg_rs_val + reg_rt_val];
        break;
    case sw:
        mem[reg_rs_val + reg_rt_val] = reg_rd_val;
        break;
    case reti:
        *pc = hw_reg[7];
        *pc_updated = 1;
        *hw_updated = 1;
        break;
    case in:
        reg[inst->rd] = hw_reg[reg_rs_val + reg_rt_val];
        if (reg_rs_val + reg_rt_val == MONITORCMD) {
            reg[inst->rd] = 0;
        }
        *hw_updated = 1;
        break;

    case out:

        switch (reg_rs_val + reg_rt_val) {
        case LEDS:
            *leds_changed = (hw_reg[LEDS] != reg_rd_val);
            break;
        case DISPLAY7SEG:
            *disp_7seg_changed = (hw_reg[DISPLAY7SEG] != reg_rd_val);
            break;
        case DISKCMD:
            if (reg_rd_val == 1 || reg_rd_val == 2)
                *disk_action = 1;
        }

        hw_reg[reg_rs_val + reg_rt_val] = reg_rd_val;
        *hw_updated = 1;
        break;

    default:
        printf("Unknown opcode %d\n", inst->opcode);
        break;
    }
}

void init_registers(int* reg) {
    for (int i = 0; i < REG_NUM; i++) {
        reg[i] = 0;
    }
}

void close_all_files(FILE* memin, FILE* diskin, FILE* irq2in, FILE* memout, FILE* regout,
    FILE* trace, FILE* hwregtrace, FILE* cycles, FILE* leds, FILE* display7seg,
    FILE* diskout, FILE* monitor, FILE* mon_yuv) {
    fclose(memin);
    if (memin) fclose(memin);
    if (diskin) fclose(diskin);
    if (irq2in) fclose(irq2in);
    if (memout) fclose(memout);
    if (regout) fclose(regout);
    if (trace) fclose(trace);
    if (hwregtrace) fclose(hwregtrace);
    if (cycles) fclose(cycles);
    if (leds) fclose(leds);
    if (display7seg) fclose(display7seg);
    if (diskout) fclose(diskout);
    if (monitor) fclose(monitor);
    if (mon_yuv) fclose(mon_yuv);
}

// takes pointer to file pointer
int open_file_check(FILE** fp, char* file_name, char* mode) {
    fopen_s(fp, file_name, mode);
    if (*fp == NULL) {
        printf("error opening %s\n", file_name);
        return 1;
    }
    return 0;
}

void write_memory_to_memout(FILE* memout, int* memory) {
    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(memout, "%05X\n", memory[i] & 0xFFFFF);
    }
}

void write_registers_to_regout(FILE* regout, int* registers) {
    for (int i = 2; i < REG_NUM; i++) {
        fprintf(regout, "%.8X\n", registers[i]);
    }
}

void update_trace(FILE* trace, int PC,int inst_dec,int* registers, PL* inst) {
    int imm_content = registers[1]; // = 0
    if (inst->i_format) {
        imm_content = inst->imm;
    }
    fprintf(trace,
        "%.3X %.5X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X %.8X\n",
        PC, inst_dec, registers[0], imm_content, registers[2],
        registers[3], registers[4], registers[5], registers[6],
        registers[7], registers[8], registers[9], registers[10],
        registers[11], registers[12], registers[13], registers[14],
        registers[15]);
}

void init_hw_reg(int* hw_reg) {
    for (int i = 0; i < HW_REG_NUM; i++) {
        hw_reg[i] = 0;
    }
}

void update_hwregtrace(FILE* hwregtrace, char* file_name, PL* inst, int* reg, int* hw_reg, int cycle) {
    char* mode_label = "READ";
    char hwreg_label[MAX_HWREGLABEL_LEN + 1];

    char* hwreg_labels[HW_REG_NUM];
    int data = 0;
    init_hwreg_labels(hwreg_labels);
    int reg_rd_val = reg[inst->rd], reg_rs_val = reg[inst->rs],
        reg_rt_val = reg[inst->rt];
    if (inst->i_format) {
        if (inst->rd == 1) {
            reg_rd_val = inst->imm;
        }
        if (inst->rs == 1) {
            reg_rs_val = inst->imm;
        }
        if (inst->rt == 1) {
            reg_rt_val = inst->imm;
        }
    }
    switch (inst->opcode) {
    case reti:
        data = hw_reg[7];
        strcpy_s(hwreg_label, MAX_HWREGLABEL_LEN + 1, hwreg_labels[7]);
        break;
    case in:
        data = hw_reg[reg_rs_val + reg_rt_val];
        strcpy_s(hwreg_label, MAX_HWREGLABEL_LEN + 1, hwreg_labels[reg_rs_val + reg_rt_val]);
        break;
    case out:
        data = reg_rd_val;
        mode_label = "WRITE";
        strcpy_s(hwreg_label, MAX_HWREGLABEL_LEN + 1, hwreg_labels[reg_rs_val + reg_rt_val]);
        break;
    }
    fprintf(hwregtrace, "%d %s %s %.5X\n", cycle, mode_label, hwreg_label, data);
}

void init_hwreg_labels(char** hwreg_labels) {
    hwreg_labels[0] = "irq0enable";
    hwreg_labels[1] = "irq1enable";
    hwreg_labels[2] = "irq2enable";
    hwreg_labels[3] = "irq0status";
    hwreg_labels[4] = "irq1status";
    hwreg_labels[5] = "irq2status";
    hwreg_labels[6] = "irqhandler";
    hwreg_labels[7] = "irqreturn";
    hwreg_labels[8] = "clks";
    hwreg_labels[9] = "leds";
    hwreg_labels[10] = "display7seg";
    hwreg_labels[11] = "timerenable";
    hwreg_labels[12] = "timercurrent";
    hwreg_labels[13] = "timermax";
    hwreg_labels[14] = "diskcmd";
    hwreg_labels[15] = "disksector";
    hwreg_labels[16] = "diskbuffer";
    hwreg_labels[17] = "diskstatus";
    hwreg_labels[18] = "reserved";
    hwreg_labels[19] = "reserved";
    hwreg_labels[20] = "monitoraddr";
    hwreg_labels[21] = "monitordata";
    hwreg_labels[22] = "monitorcmd";
}

void init_framebuffer(unsigned char framebuffer[][X_PIXELS]) {
    for (int y = 0; y < Y_PIXELS; y++) {
        for (int x = 0; x < X_PIXELS; x++) {
            framebuffer[y][x] = 0;
        }
    }
}

void update_framebuffer(unsigned char fb[][X_PIXELS], unsigned short px_addr,
    unsigned char px_data){
    int y = px_addr/X_PIXELS, x = px_addr%X_PIXELS;
    fb[y][x] = px_data;
}

void write_framebuffer_to_monitor(unsigned char fb[][X_PIXELS], FILE* monitor, FILE* monitor_yuv) {
    for (int y = 0; y < Y_PIXELS; y++) {
        for (int x = 0; x < X_PIXELS; x++) {
            unsigned char data = fb[y][x];
            fwrite(&data, sizeof(unsigned char), 1, monitor_yuv);
            fprintf_s(monitor, "%.2X\n", (int)data);
        }
    }
}

void read_irq2in_to_queue(FILE* irq2in, Queue* q) {
    int irq2_status = 1, data = 0;
    while (irq2_status = fscanf_s(irq2in, "%d\n", &data) != EOF) {
        enqueue(q, data);
    }
}

void hardcopy_diskin_to_diskout(FILE* file1, FILE* file2) {
    char str[MEMIN_HEX_LEN + 1]; // +1 for null terminator
    str[5] = '\0';
    int line_counter = 0;

    while (fscanf(file1, "%5s", str) != EOF) {
        str_toupper(str);  // Convert the string to uppercase
        fprintf(file2, "%s\n", str);
        line_counter++;
    }

    while (line_counter < 16384) { // fill rest of lines "00000\n"
        fprintf(file2, "00000\n");
        line_counter++;
    }
}

void str_toupper(char* str) {
    while (*str != '\0') {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void read_diskoutSector_to_mem(FILE* diskout,int* memory,int disk_sector, int mem_adress) {
    int line, data, mem_offset = 0;
    line = DISK_SECTOR_SIZE * disk_sector;
    fseek(diskout, (MEMIN_HEX_LEN + 2) * line, SEEK_SET); // seek start of sector

    char* str = (char*)malloc(sizeof(char) * (MEMIN_HEX_LEN + 1));
    str[5] = '\0';
    for (int i = 0; i < DISK_SECTOR_SIZE; i++) { // read sector
        fscanf(diskout, "%5s", str);
        data = convert_hexStr_to_int(str);
        memory[mem_adress + i] = data;
    }
}

void write_mem_to_diskoutSector(int* memory, FILE* diskout, int disk_sector, int mem_adress) {
    int line = DISK_SECTOR_SIZE * disk_sector, mem_offset = 0;
    fseek(diskout, (MEMIN_HEX_LEN + 2) * line, SEEK_SET); // seek start of sector

    for (int i = 0; i < DISK_SECTOR_SIZE; i++) { // overwrite sector
        if (i == DISK_SECTOR_SIZE - 1)
            fprintf(diskout, "%.5X", memory[mem_adress + i]);
        else
            fprintf(diskout, "%.5X\n", memory[mem_adress + i]);
    }
}