#include <math.h>


#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

#define GAME_BULLET_COUNT 32
#define GAME_DUST_COUNT 320

typedef struct{
	float pos[3];
	float vec[3];
	float time;
}COParticle;

typedef struct{
	float pos[2];
	float vec[2];
	float time;
}COFire;

typedef struct{
	float pos[2];
	float vec[2];
	float size;
	uint type;
	uint generation;
}COPebbel;

struct{
	float		player_pos[2];
	float		player_vec[2];
	float		player_rot;
	COPebbel	*pebbels;
	uint		pebbel_count;
	COFire		fire[GAME_BULLET_COUNT];
	float		fire_time;
	uint		fire_next;
	COParticle	dust[GAME_DUST_COUNT];
	uint		dust_next;
	boolean		engine;
	float		time;
	uint		lifes;
	float		aspect;
}COGame;

boolean active_game = FALSE;

void co_init_game(uint count)
{
	uint i;
	COGame.player_pos[0] = 0;
	COGame.player_pos[1] = 0;
	COGame.player_vec[0] = 0;
	COGame.player_vec[1] = 0;
	COGame.player_rot = 0;
	COGame.pebbels = malloc((sizeof *COGame.pebbels) * count * 8);
	COGame.pebbel_count = 0;
	active_game = TRUE;
	COGame.fire_time = 0;
	COGame.fire_next = 0;
	COGame.dust_next = 0;
	COGame.engine = FALSE;
	COGame.time = 0;
	COGame.lifes = 3;
	for(i = 0; i < GAME_BULLET_COUNT; i++)
		COGame.fire[i].time = 0;
	for(i = 0; i < GAME_DUST_COUNT; i++)
		COGame.dust[i].time = 0;	
}

void co_spread_dust(float *pos, float *vec, float blast_size, float object_size, uint count)
{
	uint i;
	for(i = 0; i < count; i++)
	{
		COGame.dust[COGame.dust_next].vec[0] = (get_rand(COGame.dust_next * 4)  - 0.5) * 0.4 * blast_size + vec[0] * 0.2;
		COGame.dust[COGame.dust_next].vec[1] = (get_rand(COGame.dust_next * 4 + 1) - 0.5) * 0.4 * blast_size + vec[1] * 0.2;
		COGame.dust[COGame.dust_next].vec[2] = (get_rand(COGame.dust_next * 4 + 2) - 0.5) * 1 * blast_size;

		COGame.dust[COGame.dust_next].pos[0] = pos[0] + COGame.dust[COGame.dust_next].vec[0] * (get_rand(COGame.dust_next * 4 + 3) - 0.5) * object_size * 2;
		COGame.dust[COGame.dust_next].pos[1] = pos[1] + COGame.dust[COGame.dust_next].vec[1] * (get_rand(COGame.dust_next * 4 + 3) - 0.5) * object_size * 2;
		COGame.dust[COGame.dust_next].pos[2] = COGame.dust[COGame.dust_next].vec[2] * (get_rand(COGame.dust_next * 4 + 3) - 0.5) * object_size * 2;

		COGame.dust[COGame.dust_next].time = 0.5 * (float)i / (float)count;
		COGame.dust_next = (COGame.dust_next + 1) % GAME_DUST_COUNT;
	}
}

void co_end_game(void)
{
	uint i;
	for(i = 0; i < COGame.pebbel_count; i++)
		co_spread_dust(COGame.pebbels[i].pos, COGame.pebbels[i].vec, 1 + 1 / (COGame.pebbels[i].generation + 1), COGame.pebbels[i].size, GAME_DUST_COUNT / COGame.pebbel_count);
	free(COGame.pebbels);
	COGame.pebbel_count = 0;
}


void co_create_pebel(uint type, float pos_x, float pos_y, float scale)
{
	COGame.pebbels[COGame.pebbel_count].pos[0] = pos_x;
	COGame.pebbels[COGame.pebbel_count].pos[1] = pos_y;
	COGame.pebbels[COGame.pebbel_count].vec[0] = get_rand(COGame.pebbel_count * 2) * 0.2 - 0.1/* scale*/;
	COGame.pebbels[COGame.pebbel_count].vec[1] = get_rand(COGame.pebbel_count * 2 + 1) * 0.2 - 0.1 /* scale*/;
	COGame.pebbels[COGame.pebbel_count].size = scale;
	COGame.pebbels[COGame.pebbel_count].type = type;
	COGame.pebbels[COGame.pebbel_count].generation = 0;
	COGame.pebbel_count++;
}

boolean co_is_game_active(void)
{
	return active_game;
}




/*
void	co_draw_pebbel_1(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.040000, 0.280000, 0.240000, 0.180000, 0.240000, 0.180000, 0.280000, -0.060000, 0.280000, -0.060000, 0.220000, -0.220000, 0.220000, -0.220000, -0.000000, -0.340000, -0.000000, -0.340000, -0.140000, -0.240000, -0.280000, -0.180000, -0.140000, -0.240000, -0.300000, 0.060000, -0.280000, -0.180000, -0.300000, 0.060000, -0.140000, 0.100000, -0.140000, 0.100000, -0.040000, 0.280000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 18, 2, red, green, blue);
	glPopMatrix();
}*/

void co_draw_pebbel1(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.380000, -0.120000, -0.240000, -0.420000, -0.240000, -0.420000, 0.060000, -0.480000, 0.060000, -0.480000, 0.140000, -0.400000, 0.140000, -0.400000, 0.300000, -0.460000, 0.300000, -0.440000, 0.480000, -0.100000, 0.480000, -0.100000, 0.380000, 0.120000, 0.380000, 0.120000, 0.400000, 0.300000, 0.400000, 0.300000, 0.040000, 0.440000, 0.040000, 0.440000, -0.240000, 0.400000, -0.240000, 0.400000, -0.220000, 0.300000, -0.220000, 0.300000, -0.380000, 0.220000, -0.380000, 0.220000, -0.380000, -0.120000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 24, 2, red, green, blue);
	glPopMatrix();
}

void co_draw_pebbel2(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.100000, 0.420000, -0.380000, 0.220000, -0.380000, 0.220000, -0.240000, 0.060000, -0.260000, 0.060000, -0.440000, -0.020000, -0.440000, -0.020000, -0.300000, -0.360000, -0.300000, -0.360000, 0.100000, -0.500000, 0.100000, -0.500000, 0.320000, -0.360000, 0.320000, -0.360000, 0.340000, -0.040000, 0.340000, -0.040000, 0.460000, 0.040000, 0.460000, 0.040000, 0.380000, 0.240000, 0.380000, 0.240000, 0.140000, 0.420000, 0.040000, 0.340000, -0.100000, 0.420000, 0.140000, 0.420000, 0.040000, 0.340000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 24, 2, red, green, blue);
	glPopMatrix();
}

