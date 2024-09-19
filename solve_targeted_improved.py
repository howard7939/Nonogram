# 建立日期：2023.01.04
# 完成日期：2023.01.04

# Nonogram（數織）是一種紙筆邏輯遊戲，以邏輯推理繪出黑白點陣圖。
# 這一個程式可以解出 Nonogram
# 輸入： Nonogram 的長寬、 row 和 col 的條件

import itertools
import copy
from pprint import pprint

def partitions(n, k):
    for c in itertools.combinations(range(n+k-1), k-1):
        yield [b-a-1 for a, b in zip((-1,)+c, c+(n+k-1,))]

def my_partitions(n, k): 
    # a1 + a2 + ... + ak = n
    # 輸出所有 [a1, a2, ..., ak] 的組合
    result = []  # 輸出是一個 list
    for p in partitions(n, k):
        result.append(p)
    return result

def possible_results(total, condition):
    result = []  # 最終，函式的 output 結果

    condition_sum = 0
    for item in condition:
        condition_sum += item
    free_spaces_num = total - condition_sum - (len(condition)-1)  # 有幾個可支配的空格

    possible_spaces = my_partitions(free_spaces_num, len(condition)+1)

    # 中間的間隔至少要有1個空格 --> possible_spaces 中間的 element 都要 += 1
    for i in range(len(possible_spaces)):
        for j in range(len(possible_spaces[i])-2):
            possible_spaces[i][j+1] += 1

    for i in range(len(possible_spaces)):
        possible_result = []
        for j in range(len(condition)):
            for qqq in range(possible_spaces[i][j]):
                possible_result.append(False)
            for qqq in range(condition[j]):
                possible_result.append(True)
        for qqq in range(possible_spaces[i][-1]):
                possible_result.append(False)
        result.append(possible_result)
    
    return result

def is_solved(board):
    for row_index in range(len(board)):
        for col_index in range(len(board[row_index])):
            if board[row_index][col_index] == -1:
                return False
    return True

def print_board(board):
    for row_index in range(len(board)):
        for col_index in range(len(board[row_index])):
            if board[row_index][col_index] == 0:
                print('⬜', end='')
            elif board[row_index][col_index] == 1:
                print('⬛', end='')
            else:
                print('OO', end='')
        print('')
    return


