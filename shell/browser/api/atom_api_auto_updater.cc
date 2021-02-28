// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/atom_api_auto_updater.h"

#include "base/time/time.h"
#include "shell/browser/browser.h"
#include "shell/browser/native_window.h"
#include "shell/browser/window_list.h"
#include "shell/common/gin_converters/callback_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/event_emitter_caller.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"

// TODO(zcbenz): Remove this after removing mate::EventEmitter.
#include "shell/common/native_mate_converters/callback_converter_deprecated.h"

namespace mate {

template <>
struct Converter<base::Time> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   const base::Time& val) {
    v8::MaybeLocal<v8::Value> date =
        v8::Date::New(isolate->GetCurrentContext(), val.ToJsTime());
    if (date.IsEmpty())
      return v8::Null(isolate);
    else
      return date.ToLocalChecked();
  }
};

}  // namespace mate

namespace electron {

namespace api {

AutoUpdater::AutoUpdater(v8::Isolate* isolate) {
  auto_updater::AutoUpdater::SetDelegate(this);
  Init(isolate);
}

AutoUpdater::~AutoUpdater() {
  auto_updater::AutoUpdater::SetDelegate(nullptr);
}

void AutoUpdater::OnError(const std::string& message) {
  v8::Locker locker(isolate());
  v8::HandleScope handle_scope(isolate());
  auto error = v8::Exception::Error(gin::StringToV8(isolate(), message));
  gin_helper::EmitEvent(
      isolate(), GetWrapper(), "error",
      error->ToObject(isolate()->GetCurrentContext()).ToLocalChecked(),
      // Message is also emitted to keep compatibility with old code.
      message);
}

void AutoUpdater::OnError(const std::string& message,
                          const int code,
                          const std::string& domain) {
  v8::Locker locker(isolate());
  v8::HandleScope handle_scope(isolate());
  auto error = v8::Exception::Error(gin::StringToV8(isolate(), message));
  auto errorObject =
      error->ToObject(isolate()->GetCurrentContext()).ToLocalChecked();

  auto context = isolate()->GetCurrentContext();

  // add two new params for better error handling
  errorObject
      ->Set(context, gin::StringToV8(isolate(), "code"),
            v8::Integer::New(isolate(), code))
      .Check();
  errorObject
      ->Set(context, gin::StringToV8(isolate(), "domain"),
            gin::StringToV8(isolate(), domain))
      .Check();

  gin_helper::EmitEvent(isolate(), GetWrapper(), "error", errorObject, message);
}

void AutoUpdater::OnCheckingForUpdate() {
  Emit("checking-for-update");
}

void AutoUpdater::OnUpdateAvailable() {
  Emit("update-available");
}

void AutoUpdater::OnUpdateNotAvailable() {
  Emit("update-not-available");
}

void AutoUpdater::OnUpdateDownloaded(const std::string& release_notes,
                                     const std::string& release_name,
                                     const base::Time& release_date,
                                     const std::string& url) {
  Emit("update-downloaded", release_notes, release_name, release_date, url,
       // Keep compatibility with old APIs.
       base::BindRepeating(&AutoUpdater::QuitAndInstall,
                           base::Unretained(this)));
}

void AutoUpdater::OnWindowAllClosed() {
  QuitAndInstall();
}

void AutoUpdater::SetFeedURL(gin_helper::Arguments* args) {
  auto_updater::AutoUpdater::SetFeedURL(args);
}

void AutoUpdater::QuitAndInstall() {
  Emit("before-quit-for-update");

  // If we don't have any window then quitAndInstall immediately.
  if (WindowList::IsEmpty()) {
    auto_updater::AutoUpdater::QuitAndInstall();
    return;
  }

  // Otherwise do the restart after all windows have been closed.
  WindowList::AddObserver(this);
  WindowList::CloseAllWindows();
}

// static
gin::Handle<AutoUpdater> AutoUpdater::Create(v8::Isolate* isolate) {
  return gin::CreateHandle(isolate, new AutoUpdater(isolate));
}

// static
void AutoUpdater::BuildPrototype(v8::Isolate* isolate,
                                 v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "AutoUpdater"));
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("checkForUpdates", &auto_updater::AutoUpdater::CheckForUpdates)
      .SetMethod("getFeedURL", &auto_updater::AutoUpdater::GetFeedURL)
      .SetMethod("setFeedURL", &AutoUpdater::SetFeedURL)
      .SetMethod("quitAndInstall", &AutoUpdater::QuitAndInstall);
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::AutoUpdater;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("autoUpdater", AutoUpdater::Create(isolate));
  dict.Set("AutoUpdater", AutoUpdater::GetConstructor(isolate)
                              ->GetFunction(context)
                              .ToLocalChecked());
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(atom_browser_auto_updater, Initialize)
