########################################################################
# COMP1521 22T2 -- Assignment 1 -- Connect Four!
#
#
# !!! IMPORTANT !!!
# Before starting work on the assignment, make sure you set your tab-width to 8!
# It is also suggested to indent with tabs only.
# Instructions to configure your text editor can be found here:
#   https://cgi.cse.unsw.edu.au/~cs1521/22T2/resources/mips-editors.html
# !!! IMPORTANT !!!
#
#
# This program was written by Kelly Xu (z5285375)
# on 6/06/2022
#
# Version 1.0 (05-06-2022): Team COMP1521 <cs1521@cse.unsw.edu.au>
#
########################################################################

#![tabsize(8)]

# Constant definitions.
# DO NOT CHANGE THESE DEFINITIONS

# MIPS doesn't have true/false by default
true  = 1
false = 0

# How many pieces we're trying to connect
CONNECT = 4

# The minimum and maximum board dimensions
MIN_BOARD_DIMENSION = 4
MAX_BOARD_WIDTH     = 9
MAX_BOARD_HEIGHT    = 16

# The three cell types
CELL_EMPTY  = '.'
CELL_RED    = 'R'
CELL_YELLOW = 'Y'

# The winner conditions
WINNER_NONE   = 0
WINNER_RED    = 1
WINNER_YELLOW = 2

# Whose turn is it?
TURN_RED    = 0
TURN_YELLOW = 1

########################################################################
# .DATA
# YOU DO NOT NEED TO CHANGE THE DATA SECTION
	.data

# char board[MAX_BOARD_HEIGHT][MAX_BOARD_WIDTH];
board:		.space  MAX_BOARD_HEIGHT * MAX_BOARD_WIDTH

# int board_width;
board_width:	.word 0

# int board_height;
board_height:	.word 0


enter_board_width_str:	.asciiz "Enter board width: "
enter_board_height_str: .asciiz "Enter board height: "
game_over_draw_str:	.asciiz "The game is a draw!\n"
game_over_red_str:	.asciiz "Game over, Red wins!\n"
game_over_yellow_str:	.asciiz "Game over, Yellow wins!\n"
board_too_small_str_1:	.asciiz "Board dimension too small (min "
board_too_small_str_2:	.asciiz ")\n"
board_too_large_str_1:	.asciiz "Board dimension too large (max "
board_too_large_str_2:	.asciiz ")\n"
red_str:		.asciiz "[RED] "
yellow_str:		.asciiz "[YELLOW] "
choose_column_str:	.asciiz "Choose a column: "
invalid_column_str:	.asciiz "Invalid column\n"
no_space_column_str:	.asciiz "No space in that column!\n"


############################################################
####                                                    ####
####   Your journey begins here, intrepid adventurer!   ####
####                                                    ####
############################################################


########################################################################
#
# Implement the following 7 functions,
# and check these boxes as you finish implementing each function
#
#  - [X] main
#  - [X] assert_board_dimension
#  - [X] initialise_board
#  - [X] play_game
#  - [X] play_turn
#  - [X] check_winner
#  - [X] check_line
#  - [X] is_board_full	(provided for you)
#  - [X] print_board	(provided for you)
#
########################################################################


########################################################################
# .TEXT <main>
	.text
main:
	# Args:     void
	# Returns:
	#   - $v0: int
	#
	# Frame:    [$ra]
	# Uses:     [$v0, $t0, $t1, $t2, $a0, $a1, $a2]
	# Clobbers: [$v0, $t0, $t1, $t2, $a0, $a1, $a2]
	#
	# Locals:
	#   - $t0: int board_width (scanned)
        #   - $t1: int board_height (scanned)
        #   - $t2: temp
	#
	# Structure:
	#   main
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

main__prologue:
	begin			                # begin a new stack frame
	push	$ra		                # | $ra

main__body:

	la      $a0, enter_board_width_str
        li      $v0, 4
        syscall

        li      $v0, 5                          # scanf("%d", &board_width);
        syscall
        move    $t0, $v0                        # $t0 = board_width value

        la      $t2, board_width                # t2 = address of board_width
        sw      $t0, 0($t2)                     # store board_width into address of board_width

        move    $a0, $t0                        # assert_board_dimension(board_width, MIN_BOARD_DIMENSION, MAX_BOARD_WIDTH);
        li      $a1, MIN_BOARD_DIMENSION
        li      $a2, MAX_BOARD_WIDTH
        jal     assert_board_dimension

        la      $a0, enter_board_height_str
        li      $v0, 4
        syscall

        li      $v0, 5                          # scanf("%d", &board_height);
        syscall
        move    $t1, $v0                        # $t1 = board_height value
        la      $t2, board_height
        sw      $t1, 0($t2)

        move    $a0, $t1                        # assert_board_dimension(board_height, MIN_BOARD_DIMENSION, MAX_BOARD_HEIGHT);
        li      $a1, MIN_BOARD_DIMENSION
        li      $a2, MAX_BOARD_HEIGHT
        jal     assert_board_dimension

        jal     initialise_board
        jal     print_board
        jal     play_game

