#include <Wire.h>

class maze_layout_message {
  public:
    int rows;
    int cols;
    String vWall;
    String hWall;

    maze_layout_message() {
      rows = 0;
      cols = 0;
    }
    maze_layout_message(String const &msg) {
      setMessage(msg);
    }
    void setMessage(String const &msg) {
      rows = msg.charAt(0) - '0';
      cols = msg.charAt(1) - '0';
      hWall = msg.substring(2, msg.indexOf('!'));
      vWall = msg.substring(msg.indexOf('!') + 1);
    }
    String toString() {
      String str = String("Maze size is " + String(rows) + "x" + String(cols) + " Encoded Msg hWall & vWall:" + hWall + " & " + vWall );
      return str;
    }
};
int x2i(char *s)
{
  int x = 0;
  for (;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10;
    }
    else break;
    s++;
  }
  return x;
}
class new_maze_layout {
  private:
    int cells[5][9][4];
    String statusCells[5][9];
    int rows = 5;
    int cols = 9;

  public:
    new_maze_layout(String const &h) {
      this->rows = 5;
      this->cols = 9;
      initializeCells();  // -1 is unexplored
      initializeStatusCells(); // 4 spaces
      fillCells(h);
      updateStatusCells();
    }
    void initializeCells() {
      for (int i = 0; i < 5 ; i++) {
        for (int j = 0; j < 9; j++) {
          for (int k = 0; k < 4; k++) {
            cells[i][j][k] = -1;
          }
        }
      }
    }
    void initializeStatusCells() {
      for (int i = 0; i < 5 ; i++) {
        for (int j = 0; j < 9; j++) {
          // set every cells to -1
          statusCells[i][j] = "   ";
        }
      }
    }

    void updateStatusCells() {
      // to update these cells with path
      // data and start/end
      statusCells[2][4] = " X ";
      statusCells[0][0] = " S ";
    }
    void fillCells(String const& h) {
      int index = 0;
      int len = h.length();
      while (index < len) {
        int i = h.charAt(index) - '0';
        index++;
        int j = h.charAt(index) - '0';
        index++;
        char a = h.charAt(index);
        int data = x2i(&a);
        index++;
        // Now update the cell data
        // int data = decimal * 10 + digit;
        // BITMASK N-S-E-W
        cells[i][j][0] = (data & B1000) >> 3;
        cells[i][j][1] = (data & B0100) >> 2;
        cells[i][j][2] = (data & B0010) >> 1;
        cells[i][j][3] = (data & B0001) >> 0;
        updateAdjCells(i, j, data);
      }
    }

    void updateAdjCells(int i, int j, int data) {
      if ( i == 0 ) {
        cells[i + 1][j][0] = cells[i][j][1];
      }
      if ( i == 4 ) {
        cells[i - 1][j][1] = cells[i][j][0];
      }
      if ( j == 0 ) {
        cells[i][j + 1][3] =  cells[i][j][2];
      }
      if ( j == 8 ) {
        cells[i][j - 1][2] =  cells[i][j][3];
      }
      if ( i > 0 && i < 4 && j > 0 && j < 8 ) {
        cells[i + 1][j][0] = cells[i][j][1];
        cells[i - 1][j][1] = cells[i][j][0];
        cells[i][j - 1][2] =  cells[i][j][3];
        cells[i][j + 1][3] =  cells[i][j][2];
      }
    }
    bool hasEastWall(int v) {
      return (v & B0010);
    }
    bool hasSouthWall(int v) {
      return (v & B0100);
    }
    void printNew() {
      for (int j = 0; j < cols; j++) {
        Serial.print(" ---"); // Print top walls
      }
      Serial.print("\n");
      for (int i = 0; i < rows; i++) {
        // Print Vertical;
        Serial.print("|"); // left most wall
        for (int j = 0; j < cols - 1; j++) {
          Serial.print(statusCells[i][j]);
          if (cells[i][j][2] == -1) {
            Serial.print("*");
          } else {
            if (cells[i][j][2] == 1) {
              Serial.print("|");
            } else {
              Serial.print(" ");
            }
          }
        }
        Serial.print(statusCells[i][cols - 1]);
        Serial.print("|"); // right most wall
        Serial.print("\n");
        if (i == 4) {
          break;
        }
        for (int j = 0; j < cols; j++) {
          if (cells[i][j][1] == -1) {
            Serial.print(" ***");
          } else {
            if (cells[i][j][1] == 1) {
              Serial.print(" ---");
            } else {
              Serial.print("    ");
            }
          }
        }
        Serial.print("\n");
      }
      for (int j = 0; j < cols; j++) {
        Serial.print(" ---"); // Print closing bottom walls
      }
      Serial.print("\n");
    }
};

maze_layout_message maze;

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
}

void loop()
{
  if (Serial.available()) {
    String c = Serial.readString();
    Serial.println(c);
    //maze.setMessage(c);
    //maze_layout layout(maze.rows, maze.cols, maze.hWall, maze.vWall);
    //layout.print();
    new_maze_layout lay(c);
    lay.printNew();
  }
}

// N-S-E-W e.g. All 4 walls is F in HEX 1111
// Give Cell position 00F
//00F
// --- --- --- --- --- --- --- --- ---
//| S |   *   *   *   *   *   *   *   |
// --- *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   * X *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// --- --- --- --- --- --- --- --- ---
