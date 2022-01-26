#include "pch.h"
#include <iostream>
#include <math.h>
#include "Player.h"
#include <stdlib.h>
#include <time.h>

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////////////////Possible Agent Actions/////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

void a_Attack(Player& curr, Player& enemy, bool& running)
{
	enemy.SetHP(enemy.GetHP() - 2);
	std::cout << "> Player " << curr.Name << " chose attack!, " << "Player " << enemy.Name << " now has " << enemy.GetHP() << " HP!\n\n";
	if (enemy.GetHP() <= 0) { running = false; std::cout << "Player " << curr.Name << " won!\n\n"; }
}

void a_HeavyAttack(Player& curr, Player& enemy, bool& running)
{
	curr.SetHeavyReady(false);
	enemy.SetHP(enemy.GetHP() - 4);
	std::cout << "> Player " << curr.Name << " chose heavy attack!, " << "Player " << enemy.Name << " now has " << enemy.GetHP() << " HP!\n\n";
	if (enemy.GetHP() <= 0) { running = false; std::cout << "Player " << curr.Name << " won!\n\n"; }
}

void a_Heal(Player& curr)
{
	curr.SetMK(curr.GetMK() - 1);
	curr.SetHP(curr.GetHP() + 3);
	if (curr.GetHP() > curr.MaxHP) { curr.SetHP(curr.MaxHP); }
	std::cout << "> Player " << curr.Name << " chose heal!, " << "and now has " << curr.GetHP() << " HP and has " << curr.GetMK() << " medkits left!" << "\n\n";
}

