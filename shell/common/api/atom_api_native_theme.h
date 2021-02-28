// Copyright (c) 2019 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_COMMON_API_ATOM_API_NATIVE_THEME_H_
#define SHELL_COMMON_API_ATOM_API_NATIVE_THEME_H_

#include "native_mate/handle.h"
#include "shell/browser/api/event_emitter.h"
#include "ui/native_theme/native_theme_observer.h"

namespace electron {

namespace api {

class NativeTheme : public mate::EventEmitter<NativeTheme>,
                    public ui::NativeThemeObserver {
 public:
  static v8::Local<v8::Value> Create(v8::Isolate* isolate);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

 protected:
  NativeTheme(v8::Isolate* isolate, ui::NativeTheme* theme);
  ~NativeTheme() override;

  bool ShouldUseDarkColors();
  bool ShouldUseHighContrastColors();
  bool ShouldUseInvertedColorScheme();

  // ui::NativeThemeObserver:
  void OnNativeThemeUpdated(ui::NativeTheme* theme) override;

 private:
  ui::NativeTheme* theme_;

  DISALLOW_COPY_AND_ASSIGN(NativeTheme);
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_COMMON_API_ATOM_API_NATIVE_THEME_H_
