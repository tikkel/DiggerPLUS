byte diamond_step = 8;

void drawWorld() {
  byte crop_y = 0;
  //diamond-animation
  if (diamond_step > 7) {
    diamond_step = 0;
  } else {
    diamond_step++;
  }
  //SerialUSB.printf( "diamant_animation: %i\n", DIAMOND1 + diamond_step/3 );

  for (byte y = 0; y < WORLD_H; y++) {
    int y_screen = y * SPRITE_H - camera_y;  //coordinates on the screen depending on the camera position

    for (byte x = 0; x < WORLD_W; x++) {
      int x_screen = x * SPRITE_W - camera_x;  //coordinates on the screen depending on the camera position

      byte sprite = world[x][y];
      sprite &= SPRITEMASK;

      idle = world[zufall_x][zufall_y];

      //diamond-animation
      if (sprite == DIAMOND) {
        sprite = DIAMOND1 + diamond_step / 3;
        world[x][y] = DIAMOND | RENDERSTATE;
      }

      //digger-animation
      if (sprite == PLAYER) {
        world[x][y] = PLAYER | RENDERSTATE;
        switch (dir) {
          case DIGGER_L:
            digger_in_idle = false;
            sprite = 16 + digger_step / 3;
            digger_step++;
            if (digger_step > 11)
              digger_step = 0;
            break;
          case DIGGER_U:
            digger_in_idle = false;
            sprite = 20 + digger_step / 3;
            digger_step++;
            if (digger_step > 5)
              digger_step = 0;
            break;
          case DIGGER_I:
            zufall_x++;
            if (zufall_x > WORLD_W) {
              zufall_x = 0;
              zufall_y++;
              if (zufall_y > WORLD_H) {
                zufall_y = 0;
                zufall_x = 0;
              }
            }
            if ((!digger_in_idle) && (idle == ROCK)) {
              //ROCK --> blinzeln
              digger_in_idle = true;
              idle_sprite = 29;
            } else if ((!digger_in_idle) && (idle == DIAMOND)) {
              //DIAMOND --> stampfen
              digger_in_idle = true;
              idle_sprite = 30;
            } else if (digger_in_idle) {
              //ein paar takte warten und wieder freigeben
              sprite = idle_sprite;
              idle_step++;
              if (idle_step > 11) {
                idle_step = 0;
                digger_in_idle = false;
                sprite = 22;
                if (idle_sprite == 30)
                  sfx_step = true;
              }
            } else {
              sprite = 22;
            }
            break;
          case DIGGER_D:
            digger_in_idle = false;
            sprite = 23 + digger_step / 3;
            digger_step++;
            if (digger_step > 5)
              digger_step = 0;
            break;
          case DIGGER_R:
            digger_in_idle = false;
            sprite = 25 + digger_step / 3;
            digger_step++;
            if (digger_step > 11)
              digger_step = 0;
            break;
        }
      }

      // don't draw sprites which are out of the screen
      if (!(x_screen < -SPRITE_W - 1 || x_screen > display_width - 1 || y_screen < -TOPBAR_H || y_screen > display_height - 1)) {
        //if ( !(x_screen < -SPRITE_W-1 || x_screen > display_width-1 || y_screen < 1 || y_screen > display_height-1) && (world[x][y] & RENDERSTATE)==RENDERSTATE ) {
        spritesheet.setFrame(sprite);
        //crop menubar
        if (y_screen < TOPBAR_H)
          crop_y = TOPBAR_H - y_screen;
        else
          crop_y = 0;
        gb.display.drawImage(x_screen, y_screen + crop_y, spritesheet, 0, crop_y, SPRITE_W, SPRITE_H);
      }

      world[x][y] &= ~RENDERSTATE;
    }
  }

  // LED_FX and sound_FX
  if (gb.frameCount % 4 == 0) {
    if (sfx_diamond) {
      gb.sound.play(ton_buffer[TON_DIAMANT], ton_laenge[TON_DIAMANT]);
      gb.lights.drawImage(0, 0, ledsprite_collect);
    } else if (sfx_rock) {
      gb.sound.play(ton_buffer[TON_STEIN], ton_laenge[TON_STEIN]);
      gb.lights.drawImage(0, 0, ledsprite_falled);
    } else if (sfx_step)
      gb.sound.play(ton_buffer[TON_SCHRITT], ton_laenge[TON_SCHRITT]);
    else if (dead)
      gb.lights.drawImage(0, 0, ledsprite_highscore);
    sfx_diamond = false;
    sfx_rock = false;
    sfx_step = false;
  }

  // nach losgelassener Richtungstaste, wieder auf wartend (idle) zurückstellen (timer default: nach 2 frames)
  if (timer == 0)
    dir = DIGGER_I;
  timer--;

  // S T A T U S Z E I L E :_rr_ll*_cccc_dd*00_
  if (gb.frameCount % 4 == 0) {
    // clear
    if (clr_flag) {
      clr_flag = false;
      gb.display.fill(zbg);
    }

    gb.display.setTextWrap(false);
    gb.display.cursorY = 0;

    // raum
    if (zr_flag) {
      zr_flag = false;
      gb.display.cursorX = 0;
      gb.display.setColor(zfg, zbg);
      zr = String(curlevel + 1);
      while (zr.length() < 2)
        zr = String("0" + zr);
      gb.display.print(zr);
    }

    // leben
    if (zl_flag) {
      zl_flag = false;
      gb.display.cursorX = 10;
      gb.display.setColor(zfg, zbg);
      zl = String(lives);
      while (zl.length() < 2)
        zl = String("0" + zl);
      gb.display.print(zl);
      gb.display.cursorX = 17;
      gb.display.setColor(INDEX_ORANGE, zbg);  //oranges
      gb.display.print(F("\03"));              //herz
    }

    // countdown
    gb.display.cursorX = 23;
    gb.display.setColor(zfg, zbg);
    zc = String(countdown);
    while (zc.length() < 4)
      zc = String("0" + zc);
    if (countdown == 0)
      countdown_toggle = false;
    else if ((gb.frameCount % 6 == 0) && (countdown < 1000))
      countdown_toggle ? countdown_toggle = false : countdown_toggle = true;
    countdown_toggle ? gb.display.print("    ") : gb.display.print(zc);

    // diamanten
    if (zd_flag) {
      zd_flag = false;
      gb.display.cursorX = 41;
      zd = String(diamonds);
      while (zd.length() < 2)
        zd = String("0" + zd);
      gb.display.print(zd);
      gb.display.cursorX = 48;
      gb.display.setColor(INDEX_YELLOW, zbg);  //gelber
      gb.display.print(F("\04"));              //diamant
      gb.display.cursorX = 51;
      gb.display.setColor(zfg, zbg);
      gb.display.print(diams_need);
    }

    // score
    if (zs_flag) {
      zs_flag = false;
      gb.display.cursorX = 61;
      zs = String(score);
      while (zs.length() < 5)
        zs = String("0" + zs);
      gb.display.print(zs);
    }
  }
}
