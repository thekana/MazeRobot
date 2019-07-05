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
  public:
    int cells[5][9][4];
    String statusCells[5][9];
    int rows = 5;
    int cols = 9;

    new_maze_layout(String const &h) {
      this->rows = 5;
      this->cols = 9;
      initializeCells();  // -1 is unexplored
      initializeStatusCells(); // 4 spaces
      //fillCells(h);
      updateStatusCells();
    }
    void initializeCells() {
      for (int i = 0; i < 5 ; i++) {
        for (int j = 0; j < 9; j++) {
          for (int k = 0; k < 4; k++) {
            cells[i][j][k] = -1;
            if (i == 0) {
              cells[i][j][0] = 1;
            }
            if (i == 4) {
              cells[i][j][2] = 1;
            }
            if (j == 0) {
              cells[i][j][3] = 1;
            }
            if (j == 8) {
              cells[i][j][1] = 1;
            }
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
        // BITMASK E-S-W-N
        cells[i][j][0] = (data & B1000) >> 3;
        cells[i][j][1] = (data & B0100) >> 2;
        cells[i][j][2] = (data & B0010) >> 1;
        cells[i][j][3] = (data & B0001) >> 0;
        updateAdjCells(i, j, data);
      }
    }

    void updateAdjCells(int i, int j, int data) {
      if ( i >= 0 && i < 4 ) {
        // can go down
        cells[i + 1][j][0] = cells[i][j][2];
      }
      if ( i <= 4 && i > 0 ) {
        // can go up
        cells[i - 1][j][2] = cells[i][j][0];
      }
      if ( j >= 0 && j < 8 ) {
        cells[i][j + 1][3] =  cells[i][j][1];
      }
      if ( j <= 8 && j > 0 ) {
        cells[i][j - 1][1] =  cells[i][j][3];
      }
      if ( i > 0 && i < 4 && j > 0 && j < 8 ) {
        cells[i + 1][j][0] = cells[i][j][2];
        cells[i - 1][j][2] = cells[i][j][0];
        cells[i][j + 1][3] =  cells[i][j][1];
        cells[i][j - 1][1] =  cells[i][j][3];
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
          if (cells[i][j][1] == -1) {
            Serial.print("*");
          } else {
            if (cells[i][j][1] == 1) {
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
          if (cells[i][j][2] == -1) {
            Serial.print(" ***");
          } else {
            if (cells[i][j][2] == 1) {
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
class FloodFill {
  private:
    new_maze_layout *maze;
    int cell[5][9];
    int rows = 5;
    int cols = 9;
    int noWallsPathCount;
    int wallsPathCount;
    bool wall = true;
  public:
    FloodFill(new_maze_layout *maze = nullptr) {
      this->maze = maze;
    }
    void AssumeNoWalls() {
      this->wall = false;
    }
    void AssumeWalls() {
      this->wall = true;
    }
    void doFloodFill() {
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          this->cell[i][j] = 50;
        }
      }
      this->cell[2][4] = 0;
      int currExploredValue = 0;
      int mazeValueChange = 1;
      while (mazeValueChange != 0) {
        mazeValueChange = 0;
        for (int i = 0; i < rows; i++) {
          for (int j = 0; j < cols; j++) {
            if (cell[i][j] == currExploredValue) {
              Serial.print("I is: "); Serial.print(i); Serial.print(" J is: "); Serial.print(j); Serial.print(" Curr is: "); Serial.print(currExploredValue);
              Serial.print("\n");
              for (int k = 0; k < 4 ; k++) {
                // Check unexplored wall assumption
                if (maze->cells[i][j][k] == -1) {
                  // unexplored wall assume no walls
                  if ( !wall ) {
                    // do update
                    mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
                  }
                } else if (maze->cells[i][j][k] == 0) {
                  Serial.print("I is: "); Serial.print(i); Serial.print(" J is: "); Serial.print(j); Serial.print(" K is: "); Serial.print(k);
                  Serial.print("\n");
                  mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
                }
              }
            }
          }
        }
        currExploredValue++;
      }
    }
    int incrementNeighbour(int i, int j, int k, int value) {
      if (k == 0) {
        i = i - 1;
      } else if (k == 1) {
        j = j + 1;
      } else if (k == 2) {
        i = i + 1;
      } else {
        j = j - 1;
      }
      if (cell[i][j] == 50) {
        cell[i][j] = value + 1;
        return 1;
      } else {
        return 0;
      }
    }
    void print() {
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          Serial.print(cell[i][j]);
          Serial.print("\t");
        }
        Serial.print("\n");
      }
      Serial.print("\n");
      printCell(4, 4);
      printCell(4, 5);
      printCell(0, 4);
      printCell(0, 5);
      printCell(1, 5);
      printCell(1, 4);
    }
    void printCell(int i, int j) {
      for (int k = 0; k < 4 ; k++) {
        Serial.print(maze->cells[i][j][k]);
      }
      Serial.print("\n");
    }
};
maze_layout_message maze;
new_maze_layout *lay = new new_maze_layout("");
FloodFill ff(lay);
//new_maze_layout lay("");
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
    if (c.startsWith("f")) {
      ff.AssumeNoWalls();
      ff.doFloodFill();
      ff.print();
      ff.AssumeWalls();
      ff.doFloodFill();
      ff.print();
    } else if (c.startsWith("P")) {
      ff.printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    } else {
      lay->fillCells(c);
      lay->printNew();
    }
    //maze.setMessage(c);
    //maze_layout layout(maze.rows, maze.cols, maze.hWall, maze.vWall);
    //layout.print();
  }
}

// ESWN e.g. All 4 walls is F in HEX 1111
// Give Cell position 00F
//   E
//  N S
//   W
// 00B01002A03A04E116109
// 00B01002A03A04E11610920021A22412913C235
// 00B01002A03A04E11610920021A22412913C23530531B32433540341A42243244A
// 00B01802A03A04E11610920021A22412913C23530531B32433540341A42243244A456358343249
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