void co_draw_pebbel3(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.100000, 0.420000, -0.460000, 0.160000, -0.460000, 0.160000, -0.500000, -0.140000, -0.500000, -0.140000, -0.320000, -0.280000, -0.320000, -0.280000, -0.160000, -0.480000, -0.160000, -0.480000, 0.120000, -0.340000, 0.120000, -0.340000, 0.260000, -0.400000, 0.260000, -0.400000, 0.480000, -0.100000, 0.480000, -0.100000, 0.360000, 0.240000, -0.100000, 0.420000, 0.360000, 0.240000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 18, 2, red, green, blue);
	glPopMatrix();
}

void co_draw_pebbel4(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.400000, -0.320000, -0.460000, 0.040000, -0.460000, 0.040000, -0.180000, 0.120000, -0.180000, 0.120000, -0.020000, 0.400000, -0.020000, 0.400000, 0.360000, 0.280000, 0.480000, -0.080000, 0.400000, -0.340000, 0.360000, 0.280000, 0.480000, -0.080000, 0.400000, -0.340000, 0.060000, -0.500000, 0.060000, -0.500000, -0.080000, -0.340000, -0.080000, -0.340000, -0.400000, -0.320000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 18, 2, red, green, blue);
	glPopMatrix();
}



void	co_draw_ship_engine(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.000000, 0.024000, -0.018000, -0.020000, -0.000000, 0.024000, 0.018000, -0.020000, -0.004000, -0.012000, -0.018000, -0.020000, -0.004000, -0.012000, 0.004000, -0.012000, 0.004000, -0.012000, 0.018000, -0.020000, -0.018000, -0.038000, -0.012000, -0.020000, -0.006000, -0.024000, -0.018000, -0.038000, -0.006000, -0.024000, -0.000000, -0.050000, -0.000000, -0.050000, 0.006000, -0.024000, 0.012000, -0.020000, 0.018000, -0.038000, 0.018000, -0.038000, 0.006000, -0.024000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 22, 2, red, green, blue);
	glPopMatrix();
}

void	co_draw_ship(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.018000, -0.020000, -0.000000, 0.024000, -0.000000, 0.024000, -0.018000, -0.020000, -0.004000, -0.012000, -0.018000, -0.020000, 0.004000, -0.012000, -0.004000, -0.012000, 0.004000, -0.012000, 0.018000, -0.020000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 10, 2, red, green, blue);
	glPopMatrix();
}


void co_draw_game_over(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.500000, -0.080000, -0.580000, -0.080000, 0.080000, 0.080000, 0.080000, -0.080000, -0.580000, -0.080000, -0.580000, 0.080000, -0.580000, 0.080000, -0.500000, 0.080000, 0.220000, 0.080000, 0.260000, -0.080000, 0.260000, -0.080000, 0.300000, 0.080000, 0.360000, 0.080000, 0.360000, -0.080000, 0.360000, -0.080000, 0.440000, -0.080000, 0.440000, 0.080000, 0.360000, 0.080000, 0.160000, 0.080000, 0.080000, 0.080000, -0.160000, 0.080000, -0.160000, -0.080000, 0.160000, -0.080000, 0.160000, 0.080000, 0.080000, -0.080000, 0.160000, -0.080000, 0.420000, 0.000000, 0.360000, 0.000000, 0.500000, 0.080000, 0.500000, -0.080000, -0.080000, -0.080000, -0.160000, -0.080000, -0.160000, 0.080000, -0.080000, 0.080000, -0.160000, 0.000000, -0.100000, 0.000000, 0.520000, 0.000000, 0.580000, -0.080000, -0.260000, 0.040000, -0.220000, 0.080000, -0.300000, 0.080000, -0.300000, -0.080000, -0.220000, 0.080000, -0.220000, -0.080000, -0.260000, 0.040000, -0.300000, 0.080000, -0.360000, 0.080000, -0.440000, 0.080000, -0.360000, -0.080000, -0.360000, 0.080000, -0.440000, 0.080000, -0.440000, -0.080000, -0.440000, 0.000000, -0.360000, 0.000000, -0.500000, 0.000000, -0.500000, -0.080000, 0.500000, 0.080000, 0.580000, 0.080000, 0.580000, 0.080000, 0.580000, 0.000000, 0.580000, 0.000000, 0.500000, 0.000000, -0.540000, 0.000000, -0.500000, 0.000000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 64, 2, red, green, blue);
	glPopMatrix();
}


