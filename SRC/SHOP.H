#define OTHER_STUFF 2
void draw_wares( int row, int colu )
{
    int a;
    for ( a = 0; a < DIFF_WEAPONS; a ++  )
    {
        draw_ware( 9 + a*18, 130, 0, a );
        if ( row == 0 ) if ( colu == a )
        rectangle( 8 + a*18, 129, 8 + 17 + a*18, 129 + 17, 98 );
    }
    for ( a = 0; a < DIFF_BULLETS; a ++  )
    {
        draw_ware( 9 + a*18, 130 + 18, 1, a );
        if ( row == 1 ) if ( colu == a )
        rectangle( 8 + a*18, 129 + 18, 8 + 17 + a*18, 129 + 17 + 18, 98 );
    }
    for ( a = 0; a < OTHER_STUFF; a ++  )
    {
        draw_ware( 9 + a*18, 130 + 18 + 18, 2, a );
        if ( row == 2 ) if ( colu == a )
        rectangle( 8 + a*18, 129 + 18 + 18, 8 + 17 + a*18, 129 + 17 + 18 + 18, 98 );
    }
}

void draw_ware_info( Player *P, int sel1, int sel2 )
{
    char text[20];
    int a;
    FONT_NUM = 1;
    if ( sel1 == 0 )
    {
        writefonts( 20, 50, weapon[sel2 + 1].name, M_YELLOW );
        writefonts( 20, 70, "Cost:", M_BLUE );
        sprintf( text, "%d", weapon[sel2 + 1].cost );
        writefonts( 20 + str_length( "Cost: " ), 70, text, M_YELLOW );
        writefonts( 20, 80, "Sell price:", M_BLUE );
        sprintf( text, "%d", sell_price[sel2] );
        writefonts( 20 + str_length( "Sell_price: " ), 80, text, M_YELLOW );
        writefonts( 20, 90, "Bullet type:", M_BLUE );
        writefonts( 20 + str_length( "Bullet type: " ), 90, bullet_type[weapon[sel2 + 1].btype].name, M_YELLOW );
    }
    if ( sel1 == 1 )
    {
        writefonts( 20, 50, bullet_type[sel2 + 1].name, M_YELLOW );
        writefonts( 20, 70, "Cost:", M_BLUE );
        sprintf( text, "%d", bullet_type[sel2 + 1].cost );
        writefonts( 20 + str_length( "Cost: " ), 70, text, M_YELLOW );
    }
    if ( sel1 == 2 )
    {
        if ( sel2 == 0 )
        {
            writefonts( 20, 50, "Shield", M_YELLOW );
            writefonts( 20, 70, "Cost:", M_BLUE );
            sprintf( text, "%d", shield_data.cost + P-> shield*15 );
            writefonts( 20 + str_length( "Cost: " ), 70, text, M_YELLOW );
            writefonts( 20, 80, "Sell price:", M_BLUE );
            a = P-> shield - 1;if ( a < 0 ) a = 0;
            sprintf( text, "%d", sell_price[DIFF_WEAPONS] + ( ( a*15 )  >> 1 ) );
            writefonts( 20 + str_length( "Sell_price: " ), 80, text, M_YELLOW );
        }
        if ( sel2 == 1 )
        {
            writefonts( 20, 50, "Target system", M_YELLOW );
            writefonts( 20, 70, "Cost:", M_BLUE );
            sprintf( text, "%d", target_cost );
            writefonts( 20 + str_length( "Cost: " ), 70, text, M_YELLOW );
            writefonts( 20, 80, "Sell price:", M_BLUE );
            sprintf( text, "%d", sell_price[DIFF_WEAPONS + 1] );
            writefonts( 20 + str_length( "Sell_price: " ), 80, text, M_YELLOW );
        }
    }
}

