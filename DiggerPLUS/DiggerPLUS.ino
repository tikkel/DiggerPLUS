#include <Gamebuino-Meta.h>
#include "DiggerPLUS.h"
#include "Sprites.h"
#include "Levels.h"
#include "Menu.h"
#include "Sounds.h"

byte world[WORLD_W][WORLD_H];

//generate Multilang-Array for pause menu
const MultiLang* Pausemenu_easy[MENULENGTH] = {
  lang_resume,
  lang_new,
  lang_levelup,
  lang_leveldown,
  lang_toggle_easy,
  lang_score
};
const MultiLang* Pausemenu_hard[MENULENGTH] = {
  lang_resume,
  lang_new,
  lang_levelup,
  lang_leveldown,
  lang_toggle_hard,
  lang_score
};

// cursor
int cursor_x, cursor_y;
int camera_x, camera_y;
bool scroll_xp, scroll_xm, scroll_yp, scroll_ym = false;
int display_width = gb.display.width();
int display_height = gb.display.height();

int score = 0;
int scoreeasy;
int scorehard;
int highscore[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};
char scorename[7][9 + 1] = {
  "         ",
  "         ",
  "         ",
  "         ",
  "         ",
  "         ",
  "         ",
};
int countdown = 5000;
byte diams_need, diamonds;
byte curlevel = 0;
byte curhard = 0;
byte cureasy = 0;
byte lives = 0;
byte liveseasy = 0;
byte liveshard = 0;
bool dead = false;
byte gamemode = 0;
byte gamestate = PAUSED;
int codepos = 0;
int worldpos = 0;
byte dir = DIGGER_I;
byte ldir = DIGGER_I;
byte digger_step = 0;
byte idle_step = 0;
bool countdown_step = false;
bool countdown_toggle = false;
byte idle_sprite = 0;
byte timer = 0;
unsigned long starttime = 0;

bool sfx_diamond = false;
bool sfx_rock = false;
bool sfx_step = false;
bool gfx_inverse = false;

bool digger_in_idle = true;
byte zufall_x = 0;
byte zufall_y = 0;
byte idle = 0;

//Statuszeile
bool clr_flag = true;
ColorIndex zfg, zbg;        //foreground- and backgroundcolor
String zs = String(score);  //Score
bool zs_flag = true;
String zc = String(countdown);  //Counter
String zd = String(diamonds);   //Diamanten ist/soll
bool zd_flag = true;
String zr = String(curlevel + 1);  //Raum
bool zr_flag = true;
String zl = String(lives);  //Leben
bool zl_flag = true;
String hs = String(score);