void co_game_draw_ship1(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.040000, -0.020000, 0.040000, -0.020000, 0.040000, -0.020000, 0.080000, 0.000000, 0.080000, 0.000000, 0.040000, 0.020000, -0.080000, 0.000000, -0.040000, -0.020000, -0.040000, 0.020000, -0.080000, 0.000000, 0.040000, 0.020000, -0.040000, 0.020000, -0.040000, 0.020000, -0.020000, 0.040000, -0.020000, 0.040000, 0.020000, 0.040000, 0.020000, 0.040000, 0.040000, 0.020000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	sui_draw_gl(GL_LINES, array, 18, 2, red, green, blue);
	glPopMatrix();
}
/*
void co_game_draw_ship2(float pos_x, float pos_y, float rot, float red, float green, float blue)
{
	static float array[] = {-0.000000, 0.060000, 0.030000, 0.051962, 0.030000, 0.051962, 0.051962, 0.030000, 0.051962, 0.030000, 0.060000, 0.000000, -0.000000, -0.540000, -0.010000, -0.537321, -0.010000, -0.537321, -0.017321, -0.530000, -0.017321, -0.530000, -0.020000, -0.520000, -0.000000, 0.060000, -0.030000, 0.051962, -0.030000, 0.051962, -0.051962, 0.030000, -0.051962, 0.030000, -0.060000, 0.000000, 0.040000, -0.200000, 0.050000, -0.197321, 0.050000, -0.197321, 0.057321, -0.190000, 0.057321,
 -0.190000, 0.060000, -0.180000, -0.040000, -0.200000, -0.050000, -0.197321, -0.050000, -0.197321, -0.057320, -0.190000, -0.057320, -0.190000, -0.060000, -0.180000, -0.040000, -0.200000, -0.040000, -0.240000, -0.040000, -0.240000, -0.020000, -0.260000, -0.020000, -0.260000, 0.020000, -0.260000, 0.020000, -0.260000, 0.040000, -0.240000, 0.040000, -0.240000, 0.040000, -0.200000, 0.060000, -0.180000, 0.060000, 0.000000, -0.060000, 0.000000, -0.060000, -0.180000, -0.000000, -0.260000, -0.000000, 
 -0.480000, -0.000000, -0.500000, -0.010000, -0.502680, -0.010000, -0.502680, -0.017321, -0.510000, -0.017321, -0.510000, -0.020000, -0.520000, -0.000000, -0.500000, 0.010000, -0.502680, 0.010000, -0.502680, 0.017320, -0.510000, 0.017320, -0.510000, 0.020000, -0.520000, -0.000000, -0.540000, 0.010000, -0.537321, 0.010000, -0.537321, 0.017320, -0.530000, 0.017320, -0.530000, 0.020000, -0.520000, 0.080000, 0.000000, 0.400000, 0.000000, -0.080000, 0.000000, -0.400000, 0.000000, -0.100000, 0.020000,
 -0.100000, 0.060000, -0.100000, -0.020000, -0.100000, -0.060000, 0.100000, -0.020000, 0.100000, -0.060000, 0.100000, 0.020000, 0.100000, 0.060000, 0.100000, 0.060000, 0.380000, 0.060000, 0.380000, 0.060000, 0.380000, 0.020000, 0.380000, -0.020000, 0.380000, -0.060000, 0.100000, -0.060000, 0.380000, -0.060000, 0.340000, -0.020000, 0.340000, -0.040000, 0.300000, -0.020000, 0.300000, -0.040000, 0.260000, -0.020000, 0.260000, -0.040000, 0.220000, -0.020000, 0.220000, -0.040000, 0.180000, -0.020000,
 0.180000, -0.040000, 0.140000, -0.020000, 0.140000, -0.040000, 0.140000, 0.040000, 0.140000, 0.020000, 0.180000, 0.040000, 0.180000, 0.020000, 0.220000, 0.040000, 0.220000, 0.020000, 0.260000, 0.040000, 0.260000, 0.020000, 0.300000, 0.040000, 0.300000, 0.020000, 0.340000, 0.040000, 0.340000, 0.020000, -0.100000, 0.060000, -0.380000, 0.060000, -0.380000, 0.060000, -0.380000, 0.020000, -0.380000, -0.020000, -0.380000, -0.060000, -0.380000, -0.060000, -0.100000, -0.060000, -0.220000, -0.020000, 
	-0.220000, -0.040000, -0.180000, -0.020000, -0.180000, -0.040000, -0.140000, -0.020000, -0.140000, -0.040000, -0.140000, 0.040000, -0.140000, 0.020000, -0.180000, 0.040000, -0.180000, 0.020000, -0.220000, 0.040000, -0.220000, 0.020000, -0.260000, 0.040000, -0.260000, 0.020000, -0.260000, -0.020000, -0.260000, -0.040000, -0.340000, -0.020000, -0.340000, -0.040000, -0.300000, -0.020000, -0.300000, -0.040000, -0.300000, 0.040000, -0.300000, 0.020000, -0.340000, 0.040000, -0.340000, 0.020000, 
 0.040000, 0.060000, 0.140000, 0.260000, -0.040000, 0.060000, -0.140000, 0.260000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, array, 144, 2, red, green, blue);
	glPopMatrix();
}*/


void co_game_draw_lander(float pos_x, float pos_y, float rot, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, 0.040000, 0.020000, 0.040000, -0.020000, 0.040000, -0.020000, 0.040000, -0.020000, 0.020000, -0.020000, 
0.020000, -0.060000, -0.040000, -0.060000, -0.040000, -0.040000, -0.060000, -0.040000, -0.060000, -0.060000, -0.080000, -0.060000, -0.080000, -0.060000, -0.200000,
 0.060000, -0.200000, 0.080000, -0.260000, -0.060000, -0.200000, -0.080000, -0.260000, -0.080000, -0.260000, -0.080000, -0.420000, -0.080000, -0.580000, 0.080000, 
-0.580000, 0.100000, -0.580000, 0.080000, -0.420000, -0.100000, -0.580000, -0.080000, -0.600000, -0.080000, -0.600000, 0.080000, -0.600000, 0.080000, -0.260000, 
0.080000, -0.420000, -0.080000, -0.420000, 0.080000, -0.420000, 0.080000, -0.680000, -0.080000, -0.680000, 0.080000, -0.600000, 0.100000, -0.580000, 0.040000, 
-0.080000, -0.040000, -0.080000, -0.060000, -0.260000, 0.060000, -0.260000, 0.020000, -0.620000, 0.020000, -0.600000, 0.040000, -0.620000, 0.060000, -0.628038,
 0.060000, -0.628038, 0.074641, -0.650000, 0.074641, -0.650000, 0.080000, -0.680000, 0.040000, -0.620000, 0.020000, -0.628038, 0.020000, -0.628038, 0.005359, 
-0.650000, 0.005359, -0.650000, -0.000000, -0.680000, -0.040000, -0.620000, -0.020000, -0.628038, -0.020000, -0.628038, -0.005359, -0.650000, -0.005359, -0.650000,
 -0.000000, -0.680000, -0.040000, -0.620000, -0.060000, -0.628038, -0.060000, -0.628038, -0.074641, -0.650000, -0.074641, -0.650000, -0.080000, -0.680000, -0.020000,
 0.040000, -0.020000, 0.200000, -0.020000, 0.200000, -0.000000, 0.220000, -0.000000, 0.220000, 0.020000, 0.200000, 0.020000, 0.200000, 0.020000, 0.040000, 0.020000,
 0.060000, -0.020000, 0.100000, -0.020000, 0.060000, 0.020000, 0.100000, 0.020000, 0.120000, -0.020000, 0.160000, 0.020000, 0.160000, -0.020000, 0.120000, 0.020000,
 0.040000, 0.020000, 0.020000, 0.020000, 0.020000, 0.060000, -0.040000, 0.060000, -0.040000, 0.040000, -0.060000, 0.040000, -0.060000, 0.060000, -0.080000, 0.040000,
 -0.060000, -0.040000, -0.060000, -0.040000, -0.040000, 0.040000, -0.040000, 0.040000, -0.200000, -0.040000, -0.200000, 0.060000, -0.080000, 0.060000, -0.200000,
 -0.080000, -0.420000, -0.100000, -0.580000, 0.060000, -0.620000, 0.060000, -0.600000, -0.020000, -0.620000, -0.020000, -0.600000, -0.060000, -0.620000, -0.060000,
 -0.600000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, array, 106, 2, red, green, blue);
	glPopMatrix();
}

