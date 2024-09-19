// 建立日期：2024.09.13
// 完成日期：2023.09.14

// Nonogram（數織）是一種紙筆邏輯遊戲，以邏輯推理繪出黑白點陣圖。
// 這一個程式可以解出 Nonogram
// 輸入： row 和 col 的條件

#include <iostream>
#include <vector>
using namespace std;

const double REDUCTION_RATIO = 0.75;

double C(int n, int k) {
    // 組合數 C(n, k) = n! / (k! * (n-k)!)
    // Base Case
    if (k == 0 || k == n) {
        return 1;
    }
    // C(n, k) = C(n, n-k) in order to reduce calculation
    if (k > n-k) {
        k = n-k;
    }
    // Calculation
    double result = 1;
    for (int i = 1; i <= k; i++) {
        result *= (n + 1 - i);
        result /= i;
    }
    return result;
}

double H(int n, int m) {
    // a_1+a_2+...+a_n = m
    // 若 a_1, a_2, ..., a_n 是非負整數，
    // 則總共有 H(n, m) = C(n+m-1, m) 種解法
    return C(n+m-1, m);
}

void print2dvector(vector<vector<char>> v) {
    for (auto x : v) {
        for (auto y : x) {
            cout << y << " ";
        }
        cout << endl;
    }
}

vector<vector<int>> partitions(int n, int k) {
    // a1 + a2 + ... + ak = n
    // 輸出所有 [a1, a2, ..., ak] 的組合
    vector<vector<int>> result;  // 輸出是一個 vector<vector<int>>
    // Base Case
    if (k == 1) {
        result.push_back(vector<int>({n}));
        return result;
    }
    // Recursive Case
    for (int i = 0; i <= n; i++) {
        vector<vector<int>> sub_result = partitions(n - i, k - 1);
        for (auto x : sub_result) {
            x.push_back(i);
            result.push_back(x);
        }
    }
    return result;
}

class Pixel {
    public:
    int x;
    int y;
    char val = 'u';
    Pixel(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

int row, col;
vector<vector<Pixel*>> board;

void board_setup() {
    for (int i = 0; i < row; i++) {
        board.push_back(vector<Pixel*>());
        for (int j = 0; j < col; j++) {
            board[i].push_back(new Pixel(i, j));
        }
    }
    cout << "Done board_setup()." << endl;
    cout << "This is a " << row << " x " << col << " board" << endl;
    return void();
}

void print_board() {
    system("chcp 65001");
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j]->val == '0') {
                cout << "⬜";
                // cout << "X ";
            } else if (board[i][j]->val == '1') {
                cout << "⬛";
                // cout << "88";
            } else {
                cout << "--";
            }
        }
        cout << endl;
    }
}

void update_pixel(int x, int y, char c);

class Line {
    public:
    // 以下為 solve 過程中不會更改的變數
    char line_type;
    int index;
    int length;
    vector<int> condition;
    vector<Pixel*> pixels;
    int eventual_black_num = 0;
    // 以下為 solve 過程中會做更改的變數
    bool updated = false;
    bool no_clue = false;
    vector<vector<char>> possible_result;
    int black_num = 0, white_num = 0;

    Line(char line_type, int index, int length, vector<int> condition) {
        this->line_type = line_type;
        this->index = index;
        this->length = length;
        this->condition = condition;
        if (line_type == 'r') {
            pixels = board[index];
        } else if (line_type == 'c') {
            for (int i = 0; i < board.size(); i++) {
                pixels.push_back(board[i][index]);
            }
        }
        for (const auto & x : condition) {
            eventual_black_num += x;
        }
    }

    int blank_num() {
        return length - black_num - white_num;
    }

    bool have_constructed_possible_result() {
        return !possible_result.empty();
    }

    void unit_func(vector<char> v, int condition_index, int pixel_index, int free_space) {
        // Base Case
        if (condition_index > condition.size()) {
            possible_result.push_back(v);
            return void();
        }

        // Recursive Case
        bool is_last = (condition_index == condition.size());
        int black_count = (is_last ? 0 : condition[condition_index]);
        for (int i = (is_last ? free_space : 0); i <= free_space; i++) {
            int white_count = i;
            if (0 < condition_index && condition_index < condition.size()) {
                white_count += 1;
            }
            // 以上完成黑白格子數的計算

            // 檢查此組合是否合法
            bool is_valid = true;
            for (int j = pixel_index; j < pixel_index + white_count; j++) {
                if (pixels[j]->val == '1') {
                    is_valid = false;  // 落入不可能的分支
                }
            }
            for (int j = pixel_index + white_count; j < pixel_index + white_count + black_count; j++) {
                if (pixels[j]->val == '0') {
                    is_valid = false;  // 落入不可能的分支
                }
            }
            if (!is_valid) {
                continue;
            }

            for (int j = pixel_index; j < pixel_index + white_count; j++) {
                v[j] = '0';
            }
            for (int j = pixel_index + white_count; j < pixel_index + white_count + black_count; j++) {
                v[j] = '1';
            }
            unit_func(v, condition_index + 1, pixel_index + white_count + black_count, free_space - i);
        }
        return void();
    }

