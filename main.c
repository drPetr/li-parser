#include <stdio.h>
#include <time.h>

#include "li.h"

#define listr           liStr_t
#define li_salloc       LiStrAlloc
#define li_srealloc     LiStrRealloc
#define li_sfree        LiStrFree
#define li_screate      LiStrCreate
#define li_screate_cs   LiStrCreateCstr
#define li_sconcat      LiStrConcat
#define li_sconcat_cs   LiStrConcatCstr

#define li_ins_first_child  LiInsertFirstChild
#define li_ins_last_child   LiInsertLastChild
#define li_ins_before       LiInsertBefore
#define li_ins_after        LiInsertAfter
#define li_ins_first        LiInsertFirst
#define li_ins_last         LiInsertLast
#define li_ext              LiExtract
#define li_ext_siblings     LiExtractSiblings
#define li_ext_children     LiExtractChildren
#define li_ncreate          LiNodeCreate
#define li_free_subtree     LiFreeSubtree
#define li_free             LiFree

#define liobj               liObj_t
#define li_setkey           LiSetKeyStr
#define li_setkey_cs        LiSetKeyCstr
#define li_obj              LiObj
#define li_null             LiNull
#define li_s                LiStr
#define li_cs               LiCstr


#define li_write            LiWrite


liobj *mcom( const char *id, const char *exe ) {
    liobj *c, *o;
    
    c = li_obj();
    o = li_cs( id );
    li_setkey_cs( o, "id" );
    li_ins_last_child( c, o );
    o = li_cs( exe );
    li_setkey_cs( o, "exe" );
    li_ins_last_child( c, o );
    
    return c;
}

liobj *mcom0( const char *id, const char *exe ) {
    liobj *c, *o;
    
    c = li_obj();
    li_setkey_cs( c, "command" ); 
    o = li_cs( id );
    li_setkey_cs( o, "id" );
    li_ins_last_child( c, o );
    o = li_cs( exe );
    li_setkey_cs( o, "exe" );
    li_ins_last_child( c, o );
    
    return c;
}

