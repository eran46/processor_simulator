#define _CRT_SECURE_NO_WARNINGS
#include "functions.h"


int main(int argc, char* argv[]) {
	// ----------- local variables ----------->
	int PC = 0, PC_updated = 0, hw_updated = 0, leds_changed = 0, disp_7seg_changed = 0,
		disk_action = 0, disk_timer=0, interrupt = 0;
	/* Error 1 = general, -1 no irq2in, -2 no diskin, -3 no both, */
	int Error = 0;

	// ----------- initialization ----------->
	int* memory = (int*)malloc(sizeof(int) * MEM_SIZE);
	if (memory == NULL) return 1;

	int* registers = (int*)malloc(sizeof(int) * REG_NUM);
	if (registers == NULL) {
		printf("Error while allocating memory.");
		free(memory);
		return 1;
	}
	init_registers(registers);

	PL* instruction = (PL*)malloc(sizeof(PL));
	if (instruction == NULL) {
		printf("Error while allocating ParsedLine.\n");
		free(memory);
		free(registers);
		return 1;
	}

	int* hw_registers = (int*)malloc(sizeof(int) * HW_REG_NUM);
	if (hw_registers == NULL) {
		printf("Error while allocating hardware registers.\n");
		free(memory);
		free(registers);
		free(instruction);
		return 1;
	}
	init_hw_reg(hw_registers);

	unsigned char framebuffer[Y_PIXELS][X_PIXELS];
	init_framebuffer(framebuffer);

	Queue* queue = createQueue();


	// ----------- opening files ----------->
	FILE *memin = NULL, *irq2in = NULL, *memout = NULL,
		*regout = NULL, *trace = NULL, *diskin = NULL, *hwregtrace = NULL, *cycles = NULL,
		*leds = NULL, *display7seg = NULL, *diskout = NULL, *monitor = NULL,
		*monitor_yuv = NULL;

	open_file_check(&memin, argv[MEMIN], "r+");
	open_file_check(&trace, argv[TRACE], "w+");
	open_file_check(&hwregtrace, argv[HWREGTRACE], "w+");
	if (open_file_check(&irq2in, argv[IRQ2IN], "r+")) {
		Error = -1;
	}
	if (open_file_check(&diskin, argv[DISKIN], "r+")) {
		switch (Error) {
		case -1:
			Error = -3;
			break;
		case 0:
			Error = -2;
			break;
		default:
			break;
		}
	}
	open_file_check(&diskout, argv[DISKOUT], "w+");
	open_file_check(&leds, argv[LEDS], "w+");
	open_file_check(&display7seg, argv[DISPLAY7SEG], "w+");
	switch (Error) {
	case -1:
		hardcopy_diskin_to_diskout(diskin, diskout);
		fclose(diskin);
		break;
	case -2:
		read_irq2in_to_queue(irq2in, queue);
		fclose(irq2in);
		break;
	case -3:
		break;
	default:
		hardcopy_diskin_to_diskout(diskin, diskout);
		fclose(diskin);
		read_irq2in_to_queue(irq2in, queue);
		fclose(irq2in);
		break;
	}
	read_memin_to_memory(memin, memory);
	fclose(memin);



	// ----------- fetch-decode-execute ----------->
	while (memory[PC] != HALT_DEC && PC < MEM_SIZE) {
		// IRQ2IN
		if (!isQueueEmpty(queue) && peek(queue) == hw_registers[CLKS]) {
			hw_registers[IRQ2STATUS] = 1;
			dequeue(queue);
		}

		// INTERRUPT
		int irq = (hw_registers[IRQ0ENABLE] && hw_registers[IRQ0STATUS]) || (hw_registers[IRQ1ENABLE] && hw_registers[IRQ1STATUS]) || (hw_registers[IRQ2ENABLE] && hw_registers[IRQ2STATUS]);
		if (irq && !interrupt) {
			hw_registers[IRQRETURN] = PC;
			PC = hw_registers[IRQHANDLER];
			interrupt = 1;
		}

		// Fetch *
		int line = memory[PC];

		// Decode *
		parse_line(instruction, line);
		instruction->i_format = is_I_Format(instruction);
		if (instruction->i_format) {
			instruction->imm = read_imm(memory[++PC]);
		}

		// TRACE
		if (instruction->i_format) {
			update_trace(trace, PC - 1, memory[PC - 1], registers, instruction);
		}
		else {
			update_trace(trace, PC, memory[PC], registers, instruction);
		}

		// Execute *
		execute_instruction(instruction, memory, registers, &PC, &PC_updated, hw_registers, &hw_updated, &leds_changed, &disp_7seg_changed, &disk_action);

		if (instruction->opcode == reti) { // return from interrupt
			interrupt = 0;
		}

		// HWREGTRACE
		if (hw_updated)	{
			update_hwregtrace(hwregtrace, argv[HWREGTRACE], instruction, registers, hw_registers, hw_registers[CLKS]);
		}

		// MONITOR
		if (hw_registers[MONITORCMD] == 1) {
			update_framebuffer(framebuffer, hw_registers[MONITORADDR], hw_registers[MONITORDATA]);
			hw_registers[MONITORCMD] = 0;
		}

		// LEDS
		if (leds_changed) {
			fprintf_s(leds, "%d %.8X\n", hw_registers[CLKS], hw_registers[LEDS]);
			leds_changed = 0;
		}

		// 7 SEGMENTS DISPLAY
		if (disp_7seg_changed) {
			fprintf_s(display7seg, "%d %.8X\n", hw_registers[CLKS], hw_registers[DISPLAY7SEG]);
			disp_7seg_changed = 0;
		}

		// HARD DISK
		if (hw_registers[DISKCMD] != 0 && disk_action == 0) { // disk timer starts from the next clock after activation
			disk_timer++;
			if (disk_timer == 1024)
			{
				disk_timer = 0;
				hw_registers[DISKCMD] = 0;
				hw_registers[DISKSTATUS] = 0;
				hw_registers[IRQ1STATUS] = 1;
			}
		}
		if (disk_action) { // copy is implemented at same clock, timer enabled for 1024 clocks
			switch (hw_registers[DISKCMD]) {
			case 1: // read
				read_diskoutSector_to_mem(diskout, memory, hw_registers[DISKSECTOR], hw_registers[DISKBUFFER]);
				break;
			case 2: // write
				write_mem_to_diskoutSector(memory, diskout, hw_registers[DISKSECTOR], hw_registers[DISKBUFFER]);
				break;
			}
			hw_registers[DISKSTATUS] = 1;
			disk_action = 0;
		}

	
		if (!PC_updated) PC++;	// if PC changed, skip increment.
		PC_updated = 0;
		hw_updated = 0;
		instruction->imm = 0;

		// TIMER
		if (hw_registers[TIMERENABLE]) {
			if (hw_registers[TIMERCURRENT] == hw_registers[TIMERMAX]) {
				hw_registers[TIMERCURRENT] = 0;
				hw_registers[IRQ0STATUS] = 1;
			}
			else
			{
				hw_registers[TIMERCURRENT]++;
			}
		}

		// CLOCK
		hw_registers[CLKS]++;
	}
	instruction->i_format = 0;

	if (PC == MEM_SIZE) {
		printf("PC out pf memory bounds.\n");
	}

	//----------- House Keeping ----------->
	fclose(diskout);
	fclose(leds);
	fclose(display7seg);

	update_trace(trace, PC, memory[PC], registers, instruction); // TRACE
	fclose(trace);
	free(instruction);

	open_file_check(&monitor, argv[MONITOR], "w+");
	open_file_check(&monitor_yuv, argv[MONITOR_YUV], "wb+");
	write_framebuffer_to_monitor(framebuffer, monitor, monitor_yuv); // FINAL MONITOR AND MONITOR.YUV UPDATE
	fclose(monitor);
	fclose(monitor_yuv);
	

	open_file_check(&memout, argv[MEMOUT], "w+");
	write_memory_to_memout(memout, memory); // MEMOUT
	fclose(memout);
	free(memory);

	open_file_check(&regout, argv[REGOUT], "w+");
	write_registers_to_regout(regout, registers); // REGOUT
	fclose(regout);
	free(registers);

	open_file_check(&cycles, argv[CYCLES], "w+");
	fprintf(cycles, "%d", hw_registers[CLKS]); // CYCLES
	fclose(cycles);
	free(hw_registers);

	return 0;
}

