
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


#ifndef __SCIM_AVRO_IMENGINE_H__
#define __SCIM_AVRO_IMENGINE_H__

#include <scim.h>
#include "scim_avro_englishtobangla.h"


using namespace scim;

class AvroInstance : public IMEngineInstanceBase {
  friend class AvroFactory;

private:
  //Conigure scim-avro

  // gedit specific problem fix. If you want to fix gedit OI-kar, OU-kar
  // problem, set gedit_compatible to true.
  const static bool gedit_compatible = false;

  // Preview window show/hide. if you want to hide preview window set
  // show_preview_window to false.
  const static bool show_preview_window=true;

  // Don't change anything below this line, Or I will kick your ass.

  AvroFactory *m_factory;

  // for toolbar
  PropertyList m_properties;

  void select_candidate_no_direct(unsigned int item);

public:
  // FIXME: avoid default values for variables.
  AvroInstance(AvroFactory *factory, const String &encoding, int id = -1);
  virtual ~AvroInstance();

  virtual bool process_key_event(const KeyEvent& key);
  virtual void move_preedit_caret(unsigned int pos);
  virtual void select_candidate(unsigned int item);
  virtual void update_lookup_table_page_size(unsigned int page_size);
  virtual void lookup_table_page_up(void);
  virtual void lookup_table_page_down(void);
  virtual void reset(void);
  virtual void focus_in(void);
  virtual void focus_out(void);
  virtual void trigger_property(const String &property);

private:
  // for candidates window
  CommonLookupTable m_lookup_table;
  std::vector<WideString> m_lookup_table_labels;

  KeyEvent m_prev_key;
  cEnglishToBangla Parser;

  WideString PrevBanglaT;
  String EnglishT;
  bool BlockLast;

  // Functions
  void ParseAndSend();
  void MyProcessVKeyDown(const KeyEvent &key, bool &Block);
  void DoBackspace(bool &Block);
  void AddStr(std::string Str);
  void ResetDeadKey();
  void Backspace(int Repeat);

  // for candidates window
  void UpdateWindow(String t);
  void HideWindow();
};

#endif /* __SCIM_AVRO_IMENGINE_H__ */

