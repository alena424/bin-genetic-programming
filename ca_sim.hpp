#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cassert>
#include <cstring>

/***
 * Objekt pro simulaci celuarniho automatu pomoci pravidel Game of Life
 */
class CAsim
{
public:
	CAsim(int length, int rules_length, int *init_config, int neighborhood, int steps_max) : length_(length), rules_length_(rules_length), neighborhood_(neighborhood), steps_max_(steps_max)
	{
		states_mem_ = new int[steps_max_ * length_];
		memcpy(states_mem_, init_config, sizeof(int) * length_);
		rules_ = new int[rules_length];
	}

	~CAsim()
	{
		delete[] states_mem_;
	}

	/** 
	 * nastaveni pocatecniho stavu simulace
	 **/
	void set_init(int *rules)
	{
		memcpy(rules_, rules, sizeof(int) * rules_length_);
	}

	/***
     * Vrati 1D pole o delce rows * cols
     **/
	int *get_states(int step)
	{

		assert(step >= 1 && step < steps_max_); // nebudeme cist nulty stav

		return states_mem_ + (step * length_);
	}

	/***
	 * Spusteni simulace na urcity pocet kroku
	 */
	void run_sim(int steps)
	{
		assert(steps <= steps_max_);

		for (int i = 1; i < steps; i++)
		{
			int *src = states_mem_ + ((i - 1) * length_);
			int *dest = states_mem_ + (i * length_);

			for (int r = 0; r < length_; r++)
			{
				applyRules(src, dest, r);
			}
		}
	}

private:
	int length_, rules_length_;

	int *rules_;
	int neighborhood_;
	int *states_mem_;
	int states_count_;
	int steps_max_;

	// RULES
	// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
	// Any live cell with two or three live neighbours lives on to the next generation.
	// Any live cell with more than three live neighbours dies, as if by overcrowding.
	// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	void applyRules(int *grid, int *nextGrid, int elemIndex)
	{

		int ruleIndex = findRuleIndex(grid, elemIndex);
		// std::cout << "elemIndex is: " << elemIndex << ", ruleIndex is: " << ruleIndex << ", result: " << rules_[ruleIndex] << std::endl;
		nextGrid[elemIndex] = rules_[ruleIndex];
		// if (grid[elemIndex * cols_ + col] == 1)
		// {
		// 	if (numNeighbors < 2)
		// 	{
		// 		nextGrid[elemIndex * cols_ + col] = 0;
		// 	}
		// 	else if (numNeighbors == 2 || numNeighbors == 3)
		// 	{
		// 		nextGrid[elemIndex * cols_ + col] = 1;
		// 	}
		// 	else if (numNeighbors > 3)
		// 	{
		// 		nextGrid[elemIndex * cols_ + col] = 0;
		// 	}
		// }
		// else if (grid[elemIndex * cols_ + col] == 0)
		// {
		// 	if (numNeighbors == 3)
		// 	{
		// 		nextGrid[elemIndex * cols_ + col] = 1;
		// 	}
		// }
	}

	// int getAddr(int r, int c)
	// {
	// 	return r * cols_ + c;
	// }

	/***
	 * modulo vracejici positivni vysledek (-1 % n = n - 1)
	 */
	// int posmod(int a, int b)
	// {
	// 	int ret = a % b;
	// 	if (ret < 0)
	// 		ret += b;
	// 	return ret;
	// }

	int gridValue(int *grid, int index)
	{
		if (index < 0 || index >= length_)
		{
			return 0;
		}
		return grid[index];
	}

	int findRuleIndex(int *grid, int index)
	{
		//int ruleIndex[2 * _neighborhood + 1]; // Left State Right = (2*L/R + 1 for State)
		int ruleIndex = 0;
		int j = 1; // for indexing ruleIndex
		for (int i = index - neighborhood_; i <= index + neighborhood_; i++)
		{
			//ruleIndex[j] = grid[i];
			ruleIndex += gridValue(grid, i) * j;
			j = j + j; // 1, 2, 4, 8, 16 ...
		}
		return ruleIndex;
	}

	/**
	 * zjisteni poctu zivych sousedu
	 */
	// int countNeighbors(int *grid, int row, int col)
	// {
	// 	int count = 0;
	// 	if (grid[getAddr(posmod(row - 1, length_), col)] == 1)
	// 		count++;
	// 	if (grid[getAddr(posmod(row - 1, length_), posmod(col - 1, cols_))] == 1)
	// 		count++;
	// 	if (grid[getAddr(posmod(row - 1, length_), posmod(col + 1, cols_))] == 1)
	// 		count++;
	// 	if (grid[getAddr(row, posmod(col - 1, cols_))] == 1)
	// 		count++;
	// 	if (grid[getAddr(row, posmod(col + 1, cols_))] == 1)
	// 		count++;
	// 	if (grid[getAddr(posmod(row + 1, length_), col)] == 1)
	// 		count++;
	// 	if (grid[getAddr(posmod(row + 1, length_), posmod(col - 1, cols_))] == 1)
	// 		count++;
	// 	if (grid[getAddr(posmod(row + 1, length_), posmod(col + 1, cols_))] == 1)
	// 		count++;
	// 	return count;
	// }
};