void co_game_draw_station(float pos_x, float pos_y, float rot, float red, float green, float blue)
{
	static float array[] = {-0.020000, 0.180000, -0.040000, 0.174641, -0.040000, 0.174641, -0.054641, 0.160000, -0.054641, 0.160000, -0.060000, 0.140000, 0.020000, 
0.180000, 0.040000, 0.174641, 0.040000, 0.174641, 0.054641, 0.160000, 0.054641, 0.160000, 0.060000, 0.140000, 0.020000, 0.060000, 0.040000, 0.065359, 0.040000, 
0.065359, 0.054641, 0.080000, 0.054641, 0.080000, 0.060000, 0.100000, -0.020000, 0.060000, -0.040000, 0.065359, -0.040000, 0.065359, -0.054641, 0.080000, -0.054641,
 0.080000, -0.060000, 0.100000, -0.020000, 0.180000, 0.020000, 0.180000, 0.060000, 0.140000, 0.060000, 0.100000, -0.060000, 0.140000, -0.060000, 0.100000, -0.020000,
 0.060000, -0.020000, 0.040000, -0.020000, 0.040000, -0.040000, 0.020000, -0.040000, 0.020000, -0.040000, -0.080000, -0.040000, -0.080000, -0.020000, -0.100000, 
-0.020000, -0.100000, -0.020000, -0.120000, -0.060000, -0.120000, -0.060000, -0.260000, 0.020000, -0.100000, 0.020000, -0.120000, 0.020000, -0.100000, 0.040000, 
-0.080000, 0.040000, -0.080000, 0.040000, 0.020000, 0.040000, 0.020000, 0.020000, 0.040000, 0.020000, 0.040000, 0.020000, 0.060000, 0.060000, -0.120000, 0.060000,
 -0.260000, 0.060000, -0.120000, -0.060000, -0.120000, -0.020000, -0.080000, 0.020000, -0.080000, 0.020000, 0.020000, -0.020000, 0.020000, 0.060000, -0.260000, 
0.040000, -0.280000, 0.020000, -0.300000, -0.020000, -0.300000, 0.020000, -0.280000, 0.040000, -0.300000, 0.040000, -0.300000, 0.040000, -0.400000, -0.020000, 
-0.280000, -0.040000, -0.300000, -0.040000, -0.300000, -0.040000, -0.400000, -0.040000, -0.400000, -0.020000, -0.420000, -0.020000, -0.420000, 0.020000, -0.420000,
 0.020000, -0.420000, 0.040000, -0.400000, -0.020000, -0.400000, 0.020000, -0.400000, -0.040000, -0.280000, -0.060000, -0.260000, -0.540000, -0.060000, -0.540000, 
-0.100000, -0.120000, 0.040000, -0.560000, 0.040000, 0.040000, -0.280000, -0.040000, -0.280000, -0.560000, 0.040000, -0.560000, -0.120000, 0.560000, 0.040000, 
0.560000, -0.120000, 0.380000, 0.020000, 0.380000, -0.020000, 0.420000, 0.020000, 0.420000, -0.020000, 0.120000, -0.120000, 0.100000, -0.060000, 0.100000, -0.020000,
 0.120000, 0.040000, 0.060000, -0.040000, 0.540000, -0.040000, 0.180000, 0.020000, 0.180000, -0.020000, 0.140000, 0.020000, 0.140000, -0.020000, 0.220000, 0.020000, 
0.220000, -0.020000, 0.260000, 0.020000, 0.260000, -0.020000, 0.300000, 0.020000, 0.300000, -0.020000, 0.340000, 0.020000, 0.340000, -0.020000, -0.100000, -0.060000,
 -0.120000, -0.120000, 0.420000, -0.060000, 0.420000, -0.100000, 0.380000, -0.060000, 0.380000, -0.100000, 0.340000, -0.060000, 0.340000, -0.100000, 0.300000, -0.060000,
 0.300000, -0.100000, 0.260000, -0.060000, 0.260000, -0.100000, 0.220000, -0.060000, 0.220000, -0.100000, 0.180000, -0.060000, 0.180000, -0.100000, 0.140000, -0.060000, 
0.140000, -0.100000, -0.120000, 0.040000, -0.100000, -0.020000, -0.140000, 0.020000, -0.140000, -0.020000, -0.180000, 0.020000, -0.180000, -0.020000, -0.220000, 0.020000,
 -0.220000, -0.020000, -0.260000, 0.020000, -0.260000, -0.020000, -0.300000, 0.020000, -0.300000, -0.020000, -0.340000, 0.020000, -0.340000, -0.020000, -0.380000, 0.020000,
 -0.380000, -0.020000, -0.420000, 0.020000, -0.420000, -0.020000, -0.140000, -0.060000, -0.140000, -0.100000, 0.560000, -0.120000, 0.120000, -0.120000, -0.100000, 0.200000,
 -0.120000, 0.180000, -0.060000, -0.040000, -0.540000, -0.040000, -0.540000, 0.020000, -0.540000, -0.020000, -0.420000, -0.060000, -0.420000, -0.100000, -0.380000, -0.060000,
 -0.380000, -0.100000, -0.340000, -0.060000, -0.340000, -0.100000, -0.300000, -0.060000, -0.300000, -0.100000, -0.260000, -0.060000, -0.260000, -0.100000, -0.180000, 
-0.060000, -0.180000, -0.100000, -0.220000, -0.060000, -0.220000, -0.100000, -0.560000, -0.120000, -0.120000, -0.120000, 0.260000, 0.300000, 0.180000, 0.380000, 0.100000,
 0.200000, 0.120000, 0.180000, 0.120000, 0.220000, 0.140000, 0.200000, 0.140000, 0.240000, 0.160000, 0.220000, 0.160000, 0.260000, 0.180000, 0.240000, 0.180000, 0.280000,
 0.200000, 0.260000, 0.200000, 0.300000, 0.220000, 0.280000, 0.220000, 0.320000, 0.240000, 0.300000, 0.040000, 0.240000, 0.180000, 0.380000, 0.260000, 0.300000, 0.120000,
 0.160000, 0.120000, 0.160000, 0.080000, 0.180000, 0.060000, 0.200000, 0.040000, 0.240000, 0.200000, 0.320000, 0.060000, 0.180000, 0.200000, 0.340000, 0.180000, 0.360000,
 0.180000, 0.320000, 0.160000, 0.340000, 0.160000, 0.300000, 0.140000, 0.320000, 0.140000, 0.280000, 0.120000, 0.300000, 0.120000, 0.260000, 0.100000, 0.280000, 0.100000,
 0.240000, 0.080000, 0.260000, 0.080000, 0.220000, 0.060000, 0.240000, -0.060000, 0.180000, -0.040000, 0.240000, -0.260000, 0.300000, -0.120000, 0.160000, -0.180000, 
0.380000, -0.260000, 0.300000, -0.200000, 0.320000, -0.060000, 0.180000, -0.040000, 0.240000, -0.180000, 0.380000, -0.060000, 0.180000, -0.120000, 0.160000, -0.180000,
 0.360000, -0.200000, 0.340000, -0.160000, 0.340000, -0.180000, 0.320000, -0.140000, 0.320000, -0.160000, 0.300000, -0.120000, 0.300000, -0.140000, 0.280000, -0.100000,
 0.280000, -0.120000, 0.260000, -0.080000, 0.260000, -0.100000, 0.240000, -0.060000, 0.240000, -0.080000, 0.220000, -0.220000, 0.320000, -0.240000, 0.300000, -0.200000,
 0.300000, -0.220000, 0.280000, -0.180000, 0.280000, -0.200000, 0.260000, -0.160000, 0.260000, -0.180000, 0.240000, -0.140000, 0.240000, -0.160000, 0.220000, -0.120000,
 0.220000, -0.140000, 0.200000, 0.460000, 0.020000, 0.460000, -0.020000, 0.500000, 0.020000, 0.500000, -0.020000, 0.540000, 0.020000, 0.540000, -0.020000, 0.460000, 
-0.060000, 0.460000, -0.100000, 0.500000, -0.060000, 0.500000, -0.100000, 0.540000, -0.060000, 0.540000, -0.100000, -0.460000, 0.020000, -0.460000, -0.020000, -0.500000,
 0.020000, -0.500000, -0.020000, 0.120000, 0.040000, 0.560000, 0.040000, -0.460000, -0.060000, -0.460000, -0.100000, -0.500000, -0.060000, -0.500000, -0.100000, -0.020000,
 -0.420000, -0.040000, -0.440000, -0.040000, -0.440000, -0.040000, -0.480000, -0.040000, -0.480000, -0.020000, -0.500000, -0.020000, -0.500000, 0.020000, -0.500000, 
0.020000, -0.500000, 0.040000, -0.480000, 0.040000, -0.480000, 0.040000, -0.440000, 0.040000, -0.440000, 0.020000, -0.420000, 0.040000, -0.440000, 0.060000, -0.420000,
 0.060000, -0.420000, 0.200000, -0.420000, 0.200000, -0.420000, 0.220000, -0.440000, 0.220000, -0.440000, 0.220000, -0.480000, 0.220000, -0.480000, 0.200000, -0.500000,
 0.200000, -0.500000, 0.060000, -0.500000, 0.060000, -0.500000, 0.040000, -0.480000, 0.060000, -0.420000, 0.060000, -0.500000, 0.200000, -0.420000, 0.200000, -0.500000,
 -0.040000, -0.440000, -0.060000, -0.420000, -0.060000, -0.420000, -0.200000, -0.420000, -0.060000, -0.500000, -0.200000, -0.500000, 0.060000, -0.460000, 0.120000, 
-0.460000, -0.060000, -0.420000, -0.060000, -0.500000, -0.200000, -0.420000, -0.200000, -0.500000, -0.200000, -0.420000, -0.220000, -0.440000, -0.220000, -0.440000,
 -0.220000, -0.480000, -0.220000, -0.480000, -0.200000, -0.500000, -0.040000, -0.480000, -0.060000, -0.500000, -0.020000, -0.500000, -0.060000, -0.540000, -0.060000,
 -0.540000, -0.060000, -0.740000, -0.060000, -0.740000, -0.040000, -0.760000, -0.040000, -0.760000, 0.040000, -0.760000, 0.040000, -0.760000, 0.060000, -0.740000,
 0.060000, -0.740000, 0.060000, -0.540000, 0.060000, -0.540000, 0.020000, -0.500000, 0.020000, -0.760000, 0.020000, -0.780000, -0.020000, -0.760000, -0.020000,
 -0.780000, -0.020000, -0.780000, 0.020000, -0.780000, -0.000000, -0.780000, 0.060000, -0.840000, -0.000000, -0.780000, -0.060000, -0.840000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, array, 352, 2, red, green, blue);
	glPopMatrix();
}

