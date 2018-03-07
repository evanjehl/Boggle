This program implements a Boggle game between the user and the computer. Sixteen arrays of characters implement the letter dice. To begin the game, each die array is assigned a random index and position on the board to simulate shaking them. The user is then given to chance to form words of four letters or more on the board, which the program validates against the board and scores accordingly. Once the user has finished, the computer uses a recursive algorithm to explore every possible legal path on the Boggle board, validating the resulting string of each path against a dictionary text file to ensure a valid English word and eliminating any words that the user has already found. The computer is then scored, and the player with the highest score wins.
