/*
 *  Copyright (C) OmicronLab (http://www.omicronlab.com)
 *
 *  Developer: M M Rifat-Un-Nabi, Mehdi Hasan Khan
 *
 *  Listen us on http://www.twitter.com/omicronlab
 *  Support Forum: http://www.omicronlab.com/forum/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.find_consonent_by_key
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#define Uses_SCIM_UTILITY
#define Uses_SCIM_IMENGINE
#define Uses_SCIM_LOOKUP_TABLE
#define Uses_SCIM_CONFIG_BASE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <scim.h>
#include "scim_avro_imengine_factory.h"
#include "scim_avro_imengine.h"
//#include "scim_avro_prefs.h"
#include "intl.h"

AvroInstance::AvroInstance(AvroFactory *factory, const String &encoding, int id)
    : IMEngineInstanceBase(factory, encoding, id),
      m_factory(factory),
      m_prev_key(0, 0) {
  SCIM_DEBUG_IMENGINE(1) << "Create AVRO Instance : ";

  m_lookup_table.set_page_size(1);
  m_lookup_table.fix_page_size(true);
  m_lookup_table.show_cursor(false);
}

AvroInstance::~AvroInstance() {
}

void AvroInstance::move_preedit_caret(unsigned int pos) {
}

void AvroInstance::select_candidate(unsigned int item) {
  SCIM_DEBUG_IMENGINE(2) << "select_candidate.\n";

  select_candidate_no_direct(item);
}

void AvroInstance::select_candidate_no_direct(unsigned int item) {
  SCIM_DEBUG_IMENGINE(2) << "select_candidate_no_direct.\n";

  m_lookup_table.set_cursor_pos_in_current_page(item);
  update_lookup_table(m_lookup_table);
}

void AvroInstance::update_lookup_table_page_size(unsigned int page_size) {
  SCIM_DEBUG_IMENGINE(2) << "update_lookup_table_page_size.\n";

  m_lookup_table.set_page_size(page_size);
}

void AvroInstance::lookup_table_page_up() {
  SCIM_DEBUG_IMENGINE(2) << "lookup_table_page_up.\n";

  m_lookup_table.page_up();
  update_lookup_table(m_lookup_table);
}

void AvroInstance::lookup_table_page_down() {
  SCIM_DEBUG_IMENGINE(2) << "lookup_table_page_down.\n";

  m_lookup_table.page_down();
  update_lookup_table(m_lookup_table);
}

void AvroInstance::reset() {
  SCIM_DEBUG_IMENGINE(2) << "reset.\n";

  HideWindow();
}

void AvroInstance::focus_in() {
  SCIM_DEBUG_IMENGINE(2) << "focus_in.\n";

  hide_aux_string();
}

void AvroInstance::focus_out() {
  SCIM_DEBUG_IMENGINE(2) << "focus_out.\n";

  ResetDeadKey();
}

void AvroInstance::trigger_property(const String &property) {
  String avro_prop = property.substr(property.find_last_of('/') + 1);

  SCIM_DEBUG_IMENGINE(2) << "trigger_property : " << property << " - " << avro_prop << "\n";
}

bool AvroInstance::process_key_event(const KeyEvent& key) {
  SCIM_DEBUG_IMENGINE(2) << "process_key_event.\n";

  if (key.is_control_down() || key.is_alt_down() || key.is_super_down()) {
    BlockLast = false;
    return false;
  }

  if ((key.code == SCIM_KEY_Shift_L) || (key.code == SCIM_KEY_Shift_R)) {
    BlockLast = false;
    return false;
  }

  if ((key.code == SCIM_KEY_space) || (key.code == SCIM_KEY_Tab) ||
      (key.code == SCIM_KEY_Return) || (key.code == SCIM_KEY_KP_Enter)) {
    ResetDeadKey();
    return false;
  }

  if (key.is_key_release()) {
    if ((key.code == SCIM_KEY_Control_L) || (key.code == SCIM_KEY_Control_R) ||
        (key.code == SCIM_KEY_Super_L) || (key.code == SCIM_KEY_Super_R)||
        (key.code == SCIM_KEY_Alt_L) || (key.code == SCIM_KEY_Alt_R)) {
      BlockLast = false;
      return false;
    }
    if (BlockLast == true) {
      return true;
    }
  } else {
    bool m_Block = false;

    MyProcessVKeyDown(key, m_Block);
    BlockLast = m_Block;
    return m_Block;
  }
}

void AvroInstance::ParseAndSend() {
  WideString NewBanglaText;
  int i = 0;
  int Matched = 0;
  int UnMatched = 0;

  NewBanglaText = Parser.Convert(EnglishT);

  if (PrevBanglaT == utf8_mbstowcs("")) {
    commit_string(NewBanglaText);
    PrevBanglaT = NewBanglaText;
  } else {
    for (i = 1; i <= PrevBanglaT.length(); i++) {
      if (PrevBanglaT.substr(i - 1, 1) == NewBanglaText.substr(i - 1, 1)) {
        Matched = Matched + 1;
      } else {
        break;
      }
    }

    UnMatched = PrevBanglaT.length() - Matched;

    // FIXME: Linux text editor (gedit) and OUKAR related quick fix
    if (gedit_compatible == true) {
      WideString OUKAR, OKAR;
      OUKAR = ucs4_t(0x9CC);
      OKAR = ucs4_t(0x9CB);

      WideString UnMatchedT = PrevBanglaT.substr(Matched);
      size_t found = WideString::npos;

      do {
        found = UnMatchedT.find(OKAR, found + 1);
        if (found != WideString::npos) {
          UnMatched++;
        }
      } while (found != WideString::npos);

      do {
        found = UnMatchedT.find(OUKAR, found + 1);
        if (found!=WideString::npos) {
          UnMatched++;
        }
      } while (found != WideString::npos);
    }
    // End of linux gedit fix

    if (UnMatched >= 1) {
      Backspace(UnMatched);
    }

    commit_string(NewBanglaText.substr(Matched));
    PrevBanglaT = NewBanglaText;
  }
}

void AvroInstance::MyProcessVKeyDown(const KeyEvent &mkey, bool &Block) {
  Block = false;

  switch (mkey.code) {
    case SCIM_KEY_KP_Decimal:
      AddStr(".`");
      Block = true;
      break;
    case SCIM_KEY_decimalpoint:
    case SCIM_KEY_period:
      AddStr(".");
      Block = true;
      break;
    case SCIM_KEY_KP_Divide:
    case SCIM_KEY_slash:
      AddStr("/");
      Block = true;
      break;
    case SCIM_KEY_KP_Multiply:
    case SCIM_KEY_asterisk:
      AddStr("*");
      Block = true;
      break;
    case SCIM_KEY_KP_Subtract:
    case SCIM_KEY_minus:
      AddStr("-");
      Block = true;
      break;
    case SCIM_KEY_KP_Add:
    case SCIM_KEY_plus:
      AddStr("+");
      Block = true;
      break;
    case SCIM_KEY_KP_0:
    case SCIM_KEY_0:
      AddStr("0");
      Block = true;
      break;
    case SCIM_KEY_KP_1:
    case SCIM_KEY_1:
      AddStr("1");
      Block = true;
      break;
    case SCIM_KEY_KP_2:
    case SCIM_KEY_2:
      AddStr("2");
      Block = true;
      break;
    case SCIM_KEY_KP_3:
    case SCIM_KEY_3:
      AddStr("3");
      Block = true;
      break;
    case SCIM_KEY_KP_4:
    case SCIM_KEY_4:
      AddStr("4");
      Block = true;
      break;
    case SCIM_KEY_KP_5:
    case SCIM_KEY_5:
      AddStr("5");
      Block = true;
      break;
    case SCIM_KEY_KP_6:
    case SCIM_KEY_6:
      AddStr("6");
      Block = true;
      break;
    case SCIM_KEY_KP_7:
    case SCIM_KEY_7:
      AddStr("7");
      Block = true;
      break;
    case SCIM_KEY_KP_8:
    case SCIM_KEY_8:
      AddStr("8");
      Block = true;
      break;
    case SCIM_KEY_KP_9:
    case SCIM_KEY_9:
      AddStr("9");
      Block = true;
      break;

    // Reset keys
    case SCIM_KEY_KP_Home:
    case SCIM_KEY_KP_Left:
    case SCIM_KEY_KP_Up:
    case SCIM_KEY_KP_Right:
    case SCIM_KEY_KP_Down:
    case SCIM_KEY_KP_Page_Up:
    case SCIM_KEY_KP_Page_Down:
    case SCIM_KEY_KP_End:
    case SCIM_KEY_Home:
    case SCIM_KEY_Left:
    case SCIM_KEY_Up:
    case SCIM_KEY_Right:
    case SCIM_KEY_Down:
    case SCIM_KEY_Page_Up:
    case SCIM_KEY_Page_Down:
    case SCIM_KEY_End:
    case SCIM_KEY_Delete:
      ResetDeadKey();
      Block = false;
      break;

  //Alphabets
    case SCIM_KEY_a:
      AddStr("a");
      Block = true;
      break;
    case SCIM_KEY_b:
      AddStr("b");
      Block = true;
      break;
    case SCIM_KEY_c:
      AddStr("c");
      Block = true;
      break;
    case SCIM_KEY_d:
      AddStr("d");
      Block = true;
      break;
    case SCIM_KEY_e:
      AddStr("e");
      Block = true;
      break;
    case SCIM_KEY_f:
      AddStr("f");
      Block = true;
      break;
    case SCIM_KEY_g:
      AddStr("g");
      Block = true;
      break;
    case SCIM_KEY_h:
      AddStr("h");
      Block = true;
      break;
    case SCIM_KEY_i:
      AddStr("i");
      Block = true;
      break;
    case SCIM_KEY_j:
      AddStr("j");
      Block = true;
      break;
    case SCIM_KEY_k:
      AddStr("k");
      Block = true;
      break;
    case SCIM_KEY_l:
      AddStr("l");
      Block = true;
      break;
    case SCIM_KEY_m:
      AddStr("m");
      Block = true;
      break;
    case SCIM_KEY_n:
      AddStr("n");
      Block = true;
      break;
    case SCIM_KEY_o:
      AddStr("o");
      Block = true;
      break;
    case SCIM_KEY_p:
      AddStr("p");
      Block = true;
      break;
    case SCIM_KEY_q:
      AddStr("q");
      Block = true;
      break;
    case SCIM_KEY_r:
      AddStr("r");
      Block = true;
      break;
    case SCIM_KEY_s:
      AddStr("s");
      Block = true;
      break;
    case SCIM_KEY_t:
      AddStr("t");
      Block = true;
      break;
    case SCIM_KEY_u:
      AddStr("u");
      Block = true;
      break;
    case SCIM_KEY_v:
      AddStr("v");
      Block = true;
      break;
    case SCIM_KEY_w:
      AddStr("w");
      Block = true;
      break;
    case SCIM_KEY_x:
      AddStr("x");
      Block = true;
      break;
    case SCIM_KEY_y:
      AddStr("y");
      Block = true;
      break;
    case SCIM_KEY_z:
      AddStr("z");
      Block = true;
      break;
    case SCIM_KEY_A:
      AddStr("A");
      Block = true;
      break;
    case SCIM_KEY_B:
      AddStr("B");
      Block = true;
      break;
    case SCIM_KEY_C:
      AddStr("C");
      Block = true;
      break;
    case SCIM_KEY_D:
      AddStr("D");
      Block = true;
      break;
    case SCIM_KEY_E:
      AddStr("E");
      Block = true;
      break;
    case SCIM_KEY_F:
      AddStr("F");
      Block = true;
      break;
    case SCIM_KEY_G:
      AddStr("G");
      Block = true;
      break;
    case SCIM_KEY_H:
      AddStr("H");
      Block = true;
      break;
    case SCIM_KEY_I:
      AddStr("I");
      Block = true;
      break;
    case SCIM_KEY_J:
      AddStr("J");
      Block = true;
      break;
    case SCIM_KEY_K:
      AddStr("K");
      Block = true;
      break;
    case SCIM_KEY_L:
      AddStr("L");
      Block = true;
      break;
    case SCIM_KEY_M:
      AddStr("M");
      Block = true;
      break;
    case SCIM_KEY_N:
      AddStr("N");
      Block = true;
      break;
    case SCIM_KEY_O:
      AddStr("O");
      Block = true;
      break;
    case SCIM_KEY_P:
      AddStr("P");
      Block = true;
      break;
    case SCIM_KEY_Q:
      AddStr("Q");
      Block = true;
      break;
    case SCIM_KEY_R:
      AddStr("R");
      Block = true;
      break;
    case SCIM_KEY_S:
      AddStr("S");
      Block = true;
      break;
    case SCIM_KEY_T:
      AddStr("T");
      Block = true;
      break;
    case SCIM_KEY_U:
      AddStr("U");
      Block = true;
      break;
    case SCIM_KEY_V:
      AddStr("V");
      Block = true;
      break;
    case SCIM_KEY_W:
      AddStr("W");
      Block = true;
      break;
    case SCIM_KEY_X:
      AddStr("X");
      Block = true;
      break;
    case SCIM_KEY_Y:
      AddStr("Y");
      Block = true;
      break;
    case SCIM_KEY_Z:
      AddStr("Z");
      Block = true;
      break;

    case SCIM_KEY_grave:
      AddStr("`");
      Block = true;
      break;
    case SCIM_KEY_asciitilde:
      AddStr("~");
      Block = true;
      break;
    case SCIM_KEY_exclam:
      AddStr("!");
      Block = true;
      break;
    case SCIM_KEY_at:
      AddStr("@");
      Block = true;
      break;
    case SCIM_KEY_numbersign:
      AddStr("#");
      Block = true;
      break;
    case SCIM_KEY_dollar:
      AddStr("$");
      Block = true;
      break;
    case SCIM_KEY_percent:
      AddStr("%");
      Block = true;
      break;
    case SCIM_KEY_asciicircum:
      AddStr("^");
      Block = true;
      break;
    case SCIM_KEY_ampersand:
      AddStr("&");
      Block = true;
      break;
    case SCIM_KEY_parenleft:
      AddStr("(");
      Block = true;
      break;
    case SCIM_KEY_parenright:
      AddStr(")");
      Block = true;
      break;
    case SCIM_KEY_underscore:
      AddStr("_");
      Block = true;
      break;
    case SCIM_KEY_equal:
      AddStr("=");
      Block = true;
      break;
    case SCIM_KEY_bar:
      AddStr("|");
      Block = true;
      break;
    case SCIM_KEY_backslash:
      AddStr("\\");
      Block = true;
      break;
    case SCIM_KEY_braceleft:
      AddStr("{");
      Block = true;
      break;
    case SCIM_KEY_braceright:
      AddStr("}");
      Block = true;
      break;
    case SCIM_KEY_bracketleft:
      AddStr("[");
      Block = true;
      break;
    case SCIM_KEY_bracketright:
      AddStr("]");
      Block = true;
      break;
    case SCIM_KEY_colon:
      AddStr(":");
      Block = true;
      break;
    case SCIM_KEY_semicolon:
      AddStr(";");
      Block = true;
      break;
    case SCIM_KEY_quotedbl:
      AddStr("\"");
      Block = true;
      break;
    case SCIM_KEY_apostrophe:
      AddStr("'");
      Block = true;
      break;
    case SCIM_KEY_question:
      AddStr("?");
      Block = true;
      break;
    case SCIM_KEY_greater:
      AddStr(">");
      Block = true;
      break;
    case SCIM_KEY_less:
      AddStr("<");
      Block = true;
      break;
    case SCIM_KEY_comma:
      AddStr(",");
      Block = true;
      break;

    case SCIM_KEY_Insert:
      Block = true;
      break;

    case SCIM_KEY_BackSpace:
      DoBackspace(Block);
      break;
  }
}
void AvroInstance::DoBackspace(bool &Block) {
  if ((EnglishT.length() == 1) || (EnglishT == ""))  {
    ResetDeadKey();
    Block = false;
    return;
  }

  if ((EnglishT.length() - 1) > 0) {
    Block = true;
    EnglishT = EnglishT.substr(0, (EnglishT.length() - 1));
    ParseAndSend();
  }

  if (EnglishT != "")  {
    UpdateWindow(EnglishT);
  } else {
    HideWindow();
  }
}

void AvroInstance::AddStr(std::string Str) {
  EnglishT = EnglishT + Str;

  ParseAndSend();

  UpdateWindow(EnglishT);
  //UpdateWindow_W(Parser.Convert(EnglishT));
}

void AvroInstance::ResetDeadKey() {
  PrevBanglaT = utf8_mbstowcs("");
  EnglishT = "";
  BlockLast = false;

  HideWindow();

  // reset();
}

void AvroInstance::Backspace(int Repeat) {
  if (Repeat < 1) {
    Repeat=1;
  }

  int iii = 1;
  KeyEvent kk(SCIM_KEY_BackSpace, 0, 0);

  for (iii = 1; iii <= Repeat; iii++) {
    forward_key_event(kk); //Send Backspace

    //Sleep for some microsecond, let the application process Backspace
    scim_usleep(20000); //20 milisecond pause for each backspace
  }
}

/* for candidates window */
inline static int int_to_ascii(int hex) {
  hex %= 10;
  return hex + '0';
}

void AvroInstance::UpdateWindow(String t) {
  if (show_preview_window == false) {
    HideWindow();
    return;
  }

  WideString trail;

  m_lookup_table.clear();
  m_lookup_table_labels.clear();

  //Show text in lookup window
  m_lookup_table.append_candidate(utf8_mbstowcs(t));

  //Hide "1" from candidate
  trail [0] = (ucs4_t) int_to_ascii(0);
  m_lookup_table_labels.push_back(trail);

  m_lookup_table.set_candidate_labels(m_lookup_table_labels);
  update_lookup_table(m_lookup_table);
  show_lookup_table();
}

void AvroInstance::HideWindow() {
  m_lookup_table.clear();
  update_preedit_caret(0);
  update_preedit_string(utf8_mbstowcs(""));
  hide_lookup_table();
  hide_preedit_string();
}
