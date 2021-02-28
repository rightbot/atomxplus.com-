// Copyright (c) 2019 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_BROWSER_API_ATOM_API_SERVICE_WORKER_CONTEXT_H_
#define SHELL_BROWSER_API_ATOM_API_SERVICE_WORKER_CONTEXT_H_

#include "content/public/browser/service_worker_context.h"
#include "content/public/browser/service_worker_context_observer.h"
#include "native_mate/handle.h"
#include "shell/browser/api/trackable_object.h"

namespace electron {

class AtomBrowserContext;

namespace api {

class ServiceWorkerContext : public mate::TrackableObject<ServiceWorkerContext>,
                             public content::ServiceWorkerContextObserver {
 public:
  static mate::Handle<ServiceWorkerContext> Create(
      v8::Isolate* isolate,
      AtomBrowserContext* browser_context);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  base::DictionaryValue GetWorkerInfoFromID(gin_helper::ErrorThrower thrower,
                                            int64_t version_id);

  // content::ServiceWorkerContextObserver
  void OnReportConsoleMessage(int64_t version_id,
                              const content::ConsoleMessage& message) override;

 protected:
  explicit ServiceWorkerContext(v8::Isolate* isolate,
                                AtomBrowserContext* browser_context);
  ~ServiceWorkerContext() override;

 private:
  AtomBrowserContext* browser_context_;

  content::ServiceWorkerContext* service_worker_context_;

  base::WeakPtrFactory<ServiceWorkerContext> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(ServiceWorkerContext);
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_BROWSER_API_ATOM_API_SERVICE_WORKER_CONTEXT_H_
