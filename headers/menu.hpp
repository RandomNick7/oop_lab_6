extern int wx, wy, W, H;

//Sort names when showing filelist by ASCII value
bool cmpNames(char* a, char* b){
	int t = strcmp(b,a);
	if(t<0){
		t=0;
	}
	return t;
}

//All the formalities before doing anything
void setup(){
	setlocale(LC_ALL, "");					// Allow UTF-8 characters
	initscr();								// Curse the Terminal
	noecho();								// Turn off char echoing
	raw();									// Turn off line buffering
	nodelay(stdscr, TRUE);					// Turn off getch() halt
	keypad(stdscr, TRUE);					// Enable reading function & arrow keys
	curs_set(0);
	start_color();
	srand(time(0));

	if((has_colors() || can_change_color())==FALSE){	// Check if colors are supported in the first place
		endSim(1,"Your terminal does not support color/color modifications!\n");
	}
	initPallette();
}

int simMenu(){
	int selection = 0;
	while(TRUE){
		mvprintw(wy/2-3,(wx-12)/2,"Random World");
		mvprintw(wy/2,(wx-12)/2,  "Preset World");
		mvprintw(wy/2+3,(wx-12)/2,"  Quit Sim  ");
		mvchgat(wy/2+(selection-1)*3,(wx-12)/2,12,A_REVERSE,0,NULL);
		int c = getch();
		if(c == KEY_DOWN){
			selection++;
			selection%=3;
		}else if(c == KEY_UP){
			selection--;
			selection+=3;
			selection%=3;
		}else if(c == ' '){
			for(int i=0;i<3;i++){
				mvprintw(wy/2-(i-1)*3,(wx-14)/2,"                 ");
			}
			return selection;
		}
		this_thread::sleep_for(chrono::milliseconds(1000/60));
		refresh();
	}
}


//Load ALL the data from some .txt
void loadMap(char* savename, Model* m){
	FILE* savefile;
	delete m;
	savefile = fopen(savename,"r");
	fscanf(savefile,"%d %d\n",&W,&H);
	
	m = new Model();
	
	int life_num;
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			fscanf(savefile, "/%d %f %f %f %d", &m->world[i][j].altit, &m->world[i][j].water, &m->world[i][j].temp, &m->sky[i][j].water, &life_num);
			m->weed[i][j].num = life_num%10;
			m->shroom[i][j].num = life_num/10;
		}
		fscanf(savefile,"\n");
	}
	int vec_size;
	for(int i=0;i<6;i++){
		fscanf(savefile,"\n%d",&vec_size);
		for(int j=0;j<vec_size;j++){
			int x, y, health, energy;
			switch(i){
				case 0:{
					int food;
					fscanf(savefile,"/%d %d %d %d %d",&x,&y,&health,&energy,&food);
					Tree* temp = new Tree(y,x);
					temp->setStats(health,energy,food);
					m->trees.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}case 1:{
					fscanf(savefile,"/%d %d %d",&x,&y,&health);
					Corpse* temp = new Corpse(health,x,y);
					m->bodies.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}case 2:{
					fscanf(savefile,"/%d %d %d %d",&x,&y,&health,&energy);
					Herbivore* temp = new Herbivore(x,y);
					temp->setStats(health,energy);
					m->a_herb.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}case 3:{
					fscanf(savefile,"/%d %d %d %d",&x,&y,&health,&energy);
					Omnivore* temp = new Omnivore(x,y);
					temp->setStats(health,energy);
					m->a_omni.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}case 4:{
					fscanf(savefile,"/%d %d %d %d",&x,&y,&health,&energy);
					Carnivore* temp = new Carnivore(x,y);
					temp->setStats(health,energy);
					m->a_carn.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}case 5:{
					fscanf(savefile,"/%d %d %d %d",&x,&y,&health,&energy);
					Detritivore* temp = new Detritivore(x,y);
					temp->setStats(health,energy);
					m->a_detr.push_back(temp);
					m->world[y][x].entity = temp;
					break;
				}
			}
			m->world[y][x].id = i;
		}
	}
	fclose(savefile);
}