    void update_possible_result() {
        // cout << "Called update_possible_result()." << endl;
        if (!have_constructed_possible_result()) {
            int free_space = length - eventual_black_num - (condition.size()-1);
            unit_func(vector<char>(length, 'u'), 0, 0, free_space);
        } else if (!updated) {
            for (int pixel_index = 0; pixel_index < length; pixel_index++) {
                char c = pixels[pixel_index]->val;
                int i = 0;
                while (i < possible_result.size()) {
                    if (possible_result[i][pixel_index] - c == 1 || possible_result[i][pixel_index] - c == -1) {
                        swap(possible_result[i], possible_result.back());
                        possible_result.pop_back();
                    } else {
                        i += 1;
                    }
                }
            }
        }
        updated = true;
        return void();
    }

    void update_line() {
        // Step 1 : 
        // 先執行 update_possible_result
        update_possible_result();
        // Step 2 :
        // 根據目前已經有的 possible_result，找到這條 Line 的所有可能的結果；
        // 若有完全相同的部分，則執行 update_pixel
        for (int pixel_index = 0; pixel_index < length; pixel_index++) {
            if (pixels[pixel_index]->val == 'u') {
                char c = possible_result[0][pixel_index];
                bool all_same = true;
                for (int i = 0; i < possible_result.size(); i++) {
                    if (possible_result[i][pixel_index] != c) {
                        all_same = false;
                        break;
                    }
                }
                if (all_same) {
                    if (line_type == 'r') {
                        update_pixel(index, pixel_index, c);
                    } else if (line_type == 'c') {
                        update_pixel(pixel_index, index, c);
                    }
                }
            }
        }
        // Step 3 :
        // 標註此 Line 已無線索了
        no_clue = true;

        return void();
    }

    double estimated_possible_result_num() {  // 可能的結果數量（預估值）
        if (have_constructed_possible_result()) {
            return possible_result.size();
        }
        double ans = H(condition.size()+1, length-eventual_black_num-(condition.size()-1));  // 這條 Line 全空時，可能的結果數量
        double line_reduction_ratio = 1;
        for (int i = 0; i < black_num + white_num; i++) {
            line_reduction_ratio *= REDUCTION_RATIO;
        }
        ans = 1 + (ans - 1) * line_reduction_ratio;
        return ans;
    }
};

vector<vector<int>> rows_condition, cols_condition;

vector<Line*> rows, cols;
void rows_and_cols_setup() {
    for (int i = 0; i < row; i++) {
        rows.push_back(new Line('r', i, col, rows_condition[i]));
    }
    for (int i = 0; i < col; i++) {
        cols.push_back(new Line('c', i, row, cols_condition[i]));
    }
}

Line* find_most_certain_line() {
    Line* ans = rows[0];
    for (const auto & x : rows) {
        if (
            ( x->blank_num() != 0 && !x->no_clue && (x->estimated_possible_result_num() < ans->estimated_possible_result_num()) )
             || ans->blank_num() == 0 || ans->no_clue
        ) {
            ans = x;
        }
    }
    for (const auto & x : cols) {
        if (
            ( x->blank_num() != 0 && !x->no_clue && (x->estimated_possible_result_num() < ans->estimated_possible_result_num()) )
             || ans->blank_num() == 0 || ans->no_clue
        ) {
            ans = x;
        }
    }
    // cout << "most certain line: " << ans->line_type << " " << ans->index << endl;
    // cout << "estimated_possible_result_num: " << ans->estimated_possible_result_num() << endl;
    return ans;
}

void update_pixel(int x, int y, char c) {
    board[x][y]->val = c;
    rows[x]->updated = false;
    cols[y]->updated = false;
    rows[x]->no_clue = false;
    cols[y]->no_clue = false;
    if (c == '0') {
        rows[x]->white_num += 1;
        cols[y]->white_num += 1;
    } else if (c == '1') {
        rows[x]->black_num += 1;
        cols[y]->black_num += 1;
    }
    return void();
}

