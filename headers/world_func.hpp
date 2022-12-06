//All of below could be part of the Model, but "death","noahs_arc" and "animal_logic" need to be refactored then

//Make the world terrain be more smooth
void smoothen(Tile** world){
	//For simplicity, the world loops around the edges
	Tile** new_world = new Tile* [H];
	for(int i=0;i<H;i++){
		new_world[i] = new Tile[W];
		for(int j=0;j<W;j++){
			new_world[i][j].setProp(world[i][j].altit, world[i][j].water, world[i][j].temp, world[i][j].id, world[i][j].entity);
		}
	}
	
	for(int i=0;i<H;i++){						//Take the average values of a tile's Moore neighborhood
		for(int j=0;j<W;j++){
			for(int k=0;k<9;k++){
				if(k==4){
					continue;
				}
				Tile curr = world[(i+(k/3)-1+H)%H][(j+(k%3)-1+W)%W];
				new_world[i][j].altit += curr.altit;
				new_world[i][j].temp += curr.temp;
				new_world[i][j].water += curr.water;
			}
			new_world[i][j].altit /= 9;
			new_world[i][j].temp = new_world[i][j].temp/9;
			new_world[i][j].water /= 9;
		}
	}
	
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			world[i][j].setProp(new_world[i][j].altit, new_world[i][j].water, new_world[i][j].temp, new_world[i][j].id, world[i][j].entity);
		}
	}
}

//Simulate water flowing from one tile to other nearby ones
void wh_transfer(Tile** world){
	Tile** new_world = new Tile* [H];
	for(int i=0;i<H;i++){
		new_world[i] = new Tile[W];
		for(int j=0;j<W;j++){
			new_world[i][j].setProp(world[i][j].altit, world[i][j].water, world[i][j].temp, world[i][j].id, world[i][j].entity);
		}
	}
	
	//Spread the values to a tile's Moore neighborhood
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			for(int k=0;k<9;k++){
				if(k==4){
					continue;
				}
				Tile* curr = &new_world[(i+(k/3)-1+H)%H][(j+(k%3)-1+W)%W];
				float delta = (world[i][j].water+world[i][j].altit/2-(curr->water+curr->altit/2))/50;
				//if water transfer is within [0,1000] for both tiles, and water remains over threshold inversely proportional to alititude
				//More altitude -> More water may be transferred -> Less water to be remaining
				if(delta > 0 && new_world[i][j].water-delta>0 && curr->water+delta<999 && new_world[i][j].water>(2500-world[i][j].altit)/5){
					new_world[i][j].water-=delta;
					curr->water+=delta;
				}
				
				//if heat transfer is within [-15,35] for both tiles, and heat remains over threshold inversely proportional to alititude
				//More altitude -> More heat may be transferred -> Less heat to be remaining
				delta = (world[i][j].temp+world[i][j].altit/100-(curr->temp+curr->altit/100))/100;
				if(delta > 0 && new_world[i][j].temp-delta>-15 && curr->temp+delta<35 && new_world[i][j].temp>(2500-world[i][j].altit)/100-15){
					new_world[i][j].temp-=delta;
					curr->temp+=delta;
				}
			}
		}
	}
	
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			world[i][j].setProp(new_world[i][j].altit, new_world[i][j].water, new_world[i][j].temp, world[i][j].id, new_world[i][j].entity);
		}
	}
}

//Start creating clouds from evaporating water
void form_weather(Tile** world, Cloud** sky){
	Cloud** new_sky = new Cloud* [H];
	for(int i=0;i<H;i++){
		new_sky[i] = new Cloud[W];
		for(int j=0;j<W;j++){
			new_sky[i][j].water = sky[i][j].water;
		}
	}
	
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){	//Tiles that are warm and have enough water produce clouds above them
			if(world[i][j].water>350){
				float qty = (world[i][j].water-350)/5*(world[i][j].temp)/10;
				if(qty>0 && new_sky[i][j].water+qty<999){
					new_sky[i][j].water+=qty;
					world[i][j].water-=qty;
					qty/=500;
					if(world[i][j].temp-qty>-15){
						world[i][j].temp-=qty;
					}
				}
			}
		}
	}
	
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			sky[i][j].water = new_sky[i][j].water;
		}
	}
	
	for(int i=0;i<H;i++){							//Spread the values to a cloud's von Neumann neighborhood
		for(int j=0;j<W;j++){
			if(sky[i][j].water>0){
				new_sky[i][j].rain(&world[i][j]);	//Let a portion of the water in the cloud rain down on the ground
				float qty = sky[i][j].water/20;
				new_sky[(i-1+H)%H][j].water+=qty;
				new_sky[(i+1)%H][j].water+=qty;
				new_sky[i][(j-1+W)%W].water+=qty;
				new_sky[i][(j+1)%W].water+=qty;
				new_sky[i][j].water-=qty*4;
			}
		}
	}
	
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			sky[i][j].water = new_sky[i][j].water;
		}
	}
}