//16 colors index palette
const Color PALETTE_GAME[] = {
  //     RGB565   idx name           customized
  (Color)0x0000,  //0 INDEX_BLACK
  (Color)0x0210,  //1 INDEX_DARKBLUE
  (Color)0x0011,  //2 INDEX_PURPLE   gb.gui.menu-Hintergrund? KCB_BLUE
  (Color)0x9338,  //3 INDEX_GREEN    --> "Violett" für Steine
  (Color)0xCC68,  //4 INDEX_BROWN    gb.gui.menu-Akzent?
  (Color)0x6078,  //5 INDEX_DARKGRAY --> "dunkles Violett" für Steine
  (Color)0xac98,  //6 INDEX_GRAY     --> "helles Violett" für Steine
  (Color)0xFFFF,  //7 INDEX_WHITE    gb.gui.menu-Vordergrund?
  (Color)0xD8E4,  //8 INDEX_RED
  (Color)0xFD42,  //9 INDEX_ORANGE
  (Color)0xF720,  //a INDEX_YELLOW     --> Highscoreschrift
  (Color)0x8668,  //b INDEX_LIGHTGREEN --> Highscorehintergrund
  (Color)0x7DDF,  //c INDEX_LIGHTBLUE
  (Color)0x4439,  //d INDEX_BLUE
  (Color)0xCA30,  //e INDEX_PINK
  (Color)0xFEB2   //f INDEX_BEIGE
};
const Color PALETTE_MENU_EASY[] = {
  //     RGB565   idx name           customized
  (Color)0x4439,  //0 INDEX_BLACK
  (Color)0x4439,  //1 INDEX_DARKBLUE
  (Color)0x4439,  //2 INDEX_PURPLE   gb.gui.menu-Hintergrund? KCB_BLUE
  (Color)0x4439,  //3 INDEX_GREEN    --> "Violett" für Steine
  (Color)0x4439,  //4 INDEX_BROWN    gb.gui.menu-Akzent?
  (Color)0x4439,  //5 INDEX_DARKGRAY --> "dunkles Violett" für Steine
  (Color)0x4439,  //6 INDEX_GRAY     --> "helles Violett" für Steine
  (Color)0xFFFF,  //7 INDEX_WHITE    gb.gui.menu-Vordergrund?
  (Color)0x4439,  //8 INDEX_RED
  (Color)0xFD42,  //9 INDEX_ORANGE
  (Color)0x4439,  //a INDEX_YELLOW     --> Highscoreschrift
  (Color)0x4439,  //b INDEX_LIGHTGREEN --> Highscorehintergrund
  (Color)0x4439,  //c INDEX_LIGHTBLUE
  (Color)0x4439,  //d INDEX_BLUE
  (Color)0x4439,  //e INDEX_PINK
  (Color)0x4439   //f INDEX_BEIGE
};
const Color PALETTE_MENU_HARD[] = {
  //     RGB565   idx name           customized
  (Color)0xD8E4,  //0 INDEX_BLACK
  (Color)0xD8E4,  //1 INDEX_DARKBLUE
  (Color)0xD8E4,  //2 INDEX_PURPLE   gb.gui.menu-Hintergrund? KCB_BLUE
  (Color)0xD8E4,  //3 INDEX_GREEN    --> "Violett" für Steine
  (Color)0xD8E4,  //4 INDEX_BROWN    gb.gui.menu-Akzent?
  (Color)0xD8E4,  //5 INDEX_DARKGRAY --> "dunkles Violett" für Steine
  (Color)0xD8E4,  //6 INDEX_GRAY     --> "helles Violett" für Steine
  (Color)0xFFFF,  //7 INDEX_WHITE    gb.gui.menu-Vordergrund?
  (Color)0xD8E4,  //8 INDEX_RED
  (Color)0xFD42,  //9 INDEX_ORANGE
  (Color)0xD8E4,  //a INDEX_YELLOW     --> Highscoreschrift
  (Color)0xD8E4,  //b INDEX_LIGHTGREEN --> Highscorehintergrund
  (Color)0xD8E4,  //c INDEX_LIGHTBLUE
  (Color)0xD8E4,  //d INDEX_BLUE
  (Color)0xD8E4,  //e INDEX_PINK
  (Color)0xD8E4   //f INDEX_BEIGE
};

void setup() {
  gb.begin();
  gb.setFrameRate(36);
  Fill_TonBuffer(TON_LOW, TON_HIGH, TON_RATE);
  gamemode = gb.save.get(0);
  if (gamemode == 0)
    gamemode = HARD;
  initScore();
}

void loop() {
  while (!gb.update())
    ;  //update the gb.display

  switch (gamestate) {
    case PAUSED:
      if (gamemode == EASY) gb.display.setPalette(PALETTE_MENU_EASY);
      else gb.display.setPalette(PALETTE_MENU_HARD);
      gb.lights.clear();
      switch (gb.gui.menu(lang_title, (gamemode == EASY) ? Pausemenu_easy : Pausemenu_hard, MENULENGTH)) {

        case 0:              //resume game
          if (lives != 0) {  //resume from pause
            initStatus();
            gamestate = RUNNING;
          } else {  //load last game stand
            oldGame();
            initWorld(curlevel);
            initStatus();
            gamestate = RUNNING;
          }
          break;

        case 1:  //new game
          newGame();
          initWorld(curlevel);
          initStatus();
          gamestate = RUNNING;
          break;

        case 2:  //lvlup
          if (lives != 0) {
            if (curlevel < NUM_LEVELS) {
              curlevel++;
              initWorld(curlevel);
              dead = false;
            }
            initStatus();
            gamestate = RUNNING;
          }
          break;

        case 3:  //lvldown
          if (lives != 0) {
            if (curlevel > 0) {
              curlevel--;
              initWorld(curlevel);
              dead = false;
            }
            initStatus();
            gamestate = RUNNING;
          }
          break;

        case 4:  //gamemode
          if (gamemode == EASY) gamemode = HARD;
          else gamemode = EASY;
          SerialUSB.printf("switch to %s\n", (gamemode == EASY) ? "EASY" : "HARD");

          curlevel = (gamemode == EASY) ? cureasy : curhard;
          lives = (gamemode == EASY) ? liveseasy : liveshard;
          score = (gamemode == EASY) ? scoreeasy : scorehard;

          initWorld(curlevel);
          dead = false;
          break;

        case 5:  //highscore
          loadScore();
          gamestate = SCORE;
          break;

        default:  //nothing selected
          break;
      };
      break;

    case SCORE:
      gb.display.setPalette(PALETTE_GAME);
      if (gb.buttons.released(BUTTON_B))
        gamestate = PAUSED;
      viewScore();
      if (gb.frameCount % 10)
        gb.lights.drawImage(0, 0, ledsprite_highscore);
      break;

    case RUNNING:
      gb.display.setPalette(PALETTE_GAME);
      //pause the game if menu is pressed
      if (gb.buttons.pressed(BUTTON_B))
        gamestate = PAUSED;

      //GAMELOOP
      updateCursor();   //Player  ( digger action )
      updatePhysics();  //Physics ( falling() rocks & diamonds, moveMonster() )
      drawWorld();      //Render  ( scrolling screen, draw tiles )

      if (lives < 1) {
        saveScore();
        gamestate = SCORE;
      }

      //for debugging: show some stats
      if (gb.frameCount % 10 == 0) {
        SerialUSB.printf("CPU load: %i %% \n", gb.getCpuLoad());
        //SerialUSB.printf( "RAM free: %i bytes \n", gb.getFreeRam() );
      }
      break;
  }
}