main__epilogue:
	pop	$ra		                # | $ra
	end			                # ends the current stack frame

	li	$v0, 0
	jr	$ra		                # return 0;


########################################################################
# .TEXT <assert_board_dimension>
	.text
assert_board_dimension:
	# Args:
	#   - $a0: int dimension
	#   - $a1: int min
	#   - $a2: int max
	# Returns:  void
	#
	# Frame:    [$ra]
	# Uses:     [$v0, $a0, $a1, $a2]
	# Clobbers: [$v0, $a0, $a1, $a2]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   assert_board_dimension
	#   -> [prologue]
	#   -> body
        #   -> max
        #   -> exit
	#   -> [epilogue]

assert_board_dimension__prologue:
assert_board_dimension__body:
        begin
        push    $a0
	bge     $a0, $a1, max                                   # if dimension >= min
        la      $a0, board_too_small_str_1                      # printf("Board dimension too small (min
        li      $v0, 4
        syscall

        move    $a0, $a1                                        # printf("%d", min)
        li      $v0, 1
        syscall

        la      $a0, board_too_small_str_2                      # printf(")\n")
        li      $v0, 4
        syscall
        j       exit
max:
        pop     $a0
        ble     $a0, $a2, assert_board_dimension__epilogue      # dimension <= max
        la      $a0, board_too_large_str_1
        li      $v0, 4
        syscall

        move    $a0, $a2
        li      $v0, 1
        syscall

        la      $a0, board_too_large_str_2
        li      $v0, 4
        syscall

exit:   li      $v0, 10                                         # exit(1)
        syscall

assert_board_dimension__epilogue:
        end
	jr	$ra		                                # return;


########################################################################
# .TEXT <initialise_board>
	.text
initialise_board:
	# Args:     void
	# Returns:  void
	#
	# Frame:    [$ra]
	# Uses:     [$v0, $t0, $t1, $t2, $t3, $t4, $t5, $t6]
	# Clobbers: [$v0, $t0, $t1, $t2, $t3, $t4, $t5, $t6]
	#
	# Locals:
	#   - $t0: int row
        #   - $t1: int col
        #   - $t2: int board_height
        #   - $t3: int board_width
        #   - $t4: address of board
        #   - $t5: temp value
        #   - $t6: address of board[row][col]
	#
	# Structure:
	#   initialise_board
	#   -> [prologue]
	#   -> body
        #   -> rowLoop
        #     -> colLoop
        #   -> rowLoopEnd
	#   -> [epilogue]

initialise_board__prologue:
        la      $t0, board_height
        lw      $t2, 0($t0)             # load board_height -> $t2
        la      $t0, board_width
        lw      $t3, 0($t0)             # load board_width  -> $t3
        li      $t0, 0                  # int row = 0
initialise_board__body:
rowLoop:
        bge     $t0, $t2, initialise_board__epilogue
        li      $t1, 0                  # int col = 0

colLoop:
        bge     $t1, $t3, rowLoopEnd

        la      $t4, board              # $t4 = address of board
        mul     $t5, MAX_BOARD_WIDTH, $t0
        add     $t6, $t4, $t5
        add     $t6, $t6, $t1           # address of board[row][col]

        li      $t5, CELL_EMPTY
        sb      $t5, 0($t6)             # board[row][col] = CELL_EMPTY

        addi    $t1, $t1, 1
        j       colLoop                 # col++

rowLoopEnd:
        addi    $t0, $t0, 1             # row++
        j rowLoop

initialise_board__epilogue:
	jr	$ra		        # return;


########################################################################
# .TEXT <play_game>
	.text
