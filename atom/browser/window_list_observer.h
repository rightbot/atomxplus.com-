// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_WINDOW_LIST_OBSERVER_H_
#define ATOM_BROWSER_WINDOW_LIST_OBSERVER_H_

#include "base/observer_list_types.h"

namespace atom {

class NativeWindow;

class WindowListObserver : public base::CheckedObserver {
 public:
  // Called immediately after a window is added to the list.
  virtual void OnWindowAdded(NativeWindow* window) {}

  // Called immediately after a window is removed from the list.
  virtual void OnWindowRemoved(NativeWindow* window) {}

  // Called when a window close is cancelled by beforeunload handler.
  virtual void OnWindowCloseCancelled(NativeWindow* window) {}

  // Called immediately after all windows are closed.
  virtual void OnWindowAllClosed() {}

 protected:
  ~WindowListObserver() override {}
};

}  // namespace atom

#endif  // ATOM_BROWSER_WINDOW_LIST_OBSERVER_H_