liobj *MakeExample1( void ) {
    liobj *root = NULL;
    liobj *o, *li;
    
    root = li_obj();
    li_setkey_cs( root, "keybingidg" ); 
    
    o = li_null();
    li_setkey_cs( o, "a_null" );
    li_ins_last_child( root, o );
    
    li = li_cs( "Hello" );
    li_ins_last( o, li );
    li = li_cs( "My" );
    li_ins_last( o, li );
    li = li_cs( "Favorite" );
    li_ins_last( o, li );
    li = li_cs( "World!" );
    li_ins_last( o, li );
    
    o = li_obj();
    li_setkey_cs( o, "group" );
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_direction" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    
    li_ins_last_child( o, mcom0( "kb_left", "left" ) );
    li_ins_last_child( o, mcom( "kb_right", "right" ) );
    li_ins_last_child( o, mcom( "kb_up", "up" ) );
    li_ins_last_child( o, mcom( "kb_down", "down" ) );
    
    
    o = li_obj();
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_movement" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    
    
    li_ins_last_child( o, mcom0( "kb_forward", "forward" ) );
    li_ins_last_child( o, mcom( "kb_backward", "back" ) );
    li_ins_last_child( o, mcom( "kb_left_strafe", "lstrafe" ) );
    li_ins_last_child( o, mcom( "kb_right_strafe", "rstrafe" ) );
    li_ins_last_child( o, mcom( "kb_jump", "jump" ) );
    li_ins_last_child( o, mcom( "kb_crouch", "crouch" ) );
    li_ins_last_child( o, mcom( "kb_crouch_toggle", "crouch_toggle" ) );
    li_ins_last_child( o, mcom( "kb_always_run", "accel" ) );
    li_ins_last_child( o, mcom( "kb_sprint", "sprint_toggle" ) );
    li_ins_last_child( o, mcom( "kb_left_lookout", "llookout" ) );
    li_ins_last_child( o, mcom( "kb_right_lookout", "rlookout" ) );
    
    
    
    o = li_obj();
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_weapons" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    
    
    li_ins_last_child( o, mcom0( "kb_weapon1", "wpn_1" ) );
    li_ins_last_child( o, mcom( "kb_weapon2", "wpn_2" ) );
    li_ins_last_child( o, mcom( "kb_weapon3", "wpn_3" ) );
    li_ins_last_child( o, mcom( "kb_weapon4", "wpn_4" ) );
    li_ins_last_child( o, mcom( "kb_weapon5", "wpn_5" ) );
    li_ins_last_child( o, mcom( "kb_weapon6", "wpn_6" ) );
    li_ins_last_child( o, mcom( "kb_wpn_next", "wpn_next" ) );
    li_ins_last_child( o, mcom( "kb_next_slot", "next_slot" ) );
    li_ins_last_child( o, mcom( "kb_prev_slot", "prev_slot" ) );
    li_ins_last_child( o, mcom( "kb_fire", "wpn_fire" ) );
    li_ins_last_child( o, mcom( "kb_zoom", "wpn_zoom" ) );
    li_ins_last_child( o, mcom( "kb_reload", "wpn_reload" ) );
    li_ins_last_child( o, mcom( "kb_func", "wpn_func" ) );
    li_ins_last_child( o, mcom( "kb_firemode_next", "wpn_firemode_next" ) );
    li_ins_last_child( o, mcom( "kb_firemode_prev", "wpn_firemode_prev" ) );
    
    
    
    o = li_obj();
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_inventory" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    
    
    
    li_ins_last_child( o, mcom0( "kb_inventory", "inventory" ) );
    li_ins_last_child( o, mcom( "kb_active_jobs", "active_jobs" ) );
    li_ins_last_child( o, mcom( "kb_map", "map" ) );
    li_ins_last_child( o, mcom( "kb_contacts", "contacts" ) );
    li_ins_last_child( o, mcom( "kb_torch", "torch" ) );
    li_ins_last_child( o, mcom( "kb_night_vision", "night_vision" ) );
    li_ins_last_child( o, mcom( "kb_use_bandage", "use_bandage" ) );
    li_ins_last_child( o, mcom( "kb_use_medkit", "use_medkit" ) );
    li_ins_last_child( o, mcom( "kb_drop", "drop" ) );
    li_ins_last_child( o, mcom( "kb_turn_engine", "turn_engine" ) );
    
    o = li_obj();
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_common" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    
    
    
    li_ins_last_child( o, mcom0( "kb_pause", "pause" ) );
    li_ins_last_child( o, mcom( "kb_use", "use" ) );
    li_ins_last_child( o, mcom( "kb_screenshot", "screenshot" ) );
    li_ins_last_child( o, mcom( "kb_quit", "quit" ) );
    li_ins_last_child( o, mcom( "kb_console", "console" ) );
    li_ins_last_child( o, mcom( "ui_mm_save_game", "quick_save" ) );
    li_ins_last_child( o, mcom( "ui_mm_load_game", "quick_load" ) );
    li_ins_last_child( o, mcom( "kb_cam_1", "cam_1" ) );
    li_ins_last_child( o, mcom( "kb_cam_2", "cam_2" ) );
    li_ins_last_child( o, mcom( "kb_cam_3", "cam_3" ) );
    
    
    o = li_obj();
    li_ins_last_child( root, o );
    
    li = li_cs( "kb_grp_multiplayer" );
    li_setkey_cs( li, "name" );
    li_ins_last_child( o, li );
    

    li_ins_last_child( o, mcom0( "kb_artefact", "artefact" ) );
    li_ins_last_child( o, mcom( "kb_scores", "scores" ) );
    li_ins_last_child( o, mcom( "kb_chat", "chat" ) );
    li_ins_last_child( o, mcom( "kb_chat_team", "chat_team" ) );
    li_ins_last_child( o, mcom( "kb_buy_menu", "buy_menu" ) );
    li_ins_last_child( o, mcom( "kb_skin_menu", "skin_menu" ) );
    li_ins_last_child( o, mcom( "kb_team_menu", "team_menu" ) );
    li_ins_last_child( o, mcom( "kb_vote_begin", "vote_begin" ) );
    li_ins_last_child( o, mcom( "kb_vote_menu", "vote" ) );
    li_ins_last_child( o, mcom( "kb_vote_yes", "vote_yes" ) );
    li_ins_last_child( o, mcom( "kb_vote_no", "vote_no" ) );
    li_ins_last_child( o, mcom( "speech_menu_group", "speech_menu_0" ) );
    li_ins_last_child( o, mcom( "speech_menu_personal", "speech_menu_1" ) );
    
    return root;
}




int main( int argc, char **argv ) {
    clock_t start = clock();
    
    liobj   *li = MakeExample1();
    
    
    
    li_write( NULL, li, "out/out.li", 0 );
    
    li_free( li );
    
    clock_t stop = clock();
    double d = (double)(stop - start) / CLOCKS_PER_SEC;
    printf( "Loop required %f seconds", d );
    
    return 0;
}


