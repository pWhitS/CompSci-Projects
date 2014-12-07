AdjacentSquaresGame
===================

Artificial Neural Network  

This program plays the game Adjacent Squares, and uses an artificial neural network to learn from its previous games. The game is can play against itself as training, or against a human player via the web interface.

For training the game is compiled using the provided make file, and can then be ran using the generated executable.

    make -f trainer.mak
    ./trainer.x
    
The training program will then output the results of each game that it plays. 

    --Sample output--
    Draw... no winner found after 200 moves.
    Player2 Wins!
    Moves: 35
    Draw... no winner found after 200 moves.
    Draw... no winner found after 200 moves.

During the training process the training program outputs new weight values after each training game.

To play the game, the program is compiled manually and must be located on a web server.

    g++ -o player.cgi player.cpp annet.cpp
    
The human player will then use adjacentSquares.html, which will then communicate player.cgi through AJAX requests. 
    
