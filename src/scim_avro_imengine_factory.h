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

#ifndef __SCIM_AVRO_IMENGINE_FACTORY_H__
#define __SCIM_AVRO_IMENGINE_FACTORY_H__

#define Uses_SCIM_ICONV
#include <scim.h>
//#include "scim_avro_action.h"

using namespace scim;

class AvroFactory : public IMEngineFactoryBase {
  String m_uuid;

  friend class AvroInstance;

  // config
  ConfigPointer m_config;
  Connection m_reload_signal_connection;

public:
  AvroFactory(
      const String &lang, const String &uuid, const ConfigPointer &config);
  virtual ~AvroFactory();

  virtual WideString get_name() const;
  virtual WideString get_authors() const;
  virtual WideString get_credits() const;
  virtual WideString get_help() const;
  virtual String get_uuid() const;
  virtual String get_icon_file() const;

  virtual IMEngineInstancePointer create_instance(
      const String& encoding, int id = -1);

private:
  void reload_config(const ConfigPointer &config);
};

#endif /* __SCIM_AVRO_IMENGINE_FACTORY_H__ */
