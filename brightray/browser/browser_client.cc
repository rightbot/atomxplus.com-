// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "brightray/browser/browser_client.h"

#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "brightray/browser/browser_context.h"
#include "brightray/browser/browser_main_parts.h"
#include "brightray/browser/devtools_manager_delegate.h"
#include "brightray/browser/media/media_capture_devices_dispatcher.h"
#include "brightray/browser/notification_presenter.h"
#include "brightray/browser/platform_notification_service.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/url_constants.h"

using content::BrowserThread;

namespace brightray {

namespace {

BrowserClient* g_browser_client;

base::LazyInstance<std::string>::DestructorAtExit
    g_io_thread_application_locale = LAZY_INSTANCE_INITIALIZER;

std::string g_application_locale;

void SetApplicationLocaleOnIOThread(const std::string& locale) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  g_io_thread_application_locale.Get() = locale;
}

}  // namespace

// static
void BrowserClient::SetApplicationLocale(const std::string& locale) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  if (!BrowserThread::PostTask(
          BrowserThread::IO, FROM_HERE,
          base::BindOnce(&SetApplicationLocaleOnIOThread, locale))) {
    g_io_thread_application_locale.Get() = locale;
  }
  g_application_locale = locale;
}

BrowserClient* BrowserClient::Get() {
  return g_browser_client;
}

BrowserClient::BrowserClient() : browser_main_parts_(nullptr) {
  DCHECK(!g_browser_client);
  g_browser_client = this;
}

BrowserClient::~BrowserClient() {}


void BrowserClient::WebNotificationAllowed(
    int render_process_id,
    const base::Callback<void(bool, bool)>& callback) {
  callback.Run(false, true);
}

NotificationPresenter* BrowserClient::GetNotificationPresenter() {
  if (!notification_presenter_) {
    // Create a new presenter if on OS X, Linux, or Windows 7+
    notification_presenter_.reset(NotificationPresenter::Create());
  }
  return notification_presenter_.get();
}

BrowserMainParts* BrowserClient::OverrideCreateBrowserMainParts(
    const content::MainFunctionParams&) {
  return new BrowserMainParts;
}

content::BrowserMainParts* BrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  DCHECK(!browser_main_parts_);
  browser_main_parts_ = OverrideCreateBrowserMainParts(parameters);
  return browser_main_parts_;
}

content::MediaObserver* BrowserClient::GetMediaObserver() {
  return MediaCaptureDevicesDispatcher::GetInstance();
}

content::PlatformNotificationService*
BrowserClient::GetPlatformNotificationService() {
  if (!notification_service_)
    notification_service_.reset(new PlatformNotificationService(this));
  return notification_service_.get();
}

void BrowserClient::GetAdditionalAllowedSchemesForFileSystem(
    std::vector<std::string>* additional_schemes) {
  additional_schemes->push_back(content::kChromeDevToolsScheme);
  additional_schemes->push_back(content::kChromeUIScheme);
}

void BrowserClient::GetAdditionalWebUISchemes(
    std::vector<std::string>* additional_schemes) {
  additional_schemes->push_back(content::kChromeDevToolsScheme);
}

net::NetLog* BrowserClient::GetNetLog() {
  return &net_log_;
}

base::FilePath BrowserClient::GetDefaultDownloadDirectory() {
  // ~/Downloads
  base::FilePath path;
  if (PathService::Get(base::DIR_HOME, &path))
    path = path.Append(FILE_PATH_LITERAL("Downloads"));

  return path;
}

content::DevToolsManagerDelegate* BrowserClient::GetDevToolsManagerDelegate() {
  return new DevToolsManagerDelegate;
}

std::string BrowserClient::GetApplicationLocale() {
  if (BrowserThread::CurrentlyOn(BrowserThread::IO))
    return g_io_thread_application_locale.Get();
  return g_application_locale;
}

}  // namespace brightray