void oldGame() {
  loadGame();
  dead = false;
}

void newGame() {
  score = 0;
  curlevel = 0;
  if (gamemode == EASY) lives = LIVESEASY;
  else lives = LIVES;
  dead = false;
}

void initStatus() {
  zr_flag = true;
  zl_flag = true;
  zd_flag = true;
  zs_flag = true;
  clr_flag = true;
}

//decoder of compressed level data
void initWorld(byte _level) {
  gb.lights.clear();
  zfg = (gamemode == EASY) ? INDEX_BLUE : INDEX_RED;
  zbg = (gamemode == EASY) ? INDEX_BLACK : INDEX_BLACK;
  countdown = 5000;
  countdown_step = false;
  countdown_toggle = false;
  diamonds = 0;
  codepos = 0;
  worldpos = 0;
  diams_need = getBits(_level, 8);
  while (worldpos < WORLD_H * WORLD_W) {
    if (getBits(_level, 1)) {
      byte r = getBits(_level, BITREF);
      byte c = getBits(_level, BITCOUNT) + MINMATCH;

      for (byte i = 0; i < c; i++) {
        if (worldpos < WORLD_W * WORLD_H) {
          int refpos = worldpos - r;
          setTile(world[refpos % WORLD_W][refpos / WORLD_W]);
          worldpos++;
        }
      }
    } else {
      setTile(getBits(_level, CODELENGTH));
      worldpos++;
    }
  }
  saveGame();
}

//helper for decompressor, returns number of bits of an compressed level starting at (bit-) position "codepos"
byte getBits(byte _level, byte bits) {
  //uses global var codepos for bitoffset
  const byte* pos = levels[_level] + codepos / 8;
  int w = pgm_read_byte(pos) * 256 + pgm_read_byte(pos + 1);
  w >>= (16 - (codepos % 8) - bits);  //shift bits to right
  w = (w & ((1 << bits) - 1));        //mask desired bits
  codepos += bits;
  return (byte)w;
}

// set Tile to worldmap. Note: codes in level data have slightly different meanings than in world map.
void setTile(byte sprite) {
  byte x = worldpos % WORLD_W;
  byte y = worldpos / WORLD_W;
  switch (sprite) {
    case MONSTER_RT: world[x][y] = MONSTER_R | TYPE_RT | RENDERSTATE; break;
    case MONSTER_LT: world[x][y] = MONSTER_L | TYPE_LT | RENDERSTATE; break;
    case MONSTER_D: world[x][y] = MONSTER_D | TYPE_UD | RENDERSTATE; break;
    case MONSTER_U: world[x][y] = MONSTER_U | TYPE_UD | RENDERSTATE; break;
    case MONSTER_L: world[x][y] = MONSTER_L | TYPE_LR | RENDERSTATE; break;
    case MONSTER_R: world[x][y] = MONSTER_R | TYPE_LR | RENDERSTATE; break;
    default: world[x][y] = sprite | RENDERSTATE; break;
  }
  if (sprite == PLAYER) {
    cursor_x = x;
    cursor_y = y;
  }
}

void nextLevel() {
  if (curlevel < NUM_LEVELS)
    curlevel++;
  initStatus();
  initWorld(curlevel);
}

void viewScore() {
  gb.display.fill(INDEX_LIGHTGREEN);
  gb.display.setColor(INDEX_YELLOW);
  gb.display.setCursor(36, 0);
  gb.display.print((gamemode == EASY) ? lang_score_easy : lang_score_hard);
  gb.display.setCursor(4, 6);
  gb.display.print(lang_highscore);
  gb.display.fillRect(4, 12, 44, 3);
  for (byte i = 0; i < 7; i++) {
    gb.display.setCursor(8, 18 + (i * 6));
    //score
    hs = String(highscore[i]);
    while (hs.length() < 5)
      hs = String("0" + hs);
    gb.display.print(hs);
    //space
    gb.display.print("  ");
    //name
    gb.display.print(scorename[i]);
  }
}

