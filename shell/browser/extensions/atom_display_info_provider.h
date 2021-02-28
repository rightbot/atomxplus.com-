// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_BROWSER_EXTENSIONS_ATOM_DISPLAY_INFO_PROVIDER_H_
#define SHELL_BROWSER_EXTENSIONS_ATOM_DISPLAY_INFO_PROVIDER_H_

#include "base/macros.h"
#include "extensions/browser/api/system_display/display_info_provider.h"

namespace extensions {

class AtomDisplayInfoProvider : public DisplayInfoProvider {
 public:
  AtomDisplayInfoProvider();

 private:
  DISALLOW_COPY_AND_ASSIGN(AtomDisplayInfoProvider);
};

}  // namespace extensions

#endif  // SHELL_BROWSER_EXTENSIONS_ATOM_DISPLAY_INFO_PROVIDER_H_