//Create, grow and multiply grass each tick
//Check if the grass can grow into trees each time
void grow_grass(Tile** world, Plant** grass, Fungi** shrooms, vector<Tree*>* trees){
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			grass[i][j].absorb(&world[i][j]);
			grass[i][j].multiply(&world[i][j]);
			if(grass[i][j].num == 0 && world[i][j].water > 300 && world[i][j].temp > 0 && world[i][j].id == 0){
				grass[i][j].num++;
			}
			if(grass[i][j].num == 9 && grass[i][j].energy>=10 && world[i][j].id == 0 && rand()%500<(world[i][j].water-100)/100){
				grass[i][j].grow(&world[i][j],trees,i,j);
				shrooms[i][j].num = 0;
			}
		}
	}
}

//Generate and multiply mushrooms on high water Tiles
void grow_shrooms(Tile** world, Fungi** shrooms){
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			shrooms[i][j].absorb(&world[i][j]);
			shrooms[i][j].multiply(&world[i][j]);
			if(shrooms[i][j].num==0 && world[i][j].water>500 && world[i][j].id == 0){
				shrooms[i][j].num++;
			}
		}	
	}
}

void grow_trees(Tile** world, Cloud** sky, vector<Tree*>* trees){
	for(int i=0;i<trees->size();i++){
		Tree* t = (*trees)[i];
		t->fruit();
		t->transpirate(&sky[t->y][t->x]);
		t->absorb(&world[t->y][t->x]);
	}
}

//Spawn animals over time to keep stuff going
void noahs_arc(Tile** world, vector<Herbivore*>* a_herb, vector<Omnivore*>* a_omni, vector<Carnivore*>* a_carn, vector<Detritivore*>* a_detr){
	if(a_herb->size()<5 && rand()%(W*H/10) < 5-a_herb->size()){
		int x = rand()%W;
		int y = rand()%H;
		while(world[y][x].id!=0){
			x = rand()%W;
			y = rand()%H;	
		}
		world[y][x].id = 3;
		a_herb->push_back(new Herbivore(x,y));
		world[y][x].entity = a_herb->back();
	}
	if(a_omni->size()<5 && rand()%(W*H/7) < 5-a_omni->size()){
		int x = rand()%W;
		int y = rand()%H;
		while(world[y][x].id!=0){
			x = rand()%W;
			y = rand()%H;	
		}
		world[y][x].id = 4;
		a_omni->push_back(new Omnivore(x,y));
		world[y][x].entity = a_omni->back();
	}
	if(a_carn->size()<5 && rand()%(W*H/6) < 5-a_carn->size()){
		int x = rand()%W;
		int y = rand()%H;
		while(world[y][x].id!=0){
			x = rand()%W;
			y = rand()%H;	
		}
		world[y][x].id = 5;
		a_carn->push_back(new Carnivore(x,y));
		world[y][x].entity = a_carn->back();
	}
	if(a_detr->size()<5 && rand()%(W*H/8) < 5-a_detr->size()){
		int x = rand()%W;
		int y = rand()%H;
		while(world[y][x].id!=0){
			x = rand()%W;
			y = rand()%H;	
		}
		world[y][x].id = 6;
		a_detr->push_back(new Detritivore(x,y));
		world[y][x].entity = a_detr->back();
	}
}