void loadScore() {
  byte offset;
  if (gamemode == EASY)
    offset = 7;  //blocks 7-20 hard highscore
  else
    offset = 21;  //blocks 21-34 easy highscore
  for (byte i = 0; i < 7; i++) {
    highscore[i] = gb.save.get(i + offset);
    gb.save.get(i + offset + 7, scorename[i], 9 + 1);
  }
}

void saveScore() {
  //prüfe Zeile 0-6
  for (byte j = 0; j < 7; j++) {
    //ist der Highscore größer als die aktuelle Zeile?
    if (score > highscore[j]) {

      //Zeilen unterhalb, nach unten kopieren
      for (byte z = 5; z >= j && z < 6; z--) {
        highscore[z + 1] = highscore[z];
        strcpy(scorename[z + 1], scorename[z]);
      }

      //Eingabe neuen scorename
      char defaultUser[12 + 1];
      gb.getDefaultName(defaultUser);  //hole 12stelligen Benutzernamen aus loader.bin
      for (byte i = 0; i < 9; i++) {   //einkürzen auf 9stellig
        scorename[j][i] = defaultUser[i];
      }
      gb.gui.keyboard(lang_name, scorename[j], 9);

      //Speicher neuen Highscore
      highscore[j] = score;
      score = 0;
      saveGame();
      byte offset;
      if (gamemode == EASY)
        offset = 7;  //blocks 7-20 hard highscore
      else
        offset = 21;  //blocks 21-34 easy highscore
      for (byte i = 0; i < 7; i++) {
        gb.save.set(i + offset, highscore[i]);
        gb.save.set(i + offset + 7, scorename[i]);
      }
    }
  }
}

void initScore() {
  byte offset;
  for (offset = 7; offset < 22; offset += 14) {  //offset=7=EASY offset=21=HARD
    char initname[7][9 + 1] = {
      "---------",
      "Digger   ",
      "(c) 1988 ",
      "by A.Lang",
      "---------",
      "         ",
      "         ",
    };
    for (byte i = 0; i < 7; i++) {
      highscore[i] = gb.save.get(i + offset);
      gb.save.get(i + offset + 7, scorename[i], 9 + 1);
      if ((highscore[i] == 0) && (String(scorename[i]) == ""))
        gb.save.set(i + offset + 7, initname[i]);
    }
  }
}

void loadGame() {
  gamemode = gb.save.get(0);
  cureasy = gb.save.get(1);
  curhard = gb.save.get(2);
  liveseasy = gb.save.get(3);
  liveshard = gb.save.get(4);
  scoreeasy = gb.save.get(5);
  scorehard = gb.save.get(6);

  if (gamemode == 0)
    gamemode = HARD;

  if (liveseasy == 0)
    liveseasy = LIVESEASY;
  if (liveshard == 0)
    liveshard = LIVES;

  curlevel = (gamemode == EASY) ? cureasy : curhard;
  lives = (gamemode == EASY) ? liveseasy : liveshard;
  score = (gamemode == EASY) ? scoreeasy : scorehard;

  SerialUSB.printf("LOADED gamemode:%s\n", (gamemode == EASY) ? "EASY" : "HARD");
  SerialUSB.printf("       cureasy:%i curhard:%i\n", cureasy, curhard);
  SerialUSB.printf("       liveseasy:%i liveshard:%i\n", liveseasy, liveshard);
  SerialUSB.printf("       scoreeasy:%i scorehard:%i\n", scoreeasy, scorehard);
  SerialUSB.printf("       lives:%i\n", lives);
}

void saveGame() {
  (gamemode == EASY) ? cureasy = curlevel : curhard = curlevel;
  (gamemode == EASY) ? liveseasy = lives : liveshard = lives;
  (gamemode == EASY) ? scoreeasy = score : scorehard = score;
  gb.save.set(0, gamemode);
  gb.save.set(1, cureasy);
  gb.save.set(2, curhard);
  gb.save.set(3, liveseasy);
  gb.save.set(4, liveshard);
  gb.save.set(5, scoreeasy);
  gb.save.set(6, scorehard);

  SerialUSB.printf("SAVED  gamemode:%s\n", (gamemode == EASY) ? "EASY" : "HARD");
  SerialUSB.printf("       cureasy:%i curhard:%i\n", cureasy, curhard);
  SerialUSB.printf("       liveseasy:%i liveshard:%i\n", liveseasy, liveshard);
  SerialUSB.printf("       scoreeasy:%i scorehard:%i\n", scoreeasy, scorehard);
  SerialUSB.printf("       lives:%i\n", lives);
}