void a_Bandage(Player& curr)
{
	curr.SetHP(curr.GetHP() + 1);
	if (curr.GetHP() > curr.MaxHP) { curr.SetHP(curr.MaxHP); }
	std::cout << "> Player " << curr.Name << " chose bandage " << "and now has " << curr.GetHP() << "HP!" << "\n\n";
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//All considerations (weights) used for scoring the actions//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

float c_healthWeight(Player& player)
{
	//Normalizing HP weight to 1
	return float(player.GetHP()) / player.MaxHP;
}

int c_medKitWeight(Player& player)
{
	//Check if the player has medkits
	if (player.GetMK() > 0) { return 1; }
	else { return 0; }
}

bool c_heavyWeight(Player& player)
{
	//Check if the player can do a heavy attack
	if (player.GetHeavyReady()) { return 1; }
	else { return 0; }
}

float c_medKitCountWeight(Player& player)
{
	//Normalizing medkit count weight to 1
	return float(player.GetMK()) / player.MaxMedKit;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////Curves to map weights depending on situation////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

float cu_desc_lin(float weight)
{
	//descending linear curve with bound min bound 0.2f
	if ((1 - weight) < 0.2f) { return 0.2f; }
	else { return 1 - weight; }
}

float cu_inverse(float weight)
{ //   1/x (inverse/asymptote function)
	// we only want to heal when very low hp so we set min (20% for exmaple)
	if (weight <= 0.2f)
	{ 
	return (0.2f / weight) - 0.1f;
	}
	else { return 0.00f; }
}

float cu_exp(float weight)
{
	// expontential function (rapid increase)
	// we only want to heal when very low hp so we set min (20% for exmaple)
	if (weight <= 0.4f)
	{
		return  float(1 - pow(weight, 6));
	}
	else { return 0.00f; }
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//Reasoner to determine the which action to use for a player//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Reasoner(Player& curr, Player& enemy, bool& running, bool randomselection)
{
	if (randomselection)
	{
		if (running)
		{
			//Calculate final scores Weights
			float heavy_score = c_heavyWeight(curr);
			float attack_score = cu_desc_lin(c_healthWeight(enemy));
			float heal_score = c_medKitWeight(curr) * cu_inverse(c_healthWeight(curr));
			float bandage_score = cu_exp(c_healthWeight(curr));
			//Decide action based on a weighted seeded selection using the scores
			float scoresum = heavy_score + attack_score + heal_score + bandage_score;
			float rValue = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
			heavy_score /= scoresum;
			attack_score /= scoresum;
			heal_score /= scoresum;
			bandage_score /= scoresum;
			if (rValue >= 0 && heavy_score > rValue) { a_HeavyAttack(curr, enemy, running); }
			else if (rValue >= attack_score && attack_score + heavy_score > rValue) { a_Attack(curr, enemy, running); }
			else if (rValue >= attack_score + heavy_score && attack_score + heavy_score + heal_score > rValue) { a_Heal(curr); }
			else if (rValue >= attack_score + heavy_score + heal_score && attack_score + heavy_score + heal_score + bandage_score > rValue) { a_Bandage(curr); }
		}
	}
	else
	{
		if (running)
		{
			//Calculate final scores Weights
			float heavy_score = c_heavyWeight(curr);
			float attack_score = cu_desc_lin(c_healthWeight(enemy));
			float heal_score = c_medKitWeight(curr) * cu_inverse(c_healthWeight(curr));
			float bandage_score = cu_exp(c_healthWeight(curr));
			float highestScore{};
			//Decide action based on highest score
			if (heavy_score > highestScore) { highestScore = heavy_score; }
			if (attack_score > highestScore) { highestScore = attack_score; }
			if (heal_score > highestScore) { highestScore = heal_score; }
			if (bandage_score > highestScore) { highestScore = bandage_score; }
			if (highestScore == heavy_score && heavy_score > 0) { a_HeavyAttack(curr, enemy, running); }
			else if (highestScore == attack_score && attack_score > 0) { a_Attack(curr, enemy, running); }
			else if (highestScore == heal_score && heal_score > 0) { a_Heal(curr); }
			else if (highestScore == bandage_score && bandage_score > 0) { a_Bandage(curr); }
		}
	}

}

//Adding extra randomness for better testing
void RandomVariabels(Player& A, Player& B)
{
	// 1/4 chance to gain heavy attack
	if (rand() % 4 == 0) { A.SetHeavyReady(true); }
	if (rand() % 4 == 0) { B.SetHeavyReady(true); }
	// 1/3 chance to get extra medkit
	if (rand() % 3 == 0 && A.MaxMedKit < A.GetMK()) { A.SetMK(A.GetMK() + 1); }
	if (rand() % 3 == 0 && B.MaxMedKit < B.GetMK()) { B.SetMK(B.GetMK() + 1); }
}

int main()
{
	srand(time(NULL));
	//Contains all data of Players (used in considerations for scoring)
	Player PlayerA('A');
	Player PlayerB('B');
	bool running{ true };
	bool randomselection{};
	char input{};
	std::cout << "Input 'y' to use seeded random selection, input anything else for picking action based on highest score\n\n";
	std::cin >> input;
	if (input == 'y') { randomselection = true; }
	if (randomselection) { std::cout << "Using random selection!\n\n"; }
	else { std::cout << "Using highest score selection!\n\n"; }
	//Program stops running when a player dies
	while (running)
	{
		// 50/50 for whos turn goes first
		if (rand() % 2 == 0)
		{
			//Player A logic
			Reasoner(PlayerA, PlayerB, running, randomselection);
			//Player B logic
			Reasoner(PlayerB, PlayerA, running, randomselection);
		}
		else
		{
			//Player B logic
			Reasoner(PlayerB, PlayerA, running, randomselection);
			//Player A logic
			Reasoner(PlayerA, PlayerB, running, randomselection);
		}
		RandomVariabels(PlayerA, PlayerB);
	}
	std::cout << "> PlayerA medkits left = " << PlayerA.GetMK() << "\n\n";
	std::cout << "> PlayerB medkits left = " << PlayerB.GetMK() << "\n\n";
	system("pause");
	return 0;
}