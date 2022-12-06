//The Model container itself
class Model{
	public:
		int edit_mode = 0;
		Tile** world;
		Cloud** sky;
		Plant** weed;
		Fungi** shroom;
		vector<Tree*> trees;
		vector<Herbivore*> a_herb;
		vector<Omnivore*> a_omni;
		vector<Carnivore*> a_carn;
		vector<Detritivore*> a_detr;
		vector<Corpse*> bodies;
		
		Model(){
			world = new Tile* [H];
			sky = new Cloud* [H];
			weed = new Plant* [H];
			shroom = new Fungi* [H];
			for(int i=0;i<H;i++){
				world[i] = new Tile[W];
				sky[i] = new Cloud[W];
				weed[i] = new Plant[W];
				shroom[i] = new Fungi[W];
				for(int j=0;j<W;j++){
					world[i][j] = *(new Tile());
					sky[i][j] = *(new Cloud());
					weed[i][j] = *(new Plant());
					shroom[i][j] = *(new Fungi());
				}
			}
		}
		
		~Model(){
			for(int i=0;i<H;i++){
				delete world[i];
				delete sky[i];
				delete weed[i];
				delete shroom[i];
			}
			delete world;
			delete sky;
			delete weed;
			delete shroom;
			
			int i;
			for(i=0;i<trees.size();i++){
		 		delete trees[i];
		 	}
		 	trees.clear();
			for(i=0;i<bodies.size();i++){
		 		delete bodies[i];
		 	}
		 	bodies.clear();
			for(i=0;i<a_herb.size();i++){
		 		delete a_herb[i];
		 	}
		 	a_herb.clear();
			for(i=0;i<a_omni.size();i++){
		 		delete a_omni[i];
		 	}
		 	a_omni.clear();
			for(i=0;i<a_carn.size();i++){
		 		delete a_carn[i];
		 	}
		 	a_carn.clear();
			for(i=0;i<a_detr.size();i++){
		 		delete a_detr[i];
		 	}
		 	a_detr.clear();
		}
		
		//Take in ALL the data and process it according to the functions called
		void world_logic(){
			if(edit_mode == 0){
				wh_transfer(world);
				form_weather(world,sky);
				grow_grass(world,weed,shroom,&trees);
				grow_shrooms(world,shroom);
				grow_trees(world,sky,&trees);
				noahs_arc(world,&a_herb,&a_omni,&a_carn,&a_detr);
				animal_logic(world,shroom,&a_herb,&a_omni,&a_carn,&a_detr,&bodies);
				death(world,&trees,&a_herb,&a_omni,&a_carn,&a_detr,&bodies);
			}
		}
		
		//Save ALL the data to some .txt
		void saveMap(char* name){
			FILE* savefile;
			char file_addr[31] = "save/";
			strcat(file_addr,name);
			savefile = fopen(file_addr,"w");
			//Write absolutely everything on the map into the txt
			fprintf(savefile,"%d %d\n",W,H);
			for(int i=0;i<H;i++){
				for(int j=0;j<W;j++){
					Tile g = world[i][j];
					Cloud c = sky[i][j];
					Plant p = weed[i][j];
					Fungi f = shroom[i][j];
					fprintf(savefile, "/%d %f %f %f %d", g.altit, g.water, g.temp, c.water, p.num+f.num*10);
				}
				fprintf(savefile,"\n");
			}
			
			fprintf(savefile,"\n%ld",trees.size());
			for(int i=0;i<trees.size();i++){
				Tree* t = trees[i];
				fprintf(savefile,"/%d %d %d %d %d",t->x,t->y,t->health,t->energy,t->food);
			}
			fprintf(savefile,"\n%ld",bodies.size());
			for(int i=0;i<bodies.size();i++){
				Corpse* b = bodies[i];
				fprintf(savefile,"/%d %d %d",b->x,b->y,b->health);
			}
			fprintf(savefile,"\n%ld",a_herb.size());
			for(int i=0;i<a_herb.size();i++){
				Herbivore* h = a_herb[i];
				fprintf(savefile,"/%d %d %d %d",h->x,h->y,h->health,h->energy);
			}
			fprintf(savefile,"\n%ld",a_omni.size());
			for(int i=0;i<a_omni.size();i++){
				Omnivore* o = a_omni[i];
				fprintf(savefile,"/%d %d %d %d",o->x,o->y,o->health,o->energy);
			}
			fprintf(savefile,"\n%ld",a_carn.size());
			for(int i=0;i<a_carn.size();i++){
				Carnivore* k = a_carn[i];
				fprintf(savefile,"/%d %d %d %d",k->x,k->y,k->health,k->energy);
			}
			fprintf(savefile,"\n%ld",a_detr.size());
			for(int i=0;i<a_detr.size();i++){
				Detritivore* d = a_detr[i];
				fprintf(savefile,"/%d %d %d %d",d->x,d->y,d->health,d->energy);
			}
			fclose(savefile);
		}
};

