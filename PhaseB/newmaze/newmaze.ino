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
class maze_layout {
  private:
    int hWall[4][9];
    int vWall[5][8];
    int rows;
    int cols;
  public:
    maze_layout(int rows, int cols, String const &h, String const &v) {
      this->rows = rows;
      this->cols = cols;
      fillHorizontal(h);
      fillVertical(v);
    }
    void fillHorizontal(String const& h) {
      for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols; j++) {
          hWall[i][j] = h.charAt(cols * i + j) - '0';
          Serial.print(hWall[i][j]);
        }
        Serial.print("\n");
      }
    }
    void fillVertical(String const& v) {
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols - 1; j++) {
          vWall[i][j] = v.charAt((cols - 1) * i + j) - '0';
          Serial.print(vWall[i][j]);
        }
        Serial.print("\n");
      }
    }

    void print() {
      for (int i = 0; i <= rows; i++) {
        for (int j = 0; j < cols; j++) {
          if (i == 0 || i == rows) {
            Serial.print(" ---");
          }
          else {
            if (hWall[i - 1][j]) {
              Serial.print(" ---");
            }
            else {
              Serial.print("    ");
            }
          }
        }
        Serial.print("\n");
        if (i == rows) {
          break;
        }
        for (int k = 0; k <= cols; k++) {
          if (k == 0 || k == cols) {
            Serial.print("|   ");
          }
          else {
            if (vWall[i][k - 1]) {
              Serial.print("|   ");
            }
            else {
              Serial.print("    ");
            }
          }
        }
        Serial.print("\n");
      }
    }
};

class new_maze_layout {
  private:
    int cells[5][9];
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
          // set every cells to -1
          cells[i][j] = -1;
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
        int decimal = h.charAt(index) - '0';
        index++;
        int digit = h.charAt(index) - '0';
        index++;
        // Now update the cell data
        cells[i][j] = decimal * 10 + digit;
        //updateAdjCells(i,j,decimal * 10 + digit);
      }
    }

//    void updateAdjCells(int i, int j, int data) {
//
//    }
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
          if (cells[i][j] == -1) {
            Serial.print("*");
          } else {
            if (hasEastWall(cells[i][j])) {
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
          if (cells[i][j] == -1) {
            Serial.print(" ***");
          } else {
            if (hasSouthWall(cells[i][j])) {
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
