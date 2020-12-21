#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <math.h>

#define HAT_WINDOW L"LaunchUnrealUWindowsClient"
#define HAT_TITLE L"A Hat in Time (64-bit, Final Release Shipping PC, DX9, Modding)"
#define HAT_EXE_NAME L"HatinTimeGame.exe"

#define PE_SECTION_PTR (0x3C)
#define PE_TS_OFFSET (0x08)

enum {
	DLC21_242922671485761424 = 1557549916, // DLC 2.1
	DLC232_5506509173732835905 = 1565114742, // 110% patch
	//DLC15_8061143192026666389 = 1537061434, // dw patch
	//DLC231_7770543545116491859 = 1561041656, // tas patch
	//MODDING_2996573833536660306 = 1527447656, // modding patch
	//CURRENT_20200903 = 1594660498, // current 2020/09/03
	VER_MAX = 2
};

struct hotkey {
	unsigned int vk;
	float duration;
};

HWND window;
HANDLE process;
HANDLE thread;
DWORD pid;
DWORD tid;

BYTE* hat_address = NULL;
unsigned int pe_ts;
unsigned int i_am_testing = 0;


BYTE* timer_state_address = NULL;
unsigned long long timer_state_addresses[VER_MAX] = {
	0x106AD34,
	0x10A8064
};

BYTE* igt_address = NULL;
unsigned long long igt_addresses[VER_MAX] = {
	0x106AD54,
	0x10A8084
};

BYTE* real_igt_address = NULL;
unsigned long long real_igt_addresses[VER_MAX] = {
	0x106AD64,
	0x10A8094
};

BYTE* tp_count_address = NULL;
unsigned long long tp_count_addresses[VER_MAX] = {
	0x106AD74,
	0x10A80A4
};

BYTE* chapter_address = NULL;
char* chapter_path = NULL;
char* chapter_paths[VER_MAX] = {
	"0x011E1570, 0x68, 0x108",
	"0x0121F280, 0x68, 0x108",
};


unsigned int get_pe_ts(BYTE* base) {
	unsigned int tmp;
	ReadProcessMemory(process, base + PE_SECTION_PTR, &tmp, sizeof(tmp), NULL);
	ReadProcessMemory(process, base + tmp + PE_TS_OFFSET, &tmp, sizeof(tmp), NULL);

	return tmp;
}

BYTE* resolve_ptr(HANDLE process, void* address) {
	BYTE* resolved;
	ReadProcessMemory(process, address, &resolved, 8, NULL);
	return resolved;
}

void* resolve_ptr_path(HANDLE process, BYTE* base, const char* path) {
	char* next;
	unsigned long long offset;

	if(!process || !base) {
		return NULL;
	}

	unsigned long long resolved = (unsigned long long)base + strtoull(path, &next, 0);

	while(path + strlen(path) > next) {
		offset = strtoull(next + 1, &next, 0);
		ReadProcessMemory(process, (void*)resolved, &resolved, 8, NULL);
		resolved += offset;
	}

	return (void*)resolved;
}


int is_hat_open() {
	if(FindWindow(HAT_WINDOW, NULL) != NULL) {
		return 1;
	}
	return 0;
}

int init() {
	if(!is_hat_open()) {
		return 0;
	}

	// find pid
	window = FindWindow(HAT_WINDOW, NULL);
	tid = GetWindowThreadProcessId(window, &pid);

	// get process handle passing in the pid
	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == NULL) {
		printf("Failed to open the game process, press any key to exit.\n");
		getchar();
		return 0;
	}
	// get thread handle passing in the tid
	thread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
	if(thread == NULL) {
		printf("Failed to open the main thread, press any key to exit.\n");
		getchar();
		return 0;
	}

	// find base addresses and store them
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if(snapshot == INVALID_HANDLE_VALUE) {
		printf("Failed to create snapshot of modules, press any key to exit.\n");
		getchar();
		return 0;
	}

	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	if(Module32First(snapshot, &entry)) {
		do {
			if(_wcsicmp(entry.szModule, HAT_EXE_NAME) == 0) {
				hat_address = entry.modBaseAddr;
				break;
			}
		} while(Module32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);

	if(hat_address == NULL) {
		printf("Failed to find base address of HatinTimeGame.exe, press any key to exit.\n");
		getchar();
		return 0;
	}

	return 1;
}
/*
struct hotkey* parse_keybinds(const char* path, unsigned int* out_num) {
	*out_num = 0;
	size_t hotkeys_size = sizeof(struct hotkey) * 8;
	struct hotkey* hotkeys = realloc(NULL, hotkeys_size);
	if(hotkeys == NULL) {
		printf("Unable to allocate buffer for hotkey data...\n");
		return NULL;
	}

	FILE* txtf;
	fopen_s(&txtf, path, "r");
	if(txtf == NULL) {
		printf("Unable to open file '%s'...\n", path);
		return NULL;
	}

	fseek(txtf, 0, SEEK_END);
	size_t txtsize = ftell(txtf);
	fseek(txtf, 0, SEEK_SET);

	char* txtbuf = malloc(txtsize + 1);
	if(txtbuf == NULL) {
		printf("Unable to allocate buffer for file '%s'...\n", path);
		fclose(txtf);
		return NULL;
	}

	fread(txtbuf, txtsize, 1, txtf);
	fclose(txtf);
	txtbuf[txtsize] = '\0';

	char* txtend = txtbuf + txtsize;
	while(txtbuf < txtend - 1) {
		if(*out_num * sizeof(struct hotkey) > hotkeys_size) {
			hotkeys_size *= 2;

			struct hotkey* new_loc = realloc(hotkeys, hotkeys_size);
			if(new_loc == NULL) {
				printf("Unable to increase size of the hotkey data buffer...\n");
				free(hotkeys);
				return NULL;
			}

			hotkeys = new_loc;
		}

		hotkeys[*out_num].vk = strtoul(txtbuf, &txtbuf, 0);
		txtbuf++;
		hotkeys[*out_num].duration = !i_am_testing ? 400.0f : strtof(txtbuf, NULL);
		(*out_num)++;

		while(*txtbuf == '\r' || *txtbuf == '\n') {
			txtbuf++;
		}
	}

	free(txtend - txtsize);

	return hotkeys;
}*/