void co_game_draw_sputnik(float pos_x, float pos_y, float rot, float red, float green, float blue)
{
	static float array[] = {-0.000000, -0.100000, 0.050000, -0.086603, 0.050000, -0.086603, 0.086603, -0.050000, 0.086603, -0.050000, 0.100000, 0.000000, -0.000000, 
0.100000, 0.050000, 0.086603, 0.050000, 0.086603, 0.086603, 0.050000, 0.086603, 0.050000, 0.100000, 0.000000, -0.000000, 0.100000, -0.050000, 0.086603, -0.050000, 
0.086603, -0.086603, 0.050000, -0.086603, 0.050000, -0.100000, 0.000000, -0.000000, -0.100000, -0.050000, -0.086603, -0.050000, -0.086603, -0.086603, -0.050000, 
-0.086603, -0.050000, -0.100000, 0.000000, -0.040000, 0.060000, -0.040000, 0.040000, -0.040000, 0.040000, 0.040000, 0.040000, 0.040000, 0.040000, 0.040000, 0.060000, 
0.040000, 0.060000, -0.040000, 0.060000, 0.080000, -0.080000, 0.240000, -0.240000, -0.080000, -0.080000, -0.240000, -0.240000, 0.040000, -0.100000, 0.040000, -0.120000,
 -0.040000, -0.120000, -0.040000, -0.100000, 0.040000, -0.120000, 0.020000, -0.140000, 0.020000, -0.140000, -0.020000, -0.140000, -0.040000, -0.120000, -0.020000, 
-0.140000, -0.000000, -0.160000, -0.000000, -0.580000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, array, 48, 2, red, green, blue);
	glPopMatrix();
}