class View{
	//TODO: Refactor showStats. Declare another method for avoiding the extra copy-paste code.
	public:
		//Show controls at the bottom
		void helpBar(int flag){
			attron(COLOR_PAIR(7));
			mvprintw(wy-1,0,"Q");
			mvprintw(wy-1,8,"E");
			mvprintw(wy-1,21,"1-5");
			mvprintw(wy-1,38,"WASD");
			if(flag == 1){
				mvprintw(wy-1,56,"+ *");
				mvprintw(wy-1,70,"- /");
				mvprintw(wy-1,87,"~ TAB");
				mvprintw(wy-1,113,"< >");
				mvprintw(wy-1,136,"S");
				mvprintw(wy-1,143,"K L");
			}
			attroff(COLOR_PAIR(7));
			
			attron(COLOR_PAIR(0));
			mvprintw(wy-1,2,"Quit");
			mvprintw(wy-1,10,"Edit Mode");
			mvprintw(wy-1,25,"Change View"); //❮❯￪￬
			mvprintw(wy-1,43,"Move Cursor");
			if(flag == 1){
				mvprintw(wy-1,57,",");
				mvprintw(wy-1,60,"Add 1/10");
				mvprintw(wy-1,71,",");
				mvprintw(wy-1,74,"Reduce 1/10");
				mvprintw(wy-1,88,",");
				mvprintw(wy-1,93,"Use/Set Custom Val");
				mvprintw(wy-1,114,",");
				mvprintw(wy-1,117,"Change Brush Size");
				mvprintw(wy-1,138,"Save");
				mvprintw(wy-1,144,",");
				mvprintw(wy-1,147,"Kill/Live");
			}else{
				move(wy-1,56);
				hline(' ',87);
			}
			attroff(COLOR_PAIR(0));
		}
	
