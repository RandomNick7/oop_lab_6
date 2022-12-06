// The building block of the world. Things can exist on them
class Tile{
	public:
		int altit=rand()%(3000+1)-500;			//[-500,2500]
		float water=rand()%(6000+1)/10+300;		//[300,900]
		float temp=rand()%(500+1)/10-15;		//[-15,35]
		int id = 0;
		void* entity = NULL;					//Firearms regulations won't stop me from shooting my foot!
		
		void setProp(int a, float w, float t, int i, void* e){
			altit = a;
			water = w;
			temp = t;
			id = i;
			entity = e;
		}
};

// Entity used for water storage and transportation across the world
class Cloud{
	public:
		float water = 0;
		//When over a tile, rain down some amount of water (max 20%);
		void rain(Tile* ground){
			//The higher the altitude and the colder it is, the more it should rain
			float qty = water/5*(ground->altit+500)/3000*(35-ground->temp)/50;
			if((qty>10) && (ground->water+qty)<999){
				water-=qty;
				ground->water+=qty;
				ground->temp+=qty/500;
			}
		}
};

//Includes living things that can grow anywhere and are numerically contained to a single tile, such as Plants and Fungi
class Grower{
	public:
		int num=0;
		int energy;
		virtual void multiply(Tile* ground) = 0;
		void absorb(Tile* ground){
			if(ground->water-num>20 && energy<10){
				ground->water-=num/5+0.45;
				energy+=num;
			}else{
				if(energy>0){
					energy--;
					ground->temp-=0.0004*num;
				}else if(num>0){
					num--;
					ground->water+=5;
					ground->temp+=0.005;
					energy++;
				}
			}
		}
};

//Anything with health and which can perish is regarded as a "Creature". This includes Trees and Animals
class Creature{
	public:
		int health;
		int energy;
		int x;
		int y;
		
		void setStats(int h, int e){
			health = h;
			energy = e;
		}
};

class Tree: public Creature{
	public:
		int food = 0;
		Tree(int y, int x){
			this->x = x;
			this->y = y;
			energy = 10;
			health = 20;
		}
		
		void setStats(int h, int e, int f){
			health = h;
			energy = e;
			food = f;
		}
		
		//Convert water to energy, cool down air
		void absorb(Tile* ground){
			if(ground->water-8>50 && energy<29){
				ground->water-=7.5;
				energy+=2;
				ground->temp-=0.012;
			}else{
				if(energy>0){
					energy--;
					ground->temp-=0.012;
				}else{
					health--;
					ground->temp-=0.006;
				}
			}
		}
		
		//Generate food that can be eaten by other animals
		void fruit(){
			if(energy>10 && food<4){
				energy-=4;
				food++;
			}
		}
		
		//Convert energy to potential water
		void transpirate(Cloud* sky){
			if(energy>0 && sky->water<989){
				sky->water+=11;
				energy--;
			}
		}
};

//The primordial "living" entity. Uses water to grow into trees. Serves as a food source for some animals 
class Plant: public Grower{
	public:
		//Grow based on existing energy and waterity
		void multiply(Tile* ground){
			if(energy>5 && num<9 && rand()%100<(ground->water-300)/100 && ground->temp>5 && ground->id==0){
				num++;
				energy-=3+num/5;
			}
		}

		void grow(Tile* ground, vector<Tree*>* trees, int i, int j){
			num = 0;
			energy = 0;
			ground->id = 1;
			trees->push_back(new Tree(i,j));
			ground->entity = trees->back();
		}
};

//The Plant's evil twin. Grows in water spots. Serves as a food source.
class Fungi: public Grower{
	public:
		void multiply(Tile* ground){
			if(energy>3+num && num<4 && ground->temp>0 && rand()%200<(ground->water-400)/100){
				num++;
				energy-=3+num;
			}
		}
};

//The remnants of a dead animal.
class Corpse: public Creature{
	public:
		Corpse(int h, int pos_x, int pos_y){
			health = h;
			x = pos_x;
			y = pos_y;
			energy = 0;
		}
		