int main(int argc, char** argv) {

	int always = 1;
	/*
	char* hotkey_path = "keybinds.txt";

	for(int i = 1; i < argc; i++) {
		if(strncmp(argv[i], "--i-am-testing", sizeof("--i-am-testing") - 1) == 0) {
			i_am_testing = 1;
		}
		else {
			hotkey_path = argv[i];
		}
	}
	*/
	unsigned int game_open = 0;
	//unsigned int hotkey_num;
	//struct hotkey* hotkeys = NULL;

	//double igt;
	double real_igt;
	double saved_igt;
	int tp_count;
	int chapter = 0;
	int old_timer_state = 0;
	int timer_state = 0;
	short replace_igt = 0;

	while(1) {
		if(!is_hat_open() || always) {
			always = 0;
			game_open = 0;
			printf("Waiting for the game to open...\n");
			
			while(!init()) {
				Sleep(1000);
			}
			
			pe_ts = get_pe_ts(hat_address);
			
			switch(pe_ts) {
				case DLC21_242922671485761424:
					real_igt_address = (BYTE*)(real_igt_addresses[0] + (unsigned long long)hat_address);
					igt_address = (BYTE*)(igt_addresses[0] + (unsigned long long)hat_address);
					timer_state_address = (BYTE*)(timer_state_addresses[0] + (unsigned long long)hat_address);
					tp_count_address = (BYTE*)(tp_count_addresses[0] + (unsigned long long)hat_address);
					chapter_path = chapter_paths[0];
					break;
				case DLC232_5506509173732835905:
					real_igt_address = (BYTE*)(real_igt_addresses[1] + (unsigned long long)hat_address);
					igt_address = (BYTE*)(igt_addresses[1] + (unsigned long long)hat_address);
					timer_state_address = (BYTE*)(timer_state_addresses[1] + (unsigned long long)hat_address);
					tp_count_address = (BYTE*)(tp_count_addresses[1] + (unsigned long long)hat_address);
					chapter_path = chapter_paths[1];
					break;
				//case DLC15_8061143192026666389: fps_path = fps_paths[0]; break;
				//case DLC231_7770543545116491859: fps_path = fps_paths[2]; break;
				//case MODDING_2996573833536660306: fps_path = fps_paths[4]; break;
				//case CURRENT_20200903: fps_path = fps_paths[5]; break;
				default: {
					printf("Your version of the game is not supported (%u), doing nothing.\n", pe_ts);
					continue;
				}
			}

			do {
				Sleep(500);
				chapter_address = resolve_ptr_path(process, hat_address, chapter_path);
			} while (chapter_address < (BYTE*)0x10000); // arbitrary

			game_open = 1;
			printf("Game opened!\n");
			if (timer_state == 1) {
				replace_igt = 2;
				saved_igt = real_igt;
				printf("\nThe game may have been closed unexpectedly, if that is the case, open a file to restore the timer.\nOtherwise, open a new file to cancel the restoration.\n");
				// print message (this makes the time printed follow the format of the igt)
				// message + hours
				printf("Time to be restored: %f (%d", saved_igt, (int)saved_igt / 3600);
				// minutes [ (int)igt % 60 ]
				printf(":%02d:", (int)saved_igt % 3600 / 60);
				// seconds + ms [ igt % 60 + igt - (int)igt ], then multiply by 100, apply trunc(), divide by 100
				printf("%05.2f)\n", trunc(((double)((int)saved_igt % 60) + saved_igt - (double)(int)saved_igt) * 100.0) / 100.0);
			}
			else if (replace_igt == 0) {
				printf("Waiting for the timer to stop...\n");
			}
		}

		// testing / debugging
		/*
		if (GetAsyncKeyState('J') & 0x8000) {

			ReadProcessMemory(process, real_igt_address, &real_igt, sizeof(real_igt), NULL);
			ReadProcessMemory(process, igt_address, &saved_igt, sizeof(saved_igt), NULL);

			// print message (this makes the time printed follow the format of the igt)

			// message + hours
			printf("\nREAL IGT value:\t%f (%d", real_igt, (int)real_igt / 3600);
			// minutes [ (int)igt % 60 ]
			printf(":%02d:", (int)real_igt % 3600 / 60);
			// seconds + ms [ igt % 60 + igt - (int)igt ], then multiply by 100, apply trunc(), divide by 100
			printf("%05.2f)\n", trunc(((double)((int)real_igt % 60) + real_igt - (double)(int)real_igt) * 100.0) / 100.0);

			// message + hours
			printf("IGT value:  \t%f (%d", saved_igt, (int)saved_igt / 3600);
			// minutes [ (int)igt % 60 ]
			printf(":%02d:", (int)saved_igt % 3600 / 60);
			// seconds + ms [ igt % 60 + igt - (int)igt ], then multiply by 100, apply trunc(), divide by 100
			printf("%05.2f)\n", trunc(((double)((int)saved_igt % 60) + saved_igt - (double)(int)saved_igt) * 100.0) / 100.0);

			ReadProcessMemory(process, tp_count_address, &tp_count, sizeof(tp_count), NULL);
			printf("TIMEPIECES WOO:\t%d\n", tp_count);
			printf("TIMER STATE:\t%d\n", timer_state);
			printf("replace_igt:\t%d\n", replace_igt);
			printf("timer_state:\t%d\n", timer_state);
			printf("Chapter:\t%d\n\n", chapter);

			//always = 1;
		}
		*/
		
		if (is_hat_open() && game_open) {

			// the chapter address will be constantly calculated when the timer will be restored
			if (replace_igt != 0) {
				chapter_address = resolve_ptr_path(process, hat_address, chapter_path);
				ReadProcessMemory(process, chapter_address, &chapter, sizeof(chapter), NULL);
			}

			ReadProcessMemory(process, real_igt_address, &real_igt, sizeof(real_igt), NULL);
			ReadProcessMemory(process, timer_state_address, &timer_state, sizeof(timer_state), NULL);
			ReadProcessMemory(process, tp_count_address, &tp_count, sizeof(tp_count), NULL);

			// timer state 2 means that finale has been completed, or rather, that the timer is green and frozen
			if (timer_state == 2 && replace_igt == 0) {
				// save igt and tp count
				ReadProcessMemory(process, igt_address, &saved_igt, sizeof(saved_igt), NULL);

				// print message (this makes the time printed follow the format of the igt)
				// message + hours
				printf("\nTimer stop detected!\nSaving the following IGT value: %f (%d", saved_igt, (int)saved_igt / 3600);
				// minutes [ (int)igt % 60 ]
				printf(":%02d:", (int)saved_igt % 3600 / 60);
				// seconds + ms [ igt % 60 + igt - (int)igt ], then multiply by 100, apply trunc(), divide by 100
				printf("%05.2f).\n", trunc(((double)((int)saved_igt % 60) + saved_igt - (double)(int)saved_igt) * 100.0) / 100.0);

				printf("It will be restored to the next file opened with The Finale as its last completed level.\n");
				
				// set replace igt flag
				replace_igt = 1;
			}

			// if the replace flag is on but an empty file is opened, the program will go back to waiting for the timer state 2
			if (replace_igt == 1 && timer_state == 1 && tp_count == 0) {
				printf("\nEmpty file opened, the restoration will be canceled.\nWaiting for the timer to stop...\n");
				replace_igt = 0;
			}

			// saved time is restored when the igt is detected as active (1) in a file that opened with the finale spawn point
			if (replace_igt == 1 && old_timer_state == 0 && timer_state == 1 && chapter == 5) {
				printf("\nRestoring IGT...");
				WriteProcessMemory(process, igt_address, &saved_igt, sizeof(saved_igt), NULL);
				printf(" Success!\nWaiting for the timer to stop...\n");
				replace_igt = 0;
			}

			else if (replace_igt == 2 && old_timer_state == 0 && timer_state == 1) {
				printf("\nRestoring IGT...");
				WriteProcessMemory(process, igt_address, &saved_igt, sizeof(saved_igt), NULL);
				printf(" Success!\nWaiting for the timer to stop...");
				replace_igt = 0;
			}

		}

		old_timer_state = timer_state;
		Sleep(10);
		
	}

	return 0;
}