bool is_solved() {
    for (const auto & x : rows) {
        if (x->blank_num() != 0) {
            return false;
        }
    }
    for (const auto & x : cols) {
        if (x->blank_num() != 0) {
            return false;
        }
    }
    return true;
}

const bool QUICK_TYPE = true;
const int SEPARATOR = 44;
vector<vector<int>> quick_type_analysis(vector<int> v) {
    vector<vector<int>> ans;
    vector<int> tmp;
    for (const auto & x : v) {
        if (x == SEPARATOR) {
            ans.push_back(tmp);
            tmp.clear();
        } else {
            tmp.push_back(x);
        }
    }
    return ans;
}

vector<int> rows_condition_input = vector<int>({
    11,44,4,6,10,44,3,11,3,3,44,3,15,2,4,2,44,20,2,3,6,44,
        
        15,2,2,1,2,5,44,15,1,1,1,1,1,2,4,44,14,1,1,1,2,1,2,4,44,14,1,1,2,1,2,1,3,44,14,3,1,1,3,2,1,2,3,44,

        14,1,1,1,2,1,2,1,2,44,14,4,3,4,2,1,2,44,7,4,1,2,1,4,3,2,1,2,44,7,1,2,1,2,6,1,2,44,6,2,1,4,4,2,44,

        7,1,3,4,1,2,3,44,2,6,2,1,2,2,1,3,44,1,2,2,1,1,1,4,44,2,2,2,2,1,2,2,5,44,4,2,3,1,1,2,2,2,44,

        5,2,3,1,1,3,4,1,44,6,2,5,3,1,2,4,44,7,2,2,2,1,2,2,1,44,7,4,2,2,3,2,44,6,2,2,2,3,1,44,

        4,1,1,6,2,44,6,2,1,3,3,44,2,3,2,2,10,44,2,3,2,2,9,2,44,3,4,4,8,4,44,

        4,2,6,1,5,6,44,5,1,7,1,1,7,44,5,2,7,2,1,8,44,5,1,8,1,2,8,44,5,2,8,1,1,8,44,

        5,1,2,6,2,1,8,44,5,1,1,1,6,1,2,8,44,5,1,1,1,5,1,1,8,44,5,1,1,1,6,3,9,44,4,2,1,2,5,1,9,44,

        4,1,1,1,6,1,10,44,3,2,2,1,6,7,2,44,3,1,1,2,6,6,3,44,2,2,2,1,6,2,6,44,1,2,2,2,18,44
});
vector<int> cols_condition_input = vector<int>({
    6,16,44,8,8,16,44,13,6,16,1,44,13,6,2,11,2,44,14,7,8,3,44,

        16,6,3,44,2,14,6,4,44,2,16,5,44,2,9,2,2,4,44,1,9,2,1,2,3,1,44,

        1,11,1,2,2,4,2,44,1,12,2,2,2,3,3,44,1,12,2,2,2,8,44,1,11,1,2,2,44,12,2,4,44,

        7,1,2,44,6,1,1,3,44,5,1,1,1,44,4,1,2,1,6,44,5,2,1,4,11,44,

        10,1,1,2,8,4,44,2,2,5,2,9,3,44,7,3,2,10,1,44,2,3,5,12,44,2,9,3,13,44,

        2,4,1,2,3,12,44,1,2,1,2,2,5,10,44,2,3,2,1,1,1,1,8,7,44,1,3,1,1,3,5,4,5,44,1,1,3,1,1,4,4,3,44,

        1,1,2,1,2,4,3,1,44,1,1,3,2,2,1,4,3,3,44,1,2,3,2,2,2,4,4,5,44,1,1,2,1,1,1,8,5,1,44,1,2,3,1,2,4,7,1,44,

        1,2,7,2,3,9,1,44,2,2,2,4,3,10,1,44,1,3,2,5,10,2,44,2,8,2,11,2,44,2,2,11,3,44,

        2,2,15,44,4,2,16,44,6,4,7,2,44,16,6,44,17,5,44
});

void solve() {
    // Step 1 : Set up board, rows and cols
    if (QUICK_TYPE) {
        rows_condition = quick_type_analysis(rows_condition_input);
        cols_condition = quick_type_analysis(cols_condition_input);
    }
    row = rows_condition.size();
    col = cols_condition.size();
    board_setup();
    rows_and_cols_setup();

    // Step 2 : Solve
    while (!is_solved()) {
        // print_board();
        Line* line = find_most_certain_line();
        line->update_line();
    }
    return void();
}

int main() {
    solve();
    print_board();
    return 0;
}