		void decompose(Tile* ground){
			ground->temp+=0.004;
			ground->water+=0.5;
			health--;
		}
};

//The living creatures that can move and feed upon other living creatures
class Animal: public Creature{
	protected:
		int range;
		//Movement based on greedy approach
		void move(Tile** map, int* x, int* y, int t_x, int t_y){
			int id = map[*y][*x].id;
			void* ent = map[*y][*x].entity;

			int d_x = t_x-*x;
			if(d_x!=0){
				d_x/=abs(d_x);
			}
			int d_y = t_y-*y;
			if(d_y!=0){
				d_y/=abs(d_y);
			}

			if(map[(*y+d_y+H)%H][(*x+d_x+W)%W].id!=0){
				if(d_x!=-1 || d_y!=0){
					d_x--;
				}else if(d_y!=-1){
					d_y--;
				}else{
					d_x++;
				}
			}

			if(map[(*y+d_y+H)%H][(*x+d_x+W)%W].id!=0){
				d_x = t_x-*x;
				if(d_x!=0){
					d_x/=abs(d_x);
				}
				d_y = t_y-*y;
				if(d_y!=0){
					d_y/=abs(d_y);
				}
				if(d_x!=1 || d_y!=0){
					d_x++;
				}else if(d_y!=1){
					d_y++;
				}else{
					d_x--;
				}
			}
			
			if(map[(*y+d_y+H)%H][(*x+d_x+W)%W].id!=0){
				d_x = 0;
				d_y = 0;
			}
			map[*y][*x].id = 0;
			map[*y][*x].entity = NULL;
			*x=(*x+d_x+W)%W;
			*y=(*y+d_y+H)%H;
			map[*y][*x].id = id;
			map[*y][*x].entity = ent;
		}
		
	public:
		int attack;
		//Lose energy over time
		void hunger(Tile* ground){
			if(energy>0){
				energy--;
				ground->temp+=0.05;
			}else{
				health--;
				ground->temp+=0.025;
			}
		}
};

class Detritivore: public Animal{
	public:
		Detritivore(int pos_x, int pos_y){
			health = 50;
			energy = 25;
			attack = 3;
			range = 4;
			x = pos_x;
			y = pos_y;
		}
		
		void target(Tile** map, Fungi** shroom){
			int new_x, new_y;
			//Check neighborhood for body/shrooms to eat
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==0 && shroom[new_y][new_x].num>0){
						energy+=shroom[new_y][new_x].num;
						shroom[new_y][new_x].num=0;
						return;
					}
					if(map[new_y][new_x].id==2){
						Corpse* b = (Corpse*)map[new_y][new_x].entity;
						b->health-=2*attack;
						energy+=3;
						return;
					}
				}
			}
			//Check in range for tree to go to
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==2){
						move(map,&x,&y,new_x,new_y);
						return;
					}
				}
			}
			new_x = (x+rand()%3-1+W)%W;
			new_y = (y+rand()%3-1+H)%H;
			move(map,&x,&y,new_x,new_y);
		}
		
		void breed(Tile** map, vector<Detritivore*>* a_detr){
			int new_x=x, new_y=y;
			//If next to a mate, spawn a new animal
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3 && new_x!=x && new_y!=y){
						Detritivore* d = (Detritivore*)map[new_y][new_x].entity;
						if(d->energy>40){
							//If neighboring tiles are free, spawn new animal there
							for(int k=-1;k<2;k++){
								for(int l=-1;l<2;l++){
									new_x = (k+d->x+W)%W;
									new_y = (l+d->y+H)%H;
									if(map[new_y][new_x].id == 0){
										d->energy-=20;
										energy-=20;
										map[new_y][new_x].id = 6;
										a_detr->push_back(new Detritivore(new_x,new_y));
										map[new_y][new_x].entity = a_detr->back();
										return;
									}
								}
							}
							//Otherwise, don't do anything
							return;
						}else{
							continue;
						}
					}
				}
			}
			//Look for a mate
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==6){
						Detritivore* d = (Detritivore*)map[new_y][new_x].entity;
						if(d->energy>40){
							move(map,&x,&y,new_x,new_y);
							return;
						}else{
							continue;
						}
					}
				}
			}
			new_x = (x+rand()%3-1+W)%W;
			new_y = (y+rand()%3-1+H)%H;
			move(map,&x,&y,new_x,new_y);
		}
};