void death(Tile** world, vector<Tree*>* trees, vector<Herbivore*>* a_herb, vector<Omnivore*>* a_omni, vector<Carnivore*>* a_carn, vector<Detritivore*>* a_detr, vector<Corpse*>* bodies){
	//Trees
	for(int i=0;i<trees->size();i++){
		Tree* t = (*trees)[i];
		if(t->health <= 0){
			world[t->y][t->x].id = 0;
			world[t->y][t->x].water += 10;
			world[t->y][t->x].temp += 0.2;
			world[t->y][t->x].entity = NULL;
			delete (*trees)[i];
			trees->erase(trees->begin()+i);
			i--;
		}
	}
	//Bodies
	for(int i=0;i<bodies->size();i++){
		Corpse* b = (*bodies)[i];
		if(b->health <= 0){
			world[b->y][b->x].id = 0;
			world[b->y][b->x].entity = NULL;
			delete (*bodies)[i];
			bodies->erase(bodies->begin()+i);
			i--;
		}
	}
	//Herbivores
	for(int i=0;i<a_herb->size();i++){
		Herbivore* h = (*a_herb)[i];
		if(h->health <= 0){
			world[h->y][h->x].id = 2;
			bodies->push_back(new Corpse(100+h->energy*2,h->x,h->y));
			world[h->y][h->x].entity = bodies->back();
			delete (*a_herb)[i];
			a_herb->erase(a_herb->begin()+i);
			i--;
		}
	}
	//Omnivores
	for(int i=0;i<a_omni->size();i++){
		Omnivore* o = (*a_omni)[i];
		if(o->health <= 0){
			world[o->y][o->x].id = 2;
			bodies->push_back(new Corpse(100+o->energy*2,o->x,o->y));
			world[o->y][o->x].entity = bodies->back();
			delete (*a_omni)[i];
			a_omni->erase(a_omni->begin()+i);
			i--;
		}
	}
	//Carnivores
	for(int i=0;i<a_carn->size();i++){
		Carnivore* c = (*a_carn)[i];
		if(c->health <= 0){
			world[c->y][c->x].id = 2;
			bodies->push_back(new Corpse(100+c->energy*2,c->x,c->y));
			world[c->y][c->x].entity = bodies->back();
			delete (*a_carn)[i];
			a_carn->erase(a_carn->begin()+i);
			i--;
		}
	}
	//Detritivores
	for(int i=0;i<a_detr->size();i++){
		Detritivore* d = (*a_detr)[i];
		if(d->health <= 0){
			world[d->y][d->x].id = 2;
			bodies->push_back(new Corpse(100+d->energy*2,d->x,d->y));
			world[d->y][d->x].entity = bodies->back();
			delete (*a_detr)[i];
			a_detr->erase(a_detr->begin()+i);
			i--;
		}
	}
}

void animal_logic(Tile** world, Fungi** shroom, vector<Herbivore*>* a_herb, vector<Omnivore*>* a_omni, vector<Carnivore*>* a_carn, vector<Detritivore*>* a_detr, vector<Corpse*>* bodies){
	//Bodies
	for(int i=0;i<bodies->size();i++){
		Corpse* b = (*bodies)[i];
		b->decompose(&world[b->y][b->x]);
	}
	//Herbivores
	for(int i=0;i<a_herb->size();i++){
		Herbivore* h = (*a_herb)[i];
		if(h->energy>50){
			h->breed(world, a_herb);
		}else{
			h->target(world);
		}
		h->hunger(&world[h->y][h->x]);
	}
	//Omnivores
	for(int i=0;i<a_omni->size();i++){
		Omnivore* o = (*a_omni)[i];
		if(o->energy>45){
			o->breed(world, a_omni);
		}else{
			o->target(world);
		}
		o->hunger(&world[o->y][o->x]);
	}
	//Carnivores
	for(int i=0;i<a_carn->size();i++){
		Carnivore* c = (*a_carn)[i];
		if(c->energy>45){
			c->breed(world, a_carn);
		}else{
			c->target(world);
		}
		c->hunger(&world[c->y][c->x]);
	}
	//Detritivores
	for(int i=0;i<a_detr->size();i++){
		Detritivore* d = (*a_detr)[i];
		if(d->energy>50){
			d->breed(world, a_detr);
		}else{
			d->target(world, shroom);
		}
		d->hunger(&world[d->y][d->x]);
	}
}