void Player::shop()
{
    int a, sx, sel1 = 0, sel2 = 0, quit = 0, x;
    char text[20];
    load_efp( "EFPS/SHOP.EFP", picture, 0 );
    for ( a = 0; a < 17; a ++  )
    {
        draw_box1( 9 + a*18, 130, 25 + a*18, 130 + 16, 8 );
        draw_box1( 9 + a*18, 130 + 18, 25 + a*18, 130 + 16 + 18, 8 );
        draw_box1( 9 + a*18, 130 + 18 + 18, 25 + a*18, 130 + 16 + 18 + 18, 8 );
    }
    draw_box1( 115, 7, 205, 27, 13 );
    draw_box1( 10, 10, 80, 33, 13 );
    draw_box1( 10, 40, 230, 113, 13 );
    FONT_NUM = 0;
    sx = 160 - ( str_length( "the shop" )  / 2 );
    first = 1;
    shield_data.max = 30;
    while ( !quit )
    {
        memcpy( virbuff, picture, 64000 );
        FONT_NUM = 0;
        writefonts2( sx, 10, "the shop", 1 );
        FONT_NUM = 1;
        writefonts( 10, 190, "Press FIRE to buy, CHANGE to sell, ESC to exit", M_BLUE );
        writefonts( 15, 12, name, M_YELLOW );
        sprintf( text, "%d", cash );
        writefonts( 15, 20, "Cash:", M_RED );
        writefonts( 40, 20, text, M_YELLOW );
        draw_wares( sel1, sel2 );
        draw_ware_info( this, sel1, sel2 );
        FONT_NUM = 3;
        for ( a = 0; a < DIFF_WEAPONS; a ++  )
        if ( weapons[a + 1] == 1 )
        {
            writefonts( 8 + a*18, 131 + 11, "1", 0 );
            writefonts( 9 + a*18, 130 + 11, "1", M_YELLOW );
        }
        for ( a = 0; a < DIFF_BULLETS; a ++  )
        if ( bullets[a] > 0 )
        {
            x = bullets[a] / bullet_type[a + 1].mul;
            if ( x < 1 ) x = 1;
            sprintf( text, "%d", x );
            writefonts( 8 + a*18, 131 + 11 + 18, text, 0 );
            writefonts( 9 + a*18, 130 + 11 + 18, text, M_YELLOW );
        }
        if ( shield > 0 )
        {
            sprintf( text, "%d", shield );
            writefonts( 8, 131 + 11 + 18 + 18, text, 0 );
            writefonts( 9, 130 + 11 + 18 + 18, text, M_YELLOW );
        }
        if ( TARGET == 1 )
        {
            writefonts( 8 + 18, 131 + 11 + 18 + 18, "1", 0 );
            writefonts( 9 + 18, 130 + 11 + 18 + 18, "1", M_YELLOW );
        }
        if ( !first ) memcpy( screen, virbuff, 64000 );
        if ( first == 1 )
        {
            fadein( virbuff, pal );
            first = 0;
        }
        while ( !i::state(k::ESC)&&
        !i::state(K_LEFT)&&
        !i::state(K_RIGHT)&&
        !i::state(K_UP)&&
        !i::state(K_DOWN)&&
        !i::state(K_SHOOT)&&
        !i::state(K_SHIFT))
        {
            tk_port::event_tick();
        }
        if ( i::state(K_UP) ) if ( sel1 > 0 ) sel1 --;
        if ( i::state(K_DOWN) ) if ( sel1 < 2 ) sel1 ++;
        if ( i::state(K_LEFT) ) sel2 --;
        if ( i::state(K_RIGHT) ) sel2 ++;
        if ( sel2 < 0 ) sel2 = 0;
        if ( sel1 == 0 ) if ( sel2 >= DIFF_WEAPONS ) sel2 = DIFF_WEAPONS - 1;
        if ( sel1 == 1 ) if ( sel2 >= DIFF_BULLETS ) sel2 = DIFF_BULLETS - 1;
        if ( sel1 == 2 ) if ( sel2 >= OTHER_STUFF ) sel2 = OTHER_STUFF - 1;
        if ( i::state(K_SHOOT) )
        {
            if ( sel1 == 0 )
            if ( weapons[sel2 + 1] == 0 )
            if ( cash >= weapon[sel2 + 1].cost )
            {
                weapons[sel2 + 1] = 1;
                cash -= weapon[sel2 + 1].cost;
            }
            if ( sel1 == 1 )
            if ( bullets[sel2] < bullet_type[sel2 + 1].max*bullet_type[sel2 + 1].mul )
            if ( cash >= bullet_type[sel2 + 1].cost )
            {
                bullets[sel2] += bullet_type[sel2 + 1].mul;
                cash -= bullet_type[sel2 + 1].cost;
            }
            if ( sel1 == 2 )
            {
                if ( sel2 == 0 )
                if ( cash >= shield_data.cost + shield*15 )
                if ( shield < shield_data.max )
                {
                    cash -= shield_data.cost + shield*15;
                    shield ++;
                }
                if ( sel2 == 1 )
                if ( TARGET == 0 )
                if ( cash >= target_cost )
                {
                    TARGET = 1;
                    cash -= target_cost;
                }
            }
        }
        if ( i::state(K_SHIFT) )
        {
            if ( sel1 == 0 )
            if ( weapons[sel2 + 1] > 0 )
            {
                weapons[sel2 + 1] = 0;
                cash += sell_price[sel2];
            }
            if ( sel1 == 1 )
            if ( bullets[sel2] > 0 )
            {
                bullets[sel2] -= bullet_type[sel2 + 1].mul;
                if ( bullets[sel2] < 0 ) bullets[sel2] = 0;
                cash += bullet_type[sel2 + 1].cost;
            }
            if ( sel1 == 2 )
            {
                if ( sel2 == 0 )
                if ( shield > 0 )
                {
                    a = shield - 1; if ( a < 0 ) a = 0;
                    cash += sell_price[DIFF_WEAPONS] + ( ( 15*a )  / 2 );
                    shield --;
                }
                if ( sel2 == 1 )
                if ( TARGET == 1 )
                {
                    TARGET = 0;
                    cash += sell_price[DIFF_WEAPONS + 1];
                }
            }
        }
        i::clear(K_UP);
        i::clear(K_DOWN);
        i::clear(K_LEFT);
        i::clear(K_RIGHT);
        i::clear(K_SHOOT);
        i::clear(K_SHIFT);
        MIDASplaySample( samplep[KLIKWAV], MIDAS_CHANNEL_AUTO, 0, 22000, EFFECT_VOLUME, MIDAS_PAN_MIDDLE );
        if ( i::state(k::ESC) )
        {
            FONT_NUM = 0;
            writefonts2( 10, 115, "press fire to confirm", 1 );
            memcpy( screen, virbuff, 64000 );
            i::clear_stack();
            if ( i::wait_for_keypress() == K_SHOOT  ) {
                quit = 1;
            }
            i::clear(k::ESC);
        }
    }
    fadeout( virbuff, pal );
}