play_game:
	# Args:
        #   - $a0: whose_turn
	# Returns:  void
	#
	# Frame:    [$ra, $a0, $s0]
	# Uses:     [$v0, $s0, $a0, $t0, $t1]
	# Clobbers: [$v0, $s0, $a0, $t0, $t1]
	#
	# Locals:
        #   - $s0: int winner
	#   - $t0: int whose_Turn (intialiser)
        #   - $t1: boolean boardFull()
	#
	# Structure:
	#   play_game
	#   -> [prologue]
	#   -> body
        #   -> result1
        #   -> result2
        #   -> result3
	#   -> [epilogue]

play_game__prologue:
        begin
        push    $ra                             # save play_game return address
        li      $t0, TURN_RED                   # int whose_turn = TURN_RED;
        move    $a0, $t0

play_game__body:
        jal     play_turn                       # whose_turn = play_turn(whose_turn);
        move    $a0, $v0
        push    $a0
        jal     print_board                     # print_board();
        jal     check_winner                    # winner = check_winner();
        move    $s0, $v0
        push    $s0
                                                # while (winner == WINNER_NONE && !is_board_full());
        bne     $s0, WINNER_NONE, play_game__result1
        jal     is_board_full
        move    $t1, $v0
        beq     $t1, 1, play_game__result1      # if boardFull() == True
        pop     $s0
        pop     $a0
        j       play_game__body

play_game__result1:                             # if winner != WINNER_NONE
        pop     $s0
        bne     $s0, WINNER_NONE, play_game__result2
        la      $a0, game_over_draw_str
        li      $v0, 4
        syscall
        pop     $a0
        j       play_game__epilogue

play_game__result2:                             # if winner != WINNER_RED
        bne     $s0, WINNER_RED, play_game__result3
        la      $a0, game_over_red_str
        li      $v0, 4
        syscall
        pop     $a0
        j       play_game__epilogue

play_game__result3:
        la      $a0, game_over_yellow_str
        li      $v0, 4
        syscall
        pop     $a0

play_game__epilogue:
        pop     $ra
        end
	jr	$ra		                # return;


########################################################################
# .TEXT <play_turn>
	.text
play_turn:
	# Args:
	#   - $a0: int whose_turn
	# Returns:  void
	#
	# Frame:    [$ra]
	# Uses:     [$v0, $a0, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9]
	# Clobbers: [$v0, $a0, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9]
	#
	# Locals:
        #   - $t0: int CELL_RED
        #   - $t1: int CELL_YELLOW
        #   - $t2: int target_col
        #   - $t3: int target_row
        #   - $t4: int board_width
        #   - $t5: int board_height
        #   - $t6: whose_turn
        #   - $t7: temp
        #   - $t8: address of board[target_row][target_col]
        #   - $t9: board[target_row][target_col]
        #
	# Structure:
	#   play_turn
	#   -> [prologue]
        #   -> initialise
        #   -> printYellow
	#   -> body
        #   -> printInvalid
        #   -> targetRow
        #   -> targetRowBody
        #   -> changeTurns
        #   -> else
	#   -> [epilogue]

play_turn__prologue:
play_turn__initialise:

        la      $t7, board_width                # $t4 = board_width
        lw      $t4, 0($t7)
        la      $t7, board_height               # $t5 = board_height
        lw      $t5, 0($t7)

        li      $t0, CELL_RED
        li      $t1, CELL_YELLOW
        move    $t6, $a0                        # $t6 = $a0 = whose_turn
        bne     $a0, TURN_RED, play_turn__printYellow

        la      $a0, red_str                    # printf("[RED] ")
        li      $v0, 4
        syscall
        j       play_turn__body

play_turn__printYellow:
        la      $a0, yellow_str
        li      $v0, 4
        syscall

play_turn__body:
        la      $a0, choose_column_str          # printf("Choose a column: ")
        li      $v0, 4
        syscall

        li      $t2, 0                          # int target_col = 0
        li      $v0, 5                          # scanf("%d", &target_col);
        syscall
        move    $t2, $v0
        addi    $t2, $t2, -1                    # target_col--;

        blt     $t2, 0, play_turn__printInvalid
        bge     $t2, $t4, play_turn__printInvalid
        j       play_turn__targetRow

play_turn__printInvalid:
        la      $a0, invalid_column_str         # printf("Invalid column\n");
        li      $v0, 4
        syscall
        move    $v0, $t6                        # return whose_turn
        j       play_turn__epilogue


play_turn__targetRow:
        addi    $t3, $t5, -1                    # int target_row = board_height - 1;