		//Show a menu on the side with all the stats of the current tile
		void showStats(int d, Tile ground, Cloud sky, Plant grass, Fungi shroom){
			move(1,wx-30);
			attron(COLOR_PAIR(7));
			hline(' ',28);
			vline(' ',wy-3);
			move(wy-3,wx-30);
			hline(' ',28);
			move(1,wx-2);
			vline(' ',wy-3);
			attron(A_REVERSE);
			mvprintw(3,wx-28,"Current Mode:");
			mvprintw(6,wx-28,"Tile Stats:");
			attroff(COLOR_PAIR(7));
			
			attroff(A_REVERSE);
			for(int i=0;i<4;i++){
				mvprintw(8+i,wx-19,"               ");
			}
			attron(A_REVERSE);
			
			int i,lim = 0;
			//Display Altitude
			attron(COLOR_PAIR(77));
			mvprintw(8,wx-28,"Altitude");
			attroff(COLOR_PAIR(77));
			lim = (ground.altit+500-1)/300;
			for(i=0;i<lim;i++){
				attron(COLOR_PAIR(i*2+59));
				mvprintw(8,wx-19+i,"■");
				attroff(COLOR_PAIR(i*2+59));
			}
			attron(COLOR_PAIR(i*2+59));
			mvprintw(8,wx-8,"%d",ground.altit);
			attroff(COLOR_PAIR(i*2+59));
			
			//Display Temperature
			attron(COLOR_PAIR(97));
			mvprintw(9,wx-28,"Heat");
			attroff(COLOR_PAIR(97));
			lim = (ground.temp+15)/5;
			for(i=0;i<lim;i++){
				attron(COLOR_PAIR(i*2+79));
				mvprintw(9,wx-19+i,"■");
				attroff(COLOR_PAIR(i*2+79));
			}
			attron(COLOR_PAIR(i*2+79));
			mvprintw(9,wx-8,"%.1f",ground.temp);
			attroff(COLOR_PAIR(i*2+79));
			
			//Display Humidity
			attron(COLOR_PAIR(117));
			mvprintw(10,wx-28,"Water");
			attroff(COLOR_PAIR(117));
			lim = (ground.water-0.1)/100;
			for(i=0;i<lim;i++){
				attron(COLOR_PAIR(i*2+99));
				mvprintw(10,wx-19+i,"■");
				attroff(COLOR_PAIR(i*2+99));
			}
			attron(COLOR_PAIR(i*2+99));
			mvprintw(10,wx-8,"%.1f",ground.water);
			attroff(COLOR_PAIR(i*2+99));
			
			//Display Clouds
			attron(COLOR_PAIR(137));
			mvprintw(11,wx-28,"Clouds");
			attroff(COLOR_PAIR(137));
			lim = (sky.water-0.1)/100;
			for(i=0;i<lim;i++){
				attron(COLOR_PAIR(i*2+119));
				mvprintw(11,wx-19+i,"■");
				attroff(COLOR_PAIR(i*2+119));
			}
			attron(COLOR_PAIR(i*2+119));
			mvprintw(11,wx-8,"%.1f",sky.water);
			attroff(COLOR_PAIR(i*2+119));
			
			//Display the Living:
			attroff(A_REVERSE);
			for(i=0;i<4;i++){
					mvprintw(13+i,wx-28,"                          ");
			}
			attron(A_REVERSE);
			switch(ground.id){
				case 0:{
					//Show Plants
					attron(COLOR_PAIR(17));
					mvprintw(13,wx-28,"Plants");
					attroff(COLOR_PAIR(17));
					for(i=0;i<grass.num;i++){
						attron(COLOR_PAIR(i+8));
						mvprintw(13,wx-19+i,"■");
						attroff(COLOR_PAIR(i+8));
					}
					attron(COLOR_PAIR(i+8));
					mvprintw(13,wx-8,"%d",grass.num);
					attroff(COLOR_PAIR(i+8));
					
					//Show Fungi
					attron(COLOR_PAIR(48));
					mvprintw(14,wx-28,"Fungi ");
					attroff(COLOR_PAIR(48));
					for(i=0;i<shroom.num;i++){
						attron(COLOR_PAIR(i*10+8));
						mvprintw(14,wx-19+i*2,"■■");
						attroff(COLOR_PAIR(i*10+8));
					}
					attron(COLOR_PAIR(i*10+8));
					mvprintw(14,wx-8,"%d",shroom.num);
					attroff(COLOR_PAIR(i*10+8));
					break;
				}case 1:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					mvprintw(15,wx-28,"Energy ");
					mvprintw(16,wx-28,"Food:   ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(138));
					mvprintw(13,wx-19,"Tree");
					attroff(COLOR_PAIR(138));
					Tree* t = (Tree*)ground.entity;
					
					lim = (t->health-1)/2;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",t->health);
					attroff(COLOR_PAIR(144+i));
					
					lim = (t->energy-1)/3;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+154));
						mvprintw(15,wx-19+i,"■");
						attroff(COLOR_PAIR(i+154));
					}
					attron(COLOR_PAIR(154+i));
					mvprintw(15,wx-8,"%d",t->energy);
					attroff(COLOR_PAIR(154+i));
					
