/*
 * Copyright (C) 2013-2015 Snailsoft <https://github.com/snailsoft/starborn/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

namespace ss
{
    namespace constants
    {
        #define ACTION_DOWN                         "down"
        #define ACTION_ESCAPE                       "escape"
        #define ACTION_EXIT                         "exit"
        #define ACTION_LEFT                         "left"
        #define ACTION_RIGHT                        "right"
        #define ACTION_SCREENSHOT                   "screenshot"
        #define ACTION_SELECT                       "select"
        #define ACTION_UP                           "up"

        #define ANCHOR_BOTTOM                       "bottom"
        #define ANCHOR_BOTTOM_LEFT                  "bottom_left"
        #define ANCHOR_BOTTOM_RIGHT                 "bottom_right"
        #define ANCHOR_CENTER                       "center"
        #define ANCHOR_CENTER_BOTTOM                "center_bottom"
        #define ANCHOR_CENTER_LEFT                  "center_left"
        #define ANCHOR_CENTER_RIGHT                 "center_right"
        #define ANCHOR_CENTER_TOP                   "center_top"
        #define ANCHOR_LEFT                         "left"
        #define ANCHOR_RIGHT                        "right"
        #define ANCHOR_TOP                          "top"
        #define ANCHOR_TOP_LEFT                     "top_left"
        #define ANCHOR_TOP_RIGHT                    "top_right"

        #define ANIMATED_DRAWABLE_PREFIX            "animated_"

        #define ANIMATION_FADE                      "fade"
        #define ANIMATION_FADE_IN                   "fade_in"
        #define ANIMATION_FADE_OUT                  "fade_out"

        #define ANIMATION_TYPE_DEFAULT              "default"
        #define ANIMATION_TYPE_FADE                 "fade"

        #define BUTTON_PREFIX                       "button_"
        #define BUTTON_CONTINUE                     BUTTON_PREFIX "continue"
        #define BUTTON_EXIT                         BUTTON_PREFIX "exit"
        #define BUTTON_MIDNIGHT                     BUTTON_PREFIX "midnight"
        #define BUTTON_NEW_GAME                     BUTTON_PREFIX "new_game"
        #define BUTTON_NIGHTFALL                    BUTTON_PREFIX "nightfall"

        #define DRAWABLE_TYPE_ANIMATED_RECTANGLE    ANIMATED_DRAWABLE_PREFIX "rectangle"
        #define DRAWABLE_TYPE_ANIMATED_SPRITE       ANIMATED_DRAWABLE_PREFIX "sprite"
        #define DRAWABLE_TYPE_ANIMATED_STRING       ANIMATED_DRAWABLE_PREFIX "string"
        #define DRAWABLE_TYPE_BACKGROUND            "background"
        #define DRAWABLE_TYPE_SPRITE                "sprite"

        #define MUSIC_PREFIX                        "music."
        #define MUSIC_CRYPT                         MUSIC_PREFIX "crypt"
        #define MUSIC_TITLE                         MUSIC_PREFIX "title"

        #define SETTING_ZOOM                        3

        #define SOUND_PREFIX                        "sound."
        #define SOUND_MENU_SCROLL                   SOUND_PREFIX "menu_scroll"
        #define SOUND_MENU_SELECT                   SOUND_PREFIX "menu_select"
        #define SOUND_SNAILSOFT                     SOUND_PREFIX "snailsoft"

        #define SPRITE_SNAILSOFT                    "snailsoft"
        #define SPRITE_STARBORN_HORIZONTAL          "starborn_horizontal"
        #define SPRITE_STARBORN_VERTICAL            "starborn_vertical"

        #define STATE_CLOSING                       "closing"
        #define STATE_LOADING                       "loading"
        #define STATE_LOAD_GAME                     "load_game"
        #define STATE_MAIN_MENU                     "main_menu"
        #define STATE_NEW_GAME                      "new_game"
        #define STATE_OPTIONS                       "options"
        #define STATE_RUNNING                       "running"
        #define STATE_SAVE_GAME                     "save_game"
        #define STATE_SNAILSOFT_LOGO                "snailsoft_logo"
        #define STATE_STARBORN_LOGO                 "starborn_logo"
    }
}