play_turn__targetRowBody:
        blt     $t3, 0, play_turn__changeTurns  # if target_row < 0

        la      $t8, board                      # get address of board
        mul     $t7, $t3, MAX_BOARD_WIDTH
        add     $t8, $t8, $t7
        add     $t8, $t8, $t2                   # $t8 = address of board[target_row][target_col]
        lb      $t9, 0($t8)                     # $t9 = board[target_row][target_col]

        beq     $t9, CELL_EMPTY, play_turn__changeTurns
        addi    $t3, $t3, -1
        bge     $t3, 0, play_turn__targetRowBody

        la      $a0, no_space_column_str
        li      $v0, 4
        syscall
        move    $v0, $t6                        # return whose_turn;
        j       play_turn__epilogue

play_turn__changeTurns:
        bne     $t6, TURN_RED, play_turn__else
        sb      $t0, 0($t8)                     # board[target_row][target_col] = CELL_RED;
        li      $t7, TURN_YELLOW
        move    $v0, $t7                        # return TURN_YELLOW;
        j       play_turn__epilogue
play_turn__else:
        sb      $t1, 0($t8)                     # board[target_row][target_col] = CELL_YELLOW;
        li      $t7, TURN_RED
        move    $v0, $t7                        # return TURN_RED;

play_turn__epilogue:
	jr	$ra		                # return;


########################################################################
# .TEXT <check_winner>
	.text
check_winner:
	# Args:	    void
	# Returns:
	#   - $v0: int
	#
	# Frame:    [$ra, $t0, $t1]
	# Uses:     [$v0, $a0, $s0, $s1, $a1, $a2, $a3, $t1, $t2, $t3]
	# Clobbers: [$v0, $a0, $s0, $s1, $a1, $a2, $a3, $t1, $t2, $t3]
	#
	# Locals:
	#   - $s0: int row
        #   - $s1: int col
        #   - $t1: temp
        #   - $t2: board_height
        #   - $t3: board_ width
	#
	# Structure:
	#   check_winner
	#   -> [prologue]
	#   -> body
        #   -> rowLoop
        #   -> colLoop
        #   -> rowLoopEnd
        #   -> returnCheck
        #   -> none
	#   -> [epilogue]

check_winner__prologue:
        begin
        push    $ra

check_winner__body:
        la      $t1, board_height               # $t2 = board_height
        lw      $t2, 0($t1)
        la      $t1, board_width                # $t3 = board_width
        lw      $t3, 0($t1)
        li      $s0, 0                          # int row = 0
check_winner__rowLoop:
        bge     $s0, $t2, check_winner__none
        li      $s1, 0                          # int col = 0
check_winner__colLoop:
        bge     $s1, $t3, check_winner__rowLoopEnd

        push    $s0
        push    $s1
        move    $a0, $s0                        # check = check_line(row, col, 1, 0);
        move    $a1, $s1
        li      $a2, 1
        li      $a3, 0
        jal     check_line
        bne     $v0, WINNER_NONE, check_winner__returnCheck

        pop     $s1
        pop     $s0
        push    $s0
        push    $s1

        move    $a0, $s0                        # check = check_line(row, col, 0, 1);
        move    $a1, $s1
        li      $a2, 0
        li      $a3, 1
        jal     check_line
        bne     $v0, WINNER_NONE, check_winner__returnCheck

        pop     $s1
        pop     $s0
        push    $s0
        push    $s1

        move    $a0, $s0                        # check = check_line(row, col, 1, 1);
        move    $a1, $s1
        li      $a2, 1
        li      $a3, 1
        jal     check_line
        bne     $v0, WINNER_NONE, check_winner__returnCheck

        pop     $s1
        pop     $s0
        push    $s0
        push    $s1

        move    $a0, $s0                        # check = check_line(row, col, 1, -1);
        move    $a1, $s1
        li      $a2, 1
        li      $a3, -1
        jal     check_line
        bne     $v0, WINNER_NONE, check_winner__returnCheck

        pop     $s1
        pop     $s0

        addi    $s1, $s1, 1                     # col++
        j       check_winner__colLoop

check_winner__rowLoopEnd:
        addi    $s0, $s0, 1                     # row ++
        j       check_winner__rowLoop

check_winner__returnCheck:                      # return check;
        pop     $s1
        pop     $s0
        j       check_winner__epilogue

check_winner__none:
        li      $v0, WINNER_NONE                # return WINNER_NONE
check_winner__epilogue:
        pop     $ra
        end
	jr	$ra


########################################################################
# .TEXT <check_line>
	.text
