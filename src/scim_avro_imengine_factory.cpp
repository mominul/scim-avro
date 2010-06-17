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
#define Uses_SCIM_CONFIG_BASE

#include <scim.h>
#include "scim_avro_imengine_factory.h"
#include "scim_avro_imengine.h"
#include "intl.h"

/* SCIM Related Definitions */
#define scim_module_init avro_LTX_scim_module_init
#define scim_module_exit avro_LTX_scim_module_exit
#define scim_imengine_module_init avro_LTX_scim_imengine_module_init
#define scim_imengine_module_create_factory avro_LTX_scim_imengine_module_create_factory
#define SCIM_CONFIG_IMENGINE_AVRO_UUID     "/IMEngine/Avro/UUID-"
#ifndef SCIM_AVRO_ICON_FILE
#define SCIM_AVRO_ICON_FILE           (SCIM_ICONDIR"/avro.png")
#endif

static ConfigPointer _scim_config (0);

extern "C" {
  void scim_module_init() {
    bindtextdomain (GETTEXT_PACKAGE, SCIM_AVRO_LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  }

  void scim_module_exit() {
    _scim_config.reset();
  }

  uint32 scim_imengine_module_init(const ConfigPointer &config) {
    SCIM_DEBUG_IMENGINE(1) << "Initialize AVRO Engine.\n";

    _scim_config = config;
    return 1;
  }

  IMEngineFactoryPointer scim_imengine_module_create_factory(uint32 engine) {
    AvroFactory *factory = 0;

    try {
      factory = new AvroFactory(
          String("bn_BD"),
          String("1eb0bd1f-af66-4feb-a8b2-1899b8064a00"),
          _scim_config);
    } catch(...) {
      delete factory;
      factory = 0;
    }

    return factory;
  }
}


AvroFactory::AvroFactory(
    const String &lang, const String &uuid, const ConfigPointer &config)
    : m_uuid(uuid), m_config(config) {
  SCIM_DEBUG_IMENGINE(1) << "Create Avro Factory :\n";
  SCIM_DEBUG_IMENGINE(1) << "  Lang : " << lang << "\n";
  SCIM_DEBUG_IMENGINE(1) << "  UUID : " << uuid << "\n";

  if (lang.length() >= 2) {
    set_languages(lang);
  }

  // config
  reload_config(m_config);
  m_reload_signal_connection =
      m_config->signal_connect_reload(slot(this, &AvroFactory::reload_config));
}

AvroFactory::~AvroFactory() {
  m_reload_signal_connection.disconnect();
}

WideString AvroFactory::get_name() const {
  return utf8_mbstowcs(String("Avro Phonetic"));
}

WideString AvroFactory::get_authors() const {
  return WideString();
}

WideString AvroFactory::get_credits() const {
  return WideString();
}

WideString AvroFactory::get_help() const {
  String msg;

  msg="\nAvro Phonetic (Ver. 0.0.2.1)\n";
  msg+="\n";
  msg+="Developer:\n";
  msg+="M M Rifat-Un-Nabi, Mehdi Hasan\n\n";
  msg+="Copyright (C) OmicronLab\n";
  msg+="http://www.omicronlab.com";

  return utf8_mbstowcs(msg);
}

String AvroFactory::get_uuid() const {
  return m_uuid;
}

String AvroFactory::get_icon_file() const {
  return String(SCIM_AVRO_ICON_FILE);
}

IMEngineInstancePointer AvroFactory::create_instance(const String &encoding,
                                                     int id) {
  return new AvroInstance(this, encoding, id);
}

void AvroFactory::reload_config(const ConfigPointer &config) {
  if (!config) {
    return;
  }
}