class Herbivore: public Animal{
	public:
		Herbivore(int pos_x, int pos_y){
			health = 60;
			energy = 30;
			attack = 2;
			range = 3;
			x = pos_x;
			y = pos_y;
		}
		
		void target(Tile** map){
			int new_x, new_y;
			//Check neighborhood for tree to eat
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==1){
						Tree* t = (Tree*)map[new_y][new_x].entity;
						if(t->food>0){
							t->food--;
							t->health-=attack;
							energy+=3;
						}else{
							t->health-=2*attack;
							energy+=2;
						}
						return;
					}
				}
			}
			//Check in range for tree to go to
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==1){
						move(map,&x,&y,new_x,new_y);
						return;
					}
				}
			}
			new_x = (x+rand()%3-1+W)%W;
			new_y = (y+rand()%3-1+H)%H;
			move(map,&x,&y,new_x,new_y);
		}
		
		void breed(Tile** map, vector<Herbivore*>* a_herb){
			int new_x=x, new_y=y;
			//If next to a mate, spawn a new animal
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3 && new_x!=x && new_y!=y){
						Herbivore* h = (Herbivore*)map[new_y][new_x].entity;
						if(h->energy>50){
							//If neighboring tiles are free, spawn new animal there
							for(int k=-1;k<2;k++){
								for(int l=-1;l<2;l++){
									new_x = (k+h->x+W)%W;
									new_y = (l+h->y+H)%H;
									if(map[new_y][new_x].id == 0){
										h->energy-=25;
										energy-=25;
										map[new_y][new_x].id = 3;
										a_herb->push_back(new Herbivore(new_x,new_y));
										map[new_y][new_x].entity = a_herb->back();
										return;
									}
								}
							}
							//Otherwise, don't do anything
							return;
						}else{
							continue;
						}
					}
				}
			}
			//Look for a mate
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3){
						Herbivore* h = (Herbivore*)map[new_y][new_x].entity;
						if(h->energy>50){
							move(map,&x,&y,new_x,new_y);
							return;
						}else{
							continue;
						}
					}
				}
			}
		}
};

class Omnivore: public Animal{
	public:
		Omnivore(int pos_x, int pos_y){
			health = 90;
			energy = 35;
			attack = 6;
			range = 5;
			x = pos_x;
			y = pos_y;
		}
		