//Show the secondary menus
int simPrep(int opt, Model* m){
	int selection = 0;
	int screen = 0;
	vector<char*> file_names; 				//Store all file names in some variable
	int l;
	while(TRUE){
		int c = getch();
		//If you picked the random sim option...
		if(opt == 0){
			mvprintw(wy/2-3,(wx-11)/2," Width: %2d ",W);
			mvprintw(wy/2,(wx-11)/2,  "Height: %2d ",H);
			mvprintw(wy/2+3,(wx-11)/2,"   Start   ");
			mvprintw(wy/2+6,(wx-11)/2,"  Go Back  ");
			mvchgat(wy/2+(selection-1)*3,(wx-11)/2,11,A_REVERSE,0,NULL);
			//Select option
			if(c == KEY_DOWN){
				selection++;
				selection%=4;
			}else if(c == KEY_UP){
				selection--;
				selection+=4;
				selection%=4;
			}
			//Set size:
			if(selection == 0){
				if(c == KEY_LEFT && W>5){
					W--;
				}else if(c == KEY_RIGHT && W<(wx-36)/2){
					W++;
				}
			}else if(selection == 1){
				if(c == KEY_LEFT && H>5){
					H--;
				}else if(c == KEY_RIGHT && H<wy-3){
					H++;
				}
			}
			else if(selection == 2 && c == ' '){
				//Begin Simulation with Randomized flag ON
				return 1;
			}else if(selection == 3 && c == ' '){
				for(int i=0;i<4;i++){
					mvprintw(wy/2+(i-1)*3,(wx-14)/2,"                 ");
				}
				opt = simMenu();
				selection = 0;
			}
		}else if(opt == 1){
			l = file_names.size();
			//If we don't know what files we got in the save folder, check that directory out
			if(l == 0){
				DIR* dr;
				struct dirent *en;
				dr = opendir("save");
				if(dr){
					while ((en = readdir(dr)) != NULL) {
						if(en->d_name[0]!='.'){			//Look for all files that don't start with . 
							char* t = new char[21];
							memcpy(t,en->d_name,21);
							//If file's name (with the ".txt") is bigger than 25 characters, don't show it on our list
							if(strlen(en->d_name)<=25){
								//If file's name is under 21 characters, pad out the missing space with spaces
								for(int i=21;i>0;i--){
									if(t[i]=='.'){
										for(i;i<21;i++){
											t[i]=' ';
										}
										break;
									}
								}
								file_names.push_back(t);	//Add it to our list
							}
						}
					}
					closedir(dr);
				}
				l = file_names.size();
				sort(file_names.begin(),file_names.end(), cmpNames);	//Sort names alphabetically
			}
			if(l>7){
				for(int i=0;i<5;i++){
					mvprintw(wy/2-2+i,wx/2-8,"%s",file_names[i+screen]);
				}
				mvprintw(wy/2-3,(wx-10)/2,"     ᐱ ᐱ ᐱ");
				mvprintw(wy/2+3,(wx-10)/2,"     ᐯ ᐯ ᐯ");
				mvprintw(wy/2+l,(wx-10)/2,"    Go Back  ");
				if(selection != l){
					mvchgat(wy/2+selection-screen-2,wx/2-8,21,A_REVERSE,0,NULL);
				}else{
					mvchgat(wy/2+selection,(wx-6)/2,11,A_REVERSE,0,NULL);
				}
				if(c == KEY_DOWN){
					selection++;
					selection%=l+1;
					if(selection == 0){
						screen = 0;
					}
					if(selection>screen+4 && screen<l-5 && selection != l){
						screen++;
					}
				}else if(c == KEY_UP){
					selection--;
					selection+=l+1;
					selection%=l+1;
					if(selection == l-1){
						screen = l-5;
					}
					if(selection<screen && screen>0 && selection != l){
						screen--;
					}
				}
			}else{
				mvprintw(wy/2+l,(wx-10)/2,"    Go Back  ");
				for(int i=0;i<l;i++){
					mvprintw(wy/2-4+i,wx/2-8,"%s",file_names[i]);
				}
				if(selection != l){
					mvchgat(wy/2+selection-4,wx/2-8,21,A_REVERSE,0,NULL);
				}else{
					mvchgat(wy/2+selection,(wx-6)/2,11,A_REVERSE,0,NULL);
				}
				if(c == KEY_DOWN){
					selection++;
					selection%=l+1;
				}else if(c == KEY_UP){
					selection--;
					selection+=l+1;
					selection%=l+1;
				}
			}
			if(c == ' '){
				if(selection == l){
					for(int i=0;i<l;i++){
						mvprintw(wy/2-4+i,wx/2-10,"                       ");
					}
					mvprintw(wy/2+l,(wx-10)/2,"               ");
					opt = simMenu();
					selection = 0;
					screen = 0;
				}else{
					char savename[27] = "save/";
					int i;
					strcat(savename,file_names[selection]);
					for(i=23;i>0;i--){
						if(savename[i]!=' '){
							break;
						}
					}
					savename[i+1]='\0';
				
					strcat(savename,".txt");
					mvprintw(0,0,"%s",savename);
					refresh();
					loadMap(savename,m);
					//Begin Simulation with Randomized flag OFF
					return 0;
				}
			}
		}else if(opt == 2){
			endSim(0,"");
		}
		this_thread::sleep_for(chrono::milliseconds(1000/60));
		refresh();
	}
}