def solve(board, rows_condition, cols_condition):

    ### 參數 ###
    count = 0
    threshold_max = 1000000
    threshold = 10000
    threshold_multiplier = 3
    print_progress = True

    # board 是一個 MxN 的 list
    M = len(rows_condition)
    N = len(cols_condition)
    # board 裡面的元素有3種：0(確定不是黑色)、1(確定是黑色)、-1(尚未填入)

    rows_possible_results = []  # 儲存每一個 row 可能的黑色組合
    for i, condition in enumerate(rows_condition):
        P = possible_results(N,condition)
        rows_possible_results.append(P)
        if print_progress:
            print('number of possible results of row', i, '=',len(P))
    print('前期準備1(rows_possible_results)完成')
    rows_possible_results_length = []
    for item in rows_possible_results:
        rows_possible_results_length.append(len(item))
    print('rows_possible_results_length =')
    print(rows_possible_results_length)

    cols_possible_results = []  # 儲存每一個 col 可能的黑色組合
    for i, condition in enumerate(cols_condition):
        P = possible_results(M,condition)
        cols_possible_results.append(P)
        if print_progress:
            print('number of possible results of col', i, '=',len(P))
    print('前期準備2(cols_possible_results)完成')
    cols_possible_results_length = []
    for item in cols_possible_results:
        cols_possible_results_length.append(len(item))
    print('cols_possible_results_length =')
    print(cols_possible_results_length)

    
    do_something_row_index = set([i for i in range(M)])  # 儲存此次迴圈改動過的 row index
    do_something_col_index = set([i for i in range(N)])

    # 取「有更新的 row 和 col」以及「可能情況數量小於等於 threshold」的交集
    # --- row ---
    pass_threshold_row_index = set()
    for row_index in range(M):
        if len(rows_possible_results[row_index]) <= threshold:
            pass_threshold_row_index.add(row_index)
    do_something_row_index = do_something_row_index & pass_threshold_row_index
    # --- col ---
    pass_threshold_col_index = set()
    for col_index in range(N):
        if len(cols_possible_results[col_index]) <= threshold:
            pass_threshold_col_index.add(col_index)
    do_something_col_index = do_something_col_index & pass_threshold_col_index

    while True:
        do_something = False  # 是否有改動 board 的任何一個地方
        new_do_something_row_index = set()
        new_do_something_col_index = set()

        ### --- Row Part --- ###

        for row_index in do_something_row_index:

            # 檢查是否有「確定是黑色」的格子
            check_and = copy.deepcopy(rows_possible_results[row_index][0])
            for i in range(len(rows_possible_results[row_index])):
                for col_index in range(len(check_and)):
                    check_and[col_index] = check_and[col_index] and rows_possible_results[row_index][i][col_index]
            for col_index in range(len(check_and)):
                if check_and[col_index] == True and board[row_index][col_index] == -1:
                    board[row_index][col_index] = 1
                    do_something = True
                    new_do_something_row_index.add(row_index)
                    new_do_something_col_index.add(col_index)
            
            # 檢查是否有「確定不是黑色」的格子
            check_or = copy.deepcopy(rows_possible_results[row_index][0])
            for i in range(len(rows_possible_results[row_index])):
                for col_index in range(len(check_or)):
                    check_or[col_index] = check_or[col_index] or rows_possible_results[row_index][i][col_index]
            for col_index in range(len(check_or)):
                if check_or[col_index] == False and board[row_index][col_index] == -1:
                    board[row_index][col_index] = 0
                    do_something = True
                    new_do_something_row_index.add(row_index)
                    new_do_something_col_index.add(col_index)
        
        if print_progress: print('row part done')

        # 更新 cols_possible_results
        for row_index in new_do_something_row_index:
            for col_index in new_do_something_col_index:
                if board[row_index][col_index] != -1:
                    new_possible_results = []
                    for item in cols_possible_results[col_index]:
                        if item[row_index] == board[row_index][col_index]:
                            new_possible_results.append(item)
                    cols_possible_results[col_index] = new_possible_results
        if print_progress:
            print('update cols_possible_results done')



        ### --- Col Part --- ###

        for col_index in do_something_col_index:

            # 檢查是否有「確定是黑色」的格子
            check_and = copy.deepcopy(cols_possible_results[col_index][0])
            for i in range(len(cols_possible_results[col_index])):
                for row_index in range(len(check_and)):
                    check_and[row_index] = check_and[row_index] and cols_possible_results[col_index][i][row_index]
            for row_index in range(len(check_and)):
                if check_and[row_index] == True and board[row_index][col_index] == -1:
                    board[row_index][col_index] = 1
                    do_something = True
                    new_do_something_row_index.add(row_index)
                    new_do_something_col_index.add(col_index)
            
            # 檢查是否有「確定不是黑色」的格子
            check_or = copy.deepcopy(cols_possible_results[col_index][0])
            for i in range(len(cols_possible_results[col_index])):
                for row_index in range(len(check_or)):
                    check_or[row_index] = check_or[row_index] or cols_possible_results[col_index][i][row_index]
            for row_index in range(len(check_or)):
                if check_or[row_index] == False and board[row_index][col_index] == -1:
                    board[row_index][col_index] = 0
                    do_something = True
                    new_do_something_row_index.add(row_index)
                    new_do_something_col_index.add(col_index)
        if print_progress:
            print('col part done')

        # 更新 rows_possible_results
        for col_index in new_do_something_col_index:
            for row_index in new_do_something_row_index:
                if board[row_index][col_index] != -1:
                    new_possible_results = []
                    for item in rows_possible_results[row_index]:
                        if item[col_index] == board[row_index][col_index]:
                            new_possible_results.append(item)
                    rows_possible_results[row_index] = new_possible_results
        if print_progress:
            print('update rows_possible_results done')
        
        do_something_row_index = new_do_something_row_index
        do_something_col_index = new_do_something_col_index

        
        if do_something == False or is_solved(board):
            break

        # print_board(board)

        count += 1
        print('迴圈運行了', count, '次')

    return board

