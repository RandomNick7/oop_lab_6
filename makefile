all:
	g++ main.cpp -Iheaders -o "Simulation" -lncursesw
clear:
	find . -maxdepth 1 -type f -executable -delete