void co_game_draw_rocket(float pos_x, float pos_y, float rot, float red, float green, float blue)
{
	static float array[] = {-0.040000, 0.000000, 0.040000, 0.000000, 0.040000, 0.000000, 0.080000, -0.040000, 0.080000, -0.040000, 0.080000, -0.100000, 0.080000, -0.100000, 0.040000, -0.140000, -0.000000, 
-0.180000, 0.020000, -0.185359, 0.020000, -0.185359, 0.034641, -0.200000, 0.034641, -0.200000, 0.040000, -0.220000, 0.060000, -0.140000, 0.100000, -0.180000, 0.100000, -0.180000, -0.100000, -0.180000, 
0.080000, -0.180000, 0.120000, -0.240000, 0.100000, -0.240000, 0.120000, -0.260000, 0.120000, -0.260000, 0.140000, -0.240000, 0.140000, -0.240000, 0.100000, -0.240000, 0.120000, -0.240000, 0.020000, 
-0.180000, -0.020000, -0.180000, -0.120000, -0.240000, -0.080000, -0.180000, -0.120000, -0.240000, -0.140000, -0.240000, -0.100000, -0.240000, -0.100000, -0.240000, -0.120000, -0.260000, -0.140000, 
-0.240000, -0.120000, -0.260000, -0.100000, -0.180000, -0.060000, -0.140000, -0.000000, -0.180000, -0.020000, -0.185359, -0.020000, -0.185359, -0.034641, -0.200000, -0.034641, -0.200000, -0.040000, 
-0.220000, -0.040000, -0.140000, -0.080000, -0.100000, -0.080000, -0.100000, -0.080000, -0.040000, -0.080000, -0.040000, -0.040000, 0.000000, -0.040000, -0.220000, 0.040000, -0.220000, 0.060000, 
-0.140000, -0.060000, -0.140000, 0.040000, 0.000000, 0.040000, -0.140000, 0.080000, -0.040000, -0.080000, -0.040000, -0.080000, -0.100000, 0.080000, -0.100000, -0.040000, 0.000000, -0.040000, -0.140000,
 -0.020000, 0.000000, -0.020000, 0.020000, -0.020000, 0.020000, -0.100000, 0.160000, -0.100000, 0.160000, 0.100000, 0.160000, 0.100000, 0.160000, 0.020000, 0.020000, 0.020000, 0.020000, 0.020000, 
0.000000, 0.020000, 0.060000, 0.040000, 0.100000, -0.020000, 0.060000, -0.020000, 0.100000, -0.040000, 0.100000, -0.020000, 0.060000, -0.020000, 0.100000, -0.040000, 0.100000, 0.020000, 0.100000, 
0.020000, 0.060000, 0.040000, 0.100000, 0.020000, 0.100000, -0.000000, 0.200000, 0.050000, 0.194641, 0.050000, 0.194641, 0.086603, 0.180000, 0.086603, 0.180000, 0.100000, 0.160000, -0.000000, 0.200000,
 -0.050000, 0.194641, -0.050000, 0.194641, -0.086603, 0.180000, -0.086603, 0.180000, -0.100000, 0.160000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, -1);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, array, 98, 2, red, green, blue);
	glPopMatrix();
}

#define SHIP_COUNT 8

boolean co_draw_ships(BInputState *input, boolean clickable)
{
	static float pos[SHIP_COUNT][2], vec[SHIP_COUNT][2], rot[SHIP_COUNT];
	static uint rand[SHIP_COUNT];
	static boolean init = FALSE;
	uint i;
	if(!init)
	{
		for(i = 0; i < SHIP_COUNT; i++)
		{
			float f;
			rand[i] = i * 50 + i;
			pos[i][0] = (get_rand(rand[i] * 4) - 0.5);
			pos[i][1] = (get_rand(rand[i] * 4 + 1) - 0.5);
			vec[i][0] = get_rand(rand[i] * 4 + 2) - 0.5;
			vec[i][1] = get_rand(rand[i] * 4 + 3) - 0.5;
			f = sqrt(vec[i][0] * vec[i][0] + vec[i][1] * vec[i][1]) * 2;
			vec[i][0] /= f;
			vec[i][1] /= f;
			rot[i] = 0;
		}
		init = TRUE;
	}
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < SHIP_COUNT; i++)
		{
			float color = 0.9;
			pos[i][0] += vec[i][0] * betray_get_delta_time();
			pos[i][1] += vec[i][1] * betray_get_delta_time();
			rot[i] += betray_get_delta_time() * (get_rand(rand[i]) - 0.5) * 100;
			if(2 * 2 < pos[i][0] * pos[i][0] + pos[i][1] * pos[i][1])
			{
				float f;
				pos[i][0] = get_rand(rand[i] * 4) - 0.5;
				pos[i][1] = get_rand(rand[i] * 4 + 1) - 0.5;
				f = sqrt(pos[i][0] * pos[i][0] + pos[i][1] * pos[i][1]) / 1.9;
				pos[i][0] /= f;
				pos[i][1] /= f;
				vec[i][0] = get_rand(rand[i] * 4 + 2) - 0.5;
				vec[i][1] = get_rand(rand[i] * 4 + 3) - 0.5;
				f = sqrt(vec[i][0] * vec[i][0] + vec[i][1] * vec[i][1]) * 4;
				vec[i][0] /= f;
				vec[i][1] /= f;
				rand[i]++;
			}
			if(clickable)
				color = 0.8;
			switch(rand[i] % 4)
			{
				case 0 :
				co_game_draw_lander(pos[i][0], pos[i][1], rot[i], color, color, color);
				break;
				case 1 :
				co_game_draw_station(pos[i][0], pos[i][1], rot[i], color, color, color);
				break;
				case 2 :
				co_game_draw_sputnik(pos[i][0], pos[i][1], rot[i], color, color, color);
				break;
				case 3 :
				co_game_draw_rocket(pos[i][0], pos[i][1], rot[i], color, color, color);
				break;
			}
		}
	}else if(clickable)
	{
		for(i = 0; i < SHIP_COUNT; i++)
			if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == TRUE)
				if(0.1 * 0.1 > (pos[i][0] - input->pointer_x) * (pos[i][0] - input->pointer_x) + (pos[i][1] - input->pointer_y) * (pos[i][1] - input->pointer_y))
					return TRUE;
	}
	return FALSE;
}

