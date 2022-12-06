//We're allowed up to 255 colors from ncurses 6.0 onwards, better make them count!

//Colors - Default Set: [0,7] -> Black, Scarlet, _, _, _, Magenta, _, White
//Colors - Living Things: [8,50] -> Dirt Brown (608,463,326) -> Dark Green (361,663,16) |-> Brown Orange (644,349,7) -> Pea Green (688,741,184)
//Colors - Ground Altitude: [58,77] -> Viridian (0,498,400)-> Umber (388,318,278)
//Colors - Temperature: [78,97] -> Steel Blue (275,510,706) -> Scarlet Red (733,0,0)
//Colors - Humidity: [98,117] -> Dark Navy (8,28,365) -> Capri (0,749,1000)
//Colors - Clouds: [118,137] -> Eigengrau (86,86,114) -> Gainsboro (863,863,863)
//Colors - Trees: [138] -> Forest Green (133,545,133)
//Colors - Bodies: [139] -> Onyx (208,220,220)
//Colors - Animals: [140,143] -> Lime (749,1000,0), Tangerine(949,522,0), Vermillion (851,220,118), Aureolin(992,993,0);
//Colors - HP: [144,153] -> Blood Red (541,12,12) -> Amber (1000,749,0) -> Emerald (15,388,28)
//Colors - Energy: [154,163] -> Coffee (435,306,216) -> Lemon (1000,957,310)
//Colors - Food: [164,168] -> Honeydew (941,1000,941) -> Rasberry (890,43,365)

void initGradient(float iter, int o, int s_r, int s_g, int s_b, int e_r, int e_g, int e_b){
	iter--;
	for(int i=0;i<=iter;i++){
		init_color(i+o,((1 - i/iter) * s_r) + (i/iter * e_r),((1 - i/iter) * s_g) + (i/iter * e_g),((1 - i/iter) * s_b) + (i/iter * e_b));
		init_pair(i+o,0,i+o);
	}
}

void initPallette(){
	init_color(0,0,0,0);
	init_color(1,1000,141,0);
	init_color(5,1000,0,1000);
	init_color(7,1000,1000,1000);
	init_pair(0,7,0);
	init_pair(1,0,1);
	init_pair(5,0,5);
	init_pair(7,0,7);
	
	int s_r,s_g,s_b,
		e_r,e_g,e_b;
		
	for(int j=0;j<=4;j++){
		move(1+j,0);
		s_r = ((1 - j/4.0) * 608) + (j/4.0 * 644);
		s_g = ((1 - j/4.0) * 463) + (j/4.0 * 349);
		s_b = ((1 - j/4.0) * 326) + (j/4.0 * 7);

		e_r = ((1 - j/4.0) * 361) + (j/4.0 * 688);
		e_g = ((1 - j/4.0) * 663) + (j/4.0 * 741);
		e_b = ((1 - j/4.0) * 16) + (j/4.0 * 184);
		
		initGradient(10.0,8+j*10,s_r,s_g,s_b,e_r,e_g,e_b);
	}
	initGradient(20.0,58,0,498,400,388,318,278);
	initGradient(20.0,78,275,510,706,733,0,0);
	initGradient(20.0,98,8,28,365,0,749,1000);
	initGradient(20.0,118,86,86,114,863,863,863);
	init_color(138,133,545,133);
	init_pair(138,0,138);
	init_color(139,208,220,220);
	init_pair(139,0,139);
	init_color(140,749,1000,0);
	init_pair(140,0,140);
	init_color(141,949,522,0);
	init_pair(141,0,141);
	init_color(142,851,220,118);
	init_pair(142,0,142);
	init_color(143,992,993,0);
	init_pair(143,0,143);
	initGradient(5.0,144,541,12,12,1000,749,0);
	initGradient(6.0,148,1000,749,0,15,388,28);
	initGradient(10.0,154,435,306,216,1000,957,310);
	initGradient(4.0,164,941,1000,941,890,43,365);
}
