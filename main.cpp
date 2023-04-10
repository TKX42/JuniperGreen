/*
 * 2023 Tristan Kaupa
 */

#include <iostream>
#include <list>
#include <algorithm>
#include <utility>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int START = 1;
const int END = 50;
const int DEPTH = 7;
const int MAX_VAL = INT_MAX;    // INT_MIN cannot be negated!!
const bool COMPUTER_STARTS = true;
const bool DEBUG = true;

int calc_count = 0;
int next_move = -1;

void print(const auto& data) {
    for (auto x: data) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

bool valid(int last_number, int x) {
    return (x % last_number == 0 || last_number % x == 0) && x > 0;
}

std::list<int> get_possible_moves(const std::list<int>& numbers, int last_number) {
    std::list<int> possible_moves;
    for (int x: numbers) {
        if (valid(last_number, x)) {
            possible_moves.push_back(x);
        }
    }
    return possible_moves;
}

int score(const auto& possible_moves) {
    if (possible_moves.empty()) return -MAX_VAL;
    return possible_moves.size();
}

int minimax(int player, int depth, const std::list<int>& numbers, int last_move, int alpha, int beta) {
    calc_count++;
    auto possible_moves = get_possible_moves(numbers, last_move);
    if (depth == 0 || possible_moves.empty()) {
        return score(possible_moves);
    }

    int max_val = alpha;
    int i = 0;
    for (auto possible_move: possible_moves) {
        auto start = std::chrono::steady_clock::now();
        if(DEBUG && depth == DEPTH) {
            std::cout << "calculating move " << i << "/" << possible_moves.size();
        }
        auto new_field = numbers;   // copy
        new_field.remove(possible_move);
        auto val = -minimax(-player, depth - 1, new_field, possible_move, -beta, -max_val);
        if (val > max_val) {
            max_val = val;
            if (depth == DEPTH) {
                next_move = possible_move;
            }
            if(max_val >= beta) {
                break;
            }
        }

        if(DEBUG && depth == DEPTH) {
            auto end = std::chrono::steady_clock::now();
            std::cout << " - took: " <<  (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) /1000000.0  << " (sec)" << "\r";
        }
        i++;
    }

    return max_val;
}

int next(std::list<int> numbers, int last_number) {
    next_move = -1;
    int score = minimax(+1, DEPTH, std::move(numbers), last_number, -MAX_VAL, MAX_VAL);
    std::cout << "\33[2K\r"; std::cout << "finished " << calc_count << " calculations" << std::endl;
    calc_count = 0;
    std::cout << "next move is " << next_move << " with score " << score << std::endl;
    return next_move;
}

int main() {
    std::cout << "-------------- Juniper Green --------------" << std::endl;
    std::cout << "CONFIG:\n";
    std::cout << "\tdepth: " << DEPTH << std::endl;
    std::cout << "\trange: " << START << "-" << END << std::endl;

    std::list<int> numbers;
    bool computer_turn = COMPUTER_STARTS;
    int last_number = 2;        // initialized with 2 so every even number is valid as a first number

    for (int i = START; i <= END; i++) {
        numbers.push_back(i);
    }

    while (true) {
        std::cout << "field: ";print(numbers);

        if (computer_turn) {
            int x = next(numbers, last_number);
            std::cout << "i choose: " << x << "\n\n";
            numbers.remove(x);
            last_number = x;
        } else {
            int x;
            std::cout << "> ";
            std::cin >> x;
            std::cout << std::endl;
            // invalid number: not in available numbers, not according to last number
            if (std::find(numbers.begin(), numbers.end(), x) == numbers.end() || !valid(last_number, x)) {
                std::cout << "invalid number!" << std::endl;
                continue;
            }

            numbers.remove(x);
            last_number = x;
        }

        auto possible_moves = get_possible_moves(numbers, last_number);
        std::cout << "possible moves: "; print(possible_moves);

        computer_turn = !computer_turn;
        if (computer_turn && possible_moves.empty()) {
            std::cout << "I lost :(" << std::endl;
            break;
        }
        if (!computer_turn && possible_moves.empty()) {
            std::cout << "HAHAHA I WON!!" << std::endl;
            break;
        }
    }

    std::cin.get();
    return 0;
}