					lim = t->food-1;
					for(i=0;i<lim+1;i++){
						attron(COLOR_PAIR(i+164));
						mvprintw(16,wx-19+i*2,"■■");
						attroff(COLOR_PAIR(i+164));
					}
					attron(COLOR_PAIR(i+163));
					mvprintw(16,wx-8,"%d",t->food);
					attroff(COLOR_PAIR(i+163));
					break;
				}case 2:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(139));
					mvprintw(13,wx-19,"Corpse");
					attroff(COLOR_PAIR(139));
					Corpse* b = (Corpse*)ground.entity;
					
					lim = b->health/30;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",b->health);
					attroff(COLOR_PAIR(144+i));
					break;
				}case 3:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					mvprintw(15,wx-28,"Energy ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(140));
					mvprintw(13,wx-19,"Herbivore");
					attroff(COLOR_PAIR(140));
					Herbivore* h = (Herbivore*)ground.entity;
					
					lim = h->health/6;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",h->health);
					attroff(COLOR_PAIR(144+i));
					
					lim = h->energy/6;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+154));
						mvprintw(15,wx-19+i,"■");
						attroff(COLOR_PAIR(i+154));
					}
					attron(COLOR_PAIR(154+i));
					mvprintw(15,wx-8,"%d",h->energy);
					attroff(COLOR_PAIR(154+i));
					break;
				}case 4:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					mvprintw(15,wx-28,"Energy ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(141));
					mvprintw(13,wx-19,"Omnivore");
					attroff(COLOR_PAIR(141));
					Omnivore* o = (Omnivore*)ground.entity;
					
					lim = o->health/9;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",o->health);
					attroff(COLOR_PAIR(144+i));
					
					lim = o->energy/6;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+154));
						mvprintw(15,wx-19+i,"■");
						attroff(COLOR_PAIR(i+154));
					}
					attron(COLOR_PAIR(154+i));
					mvprintw(15,wx-8,"%d",o->energy);
					attroff(COLOR_PAIR(154+i));
					break;
				}case 5:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					mvprintw(15,wx-28,"Energy ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(142));
					mvprintw(13,wx-19,"Carnivore");
					attroff(COLOR_PAIR(142));
					Tree* t = (Tree*)ground.entity;
					
					lim = t->health/10;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",t->health);
					attroff(COLOR_PAIR(144+i));
					
					lim = t->energy/6;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+154));
						mvprintw(15,wx-19+i,"■");
						attroff(COLOR_PAIR(i+154));
					}
					attron(COLOR_PAIR(154+i));
					mvprintw(15,wx-8,"%d",t->energy);
					attroff(COLOR_PAIR(154+i));
					break;
				}case 6:{
					attroff(A_REVERSE);
					mvprintw(13,wx-28,"Entity ");
					mvprintw(14,wx-28,"Health ");
					mvprintw(15,wx-28,"Energy ");
					attron(A_REVERSE);
					attron(COLOR_PAIR(143));
					mvprintw(13,wx-19,"Detritivore");
					attroff(COLOR_PAIR(143));
					Detritivore* d = (Detritivore*)ground.entity;
					
					lim = d->health/5;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+144));
						mvprintw(14,wx-19+i,"■");
						attroff(COLOR_PAIR(i+144));
					}
					attron(COLOR_PAIR(144+i));
					mvprintw(14,wx-8,"%d",d->health);
					attroff(COLOR_PAIR(144+i));
					
					lim = d->energy/6;
					for(i=0;i<lim;i++){
						attron(COLOR_PAIR(i+154));
						mvprintw(15,wx-19+i,"■");
						attroff(COLOR_PAIR(i+154));
					}
					attron(COLOR_PAIR(154+i));
					mvprintw(15,wx-8,"%d",d->energy);
					attroff(COLOR_PAIR(154+i));
					break;
				}
			}
			
			switch(d){
				case 0:
					attron(COLOR_PAIR(17));
					mvprintw(3,wx-14,"Life       ");
					attroff(COLOR_PAIR(17));
				break;
				case 1:
					attron(COLOR_PAIR(77));
					mvprintw(3,wx-14,"Altitude   ");
					attroff(COLOR_PAIR(77));
				break;
				case 2:
					attron(COLOR_PAIR(97));
					mvprintw(3,wx-14,"Temperature");
					attroff(COLOR_PAIR(97));
				break;
				case 3:
					attron(COLOR_PAIR(117));
					mvprintw(3,wx-14,"Humidity   ");
					attroff(COLOR_PAIR(117));
				break;
				case 4:
					attron(COLOR_PAIR(130));
					mvprintw(3,wx-14,"Clouds     ");
					attroff(COLOR_PAIR(130));
				break;
			}
			attroff(A_REVERSE);
		}

		//Show the entire grid to the right of the center
		void show_world(int d, Tile** world, Cloud** sky, Plant** grass, Fungi** shrooms){	
			for(int i=0;i<H;i++){
				move((wy-H-1)/2+i,(wx-32-W*2)/2);
				for(int j=0;j<W;j++){
					int a;
					if(cx == j && cy == i){
						a = 5;
					}else{
						if(d==0){
							if(world[i][j].id == 0){
								a = 8+grass[i][j].num + shrooms[i][j].num*10;
							}else{
								a = 137+world[i][j].id;
							}
						}else if(d==1){
							a = 58+(world[i][j].altit+500)/150;
						}else if(d==2){
							a = 78+(world[i][j].temp+15)/2.5;
						}else if(d==3){
							a = 98+world[i][j].water/50;
						}else{
							a = 118+sky[i][j].water/50;
						}
					}
					attron(COLOR_PAIR(a));
					addch(' ');
					addch(' ');
					attroff(COLOR_PAIR(a));
				}
			}
			showStats(d, world[cy][cx], sky[cy][cx], grass[cy][cx], shrooms[cy][cx]);
			refresh();
		}
};