check_line:
	# Args:
	#   - $a0: int start_row
	#   - $a1: int start_col
	#   - $a2: int offset_row
	#   - $a3: int offset_col
	# Returns:
	#   - $v0: int
	#
	# Frame:    [$ra]
	# Uses:     [$a0, $v0, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9]
	# Clobbers: [$a0, $v0, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9]
	#
	# Locals:
	#   - $t0: temp
        #   - $t1: CONNECT - 1
        #   - $t2: board_height
        #   - $t3: board_width
        #   - $t4: temp address
        #   - $t5: char first_cell
        #   - $t6: int row
        #   - $t7: int col
        #   - $t8: int i
        #   - $t9: char cell
	#
	# Structure:
	#   check_line
	#   -> [prologue]
        #   -> initialise
	#   -> body
        #   -> for
        #   -> winner
        #   -> yellowWinner
        #   -> winnerNone
	#   -> [epilogue]

check_line__prologue:
check_line__initialise:
        la      $t0, board_height               # $t2 = board_height
        lw      $t2, 0($t0)
        la      $t0, board_width                # $t3 = board_width
        lw      $t3, 0($t0)

check_line__body:

        la      $t0, board
        mul     $t4, $a0, MAX_BOARD_WIDTH
        add     $t4, $t4, $t0                   # $t4 = address of board[target_row][target_col]
        add     $t4, $t4, $a1
        lb      $t5, 0($t4)                     # char first_cell = board[start_row][start_col];

        beq     $t5, CELL_EMPTY, check_line__winnerNone

        add     $t6, $a0, $a2                   # int row = start_row + offset_row;
        add     $t7, $a1, $a3                   # int col = start_col + offset_col;

        li      $t8, 0                          # int i = 0
        li      $t1, CONNECT
        addi    $t1, $t1, -1
check_line__for:

        bge     $t8, $t1, check_line__winner

        blt     $t6, 0, check_line__winnerNone  # if (row < 0 || col < 0) return WINNER_NONE;
        blt     $t7, 0, check_line__winnerNone
        bge     $t6, $t2, check_line__winnerNone# if (row >= board_height || col >= board_width) return WINNER_NONE;
        bge     $t7, $t3, check_line__winnerNone


        la      $t0, board                      # char cell = board[row][col];
        mul     $t4, $t6, MAX_BOARD_WIDTH
        add     $t4, $t4, $t7
        add     $t4, $t4, $t0                   # $t4 = address of board[row][col]
        lb      $t9, 0($t4)                     # char cell = board[row][col];

        bne     $t9, $t5, check_line__winnerNone# if (cell != first_cell) return WINNER_NONE;

        addi    $t8, $t8, 1                     # i++
        add     $t6, $t6, $a2                   # row += offset_row;
        add     $t7, $t7, $a3                   # col += offset_col;
        j       check_line__for

check_line__winner:
        bne     $t5, CELL_RED, check_line__yelloWinner
        li      $v0, WINNER_RED
        j       check_line__epilogue

check_line__yelloWinner:
        li      $v0, WINNER_YELLOW
        j       check_line__epilogue

check_line__winnerNone:
        li      $v0, WINNER_NONE
        j       check_line__epilogue

check_line__epilogue:
	jr	$ra		                # return;


########################################################################
# .TEXT <is_board_full>
# YOU DO NOT NEED TO CHANGE THE IS_BOARD_FULL FUNCTION
	.text
is_board_full:
	# Args:     void
	# Returns:
	#   - $v0: bool
	#
	# Frame:    []
	# Uses:     [$v0, $t0, $t1, $t2, $t3]
	# Clobbers: [$v0, $t0, $t1, $t2, $t3]
	#
	# Locals:
	#   - $t0: int row
	#   - $t1: int col
	#
	# Structure:
	#   is_board_full
	#   -> [prologue]
	#   -> body
	#   -> loop_row_init
	#   -> loop_row_cond
	#   -> loop_row_body
	#     -> loop_col_init
	#     -> loop_col_cond
	#     -> loop_col_body
	#     -> loop_col_step
	#     -> loop_col_end
	#   -> loop_row_step
	#   -> loop_row_end
	#   -> [epilogue]

is_board_full__prologue:
is_board_full__body:
	li	$v0, true

is_board_full__loop_row_init:
	li	$t0, 0						# int row = 0;

is_board_full__loop_row_cond:
	lw	$t2, board_height
	bge	$t0, $t2, is_board_full__epilogue		# if (row >= board_height) goto is_board_full__loop_row_end;

is_board_full__loop_row_body:
is_board_full__loop_col_init:
	li	$t1, 0						# int col = 0;