void co_pos_wrap(float *pos)
{
	if(pos[0] > 1)
		pos[0] -= 2; 
	if(pos[0] < -1)
		pos[0] += 2; 
	if(pos[1] > COGame.aspect)
		pos[1] -= 2 * COGame.aspect; 
	if(pos[1] < -COGame.aspect)
		pos[1] += 2 * COGame.aspect; 
}

void co_draw_dust(void)
{
	boolean alive = FALSE;
	COParticle *p;
	uint i;
	glPushMatrix();
	glTranslatef(0, 0, -1);
	for(i = 0; i < GAME_DUST_COUNT; i++)
	{
		float color = 0;
		p = &COGame.dust[i];
		if(p->time < 1)
		{
			color = 1 - COGame.dust[i].time;
			sui_draw_3d_line_gl(p->pos[0], p->pos[1], p->pos[2], p->pos[0] + p->vec[0] * 0.2 * color, p->pos[1] + p->vec[1] * 0.2 * color, p->pos[2] + p->vec[2] * 0.2 * color, p->time, p->time, p->time);
			alive = TRUE;
		}
	}
	if(!alive && (COGame.lifes == 0 || COGame.pebbel_count == 0))
		active_game = FALSE;
	glPopMatrix();
}

void co_play_game(BInputState *input)
{
	uint i, j, k;
	if(input->mode == BAM_DRAW)
	{
		COParticle *p;
		COGame.aspect = betray_get_screen_mode(NULL, NULL, NULL);
		COGame.time += betray_get_delta_time();
		co_draw_dust();
		if(COGame.lifes == 0)
		{
			glPushMatrix();
			glTranslatef(0, 0, -1);
			co_draw_game_over(0, 0, 0, 0, 0);
			glPopMatrix();
			return;
		}
		glPushMatrix();
		glTranslatef(0, 0, -1);

	/*	sui_draw_ship_1(0.5, 0, 0, 0, 0);*/
		glPushMatrix();
		glScalef(0.2, 0.2, 0.2);
	/*	sui_draw_ship_2(-0.5, 0, 0.1, 0.1, 0.1);
		draw_ship3(-1, 1, 0.1, 0.1, 0.1);
		draw_ship4(-0, 1, 0.1, 0.1, 0.1);
		draw_ship5(1, 1, 0.1, 0.1, 0.1);
		draw_ship6(2, 1, 0.1, 0.1, 0.1);*/
		glPopMatrix();
		glPushMatrix();
		glTranslatef(COGame.player_pos[0], COGame.player_pos[1], 0);
		glRotatef(COGame.player_rot, 0, 0, 1);
		if(COGame.engine)
			co_draw_ship_engine(0, 0, 0, 0, 0);
		else
			co_draw_ship(0, 0, 0, 0, 0);
		glPopMatrix();
		for(i = 0; i < COGame.lifes; i++)
			co_draw_ship(0.95 - 0.04 * (float)i, COGame.aspect - 0.05, 0, 0, 0);
		for(i = 0; i < COGame.pebbel_count; i++)
		{
			glPushMatrix();
			glTranslatef(COGame.pebbels[i].pos[0], COGame.pebbels[i].pos[1], 0);
			glScalef(COGame.pebbels[i].size, COGame.pebbels[i].size, COGame.pebbels[i].size);
			if(COGame.pebbels[i].generation > 0)
			{
				glRotatef(COGame.time * 20 * ((float)(i % 5) - 2.32), 0, 0, 1);
				switch(i % 4)
				{
					case 0 :
					co_draw_pebbel1(0, 0, 0, 0, 0);
					break;
					case 1 :
					co_draw_pebbel2(0, 0, 0, 0, 0);
					break;
					case 2 :
					co_draw_pebbel3(0, 0, 0, 0, 0);
					break;
					case 3 :
					co_draw_pebbel4(0, 0, 0, 0, 0);
					break;
				}

			}else
			{
				co_vng_ring();
				switch(COGame.pebbels[i].type)
				{
					case V_NT_OBJECT :
						co_vng_sun();
						co_vng_sunrays();
					break;
					case V_NT_GEOMETRY :
						co_vng_saturn_ring();
						co_vng_gas_planet();
					break;
					case V_NT_MATERIAL :
					break;
					case V_NT_BITMAP :
						co_vng_dust_planet();
					break;
					case V_NT_TEXT :
						co_vng_geo_planet();
					break;
					case V_NT_CURVE :
						co_vng_dust_planet();
					break;
				}
			}
			glPopMatrix();
		}

		for(i = 0; i < GAME_BULLET_COUNT; i++)
		{
			if(COGame.fire[i].time < 1)
				sui_draw_2d_line_gl(COGame.fire[i].pos[0],
					COGame.fire[i].pos[1],
					COGame.fire[i].pos[0] + COGame.fire[i].vec[0] * 0.05,
					COGame.fire[i].pos[1] + COGame.fire[i].vec[1] * 0.05, 0, 0, 0);
		}
	/*	for(i = 0; i < GAME_DUST_COUNT; i++)
		{
			float color = 0;
			p = &COGame.dust[i];
			if(p->time < 1)
			{
				color = 1 - COGame.dust[i].time;
				sui_draw_3d_line_gl(p->pos[0], p->pos[1], p->pos[2], p->pos[0] + p->vec[0] * 0.2 * color, p->pos[1] + p->vec[1] * 0.2 * color, p->pos[2] + p->vec[2] * 0.2 * color, p->time, p->time, p->time);
			}
		}*/
		glPopMatrix();

	}else
	{
		static boolean d = FALSE, d_l = FALSE, a = FALSE, a_l = FALSE, w = FALSE, w_l = FALSE, s = FALSE, s_l = FALSE, space = FALSE, space_l = FALSE;
		betray_get_key_up_down(&d, &d_l, 'd');
		betray_get_key_up_down(&a, &a_l, 'a');
		betray_get_key_up_down(&w, &w_l, 'w');
		betray_get_key_up_down(&s, &s_l, 's');
		betray_get_key_up_down(&space, &space_l, ' ');
		if(d)
			COGame.player_rot -= betray_get_delta_time() * 200;
		if(a)
			COGame.player_rot += betray_get_delta_time() * 200;
		if(w)
		{
			COGame.player_vec[0] *= 0.9 + (1 - betray_get_delta_time()) * 0.1;
			COGame.player_vec[1] *= 0.9 + (1 - betray_get_delta_time()) * 0.1;
			COGame.player_vec[0] -= sin(COGame.player_rot * PI / 180) * betray_get_delta_time();
			COGame.player_vec[1] += cos(COGame.player_rot * PI / 180) * betray_get_delta_time();
		}
		COGame.engine = w;
		if(s)
		{
			COGame.player_vec[0] *= 0.9;
			COGame.player_vec[1] *= 0.9;
		}
		COGame.player_pos[0] += COGame.player_vec[0] * betray_get_delta_time();
		COGame.player_pos[1] += COGame.player_vec[1] * betray_get_delta_time();

		COGame.fire_time += betray_get_delta_time();
		if(space && (COGame.fire_time > 0.23 /*|| space_l == FALSE*/))
		{

			COGame.fire[COGame.fire_next].vec[0] = -sin(COGame.player_rot * PI / 180);
			COGame.fire[COGame.fire_next].vec[1] = cos(COGame.player_rot * PI / 180);
			COGame.fire[COGame.fire_next].pos[0] = COGame.player_pos[0] + COGame.fire[COGame.fire_next].vec[0] * 0.04;
			COGame.fire[COGame.fire_next].pos[1] = COGame.player_pos[1] + COGame.fire[COGame.fire_next].vec[1] * 0.04;
			COGame.fire[COGame.fire_next].time = 0;
			COGame.fire_next = (COGame.fire_next + 1) % GAME_BULLET_COUNT;
			COGame.fire_time = 0;
		}

		co_pos_wrap(COGame.player_pos);

		for(i = 0; i < GAME_BULLET_COUNT; i++)
		{
			if(COGame.fire[i].time < 1)
			{
				COGame.fire[i].time += betray_get_delta_time();
				COGame.fire[i].pos[0] += COGame.fire[i].vec[0] * betray_get_delta_time();
				COGame.fire[i].pos[1] += COGame.fire[i].vec[1] * betray_get_delta_time();
				co_pos_wrap(COGame.fire[i].pos);
			}
		}
		for(i = 0; i < GAME_DUST_COUNT; i++)
		{
			if(COGame.dust[i].time < 1)
			{
				COGame.dust[i].time += betray_get_delta_time() * 0.2;
				COGame.dust[i].pos[0] += COGame.dust[i].vec[0] * betray_get_delta_time();
				COGame.dust[i].pos[1] += COGame.dust[i].vec[1] * betray_get_delta_time();
				COGame.dust[i].pos[2] += COGame.dust[i].vec[2] * betray_get_delta_time();
				co_pos_wrap(COGame.dust[i].pos);
			}
		}

		for(i = 0; i < COGame.pebbel_count; i++)
		{
			float x, y, size;
			COGame.pebbels[i].pos[0] += COGame.pebbels[i].vec[0] * betray_get_delta_time();
			COGame.pebbels[i].pos[1] += COGame.pebbels[i].vec[1] * betray_get_delta_time();
			co_pos_wrap(COGame.pebbels[i].pos);
			size = COGame.pebbels[i].size * COGame.pebbels[i].size * 0.4 * 0.4;
			if(size > (COGame.pebbels[i].pos[0] - COGame.player_pos[0]) * (COGame.pebbels[i].pos[0] - COGame.player_pos[0]) + (COGame.pebbels[i].pos[1] - COGame.player_pos[1]) * (COGame.pebbels[i].pos[1] - COGame.player_pos[1]))
			{
				co_spread_dust(COGame.player_pos, COGame.player_vec, 2, 0.2, 192);
				COGame.lifes--;
				if(COGame.lifes == 0)
					co_end_game();
				COGame.player_pos[0] = 0;
				COGame.player_pos[1] = 0;
				COGame.player_vec[0] = 0;
				COGame.player_vec[1] = 0;
				COGame.player_rot = 0;
			}
			for(j = 0; j < GAME_BULLET_COUNT; j++)
			{
				if(COGame.fire[j].time < 1)
				{
					x = COGame.pebbels[i].pos[0] - COGame.fire[j].pos[0];
					y = COGame.pebbels[i].pos[1] - COGame.fire[j].pos[1];
					if(size > x * x + y * y)
					{
						COGame.fire[j].time = 10;
						COGame.pebbels[i].generation++;
						co_spread_dust(COGame.pebbels[i].pos, COGame.fire[j].vec, 1 + 1 / COGame.pebbels[i].generation, COGame.pebbels[i].size, 192 / COGame.pebbels[i].generation);
						if(COGame.pebbels[i].generation < 4)
						{
	
							COGame.pebbels[i].size *= 0.6;
							COGame.pebbels[COGame.pebbel_count] = COGame.pebbels[i];
							COGame.pebbels[i].vec[0] = COGame.pebbels[i].vec[0] * 0.2 + COGame.fire[j].vec[1] * (0.1 + 0.08 * (float)COGame.pebbels[i].generation);
							COGame.pebbels[i].vec[1] = COGame.pebbels[i].vec[1] * 0.2 - COGame.fire[j].vec[0] * (0.1 + 0.08 * (float)COGame.pebbels[i].generation);
							COGame.pebbels[i].pos[0] += COGame.pebbels[i].vec[0] * 0.2 * COGame.pebbels[i].size;
							COGame.pebbels[i].pos[1] += COGame.pebbels[i].vec[1] * 0.2 * COGame.pebbels[i].size;
							COGame.pebbels[COGame.pebbel_count].vec[0] = COGame.pebbels[COGame.pebbel_count].vec[0] * 0.2 - COGame.fire[j].vec[1] * (0.1 + 0.08 * (float)COGame.pebbels[i].generation);
							COGame.pebbels[COGame.pebbel_count].vec[1] = COGame.pebbels[COGame.pebbel_count].vec[0] * 0.2 + COGame.fire[j].vec[0] * (0.1 + 0.08 * (float)COGame.pebbels[i].generation);
							COGame.pebbels[COGame.pebbel_count].pos[0] += COGame.pebbels[COGame.pebbel_count].vec[0] * 0.2 * COGame.pebbels[i].size;
							COGame.pebbels[COGame.pebbel_count].pos[1] += COGame.pebbels[COGame.pebbel_count].vec[1] * 0.2 * COGame.pebbels[i].size;
							COGame.pebbel_count++;
						}else
						{
							COGame.pebbels[i] = COGame.pebbels[--COGame.pebbel_count];
							if(COGame.pebbel_count == 0)
								co_end_game();
						}
						break;
					}
				}
			}
		}
	}
}