def quick_type_analysis(A , keyword_of_separation = 44):
    result = []
    unit = []
    for item in A:
        if item == keyword_of_separation:
            result.append(unit)
            unit = []
        else:
            unit.append(item)
    print(len(result))
    pprint(result)
    return result

def main():
    
    # quick_type : 
    # 在 rows_condition 以及 cols_condition 中，
    # 用 44 這個數字，來區隔「行與行」或「列與列」，
    # 這樣一來，可以加快輸入 rows_condition 以及 cols_condition 的速度。
    # 注意：最後亦要打上 44
    quick_type = True
    
    rows_condition = [
        32,44,2,1,5,4,1,2,44,1,1,3,5,5,1,1,1,44,1,2,1,2,5,2,2,1,1,44,1,3,2,9,2,3,1,44,1,2,2,2,3,2,2,1,1,44,2,1,3,1,3,1,2,44,5,1,2,3,2,1,4,44,3,2,1,1,1,2,2,44,3,1,3,1,9,1,3,1,2,44,2,1,5,3,1,3,5,1,1,44,1,1,1,1,1,1,3,44,3,10,10,2,44,3,1,2,2,1,1,2,2,1,2,44,4,1,3,3,3,3,1,3,44,1,1,2,7,7,2,1,44,5,5,5,5,44,2,2,4,2,2,4,2,1,44,2,2,1,2,1,1,2,1,2,1,44,1,5,3,1,1,3,5,44,2,7,1,1,7,2,44,2,2,4,2,2,4,2,2,44,2,1,1,11,1,1,2,44,1,8,8,44,1,4,1,3,3,1,4,1,44,2,1,4,7,4,1,2,44,3,1,6,1,6,1,2,44,2,2,1,1,1,1,1,2,1,44,2,3,2,3,2,3,1,44,1,3,1,6,6,1,3,44,1,3,2,3,3,2,3,44,1,3,4,9,4,3,44
    ]
    cols_condition = [
        16,3,3,6,44,2,5,3,2,3,4,44,1,3,3,1,3,2,44,4,1,6,4,44,1,5,4,6,3,4,44,2,2,5,6,3,44,3,1,5,3,2,2,2,1,44,4,4,3,2,3,4,3,44,3,1,2,4,2,2,1,2,2,44,2,2,1,1,2,1,10,1,44,1,2,1,2,2,2,4,1,2,2,44,1,2,3,2,2,2,2,1,2,44,1,1,3,2,1,4,5,1,3,44,1,1,1,1,12,2,2,3,44,1,3,2,3,1,2,1,1,44,1,4,1,1,1,1,2,1,44,1,9,1,4,1,44,1,4,1,1,1,1,2,1,44,1,3,2,3,1,2,1,1,44,1,1,1,1,12,2,2,3,44,1,1,3,2,1,4,5,1,3,44,1,2,3,2,2,2,2,1,2,44,1,2,1,2,2,2,4,1,2,2,44,2,2,1,1,2,1,10,1,44,3,1,2,4,2,2,1,2,2,44,2,1,4,3,2,3,4,3,44,2,2,5,3,2,2,2,1,44,1,2,6,6,3,44,5,2,4,6,3,4,44,1,1,6,4,44,2,4,3,3,3,2,44,11,3,2,3,4,44
    ]

    if quick_type:
        rows_condition = quick_type_analysis(rows_condition)
        cols_condition = quick_type_analysis(cols_condition)
    
    board = []
    M = len(rows_condition)
    N = len(cols_condition)
    print('This is a', M, 'x', N, 'board.')

    for qqq in range(M):
        row = []
        for aaa in range(N):
            row.append(-1)
        board.append(row)
    
    answer = solve(board, rows_condition, cols_condition)
    
    # pprint(answer)
    
    print_board(answer)

if __name__=='__main__':
    main()

# This is the END of the code.