is_board_full__loop_col_cond:
	lw	$t2, board_width
	bge	$t1, $t2, is_board_full__loop_col_end		# if (col >= board_width) goto is_board_full__loop_col_end;

is_board_full__loop_col_body:
	mul	$t2, $t0, MAX_BOARD_WIDTH			# row * MAX_BOARD_WIDTH
	add	$t2, $t2, $t1					# row * MAX_BOARD_WIDTH + col
	lb	$t3, board($t2)					# board[row][col];
	bne	$t3, CELL_EMPTY, is_board_full__loop_col_step	# if (cell != CELL_EMPTY) goto is_board_full__loop_col_step;

	li	$v0, false
	b	is_board_full__epilogue				# return false;

is_board_full__loop_col_step:
	addi	$t1, $t1, 1					# col++;
	b	is_board_full__loop_col_cond			# goto is_board_full__loop_col_cond;

is_board_full__loop_col_end:
is_board_full__loop_row_step:
	addi	$t0, $t0, 1					# row++;
	b	is_board_full__loop_row_cond			# goto is_board_full__loop_row_cond;

is_board_full__loop_row_end:
is_board_full__epilogue:
	jr	$ra						# return;


########################################################################
# .TEXT <print_board>
# YOU DO NOT NEED TO CHANGE THE PRINT_BOARD FUNCTION
	.text
print_board:
	# Args:     void
	# Returns:  void
	#
	# Frame:    []
	# Uses:     [$v0, $a0, $t0, $t1, $t2]
	# Clobbers: [$v0, $a0, $t0, $t1, $t2]
	#
	# Locals:
	#   - `int col` in $t0
	#   - `int row` in $t0
	#   - `int col` in $t1
	#
	# Structure:
	#   print_board
	#   -> [prologue]
	#   -> body
	#   -> for_header_init
	#   -> for_header_cond
	#   -> for_header_body
	#   -> for_header_step
	#   -> for_header_post
	#   -> for_row_init
	#   -> for_row_cond
	#   -> for_row_body
	#     -> for_col_init
	#     -> for_col_cond
	#     -> for_col_body
	#     -> for_col_step
	#     -> for_col_post
	#   -> for_row_step
	#   -> for_row_post
	#   -> [epilogue]

print_board__prologue:
print_board__body:
	li	$v0, 11			# syscall 11: print_int
	la	$a0, '\n'
	syscall				# printf("\n");

print_board__for_header_init:
	li	$t0, 0			# int col = 0;

print_board__for_header_cond:
	lw	$t1, board_width
	blt	$t0, $t1, print_board__for_header_body	# col < board_width;
	b	print_board__for_header_post

print_board__for_header_body:
	li	$v0, 1			# syscall 1: print_int
	addiu	$a0, $t0, 1		#              col + 1
	syscall				# printf("%d", col + 1);

	li	$v0, 11			# syscall 11: print_character
	li	$a0, ' '
	syscall				# printf(" ");

print_board__for_header_step:
	addiu	$t0, 1			# col++
	b	print_board__for_header_cond

print_board__for_header_post:
	li	$v0, 11
	la	$a0, '\n'
	syscall				# printf("\n");

print_board__for_row_init:
	li	$t0, 0			# int row = 0;

print_board__for_row_cond:
	lw	$t1, board_height
	blt	$t0, $t1, print_board__for_row_body	# row < board_height
	b	print_board__for_row_post

print_board__for_row_body:
print_board__for_col_init:
	li	$t1, 0			# int col = 0;

print_board__for_col_cond:
	lw	$t2, board_width
	blt	$t1, $t2, print_board__for_col_body	# col < board_width
	b	print_board__for_col_post

print_board__for_col_body:
	mul	$t2, $t0, MAX_BOARD_WIDTH
	add	$t2, $t1
	lb	$a0, board($t2)		# board[row][col]

	li	$v0, 11			# syscall 11: print_character
	syscall				# printf("%c", board[row][col]);

	li	$v0, 11			# syscall 11: print_character
	li	$a0, ' '
	syscall				# printf(" ");

print_board__for_col_step:
	addiu	$t1, 1			# col++;
	b	print_board__for_col_cond

print_board__for_col_post:
	li	$v0, 11			# syscall 11: print_character
	li	$a0, '\n'
	syscall				# printf("\n");

print_board__for_row_step:
	addiu	$t0, 1
	b	print_board__for_row_cond

print_board__for_row_post:
print_board__epilogue:
	jr	$ra			# return;

