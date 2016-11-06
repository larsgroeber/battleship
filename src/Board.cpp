//
// Created by lars on 11/4/16.
//

#include <sstream>
#include <iostream>

#include "../inc/Board.h"
#include "../inc/Error.h"

using namespace std;

//// PUBLIC METHODS ////

/**
 * constructs the object, initialise all variables which are important
 * for the look of the board and set the board to its initial state
 */
Board::Board ( unsigned int x, unsigned int y, WINDOW* win )
    : dim     ( x, y )
    , window  ( win )
    , board ( dim.second ) // y_dim is the # of cols, so the vectors in *board* represent the rows
    , vSpace    ( 1 )
    , posCursorStart ( 1, 1 ) // this is now in local coords of the board
    , posCursor ( posCursorStart )
{
    // set some cosmetics
    fieldSeparate = "  ";  // horizontal separation of fields
    fieldFree     =  ".";  // symbol for a field where you can shoot
    fieldFail     =  "x";  // symbol for a field where you missed
    fieldSuccess  =  "o";  // symbol for a field where you hit a ship

    // build vector *board*
    // remember x are the columns and y the rows
    for ( int row = 0; row < dim.second; ++row )
    {
        // initialise row vector
        board[row] = vector<string> ( dim.first );
        for ( int col = 0; col < dim.first; ++col )
        {
            board[row][col] = fieldFree;
        }
    }
}

/**
 * prints the board in the middle of the window and adds numbers to cols and letters to rows
 */
void Board::draw () const
{
    update();

    // draw the first line with numbers
    // TODO: after 10 we need less space between numbers
    vector<string> firstLine ( dim.first + 1 );
    firstLine[0] = " ";
    for ( int i = 1; i < firstLine.size(); ++i ) {
        firstLine[i] = to_string(i);
    }
    mvprintw( bStartGlobal.second, bStartGlobal.first
            , "%s", vector_to_string(firstLine, fieldSeparate).c_str() );

    // draw the board onto the window
    for ( unsigned int row = 0; row < board.size(); ++row ) {
        mvprintw ( bStartGlobal.second + vSpace * row + 1, bStartGlobal.first
                 , "%c%s", (const char)( 'A' + row ), fieldSeparate.c_str() );
        printw ( "%s", vector_to_string( board[row], fieldSeparate ).c_str() );
    }

    reset_cursor();
}


bool Board::set_field ( const std::pair<int, int> pos, const std::string& type )
{
    if ( pos.first  < 1 || pos.first  > dim.first
      || pos.second < 1 || pos.second > dim.second )
    {
        throw Error ( "Position is not on board in Board::set_field!" );
    }

    if ( board[pos.second - 1][pos.first - 1] != fieldFree )
    {
        return false;
    }

    string replace;
    if ( type == "FAIL" )
    {
        replace = fieldFail;
    }
    else if ( type == "SUCCESS" )
    {
        replace = fieldSuccess;
    }
    else
    {
        throw Error ( "Wrong replacement type in Board::set_field!" );
    }
    board[pos.second - 1][pos.first - 1] = replace;
    return true;
}


std::pair<int,int> Board::get_cursor_pos () const
{
    return posCursor;
}

std::vector<std::vector<std::string>> Board::get_board () const
{
    return board;
}

std::pair<int, int> Board::get_dim () const
{
    return dim;
}

/**
 * loop through board and check if you find a field which is still free
 */
bool Board::is_board_full ()
{
    for ( int row = 0; row < board.size(); ++row ) {
        for ( int col = 0; col < board[row].size(); ++col ) {
            if ( board[row][col] == fieldFree )
                return false;
        }
    }
    return true;
}


void Board::reset_cursor () const
{
    // move the cursor to starting position
    move_local( posCursor );
}


bool Board::user_move_cursor ( const int key ) const
{
    pair<int,int> newPos = posCursor;
    switch ( key )
    {
        case KEY_RIGHT:
            newPos.first++;
            break;
        case KEY_LEFT:
            newPos.first--;
            break;
        case KEY_DOWN:
            newPos.second++;
            break;
        case KEY_UP:
            newPos.second--;
            break;
        default:
            // throw an error?
            break;
    }

    // move cursor
    return move_local( newPos );
}

//// PRIVATE METHODS ////

bool Board::move_local ( std::pair<int, int> pos ) const
{
    // check if position is on the board
    if ( pos.first  < 1 || pos.first  > dim.first
      || pos.second < 1 || pos.second > dim.second)
    {
        return false;
    }
    else
    {
        int sepLength = (int)fieldSeparate.length();
        move( pos.second + bStartGlobal.second
            , bStartGlobal.first + pos.first * ( sepLength + 1 ) );
        posCursor = pos;
        return true;
    }
}


void Board::update () const
{
    // first get maximal dimensions of window
    int maxX, maxY;
    getmaxyx( window, maxY, maxX );

    int sepLength = (int)fieldSeparate.length();

    // starting pos (you need to add 1 to x/y_dim for the actual size of the board
    bStartGlobal = make_pair( ( maxX - dim.first * ( sepLength + 1 ) - 1 ) / 2
            , ( maxY - dim.second - 1 ) / 2 );
}


/**
 * helper function
 */
template <typename T>
string Board::vector_to_string ( const vector<T>& vector, const string& separate ) const
{
    // use streams as they are heavily overloaded
    stringstream finalStrStr;

    for ( int i = 0; i < vector.size(); ++i ) {
        finalStrStr << vector[i] << separate;
    }

    return finalStrStr.str();
}

