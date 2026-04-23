#include "MapManager.h"
#include "raylib.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "PlayerManager.h"
#include "Enemy.h"

// Level Templates
static const int LEVEL_1_DATA[MAP_ROWS][MAP_COLS] = {
    {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0} ,
    {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0} ,
    {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0} ,
    {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0} ,
    {0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 ,22 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0} ,
    {0 , 0 , 0 , 0 , 0 ,22 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 15, 15, 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 14, 0 , 15, 0 , 15, 15, 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0} ,
    {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 14, 0 , 0 , 0 , 0 , 0 , 15, 0 , 0 , 0 , 15, 15, 0 , 0 , 0 , 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,20 ,22 , 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,20 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,20 , 0 , 0} ,
    {0 ,20 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,23 ,22 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,19 , 15, 15, 0 , 0 , 0 , 0 , 0 , 0 , 14,20 , 0 , 0 ,19 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,19 , 0 , 0} ,
    {0 ,19 ,20 , 0 , 0 , 0 , 0 , 0 , 0 ,20 , 0 , 0 , 0 , 0 , 0 , 15,22 , 0 , 0 , 0 , 0 , 14, 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,20 , 0 , 0 ,19 , 14, 15, 0 , 0 , 0 , 15,22 , 0 , 15,19 , 0 , 15,19 , 0 , 15,20 , 0 , 0 , 0 , 0 , 0 , 0 , 14, 14, 14, 14, 14, 14, 14,20 , 0 ,20 , 0 , 0 , 0 , 0 , 0 ,38 , 0 , 0 , 0 ,19 , 0 , 0} ,
    {0 ,19 ,19 , 0 , 0 , 0 , 0 , 0 , 0 ,19 , 0 , 15, 15, 0 ,20 , 15,22 , 0 , 0 , 15, 0 , 15, 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 14,19 , 0 , 0 ,19 , 15, 15, 0 , 0 , 0 , 15,22 , 0 , 0 ,19 , 0 , 14,19 , 0 , 0 ,19 , 0 , 0 , 0 , 0 , 0 , 0 , 15, 15, 15, 15, 15, 15, 15,19 ,20 ,19 , 0 , 0 , 0 , 0 ,28 , 0 , 0 ,29 , 0 ,19 ,20 , 0} ,
    {0 ,19 ,19 , 0 , 0 , 0 , 0 , 15, 15,19 , 0 , 15, 15, 0 ,19 , 15, 14, 0 , 0 , 15, 0 , 0 , 0 , 0 , 15, 0 , 0 , 0 , 0 , 0 ,20 ,19 , 0 , 15,18 , 15,23 , 0 , 15, 0 , 15, 15, 0 , 0 ,19 , 0 , 0 ,18 , 0 , 0 ,19 , 14, 0 , 0 , 0 , 0 , 15, 15, 0 , 0 ,-2 , 0 , 0 , 15,19 ,19 ,19 , 0 , 0 , 0 , 0 ,30 ,32 ,33 ,31 ,25 ,18 ,19 , 0} ,
    {0 ,18 ,18 , 15, 15, 0 , 0 , 15,23 ,18 , 0 ,23 , 14, 0 ,19 , 15,23 , 0 ,23 , 14, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,19 ,18 , 0 , 14,18 , 15, 14, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,18 , 0 , 0 ,18 , 0 , 0 ,18 , 0 , 0 , 0 , 0 , 15, 15, 15, 0 ,22 ,22 ,22 , 0 , 14,18 ,19 ,18 , 0 , 0 , 0 , 0 ,30 ,34 ,35 ,31 , 0 ,18 ,19 , 0} ,
    {-9 ,18 ,18 , 15, 15, 0 , 0 , 15, 15,18 , 0 , 15, 14, 0 ,18 , 14, 15, 0 , 0 , 15, 0 , 0 , 0 , 0 , 0 ,-5 , 0 , 0 , 0 , 0 ,18 ,18 , 0 , 0 ,18 , 15, 15, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,18 , 0 , 0 ,18 , 0 , 0 ,18 ,-3 , 0 , 0 , 15, 15, 15, 14, 0 , 0 , 0 , 0 , 0 , 14,18 ,18 ,18 , 0 , 0 , 0 , 0 ,30 ,36 ,37 ,31 , 0 ,18 ,18 , 0} ,
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,-4 ,-4 ,-4 ,-4 ,12 , 12, 12, 12, 12, 12, 12, 12, 12,-4 ,-4 ,-4 ,-4 , 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 27, 27, 27, 27, 12, 12, 12,12} ,
    {13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,24 ,24 ,24 ,24 ,13 , 13, 13, 13, 13, 13, 13, 13, 13,24 ,24 ,24 ,24 , 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,13}
};

