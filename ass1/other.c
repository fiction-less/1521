
void assert_board_dimension
min:
if dimension >= goto max
    print "board dimenstion too small"
    print %d\n , min
    goto end


max:
if dimension <= max goto end
    print " "
    print %d\n max

end:


void initialise_board(void)
outLoop:
    int row = 0;
    if row >= boardHeight goto end
    int col = 0
        innerLoop:
            if col >= boardWidth goto outLoopEnd
            baord[row][col] = CELL_EMPTY
            col ++
            j innerLoop

outLoopEnd:
    row ++
    j outLoop;

end:


void play_game(void)
int whose_Turn = TURN_RED
int winner

do:
    whose_turn = play_turn(whose_turn)
    print_board()
    winner = check_winner()

    if winner != WINNER_NONE goto printResult
    if boardFull() == true goto printResult
    j do

printResult:
    if winner != WINNER_NONE j result2
        print "draw"
        j end

result2:
    if != WINNER_RED j result 3
    print "red wins"
    j end

resul3:
    print "yellow wins"

end:


int play_turn(int whose_turn)
    int TURN_RED
    int TURN_YELLO
    int target_col
    int target_row

    if whose_turn != TURN_RED goto yellow
    print RED
    j   body
yellow:
    print YELLOW

body:
    print choose a column
    target_col = 0
    scanf("%d", &target_col);
	target_col--
    if target_col < 0 goto printInvalid
    if target_col >= boardWidth goto print invalid
    j  targetRow Prologue

printInvalid:
    print invalid Column
    return whose_turn in v0 goto end

targetrowPrologue:
    target_row = board_height - 1;
targetRow:

    if target_row < 0 goto whoseTurn
    if board[target_Row][target_col] == CELL_empty goto whoseTurn
    target row --
    if targetrow >= - goto targetRow
    print no space
    return whose_turn in v0 goto end

whoseTurn:
    if whoteturn != red goto else
    board[target_row][target_col] = CELL_RED;
	return TURN_YELLOW;
else:
    board[target_row][target_col] = CELL_YELLOW;
		return TURN_RED;




int check_winner(void)
    push ra
    int row = 0
rowLoop:
    if row >= board_height goto end

    int col = 0
colLoop:
    if col >= board_width goto rowEnd
    int check

    check = check_line(row, col, 1, 0);
    if (check != WINNER_NONE) return check;

    check = check_line(row, col, 0, 1);
    if (check != WINNER_NONE) return check;

    check = check_line(row, col, 1, 1);
    if (check != WINNER_NONE) return check;

    check = check_line(row, col, 1, -1);
    if (check != WINNER_NONE) return check;

    col++
    j colLoop

rowEnd:
    row++
    j rowLoop
    pop ra