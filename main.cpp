#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <ncurses.h>
#include <dirent.h>
#include <cstring>

using namespace std;

int display = 0;
int cy = 10, cx = 10;
int wy = 0, wx = 0;
int W, H;

void endSim(int exit_status, const char* exit_msg){
	delwin(stdscr);
	endwin();
	printf("%s",exit_msg);
	exit(exit_status);
}

#include "colors.hpp"
#include "class.hpp"
#include "world_func.hpp"
#include "mvc.hpp"
#include "menu.hpp"

int main(){
	setup();
	getmaxyx(stdscr,wy,wx);					// Get current screen size;

	if(wx<155 || wy<30){					// Check if Terminal is big enough to fit the simulation
		endSim(1,"Make the screen bigger! (At least 145x30)\n");
	}
	
	W = (wx-36)/2;
	H = wy-3;
	
	Model* m = new Model();
	View* v = new View();
	Controller* c = new Controller();
	
	int rand_world = simPrep(simMenu(),m);
	
	if(rand_world){
		delete m;
		m = new Model();
		smoothen(m->world);
	}
	
	move(0,0);
	int running = 1;
	
	while(running){
		v->helpBar(m->edit_mode);
		m->world_logic();
		
		running = c->handle_input(m,getch());
		
		v->show_world(display,m->world,m->sky,m->weed,m->shroom);
		this_thread::sleep_for(chrono::milliseconds(1000/60));
	}
	
	// Enact the Apocalypse
	delete m;
	delete v;
	delete c;
	delwin(stdscr);							// Pour Holy Water over the Terminal
	endwin();								// Lift the nCurses
	return 0;
}
