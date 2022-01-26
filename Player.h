#include "pch.h"
#pragma once
#include <iostream>
#include <math.h>
#include <string>

class Player
{
public:
	Player(char name) { Name = name; }
	char Name;
	const int MaxHP = 60;
	const int MaxMedKit = 5;
	int GetHP() { return HP; }
	void SetHP(int value) { HP = value; }
	int GetMK() { return MedKit; }
	void SetMK(int value) { MedKit = value; }
	bool GetHeavyReady(){ return HeavyReady; }
	void SetHeavyReady(bool value) { HeavyReady = value; }
private:
	int HP = 60;
	int MedKit = 5;
	bool HeavyReady = false;
};