//TODO: Somehow move the drawing stuff from inside edit mode to the View
class Controller{
	private:
		int brush_size = 1;
		int custom_altit = 1000, custom_plant = 5;
		float custom_temp = 10, custom_water = 500, custom_sky = 500;
	public:
		int handle_input(Model* m, int c){
			switch(c){
				case 'q':			return 0;			break;
				case KEY_UP:		cy--; 				break;
				case KEY_DOWN:		cy++; 				break;
				case KEY_LEFT:		cx--; 				break;
				case KEY_RIGHT:		cx++; 				break;
				case '1' ... '5':	display = c-'0'-1;	break;
				case 'e':{
					m->edit_mode = (m->edit_mode+1)%2;
					if(m->edit_mode){
						mvprintw(18,wx-21,"Edit Mode ON");
					}else{	
						mvprintw(18,wx-21,"            ");
						mvprintw(20,wx-22,"              ");
						mvprintw(21,wx-23,"                   ");
					}
					break;
				}
			}
			if(m->edit_mode == 1){
				mvprintw(20,wx-22,"Brush Size: %d", brush_size);
				mvprintw(21,wx-23,"Custom Value:      ");
				switch(display){
					case 0:	mvprintw(21,wx-9,"%d",custom_plant);	break;
					case 1:	mvprintw(21,wx-9,"%d",custom_altit);	break;
					case 2:	mvprintw(21,wx-9,"%.1f",custom_temp);	break;
					case 3:	mvprintw(21,wx-9,"%.1f",custom_water);	break;
					case 4:	mvprintw(21,wx-9,"%.1f",custom_sky);	break;
				}
				if(c == ',' && brush_size>0){
					brush_size--;
				}else if(c == '.' && brush_size<25){
					brush_size++;
				}
				for(int i=0;i<brush_size;i++){
					for(int j=0;j<brush_size;j++){
						int tx = (cx-(brush_size-1)/2+j+W)%W;
						int ty = (cy-(brush_size-1)/2+i+H)%H;
						if(c == '+'){
							//Increase current visible stat on the tile/Increase Plants
							switch(display){
								case 0:	if(m->weed[ty][tx].num<9)			{m->weed[ty][tx].num++;}		break;
								case 1:	if(m->world[ty][tx].altit<2500)		{m->world[ty][tx].altit++;}		break;
								case 2:	if(m->world[ty][tx].altit<=34.9)  	{m->world[ty][tx].temp+=0.1;}	break;
								case 3:	if(m->world[ty][tx].water<=999)		{m->world[ty][tx].water++;}		break;
								case 4:	if(m->sky[ty][tx].water<=999)		{m->sky[ty][tx].water++;}		break;
							}
						}else if(c == '*'){
							//Increase by 10x/Increase Fungi
							switch(display){
								case 0:	if(m->shroom[ty][tx].num<4)			{m->shroom[ty][tx].num++;}		break;
								case 1:	if(m->world[ty][tx].altit<=2490)	{m->world[ty][tx].altit+=10;}	break;
								case 2:	if(m->world[ty][tx].altit<=34)  	{m->world[ty][tx].temp++;}		break;
								case 3:	if(m->world[ty][tx].water<=989)		{m->world[ty][tx].water+=10;}	break;
								case 4:	if(m->sky[ty][tx].water<=989)		{m->sky[ty][tx].water+=10;}		break;
							}
						}else if(c == '-'){
							//Decrease current visible stat on the tile/Decrease Plants
							switch(display){
								case 0:	if(m->weed[ty][tx].num>0)			{m->weed[ty][tx].num--;}		break;
								case 1:	if(m->world[ty][tx].altit>=1)		{m->world[ty][tx].altit--;}		break;
								case 2:	if(m->world[ty][tx].altit>=-14.9)  	{m->world[ty][tx].temp-=0.1;}	break;
								case 3:	if(m->world[ty][tx].water>=1)		{m->world[ty][tx].water--;}		break;
								case 4:	if(m->sky[ty][tx].water>=1)			{m->sky[ty][tx].water--;}		break;
							}
						}else if(c == '/'){
							//Decrease by 10x/Decrease Fungi
							switch(display){
								case 0:	if(m->shroom[ty][tx].num>0)			{m->shroom[ty][tx].num--;}		break;
								case 1:	if(m->world[ty][tx].altit>=10)		{m->world[ty][tx].altit-=10;}	break;
								case 2:	if(m->world[ty][tx].altit>=-14)  	{m->world[ty][tx].temp--;}		break;
								case 3:	if(m->world[ty][tx].water>=10)		{m->world[ty][tx].water-=10;}	break;
								case 4:	if(m->sky[ty][tx].water>=10)		{m->sky[ty][tx].water-=10;}		break;
							}
						}else if(c == '`'){
							//Set the values of a tile according to what's stored
							switch(display){
								case 0:	{m->weed[ty][tx].num = custom_plant;}		break;
								case 1:	{m->world[ty][tx].altit = custom_altit;}	break;
								case 2:	{m->world[ty][tx].temp = custom_temp;}		break;
								case 3:	{m->world[ty][tx].water = custom_water;}	break;
								case 4:	{m->sky[ty][tx].water = custom_sky;}		break;
							}
						}else if(c == '\t'){	// \t == Tab
							//Pipette your current values off a tile
							switch(display){
								case 0:	{custom_plant = m->weed[cy][cx].num;}		break;
								case 1:	{custom_altit = m->world[cy][cx].altit;}	break;
								case 2:	{custom_temp = m->world[cy][cx].temp;}		break;
								case 3:	{custom_water = m->world[cy][cx].water;}	break;
								case 4:	{custom_sky = m->sky[cy][cx].water;}		break;
							}
						}else if(c == 'k'){
							//Kill entity on tile, if present
							if(m->world[ty][tx].id != 0){
								Creature* temp = (Creature*)m->world[ty][tx].entity;
								temp->health=-10;
								m->world[ty][tx].entity = NULL;
								m->world[ty][tx].id = 0;
							}
						}
					}
				}
				//Spawning life
				if(c == 'l'){
					while(TRUE){
						c = getch();
						if(c == 'l' || c == 'q' || c == 'e' || c >= '1' && c <= '6'){
							break;
						}
					}
					if(c>='1' && c<='6'){
						for(int i=0;i<brush_size;i++){
							for(int j=0;j<brush_size;j++){
								int tx = (cx-(brush_size-1)/2+j+W)%W;
								int ty = (cy-(brush_size-1)/2+i+H)%H;
								if(m->world[ty][tx].id == 0){
									switch(c){
										case '1':{
											m->world[ty][tx].id = 1;
											Tree* temp = new Tree(ty,tx);
											m->world[ty][tx].entity = temp;
											m->trees.push_back(temp);
											break;
										}
										case '2':{
											m->world[ty][tx].id = 2;
											Corpse* temp = new Corpse(100,tx,ty);
											m->world[ty][tx].entity = temp;
											m->bodies.push_back(temp);
											break;
										}
										case '3':{
											m->world[ty][tx].id = 3;
											Herbivore* temp = new Herbivore(tx,ty);
											m->world[ty][tx].entity = temp;
											m->a_herb.push_back(temp);
											break;
										}
										case '4':{
											m->world[ty][tx].id = 4;
											Omnivore* temp = new Omnivore(tx,ty);
											m->world[ty][tx].entity = temp;
											m->a_omni.push_back(temp);
											break;
										}
										case '5':{
											m->world[ty][tx].id = 5;
											Carnivore* temp = new Carnivore(tx,ty);
											m->world[ty][tx].entity = temp;
											m->a_carn.push_back(temp);
											break;
										}
										case '6':{
											m->world[ty][tx].id = 6;
											Detritivore* temp = new Detritivore(tx,ty);
											m->world[ty][tx].entity = temp;
											m->a_detr.push_back(temp);
											break;
										}
									}
								}
							}
						}
					}
				}
				//Saving the current state:
				if(c == 's'){
					mvprintw(23,wx-18,"Save?");
					mvprintw(24,wx-20,"<Y>");
					mvprintw(24,wx-14,"<N>");
					int save_option = 0;
					while(TRUE){
						c = getch();
						mvchgat(24,wx-20+save_option*6,3,A_REVERSE,0,NULL);
						mvchgat(24,wx-14-(save_option)*6,3,A_NORMAL,0,NULL);
						if(c == ' ' && save_option == 0){
							//Save ALL data about the map
							mvprintw(23,wx-23,"Name your save:");
							mvprintw(24,wx-20,"         ");
							int name_index = 0;
							char name_buffer[21];
							char save_name[25];
							//Get rid of any junk in positions that would not be copied into
							memset(name_buffer, '\0', sizeof(name_buffer));	
							memset(save_name, '\0', sizeof(save_name));
							c = getch();
							while(c != '\n'){
								c = getch();
								if(name_index<21 && (c>='0' && c<='9' || c>='A' && c<='Z' || c>='a' && c<='z' || c==' ' || c=='_')){
									name_buffer[name_index] = (char)c;
									name_index++;
									mvaddch(24,wx-26+name_index,(char)c);
								}else if(name_index>0 && c == KEY_BACKSPACE){
									name_buffer[name_index] = '\0';
									mvaddch(24,wx-26+name_index,' ');
									name_index--;
								}else if(c == '\n'){
									break;
								}
								c = '\0';
								this_thread::sleep_for(chrono::milliseconds(1000/60));
							}
							if(name_buffer[0]!='\0'){	//If the name isn't empty, save the file
								strncpy(save_name,name_buffer,name_index);
								strcat(save_name,".txt");
								m->saveMap(save_name);
							}
							mvprintw(23,wx-23,"               ");
							mvprintw(24,wx-26,"                     ");
							break;
						}else if(c == KEY_LEFT || c == KEY_RIGHT){
							save_option++;
							save_option%=2;
						}else if(c == 's' || c == 'q' || c == 'e' || c == ' ' && save_option == 1){
							break;
						}
						this_thread::sleep_for(chrono::milliseconds(1000/60));
					}
					mvprintw(23,wx-18,"     ");
					mvprintw(24,wx-20,"            ");
				}
			}
			cy+=H;cy%=H;
			cx+=W;cx%=W;
			return 1;
		}
};