static const int LEVEL_2_DATA[MAP_ROWS][MAP2_COLS] = {
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,16 ,17 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 2 , 2 , 2 , 2 , 2 , 2 ,-7 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 ,-7 , 2 , 2 , 2 , 2 , 2 , 2 ,-7 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 2 , 2 , 2 , 2 , 2 , 2 ,-7 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 ,-7 , 2 , 2 , 2 , 2 , 2 , 2 ,-7 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 2 , 2 , 2 , 2 , 2 , 2 ,-7 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 3 , 0 , 0 , 0 , 0 ,14 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,50 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 0 , 0 ,-1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 0 , 0 , 0 ,-7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 , 0 ,-7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-8 , 0 , 0 ,-8 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 ,-7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,-3 , 0 , 0 , 0 , 0 , 0 ,14 , 0 , 0 , 0 ,14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
   { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 ,-6 , 1 ,-6 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1},
   { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 ,26 , 1 ,26 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}
};

static const int LEVEL_3_DATA[12][16] = {
    {41,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0,43, 0, 0, 0, 0,42, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0,99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

MapManager::MapManager() : tilesetLoaded(false), menuTexturesLoaded(false), currentLevel(1), currentCols(MAP_COLS), currentRows(MAP_ROWS) {
    tileset = LoadTexture("Sprites/tileset.png");
    if (tileset.id != 0) tilesetLoaded = true;

    shopTex = LoadTexture("Sprites/shop.png");
    doorTex = LoadTexture("Sprites/door.png");
    shopInteriorTex = LoadTexture("Sprites/shopinterior.png");
    oneUpTex = LoadTexture("Sprites/1up.png");
    exitTex = LoadTexture("Sprites/exit.png");

    SetTextureFilter(shopTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(doorTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(shopInteriorTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(oneUpTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(exitTex, TEXTURE_FILTER_POINT);

    LoadLevel(1);
}

MapManager::~MapManager() {
    if (tilesetLoaded) UnloadTexture(tileset);
    UnloadTexture(shopTex);
    UnloadTexture(doorTex);
    UnloadTexture(shopInteriorTex);
    UnloadTexture(oneUpTex);
    UnloadTexture(exitTex);
    UnloadMenuTextures();
}

void MapManager::LoadLevel(int levelIndex, bool returningFromShop) {
    if (currentLevel == 0 && levelIndex != 0) {
        UnloadMenuTextures();
    }

    currentLevel = levelIndex;
    enemySpawns.clear();
    std::memset(mapData, 0, sizeof(mapData));

    if (levelIndex == 0) {
        LoadMenuTextures();
        currentCols = 16;
        currentRows = 12;
        spawnPosition = { 0, 0 };
        return;
    }

    if (levelIndex == 3) {
        currentCols = 16;
        currentRows = 15;
        for (int r = 0; r < 15; r++) {
            std::memcpy(mapData[r], LEVEL_3_DATA[r], sizeof(LEVEL_3_DATA[r]));
        }
    } else if (levelIndex == 2) {
        currentCols = MAP2_COLS;
        currentRows = MAP_ROWS;
        for (int r = 0; r < MAP_ROWS; r++) {
            std::memcpy(mapData[r], LEVEL_2_DATA[r], sizeof(LEVEL_2_DATA[r]));
        }

        // --- INJECT SHOP INTO LEVEL 2 ---
        mapData[8][3] = 39;  // Draw shop exterior starting at row 8, col 3
        mapData[11][3] = 40; // Draw door interact hitbox at row 11, col 3
    } else {
        currentCols = MAP_COLS;
        currentRows = MAP_ROWS;
        for (int r = 0; r < MAP_ROWS; r++) {
            std::memcpy(mapData[r], LEVEL_1_DATA[r], sizeof(LEVEL_1_DATA[r]));
        }
    }

    // Scan for spawn marker (-1, -9) and enemies (-2 to -8)
    for (int r = 0; r < currentRows; r++) {
        for (int c = 0; c < currentCols; c++) {
            if (mapData[r][c] == -1) {
                // Default player spawn: Centered in tile
                spawnPosition = {
                    (float)c * TILE_SIZE + TILE_SIZE / 2.0f,
                    (float)r * TILE_SIZE + TILE_SIZE - PlayerManager::frameHeight
                };
                mapData[r][c] = 0;
            } else if (mapData[r][c] == -9) {
                // Player spawn: 2.0 blocks offset to the right
                spawnPosition = {
                    (float)c * TILE_SIZE + 2.0f * TILE_SIZE,
                    (float)r * TILE_SIZE + TILE_SIZE - PlayerManager::frameHeight
                };
                mapData[r][c] = 0;
            } else if (mapData[r][c] <= -2 && mapData[r][c] >= -8) {
                enemySpawns.push_back({
                    mapData[r][c],
                    {(float)c * TILE_SIZE, (float)r * TILE_SIZE}
                });
                mapData[r][c] = 0;
            }
        }
    }
}

void MapManager::DrawBackground() {
    Rectangle skySource = { 16, 16, 16, 16 }; // Row 2, Col 2 (1-indexed)

    for (int r = 0; r < currentRows; r++) {
        for (int c = 0; c < currentCols; c++) {
            Vector2 pos = { (float)c * TILE_SIZE, (float)r * TILE_SIZE };
            
            // Draw Sky as base layer
            if (tilesetLoaded) {
                DrawTextureRec(tileset, skySource, pos, WHITE);
            } else {
                DrawRectangleV(pos, { (float)TILE_SIZE, (float)TILE_SIZE }, SKYBLUE);
            }
        }
    }
}

void MapManager::DrawTiles(bool shop1UpBought) {
    for (int r = 0; r < currentRows; r++) {
        for (int c = 0; c < currentCols; c++) {
            Vector2 pos = { (float)c * TILE_SIZE, (float)r * TILE_SIZE };
            
            // Draw specific tiles (Skip 12 and 27, they are foreground)
            if (mapData[r][c] != 0 && mapData[r][c] != 12 && mapData[r][c] != 27) {
                DrawTile(mapData[r][c], pos, shop1UpBought);
            }
        }
    }
}

void MapManager::DrawForeground() {
    if (!tilesetLoaded) return;
    Rectangle source12 = { 32, 128, 16, 16 }; // Grass_2_top (Tile 12)
    Rectangle source27 = { 32, 192, 16, 16 }; // Castle's grass tile (Tile 27)

    for (int r = 0; r < currentRows; r++) {
        for (int c = 0; c < currentCols; c++) {
            if (mapData[r][c] == 12 || mapData[r][c] == 27) {
                Rectangle currentSource = (mapData[r][c] == 12) ? source12 : source27;
                Vector2 pos = { (float)c * TILE_SIZE, (float)r * TILE_SIZE };
                DrawTextureRec(tileset, currentSource, pos, WHITE);
            }
        }
    }
}

void MapManager::DrawTile(int tileID, Vector2 position, bool shop1UpBought) {
    if (tileID == 0) return;

    // Custom Full-Size Textures
    if (tileID == 39) {
        // The +9 offsets it perfectly to the ground
        DrawTexture(shopTex, (int)position.x, (int)position.y + 9, WHITE);
        return;
    }
    if (tileID == 41) {
        DrawTexture(shopInteriorTex, (int)position.x, (int)position.y, WHITE);
        return;
    }
    
    // Task 3: Tile 38 Rendering Offset (-8 pixels Y)
    if (tileID == 38) {
        position.y -= 8;
    }

    // Shop System Tiles
    if (tileID == 40) {
        DrawTexture(doorTex, (int)(position.x + doorOffsetX), (int)(position.y + doorOffsetY), WHITE);
        return;
    }
    if (tileID == 42) {
        if (!shop1UpBought) {
            DrawTexture(oneUpTex, (int)(position.x + oneUpOffsetX), (int)(position.y + oneUpOffsetY), WHITE);
        }
        return;
    }
    if (tileID == 43) {
        DrawTexture(exitTex, (int)(position.x + exitOffsetX), (int)(position.y + exitOffsetY), WHITE);
        return;
    }

    if (!tilesetLoaded) return;
    Rectangle source = { 0, 0, 16, 16 }; 
    switch (tileID) {
        case 1: source = { 32, 112, 16, 16 }; break; // Rock block
        case 2: source = {0, 48, 16, 16}; break; // Destroyable Block
        case 3: source = {16, 48, 16, 16}; break; // Big money
        case 4: source = {32, 16, 16, 16}; break; // Left dirt wall
        case 5: source = {0, 16, 16, 16}; break; // Right dirt Wall
        case 6: source = {0, 0, 16, 16}; break; // Right grass corner
        case 7: source = {32, 0, 16, 16}; break; // Left grass corner
        case 8: source = {0, 32, 16, 16}; break; // Right dirt corner
        case 9: source = {32, 32, 16, 16}; break; // Left dirt corner
        case 10: source = {16, 32, 16, 16}; break; // Dirt block
        case 11: source = {16, 16, 16, 16}; break; // Sky block
        case 12: source = {32, 128, 16, 16}; break; // Tall grass block
        case 13: source = {0, 144, 16, 16}; break; // Grass block
        case 14: source = {0, 128, 16, 16}; break; // Red block
        case 15: source = {16, 128, 16, 16}; break; // Blue block
        case 16: source = {16, 160, 16, 16}; break; // Left cloud
        case 17: source = {32, 160, 16, 16}; break; // Right cloud
        case 18: source = {0, 176, 16, 16}; break; // Trunk of tree
        case 19: source = {16, 176, 16, 16}; break; // Leaves of tree
        case 20: source = {32, 176, 16, 16}; break; // Top of tree
        case 21: source = {0, 160, 16, 16}; break; // Small money
        case 22: source = {32, 48, 16, 16}; break; // Star block
        case 23: source = {0, 64, 16, 16}; break; // Stun Block
        case 24: source = {32, 144, 16, 16}; break; // Lava block
        case 25: source = {16, 192, 16, 16}; break; // Onigiri tile
        case 26: source = {0, 192, 16, 16}; break; // Quicksand tile
        case 27: source = {32, 192, 16, 16}; break; // Castle's grass tile
        case 28: source = {0, 208, 16, 16}; break; // Castle's left corner tile
        case 29: source = {16, 208, 16, 16}; break; // Castle's right corner tile
        case 30: source = {0, 224, 16, 16}; break; // Castle's left wall tile
        case 31: source = {16, 224, 16, 16}; break; // Castle's right wall tile
        case 32: source = {0, 240, 16, 16}; break; // Castle's door's left corner tile
        case 33: source = {16, 240, 16, 16}; break; // Castle's door's right corner tile
        case 34: source = {0, 256, 16, 16}; break; // Castle's left door tile
        case 35: source = {16, 256, 16, 16}; break; // Castle's right door tile
        case 36: source = {0, 272, 16, 16}; break; // Castle's left old man tile
        case 37: source = {16, 272, 16, 16}; break; // Castle's right old man tile
        case 38: source = {0, 288, 32, 40}; break; // Castle's top piece
        case 50: source = {16, 192, 16, 16}; break; // Last Onigiri tile
        case 99: return;
    }
    DrawTextureRec(tileset, source, position, WHITE);
}

InteractionResult MapManager::InteractWithMap(Rectangle hitbox, int interactionType) {
    InteractionResult result = { 0, -1, -1 };
    
    int startCol = (int)floorf(hitbox.x / TILE_SIZE);
    int endCol = (int)floorf((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)floorf(hitbox.y / TILE_SIZE);
    int endRow = (int)floorf((hitbox.y + hitbox.height) / TILE_SIZE);

    if (startCol < 0) startCol = 0;
    if (endCol >= currentCols) endCol = currentCols - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    if (interactionType == 2) {
        int bestR = -1;
        int bestC = -1;
        float maxArea = 0.0f;

        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 2 || mapData[r][c] == 15 || mapData[r][c] == 22 || mapData[r][c] == 23) {
                    Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                    Rectangle overlap = GetCollisionRec(hitbox, tileRect);
                    float area = overlap.width * overlap.height;

                    if (area > maxArea) {
                        maxArea = area;
                        bestR = r;
                        bestC = c;
                    }
                }
            }
        }

        if (bestR != -1) {
            result.tileID = mapData[bestR][bestC];
            result.row = bestR;
            result.col = bestC;
            mapData[bestR][bestC] = 0;
        }
    } else if (interactionType == 3) {
        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 3 || mapData[r][c] == 21 || mapData[r][c] == 25 || mapData[r][c] == 50) {
                    Rectangle tileRect = { (float)c * TILE_SIZE + 1, (float)r * TILE_SIZE, (float)TILE_SIZE - 2, (float)TILE_SIZE };
                    if (CheckCollisionRecs(hitbox, tileRect)) {
                        result.tileID = mapData[r][c];
                        result.row = r;
                        result.col = c;
                        mapData[r][c] = 0;
                        return result; // Return immediately on first collectible hit
                    }
                }
            }
        }
    }
    
    return result;
}

bool MapManager::CheckCollision(Rectangle hitbox) const {
    int startCol = (int)floorf(hitbox.x / TILE_SIZE);
    int endCol = (int)floorf((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)floorf(hitbox.y / TILE_SIZE);
    int endRow = (int)floorf((hitbox.y + hitbox.height) / TILE_SIZE);

    if (startCol < 0) startCol = 0;
    if (endCol >= currentCols) endCol = currentCols - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    for (int r = startRow; r <= endRow; r++) {
        for (int c = startCol; c <= endCol; c++) {
            int tid = mapData[r][c];
            // Check solid tiles (Excluding 3 and 21 which are collectibles, and decorative tiles 28-38)
            // Added 27 to solid tiles
            bool isSolid = (tid >= 1 && tid <= 15 && tid != 3) || (tid >= 21 && tid <= 24 && tid != 21) || (tid == 27);
            
            if (isSolid) {
                float yPos = (float)r * TILE_SIZE;
                float height = (float)TILE_SIZE;
                
                // Custom Hitbox for Tile 12 (Tall Grass) and Tile 27 (Castle's Grass)
                if (tid == 12 || tid == 27) {
                    yPos += 6;
                    height -= 6;
                }
                
                Rectangle tileRect = { (float)c * TILE_SIZE, yPos, (float)TILE_SIZE, height };
                if (CheckCollisionRecs(hitbox, tileRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void MapManager::CullOffscreen(float leftEdgeX) {
    int lastColToDelete = (int)(leftEdgeX / TILE_SIZE) - 1;
    if (lastColToDelete < 0) return;
    if (lastColToDelete >= currentCols) lastColToDelete = currentCols - 1;

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c <= lastColToDelete; c++) {
            mapData[r][c] = 0;
        }
    }
}

Vector2 MapManager::GetSafeRespawnPosition(float leftCameraX, const std::vector<Enemy*>& enemies) {
    // Use ceil to completely ignore partially off-screen blocks
    int startCol = std::max(0, (int)std::ceil(leftCameraX / TILE_SIZE));
    int endCol = std::min(currentCols - 1, startCol + 16); 

    for (int c = startCol; c <= endCol; c++) {
        for (int r = 2; r < MAP_ROWS; r++) { // Start at 2 to check the two blocks above
            int groundTile = mapData[r][c];
            int space1 = mapData[r-1][c];
            int space2 = mapData[r-2][c];

            // Solidity check logic based on existing tiles (1-15, 21-24 are solid)
            bool groundSolid = (groundTile >= 1 && groundTile <= 15) || (groundTile >= 21 && groundTile <= 24);
            bool space1Solid = (space1 >= 1 && space1 <= 15) || (space1 >= 21 && space1 <= 24);
            bool space2Solid = (space2 >= 1 && space2 <= 15) || (space2 >= 21 && space2 <= 24);

            if (groundSolid && !space1Solid && !space2Solid) {
                // Check if an enemy is directly on this spot
                Rectangle spawnArea = { (float)c * TILE_SIZE, (float)(r-2) * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE * 2 };
                bool enemyBlocking = false;
                for (auto enemy : enemies) {
                    if (CheckCollisionRecs(spawnArea, enemy->GetHitbox())) {
                        enemyBlocking = true;
                        break;
                    }
                }

                if (!enemyBlocking) {
                    // No clamping needed anymore because we guarantee the block is fully on-screen
                    return { (float)c * TILE_SIZE, (float)(r-2) * TILE_SIZE };
                }
            }
        }
    }
    return GetSpawnPosition(); // Fallback to map start if no safe space exists
}


void MapManager::LoadMenuTextures() {
    if (menuTexturesLoaded) return;
    titlescreen = LoadTexture("Sprites/MainMenu/titlescreen.png");
    starttitle = LoadTexture("Sprites/MainMenu/starttitle.png");
    redtitle = LoadTexture("Sprites/MainMenu/redtitle.png");
    greentitle = LoadTexture("Sprites/MainMenu/greentitle.png");
    yellowtitle = LoadTexture("Sprites/MainMenu/yellowtitle.png");
    enemytitle = LoadTexture("Sprites/MainMenu/enemytitle.png");
    flytitle = LoadTexture("Sprites/MainMenu/flytitle.png");
    boattitle = LoadTexture("Sprites/MainMenu/boattitle.png");
    swimtitle = LoadTexture("Sprites/MainMenu/swimtitle.png");
    treetitle = LoadTexture("Sprites/MainMenu/treetitle.png");
    
    SetTextureFilter(titlescreen, TEXTURE_FILTER_POINT);
    SetTextureFilter(starttitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(redtitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(greentitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(yellowtitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(enemytitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(flytitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(boattitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(swimtitle, TEXTURE_FILTER_POINT);
    SetTextureFilter(treetitle, TEXTURE_FILTER_POINT);
    
    menuTexturesLoaded = true;
}

void MapManager::UnloadMenuTextures() {
    if (!menuTexturesLoaded) return;
    UnloadTexture(titlescreen);
    UnloadTexture(starttitle);
    UnloadTexture(redtitle);
    UnloadTexture(greentitle);
    UnloadTexture(yellowtitle);
    UnloadTexture(enemytitle);
    UnloadTexture(flytitle);
    UnloadTexture(boattitle);
    UnloadTexture(swimtitle);
    UnloadTexture(treetitle);
    menuTexturesLoaded = false;
}

void MapManager::DrawMainMenu(float menuTimer, int colorVariant) {
    if (!menuTexturesLoaded) return;

    const float MENU_STEP_INTERVAL = 0.2f;
    const Vector2 POS_SWIM     = { 167, 7   };
    const Vector2 POS_BOAT     = { 103, 119 };
    const Vector2 POS_TREE     = { 8,   7  };
    const Vector2 POS_FLY      = { 215, 63  };
    const Vector2 POS_ENEMY    = { 8,   71  };
    const Vector2 POS_TITLE    = { 61,  16  };

    // Draw background
    if (menuTimer >= 0.5f + 5 * MENU_STEP_INTERVAL) {
        DrawTexture(starttitle, 0, 0, WHITE);
    } else {
        DrawTexture(titlescreen, 0, 0, WHITE);
    }

    // Reveal stages (back to front order relative to each other)
    if (menuTimer >= 0.5f) DrawTexture(swimtitle, (int)POS_SWIM.x, (int)POS_SWIM.y, WHITE);
    if (menuTimer >= 0.5f + 1 * MENU_STEP_INTERVAL) DrawTexture(boattitle, (int)POS_BOAT.x, (int)POS_BOAT.y, WHITE);
    if (menuTimer >= 0.5f + 2 * MENU_STEP_INTERVAL) DrawTexture(treetitle, (int)POS_TREE.x, (int)POS_TREE.y, WHITE);
    if (menuTimer >= 0.5f + 3 * MENU_STEP_INTERVAL) DrawTexture(flytitle, (int)POS_FLY.x, (int)POS_FLY.y, WHITE);
    if (menuTimer >= 0.5f + 4 * MENU_STEP_INTERVAL) DrawTexture(enemytitle, (int)POS_ENEMY.x, (int)POS_ENEMY.y, WHITE);

    // Color title (flicker)
    Texture2D currentTitle = redtitle;
    if (menuTimer >= 0.5f + 6 * MENU_STEP_INTERVAL) {
        if (colorVariant == 1) currentTitle = greentitle;
        else if (colorVariant == 2) currentTitle = yellowtitle;
    }
    DrawTexture(currentTitle, (int)POS_TITLE.x, (int)POS_TITLE.y, WHITE);
}

