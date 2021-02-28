// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_COMMON_NATIVE_MATE_CONVERTERS_BLINK_CONVERTER_H_
#define SHELL_COMMON_NATIVE_MATE_CONVERTERS_BLINK_CONVERTER_H_

#include "native_mate/converter.h"
#include "third_party/blink/public/platform/web_cache.h"
#include "third_party/blink/public/platform/web_input_event.h"
#include "third_party/blink/public/web/web_context_menu_data.h"

namespace blink {
class WebMouseEvent;
class WebMouseWheelEvent;
class WebKeyboardEvent;
struct WebDeviceEmulationParams;
struct WebFloatPoint;
struct WebPoint;
struct WebSize;
}  // namespace blink

namespace content {
struct NativeWebKeyboardEvent;
}

namespace mate {

blink::WebInputEvent::Type GetWebInputEventType(v8::Isolate* isolate,
                                                v8::Local<v8::Value> val);

template <>
struct Converter<blink::WebInputEvent> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebInputEvent* out);
};

template <>
struct Converter<blink::WebKeyboardEvent> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebKeyboardEvent* out);
};

template <>
struct Converter<content::NativeWebKeyboardEvent> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     content::NativeWebKeyboardEvent* out);
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   const content::NativeWebKeyboardEvent& in);
};

template <>
struct Converter<blink::WebMouseEvent> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebMouseEvent* out);
};

template <>
struct Converter<blink::WebMouseWheelEvent> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebMouseWheelEvent* out);
};

template <>
struct Converter<blink::WebFloatPoint> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebFloatPoint* out);
};

template <>
struct Converter<blink::WebPoint> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebPoint* out);
};

template <>
struct Converter<blink::WebSize> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebSize* out);
};

template <>
struct Converter<blink::WebDeviceEmulationParams> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     blink::WebDeviceEmulationParams* out);
};

template <>
struct Converter<blink::WebContextMenuData::MediaType> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::WebContextMenuData::MediaType& in);
};

template <>
struct Converter<blink::WebContextMenuData::InputFieldType> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::WebContextMenuData::InputFieldType& in);
};

template <>
struct Converter<blink::WebCache::ResourceTypeStat> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::WebCache::ResourceTypeStat& stat);
};

template <>
struct Converter<blink::WebCache::ResourceTypeStats> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::WebCache::ResourceTypeStats& stats);
};

template <>
struct Converter<network::mojom::ReferrerPolicy> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   const network::mojom::ReferrerPolicy& in);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     network::mojom::ReferrerPolicy* out);
};

v8::Local<v8::Value> EditFlagsToV8(v8::Isolate* isolate, int editFlags);
v8::Local<v8::Value> MediaFlagsToV8(v8::Isolate* isolate, int mediaFlags);

}  // namespace mate

#endif  // SHELL_COMMON_NATIVE_MATE_CONVERTERS_BLINK_CONVERTER_H_
