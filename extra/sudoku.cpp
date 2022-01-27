
#include <array>
#include <vector>


class Sudoku
{
public:
    using Board = std::array<std::array<int, 9>, 9>;

    Sudoku(const Board& board): board_(board) { fillNumber(0); }

    const std::vector<Board>& results() const { return results_; }

private:

    void fillNumber(int pos)
    {
        if (pos == 81)
        {
            results_.push_back(board_);
        }
        else
        {
            int i = pos / 9, j = pos % 9;
            if (board_[i][j] != 0)
            {
                fillNumber(pos + 1);
            }
            else
            {
                for (int num = 1; num < 10; ++num)
                {
                    if (checkAll(num, i, j))
                    {
                        board_[i][j] = num;
                        fillNumber(pos + 1);
                    }
                }
                board_[i][j] = 0;
            }
        }
    }

    bool checkAll(int num, int i, int j) const
    {
        return checkRow(num, i, j) &&
               checkCol(num, i, j) &&
               checkSurround(num, i, j);
    }

    bool checkRow(int num, int i, int j) const
    {
        for (int k = 0; k < 9; ++k)
            if (board_[i][k] == num && j != k)
                return false;
        return true;
    }

    bool checkCol(int num, int i, int j) const
    {
        for (int k = 0; k < 9; ++k)
            if (board_[k][j] == num && i != k)
                return false;
        return true;
    }

    bool checkSurround(int num, int i, int j) const
    {
        for (int k1 = (i / 3) * 3; k1 < (i / 3) * 3 + 3; ++k1)
            for (int k2 = (j / 3) * 3; k2 < (j / 3) * 3 + 3; ++k2)
                if (board_[k1][k2] == num && (i != k1 || j != k2))
                    return false;
        return true;
    }

    Board board_;
    std::vector<Board> results_;
};

inline std::vector<Sudoku::Board>
solveSudoku(const Sudoku::Board& board)
{
    return Sudoku(board).results();
}


// 测试
#include <iostream>

int main()
{
    auto res = solveSudoku(
    {
        4, 7, 0, 3, 0, 2, 0, 0, 0,
        0, 0, 6, 0, 5, 0, 2, 0, 0,
        0, 0, 0, 1, 0, 4, 0, 6, 9,
        0, 0, 0, 0, 0, 8, 5, 0, 0,
        0, 3, 1, 0, 0, 0, 0, 0, 0,
        8, 0, 0, 0, 0, 1, 0, 0, 4,
        6, 0, 0, 2, 0, 0, 0, 0, 3,
        0, 0, 0, 0, 0, 0, 8, 2, 0,
        0, 0, 7, 9, 0, 0, 0, 0, 0
    });

    using namespace std;
    cout << "Solution count: " << res.size() << endl;

    for (int k = 0; k < (int)res.size(); ++k)
    {
        for (int i = 0; i < 9; ++i)
        {
            for (int j = 0; j < 9; ++j)
            {
                cout << res[k][i][j] << " ";
                if ((j + 1) % 3 == 0) cout << "  ";
            }
            cout << endl;
            if ((i + 1) % 3 == 0) cout << endl;
        }
    }
    return 0;
}
