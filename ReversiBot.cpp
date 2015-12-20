//To strive, to seek, to find, and not to yield.

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#pragma warning(disable: 4996)

using namespace std;

clock_t start;

/*
//for testing
const int othello[8][8] = { { 45, 32, 19, 18, 31, 24, 44, 43 },
							{ 46, 36, 9, 11, 16, 15, 42, 56 },
							{ 17, 8, 3, 4, 10, 22, 38, 51 },
							{ 20, 13, 5, 0, 0, 6, 23, 40 },
							{ 21, 14, 7, 0, 0, 1, 39, 41 },
							{ 34, 30, 12, 2, 28, 29, 53, 52 },
							{ 35, 47, 33, 26, 25, 37, 59, 55 },
							{ 50, 49, 48, 27, 54, 60, 58, 57 } };
*/

class Position {
public:
	int x, y;
	Position() {}
	Position(int x1, int y1) {
		x = x1; y = y1;
	}

	bool validPosition() {
		return (x >= 0 && x < 8 && y >= 0 && y < 8);
	}

	Position operator+(Position pos) {
		return Position(x + pos.x, y + pos.y);
	}
};

class Table {
public:
	int field[8][8];
	int numberStates[3];

	void fillPosition(Position pos, int clr) {
		numberStates[field[pos.x][pos.y]]--;
		numberStates[clr]++;
		field[pos.x][pos.y] = clr;
	}

	Table() {
		numberStates[1] = 0;
		numberStates[2] = 0;
		numberStates[0] = 64;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				field[i][j] = 0;
		fillPosition(Position(3, 3), 1);
		fillPosition(Position(3, 4), 2);
		fillPosition(Position(4, 3), 2);
		fillPosition(Position(4, 4), 1);
	}

	int color(Position pos) {
		return field[pos.x][pos.y];
	}
};

class Result {
public:
	Table field;
	int flip, win;
	Position pos;
	int myTurns, opponentTurns;

	Result() {
		flip = win = 0;
	}

	Result(Table field) {
		field = field;
		flip = win = 0;
	}
};

class ReversiClient {
private:
	vector <Position> directions;
public:

	ReversiClient() {
		directions.push_back(Position(-1, -1));
		directions.push_back(Position(-1, 0));
		directions.push_back(Position(-1, 1));
		directions.push_back(Position(0, -1));
		directions.push_back(Position(0, 1));
		directions.push_back(Position(1, -1));
		directions.push_back(Position(1, 0));
		directions.push_back(Position(1, 1));
	}

	inline int filled(Table &field) {
		return field.numberStates[1] + field.numberStates[2];
	}

	inline Result turn(Table &field, Position currentPosition, int color, bool changeField) {
		if (field.color(currentPosition) != 0) {
			if (changeField)
				return Result(field);
			else
				return Result();
		}
		Result result;
		if (changeField)
			result.field = field;
		result.pos = currentPosition;
		for (int i = 0; i < directions.size(); ++i) {
			Position direction = directions[i];
			Position position = currentPosition + direction;
			while (position.validPosition() && field.color(position) == 3 - color)
				position = position + direction;
			if (position.validPosition() && field.color(position) == color) {
				position = currentPosition + direction;
				while (position.validPosition() && field.color(position) == 3 - color) {
					result.flip++;
					if (changeField)
						result.field.fillPosition(position, color);
					position = position + direction;
				}
			}
		}
		if (changeField) 
			result.field.fillPosition(currentPosition, color);
		return result;
	}

	inline string moveChip(Table &field, Position pos, int color) {
		Result result = turn(field, pos, color, true);
		field = result.field;
		string x(1, 'a' + pos.x);
		string y(1, '1' + pos.y);
		return  "move " + x + ' ' + y;
	}

	inline void ourTurn(Table &field, int color) {
		start = clock();
		int we_need_to_go_deeper;
		int sumUsed = field.numberStates[1] + field.numberStates[2];
		if (sumUsed < 45)
			we_need_to_go_deeper = 4;
		else
			if (sumUsed < 55)
				we_need_to_go_deeper = 5;
			else
				if (sumUsed < 58)
					we_need_to_go_deeper = 6;
				else
					we_need_to_go_deeper = 7;
		Result theBestResult = iAmTheBest(field, color, we_need_to_go_deeper);
		if (theBestResult.flip > 0)
			cout << moveChip(field, theBestResult.pos, color) << "\n";
	}

	inline int countTurns(Table& field, int color) {
		int count = 0;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				if (turn(field, Position(i, j), color, false).flip > 0) ++count;
		return count;
	}

	int countWin(Table& field, int color) {
		int myTurn;
		int opponentTurn;
		if (filled(field) < 50) {
			myTurn = countTurns(field, color);
			opponentTurn = countTurns(field, 3 - color);
		}
		int wining = 0;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				if (field.field[i][j]) {
					int cost = 0;
					if (filled(field) < 50) {
						if ((i % 7 == 0) && (j % 7 == 0))
							cost = 2500;
						else if (i * j % 7 == 0)
							cost = 45;
						else
							cost = 11;
					}
				else {
						cost = 10;
					}
					if (field.field[i][j] == color)
						wining += cost;
					else if (field.field[i][j] == 3 - color)
						wining -= cost;
				}
		if (filled(field) < 50)
			wining += 16 * (myTurn - opponentTurn);
		if (field.numberStates[color] == 0)
			wining = -1e10;
		if (field.numberStates[3 - color] == 0)
			wining = 1e10;
		return wining;
	}

	Result iAmTheBest(Table &field, int color, int deep) {
		Result maxResult(field);
		maxResult.win = -1e10;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j) {
				Result result = turn(field, Position(i, j), color, true);
				if (result.flip > 0) {
					if (deep > 0 && (double)(clock() - start) / CLOCKS_PER_SEC < 2.99) {
						Result theBestResult = iAmTheBest(result.field, 3 - color, deep - 1);
						result.win = -theBestResult.win;
					}
					else 
						result.win = countWin(result.field, color);
					if (result.win > maxResult.win)
						maxResult = result;
				}
			}
		if (!maxResult.flip)
			maxResult.win = countWin(field, color);
		return maxResult;
	}

};

int main() {
	//	freopen("output.txt", "w", stdout);
	ReversiClient algo = ReversiClient();
	string tmp, color, opponentMove, x, y, result;
	cin >> tmp >> color;
	int myColor;
	if (color == "white")
		myColor = 1;
	else
		myColor = 2;
	Table field = Table();
	while (true) {
		cin >> result;
		if (result == "turn")
			algo.ourTurn(field, myColor);
		else
			if (result == "move") {
				cin >> x >> y;
				algo.moveChip(field, Position(x[0] - 'a', y[0] - '1'), 3 - myColor);
			}
			else
				return 0;
	}
	return 0;
}