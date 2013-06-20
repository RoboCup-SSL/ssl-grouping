*****************************************************************
* 	RoboCup Small-Size League grouping program		*
*								*
* Last Updated: May 2011					*
*****************************************************************

- Files:

grouping.exe 	- program Windows exe
grouping.cpp 	- source code
Makefile	- a makefile
team.txt 	- example list of teams
readme.txt	- this file

- Overview

The participating teams are sorted into different bins according to their (expected) strength. 
The program then distributes these bins over the groups, while also considering the nationality of the teams (to avoid having teams of the same nationality in one group).

- Operation:

The executable should be run from the same directory where the file with the bins, teams.txt, is, and two random numbers must be provided as command line arguments. The program will then produce and print the grouping. 
The number of teams in each bin must be divisible by the number of groups. For that, it may be necessary to insert dummy teams, which can be deleted later.
The teams.txt format has three columns: Bin no., Country and Team. an example is provided below.

Suggested procedure to select the random numbers: Ask two persons to send one random number (32 Bit) each to a third person. This third person executes the program, using the two numbers as arguments, producing the grouping.

- Caveats:

If there are too many teams from the same country, they can be divided using different country names


- Example bins

Bin 1 (Top 4 of previous year):
-------------------------------
1 USA 		CMDragons
1 Thailand 	Plasma-Z
1 Japan		RoboDragons
1 China		ZJUNLICT


Bin 2 (Quarter finalists of last year):
---------------------------------------
2 Japan		KIKS
2 Germany	B-Smart
2 China		Wright Eagle
2 Mexico 	Eagle Knights


Bin 3 (Others):
---------------
3 China		FANTASIA
3 Finland	Botnia Dragon Knights
3 China 	AUA_Ares
3 Thailand	Skuba
3 China		Strive
3 Thailand	Khainui
3 Iran		MRL
3 USA		RoboJackets
3 Iran 		Parsian
3 Dummy 	Dumm
3 Dummy2 	Dumm
3 Dummy3 	Dumm