		void target(Tile** map){
			int new_x, new_y;
			//Check neighborhood for food/animal to eat
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==1){
						Tree* t = (Tree*)map[new_y][new_x].entity;
						if(t->food>0){
							t->food--;
							t->health-=attack/2;
							energy+=3;
						}else{
							t->health-=attack;
							energy+=2;
						}
						return;
					}else if(map[new_y][new_x].id == 3){
						Herbivore* h = (Herbivore*)map[new_y][new_x].entity;
						h->health-=attack;
						h->energy-=attack/2;
						health-=h->attack;
						energy+=5;
						return;
					}else if(map[new_y][new_x].id == 6){
						Detritivore* d = (Detritivore*)map[new_y][new_x].entity;
						d->health-=attack;
						d->energy-=attack/2;
						health-=d->attack;
						energy+=4;
						return;
					}
				}
			}
			
			//Check in range for a herbivore to go to
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==1){
						move(map,&x,&y,new_x,new_y);
						return;
					}
				}
			}
			//Check in range for a herbivore or carnivore to go to
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3 || map[new_y][new_x].id==6){
						move(map,&x,&y,new_x,new_y);
						return;
					}
				}
			}
			new_x = (x+rand()%3-1+W)%W;
			new_y = (y+rand()%3-1+H)%H;
			move(map,&x,&y,new_x,new_y);
		}
		
		void breed(Tile** map, vector<Omnivore*>* a_omni){
			int new_x=x, new_y=y;
			//If next to a mate, spawn a new animal
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==4 && new_x!=x && new_y!=y){
						Omnivore* o = (Omnivore*)map[new_y][new_x].entity;
						if(o->energy>45){
							//If neighboring tiles are free, spawn new animal there
							for(int k=-1;k<2;k++){
								for(int l=-1;l<2;l++){
									new_x = (k+o->x+W)%W;
									new_y = (l+o->y+H)%H;
									if(map[new_y][new_x].id == 0){
										o->energy-=30;
										energy-=30;
										map[new_y][new_x].id = 4;
										a_omni->push_back(new Omnivore(new_x,new_y));
										map[new_y][new_x].entity = a_omni->back();
										return;
									}
								}
							}
							//Otherwise, don't do anything
							return;
						}else{
							continue;
						}
					}
				}
			}
			//Look for a mate
			for(int i=1-range;i<range;i++){	
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==4){
						Omnivore* o = (Omnivore*)map[new_y][new_x].entity;
						if(o->energy>45){
							move(map,&x,&y,new_x,new_y);
							return;
						}else{
							continue;
						}
					}
				}
			}
		}
};

class Carnivore: public Animal{
	public:
		Carnivore(int pos_x, int pos_y){
			health = 100;
			energy = 40;
			attack = 8;
			range = 7;
			x = pos_x;
			y = pos_y;
		}
		
		void target(Tile** map){
			int new_x, new_y;
			//Check neighborhood for animal to eat
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3){
						Herbivore* h = (Herbivore*)map[new_y][new_x].entity;
						h->health-=attack;
						h->energy-=attack/2;
						health-=h->attack;
						energy+=10;
						return;
					}else if(map[new_y][new_x].id==4){
						Omnivore* h = (Omnivore*)map[new_y][new_x].entity;
						h->health-=attack;
						h->energy-=attack/2;
						health-=h->attack;
						energy+=6;
						return;
					}
				}
			}
			//Check in range for a herbivore or omnivore to go to
			for(int i=1-range;i<range;i++){
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==3 || map[new_y][new_x].id==4){
						move(map,&x,&y,new_x,new_y);
						return;
					}
				}
			}
			new_x = (x+rand()%3-1+W)%W;
			new_y = (y+rand()%3-1+H)%H;
			move(map,&x,&y,new_x,new_y);
		}
		
		void breed(Tile** map, vector<Carnivore*>* a_carn){
			int new_x=x, new_y=y;
			//If next to a mate, spawn a new animal
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==5 && new_x!=x && new_y!=y){
						Carnivore* c = (Carnivore*)map[new_y][new_x].entity;
						if(c->energy>45){
							//If neighboring tiles are free, spawn new animal there
							for(int k=-1;k<2;k++){
								for(int l=-1;l<2;l++){
									new_x = (k+c->x+W)%W;
									new_y = (l+c->y+H)%H;
									if(map[new_y][new_x].id == 0){
										c->energy-=25;
										energy-=25;
										map[new_y][new_x].id = 5;
										a_carn->push_back(new Carnivore(new_x,new_y));
										map[new_y][new_x].entity = a_carn->back();
										return;
									}
								}
							}
							//Otherwise, don't do anything
							return;
						}else{
							continue;
						}
					}
				}
			}
			//Look for a mate
			for(int i=1-range;i<range;i++){	
				for(int j=1-range;j<range;j++){
					new_x = (j+x+W)%W;
					new_y = (i+y+H)%H;
					if(map[new_y][new_x].id==5){
						Carnivore* c = (Carnivore*)map[new_y][new_x].entity;
						if(c->energy>45){
							move(map,&x,&y,new_x,new_y);
							return;
						}else{
							continue;
						}
					}
				}
			}
		}
};
